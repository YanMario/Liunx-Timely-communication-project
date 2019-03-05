#include<iostream>
#include<stdlib.h>
#include"tcpsever.h"

using namespace std;

map<int,struct event*> aaa;
int main(int argc,char *argv[])
{
	if(argc < 4)
	{
		cout<<"error"<<endl;
		return 0;
	}

	//分离参数
	char *p1 = argv[1];
	char *p2 = argv[2];
	char *p3 = argv[3];
	
	char *ip = p1;
	short port = atoi(p2);
	int pth_num = atoi(p3);
	
	int _pth_num = pth_num ;    //设置子线程最多的个数
	
	Tcpsever sever(ip,port,_pth_num);
	
	cout<<" Tcpsever::Tcpsever()" <<endl;	

	sever.run();
}


