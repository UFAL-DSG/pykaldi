#ifndef KALDI_PYTHON_KALDI_DUMMYIO_H_
#define KALDI_PYTHON_KALDI_DUMMYIO_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*return_answer_t)(double * prob, char **ans, size_t *size);
typedef void (*frame_in_t)(unsigned char *str_frame, size_t size);
void return_answer(double * prob, char **ans, size_t *size);
void frame_in(unsigned char *str_frame, size_t size);

#ifdef __cplusplus
}
#endif

#endif // #ifndef KALDI_PYTHON_KALDI_DUMMYIO_H_
