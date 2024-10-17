#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"decode.h"
#include"types.h"
#include"common.h"

Status read_and_validate_decode_args(int argc,char *argv[],DecodeInfo *decInfo)
{
	if(argc<=2)
	{
		printf("ERROR ! Enter more than 3 or more arguments\n");
		return e_failure;
	}
	if(strcmp(strstr(argv[2], "."), ".bmp")==0)
	{
		decInfo->stego_image_fname=argv[2];
	}
	else
	{
		return e_failure;
	}
	if(argv[3]!=NULL)
	{
		decInfo->secret_fname=argv[3];
	}
	else
	{
		//decInfo->secret_fname=malloc(8);
		decInfo->secret_fname="decode";
	}
	if(argc>=5)
        {
                printf("ERROR ! Enter less than 5 arguments\n");
                return e_failure;
        }
	return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
	decInfo->fptr_stego_image=fopen(decInfo->stego_image_fname,"r");
	if(decInfo->fptr_stego_image==NULL)
	{
		perror("fopen");
		fprintf(stderr,"Error: Unable to open file %s\n",decInfo->stego_image_fname);
		return e_failure;
	}
	/*decInfo->fptr_secret=fopen(decInfo->secret_fname,"w");
	if(decInfo->fptr_secret==NULL)
	{
		perror("fopen");
		fprintf(stderr,"Error: Unable to open file %s\n",decInfo->secret_fname);
		return e_failure;
	}*/

	return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
	fseek(decInfo->fptr_stego_image,54,SEEK_SET);
	int i=strlen(MAGIC_STRING);
	decInfo->magic_data=malloc(i+1);
	char buffer[8];
	for(int j=0;j<i;j++)
	{
		fread(buffer,8,1,decInfo->fptr_stego_image);
		decode_byte_from_lsb(&decInfo->magic_data[j],buffer);
	}
	decInfo->magic_data[i]='\0';
	if(strcmp(decInfo->magic_data,MAGIC_STRING)==0)
	{
		return e_success;
	}
	else
	{
		printf("Magic string mismatch\n");
		return e_failure;
	}
}

Status decode_extn_data_from_image(int size,FILE *fptr_stego_image,DecodeInfo *decInfo)
{
	for(int i=0;i<size;i++)
	{
		fread(decInfo->stego_image_fname,8,1,fptr_stego_image);
		decode_byte_from_lsb(&decInfo->extn_secret_file[i],decInfo->stego_image_fname);
	}
	return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	char buffer[32];
	fread(buffer,32,1,decInfo->fptr_stego_image);
	decode_size_from_lsb(&decInfo->size_secret_file,buffer);
	return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	int i=decInfo->size_secret_file;
	decInfo->extn_secret_file=malloc(i+1);
	char buffer[8];
	for(int j=0;j<i;j++)
	{
		fread(buffer,8,1,decInfo->fptr_stego_image);
		decode_byte_from_lsb(&decInfo->extn_secret_file[j],buffer);
	}
	decInfo->extn_secret_file[i]= '\0';
        
       
	 //decInfo->secret_fname="decode";
	  // Dynamically allocate memory for the concatenated string
        char *output_fname_concat = malloc(strlen(decInfo->secret_fname) + strlen(decInfo->extn_secret_file) + 2);
        if (output_fname_concat == NULL)
        {
           return e_failure;
        }

        // Concatenate the strings
        strcpy(output_fname_concat, decInfo->secret_fname);
        strcat(output_fname_concat, decInfo->extn_secret_file);

    // Update decInfo->output_fname with the concatenated string
   // free(decInfo->output_fname);
        decInfo->secret_fname = output_fname_concat;
	//open secret file
	 decInfo->fptr_secret=fopen(decInfo->secret_fname,"w");
        if(decInfo->fptr_secret==NULL)
        {
                perror("fopen");
                fprintf(stderr,"Error: Unable to open file %s\n",decInfo->secret_fname);
                return e_failure;
	}

	return e_success;
}

Status decode_secret_file_size(int *file_size,DecodeInfo *decInfo)
{
	char buffer[32];
	fread(buffer,32,1,decInfo->fptr_stego_image);
	decode_size_from_lsb(file_size,buffer);
	decInfo->size_secret_file=*file_size;
	printf("File size is found to be %d\n",*file_size);
	return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
	char ch;
	for(int i=0;i<decInfo->size_secret_file;i++)
	{
		fread(decInfo->stego_image_fname,8,1,decInfo->fptr_stego_image);
		decode_byte_from_lsb(&ch,decInfo->stego_image_fname);
		fputc(ch,decInfo->fptr_secret);
	}
	return e_success;
}

Status decode_data_from_image(int size,FILE *fptr_stego_image,DecodeInfo *decInfo)
{
	char buffer[8];
	for(int i=0;i<size;i++)
	{
		fread(buffer,8,1,fptr_stego_image);
		decode_byte_from_lsb(&decInfo->magic_data[i],buffer);
	}
	return e_success;
}

Status decode_byte_from_lsb(char *data,char *image_buffer)
{
	*data=0;
	for(int i=0;i<8;i++)
	{
		*data=(*data<<1)|(image_buffer[i]&0x01);
	}
	return e_success;
}

Status decode_size_from_lsb(int *size,char *image_buffer)
{
	*size=0;
	for(int i=0;i<32;i++)
	{
		*size=(*size<<1)|(image_buffer[i]&0x01);
	}
	return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
	if(open_decode_files(decInfo)==e_success)
	{
		printf("File opened successfully\n");
		if(decode_magic_string(decInfo)==e_success)
		{
			printf("Magic String matched\n");
			if(decode_secret_file_extn_size(decInfo)==e_success)
			{
				printf("File extension size decoded\n");
				if(decode_secret_file_extn(decInfo)==e_success)
				{
					printf("File extension decoded \n");
					if(decode_secret_file_size(&decInfo->size_secret_file,decInfo)==e_success)
					{
						printf("Secret file size decoded\n");
						if(decode_secret_file_data(decInfo)==e_success)
						{

							printf("Secret file data decode successfully\n");
							printf("secret file written succesfully\n");
							return e_success;
						}
					}
				}
			}
		}
	}
	return e_failure;
}
