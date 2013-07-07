/*
Author: Ondrej Platek, UFAL MFF UK
Date: 2013
License: Apache, Version 2.0  (Please keep this header)


*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dummyio.h"
#include "frames.h"
#include "debug.h"


void return_answer(double * prob, char **ans, size_t *size) {
    *prob = -5 ;
    char *tmp = "Dummy answer from C = Ondra testing";
    *size = strlen(tmp);
    *ans = (char*) malloc(sizeof(char) * (*size));
    strcpy(*ans, tmp);
}

void frame_in(unsigned char *str_frame, size_t size) {
    FILE *fp;

    /* open the file for appending*/
    fp = fopen("test_frames.dat", "a+");
    if (fp == NULL) {
        printf("I couldn't open results.dat for writing.\n");
    }

    printf("note that size %lu is unused\n", size);
    fprintf(fp, "%s", str_frame);
    fclose(fp);
}
