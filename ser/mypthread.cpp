#include <iostream>
#include <event.h>
#include <pthread.h>
#include <cstdlib>
#include <string.h>
#include "mypthread.h"
#include "control.h"

using namespace std;
Control control_sever;
extern map<int,struct event*> aaa;

void client_cb(int fd,short event,void *arg)
{
	//recv  ->buff
	char buff[128] = {0};
	Pthread *mythis = (Pthread*)arg;
	//struct event_base* _base = (struct event_base*)arg;
	if(-1==recv(fd,buff,127,0))
	{
		cout<<"recv error"<<endl;
		return;
	}
	else
	{
		cout<<"last recv success"<<endl;
	}

	//将buff发给control

	control_sever.process(fd,buff);
}

void sock_pair_1_cb(int fd,short event,void *arg)
{
	cout<<"++++++++++++++++++++++++++++++++++++++"<<endl;
	cout<<"in  sock_paor_1_cb"<<endl;
	//recv -> clien_fd
	Pthread *mythis = (Pthread *)arg;
	//struct event_base* _base = (struct event_base*)arg;
	char buff[20] = {0};

	if( 0 > read(mythis->_sock_fd , buff ,19))
	{
		cout<<"recv error"<<endl;
	}

	int client_fd = atoi(buff);
	//将client_fd加入到libevent     client_cb()
	struct event* listen_event = event_new(mythis->_base,client_fd,EV_READ|EV_PERSIST,client_cb,arg);
	event_add(listen_event,NULL);
	//mythis->_event_map(aaa);
	mythis->_event_map.insert(make_pair(client_fd,listen_event));
	aaa.insert(make_pair(client_fd,listen_event));
	//给主线程回复当前监听的客户端数量
	int num = mythis->_event_map.size();

	char abuff[16] = {0};
	sprintf(abuff,"%d",num);
	if( 0> write(mythis->_sock_fd , abuff ,sizeof(abuff)))
	{
		cout<<"write error"<<endl;
	}
	//_event_map.insert(fd);
	cout<<"out sock_pair_1_cb()"<<endl;
	cout<<"++++++++++++++++++++++++++++++++++++++++"<<endl;

}

void *pthread_run(void *arg)
{
	Pthread *mythis = (Pthread *)arg;
	cout<<"pthread recv socketpair[1] ="<<mythis->_sock_fd<<endl;
	//_base = (struct event_base*)arg;
	//将sock_pair_1加入到libevent  sock_pair_1_cb()
	struct event* sock_event = event_new(mythis->_base,mythis->_sock_fd,EV_READ|EV_PERSIST,sock_pair_1_cb,arg);
	
	event_add(sock_event,NULL);
	
	event_base_dispatch( mythis->_base);


}


Pthread::Pthread(int sock_fd)
{
	_sock_fd = sock_fd;
	_base = event_base_new();
	//启动线程
	

	if(0 != pthread_create(&_pthread,NULL,pthread_run,this))
	{
		cerr<<"pthread create fail"<<endl;
	}
	
}

