#ifndef TALK_ONE
#define TALK_ONE
#include "View.h"
class Talk_one : public View
{
public:
	void process(int fd,string json);
	void response();
private:
	int _fd;
	string _str;
};

#endif