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

/*
The designed is based on:
http://stackoverflow.com/questions/260915/how-can-i-create-a-dynamically-sized-array-of-structs
Stackoverflow answers are licenses under http://creativecommons.org/licenses/by-sa/2.5/
and I consider Apache 2.0 share alike license.
However, KEEP THIS HEADER for ATTRIBUTION!
*/

#ifndef KALDI_PYTHON_KALDI_FRAMES_H_
#define KALDI_PYTHON_KALDI_FRAMES_H_
#include <stdlib.h>

typedef struct {
    unsigned char ** frames;
    size_t num_frames;
    size_t frame_size;
    size_t allocated_size;
    // block_size - how much space we allocate in advance
    size_t block_size;
} frame_list;

#ifdef __cplusplus
extern "C" {
#endif

frame_list * create_frame_list(size_t block_size, size_t frame_size);
void delete_frame_list(frame_list * pfl);
int add_frame_to_list(frame_list * pfl, unsigned char * frame);
unsigned char ** frame_list_start(frame_list *pfl);
unsigned char ** frame_list_end(frame_list *pfl);
int debug_list(frame_list * fl);

#ifdef __cplusplus
}
#endif

#endif // #ifndef KALDI_PYTHON_KALDI_FRAMES_H_
