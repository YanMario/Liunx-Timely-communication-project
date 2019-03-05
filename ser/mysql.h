#ifndef MYSQL_H
#define MYSQL_H
#include<mysql/mysql.h>

class Mysql
{
public:
	Mysql();
	~Mysql();

	MYSQL* _mpcon;//数据库
	MYSQL_RES* _mp_res;//返回值
	MYSQL_ROW _mp_row;//每一行	
};
#endif
