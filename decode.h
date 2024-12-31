#ifndef DECODE_H
#define DECODE_H

#include "types.h"

#define MAX_OUTBMP_BUF_SIZE 8
#define MAX_OUTTXT_BUF_SIZE 1
#define MAX_FILE_SUFFIX 4

typedef unsigned int uint;

typedef class DecodeInfo
{
public:
    /* output.bmp -- source file*/
    char *image_fname;
    FILE *fptr_src_img;
    char image_extn[MAX_FILE_SUFFIX];
    char sec_file_extn[MAX_FILE_SUFFIX];

    int ext_size ;
    int secfile_size ;

    /* output.txt -- destination file*/
    char *out_txt_fname;
    FILE *fptr_out_txt;
    char out_sec_data[50];

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

Status open_files_decoding(DecodeInfo *decInfo);

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

Status decode_secret_file_extn(char *sec_file_extn, DecodeInfo *decInfo);

Status decode_secret_file_extn_size(int size, DecodeInfo *decInfo);

Status decode_secret_file_data_size(int file_size, DecodeInfo *decInfo);

Status decode_secret_file_data(DecodeInfo *decInfo);

Status decode_img_to_data(char *data, int size, FILE *fptr_out_txt, FILE *fptr_src_img);

Status decode_lsb_to_byte(char *data, char *image_buffer);

}DecodeInfo;

#endif
