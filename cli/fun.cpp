#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <assert.h>
#include <memory.h>
#include <string>
#include <pthread.h>
#include <errno.h>
#include <json/json.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include"fun.h"
#include"public.h"
using namespace std;


string myname;
void *pth_run(void *arg)
{
	int c = (int) arg;
	char buff[1024] = {0};
	int n = recv(c,buff,1023,0);
	while(n != 0)
	{
		cout<<"\033[36m"<<buff<<"\n\033[0m"<<endl;
		memset(buff,0,1024);
		n = recv(c,buff,1023,0);
	}
}


void do_register(int fd)
{
	string name;
	string pw;
	//让用户输入name   pw
	cout<<"-->  your name:  "<<endl;
	cin>>name;
	cout<<"-->  your password:  "<<endl; 
	cin>>pw;
	//JSON 打包数据
	Json::Value val;
	val["reason_type"] = MSG_TYPE_REGISTER;
	val["name"] = name;
	val["pw"] = pw;

	//发送到服务器
	if(-1 == send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr<<"//send reason fail;errno:"<<errno<<endl;
		return;
	}

	//接受服务器消息（判断success/error）
	char buff[256] = {0};
	if(0 < recv(fd,buff,255,0))
	{
		if(strncmp(buff,"register sucess!",16) == 0)
		{
			cout<<"-----register sucess!-----"<<endl;
			return;
		}		
	}

	cerr<<"//register fail;"<<endl;


}

void do_login(int fd)
{
	bool success = false;
	string name;
	string pw;
	

	//让用户输入name   pw
	cout<<"-->  your name:  "<<endl;
	cin>>name;
	cout<<"-->  your password:  "<<endl; 
	cin>>pw;
	//Json打包数据
	Json::Value val;
	val["reason_type"] = MSG_TYPE_LOGIN;
	val["name"] = name;
	val["pw"] = pw;
	//发送
	if(-1 == send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
		return;
	}

	//接受服务器回应（判断success/error）
	char buff[256] = {0};
	if(0 < recv(fd,buff,255,0))
	{
		if(strncmp(buff,"login sucess!",13) == 0)
		{
			cout<<"----- \033[35m"<<name<<"\033[0m login sucess!-----"<<endl;
			success = true;
		}
		else cout<<"//login fail!"<<endl;

	}

	if(success)
	{
		//启动一个线程（专门接受数据和打印数据）
		myname = name;
		while(1)
		{
			cout<<"\033[31m1. get list  2. talk to one  3.talk to group  4.exit\n\033[0m"<<endl;
					cout<<"--------------------------------------------------"<<endl;
			pthread_t id;
			pthread_create(&id,NULL,pth_run,(void *)fd);
			int c;
			cin>>c;
			switch(c)
			{
			case 1:
				do_get_list(fd);			
				break;
			case 2:
				
				do_talk_to_one(fd);			
				break;
			case 3:
				do_talk_to_group(fd);			
				break;
			case 4:
				do_exit(fd);		
				break;
			default:
				break;
			}
		}
	}
}

void do_get_list(int fd)
{
	Json::Value val;
	val["reason_type"] = MSG_TYPE_GET_LIST;
	val["name"] = myname;
	//发送
	if(-1 == send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
		return;
	}
	pthread_t id;
	pthread_create(&id,NULL,pth_run,(void *)fd);
}


void do_talk_to_one(int fd)
{
	string hername;
	cout<<"-->  choose a friend:";
	cin>>hername;
	
	char sendbuf[256] = {0};
	cout<<"-->  input:";
	cin.ignore();
	cin.getline(sendbuf,256);

	Json::Value val;
	val["reason_type"] = MSG_TYPE_TALK_TO_ONE;
	val["name"] = myname;
	val["hername"] = hername;
	val["message"] = sendbuf;
	if(-1 == send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
		return;
	}

}

void do_talk_to_group(int fd)
{
	char hername[128] = {0};
	char sendbuf[256] = {0};
	cin.ignore();
	cout<<"-->  choose  friends use ';' to divide:";
	cin.getline(hername,128);

	cout<<"-->  input:";
	cin.getline(sendbuf,256);
	Json::Value val;
	val["reason_type"] = MSG_TYPE_TALK_TO_GROUP;
	val["name"] = myname;
	val["hername"] = hername;
	val["message"] = sendbuf;

	if(-1 == send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
		return;
	}

}

void do_exit(int fd)
{
	Json::Value val;
	val["reason_type"] = MSG_TYPE_EXIT;
	val["name"] = myname;
	//发送
	if(-1 == send(fd,val.toStyledString().c_str(),strlen(val.toStyledString().c_str()),0))
	{
		cerr<<"send reason fail;errno:"<<errno<<endl;
		return;
	}
	exit(0);
	close(fd);
}
