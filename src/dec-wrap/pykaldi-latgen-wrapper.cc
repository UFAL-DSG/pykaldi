// -*- coding: utf-8 -*-
/* Copyright (c) 2013, Ondrej Platek, Ufal MFF UK <oplatek@ufal.mff.cuni.cz>
 *               2012-2013  Vassil Panayotov
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * THIS CODE IS PROVIDED *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED
 * WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE,
 * MERCHANTABLITY OR NON-INFRINGEMENT.
 * See the Apache 2 License for the specific language governing permissions and
 * limitations under the License. */

#include "feat/feature-mfcc.h"
#include "pykaldibin-util.h"
#include "pykaldi-audio-source.h"
#include "pykaldi-latgen-wrapper.h"
#include "pykaldi-feat-input.h"
#include "pykaldi-decodable.h"
#include "pykaldi-latgen-wrapper.h"
#include "pykaldi-latgen-decoder.h"

using namespace kaldi;

void unpack_GmmLatgenWrapper(GmmLatgenWrapper w, Mfcc **mfcc, PykaldiBuffSource **audio, 
    PykaldiFeInput<Mfcc> **feat_input, TransitionModel **trans_model, 
    fst::Fst<fst::StdArc> **decode_fst, PykaldiLatticeFasterDecoder **decoder, 
    PykaldiFeatInputItf **feat_transform, PykaldiFeatureMatrix **feat_matrix, 
    PykaldiDecodableDiagGmmScaled **decodable, AmDiagGmm **amm) {
  *mfcc = reinterpret_cast<Mfcc*>(w.mfcc);
  *audio = reinterpret_cast<PykaldiBuffSource*>(w.audio);
  *feat_input = reinterpret_cast<PykaldiFeInput<Mfcc>*>(w.feat_input);
  *trans_model = reinterpret_cast<TransitionModel*>(w.trans_model);
  *decode_fst = reinterpret_cast<fst::Fst<fst::StdArc>*>(w.decode_fst);
  *decoder = reinterpret_cast<PykaldiLatticeFasterDecoder*>(w.decoder);
  *feat_transform = reinterpret_cast<PykaldiFeatInputItf*>(w.feat_transform);
  *feat_matrix = reinterpret_cast<PykaldiFeatureMatrix*>(w.feat_matrix);
  *decodable = reinterpret_cast<PykaldiDecodableDiagGmmScaled*>(w.decodable);
  *amm = reinterpret_cast<AmDiagGmm*>(w.amm);
}
void del_GmmLatgenWrapper(GmmLatgenWrapper w) {
  Mfcc *mfcc; PykaldiBuffSource *audio; PykaldiFeInput<Mfcc> *feat_input; 
  TransitionModel *trans_model; fst::Fst<fst::StdArc> *decode_fst; 
  PykaldiLatticeFasterDecoder *decoder; PykaldiFeatInputItf *feat_transform; 
  PykaldiFeatureMatrix *feat_matrix; PykaldiDecodableDiagGmmScaled *decodable; 
  AmDiagGmm *amm;

  unpack_GmmLatgenWrapper(w, &mfcc, &audio, &feat_input, &trans_model, &decode_fst, 
    &decoder, &feat_transform, &feat_matrix, &decodable, &amm);

 delete audio; w.audio = NULL;
 delete mfcc; w.mfcc = NULL;
 delete feat_input; w.feat_input = NULL;
 delete feat_transform; w.feat_transform = NULL;
 delete feat_matrix; w.feat_matrix = NULL;
 delete decodable; w.decodable = NULL;
 delete trans_model; w.trans_model = NULL;
 delete amm; w.amm = NULL;
 delete decoder; w.decoder = NULL;
 delete decode_fst; w.decode_fst = NULL;

}

size_t Decode(void *decoder, void *decodableItf) {
  PykaldiLatticeFasterDecoder *d = reinterpret_cast<PykaldiLatticeFasterDecoder*>(decoder);
  PykaldiDecodableDiagGmmScaled *decodable = reinterpret_cast<PykaldiDecodableDiagGmmScaled*>(decodableItf);
  return d->Decode(decodable);
}


void FrameIn(void *audio_source, unsigned char *frame, size_t frame_len) {
  reinterpret_cast<kaldi::PykaldiBuffSource*>(audio_source)->Write(frame, frame_len);
}


// void PopHyp(void *decoder, void *decoded_hypothesis) {
//   std::vector<int32> tmp = dp->PopHyp();
//   KALDI_ASSERT(size <= tmp.size());
//   std::copy(tmp.begin(), tmp.begin()+size, word_ids);
// 
// }

