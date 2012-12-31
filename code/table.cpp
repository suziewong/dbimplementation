#include "table.h"
#include "const.h"
#include<iostream>
#include <map>
#include<vector>
#include<fstream>
using namespace std;
/*table的构造函数
 * table(string DatabaseName,string TableName)
 * string DatabaseName 数据库名
 * string TableName     表名
 * 思路：因为一张表只能属于一数据库，所以在创建的时候默认选择数据库
 * 本来是想设计database类作为父类，后来因为设计原因，改为database作为全局变量，因为同一时间只能选择一个数据库，然后对于这个数据库的所有表进行各种操作
 * 默认构造函数test数据库的test表
 */
table::table(string DatabaseName,string TableName)
{
	this->table_name = TableName;
	this->database_name = DatabaseName;
	//cout<<"我是table的构造函数\n";
}
/* 显示模式文件
 * void read_schema()
 * 思路：简单的读取文件
 */
void table::read_schema()
{
	string file= "database/"+this->database_name+"/"+this->table_name+".schema";
	char ch[1024];
	ifstream infile;
	infile.open(file);
	if(infile.is_open())
	{
		//cout<<"show schema "<<file<<endl;
		cout<<"+--------------------------------------------------------+\n";
		cout<<"| Filed	           |     Type         |          Null    |\n";
		cout<<"+--------------------------------------------------------+\n";
		int i=0;
		for(i=0;i<3;i++)
		infile.getline(ch,1022,'#');
		 i =0;
		while(!infile.eof())
		{
			infile.getline(ch,1022,'#');
			cout<<"| ";
			string s = ch;
			if((i+1)%3!=0) // i != 2,5,8  (i+1)%3!=0
			   s.append(17-s.length(),' ');
			
			i++;
			cout<<s;
		}
		cout<<"\n+--------------------------------------------------------+\n";
	}
	else
	{
		string file= "database/"+this->database_name+"/"+this->table_name+".schema";
	    cout<<"\n文件位置："<<file;
		cout<<"\nNo this table\n";
	}
	infile.close();	
}

/*读取TPH文件，并且生成schema和table文件
 * int load(const char *filename)
 * filename  需要载入的文件的文字
 * 例子：load test.txt 载入test.txt文件，然后会生成 test.schema和test.table两个文件
 * 思路：先读取第一列，因为是模式信息，保存至模式schema文件中，剩下的保存到table文件中
 *       这是类似于MySQL中MyIsAm引擎的方式，其实也可以像InnoDB那样，直接将数据文件组织成B+数存到文件中。
 * 拓展：没能更具字段名，自动匹配到类型，因为没法做判断
 */
