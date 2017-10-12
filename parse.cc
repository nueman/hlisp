#include "parse.h"
#include <assert.h>
using namespace std;





shared_ptr<expression> parse::match_and_create_token(char* token){

	if(std::regex_match(token,regex ( "^-?[1-9]\\d*$|0"))){
		return  shared_ptr<expression> ( dynamic_cast<expression*>( new int_expression(atoi(token)))  );
	}
	if(std::regex_match(token,regex ( "^-[1-9]\\d*\\.\\d*|-0\\.\\d*[1-9]\\d*$|[1-9]\\d*\\.\\d*|-0\\.\\d*[1-9]\\d*"))){
		return  shared_ptr<expression> ( dynamic_cast<expression*>(new double_expression(atof(token)))  );
	}

	if(std::regex_match(token,regex ( "\\\".*\\\""))){
		return  shared_ptr<expression> ( dynamic_cast<expression*>(new string_expression(token))  );
		// 隐式类型转换
	}
	if(std::regex_match(token,regex ( "True|Fales"))){
		    bool value = false;
		    if(string(token)=="True"   )
		    	value=true;
			return  shared_ptr<expression> ( dynamic_cast<expression*>(new bool_expression(value))  );
		}

	if(std::regex_match(token,regex ( "[a-zA-z].*|\\+|\\-|\\*|\\/|\\>|\\<|\\:"))){
				return  shared_ptr<expression> ( dynamic_cast<expression*>(new atom_expression(token))  );
	 }


  return NULL;
}



#define addqoute(x) if(is_qout)\
	{x->set_qoute_flag(qoute_flag::QOUTED);}\
	if(is_unqoute)\
	x->set_qoute_flag(qoute_flag::UNQOUTE);\


shared_ptr<expression>  parse::hread(reader& reader,list_expression* end ){
	char token[50]={};
	int index =0;
	expression* exp=NULL;
	bool is_qout=false;
	bool is_unqoute = false;

	while(1){
		char ch = reader.getch();
		if (ch == '\n' || ch == ' ' || ch == ','){  //语法中,和作用一样空格
			if(index==0)  //如果只是输入 这三格东西跳过，应付（     1    2    ） 这样的情况
				continue;
			shared_ptr<expression> ret = match_and_create_token(token);

			addqoute(ret)


			return ret;
		}
		if(ch == '(' ){
			exp= new list_expression();

			addqoute(exp)

			 shared_ptr<expression> nextexp = NULL;

			while(1){
				nextexp= hread(reader,(static_cast<list_expression*> (exp)));
				if(exp->get_qoute_flag() == qoute_flag::QOUTED && nextexp!=NULL &&nextexp->get_qoute_flag()!= qoute_flag::UNQOUTE )
					nextexp->set_qoute_flag(qoute_flag::QOUTED);

				if(nextexp!=NULL){
					(static_cast<list_expression*> (exp))->add_exp(nextexp );
				}


				if( (static_cast<list_expression*> (exp))->get_m_is_ok() )
					break;
			}

			return shared_ptr<expression> (exp);
		}
		if(ch=='`'){
			is_qout =true;
			continue;
		}
		if(ch=='~'){
			 is_unqoute = true;
			 continue;
		}
		if(ch == ')' ){
			assert(end!=NULL);  //此时一个list完成
			end->set_m_is_ok(true);
			if(index!=0)
				return match_and_create_token(token);
			else
				return NULL;
		}
    	token[index] = ch;
    	index++;
	}
	return NULL;
}







