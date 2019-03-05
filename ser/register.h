#ifndef REGISTER
#define REGISTER
#include"View.h"
class Register : public View
{
public:
	void process(int fd,string json);
	void response();
private:
	int _fd;
	string _str;
};

#endif