int table::load(const char *filename)
{
	 char *page=new char[4096];
	 for(int i =0;i<4096;i++)
		 page[i]='0';
	 FILE *fp;
	 //打开需要装配的文件
	 fp=fopen(filename,"r");
	 if(fp==NULL)
	 {
		cout<<"error\n";
		return false;
	 }
	 string file= filename;
	 file = file.substr(0,file.find_first_of("."));
	 string head="#TABLENAME#"+file+"\n";
	 string file2 = "database/"+this->database_name+"/"+file+".schema";
	 const char *name2=file2.c_str();
	 
	 file = "database/"+this->database_name+"/"+file+".table";
	 const char *name=file.c_str();
	
	 //生成装配后的文件名
	 FILE *fp1=fopen(name,"w+b");
	 FILE *fp2=fopen(name2,"w+b");
	 //初始化页中的数据结构
	 PageHeader *page_header=(PageHeader *)page;
	 ////元组数置0
	 page_header->tuple_count=0;
	 ////元组信息位置offset
	 page_header->slot_location=4096;
	 ////将元组指针指向第一个位置
	 char *tuple=page+8;
	 ///设置空闲区大小
	 int free_page_space=4096-8;
	 PageSlot *tuple_info=(PageSlot *)(page+4096);
	 int offset=8;//第一条元组在页内偏移为8
	 ////读取第一行文件装配模式信息
	 fgets(tuple,256,fp);
     int tuplesize=strlen(tuple);
	 ///转换成string来处理比较方便
	 string tuplehead = tuple;
	 string temp;
	 ////下面的while循环处理第一行模式信息的装配
		int position=0;
		int num=0;
		int temp2;
		////利用string的内置函数循环处理
		while((position=tuplehead.find_first_of("|",position))!=string::npos)  
		{  
		if(num==0)
		{
			
			temp=tuplehead.substr(0,position);
			////这里没法从TPH文件中判断字段类型，默认为VARCHAR
			head = head +"#"+temp+"#"+"VARCHAR(";
			temp =int2str(temp.length());
			head = head +temp+")#NOT NULL\n";
			temp2=position;
		}
		else
		{
			temp=tuplehead.substr(temp2+1,position-temp2-1);
			head = head +"#"+temp+"#"+"VARCHAR(";
			temp =int2str(temp.length());
			head = head +temp+")#NOT NULL\n";
			temp2=position;
		}
			position++;
			num++;
			
		}
		
		temp=tuplehead.substr(temp2+1,tuplehead.length()-tuplehead.find_last_of("|")-2);
		head = head +"#"+temp+"#"+"VARCHAR(";
		temp =int2str(temp.length());
		head = head +temp+")#NOT NULL\n";
		////以上转配完毕
		///模式文件装配完毕，写入sechma文件
		const char*tt = head.c_str();
		fwrite(tt,sizeof(char),strlen(tt),fp2);


	 //开始装配文件
	 while(true)
	 {   
		 
		 //该页有足够空间进行元组装配
		 if(free_page_space>=MAX_TUPLE_SIZE )
		 {
			 //文件还没读到末尾
			 if(fgets(tuple,250,fp)!=NULL)
			 {   
				//对元组进行装配
				tuple_info--;
				tuple_info->tuple_size=strlen(tuple)+1;//
				tuple_info->tuple_offset=offset;
				offset+=strlen(tuple)+1;
				free_page_space=free_page_space-strlen(tuple)-1-sizeof(PageSlot);
				tuple+=strlen(tuple)+1;//记录下一条元组的起始地址
				page_header->tuple_count++;
				page_header->slot_location-=sizeof(PageSlot);
			}
			 //已到文件尾退出循环
			else
			{
				fwrite(page,sizeof(char),4096,fp1);
				break;
			}
		 }
		 //没有足够的空间进行元组装配需要缓存页输出并进行清空操作
		 else
		 {   
			 //写出该页，空闲地方没有去填充
			 fwrite(page,sizeof(char),4096,fp1);
			 //恢复页内的数据结构初始状态
			 free_page_space=4096-8;
			 page_header->tuple_count=0;
	         page_header->slot_location=4096;
			 tuple_info=(PageSlot *)(page+4096);
			 offset=8;
			 tuple=page+8;
		 }
	 }
	 //装配结束后test的测试：注意，以下代码是可以省略的，这里仅仅只是做了测试写入是否成功
	 fseek(fp1,4096*444,0);
	 fread(page,sizeof(char),4096,fp1);
     tuple_info=(PageSlot *)(page+4096-sizeof(PageSlot));  
     page_header=(PageHeader *)page;
	 ///输出共load了多少元组。
	 cout<<"元组数："<<page_header->tuple_count<<endl; 
	  for(int i=1;i<=page_header->tuple_count;i++)
	 {
		 tuple_info=(PageSlot *)(page+4096-sizeof(PageSlot)*i);  
		 cout<<page+(tuple_info)->tuple_offset<<endl;
	 }
	 delete []page;
	 page=NULL;
	 fclose(fp);
	 fclose(fp1);
	 fclose(fp2);
	 return true;
	
}
/*选择函数
 * int table::select(string table_name,string projection)
 * string table_name	表名，例如test
 * string projection    投影字段名 例如id,name
 * 思路：基础选择函数，然后在通过几个函数处理抽出的结果再显示，不让函数过分庞大
 * 拓展：选择函数时间有限，待处理，不过应该是通过id，进行二次读取。
 */
