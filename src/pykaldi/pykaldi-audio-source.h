// pykaldi/pykaldi-audio-source.cc

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



#ifndef KALDI_PYKALDI_PYKALDI_AUDIO_SOURCE_H_
#define KALDI_PYKALDI_PYKALDI_AUDIO_SOURCE_H_

#include "online/online-audio-source.h"
#include "matrix/kaldi-vector.h"

namespace kaldi {

/** @brief Proxy Audio Input. Acts like a buffer.
 *
 *  OnlineAudioSource implementation which blocks on Read.
 *  It expects to be fed with the audio frame by frame.
 *  Supports only one channel. */
class OnlineBlockSource: public OnlineAudioSourceItf{
 public:
  /// Creates the OnlineBlockSource empty "buffer"
  /// @param bits_per_sample [in]  By default we expect 16-bit audio
  OnlineBlockSource(uint32 bits_per_sample=16):
      bits_per_sample_(bits_per_sample),
      no_more_input_(false) { }

  /// Implements OnlineAudioSource API
  bool Read(Vector<BaseFloat> *data);

  /// Converts and buffers  the data 
  /// based on bits_per_sample specified in constructor
  /// @param data [in] the single channel pcm audio data
  /// @param num_samples [in] number of samples in data array
  void Write(unsigned char *data, size_t num_samples);

  /// Call it, if no more data will be written in. 
  void NoMoreInput() { this->no_more_input_ = true; }


 private:
  uint32 bits_per_sample_;
  bool no_more_input_;
  std::vector<BaseFloat> src_;
  KALDI_DISALLOW_COPY_AND_ASSIGN(OnlineBlockSource);
};

} // namespace kaldi


#endif // KALDI_PYKALDI_PYKALDI_AUDIO_SOURCE_H_
