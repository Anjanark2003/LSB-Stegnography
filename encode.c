#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

// Function Definitions 
Status read_and_validate_encode_args(int argc,char *argv[], EncodeInfo *encInfo)
{
      if(argc<=2)
      {
	      printf("ERROR ! Enter more than 3 or more arguments\n");
	      return e_failure;
      }
      //Check whether .bmp file is provided
      if(strcmp((strstr(argv[2],".")),".bmp")==0)
      {
          encInfo->src_image_fname= argv[2];
      }
      else
      {
	return e_failure;
      }

      //Check whether extn file is provided
      if(strchr(argv[3],'.') !=NULL)
      {
	encInfo->secret_fname=argv[3];
        strcpy(encInfo ->extn_secret_file,strchr(argv[3],'.'));	
      }
      else
      {
        return e_failure;
      }      
      
      //Check whether new file is provided
      
      if(argv[4] !=NULL)
      {
	      encInfo->stego_image_fname=argv[4];
      }
      else
      {
	      encInfo->stego_image_fname="Stego.bmp";
      }
      if(argc>=5)
      {
	      printf("ERROR ! Enter 4 arguments\n");
	      return e_failure;
      }
        
      return e_success;

}


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

Status check_capacity(EncodeInfo *encInfo)
{
	//get the image size of bmp file
	encInfo->image_capacity=get_image_size_for_bmp(encInfo->fptr_src_image);        //get the size capacity of secret.txt file
        encInfo->size_secret_file=get_file_size(encInfo->fptr_secret);
	
	if(encInfo -> image_capacity > (54+(2+4+4+4+encInfo->size_secret_file)*8))
	{
		return e_success;
	}
	else
	{
		return e_failure;
	}
}

uint get_file_size(FILE *fptr)
{
	fseek(fptr,0,SEEK_END);
	return ftell(fptr);
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	char str[54];
	fseek(fptr_src_image,0,SEEK_SET);
	fread(str,54,1,fptr_src_image);
	fwrite(str,54,1,fptr_dest_image);
	return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
	encode_data_to_image (magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image);
	return e_success;
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
	char buffer[32];
	fread(buffer,32,1,encInfo->fptr_src_image);                                                            
       	encode_size_to_lsb(size,buffer);                                                                      
	fwrite(buffer,32,1,encInfo->fptr_stego_image);
	return e_success;
}

Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
	 encode_data_to_image (file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image);
        return e_success;
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
	char buffer[32];
	fread(buffer,32,1,encInfo->fptr_src_image);
        encode_size_to_lsb(file_size,buffer);
        fwrite(buffer,32,1,encInfo->fptr_stego_image);
	return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
	fseek(encInfo->fptr_secret,0,SEEK_SET);
	char buffer[encInfo->size_secret_file];
	fread(buffer,encInfo->size_secret_file,1,encInfo->fptr_secret);
	encode_data_to_image (buffer,strlen(buffer),encInfo->fptr_src_image,encInfo->fptr_stego_image);
        return e_success;
}

/*Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    char ch;
    while(fread(&ch,1,1,encInfo->fptr_src_image) !=0)
    {
	    fwrite(&ch,1,1,encInfo->fptr_stego_image);
    }
    e_success;
}*/


Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char buffer[8];
	for(int i=0;i<size;i++)
	{
		fread(buffer,8,1,fptr_src_image);
		encode_byte_to_lsb(data[i],buffer);
		fwrite(buffer,8,1,fptr_stego_image);
	}
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
	for(int i=0;i<=7;i++)
	{
		image_buffer[i]=((data & (1<<(7-i)))>>(7-i)) | image_buffer[i] & 0xFE;
		//image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7 - i)) & 0x01);
	}
	return e_success;
}

Status encode_size_to_lsb(int data, char *image_buffer)
{
	for(int i=0;i<=31;i++)
        {	
               image_buffer[i]=((data & (1<<(31-i)))>>(31-i)) | image_buffer[i] & 0xFE;
        }
}

Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    char ch;
    while(fread(&ch,1,1,encInfo->fptr_src_image) !=0)
    {
            fwrite(&ch,1,1,encInfo->fptr_stego_image);
    }
    e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
	//starts encoding
	if(open_files(encInfo)==e_success)
	{
		printf("Open file is successful\n");

		if(check_capacity(encInfo) == e_success)
		{
			printf("Check capacity is successful\n");
			
			if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)== e_success)
			{
				printf("Copied bmp header successfully\n");

			      	if(encode_magic_string(MAGIC_STRING,encInfo)== e_success)
				{
					printf("Encoded Magic string successfully\n");

					if(encode_secret_file_extn_size(strlen(encInfo ->extn_secret_file),encInfo)==e_success)
					{
				        	printf("Encoding secret file extension size is success\n");

						if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)== e_success)
						{
							printf("Encoding secret file extension is success\n");
							if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
							{
								printf("Encoding secret file size is successful\n");
								
									if(encode_secret_file_data(encInfo)== e_success)
									{
										printf("Encoding secret file data is success\n");
										if(copy_remaining_img_data(encInfo)== e_success)
										{
											return e_success;
										}
										else
										{
											return e_failure;
										}
									}
									else
									{
										printf("Encoding secret file data failed\n");
										return e_failure;
									}
							}
							else
							{
								printf("Encoding secret file size failed\n");
								return e_failure;
							}
						
						}
						else
						{
							 printf("Encoding secret file extension is Failed\n");
							 return e_failure;
						}
					}
					else
					{
						printf("Encoding secret file extension size Failed\n");
						return e_failure;
					}
				}
                                else
                                {
                                        printf("Encoded Magic String failed\n");
                                        return e_failure;
                                }
                        }
                        else
                        {
                                printf("Copied bmp header failure\n");
                                return e_failure;
                        }
                }
                else
                {
                        printf("Check capacity is failure\n");
                        return e_failure;
                }
        }
        else
        {
                printf("Open file is failed\n");
                return e_failure;
        }
        return e_success;
}
				