int table::select(string table_name,string projection)
{
	///测试使用投影和选择;
	vector<string> touying;
	int all=0;
	////如果是select * from test则设置显示所有字段
	if(projection=="*")
	{
		all=1;
	}
	else
	{
		///这里仅仅做了单个projection的投影，如果改变参数值，可以继续push_back以保证显示自己需要的投影字段
		touying.push_back(projection);
		//touying.push_back("name");
		//cout<<projection<<endl;
		int position=0;
		int num=0;
		int temp2;
		while((position=projection.find_first_of(",",position))!=string::npos)  
		{  
		if(num==0)
		{
			temp2=position;
		}
		else
		{
			
			temp2=position;
		}
			position++;  
			num++;
		}
		
	}
	///关于投影的新方法：1.先判断取什么字段，然后根据长度去取需要的
	///2.不是现在的每个字段都取出来
	//string xuanze="22";  ////只做了一个投影的情况，这里应该设置其他的函数进行选择，时间不够没做了

	//1.读取该表的模式信息。
	table_name ="database/"+this->database_name+"/"+table_name+".table";
	const char * name=table_name.c_str();
	char *page=new char[4096];
	FILE *fp1=fopen(name,"r+b");
	 if(fp1 ==NULL)
	 {
		cout<<"该表不存在";
		return false;
	 }
	 //初始化页中的数据结构
	// char *tuple;
	 PageSlot *tuple_info;
	 fread(page,sizeof(char),4096,fp1);
     tuple_info=(PageSlot *)(page+4096-sizeof(PageSlot));  
     PageHeader * page_header=(PageHeader *)page; 
	 //1.先输出表头
	 //cout<<"+--------------------------------------------------------+\n";
	 string file= "database/"+this->database_name+"/"+this->table_name+".schema";
	char ch[1024];
	ifstream infile;
	infile.open(file);
	int i=0;
	int j=0;
	unsigned int k=0;
	vector<string>s(30);
	vector<int>ziduan_length(30);
	if(infile.is_open())
	{
		for(i=0;i<3;i++)
			infile.getline(ch,1022,'#');
		i =0;
		
		while(!infile.eof())
		{
			infile.getline(ch,1022,'#');	
			s[i]= ch;
			if((i)%3==0)
			{
				s[i].append(17-s[i].length(),' ');
				if(all ==1)
				{
					touying.push_back(s[i]);
				}
			}
			if((i)%3==1)
			{
				ziduan_length[i/3] =ziduan(s[i]);
			}
			i++;
			
		}


		i /=3;
		for(k=0;k<touying.size();k++)
		{
			touying[k]=touying[k].append(17-touying[k].length(),' ');
		}
		
		cout<<"\n+";
		for(j=0;j<i;j++)
		{
			for(k=0;k<touying.size();k++)
			{
				if(s[j*3]==touying[k])
				cout<<"-------------------";
			}
		}
		cout<<"+\n";
		cout<<"| ";
		for(j=0;j<i*3;j+=3)
		{	
			for(k=0;k<touying.size();k++)
			{
				if(s[j]==touying[k])
				{
					cout<<" "<<s[j]<<"|";
				}
			}
		}
		cout<<"\n+";
		for(j=0;j<i;j++)
		{
			for(k=0;k<touying.size();k++)
			{
				if(s[j*3]==touying[k])
				cout<<"-------------------";
		
			}
		}
		cout<<"+\n";
	}
	 //2.输出元组,这里可以考虑投影和选择，这里只考虑的投影，因为选择需要另外的函数进行其他处理
	string tuple;
	string temp;
	string tempo;
	 for(int t=1;t<=page_header->tuple_count;t++)
	 {
		tuple_info=(PageSlot *)(page+4096-sizeof(PageSlot)*t); 
		tuple=page+(tuple_info)->tuple_offset;

		for(k=0;k<touying.size();k++)
		{
			if(s[0]==touying[k])
			{
				temp = tuple.substr(0,ziduan_length[0]); ///0.5
					cout<<"| "<<temp.append(17-temp.length(),' ');
			}
		}
		int templ=1;
		//cout<<i<<endl;
		for(int zdl=0;zdl<i-1;zdl++)
		{
			for(k=0;k<touying.size();k++)
			{
				if(s[(zdl+1)*3]==touying[k])
				{
				
					templ+=ziduan_length[zdl];
					temp = tuple.substr(templ,ziduan_length[zdl+1]);  /// 6,8 
					
					tempo=temp;

					//if(temp==xuanze)
				//	{
						cout<<"| "<<temp.append(17-temp.length(),' ');
					//}	
				}
			}
		}

		//if(tempo==xuanze)
		//{
			cout<<" |";
		cout<<"\n+";
		for(int j=0;j<i;j++)
		{
			for(k=0;k<touying.size();k++)
			{
				if(s[j*3]==touying[k])
					cout<<"-------------------";
		
			}
		}
		cout<<"+\n";
	//	}
	 }
	 //3.输出其他
	 cout<<"共返回元组数："<<page_header->tuple_count<<endl; 
	 return true;
}
/*
 *	插入处理
 *  int insert(string table_name,string tuples)
 *  table_name    表的名字
 *  tuples        插入的元组名
 *  思路：读取文件知道在哪里写入，获取offset 然后在写回文件，中间需要2次I/O
 *  拓展：如果同时插入多个元组的话，需要多次调用这函数，也就是对这个函数进行2次封装即可。
 *   可以使用multimap实现多次数据的组织
 *  疑惑: 没有找到可以同时文件读写的模式，a的模式是追加，fseek没有效果？！
 */
