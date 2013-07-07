/*
The designed based on:
http://stackoverflow.com/questions/260915/how-can-i-create-a-dynamically-sized-array-of-structs
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
