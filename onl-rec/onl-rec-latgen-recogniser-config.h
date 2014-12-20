// -*- coding: utf-8 -*-
/* Copyright (c) 2013-14, Ondrej Platek, Ufal MFF UK <oplatek@ufal.mff.cuni.cz>
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
#ifndef ONL_REC_LATGEN_RECOGNISER_CONFIG_H_
#define ONL_REC_LATGEN_RECOGNISER_CONFIG_H_
#include "util/stl-utils.h"
#include "feat/feature-mfcc.h"

namespace kaldi {

struct OnlineLatgenRecogniserConfig {

  explicit OnlineLatgenRecogniserConfig():acoustic_scale(0.1), bits_per_sample(16) {}

  void Register(OptionsItf *po) {
    mfcc_opts.Register(po);
    splice_opts.Register(po);
    // delta_feat_opts.Register(po);
    faster_decoder_opts.Register(po);
    po->Register("bits-per-sample", &bits_per_sample,
                "Number of bits used for one sample in input waveform e.g. 8, 16.");
    po->Register("acoustic-scale", &acoustic_scale,
                "Scaling factor for acoustic likelihoods for forward decoding");
  }
  BaseFloat acoustic_scale;
  int32 bits_per_sample;

  std::string model_rxfilename;
  std::string fst_rxfilename;
  std::string silence_phones;
  std::string lda_mat_rspecifier;

  MfccOptions mfcc_opts;
  OnlineSpliceOptions splice_opts;
  // DeltaFeaturesOptions delta_feat_opts; TODO add support again
  LatticeFasterDecoderConfig faster_decoder_opts;
};

}
#endif // ONL_REC_LATGEN_RECOGNISER_CONFIG_H_ 
