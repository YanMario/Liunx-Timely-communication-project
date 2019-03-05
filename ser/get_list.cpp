#include <json/json.h>
#include <mysql/mysql.h>
#include <sys/socket.h>
#include <string>
#include <string.h>
#include "get_list.h"
#include "mysql.h"
extern Mysql Mysql_sever;

void Get_list::process(int fd,string json)
{
	_fd = fd;
	Json::Value root;
	Json::Reader read;

	if(-1 == read.parse(json,root))
	{
		cerr<<"json parse fail;"<<endl;
		return;
	}

	char myname[20] = {0};
	strcpy(myname,root["name"].asString().c_str());

	char cmd[100]="SELECT name FROM online WHERE name!='";
	strcat(cmd,myname);
	strcat(cmd,"';");
	char name[1000] = "name:";
	mysql_real_query(Mysql_sever._mpcon,cmd,strlen(cmd));
	Mysql_sever._mp_res = mysql_store_result(Mysql_sever._mpcon);
cout<<cmd<<endl;
	while(Mysql_sever._mp_row=mysql_fetch_row(Mysql_sever._mp_res))
	{
		strcat(name,Mysql_sever._mp_row[0]);
		strcat(name,",");
	}
	_str = name;
cout<<_str<<endl;
}
void Get_list::response()
{		
	//·¢ËÍ
cout<<"getlist send:"<<endl;
	send(_fd,_str.c_str(),strlen(_str.c_str()),0);
}
