// online/online-feat-input.h

// Copyright 2012 Cisco Systems (author: Matthias Paulik)
//           2012-2013  Vassil Panayotov
//           2013 Johns Hopkins University (author: Daniel Povey)
//           2013 Ondrej Platek, UFAL MFF UK

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

#ifndef KALDI_PYKALDI_PYKALDI_FEAT_INPUT_H_
#define KALDI_PYKALDI_PYKALDI_FEAT_INPUT_H_

#include "feat/feature-functions.h"
#include "online/online-feat-input.h"
#include "pykaldi-audio-source.h"

namespace kaldi {

struct PykaldiFeatureMatrixOptions {
  int32 batch_size; // number of frames to request each time.
  int32 num_tries; // number of tries of getting no output and timing out,
                   // before we give up.
  PykaldiFeatureMatrixOptions(): batch_size(27),
                                num_tries(5) { }
  void Register(OptionsItf *po) {
    po->Register("batch-size", &batch_size,
                 "Number of feature vectors processed w/o interruption");
    po->Register("num-tries", &num_tries,
                 "Number of successive repetitions of timeout before we "
                 "terminate stream");
  }
};

// The class PykaldiFeatureMatrix wraps something of type
// OnlineFeatInputItf in a manner that is convenient for
// a Decodable type to consume.
class PykaldiFeatureMatrix {
 public:
  PykaldiFeatureMatrix(const PykaldiFeatureMatrixOptions &opts,
                      OnlineFeatInputItf *input):
      opts_(opts), input_(input), feat_dim_(input->Dim()),
      feat_offset_(0), finished_(false) { }
  
  bool IsValidFrame (int32 frame); 

  int32 Dim() const { return feat_dim_; }

  // GetFrame() will die if it's not a valid frame; you have to
  // call IsValidFrame() for this frame, to see whether it
  // is valid.
  SubVector<BaseFloat> GetFrame(int32 frame);

  void NewStart();
  
  // Was not implemented in online version;-)
  // bool Good(); // returns true if we have at least one frame.
 private:
  void GetNextFeatures(); // called when we need more features.  Guarantees
  // to get at least one more frame, or set finished_ = true.
  
  const PykaldiFeatureMatrixOptions opts_;
  //FIXME  change it to PykaldiFeatInputItf
  // which will enable to pass message that we want to reset the pykaldi-audio-source
  OnlineFeatInputItf *input_; 
  int32 feat_dim_;
  Matrix<BaseFloat> feat_matrix_;
  int32 feat_offset_; // the offset of the first frame in the current batch
  bool finished_; // True if there are no more frames to be got from the input.
};

} // namespace kaldi

#endif // KALDI_PYKALDI_PYKALDI_FEAT_INPUT_H_
