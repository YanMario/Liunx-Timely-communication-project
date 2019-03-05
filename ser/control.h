#ifndef CONTROL_H
#define CONTROL_H
#include "View.h"
#include <string>
#include <map>
class Control
{
public:
	Control();
	~Control(){}
	void process(int fd,string json);
private:
	map<int,View*> _map;
};
#endif
