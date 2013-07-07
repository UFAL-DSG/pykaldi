#include <stdio.h>
#include <stdlib.h>

#include "frames.h"


int debug_list(frame_list * fl) {
    char unsigned ** iter;
    char unsigned ** list_end = frame_list_end(fl);

    for( iter = frame_list_start(fl); iter != list_end; ++iter) 
        printf("string: %s\n", *iter);
    return 0;
} 

frame_list * create_frame_list(size_t block_size, size_t frame_size) {
    frame_list * pfl = (frame_list*) malloc(sizeof(frame_list));
    if (pfl != NULL) {
        pfl->num_frames = 0;
        pfl->block_size = block_size;
        pfl->frame_size = frame_size;
        pfl->allocated_size = block_size;
        // allocate some place in advance
        pfl->frames = (unsigned char **)malloc(sizeof(unsigned char *) * block_size);
        if (pfl->frames == NULL) {
            free(pfl);
            return NULL;
        }
    }
    return pfl;
}

void delete_frame_list(frame_list * pfl) {
    free(pfl->frames);
    free(pfl);
}

int add_frame_to_list(frame_list * pfl, unsigned char * frame) {
    size_t num_frames = pfl->num_frames;
    if (num_frames >= pfl->allocated_size) {
        size_t new_size = pfl->allocated_size + pfl->block_size;
        void * new_frames = realloc(pfl->frames, sizeof(unsigned char*) * new_size);
        if (new_frames == NULL) {
            return 0;
        } else {
            pfl->allocated_size = new_size;
            pfl->frames = (unsigned char **)new_frames;
        }
    }
    pfl->frames[num_frames] = frame;
    ++pfl->num_frames;

    /* printf("num_frames %zd, frame_size: %zd\n", pfl->num_frames, pfl->frame_size); */
    return 1;
}

unsigned char ** frame_list_start(frame_list *pfl) {
    return pfl->frames;
}

unsigned char ** frame_list_end(frame_list *pfl) {
    return &pfl->frames[pfl->num_frames];
}
