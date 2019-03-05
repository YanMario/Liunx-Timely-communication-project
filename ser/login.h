#ifndef LOGIN
#define LOGIN
#include <string>
#include "View.h"
class Login : public View
{
public:
	void process(int fd,string json);
	void response();
private:
	int _fd;
	string _str;
};

#endif