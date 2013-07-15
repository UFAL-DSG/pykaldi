// online/online-audio-source.cc

// Copyright 2012 Cisco Systems (author: Matthias Paulik)

//   Modifications to the original contribution by Cisco Systems made by:
//   Vassil Panayotov

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

#include <cmath>

#include "util/timer.h"
#include "online-audio-source.h"

namespace kaldi {

// The actual PortAudio callback - delegates to OnlinePaSource->PaCallback()
int PaCallback(const void *input, void *output,
               long unsigned frame_count,
               const PaStreamCallbackTimeInfo *time_info,
               PaStreamCallbackFlags status_flags,
               void *user_data) {
  OnlinePaSource *pa_src = reinterpret_cast<OnlinePaSource*>(user_data);
  return pa_src->Callback(input, output, frame_count, time_info, status_flags);
}


OnlinePaSource::OnlinePaSource(const uint32 timeout,
                               const uint32 sample_rate,
                               const uint32 rb_size,
                               const uint32 report_interval)
    : timeout_(timeout), timed_out_(false),
      sample_rate_(sample_rate), pa_started_(false),
      report_interval_(report_interval), nread_calls_(0),
      noverflows_(0), samples_lost_(0) {
  using namespace std;
  int32 rb_bits = static_cast<int32>(ceil(log(rb_size) / log(2)));
  if (rb_bits > 30) // ok, this limit is somewhat arbitrary
    throw invalid_argument("PortAudio ring buffer too large!");
  rb_size_ = 1 << rb_bits;
  ring_buffer_ = new char[rb_size_];
  ring_buffer_size_t rbs = PaUtil_InitializeRingBuffer(
                               &pa_ringbuf_, sizeof(SampleType),
                               rb_size_ / sizeof(SampleType), ring_buffer_);
  if (rbs != 0)
    throw runtime_error("Unexpected PortAudio ring buffer init error");

  PaError paerr = Pa_Initialize();
  if (paerr != paNoError)
    throw runtime_error("PortAudio initialization error");
  // Monophone, 16-bit input hardcoded
  KALDI_ASSERT(sizeof(SampleType) == 2 &&
               "The current OnlinePaSource code assumes 16-bit input");
  paerr = Pa_OpenDefaultStream(&pa_stream_, 1, 0, paInt16, sample_rate_, 0,
                               PaCallback, this);
  if (paerr != paNoError)
    throw runtime_error("PortAudio failed to open the default stream");
}


OnlinePaSource::~OnlinePaSource() {
  if (pa_started_)
    Pa_StopStream(pa_stream_);
  if (pa_stream_ != 0) {
    Pa_CloseStream(pa_stream_);
    Pa_Terminate();
  }
  if (ring_buffer_ != 0)
    delete [] ring_buffer_;
}


bool OnlinePaSource::Read(Vector<BaseFloat> *data) {
  if (!pa_started_) { // start stream the first time Read() is called
    PaError paerr = Pa_StartStream(pa_stream_);
    if (paerr != paNoError)
      throw std::runtime_error("Error while trying to open PortAudio stream");
    pa_started_ = true;
  }
  Timer timer;
  if (report_interval_ != 0
      && (++nread_calls_ % report_interval_) == 0
      && noverflows_ > 0) {
      KALDI_VLOG(1) << noverflows_ << " PortAudio ring buffer overflows detected "
                    << "and " << samples_lost_ << " sample(s) were lost";
      samples_lost_ = noverflows_ = 0;
  }
  uint32 nsamples_req = data->Dim(); // samples to request
  timed_out_ = false;
  while (true) {
    ring_buffer_size_t nsamples = PaUtil_GetRingBufferReadAvailable(&pa_ringbuf_);
    if (nsamples >= nsamples_req)
      break;
    if (timeout_ > 0) {
      int32 elapsed = static_cast<int32>(timer.Elapsed() * 1000);
      if (elapsed > timeout_) {
        nsamples_req = nsamples;
        timed_out_ = true;
        KALDI_VLOG(2) << "OnlinePaSource::Read() timeout";
        break;
      }
    }
    Pa_Sleep(2);
  }
  int16 buf[nsamples_req];
  rbs_t nsamples_rcv = PaUtil_ReadRingBuffer(&pa_ringbuf_, buf, nsamples_req);
  if (nsamples_rcv != nsamples_req) {
    KALDI_WARN << "Requested: " << nsamples_req
               << "; Received: " << nsamples_rcv << " samples";
    // This would be a PortAudio error.
  }
  KALDI_WARN << "Requested: " << nsamples_req << "; Received: " << nsamples_rcv << " samples"; // DEBUG
  data->Resize(nsamples_rcv);
  for (int i = 0; i < nsamples_rcv; ++i)
    (*data)(i) = static_cast<BaseFloat>(buf[i]);

  return (nsamples_rcv != 0);
  // NOTE (Dan): I'm pretty sure this return value is not right, it could be
  // this way because we're waiting.  Vassil or someone will have to figure this
  // out.
}


// Accepts the data and writes it to the ring buffer
int OnlinePaSource::Callback(const void *input, void *output,
                             ring_buffer_size_t frame_count,
                             const PaStreamCallbackTimeInfo *time_info,
                             PaStreamCallbackFlags status_flags) {
  if (report_interval_ != 0) {
    if (frame_count > PaUtil_GetRingBufferWriteAvailable(&pa_ringbuf_))
      ++noverflows_;
  }
  rbs_t written = PaUtil_WriteRingBuffer(&pa_ringbuf_, input, frame_count);
  samples_lost_ += frame_count - written;
  return paContinue;
}


bool OnlineVectorSource::Read(Vector<BaseFloat> *data) {
  KALDI_ASSERT(data->Dim() > 0);
  int32 n_elem = std::min(src_.Dim() - pos_,
                          static_cast<uint32>(data->Dim()));
  if (n_elem > 0) {
    SubVector<BaseFloat> subsrc(src_, pos_, n_elem);
    if (data->Dim() == subsrc.Dim())
      data->CopyFromVec(subsrc);
    else
      for (int32 i = 0; i < subsrc.Dim(); ++i)
        (*data)(i) = subsrc(i);
    pos_ += n_elem;
  }
  return (pos_ < src_.Dim());
}


/// Buffers internally the given data
void OnlineBlockSource::Write(unsigned char * data, size_t num_samples) {
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
          KALDI_ERR << "bits per sample is " << bits_per_sample_;  // already checked this.
      }
  }
}



/// Return true if some still data available after Reading
bool OnlineBlockSource::Read(Vector<BaseFloat> *data) {
  // FIXME implement timeout!
  KALDI_ASSERT(data->Dim() > 0);
  // KALDI_WARN << "Requested: " << data->Dim(); // DEBUG

  // TODO check: static_cast<size_t> from data->Dim() works on all architectures
  size_t n = std::min(src_.size(), static_cast<size_t>(data->Dim()));
  // KALDI_WARN << "DEBUG size before remove: "  << n;
  // KALDI_WARN << "DEBUG outgoing data Dim(): "  << data->Dim();
  for (size_t i = 0; i < n ; ++i) {
    (*data)(i) = src_[i];
  }
  // remove the already read elements
  std::vector<BaseFloat>(src_.begin() + n, src_.end()).swap(src_);
  // KALDI_WARN << "DEBUG size after remove: "  << src_.size();
  return ((!no_more_input_) || (src_.size() > 0));
}

void OnlineBlockSource::NoMoreInput() {
  this->no_more_input_ = true;
}

} // namespace kaldi
