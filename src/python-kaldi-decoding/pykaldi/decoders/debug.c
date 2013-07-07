#include <alsa/asoundlib.h>
#include <stdio.h>

#include "debug.h"
#include "frames.h"

int play_list(snd_pcm_t *handle, frame_list * fl) {

    char unsigned ** iter;
    char unsigned ** list_end = frame_list_end(fl);
    size_t frame_size = fl->frame_size;
    int ret_val;

    for( iter = frame_list_start(fl); iter != list_end; iter++) {
        char unsigned * buffer = *iter;
        /* printf("num_frames %zd, frame_size: %zd\n", fl->num_frames, frame_size); */
        /* printf("itter: %p\n", ( void * )iter); */
        // pcm is stored like char array:  2 chars == one 16bit sample -> frame_size/2
        ret_val = play(handle, buffer, frame_size / 2);

        if (ret_val != 0) {
            return ret_val;
        }
    }
    return 0;
} 


int play(snd_pcm_t *handle, unsigned char * buffer, size_t d) {
        snd_pcm_sframes_t frames;
        frames = snd_pcm_writei(handle, buffer, d);
        if (frames < 0)
                frames = snd_pcm_recover(handle, frames, 0);
        if (frames < 0) {
            printf("snd_pcm_writei failed: %s\n", snd_strerror(frames));
            return 1;
        }
        if (frames > 0 && frames < (long)sizeof(buffer))
                printf("Short write (expected %li, wrote %li)\n", (long)sizeof(buffer), frames);
        return 0;
}

snd_pcm_t * play_setup(void) {
    int err;
    snd_pcm_t *handle;
    if ((err = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
        printf("Playback open error: %s\n", snd_strerror(err));
        return NULL;
    }
    if ((err = snd_pcm_set_params(handle,
                                  SND_PCM_FORMAT_S16_LE,
                                  SND_PCM_ACCESS_RW_INTERLEAVED,
                                  1,
                                  16000,
                                  1,
                                  500000)) < 0) {   /* 0.5sec */
        printf("Playback open error: %s\n", snd_strerror(err));
        return NULL;
    }
    return handle;
}


void play_tear_down(snd_pcm_t *handle) {
    // tear down play
    snd_pcm_close(handle);
}
