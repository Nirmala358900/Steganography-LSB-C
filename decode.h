#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include "encode.h"
#include <stdio.h>

#define MAX_SECRET_BUF_SIZE 1
#define MAX_FILE_SUFFIX 4

/* 
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _DecodeInfo
{
    /* decode Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* output Image info */
    char *output_fname;
    FILE *fptr_output;

    char extn_secret_file[MAX_FILE_SUFFIX];
    char image_data[MAX_IMAGE_BUF_SIZE];

    int size_secret_file;
    int size_extn_file;
} DecodeInfo;

/* decoding function prototypes */

/* read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Open the decoding file*/
Status open_files_for_decode(DecodeInfo *decInfo);

/* Skipping the header of decoding file */
Status skip_bmp_header(FILE *fptr_stego_image);

/* decoding the magic string*/
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

/* decoding the secret file extn size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* decoding the secret file extn*/
Status decode_secret_file_extn(char *extn, DecodeInfo *decInfo);

/* decoding the secret file size*/
Status decode_secret_file_size(DecodeInfo *decInfo);

/* decoding the secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* decoding the data to image*/
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image);

/* decode byte to lsb*/
char decode_byte_from_lsb(char *data, char *image_buffer);

/* decode size from lsb*/
int decode_size_from_lsb(char *image_buffer);

#endif