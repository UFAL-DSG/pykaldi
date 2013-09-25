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

  // We always keep the most recent frame of features, if present,
  // in case it is needed (this may happen when someone calls
  // IsLastFrame(), which requires us to get the next frame, while
  // they're stil processing this frame.
  bool have_last_frame = (feat_matrix_.NumRows() != 0);
  Vector<BaseFloat> last_frame;
  if (have_last_frame)
    last_frame = feat_matrix_.Row(feat_matrix_.NumRows() - 1);

  Matrix<BaseFloat> next_features(opts_.batch_size, feat_dim_);
  input_->Compute(&next_features);

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
}


bool PykaldiFeatureMatrix::IsValidFrame (int32 frame) {
  if (frame < feat_offset_ + feat_matrix_.NumRows())
    return true;
  else {
    GetNextFeatures();
    if (frame < feat_offset_ + feat_matrix_.NumRows())
      return true;
    else
       return false;
  }
}

SubVector<BaseFloat> PykaldiFeatureMatrix::GetFrame(int32 frame) {
  if (frame < feat_offset_)
    KALDI_ERR << "Attempting to get a discarded frame.";
  if (frame >= feat_offset_ + feat_matrix_.NumRows())
    KALDI_ERR << "Attempt get frame without check its validity.";
  return feat_matrix_.Row(frame - feat_offset_);
}



/**********************************************************
LDA INPUT
**********************************************************/

PykaldiLdaInput::PykaldiLdaInput(PykaldiFeatInputItf *input,
                               const Matrix<BaseFloat> &transform,
                               int32 left_context,
                               int32 right_context):
    input_(input), input_dim_(input->Dim()),
    left_context_(left_context), right_context_(right_context) {

  int32 tot_context = left_context + 1 + right_context;
  if (transform.NumCols() == input_dim_ * tot_context) {
    linear_transform_ = transform;
    // and offset_ stays empty.
  } else if (transform.NumCols() == input_dim_ * tot_context + 1) {
    linear_transform_.Resize(transform.NumRows(), transform.NumCols() - 1);
    linear_transform_.CopyFromMat(transform.Range(0, transform.NumRows(),
                                           0, transform.NumCols() - 1));
    offset_.Resize(transform.NumRows());
    offset_.CopyColFromMat(transform, transform.NumCols() - 1);
  } else {
    KALDI_ERR << "Invalid parameters supplied to PykaldiLdaInput";
  }
}

// static
void PykaldiLdaInput::SpliceFrames(const MatrixBase<BaseFloat> &input1,
                                  const MatrixBase<BaseFloat> &input2,
                                  const MatrixBase<BaseFloat> &input3,
                                  int32 context_window,
                                  Matrix<BaseFloat> *output) {
  KALDI_ASSERT(context_window > 0);
  const int32 size1 = input1.NumRows(), size2 = input2.NumRows(),
      size3 = input3.NumRows();
  int32 num_frames_in = size1 + size2 + size3,
      num_frames_out = num_frames_in - (context_window - 1),
      dim = std::max(input1.NumCols(), std::max(input2.NumCols(), input3.NumCols()));
  // do std::max in case one or more of the input matrices is empty.

  if (num_frames_out <= 0) {
    output->Resize(0, 0);
    return;
  }
  output->Resize(num_frames_out, dim * context_window);
  for (int32 t_out = 0; t_out < num_frames_out; t_out++) {
    for (int32 pos = 0; pos < context_window; pos++) {
      int32 t_in = t_out + pos;
      SubVector<BaseFloat> vec_out(output->Row(t_out), pos * dim, dim);
      if (t_in < size1)
        vec_out.CopyFromVec(input1.Row(t_in));
      else if (t_in < size1 + size2)
        vec_out.CopyFromVec(input2.Row(t_in - size1));
      else
        vec_out.CopyFromVec(input3.Row(t_in - size1 - size2));
    }
  }
}

