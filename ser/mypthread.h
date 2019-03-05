#ifndef MYPTHREAD_H
#define MYPTHREAD_H
#include <pthread.h>
#include <map>
#include <event.h>

using namespace std;
class Pthread
{
public:
	Pthread(int sock_fd);
	~Pthread();  

private:
	int _sock_fd;//sock_pair    (1)
	struct event_base* _base;//libeventg
	map<int,struct event*> _event_map;//存放事件的map表
	pthread_t _pthread;//线程描述符

	friend void sock_pair_1_cb(int fd,short event,void *arg);
	friend void client_cb(int fd,short event,void *arg);
	friend void *pthread_run(void *arg);
};

#endif
