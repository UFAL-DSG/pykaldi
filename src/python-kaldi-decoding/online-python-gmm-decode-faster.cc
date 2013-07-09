// onlinebin/online-gmm-decode-faster.cc

// Copyright 2012 Cisco Systems (author: Matthias Paulik)

//   Modifications to the original contribution by Cisco Systems made by:
//   Vassil Panayotov

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
// THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
// WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
// MERCHANTABLITY OR NON-INFRINGEMENT.
// See the Apache 2 License for the specific language governing permissions and
// limitations under the License.

#include "feat/feature-mfcc.h"
#include "online/online-audio-source.h"
#include "online/online-feat-input.h"
#include "online/online-decodable.h"
#include "online/online-faster-decoder.h"
#include "online/onlinebin-util.h"

#include "online-python-gmm-decode-faster.h"

using namespace kaldi;
using namespace fst;

/**************************************************************************
* TODO wrap the functions into decoder object                             *
* TODO do the parsing in the object and not from "command arguments"      *
* FIXME do not use the decoder via command line argumets                  *
*  Do not forget to deallocate the out* parameters. They are pointers!    *
***************************************************************************/
int get_online_python_gmm_decode_faster(int argc, char *argv[], 
      OnlineFasterDecoder * out_decoder, 
      OnlineDecodableDiagGmmScaled * out_decodable, 
      OnlineFeatInputItf *out_feat_transform, 
      fst::SymbolTable *out_word_syms,
      fst::Fst<fst::StdArc> *out_decode_fst) {
  try {
    typedef kaldi::int32 int32;
    typedef OnlineFeInput<OnlinePaSource, Mfcc> FeInput;

    // Up to delta-delta derivative features are calculated (unless LDA is used)
    const int32 kDeltaOrder = 2;
    // Input sampling frequency is fixed to 16KHz
    const int32 kSampleFreq = 16000;
    // PortAudio's internal ring buffer size in bytes
    const int32 kPaRingSize = 32768;
    // Report interval for PortAudio buffer overflows in number of feat. batches
    const int32 kPaReportInt = 4;

    const char *usage =
        "Decode speech, using microphone input(PortAudio)\n\n"
        "Utterance segmentation is done on-the-fly.\n"
        "Feature splicing/LDA transform is used, if the optional(last) argument "
        "is given.\n"
        "Otherwise delta/delta-delta(2-nd order) features are produced.\n\n"
        "Usage: online-gmm-decode-faster [options] <model-in>"
        "<fst-in> <word-symbol-table> <silence-phones> [<lda-matrix-in>]\n\n"
        "Example: online-gmm-decode-faster --rt-min=0.3 --rt-max=0.5 "
        "--max-active=4000 --beam=12.0 --acoustic-scale=0.0769 "
        "model HCLG.fst words.txt '1:2:3:4:5' lda-matrix";
    ParseOptions po(usage); 
    BaseFloat acoustic_scale = 0.1;
    int32 cmn_window = 600, min_cmn_window = 100;
    int32 right_context = 4, left_context = 4;

    kaldi::DeltaFeaturesOptions delta_opts;
    delta_opts.Register(&po);
    OnlineFasterDecoderOpts decoder_opts;
    decoder_opts.Register(&po, true);
    OnlineFeatureMatrixOptions feature_reading_opts;
    feature_reading_opts.Register(&po);
    
    po.Register("left-context", &left_context, "Number of frames of left context");
    po.Register("right-context", &right_context, "Number of frames of right context");
    po.Register("acoustic-scale", &acoustic_scale,
                "Scaling factor for acoustic likelihoods");
    po.Register("cmn-window", &cmn_window,
        "Number of feat. vectors used in the running average CMN calculation");
    po.Register("min-cmn-window", &min_cmn_window,
                "Minumum CMN window used at start of decoding (adds "
                "latency only at start)");

    // FIXME at some point get rid of ParseOptions
    po.Read(argc, argv);
    if (po.NumArgs() != 4 && po.NumArgs() != 5) {
      po.PrintUsage();
      return 1;
    }
    // FIXME check this in Python
    if (po.NumArgs() == 4)
      if (left_context % kDeltaOrder != 0 || left_context != right_context)
        KALDI_ERR << "Invalid left/right context parameters!";

    std::string model_rxfilename = po.GetArg(1),
        fst_rxfilename = po.GetArg(2),
        word_syms_filename = po.GetArg(3),
        silence_phones_str = po.GetArg(4),
        lda_mat_rspecifier = po.GetOptArg(5);

    Matrix<BaseFloat> lda_transform;
    if (lda_mat_rspecifier != "") {
      bool binary_in;
      Input ki(lda_mat_rspecifier, &binary_in);
      lda_transform.Read(ki.Stream(), binary_in);
    }

    std::vector<int32> silence_phones;
    if (!SplitStringToIntegers(silence_phones_str, ":", false, &silence_phones))
        KALDI_ERR << "Invalid silence-phones string " << silence_phones_str;
    if (silence_phones.empty())
        KALDI_ERR << "No silence phones given!";

    TransitionModel trans_model;
    AmDiagGmm am_gmm;
    {
        bool binary;
        Input ki(model_rxfilename, &binary);
        trans_model.Read(ki.Stream(), binary);
        am_gmm.Read(ki.Stream(), binary);
    }

    // fst::SymbolTable *word_syms = NULL;
    if (!(out_word_syms = fst::SymbolTable::ReadText(word_syms_filename)))
        KALDI_ERR << "Could not read symbol table from file "
                    << word_syms_filename;

    // fst::Fst<fst::StdArc> *decode_fst = ReadDecodeGraph(fst_rxfilename);
    out_decode_fst = ReadDecodeGraph(fst_rxfilename);

    // We are not properly registering/exposing MFCC and frame extraction options,
    // because there are parts of the online decoding code, where some of these
    // options are hardwired(ToDo: we should fix this at some point)
    MfccOptions mfcc_opts;
    mfcc_opts.use_energy = false;
    int32 frame_length = mfcc_opts.frame_opts.frame_length_ms = 25;
    int32 frame_shift = mfcc_opts.frame_opts.frame_shift_ms = 10;

    int32 window_size = right_context + left_context + 1;
    decoder_opts.batch_size = std::max(decoder_opts.batch_size, window_size);
    // OnlineFasterDecoder decoder(*decode_fst, decoder_opts,
    //                             silence_phones, trans_model);
    out_decoder = new OnlineFasterDecoder(*out_decode_fst, decoder_opts,
                                silence_phones, trans_model);
    OnlinePaSource au_src(kSampleFreq, kPaRingSize, kPaReportInt);
    Mfcc mfcc(mfcc_opts);
    FeInput fe_input(&au_src, &mfcc,
                     frame_length * (kSampleFreq / 1000),
                     frame_shift * (kSampleFreq / 1000));
    OnlineCmnInput cmn_input(&fe_input, cmn_window, min_cmn_window);
    // OnlineFeatInputItf *feat_transform = 0;
    if (lda_mat_rspecifier != "") {
      out_feat_transform = new OnlineLdaInput(
                               &cmn_input, lda_transform,
                               left_context, right_context);
    } else {
      DeltaFeaturesOptions opts;
      opts.order = kDeltaOrder;
      // Note from Dan: keeping the next statement for back-compatibility,
      // but I don't think this is really the right way to set the window-size
      // in the delta computation: it should be a separate config.
      opts.window = left_context / 2;
      out_feat_transform = new OnlineDeltaInput(opts, &cmn_input);
    }
    
    // feature_reading_opts contains timeout, batch size.
    OnlineFeatureMatrix feature_matrix(feature_reading_opts,
                                       out_feat_transform);

    // OnlineDecodableDiagGmmScaled decodable(am_gmm, trans_model, acoustic_scale,
    //                                        &feature_matrix);
    out_decodable = new OnlineDecodableDiagGmmScaled(am_gmm, trans_model, 
            acoustic_scale, &feature_matrix); 
  } catch(const std::exception& e) {
    if (out_decoder) delete out_decoder;
    if (out_decodable) delete out_decodable;
    if (out_feat_transform) delete out_feat_transform;
    if (out_word_syms) delete out_word_syms;
    if (out_decode_fst) delete out_decode_fst;
    std::cerr << e.what();
    return -1;
  }
  return 0;
} // get_online_python_gmm_decode_faster


