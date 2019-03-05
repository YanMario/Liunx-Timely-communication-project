#include<iostream>
#include<stdio.h>
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
	Tcpsever * tmp = (Tcpsever *)arg;
	struct sockaddr_in caddr;
	socklen_t len = sizeof(caddr);
	int cli_fd = accept(fd,(struct sockaddr*)&caddr,&len);
	if(-1 == cli_fd)
	{
		cerr<<"accept fail"<<endl;
		return;
	}
	
	
	//查找当前监听数量最少的子线程
	int min = 65536;     //存储最小监听数
	int tmpfd = 0;       //存储最小监听数对应的fd
	
	
	map<int ,int>::iterator it =tmp._pth_work_num.begin();
	
	//查找当前监听数量最少的子线程//        ************************sort*************************8
	for( ; it !=tmp. _pth_work_num.end() ; ++it )
	{
		if( it->second < min )
		{
			min = it->second;
			tmpfd = it->first;
		}
	}
	
	//将主线程里的 客户端套接字 通过 socktpair 发给子线程
	char abuff[16] = {0};
	sprintf(  abuff,"%d",cli_fd);
	
	if(  0 > write(tmpfd, abuff, sizeof(abuff)) )
	{
		cout<<"write error"<<endl;
	}
	
}

void sock_pair_cb(int fd,short event,void *arg)
{
	int num;   //num为了接收读到的管道内容
	//读取管道内容
	char buff[10] = {0};
	vector<pipe>::iterator it = _socket_pair.begin();
	
	for( ; it != _socket_pair.end() ; ++it)
	{
		if(fd == it->arr[0])
		{
			num = it->arr[1];
			break;
		}
	}
	
	if( 0 > read(num,buff,9) )
	{
		cout<<"read error"<<endl;
	}
	
	
	//更新到map表_pth_work_num----->fd
	
	map<int,int>::iterator it = _pth_work_num.find(fd);
	if(it == _pth_work_num.end() )
	{
		cout<<"fd send error"<<endl;
	}
	it->second = num;
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
            return 0;
        }

		pipe pi(pair[2]);
        _sock_pair.push_back(pi); //将双向管道加入到_sock_pair.push_back();
		
        _pth_work_num.push_back(makepair(pi[0],0));
	}


}

void Tcpsever::get_pthread()
{
	//开辟线程
	for(int i = 0; i< _pth_num; i++)
	{
		_pthread.push_buch(new Pthread(_sock_pair[i][1]));
	}
}

Tcpsever::Tcpsever(char *ip,short port,int pth_num)
{
	///创建服务器
	_pth_num = pth_num;
	
	int _listen_fd = socket(AF_INET,SOCK_STREAM,0);
	if(-1 == _listen_fd)
	{
		cout<<"_listen_fd creat error"<<endl;
		return 0;
	}
	
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip);
	
	if(-1 == bind(_listen_fd,(struct sockaddr*)&saddr,sizeof(saddr)))
	{
		cout<<"bind fail"<<endl;
		return 0;
	}	

	if(-1 == listen(_listen_fd,20))
	{
		cout<<"listen fail"<<endl;
		return 0;
	}
	
	//给libevent申请空间
	_base = event_base_new();
	
	//创建事件，绑定监听套接子的回调函数(listen_cb)
	struct event* ev =  event_new(_base,_listen_fd,EV_READ|EV_PERSIST,listen_cb,_base);
	
	if(NULL == ev)
	{
		cout<<"event new fail"<<endl;
		return 0;
	}
	
	//将事件添加到事件列表
	event_add(ev,NULL);
	
	//循环监听
	event_base_dispatch(_base);

}

void Tcpsever::run()
{
	//申请socketpair（函数自查）
	get_socet_pair();

	//创建线程   //规定  int arr[2]  arr[0]<=>主线程占用   arr[1]<=>子线程占用
	get_pthread();


	//为主线程的socktpair创建事件，绑定回调函数（sock_pair_cb）	
	_base = event_base_new();
	
	for(int i = 0 ; i< _pth_num ; ++i)//由 子线程的读事件  触发
	{
		struct event* ev =  event_new(_base,_socket_pair[i][1],EV_READ|EV_PERSIST,sock_pair_cb,_base);
		if(NULL == ev)
		{
			cout<<"event new fail"<<endl;
			return 0;
		}
		event_add(ev,NULL);
		//event_base_dispatch(_base);
	}
	event_base_dispatch(_base);
	
	
}

