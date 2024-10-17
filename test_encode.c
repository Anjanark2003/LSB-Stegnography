#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include"decode.h"

int main(int argc,char *argv[])
{
   if(argc<=1)
   {
	 printf("ERROR ! Enter more than 3 or more arguments\n");
	 return 1;
   } 
   //Whether the user wants to perform encoding or decoding
   
   if(check_operation_type(argv)== e_encode)
   {
	   printf("Selected encoding\n");
	   EncodeInfo encInfo;

	   //Read and validate the arguments

	   if(read_and_validate_encode_args(argc,argv,&encInfo)== e_success)
	   {
		   printf("Read and validate is success\n");

		   if(do_encoding(&encInfo)==e_success)
		   {
			   printf("Encoding is Successful\n");
		   }
		   else
		   {
			   printf("Encoding Failed\n");
		   }
	   }
	   else
	   {
		   printf("Read and validate Failed\n");
		   printf("Please pass ./a.out -e beautiful.bmp secret.extn\n");
		   return 1;
	   }
   }
   else if(check_operation_type(argv)== e_decode)
   {
	    printf("Selected decoding\n");
	    DecodeInfo decInfo;

           //Read and validate the arguments

           if(read_and_validate_decode_args(argc,argv,&decInfo)== e_success)
           {
                   printf("Read and validate is success\n");

                   if(do_decoding(&decInfo)==e_success)
                   {
                           printf("Decoding is Successful\n");
                   }
                   else
                   {
                           printf("Decoding Failed\n");
                   }
           }
           else
           {
                   printf("Read and validate Failed\n");
                   printf("Please pass ./a.out -d Stego.bmp [output file name]\n");
                   return 1;
           }
   }
   else
   {
	    printf("Invalid input\n");
   }
   
        

  /*  EncodeInfo encInfo;
    uint img_size;

    // Fill with sample filenames
    encInfo.src_image_fname = "beautiful.bmp";
    encInfo.secret_fname = "secret.txt";
    encInfo.stego_image_fname = "stego_img.bmp";

    // Test open_files
    if (open_files(&encInfo) == e_failure)
    {
    	printf("ERROR: %s function failed\n", "open_files" );
    	return 1;
    }
    else
    {
    	printf("SUCCESS: %s function completed\n", "open_files" );
    }

    // Test get_image_size_for_bmp
    img_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    printf("INFO: Image size = %u\n", img_size);
  */
    return 0;
}

OperationType check_operation_type(char *argv[])
{
	if(strcmp(argv[1],"-e")==0)
		return e_encode;
	if(strcmp(argv[1],"-d")==0)
		return e_decode;
	else
		return e_unsupported;
}