/************************************************
 *  Decode suppose that decoder is initialized  *
 ************************************************/
int decode(OnlineFasterDecoder * decoder, 
      OnlineDecodableDiagGmmScaled * decodable,
      fst::SymbolTable *word_syms) {
    VectorFst<LatticeArc> out_fst;
    bool partial_res = false;
    while (1) {
      OnlineFasterDecoder::DecodeState dstate = decoder->Decode(decodable);
      std::vector<int32> word_ids;
      if (dstate & (decoder->kEndFeats | decoder->kEndUtt)) {
        decoder->FinishTraceBack(&out_fst);
        fst::GetLinearSymbolSequence(out_fst,
                                     static_cast<vector<int32> *>(0),
                                     &word_ids,
                                     static_cast<LatticeArc::Weight*>(0));
        PrintPartialResult(word_ids, word_syms, partial_res || word_ids.size());
        partial_res = false;
      } else {
        if (decoder->PartialTraceback(&out_fst)) {
          fst::GetLinearSymbolSequence(out_fst,
                                       static_cast<vector<int32> *>(0),
                                       &word_ids,
                                       static_cast<LatticeArc::Weight*>(0));
          PrintPartialResult(word_ids, word_syms, false);
          if (!partial_res)
            partial_res = (word_ids.size() > 0);
        }
      }
    }
    return 0;
} //decode()