GmmLatgenWrapper Setup(int argc, char **argv) {
  struct GmmLatgenWrapper w;
  try {
    KaldiDecoderGmmLatgenWrapperOptions wrapper_opts;
    PykaldiFeatureMatrixOptions feature_reading_opts;
    MfccOptions mfcc_opts;
    LatticeFasterDecoderConfig decoder_opts;
    DeltaFeaturesOptions delta_feat_opts;
    PykaldiBuffSourceOptions au_opts;  // Fixed 16 bit audio


    // Parsing options
    ParseOptions po("Utterance segmentation is done on-the-fly.\n"
      "The delta/delta-delta(2-nd order) features are produced.\n\n"
      "Usage: decoder-binary-name [options] <model-in>"
      "<fst-in> <word-symbol-table> <silence-phones> \n\n"
      "Example: decoder-binary-name --max-active=4000 --beam=12.0 "
      "--acoustic-scale=0.0769 model HCLG.fst words.txt '1:2:3:4:5'");

    wrapper_opts.Register(&po);
    mfcc_opts.Register(&po);
    decoder_opts.Register(&po);
    feature_reading_opts.Register(&po);
    delta_feat_opts.Register(&po);
    au_opts.Register(&po);

    po.Read(argc, argv);
    if (po.NumArgs() != 4 && po.NumArgs() != 5) {
      po.PrintUsage();
      return w;
    }
    if (po.NumArgs() == 4)
      if (wrapper_opts.left_context % delta_feat_opts.order != 0 ||
          wrapper_opts.left_context != wrapper_opts.right_context)
        KALDI_ERR << "Invalid left/right context parameters!";

    wrapper_opts.model_rxfilename = po.GetArg(1);
    wrapper_opts.fst_rxfilename = po.GetArg(2);
    wrapper_opts.word_syms_filename = po.GetArg(3);

    wrapper_opts.silence_phones = phones_to_vector(po.GetArg(4));
    wrapper_opts.lda_mat_rspecifier = po.GetOptArg(5);

    // Setting up components
    TransitionModel *trans_model = new TransitionModel();
    AmDiagGmm *amm = new AmDiagGmm();
    {
      bool binary;
      Input ki(wrapper_opts.model_rxfilename, &binary);
      trans_model->Read(ki.Stream(), binary);
      amm->Read(ki.Stream(), binary);
    }

    fst::Fst<fst::StdArc> *decode_fst = ReadDecodeGraph(wrapper_opts.fst_rxfilename);
    // TODO
    PykaldiLatticeFasterDecoder *decoder = new PykaldiLatticeFasterDecoder(
                                    *decode_fst, decoder_opts);

    PykaldiBuffSource *audio = new PykaldiBuffSource(au_opts);

    Mfcc *mfcc = new Mfcc(mfcc_opts);
    int32 frame_length = mfcc_opts.frame_opts.frame_length_ms;
    int32 frame_shift = mfcc_opts.frame_opts.frame_shift_ms;
    PykaldiFeInput<Mfcc> *feat_input = new PykaldiFeInput<Mfcc>(audio, mfcc,
                               frame_length * (wrapper_opts.kSampleFreq / 1000),
                               frame_shift * (wrapper_opts.kSampleFreq / 1000));

    PykaldiFeatInputItf *feat_transform;
    if (wrapper_opts.lda_mat_rspecifier != "") {
      bool binary_in;
      Matrix<BaseFloat> lda_transform;
      Input ki(wrapper_opts.lda_mat_rspecifier, &binary_in);
      lda_transform.Read(ki.Stream(), binary_in);
      // lda_transform is copied to PykaldiLdaInput
      feat_transform = new PykaldiLdaInput(feat_input,
                                lda_transform,
                                wrapper_opts.left_context, wrapper_opts.right_context);
    } else {
      feat_transform = new PykaldiDeltaInput(delta_feat_opts, feat_input);
    }

    PykaldiFeatureMatrix *feat_matrix = new PykaldiFeatureMatrix(feature_reading_opts,
                                       feat_transform);
    PykaldiDecodableDiagGmmScaled *decodable = new PykaldiDecodableDiagGmmScaled(*amm,
                                            *trans_model,
                                            wrapper_opts.acoustic_scale, feat_matrix);

    w.audio = audio; w.mfcc = mfcc; w.feat_input = feat_input;
    w.feat_transform = feat_transform; w.feat_matrix = feat_matrix;
    w.decodable = decodable; w.trans_model = trans_model;
    w.amm = amm; w.decoder = decoder; w.decode_fst = decode_fst;

  } catch(const std::exception& e) {
    std::cerr << e.what();
    del_GmmLatgenWrapper(w);
  }
  return w;
}

/*******************
 *  C++ interface  *
 *******************/

namespace kaldi {



} // namespace kaldi