void PykaldiLdaInput::TransformToOutput(const MatrixBase<BaseFloat> &spliced_feats,
                                       Matrix<BaseFloat> *output) {
  if (spliced_feats.NumRows() == 0) {
    output->Resize(0, 0);
  } else {
    output->Resize(spliced_feats.NumRows(), linear_transform_.NumRows());
    output->AddMatMat(1.0, spliced_feats, kNoTrans,
                      linear_transform_, kTrans, 0.0);
    if (offset_.Dim() != 0)
      output->AddVecToRows(1.0, offset_);
  }
}

void PykaldiLdaInput::Compute(Matrix<BaseFloat> *output) {
  KALDI_ASSERT(output->NumRows() > 0 &&
               output->NumCols() == linear_transform_.NumRows());
  // If output->NumRows() == 0, it corresponds to a request for zero frames,
  // which makes no sense.

  // We request the same number of frames of data that we were requested.
  Matrix<BaseFloat> input(output->NumRows(), input_dim_);
  input_->Compute(&input);

  if (input.NumRows() == 0) {
    output->Resize(0, 0);
    return;
  }

  // If this is the first segment of the utterance, we put in the
  // initial duplicates of the first frame, numbered "left_context".
  if (remainder_.NumRows() == 0 && input.NumRows() != 0 && left_context_ > 0) {
    remainder_.Resize(left_context_, input_dim_);
    for (int32 i = 0; i < left_context_; i++)
      remainder_.Row(i).CopyFromVec(input.Row(0));
  }

  // If this is the last segment, we put in the final duplicates of the
  // last frame, numbered "right_context".
  Matrix<BaseFloat> tail;
  // FIXME we do not detect last frame
  // if (right_context_ > 0) {
  //   tail.Resize(right_context_, input_dim_);
  //   for (int32 i = 0; i < right_context_; i++) {
  //     if (input.NumRows() > 0)
  //       tail.Row(i).CopyFromVec(input.Row(input.NumRows() - 1));
  //     else
  //       tail.Row(i).CopyFromVec(remainder_.Row(remainder_.NumRows() - 1));
  //   }
  // }

  Matrix<BaseFloat> spliced_feats;
  int32 context_window = left_context_ + 1 + right_context_;
  this->SpliceFrames(remainder_, input, tail, context_window, &spliced_feats);
  this->TransformToOutput(spliced_feats, output);
  this->ComputeNextRemainder(input);
}

void PykaldiLdaInput::ComputeNextRemainder(const MatrixBase<BaseFloat> &input) {
  // The size of the remainder that we propagate to the next frame is
  // context_window - 1, if available.
  int32 context_window = left_context_ + 1 + right_context_;
  int32 next_remainder_len = std::min(context_window - 1,
                                      remainder_.NumRows() + input.NumRows());
  if (next_remainder_len == 0) {
    remainder_.Resize(0, 0);
    return;
  }
  Matrix<BaseFloat> next_remainder(next_remainder_len, input_dim_);
  int32 rsize = remainder_.NumRows(), isize = input.NumRows();
  for (int32 i = 0; i < next_remainder_len; i++) {
    SubVector<BaseFloat> dest(next_remainder, i);
    int32 t = (rsize + isize) - next_remainder_len + i;
    // Here, t is an offset into a numbering of the frames where we first have
    // the old "remainder" frames, then the regular frames.
    if (t < rsize) dest.CopyFromVec(remainder_.Row(t));
    else dest.CopyFromVec(input.Row(t - rsize));
  }
  remainder_ = next_remainder;
}



PykaldiDeltaInput::PykaldiDeltaInput(const DeltaFeaturesOptions &delta_opts,
                                   PykaldiFeatInputItf *input):
    input_(input), opts_(delta_opts), input_dim_(input_->Dim()) { }


