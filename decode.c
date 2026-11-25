//header files
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

//opening the files in required mode
Status open_files_for_decode(DecodeInfo *decInfo)
{
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    //performing the validation
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    //opening the output file
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");

    //performing the validation
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen output");
        return e_failure;
    }
    return e_success;
}

//read and validate the arduments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (strcmp(strchr(argv[2], '.'), ".bmp") != 0)
        return e_failure;

    decInfo->stego_image_fname = argv[2];

    if (argv[3] != NULL)
        decInfo->output_fname = argv[3];
    else
        decInfo->output_fname = "output.txt";

    return e_success;
}

//skipping the header
Status skip_bmp_header(FILE *fptr_stego_image)
{
    fseek(fptr_stego_image, 54, SEEK_SET);
    return e_success;
}

//decode byte from lsb
char decode_byte_from_lsb(char *data, char *image_buffer)
{
    unsigned char ch = 0;
    for (int i = 0; i < 8; i++)
    {
        ch = (ch << 1) | (image_buffer[i] & 1);
    }
    *data = (char)ch;
    return ch;
}

//decode size from lsb
int decode_size_from_lsb(char *image_buffer)
{
    int size = 0;
    for (int i = 0; i < 32; i++)
        size = (size << 1) | (image_buffer[i] & 1);
    return size;
}

//decode data from image
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image)
{
    char image_buffer[8];
    for(int i=0; i<size; i++)
    {
        fread(image_buffer, 8, 1, fptr_stego_image);
        decode_byte_from_lsb(&data[i], image_buffer);
    }
    return e_success;
}

//decode magic string
Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    char str[3];
    decode_data_from_image(str, 2, decInfo->fptr_stego_image);
    str[2] = '\0';
    strcpy(magic_string, str);
    if (strcmp(str, MAGIC_STRING) == 0)
        return e_success;
    return e_failure;
}


//decode secret file extn size
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char image_buffer[32];
    fread(image_buffer, 1, 32, decInfo->fptr_stego_image);
    decInfo->size_extn_file = decode_size_from_lsb(image_buffer);
    return e_success;
}

//decode secret file extn
Status decode_secret_file_extn(char *extn, DecodeInfo *decInfo)
{
    decode_data_from_image(extn, decInfo->size_extn_file, decInfo->fptr_stego_image);
    extn[decInfo->size_extn_file] = '\0';
    strcpy(decInfo->extn_secret_file, extn);
    return e_success;
}

//decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char image_buffer[32];
    fread(image_buffer, 1, 32, decInfo->fptr_stego_image);
    decInfo->size_secret_file = decode_size_from_lsb(image_buffer);
    printf("DEBUG file size = %d\n", decInfo->size_secret_file);
    return e_success;
}

//decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char ch;
    for(int i=0; i < decInfo->size_secret_file; i++)
    {
        decode_data_from_image(&ch, 1, decInfo->fptr_stego_image);
        fwrite(&ch, 1, 1, decInfo->fptr_output);
    }
    return e_success;
}

//performing the decoding
Status do_decoding(DecodeInfo *decInfo)
{
    
    char magic[10];
    char extn[10];

    //opening the stego.bmp file/ decoding file
    if (open_files_for_decode(decInfo) == e_success)
    {
        printf("Opened decoding files successfully\n");
    }
    else
    {
        printf("Failed to open the decoding files successfully\n");
        return e_failure;
    }

    //skipping the header
    if (skip_bmp_header(decInfo->fptr_stego_image) == e_success)
    {
        printf("Skipped header successfully\n");
    }
    else
    {
        printf("Failed to skip the header the successfully\n");
        return e_failure;
    }    

    //decode the magic string
    if (decode_magic_string(magic, decInfo) == e_success)
    {
        printf("Decoded Magic string successfully\n");
    }
    else
    {
        printf("Failed to Decode magic string successfully\n");
        return e_failure;
    }    

    //decode the secret file extn size
    if (decode_secret_file_extn_size(decInfo) == e_success)
    {
        printf("Decoded extn size %d successfully\n", decInfo->size_extn_file);
    }
    else
    {
        printf("Failed to Decode extn size successfully\n");
        return e_failure;
    }    

    //decode the secret file extn
    if (decode_secret_file_extn(extn, decInfo) == e_success)
    {
        printf("Decoded extn %s successfully\n", extn);
    }
    else
    {
        printf("Failed to decode the secret file extn successfully\n");
        return e_failure;
    }

    //decode the secret file size
    if (decode_secret_file_size(decInfo) == e_success)
    {
        printf("Decoded file size successfully\n");
    }
   else
   {
       printf("Failed to decode the secret file size successsfully\n");
       return e_failure;
   }

   //decode the secret file data
    if (decode_secret_file_data(decInfo) == e_success)
    {
        printf("Decoded secret file data successfully\n");
    }
    else
    {
        printf("Failed to decode the secret file data successfully\n");
        return e_failure;
    }    
    return e_success;
}