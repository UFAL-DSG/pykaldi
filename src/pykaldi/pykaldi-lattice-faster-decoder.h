// pykaldi/pykaldi-lattice-faster-decoder.h
/* Copyright (c) 2013, Ondrej Platek, Ufal MFF UK <oplatek@ufal.mff.cuni.cz>
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

#ifndef KALDI_PYKALDI_PYKALDI_LATTICE_FASTER_DECODER_H_
#define KALDI_PYKALDI_PYKALDI_LATTICE_FASTER_DECODER_H_


#include "base/kaldi-common.h"
#include "util/common-utils.h"
#include "gmm/am-diag-gmm.h"
#include "tree/context-dep.h"
#include "hmm/transition-model.h"
#include "fstext/fstext-lib.h"
#include "decoder/lattice-faster-decoder.h"
#include "gmm/decodable-am-diag-gmm.h"
#include "util/timer.h"
#include "feat/feature-functions.h"  // feature reversal
#include "pykaldi-audio-source.h"

/*****************
 *  C interface  *
 *****************/

typedef void CWrapperLatFastDecoder;

#ifdef __cplusplus
extern "C" {
#endif

// explicit constructor and destructor
CWrapperLatFastDecoder* new_WrapperLatFastDecoder(void);
void del_WrapperLatFastDecoder(CWrapperLatFastDecoder *d);

// methods
// TODO full list
bool WLFD_Decode(CWrapperLatFastDecoder *d);
bool WLFD_Finished(CWrapperLatFastDecoder *d);
void WLFD_FrameIn(CWrapperLatFastDecoder *d, unsigned char *frame, size_t frame_len);
void WLFD_Reset(CWrapperLatFastDecoder *d);
int WLFD_Setup(CWrapperLatFastDecoder *d, int argc, char **argv);

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

/*******************
 *  C++ interface  *
 *******************/

namespace kaldi {

struct WrapperLatFastDecoderOptions {
  explicit WrapperLatFastDecoderOptions(): allow_partial(true),
    acoustic_scale(0.1) {} 
  bool allow_partial;
  BaseFloat acoustic_scale;
  std::string word_syms_filename;
  std::string model_in_filename;
  std::string fst_in_str;
  std::string feature_rspecifier;
  std::string lattice_wspecifier;
  std::string words_wspecifier;
  std::string alignment_wspecifier;

  void Register(OptionsItf *po) {
    po->Register("acoustic-scale", &acoustic_scale,
                "Scaling factor for acoustic likelihoods");
    po->Register("word-symbol-table", &word_syms_filename,
                "Symbol table for words [for debug output]");
    po->Register("allow-partial", &allow_partial,
                "If true, produce output even if end state was not reached.");
  }
};


/** A bit more optimized version of the lattice decoder.
   See \ref lattices_generation \ref decoders_faster and \ref decoders_simple
    for more information.
 */
class WrapperLatFastDecoder {
 public:
   WrapperLatFastDecoder():
     tot_like(0.0), frame_count(0), num_done(0), num_err(0), 
     decoder_(0), source_(0), decode_fst_(0), trans_model_(0), word_syms_(0)

  { Reset(); }

  // @brief Pass the 16 bit audio data
  /// @param data [in] the single channel pcm audio data
  /// @param num_samples [in] number of samples in data array
  void FrameIn(unsigned char *frame, size_t frame_len) {
    source_->Write(frame, frame_len);
  }

  bool Decode(void);

  bool Finished() { return feature_reader_.Done(); }

  void Reset(void);

  int Setup(int argc, char **argv);

  virtual ~WrapperLatFastDecoder(){ Reset(); }


 private:
  int ParseArgs(int argc, char **argv);

  double tot_like;
  kaldi::int64 frame_count;
  int num_done;
  int num_err;

  // FIXME remove
  CompactLatticeWriter compact_lattice_writer;
  Int32VectorWriter words_writer;
  Int32VectorWriter alignment_writer;
  SequentialBaseFloatMatrixReader feature_reader_; // REMOVE really just for first test


  LatticeFasterDecoder *decoder_;
  OnlineBlockSource *source_;
  fst::Fst<fst::StdArc> *decode_fst_;
  TransitionModel *trans_model_;
  fst::SymbolTable *word_syms_;
  AmDiagGmm am_gmm_;

  WrapperLatFastDecoderOptions opts_;
  LatticeFasterDecoderConfig config_;

  KALDI_DISALLOW_COPY_AND_ASSIGN(WrapperLatFastDecoder);
};  
  


} // namespace kaldi

#endif  // __cplusplus

#endif // if KALDI_PYKALDI_PYKALDI_LATTICE_FASTER_DECODER_H_
