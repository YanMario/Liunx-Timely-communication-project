#ifndef GET_LIST
#define GET_LIST
#include <string>
#include "View.h"
class Get_list : public View
{
public:
	void process(int fd,string json);
	void response();
private:
	int _fd;
	string _str;
};

#endif