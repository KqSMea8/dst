/***************************************************************************
 * 
 * Copyright (c) 2013 Baidu.com, Inc. All Rights Reserved
 * $Id$ 
 * 
 **************************************************************************/
 
 
 
/**
 * @file lfconvert.cc
 * @author z(z@baidu.com)
 * @date 2013/11/01 12:00:01
 * @version $Revision$ 
 * @brief convert text local file to dstream streaming input format localfile
 *  
 **/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#define MAXLINE 4<<20

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("usage: %s src_file dst_file\n", argv[0]);
        exit(-1);
    }

    FILE *infile = fopen(argv[1], "r");
    FILE *outfile = fopen(argv[2], "a+");
    if (infile == NULL || outfile == NULL) {
        perror("open file error.");
        exit(-2);
    }

    char *line = new char[MAXLINE]; 
    while (fgets(line, MAXLINE, infile) != NULL) {
        uint64_t len = strlen(line);
        fwrite((const char*)&len, sizeof(len), 1, outfile);
        fwrite(line, len, 1, outfile);
    }
    delete [] line;
    fclose(infile);
    fclose(outfile);

    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
