// pykaldi/pykaldi-audio-source.cc

/* Copyright (c) 2013, Ondrej Platek, Ufal MFF UK <oplatek@ufal.mff.cuni.cz>
 *                     2012-2013  Vassil Panayotov
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


#include <cmath>

#include "pykaldi-audio-source.h"

namespace kaldi {

bool OnlineBlockSource::Read(Vector<BaseFloat> *data) {
  KALDI_ASSERT(data->Dim() > 0);

  // TODO check: static_cast<size_t> from data->Dim() works on all architectures
  size_t n = std::min(src_.size(), static_cast<size_t>(data->Dim()));
  for (size_t i = 0; i < n ; ++i) {
    (*data)(i) = src_[i];
  }
  // remove the already read elements
  std::vector<BaseFloat>(src_.begin() + n, src_.end()).swap(src_);
  // KALDI_WARN << "src size: " << src_.size();
  // KALDI_WARN << "no_more_input_: " << no_more_input_;

  return ((!no_more_input_) || (src_.size() > 0));
}

void OnlineBlockSource::Write(unsigned char * data, size_t num_samples, size_t bits_per_sample) {
  // allocate the space at once -> should be faster
  src_.reserve(src_.size() + num_samples);
  // copy and convert the data to the buffer
  for (size_t i = 0; i < num_samples; ++i) {
      switch (bits_per_sample) {
        case 8:
          src_.push_back(*data);
          data++;
          break;
        case 16:
          {
            int16 k = *reinterpret_cast<uint16*>(data);
#ifdef __BIG_ENDIAN__
            KALDI_SWAP2(k);
#endif
            src_.push_back(k);
            data += 2;
            break;
          }
        case 32:
          {
            int32 k = *reinterpret_cast<uint32*>(data);
#ifdef __BIG_ENDIAN__
            KALDI_SWAP4(k);
#endif
            src_.push_back(k);
            data += 4;
            break;
          }
        default:
          KALDI_ERR << "bits per sample is " << bits_per_sample;
      }
  }
}

} // namespace kaldi
