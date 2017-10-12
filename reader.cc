#include "reader.h"

reader::reader(const char* name):m_fp(NULL),m_is_read_file(false){
	if(strlen(name)==0)
		return;

	m_fp=fopen(name,"r");
	if(m_fp)
		m_is_read_file = true;
	else
		std::cout<<"can not open file repl:"<<std::endl;
}

reader::~reader(){
	if(m_fp)
		fclose(m_fp);
}

char reader::getch(){
	if(m_is_read_file){
		char ret =fgetc(m_fp);
		if(ret == EOF){
			m_is_read_file =false;
			return '\n';
		}
		return ret;
	}else
		return getchar();
}





