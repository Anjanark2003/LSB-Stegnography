#ifndef DECODE_H

#define DECODE_H

#include"types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)

typedef struct _DecodeInfo
{
	char *stego_image_fname;
	FILE *fptr_stego_image;

	char *secret_fname;
	FILE *fptr_secret;
	char *extn_secret_file;
	int size_secret_file;

	char *magic_data;
	char image_data[MAX_IMAGE_BUF_SIZE];

}DecodeInfo;

/* Read and validate Decode args from argv */
Status read_and_validate_decode_args(int argc,char *argv[],DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_decode_files(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode secret file extenstion size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(int *file_size,DecodeInfo *decInfo);

/* Decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_data_from_image(int size,FILE *fptr_stego_image,DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
Status decode_byte_from_lsb(char *data,char *image_buffer);

/* Decode sizee into LSB of image data array */
Status decode_size_from_lsb(int *size,char *image_buffer);

/* Decode extension data from image data array */
Status decode_extn_data_from_image(int file_size,FILE *fptr_stego_image,DecodeInfo *decInfo);


#endif
