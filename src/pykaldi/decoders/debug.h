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


#ifndef KALDI_PYTHON_KALDI_DEBUG_H_
#define KALDI_PYTHON_KALDI_DEBUG_H_
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "frames.h"

#ifdef __cplusplus
extern "C" {
#endif

snd_pcm_t * play_setup(void);
void play_tear_down(snd_pcm_t *handle);
int play(snd_pcm_t *handle, unsigned char * buffer, size_t size);
int play_list(snd_pcm_t *handle, frame_list * fl);

#ifdef __cplusplus
}
#endif

#endif // #ifndef KALDI_PYTHON_KALDI_DEBUG_H_
