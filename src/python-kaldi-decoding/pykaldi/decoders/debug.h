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
