#ifndef VIEW_H
#define VIEW_H

#include <string>
using namespace std;
class View
{
	public:
		virtual void process(int fd,string json);
		virtual void response();
	private:
		int _fd;
		string _str;
};
#endif