int table::insert(string table_name,string tuples)
{
	//0.处理数据
	string table_name2 ="database/"+this->database_name+"/"+table_name+".table";
	 table_name ="database/"+this->database_name+"/"+table_name+".table";
	const char * name=table_name.c_str();
	char *page=new char[4096];
	FILE *fp1=fopen(name,"r+b");
	
	 if(fp1 ==NULL)
	 {
		cout<<"该表不存在";
		return false;
	 }
	 //初始化页中的数据结构
	 char *tuple;
	 PageSlot *tuple_info;
	 fread(page,sizeof(char),4096,fp1);
     tuple_info=(PageSlot *)(page+4096-sizeof(PageSlot));  
     PageHeader * page_header=(PageHeader *)page;	 
	 int freeoffset=8;
	 for(int i=1;i<=page_header->tuple_count;i++)
	 {
		 tuple_info=(PageSlot *)(page+4096-sizeof(PageSlot)*i);  
		  freeoffset+=(tuple_info)->tuple_size;

	 }
	 fclose(fp1);
	 
	const char * name2=table_name2.c_str();
	 FILE *fp2=fopen(name2,"w+b");
	 tuple=page+freeoffset;
	strcpy(tuple,tuples.c_str());
	int offset =tuple_info->tuple_offset;
	tuple_info--;
	tuple_info->tuple_size=tuples.length()+1;
	tuple_info->tuple_offset=offset+tuple_info->tuple_size;
	//tuple+=strlen(tuple)+1;//记录下一条元组的起始地址
	page_header->tuple_count++;
	///重新写回到这个文件里面
	fwrite(page,sizeof(char),4096,fp2);
	delete []page;
	fclose(fp2);

return true;
}

/* 索引函数
 * int index(string table_name)
 * string table_name 表的名字
 * 思路：索引上存储了元组的物理直接位置，索引保证4KB，也就是刚好一次I/O，然后再去磁盘上读取，再一次I/0，一共2次I/O
 * 拓展：应该使用B+树组织索引，但是时间有限，没有实现，只有简单的顺序索引。
 */
int table::index(string table_name)
{
	string tablename ="database/"+this->database_name+"/"+table_name+".table";
	string indexname ="database/"+this->database_name+"/"+table_name+".index";
	const char * iname=indexname.c_str();
	const char * tname=tablename.c_str();
	char *page=new char[4096];
	char *page2=new char[4096];
	FILE *fp1=fopen(iname,"r+b");
	FILE *fp2=fopen(tname,"r+b");
	//这里共发生了2次I/O
	 if(fp1 ==NULL)
	 {
		cout<<"该表不存在";
		return false;
	 }
	 //初始化页中的数据结构
	 string tuple;
	 PageSlot *tuple_info;
	 fread(page,sizeof(char),4096,fp1);
	 fread(page2,sizeof(char),4096,fp2);
     tuple_info=(PageSlot *)(page+4096-sizeof(PageSlot));  
     PageHeader * page_header=(PageHeader *)page;	 
	 int offset=8;
	 cout<<"+------------+"<<endl;
	 cout<<"|id   |name  |"<<endl;
	 cout<<"|";
	 for(int i=1;i<=page_header->tuple_count;i++)
	 {
		 tuple_info=(PageSlot *)(page+4096-sizeof(PageSlot)*i);
		  string tuple=page+(tuple_info)->tuple_offset;
		  char buf[10];
			strcpy(buf,tuple.substr(tuple.find("|")+1,tuple.length()-tuple.find("|")).c_str());
			sscanf(buf,"%d",&offset);
		  ////这里可以如果数据量大，应该使用B+数来组织内存中的索引。
		 cout<<page2+offset<<"|";
	 }
	 cout<<"\n+-----------+"<<endl;
	 ///回收内存
	 delete []page;
	 delete []page2;
	 ///清楚悬挂指针
	 page=NULL;
	 page2=NULL;
	 ///关闭文件
	 fclose(fp1);
	 fclose(fp2);
	return 0;
}
