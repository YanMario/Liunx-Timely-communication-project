#include <iostream>
#include <string>
#include <string.h>
#include <json/json.h>
#include <sys/socket.h>
#include <mysql/mysql.h>
#include"register.h"
#include"mysql.h"

using namespace std;
extern Mysql Mysql_sever;

void Register::process(int fd,string json)
{
	//解析 json
	//name    pw
	_fd = fd;
	bool success = false;
	Json::Value root;
	Json::Reader read;

	if(-1 == read.parse(json,root))
	{
		cerr<<"json parse fail;"<<endl;
		return;
	}
	char name[20] = {0};
	strcpy(name,root["name"].asString().c_str());
	char pw[20] = {0};
	strcpy(pw,root["pw"].asString().c_str());
	//在数据库中查找name有没有重复
	char cmd[100] = "SELECT * FROM user WHERE name='";
	strcat(cmd,name);
	strcat(cmd,"';");
	mysql_real_query(Mysql_sever._mpcon,cmd,strlen(cmd));
	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);

cout<<"register json"<<json<<endl;
cout<<"register cmd"<<cmd<<endl;	
	if(!(Mysql_sever._mp_row =mysql_fetch_row(Mysql_sever._mp_res) ))//无重复
	{
		success = true;
	}

	if(success)
	{
		//将name pw 加入到数据库的user表
		char cmd2[100] = "INSERT INTO user VALUE('";
		strcat(cmd2,name);
		strcat(cmd2,"','");
		strcat(cmd2,pw);
		strcat(cmd2,"');");

cout<<"cmd2"<<cmd2<<endl;
		if(mysql_real_query(Mysql_sever._mpcon,cmd2,strlen(cmd2)))
		{
			cerr<<"insert 1  fail;"<<endl;
			return;
		}

		_str = "register sucess!";
	}
	else
	{
		_str= "register fail!";
	}
}

void Register::response()
{		
	//发送
	send(_fd,_str.c_str(),strlen(_str.c_str()),0);
}
