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
#include <unistd.h>

#include "pykaldi-audio-source.h"

namespace kaldi {

bool PykaldiBlockSource::Read(Vector<BaseFloat> *output) {
  size_t d =  static_cast<size_t>(output->Dim());
  KALDI_ASSERT(d > 0 && "Request at least something");

  while(block_ && (src_.size() < d) && more_input_) { 
    usleep(sleep_time_);
    KALDI_WARN << "Waiting for input! (Posibbly never ending loop!)\n" 
               << "Have data " << src_.size()
               << " required data " << output->Dim();
  }

  if (src_.size() >= d) {
    // copy the buffer to output
    for (size_t i = 0; i < d ; ++i) {
      (*output)(i) = src_[i];
    }
    // remove the already read elements
    std::vector<BaseFloat>(src_.begin() + d, src_.end()).swap(src_);
    return true;
  } else {
    // block_ is false otherwise we would be waiting for input 
    KALDI_ASSERT(!block_);
    KALDI_VLOG(1) << "End of stream! more_input_: " << more_input_
                  << " src_.size(): " << src_.size()
                  << " requested: " << d;
    return false;
  }
}

void PykaldiBlockSource::Write(unsigned char * data, size_t num_samples) {
  // allocate the space at once -> should be faster
  src_.reserve(src_.size() + num_samples);
  // copy and convert the data to the buffer
  for (size_t i = 0; i < num_samples; ++i) {
      switch (bits_per_sample_) {
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
          KALDI_ERR << "unsupported bits per sample: " << bits_per_sample_;
      }
  }
}

} // namespace kaldi
