test_ENOD>C
#include <iostream>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
using namespace std;
int main(int argc,char *argv[])
{

    EncodeInfo encInfo;//object
    DecodeInfo decInfo;

    //printf("%s argv1",argv[1]);

    OperationType option = check_operation_type(argv);

    if(option == e_encode)
    {
	cout<<"You have selected ENCODING";
        if((argc==4) || (argc==5))
	{
	    if(encInfo.read_and_validate_encode_args(argv, &encInfo) == e_success)
	    {
		cout<<"Read and validaion done, proceeding for encoding";
		if(encInfo.do_encoding(&encInfo) == e_success)
			cout<<".....ENCODING done.....\n";
	    }
	}
	else
	{
	    cout<<"Provide correct number of arguments for encoding\n";
	   // return 0;
	}
    }
    else if(option == e_decode)
    {
	cout<<"You have selected DCODING\n";
	if((argc==3) || (argc == 4))
	{
	    if(decInfo.read_and_validate_decode_args(argv,&decInfo) == e_success)
	    {
		cout<<"Read and validation done, proceeding for decoding\n";
		if(decInfo.do_decoding(&decInfo) == e_success)
		    cout<<".....DECODING done.....\n";
	    }
	}
	else
	    cout<<"Provide the correct number of arguments for decoding";
    }
    else if(option == e_unsupported)
	cout<<"Enter correct argument, -e/-d\n";

    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
	return e_encode;
    else if(strcmp(argv[1],"-d")==0) 
	return e_decode;
    else
	return e_unsupported;
}
