#ifndef KALDI_PYTHON_KALDI_DUMMYIO_H_
#define KALDI_PYTHON_KALDI_DUMMYIO_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long mysizet;
typedef void (*return_answer_t)(double * prob, char **ans, mysizet *size);
typedef void (*frame_in_t)(char *str_frame, mysizet size);
void return_answer(double * prob, char **ans, mysizet *size);
void frame_in(char *str_frame, mysizet size);

#ifdef __cplusplus
}
#endif

#endif // #ifndef KALDI_PYTHON_KALDI_DUMMYIO_H_
