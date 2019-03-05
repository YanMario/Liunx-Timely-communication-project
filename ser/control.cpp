#include <string>
#include <iostream>
#include <string>
#include <string.h>
#include <cstdlib>
#include <json/json.h>
#include "control.h"
#include "public.h"
#include "View.h"
#include "register.h"
#include "login.h"
#include "get_list.h"
#include "talk_one.h"
#include "talk_group.h"
#include "exit.h" 
using namespace std;
Control contral_sever;

Control::Control()
{
	_map.insert(make_pair(MSG_TYPE_REGISTER,new Register));
	_map.insert(make_pair(MSG_TYPE_LOGIN,new Login));
	_map.insert(make_pair(MSG_TYPE_GET_LIST,new Get_list));
	_map.insert(make_pair(MSG_TYPE_TALK_TO_ONE,new Talk_one));
	_map.insert(make_pair(MSG_TYPE_TALK_TO_GROUP,new Talk_group));
	_map.insert(make_pair(MSG_TYPE_EXIT,new Exit));
}

void Control::process(int fd,string json)
{
	cout<<"control_sever.process(fd,buff)"<<endl;

	//解析json，获取消息类型
	Json::Value root;
	Json::Reader read;

	if(-1 == read.parse(json,root))
	{
		cout<<"json parse fail;"<<endl;
		return;
	}
	else
	cout<<" scan success"<<endl;
	
	map<int ,View*>::iterator it =_map.find(root["reason_type"].asInt());
	if( it ==  _map.end())
	{
		cout<<"not find"<<endl;
	}



	//cout<<root["reason_type"].asString()<<endl;
	//根据消息类型在map中查找
/*
	map<int,View*>::iterator it =_map.find(atoi(root["reason_type"].asString().c_str()));
	//判断是否找到
	//
	if(it == _map.end())
	{
		cerr<<"not find!"<<endl;
		return;
	}
	else
	{
		cout<<"message type can find "<<endl;
	}*/
/*
	map<int ,View*>::iterator it = _map.begin();
	for( ; it!=_map.end()  ; ++it)
	{
		if( it->first == atoi(root["reason_type"].asString().c_str()) )
		{
			break;
		}
	}
	cout<< it->first <<endl;
*/
	it->second->process(fd,json);
	it->second->response();
	
}




