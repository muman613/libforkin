/**
 * @file    popen2.h
 * @author  Michael Uman
 * @date    Sep 17, 2018
 */

#ifndef FORKIN_POPEN2_H
#define FORKIN_POPEN2_H

#include <stdio.h>

#define PIPE_READ_END   0
#define PIPE_WRITE_END  1

//FILE *  popen2(const char* command, const char * type);
FILE * popen2(const char* const args[], const char * type);

int     pclose2(FILE * fp);

#endif //FORKIN_POPEN2_H
