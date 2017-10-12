#include "expression.h"
#include "evn.h"

#include  <memory.h>
using namespace std;

expression::expression(int type)
:m_type(type),m_qoute_flag(qoute_flag::NORMAL)
{

}




expression:: ~expression(){
		//std::cout<<"destory expression" <<std::endl;

	};

void expression::set_qoute_flag(int flag){
	this->m_qoute_flag = flag;
}

int expression::get_qoute_flag(){
	return this->m_qoute_flag;
}

double_expression::double_expression(double value)
:expression(expression_type::DOUBLE),m_value(value)
{}

void double_expression::print(){
	if(this->m_qoute_flag == qoute_flag::QOUTED)
		cout<<"`";
	if(this->m_qoute_flag == qoute_flag::UNQOUTE)
		cout<<"~";
	cout<<"double :"<<this->m_value <<"   ";
}


string_expression::string_expression(string value)
:expression(expression_type::STRING),m_value(value)
{}

void string_expression::print(){
	if(this->m_qoute_flag == qoute_flag::QOUTED)
		cout<<"`";
	if(this->m_qoute_flag == qoute_flag::UNQOUTE)
		cout<<"~";
	cout<<" \" "<<this->m_value <<"   \"  ";
}

bool_expression::bool_expression(bool value)
:expression(expression_type::BOOL),m_value(value)
{}

void bool_expression::print(){
	if(this->m_qoute_flag == qoute_flag::QOUTED)
		cout<<"`";
	if(this->m_qoute_flag == qoute_flag::UNQOUTE)
		cout<<"~";
	if(this->m_value)
		cout<<"True";
	else
		cout<<"False";

}



int_expression::int_expression(int value)
:expression(expression_type::INT),m_value(value)
{}

void int_expression::print(){
	if(this->m_qoute_flag == qoute_flag::QOUTED)
		cout<<"`";
	if(this->m_qoute_flag == qoute_flag::UNQOUTE)
		cout<<"~";
	cout<<this->m_value <<"   ";
}

atom_expression::atom_expression(char* var)
:expression(expression_type::ATOM), m_value(var){

}

void atom_expression::print(){
	if(this->m_qoute_flag == qoute_flag::QOUTED)
		cout<<"`";
	if(this->m_qoute_flag == qoute_flag::UNQOUTE)
		cout<<"~";
	cout<<this->m_value <<"   ";
}

list_expression::list_expression()
:expression(expression_type::LIST),
 m_is_ok(false),
 m_is_lambda(false),
 m_proc_name(0),
 m_is_apply(false),
 m_need_eval_num(0),
m_need_arg(0),
m_fix_flag(0),
m_free_var(50),
m_free_var_index(0)
 {
}

void list_expression::print(){
	auto iter =this->m_list.begin();

	if(this->m_qoute_flag == qoute_flag::QOUTED)
		cout<<"`";
	if(this->m_qoute_flag == qoute_flag::UNQOUTE)
		cout<<"~";

	cout<<"(";
	if(this->m_proc_name == proc_int_name ::FUNCALL){
//		cout<<"call  need "<<this->get_need_eval_num() <<" params " ;
	}
	if(this->get_is_lambda()){
		cout<<"LAMBDA"<<"  ";
		iter++;
	}
	weak_ptr<expression> aa;
	while(iter != this->m_list.end()){
		aa= (*iter);
		(*iter)->print();
		iter++;
	}

	cout<<") ";

}

bool list_expression::add_exp(std::shared_ptr<expression>& var){
	try{
		this->m_list.push_back(var);
	}catch(...){
		return false;
	}
	return true;
}

void list_expression::set_m_is_ok(bool is){
	this->m_is_ok=is;
}

bool& list_expression::get_m_is_ok(){
	return m_is_ok;
}

std::shared_ptr<expression> list_expression::get_expression(unsigned int index){
	if(index> m_list.size())
		return NULL;

	auto iter = m_list.begin();
	for(unsigned int j=0;j<index;j++){
		iter++;
	}
	return *iter;
}

unsigned int  list_expression::get_length(){
	return this->m_list.size();
}


void list_expression::add_free(std::string& name){
	int i=0;
	while(i< m_free_var_index){
		if(m_free_var[i] == name)
			return;
		i++;
	}
	m_free_var[i] =name;
	m_free_var_index++;

}

std::vector<std::string>&  list_expression::get_free_var(){
	return this->m_free_var;
}

int& list_expression::get_free_var_length(){
	return this->m_free_var_index;
}



bool list_expression::is_free(std::string& name){
	bool ret = false;

	for(int i =0 ; i< m_free_var_index;i++){
			if(m_free_var[i] == name){
				ret =true;
				break;
			}
	}

	return ret;
}




	void list_expression::add_closure_ref(std::string& name , std::shared_ptr<expression>&exp ){
		auto iter = m_closure_map.find(name);
		if(iter == m_closure_map.end() ){
			m_closure_map[name] = exp;
			cout<<"add ref: "<<name<<" value: ";
			exp->print();
			cout<<endl;
		}
	}



std::map<std::string,std::shared_ptr<expression>  >& list_expression:: get_closure_map(){
	return m_closure_map;
}

//	closure_expression::closure_expression(std::shared_ptr<expression>   body )
//	:expression(expression_type::CLOSURE),
//	m_body(body) {
//
//	}
//	std::map<std::string,std::shared_ptr<expression>  >& closure_expression:: get_closure_map(){
//		return m_closure_map;
//	}
//

//	void closure_expression::print(){
//		cout<<"{ "<<endl;
//		auto iter = this->m_closure_map.begin();
//		while(iter !=  this->m_closure_map.end() ){
//			cout<<iter->first<<" : ";
//			iter->second->print();
//			cout<<endl;
//			iter++;
//		}
//		this->m_body->print();
//		cout<<endl<<"}"<<endl;
//	}



