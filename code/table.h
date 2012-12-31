#ifndef TABLE_H
#define TABLE_H
#include "string"
using namespace std;
class table
{
	public:
	table(string DatabaseName="test",string TableName="test");	//构造函数
	void read_schema();											//读取模式信息
	int load(const char*filename);								//外部TPH生成的文件
	int select(string table_name,string projection);			//基础选择函数
	int insert(string table_name,string tuples);				//索引
	int index(string table_name);
	private:
	string table_name;
	string database_name;
};


#endif