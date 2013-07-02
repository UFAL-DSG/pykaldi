#include <string.h>
#include <stdio.h>
#include <dlfcn.h>

#include "dummyio.h"


int main() {
    void *lib = dlopen("libdummyio.so", RTLD_NOW);
    if (!lib) {
        printf("Cannot open library: %s\n", dlerror());
        return 1;
    }   

    dlerror();  // reset errors
    char * nameFce = "return_answer";
    return_answer_t  ret_ans = (return_answer_t)dlsym(lib, nameFce); 
    /* f_t  ret_ans = (f_t)dlsym(lib, nameFce);  */
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        printf("Cannot load symbol '%s', %s\n", nameFce, dlsym_error );
        dlclose(lib);
        return 1;
    }

    dlerror();  // reset errors
    nameFce = "frame_in";
    frame_in_t  fr_in = (frame_in_t)dlsym(lib, nameFce); 
    dlsym_error = dlerror();
    if (dlsym_error) {
        printf("Cannot load symbol '%s', %s\n", nameFce, dlsym_error );
        dlclose(lib);
        return 1;
    }

    // sending data in
    char *frame = "sending data in";
    mysizet len = strlen(frame);
    /* fr_in(frame, len);  */

    // receiving the answer FIXME
    char * answer;
    mysizet ans_size=0;
    double probability= 6;
    ret_ans(&probability, &answer, &ans_size);

    printf("%s of length %lu with probability %f\n", answer, ans_size, probability);

    dlclose(lib);
    return 0;
}
