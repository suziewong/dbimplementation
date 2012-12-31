
#include<iostream>
#include<string>
#include<vector>
#include "stdio.h"
#include "table.h"
#include "func.h"
using namespace std;

//定义全局变量
#define HW "ZjutSQL>";
string DATABASE_NAME = "test";
string DATABASE_LOCATION = "database/";
int main()
{
	string sql;
	////文字输出
	cout<<"Welcome to ZjutSQL\n";
	cout<<"Server Version 0.1\n";
	cout<<"\n";
	cout<<"Try 'help;' for help. Try 'clear;' to clear the current input statement.";
	cout<<"\n";
	cout <<HW;
	///定义SQL语句，以便解析
	vector<string> str(9);
	str[0]="exit;";
	str[1]="clear;";
	str[2]="show database;";
	str[3]="desc";
	str[4]="load";
	str[5]="insert";
	str[6]="select";
	str[7]="help;";
	str[8]="index";
	///利用while循环控制消息队列
	while(1)
	{
		////获取一行SQL语句
		getline(cin,sql);
		while(sql=="")
		{
			getline(cin,sql);
		}
		///判断是否以';'结束
		if((sql.length()-1) != sql.find(";"))
		{
			cout<<"'"<<sql<<"' is wrong,";
			//cout<<(sql.length()-1);
			cout<<"Please end with ';'\n";
			
		}
		else
		{

		int ix = 0;
		for(ix =0; ix != str.size(); ++ix)
		{
			if(sql.substr(0,sql.find_first_of(" ")) == str[ix])
			{
				break;
			}
		}
		///选择数据库
		if(sql.substr(0,3) == "use" && ((sql.length()-1) == sql.find(";")))
		{
			DATABASE_NAME = sql.substr(4,sql.length()-5);
			DATABASE_LOCATION = "database/"+DATABASE_NAME;
			cout <<"switch to database "<<DATABASE_NAME<<"\n";
		}
		switch (ix)
		{	
			case 0:
			{
				 cout<<"bye";
				 exit(0);
			}
			case 1:
			{	
				////清屏
				system("cls");
				break;
			}
			case  2:
			{	
				cout<<"读取有几个数据库\n";
				break;
			}
			case 3:
			{
				cout<<"显示表结构\n";
				string table_name = sql.substr(5,sql.length()-6);
				
				cout<<table_name<<endl;
				table table(DATABASE_NAME,table_name);
				table.read_schema();
				break;
			}
			case 4:
			{
				
				///导入文件
			    string s= sql.substr(5,sql.length()-6);
				const char * filename=s.c_str();
				cout<<"导入文件: '"<<filename<<"'\n";
				table table(DATABASE_NAME,"aa");
				table.load(filename);	
				break;
				
			}
			case 5:
			{
				///插入函数
				string table_name = sql.substr(sql.find("into ")+5,sql.find("values")-sql.find("into ")-5);
				table_name=trim(table_name);
				string tuples=sql.substr(sql.find("(\"")+2,sql.find("\")")-sql.find("(")-2);
				table table(DATABASE_NAME,table_name);
				table.insert("test",tuples);
				break;	
			}
			case 6:
			{
				///选择函数
				string table_name = sql.substr(sql.find_last_of(" ")+1,sql.length());
				table_name = table_name.substr(0,table_name.length()-1);

				int fi=sql.find_first_of(" ");
				int se=sql.find(" ",7);
				table table(DATABASE_NAME,table_name);
				table.select(table_name,sql.substr(7,se-fi-1));
				break;
			}
			case 7:
			{
				//帮助函数
				print_help();
				cout<<endl;
				break;
			}
			case 8:
			{
				///索引函数
				string table_name = sql.substr(sql.find_last_of(" ")+1,sql.length());
				table_name = table_name.substr(0,table_name.length()-1);
				//cout<<table_name<<"sadas";
				table table(DATABASE_NAME,table_name);
				table.index(table_name);
				break;
			}
		}

		}
		cout<<HW;
	}
	
	return 0;
}

