#include"mysql.h"
#include <iostream>
#include <string>
#include <exception>
#include <mysql/mysql.h>
using namespace std;

Mysql::Mysql()
{
	//初始化
		_mpcon = mysql_init((MYSQL *)0);
		if(NULL == _mpcon)
		{		

		cerr<<"mpcon = NULL"<<endl;
		}
	//连接数据库

		if(!mysql_real_connect(_mpcon,"127.0.0.1","root","123456",NULL,3306,NULL,0))//成功返回0
		{
		cerr<<"mysql connect fail;"<<endl;
	}
	//选择database
	
		if(mysql_select_db(_mpcon,"chat"))
		{
		
		cerr<<"database select fail;"<<endl;
	}
}

Mysql::~Mysql()
{
	if(NULL != _mp_res)
	{
		mysql_free_result(_mp_res);
	}

	//关闭数据库
	mysql_close(_mpcon);	
}


Mysql Mysql_sever;