// static
void PykaldiDeltaInput::AppendFrames(const MatrixBase<BaseFloat> &input1,
                                    const MatrixBase<BaseFloat> &input2,
                                    const MatrixBase<BaseFloat> &input3,
                                    Matrix<BaseFloat> *output) {
  const int32 size1 = input1.NumRows(), size2 = input2.NumRows(),
      size3 = input3.NumRows(), size_out = size1 + size2 + size3;
  if (size_out == 0) {
    output->Resize(0, 0);
    return;
  }
  // do std::max in case one or more of the input matrices is empty.
  int32 dim = std::max(input1.NumCols(),
                       std::max(input2.NumCols(), input3.NumCols()));

  output->Resize(size_out, dim);
  if (size1 != 0)
    output->Range(0, size1, 0, dim).CopyFromMat(input1);
  if (size2 != 0)
    output->Range(size1, size2, 0, dim).CopyFromMat(input2);
  if (size3 != 0)
    output->Range(size1 + size2, size3, 0, dim).CopyFromMat(input3);
}

void PykaldiDeltaInput::DeltaComputation(const MatrixBase<BaseFloat> &input,
                                        Matrix<BaseFloat> *output,
                                        Matrix<BaseFloat> *remainder) const {
  int32 input_rows = input.NumRows(),
      output_rows = std::max(0, input_rows - Context() * 2),
      remainder_rows = std::min(input_rows, Context() * 2),
      input_dim = input_dim_,
      output_dim = Dim();
  if (remainder_rows > 0) {
    remainder->Resize(remainder_rows, input_dim);
    remainder->CopyFromMat(input.Range(input_rows - remainder_rows,
                                       remainder_rows, 0, input_dim));
  } else {
    remainder->Resize(0, 0);
  }
  if (output_rows > 0) {
    output->Resize(output_rows, output_dim);
    DeltaFeatures delta(opts_);
    for (int32 output_frame = 0; output_frame < output_rows; output_frame++) {
      int32 input_frame = output_frame + Context();
      SubVector<BaseFloat> output_row(*output, output_frame);
      delta.Process(input, input_frame, &output_row);
    }
  } else {
    output->Resize(0, 0);
  }
}

void PykaldiDeltaInput::Compute(Matrix<BaseFloat> *output) {
  KALDI_ASSERT(output->NumRows() > 0 &&
               output->NumCols() == Dim());
  // If output->NumRows() == 0, it corresponds to a request for zero frames,
  // which makes no sense.

  // We request the same number of frames of data that we were requested.
  Matrix<BaseFloat> input(output->NumRows(), input_dim_);
  input_->Compute(&input);

  // If we got no input (timed out) and we're not at the end, we return
  // empty output.
  if (input.NumRows() == 0) {
    output->Resize(0, 0);
    return;
  }

  // If this is the first segment of the utterance, we put in the
  // initial duplicates of the first frame, numbered "Context()"
  if (remainder_.NumRows() == 0 && input.NumRows() != 0 && Context() != 0) {
    remainder_.Resize(Context(), input_dim_);
    for (int32 i = 0; i < Context(); i++)
      remainder_.Row(i).CopyFromVec(input.Row(0));
  }

  // // If this is the last segment, we put in the final duplicates of the
  // // last frame, numbered "Context()".
  // // FIXME we do not detect last frame
  Matrix<BaseFloat> tail;
  // if (Context() > 0) {
  //   tail.Resize(Context(), input_dim_);
  //   for (int32 i = 0; i < Context(); i++) {
  //     if (input.NumRows() > 0)
  //       tail.Row(i).CopyFromVec(input.Row(input.NumRows() - 1));
  //     else
  //       tail.Row(i).CopyFromVec(remainder_.Row(remainder_.NumRows() - 1));
  //   }
  // }

  Matrix<BaseFloat> appended_feats;
  AppendFrames(remainder_, input, tail, &appended_feats);
  DeltaComputation(appended_feats, output, &remainder_);
}



} // namespace kaldi
