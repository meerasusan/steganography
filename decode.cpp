/*
NAME : MEERA SUSAN TOMMY
DATE :
DISCRIPTION :
SAMPLE INPUT :
SAMPLE OUTPUT :
*/
#include<iostream>
#include<string.h>
#include "types.h"
#include "common.h"
#include "decode.h"
using namespace std;
static int b=1;

Status DecodeInfo::open_files_decoding(DecodeInfo *decInfo)
{

    //source file -- output.bmp//
    decInfo->fptr_src_img = fopen(decInfo->image_fname,"r");
    if(decInfo->fptr_src_img == NULL) //error handling
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->image_fname);

	return e_failure;
    }

    //destinatin file -- output.txt//
    decInfo->fptr_out_txt = fopen(decInfo->out_txt_fname, "w");
    if(decInfo->fptr_out_txt == NULL)//error handling
    {
	perror("fopen");
	fprintf(stderr,"ERROR: Unable to open file %s\n", decInfo->out_txt_fname);

	return e_failure;
    }

    return e_success;
}
Status DecodeInfo::read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strstr(argv[2],".bmp") != NULL)
	decInfo->image_fname = argv[2];
    else
	return e_failure;
    if(argv[3])
    {
	if(strstr(argv[3],".txt") != NULL)
	    decInfo->out_txt_fname = argv[3];
	else if(strstr(argv[3],".sh")!= NULL)
	    decInfo->out_txt_fname = argv[3];
	else if(strstr(argv[3],".pdf") != NULL)
	    decInfo->out_txt_fname = argv[3];
	else if(strstr(argv[3],".c") != NULL)
	    decInfo->out_txt_fname = argv[3];
	else
	    return e_failure;
    }
    else
	decInfo->out_txt_fname = "output.txt";

    return e_success;
}

Status DecodeInfo::do_decoding(DecodeInfo *decInfo)
{
    if(open_files_decoding(decInfo) == e_success)//open
	cout<<"Files opened successfully\n";
    else
	return e_failure;

    if(decode_magic_string(MAGIC_STRING, decInfo) == e_success)//magic string
	cout<<"Magic string decoded\n";
    else
	return e_failure;

    if(decode_secret_file_extn_size(0,decInfo) == e_success)//file extension size
	cout<<"Decoded file extention size.\n";
    else
	return e_failure;

    if(decode_secret_file_extn(decInfo->sec_file_extn, decInfo) == e_success)//file extention
	cout<<"Decoded file extention.\n";
    else
	return e_failure;

	 if(decode_secret_file_data_size(0,decInfo) == e_success)//secret file size
	cout<<"Decoded secret file size.\n";
    else
	return e_failure;

    if(decode_secret_file_data(decInfo) == e_success)//secret file content
	cout<<"Decoded secret file data.\n";
    else
	return e_failure;

    return e_success;
}

Status DecodeInfo::decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    int n = strlen(magic_string);
    char dec_magic_string[n];
    b=0;
    fseek(decInfo->fptr_src_img,54,SEEK_SET);
    decode_img_to_data(dec_magic_string, n , decInfo->fptr_src_img, decInfo->fptr_out_txt);
    strcpy(dec_magic_string+n,"\0");
    //printf("magic dec strng %s\n",dec_magic_string);
    //printf("magic strng %s\n",magic_string);
    //printf("%d currser pos magic string\n",(int)ftell(decInfo->fptr_src_img));	
    if(strcmp(dec_magic_string,magic_string) == 0)
	return e_success;
    else
	return e_failure;
}

Status DecodeInfo::decode_img_to_data(char *data, int size, FILE *fptr_src_img ,FILE *fptr_out_txt)
{
    int I=0;
    for(I;i<size;i++)
    {
	char buffer[8];
	fread(buffer,8,1,fptr_src_img);
	decode_lsb_to_byte( (data + I) , buffer);
	if(b)
	fwrite( (data + I) ,1,1,fptr_out_txt);
	//printf("%s extn",data + I);
    }
    return e_success;
}

Status DecodeInfo::decode_lsb_to_byte(char *data,char *img_buffer)
{
	for(int i=0;i<8;i++)
	{
		if(img_buffer[i] & 1<<0)
		    *data = *data | (1<<i);
		else
		    *data = *data & (~(1<<i));
	}
	return e_success;
}

Status DecodeInfo::decode_secret_file_extn_size(int size, DecodeInfo  *decInfo)
{

    //printf("%d extention size\n",size);
    char buffer;
    //fread(buffer,32,1,decInfo->fptr_src_img);
    //printf("%d currser pos\n",(int)ftell(decInfo->fptr_src_img));
    for(int i=0;i<32;i++)
	{
    		fread(&buffer,1,1,decInfo->fptr_src_img);
		//printf("%d buffer\n",buffer);
		if(buffer & 1<<0)
		    size = size | (1<<i);
		else
		    size = size & (~(1<<i));
	}
    //printf("%d currser pos after ext size\n",(int)ftell(decInfo->fptr_src_img));

 if(size == 4)
    {
	decInfo->ext_size = size;
	//printf("%d extention size\n",size);
	//printf("%d extention size\n",decInfo->ext_size);
	return e_success;
    }
    else
	return e_failure;

}

Status DecodeInfo::decode_secret_file_extn(char *sec_file_extn, DecodeInfo *decInfo)
{
    //printf("%d extention size\n", decInfo->ext_size);
    decode_img_to_data(sec_file_extn, decInfo->ext_size, decInfo->fptr_src_img, decInfo->fptr_out_txt);
    //printf("%d currser position\n",(int)ftell(decInfo->fptr_src_img));
    strcpy(sec_file_extn+4,"\0");
    //printf("%s extention\n", sec_file_extn);
    b=1;
    if(strcmp(sec_file_extn,".txt") == 0)
    	return e_success;
    else
	return e_failure;
}

Status DecodeInfo::decode_secret_file_data_size(int file_size, DecodeInfo *decInfo)
{
    char buffer[32];
    //printf("%d currser position after ext\n",(int)ftell(decInfo->fptr_src_img));
    fread(buffer,32,1,decInfo->fptr_src_img);
	
    for(int i=0;i<32;i++)
	{
		if(buffer[i] & 1<<0)//lsb 0 or 1
		    file_size = file_size | (1<<i);//set bit
	}

    //printf("%d file sze\n",file_size);
    //printf("%d currser position after sec file size\n",(int)ftell(decInfo->fptr_src_img));
    //if(file_size == (int)encInfo->size_secret_file)
    //{
    
	decInfo->secfile_size = file_size;//storing in corresponding class element
    	//printf("%d file sze\n",decInfo->secfile_size);
	return e_success;

	 //}
    //else
	//return e_failure;
}

Status DecodeInfo::decode_secret_file_data(DecodeInfo *decInfo)
{
    decode_img_to_data(decInfo->out_sec_data, decInfo->secfile_size, decInfo->fptr_src_img, decInfo->fptr_out_txt);

    return e_success;
}

