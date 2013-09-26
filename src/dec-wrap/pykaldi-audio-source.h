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

#include "matrix/kaldi-vector.h"

namespace kaldi {


// Copied and renamed from online -> in order to get rid of portaudio dependency
class PykaldiAudioSourceItf {
 public:
  // Reads from the audio source, and writes the samples converted to BaseFloat
  // into the vector pointed by "data".
  // The user sets data->Dim() as a way of requesting that many samples.
  // The function returns number of actually read data.
  // Ideal scenerio the return value is equal to data->Dim()
  virtual MatrixIndexT Read(Vector<BaseFloat> *data) = 0;

  virtual ~PykaldiAudioSourceItf() { }
};

struct PykaldiBuffSourceOpts {
  // FIXME move the options here
};

/** @brief Proxy Audio Input. Acts like a buffer.
 *
 *  PykaldiAudioSource implementation.
 *  It expects to be fed with the audio frame by frame.
 *  Supports only one channel. */
class PykaldiBuffSource: public PykaldiAudioSourceItf {
 public:

  /// Creates the PykaldiBuffSource empty "buffer"
  PykaldiBuffSource(size_t bits_per_sample=16):
      bits_per_sample_(bits_per_sample) { }

  size_t BufferSize() { return src_.size(); }
  size_t frame_size;
  

 MatrixIndexT Read(Vector<BaseFloat> *data);

  /// Converts and buffers  the data 
  /// based on bits_per_sample specified in constructor
  /// @param data [in] the single channel pcm audio data
  /// @param num_samples [in] number of samples in data array
  void Write(unsigned char *data, size_t num_samples);

 private:
  size_t bits_per_sample_;
  std::vector<BaseFloat> src_;

  KALDI_DISALLOW_COPY_AND_ASSIGN(PykaldiBuffSource);
};

} // namespace kaldi


#endif // KALDI_PYKALDI_PYKALDI_AUDIO_SOURCE_H_
