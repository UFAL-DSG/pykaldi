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

void frame_in(char * dummydec, unsigned char *str_frame, size_t size) {
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
