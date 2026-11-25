/*Name : Nirmala B
Date : 24/11/2026
Description : This project hides a secret text file inside a BMP image using LSB Steganography in C language.
It replaces the least significant bit of image bytes with secret data so the image looks unchanged.
Encoding converts beautiful.bmp + secret.txt into stego.bmp by hiding magic string, file extension, size and data.
Decoding extracts the hidden data from stego.bmp and recreates the original output.txt file.
It is implemented using file handling, bitwise operations and structures in C.
*/

#include<stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char **argv)
{
    //validate if we have sufficient no of elements
    if(argc <= 2)
    {
        return 0;
    }

    //check if the user has selected encoding or decoding
    if(check_operation_type(argv) == e_encode)
    {
        printf("User selected encoding \n");

        printf("--------------Strat encoding------------\n");
        EncodeInfo encoding;
        if(read_and_validate_encode_args(argv , &encoding) == e_success)
        {
            printf("Validated encoding arguments successfully\n");
            if(do_encoding(&encoding) == e_success)
            {
                printf("do the encoding:\n");
            }
            else
            {
                printf("fialure to encoding\n");
            }
            

        }
        else
        {
            printf("Failed to validate encode arguments \n");
            return -1;
        }
    }
    else if(check_operation_type(argv) == e_decode)
    {
        printf("User selected decoding \n");
        printf("----------------Start decoding-------------\n");
        DecodeInfo decoding;
        if(read_and_validate_decode_args(argv, &decoding) == e_success)
        {
            printf("Validated decoding arguments successfully\n ");
            if(do_decoding(&decoding) == e_success)
            {
                printf("do the decoding\n");
            }
            else
            {
                printf("failure to do the decoding\n");
            }
        }
        else
        {
            printf("failed to validate decode argumnets\n");
            return -1;
        }
    }
    else
    {
        printf("Invalid arguments\nplease pass: \n");
        printf("For encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp] \n");
        printf("For decoding : ..a.out -d stego.bmp [output.txt] \n");
    }
    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
       return e_encode ;

    else if(strcmp(argv[1], "-d") == 0)
        return e_decode;

    else
        return e_unsupported;
}