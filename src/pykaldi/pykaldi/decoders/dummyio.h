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


#ifndef KALDI_PYTHON_KALDI_DUMMYIO_H_
#define KALDI_PYTHON_KALDI_DUMMYIO_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*return_answer_t)(double * prob, char **ans, size_t *size);
typedef void (*frame_in_t)(char *dummydec, unsigned char *str_frame, size_t size);
void return_answer(double * prob, char **ans, size_t *size);
void frame_in(char *dummydec, unsigned char *str_frame, size_t size);

#ifdef __cplusplus
}
#endif

#endif // #ifndef KALDI_PYTHON_KALDI_DUMMYIO_H_
