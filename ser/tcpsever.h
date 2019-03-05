#ifndef TCPSEVER_H
#define TCPSEVER_H
#include <vector>
#include <map>
#include <event.h>
#include "mypthread.h"

using namespace std;
class Tcpsever
{
	public:
		Tcpsever(char *ip,short port,int pth_num);
		~Tcpsever(){}
		void run();
		
		class pipe
		{
			public:
			pipe(int err[2])
			{
				arr[0] = err[0];
				arr[1] = err[1];
			}
			int arr[2];
		};
		
	private:
		int _listen_fd;//监听套接字
		int _pth_num;//启动的线程的个数
		struct event_base *_base;//libevent

		vector<pipe> _socket_pair;
		vector<Pthread*> _pthread;//pthread vector
		map<int,int> _pth_work_num;//用于和子线程交流的fd+对应子线程监听的个数

		void get_sock_pair();
		void get_pthread();


		friend void listen_cb(int fd,short event,void* arg);
		friend void sock_pair_cb(int fd,short event,void* arg);
};

#endif 
