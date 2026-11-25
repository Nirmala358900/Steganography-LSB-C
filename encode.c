#include<stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
 
//opening the files in required mode
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

//read and validate the arguments
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //check if proper bmp is passed
    if(strcmp(strchr(argv[2] , '.'), ".bmp") == 0)
        encInfo -> src_image_fname = argv[2];
    else
       return e_failure;
    
    //check if proper txt file is passed
    if(strcmp(strchr(argv[3] , '.'),".txt") == 0)
        encInfo -> secret_fname = argv[3];
    else
       return e_failure;

    //check if the output file name is passed
    if(argv[4] == NULL)
       encInfo -> stego_image_fname = "stego.bmp" ;  
    else
    {
       encInfo -> stego_image_fname = argv[4];
    }
       return e_success;    
}

//checking for the capacity
Status check_capacity(EncodeInfo *encInfo)
{
    //get the size of beautiful.bmp 
    encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image);
    encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);
    if(encInfo -> image_capacity > (54 + (2 + 4 + 4 + 4 + encInfo -> size_secret_file) * 8))
         return e_success;
    else  
         return e_failure;
}

//get the file size
uint get_file_size(FILE *fptr)
{
    uint size;
    fseek(fptr , 0 , SEEK_END);
    size = ftell(fptr);
    rewind(fptr);
    return size;
}


//function for copying the header from beautiful.bmp to stego.bmp
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek(fptr_src_image, 0 , SEEK_SET);
    char str[54];
    fread(str, 54, 1, fptr_src_image);
    fwrite(str, 54, 1, fptr_dest_image);
    return e_success;
}

//encoding the magic string
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string,2,encInfo);
    return e_success;
}

//encode data to image
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{
    unsigned char image_buffer[8];
    for(int i=0 ; i < size; i++)
    {
        fread(image_buffer, 8 , 1 , encInfo -> fptr_src_image);
        // hide 1 char in these 8 bytes
        encode_byte_to_lsb(data[i], (char*)image_buffer);
        fwrite(image_buffer, 8, 1, encInfo -> fptr_stego_image);
    }
    return e_success;
}

//encode byte to lsb
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0 ; i < 8 ; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7 - i)) &1);
    }
    return e_success;

}

//encode size to lsb
Status encode_size_to_lsb(int data, char *image_buffer)
{
    for(int i=0 ; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (31 - i)) &1);
    }
}

//encode secret file extn size
Status encode_secret_file_extn_size(int extn_size, EncodeInfo *encInfo)
{
    //encode the exten size into stego.bmp
    char buffer[32];

    //read the 32 bytes from the beautiful.bmp
    fread(buffer, 32 , 1, encInfo -> fptr_src_image);

    //encode the extn size into the buffer
    encode_size_to_lsb(extn_size ,buffer);

    //write the encoded data into stego.bmp
    fwrite(buffer, 32, 1, encInfo -> fptr_stego_image);

    return e_success;
}

//encode secret file extn
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
       encode_data_to_image(file_extn, 4, encInfo);
       return e_success;
}

//encode secret file size
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
   //encode the secret size into stego.bmp
    char buffer[32];

    //read the 32 bytes from the beautiful.bmp
    fread(buffer, 32 , 1, encInfo -> fptr_src_image);

    //encode the secret size into the buffer
    encode_size_to_lsb(file_size ,buffer);

    //write the encoded data into stego.bmp
    fwrite(buffer, 32, 1, encInfo -> fptr_stego_image);

    return e_success;
}

//encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char ch;
    rewind(encInfo->fptr_secret);
    for(int i=0; i<encInfo->size_secret_file; i++)
    {
        fread(&ch, 1, 1, encInfo->fptr_secret);
        char image_buffer[8];
        fread(image_buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(ch, image_buffer);
        fwrite(image_buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

//copying the remaining data to stego.bmp
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_stego_image)
{
    char ch;
    //till end of file, read 1 byte and write 1 byte
    while(fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1,fptr_stego_image);
    }
    return e_success;
}

//performing the encoding
Status do_encoding(EncodeInfo *encInfo)
{
    //open required files in required mode
    if(open_files(encInfo) == e_success)
       printf("Opened files successfully\n");
    else
    {
        printf("Failed to open files\n");
        return e_failure;
    }

    //check if beautiful.bmp is sufficient to perform the encoding
    if(check_capacity(encInfo) == e_success)
    {
        printf("Check capacity is successful\n");
        printf("\"%s\" file is suitable for encoding\n", encInfo -> src_image_fname);
    }
    else
    {
       printf("\"%s\" file is not suitable for encoding\n", encInfo -> src_image_fname);
       return e_failure;
    }

    rewind(encInfo -> fptr_src_image);
    rewind(encInfo -> fptr_secret);

    //copying the header of bmp file to stego.bmp
    if(copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) == e_success)
    {
        printf("Copied header successfully\n");
    }
    else
    {
        printf("Failed to copy from bmp to header successfully\n");
        return e_failure;
    }

    //encoding of magic string
    if(encode_magic_string(MAGIC_STRING,encInfo) == e_success)
    {
        printf("magic string encoded successfully\n");

    }
    else
    {
         printf("magic string not encoded successfully\n");
         return e_failure;
    }

    //get the extern from secret file name.txt
    char *extn = strstr(encInfo -> secret_fname, "." );
    printf("extn len => %zus\n", strlen(extn));

    //encode exten file size
    if(encode_secret_file_extn_size(strlen(extn),encInfo) == e_success)
    {
        printf("Encoded the secret file exten size successfully\n");
    }
    else
    {
        printf("Fail to Encoding the secret file exten size successfully\n");
        return e_failure;
    }

    //encode exten file
    if(encode_secret_file_extn(extn,encInfo) == e_success)
    {
        printf("Encoded the secret file exten successfully\n");
    }
    else
    {
        printf("Failed to encode exten file successfully\n");
        return e_failure;
    }

    //encode the secret file size
    if(encode_secret_file_size(encInfo -> size_secret_file, encInfo) == e_success)
    {
        printf("Encoded secret file size successfully\n");
    }
    else
    {
        printf("Failed to encode secret file size successfully\n");
        return e_failure;
    }

    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("Encoded secret file data successfully\n");
    }
    else
    {
        printf("Failed to Encode the secret file data successfully\n");
        return e_failure;
    }
    
    //copying the remaining from src to dest
    if(copy_remaining_img_data(encInfo -> fptr_src_image, encInfo ->fptr_stego_image) == e_success)
    {
        printf("copied the data successfully\n");
    }
    else
    {
        printf("Failed to copy the data successfully\n");
        return e_failure;
    }


    return e_success;

}

