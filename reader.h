#ifndef ___READER___
#define ___READER___

#include <iostream>
#include <string.h>
#include<stdio.h>
#include<stdlib.h>

class reader{
public:
	reader(const char* name="");
	char getch();
	 ~reader();
private :
	FILE *m_fp;
	bool m_is_read_file;
};





#endif
