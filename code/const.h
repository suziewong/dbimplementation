
#include<string>
#include <cstdlib>
#include<iostream>
#define MAX_TUPLE_SIZE  258//其中8个字节结构体PageSlot所占用的空间
using namespace std;
typedef struct PageHeader{
	int tuple_count;
	int slot_location;
} PageHeader;

typedef struct PageSlot{
	int tuple_size;
	int tuple_offset;
}PageSlot;

int ziduan(string ziduan)
{
	int length=0;
	if(ziduan == "INTERGER")
	{
		length=4;
		return length;
	}
	
	
	if(ziduan.substr(0,7)=="VARCHAR")
	{
		ziduan = ziduan.substr(8,ziduan.find_last_of(")")-8);
		length =atoi(ziduan.c_str());
		return length;
	}
	
}
////int 转换成 string
string int2str( int num)
{
    if (num == 0 )
       return " 0 " ;                                                                                                                                      
	 string str = "";
    int num_ = num > 0 ? num : - 1 * num;
    while (num_)
    {
       str = ( char )(num_ % 10 + 48 ) + str;
       num_ /= 10 ;
    }
    if (num < 0 )
       str = " - " + str;
    return str;
}

