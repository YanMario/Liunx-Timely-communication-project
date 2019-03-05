#ifndef EXIT
#define EXIT
#include "View.h"
class Exit : public View
{
public:
	void process(int fd,string json);
	void response();
private:
	int _fd;
	string _str;
};

#endif