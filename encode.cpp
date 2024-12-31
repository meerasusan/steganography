#include <iostream>
#include "types.h"
#include "common.h"
#include<string.h>
#include "encode.h"
using namespace std;
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint EncodeInfo::get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
   // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
   // printf("height = %u\n", height);

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
Status EncodeInfo::open_files(EncodeInfo *encInfo)
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

Status EncodeInfo::read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
    if(strstr(argv[2],".bmp")!=NULL)
	encInfo->src_image_fname = argv[2];
    else
    {
	cout<<"ERROR : Not a bm file.\n";
	return e_failure;
    }
    if(strstr(argv[3],".txt")!=NULL)
    {
	encInfo->secret_fname = argv[3];
	strcpy(encInfo->extn_secret_file,".txt");
    }
 else if(strstr(argv[3],".sh")!=NULL)
    {
	encInfo->secret_fname = argv[3];
	strcpy(encInfo->extn_secret_file,".sh");
    }
    else if(strstr(argv[3],".pdf")!=NULL)
    {
	encInfo->secret_fname = argv[3];
	strcpy(encInfo->extn_secret_file,".pdf");
    }
    else if(strstr(argv[3],".c")!=NULL)
    {
	encInfo->secret_fname = argv[3];
	strcpy(encInfo->extn_secret_file,".c");
    }
    else
    {
	cout<<"ERROR : Enter the correct extention.\n";
	return e_failure;
    }
 if(argv[4])
	encInfo->stego_image_fname = argv[4];
    else
	encInfo->stego_image_fname = "output.txt";

    return e_success;
}

Status EncodeInfo::do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success)
    {
	cout<<"Files have opened successfully\n";
	if(check_capacity(encInfo) == e_success)
	   cout<<"The files are comptible\n";
	else
	    return e_failure;
    }
 else
	return e_failure;
    if(copy_bmp_header(encInfo->fptr_src_image , encInfo->fptr_stego_image) == e_success)
	cout<<"Copied the heder(54 bytes) successfully\n";
    else
	return e_failure;

    if(encode_magic_string(MAGIC_STRING, encInfo)== e_success)
	cout<<"Magic String encoded\n";
    else
	return e_failure;

    if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo)== e_success)
	cout<<"secret file extension size encoded.\n";
    else
	return e_failure;
 if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo)== e_success)
	cout<<"secret file extension encoded successfuly.\n";
    else
	return e_failure;

    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
	cout<<"secret file size encoded.\n";
    else
	return e_failure;

    if(encode_secret_file_data(encInfo) == e_success)
	cout<<"secret file data ncodd successfully.\n";
    else
	return e_failure;

    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
	cout<<"copyed remaining data successully.\n";
    else
	return e_failure;

    return e_success;

}
Status EncodeInfo::check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    int req_img_cpty = strlen(MAGIC_STRING)*8 + 32 + 32 + 32 + 54 + encInfo->size_secret_file*8;
    if(encInfo->image_capacity >= req_img_cpty)
	return e_success;
    else
	return e_failure;
}

uint EncodeInfo::get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    uint size = ftell(fptr);
    return size;
}

Status EncodeInfo::copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek(fptr_src_image,0,SEEK_SET);
    fseek(fptr_dest_image,0,SEEK_SET);
    //printf("%d before headr 54by \n",(int)ftell(fptr_src_image));
    char buffer[54];
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);
    //printf("%d after headr 54by \n",(int)ftell(fptr_src_image));
    return e_success;
}
Status EncodeInfo::encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
      //printf("%d magic string\n",(int)sizeof(magic_string));
	encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    //printf("%d after magic string \n",(int)ftell(encInfo->fptr_src_image));

	    return e_success;
}

Status EncodeInfo::encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    for(int i=0;i<size;i++)
    {
	char buffer[8];
	fread(buffer, 8, 1, fptr_src_image);
	encode_byte_to_lsb(data[i],buffer);
	fwrite(buffer, 8, 1, fptr_stego_image);
    }

    return e_success;
}
Status EncodeInfo::encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
	image_buffer[i]=image_buffer[i]&(~1);//clearing lsb
	image_buffer[i]=image_buffer[i]|((data & (1<<i))>>i);//adding one bit to lsb
    }

    return e_success;
}

Status EncodeInfo::encode_secret_file_extn_size(int size,EncodeInfo *encInfo)
{
    encode_secret_file_size(size,encInfo);

    return e_success;
}

Status EncodeInfo::encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn ,strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

Status EncodeInfo::encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    char buffer;
   // printf("%d size\n",file_size);
    //printf("%d current curser\n",(int)ftell(encInfo->fptr_src_image));
    for(int i=0;i<32;i++)
    {
	fread(&buffer,1,1,encInfo->fptr_src_image);
	buffer = buffer & (~1);//clearing lsb
	buffer = buffer | ((file_size & (1<<i))>>i);//adding one bit to lsb
	fwrite(&buffer,1,1,encInfo->fptr_stego_image);
	//printf("%d buffer\n",buffer);
    }

    return e_success;
}
Status EncodeInfo::encode_secret_file_data(EncodeInfo *encInfo)
{
    int size = (int)encInfo->size_secret_file;
    char sec_buffer[size];
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    fread(sec_buffer, size, 1, encInfo->fptr_secret);
    encode_data_to_image(sec_buffer, size, encInfo->fptr_src_image, encInfo->fptr_stego_image);

    return e_success;
}

Status EncodeInfo::copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
    char buffer;
    fread(&buffer,1,1,fptr_src);
    fwrite(&buffer,1,1,fptr_dest);

    return e_success;
}
