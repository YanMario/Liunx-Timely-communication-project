#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <json/json.h>
#include <mysql/mysql.h>
#include <event.h>
#include <sys/socket.h>
#include"exit.h"
#include"mysql.h"

using namespace std;
extern Mysql Mysql_sever;
extern map<int,struct event*> aaa;

void Exit::process(int fd,string json)
{   
	_fd = fd;

	bool success = true;
	Json::Value root;
	Json::Reader read;

	if(-1 == read.parse(json,root))
	{
		cerr<<"json parse fail;"<<endl;
		return;
	}
	char name[20] = {0};
	strcpy(name,root["name"].asString().c_str());

	char cmd[100] = "DELETE  FROM online WHERE name='";
	strcat(cmd,name);
	strcat(cmd,"';");
cout<<"exit json"<<json<<endl;
cout<<"exit cmd"<<cmd<<endl;
	mysql_real_query(Mysql_sever._mpcon,cmd,strlen(cmd));
	if(mysql_real_query(Mysql_sever._mpcon,cmd,strlen(cmd)))
	{
		cout<<"exit fail"<<endl;
		return ;
	}
	_str = name;
	_str.append(" exit~");
	cout<<"one client out"<<endl;
	map<int ,struct event*>::iterator it = aaa.begin();
	for( ; it!= aaa.end() ; ++it)
	{
		if( _fd == it->first)
		{
			event_free(it->second);
			break;
		}
	}	
}

void Exit::response()
{		
	//·¢ËÍ
	send(_fd,_str.c_str(),strlen(_str.c_str()),0);
}
