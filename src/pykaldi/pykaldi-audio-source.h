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

/** @brief Proxy Audio Input. Acts like a buffer.
 *
 *  OnlineAudioSource implementation.
 *  It expects to be fed with the audio frame by frame.
 *  Supports only one channel. */
class OnlineBlockSource: public OnlineAudioSourceItf{
 public:

  /// Creates the OnlineBlockSource empty "buffer"
  OnlineBlockSource():
      no_more_input_(false) { }

  size_t BufferSize() { return src_.size(); }
  
  /// Discards the buffer data. 
  /// It should be used with caution.
  /// Read() will return false immediately
  void DiscardAndFinish() { src_.clear(); no_more_input_ = true; }

  // Promise of new data -> Read() will return true
  void NewDataPromised() { no_more_input_ = false; }

  /// Call it, if no more data will be written 
  /// Read() will return false after returning all the buffered data
  void NoMoreInput() { no_more_input_ = true; }

  /// Implements OnlineAudioSource API
  /// Returns true if we have buffered data or 
  /// we will get them soon.
  bool Read(Vector<BaseFloat> *data);

  /// Converts and buffers  the data 
  /// based on bits_per_sample specified in constructor
  /// @param data [in] the single channel pcm audio data
  /// @param num_samples [in] number of samples in data array
  /// @param bits_per_sample [in] number of bits per sample 
  void Write(unsigned char *data, size_t num_samples, size_t bits_per_sample=16);

 private:
  bool no_more_input_;
  std::vector<BaseFloat> src_;

  KALDI_DISALLOW_COPY_AND_ASSIGN(OnlineBlockSource);
};

} // namespace kaldi


#endif // KALDI_PYKALDI_PYKALDI_AUDIO_SOURCE_H_
