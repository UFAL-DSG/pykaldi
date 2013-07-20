// pykaldi/pykaldi-feat-input.cc

// Copyright 2012 Cisco Systems (author: Matthias Paulik)

//   Modifications to the original contribution by Cisco Systems made by:
//   Vassil Panayotov
//   Johns Hopkins University (author: Daniel Povey)
//   Ondrej Platek, UFAL MFF UK 

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

#include "pykaldi-feat-input.h"

namespace kaldi {

void PykaldiFeatureMatrix::GetNextFeatures() {
  if (finished_) return; // Nothing to do.
  
  // We always keep the most recent frame of features, if present,
  // in case it is needed (this may happen when someone calls
  // IsLastFrame(), which requires us to get the next frame, while
  // they're stil processing this frame.
  bool have_last_frame = (feat_matrix_.NumRows() != 0);
  Vector<BaseFloat> last_frame;
  if (have_last_frame)
    last_frame = feat_matrix_.Row(feat_matrix_.NumRows() - 1);

  int32 iter;
  for (iter = 0; iter < opts_.num_tries; iter++) {
    Matrix<BaseFloat> next_features(opts_.batch_size, feat_dim_);
    finished_ = ! input_->Compute(&next_features);
    if (next_features.NumRows() == 0 && ! finished_) {
      // It timed out.  Try again.
      continue;
    }
    if (next_features.NumRows() > 0) {
      int32 new_size = (have_last_frame ? 1 : 0) +
          next_features.NumRows();
      feat_offset_ += feat_matrix_.NumRows() -
          (have_last_frame ? 1 : 0); // we're discarding this many
                                     // frames.
      feat_matrix_.Resize(new_size, feat_dim_, kUndefined);
      if (have_last_frame) {
        feat_matrix_.Row(0).CopyFromVec(last_frame);
        feat_matrix_.Range(1, next_features.NumRows(), 0, feat_dim_).
            CopyFromMat(next_features);
      } else {
        feat_matrix_.CopyFromMat(next_features);
      }
    }
    break;
  }
  if (iter == opts_.num_tries) { // we fell off the loop
    KALDI_WARN << "After " << opts_.num_tries << ", got no features, giving up.";
    finished_ = true; // We set finished_ to true even though the stream
    // doesn't say it's finished, because the delay is too much-- we gave up.
  }
}


bool PykaldiFeatureMatrix::IsValidFrame (int32 frame) {
  KALDI_ASSERT(frame >= feat_offset_ &&
               "You are attempting to get expired frames.");
  if (frame < feat_offset_ + feat_matrix_.NumRows())
    return true;
  else {
    GetNextFeatures();
    if (frame < feat_offset_ + feat_matrix_.NumRows())
      return true;
    else {
      if (finished_) return false;
      else {
        KALDI_WARN << "Unexpected point reached in code: "
                   << "possibly you are skipping frames?";
        return false;
      }
    }
  }
}

SubVector<BaseFloat> PykaldiFeatureMatrix::GetFrame(int32 frame) {
  if (frame < feat_offset_)
    KALDI_ERR << "Attempting to get a discarded frame.";
  if (frame >= feat_offset_ + feat_matrix_.NumRows())
    KALDI_ERR << "Attempt get frame without check its validity.";
  return feat_matrix_.Row(frame - feat_offset_);
}

} // namespace kaldi
