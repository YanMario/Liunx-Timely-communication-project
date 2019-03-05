#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<memory.h>
#include<string>
#include<exception>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<json/json.h>
#include<sys/types.h>
#include<event.h>
#include"tcpsever.h"
#include "mypthread.h"

using namespace std;

void listen_cb(int fd,short event,void *arg)
{
	//接受用户链接
	Tcpsever *mythis = (Tcpsever*)arg;
	struct sockaddr_in caddr;
	socklen_t len = sizeof(caddr);
	int cli_fd = accept(fd,(struct sockaddr*)&caddr,&len);

	if(-1 == cli_fd)
	{
		cerr<<"accept fail"<<endl;
		return;
	}
	//else
	//{
	//	cout<<"listen_cb accept success"<<endl;
	//}


	//查找当前监听数量最少的子线程
	int min = 65536;//  存储最小监听数
	int tmpfd = 0;//存储最小监听数对应的fd


	map<int,int>::iterator it = mythis->_pth_work_num.begin();

	//查找当前监听数量最少的子线程
	for( ; it != mythis->_pth_work_num.end() ; ++it )
	{
		if( it->second < min )
		{
			min = it->second;
			tmpfd = it->first;
		}
	}

	//将主线程里的 客户端套接字 通过 socktpair 发给子线程
	char abuff[16] = {0};
	sprintf( abuff,"%d",cli_fd);

	if(  0 > write(tmpfd, abuff, sizeof(abuff)) )
	{
		cout<<"write error"<<endl;
	}
	else
	{
		cout<<"write success"<<endl;
	}

}

void sock_pair_cb(int fd,short event,void *arg)
{
	cout<<"+++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	cout<<"in sock_pair_cb()"<<endl;

	
	Tcpsever *mythis = (Tcpsever *)arg;
	int num = 0;
	char buff[10] = {0};
	if( 0 > read(fd,buff,9))
	{
		cout<<"read error"<<endl;
	}
	else
	{
		cout<<"read success"<<endl;
	}

	num = atoi(buff);


	//更新到map表_pth_work_num  ----->fd

	map<int,int>::iterator it2 = mythis->_pth_work_num.begin();

	for( ; it2!= mythis-> _pth_work_num.end() ; ++it2)
	{
		if( it2->first == fd)
		{
			break;
		}
	}
	it2->second = num;
	cout<<"out sock_pair_cb()"<<endl;
	cout<<"++++++++++++++++++++++++++++++++++++++"<<endl;
	//it2->second= num;
}

Tcpsever::Tcpsever(char *ip,short port,int pth_num)
{
	///创建服务器
	_pth_num = pth_num;

		_listen_fd = socket(AF_INET,SOCK_STREAM,0);
		if(-1 == _listen_fd)
		cout<<"_listen_fd creat error"<<endl;

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);
	cout<<"saddr init success"<<endl;

		if(-1 == bind(_listen_fd,(struct sockaddr*)&saddr,sizeof(saddr)))
		{
			cout<<"bind fail"<<endl;
		}

		if(-1 == listen(_listen_fd,20))
		{
			cout<<"listen fail"<<endl;
		}

	//给libevent申请空间
	_base = event_base_new();

	//创建事件，绑定监听套接子的回调函数(listen_cb)

		struct event* ev =  event_new(_base,_listen_fd,EV_READ|EV_PERSIST,listen_cb,this);
		if(ev != NULL)
		{
			cout<<"ev create success!"<<endl;
		}
		else
		{

		cout<<"event new fail"<<endl;
		}

		//将事件添加到事件列表
		event_add(ev,NULL);

		//循环监听
	//	event_base_dispatch(_base);


        cout<<"tcpsever listen cb already carry out"<<endl;


}

void Tcpsever::run()
{
	cout<<"+++++++++++++++++++++++++++++++++++++++++++++++++++++++++"<<endl;
	cout<<"in run()"<<endl;
	//申请socketpair（函数自查）
	get_sock_pair();

	//创建线程   //规定  int arr[2]  arr[0]<=>主线程占用   arr[1]<=>子线程占用
	get_pthread();

	//为主线程的socktpair创建事件，绑定回调函数（sock_pair_cb）	
	for(int i = 0 ; i< _pth_num ; ++i)//由 子线程的读事件  触发
	{
			struct event* ev =  event_new(_base,_socket_pair[i].arr[0],EV_READ|EV_PERSIST,sock_pair_cb,this);
			if(NULL == ev)
			{
				return ;
			}	
			else
			event_add(ev,NULL);
	}
	cout<<"run() for already carry out"<<endl;
	

		//event_base_dispatch(_base);

		event_base_dispatch(_base);
}

void Tcpsever::get_sock_pair()
{
	for(int i = 0;i < _pth_num;i++ )
	{
		//申请双向管道
		int pair[2];
		if( socketpair(AF_UNIX,SOCK_STREAM,0,pair) == -1  )
		{
			cout<<"socketpair error"<<endl;
			return;
		}

		pipe pi(pair);
		_socket_pair.push_back(pi); //将双向管道加入到_sock_pair.push_back();

		_pth_work_num.insert(make_pair(pi.arr[0],0));
	}
	//cout<<"get_sock_pair() success"<<endl;


}

void Tcpsever::get_pthread()
{
	//开辟线程
	for(int i = 0; i< _pth_num; i++)
	{
		_pthread.push_back(new Pthread(_socket_pair[i].arr[1]));
	}
	//cout<<"get_pthread() success"<<endl;
}
