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



#ifndef KALDI_PYKALDI_PYKALDI_AUDIO_SOURCE_H_
#define KALDI_PYKALDI_PYKALDI_AUDIO_SOURCE_H_

#include "online/online-audio-source.h"
#include "matrix/kaldi-vector.h"

namespace kaldi {

  // FIXME add options

/** @brief Proxy Audio Input. Acts like a buffer.
 *
 *  OnlineAudioSource implementation.
 *  It expects to be fed with the audio frame by frame.
 *  Supports only one channel. */
class PykaldiBlockSource: public OnlineAudioSourceItf {
 public:

  /// Creates the PykaldiBlockSource empty "buffer"
  PykaldiBlockSource(unsigned int sleep_time=10000, 
                    size_t bits_per_sample=16):
      bits_per_sample_(bits_per_sample), block_(false), 
      more_input_(true), sleep_time_(sleep_time) { }

  size_t BufferSize() { return src_.size(); }
  size_t frame_size;
  
  // Promise of new data -> Read() will return true
  void NewDataPromised() { more_input_ = true; }

  /// Call it, if no more data will be written 
  /// Read() will return false after returning all the buffered data
  void NoMoreInput(bool discard) { 
    more_input_ = false; 
    if (discard) src_.clear();
  }

  /// Implements OnlineAudioSource API
  /// Returns true if we have buffered data and we have promis for more. 
  /// It blocks if does not have enough data! 
  bool Read(Vector<BaseFloat> *data);

  /// Converts and buffers  the data 
  /// based on bits_per_sample specified in constructor
  /// @param data [in] the single channel pcm audio data
  /// @param num_samples [in] number of samples in data array
  void Write(unsigned char *data, size_t num_samples);

 private:
  size_t bits_per_sample_;
  bool block_;
  bool more_input_;
  unsigned int sleep_time_; // microseconds
  std::vector<BaseFloat> src_;

  KALDI_DISALLOW_COPY_AND_ASSIGN(PykaldiBlockSource);
};

} // namespace kaldi


#endif // KALDI_PYKALDI_PYKALDI_AUDIO_SOURCE_H_
