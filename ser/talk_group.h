#ifndef TALK_GROUP
#define TALK_GROUP
#include "View.h"
class Talk_group : public View
{
public:
	void process(int fd,string json);
	void response();
private:
	int _fd;
	string _str;
};

#endif