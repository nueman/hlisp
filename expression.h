#ifndef  __EXPRESSION__
#define __EXPRESSION__

#include <string>
#include <list>
#include <memory>
#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include "tools.hpp"



typedef enum{
	NO,
	INT ,
	DOUBLE,
	STRING,
	ATOM ,
	BOOL,
	LAMBDA  ,
	LIST,
	APPLYABLE,
	CLOSURE,
	CALLCC,
} expression_type;

typedef enum{
	NOT =0,
	IF ,
	EQ,
	DEFINE ,
	ADD,
	SUB  ,
	MUL,
	DIV,
	FUNCALL,
	SETCC,
	EVALCC,
	ID,
	CAR,
	GE,
	NE,
	NUL,
	CONS,
	CDR,
	CONCAT,
	COND,
	CASE,
} proc_int_name;


typedef enum{
	NORMAL,
	QOUTED,
	UNQOUTE,
} qoute_flag;


typedef enum{
	NORMALFIX,
	LAMBDABODY,
	UNFIX,
} fix_flag;



class expression : public std::enable_shared_from_this<expression> {
public:
	expression(int type);
	virtual ~expression();
	void set_qoute_flag(int flag);
	int get_qoute_flag();
	virtual void print() =0;
	virtual void fix( ){}

	int& get_type(){
		return m_type;
	}

protected:
	int m_type;
	int m_qoute_flag;
};


class double_expression : public  expression{
public:
	explicit double_expression(double value);
	void print();
private:
	double m_value;
};

class int_expression : public  expression{
public:
	explicit int_expression(int value);
	void print();
	int& get_value(){
		return m_value;
	}
	~int_expression(){
//		std::cout<<"destoy int: ";
//		this->print();
//		std::cout<<std::endl;
	}

private:
	int m_value;
};

class string_expression : public  expression{
public:
	explicit string_expression(std::string value);
	void print();
private:
	std::string m_value;
};

class bool_expression : public  expression{
public:
	explicit bool_expression(bool value);
	void print();
	bool& get_value(){
		return m_value;
	}

private:
	bool m_value;
};


class atom_expression: public  expression{
public:
	explicit atom_expression( char* var );
	void print();
	std::string& get_value(){
		return this->m_value;
	}
	~atom_expression(){
//		std::cout<<"destoy atom: ";
//		this->print();
//		std::cout<<std::endl;
	}

private:
	std::string m_value;
	int m_atom_type;
};



class list_expression: public  expression{
public:
	list_expression();
	~list_expression(){
//		std::cout<<"destoy list: ";
//		this->print();
//		std::cout<<std::endl;
	}

	void print();
	void set_m_is_ok(bool is);
	bool& get_m_is_ok( );
	bool add_exp(std::shared_ptr<expression>& );
	std::shared_ptr<expression> get_expression(unsigned int  index);
	unsigned int  get_length();
	void fix();

	void add_free(std::string& name);

	bool is_free(std::string& name);
	std::vector<std::string>&  get_free_var();
	int& get_free_var_length();




	void set_is_apply(bool is){
		this->m_is_apply =is;
	}
	bool& get_is_apply(){
	    return this->m_is_apply;
	}

	void set_is_lambda(bool is){
		this->m_type = expression_type::LAMBDA;
		this->m_is_lambda =is;
	}

	void set_proc_name(int name){
		this->m_proc_name =name;
	}

	int& get_proc_name(){
		return this->m_proc_name;
	}

	bool& get_is_lambda(){
		return this->m_is_lambda;
	}



	void set_need_eval_num(int num){
		m_need_eval_num=num;
	}

	int& get_need_eval_num(){
		return m_need_eval_num;
	}

	void set_need_arg(int num){
		this->m_need_arg =num;
	}

	int& get_need_arg(){
		return this->m_need_arg;
	}

	void set_fix_flag(int unfix){
		this->m_fix_flag =unfix;
	}

	int& get_fix_flag(){
		return this->m_fix_flag;
	}

	std::string& get_arg_name(unsigned int x){
		auto arg_list =  dynamic_cast<list_expression*>( this->get_expression(1).get());
		auto atom =  dynamic_cast<atom_expression*> (arg_list->get_expression(x).get());
		return atom->get_value();
	}

	int get_arg_index ( std::string& name){
		int ret=-1;
		auto arg_list =  dynamic_cast<list_expression*>( this->get_expression(1).get());
		int i = 0;
		int len = arg_list->get_length();
		while(i<len){
			if(name  ==  dynamic_cast<atom_expression*>(arg_list->get_expression(i).get())->get_value()  ){
				ret = i;
				break;
			}
			i++;
		}
		return ret;
	}

	void cons_exp(std::shared_ptr<expression> exp){
		m_list.push_front(exp);
	}

	void add_closure_ref(std::string& name , std::shared_ptr<expression>&exp);

	std::map<std::string,std::shared_ptr<expression>  >&  get_closure_map();

	void remove_fistof_list(){
		m_list.pop_front();
	}

	std::list<std::shared_ptr<expression> >& get_list(){
			return m_list;
		}

	void append(std::shared_ptr<expression> exp){
		list_expression* list= dynamic_cast<list_expression*> (exp.get());
		if(!list){
			this->add_exp(exp);
			return ;
		}
		m_list.splice(m_list.end(),list->get_list());

	}


private:



	std::list<std::shared_ptr<expression> > m_list;
	std::map<std::string,  std::shared_ptr<expression>  > m_private_evn;
	bool m_is_ok;            //仅仅用在parse中
	bool m_is_lambda;
	int m_proc_name;
	bool m_is_apply;
	int m_need_eval_num;
	//所有操作 包括基本操作 函数调用之类所需要求值的参数 由fix确认用在eval中
	int m_need_arg;
	// 如果该list是函数，则他需要几个参数，在确定list是lambda情况下 由fix确认
	//call该lambda的list的 m_need_eval_num = m_need_arg
	int m_fix_flag;
	//如果 list是lambda 则参数列表不fix,body中可能存在参数是函数，或是递归的情况
	//因此在 fix_flag == lambda_body 时 未定义变量后面跟东西全部视为funcall
	std::vector<std::string> m_free_var;
	int m_free_var_index;
	//处理闭包
	std::map<std::string,std::shared_ptr<expression>  > m_closure_map;
};








//class closure_expression : public  expression{
//public:
//	closure_expression(std::shared_ptr<expression>   body);
//	void print();
//	void add_closure_ref(std::string& name , std::shared_ptr<expression>&exp);
//	std::shared_ptr<expression> get_closure_var (std::string& name);
//	std::map<std::string,std::shared_ptr<expression>  >& get_closure_map();
//	void merge_closure_map( 	std::map<std::string,std::shared_ptr<expression>  >& pre_map );
//
//
//private:
//	std::shared_ptr<expression> m_body;
//	std::map<std::string,std::shared_ptr<expression>  > m_closure_map;
//
//};


//class lambda_expression : public  expression{
//public:
//	lambda_expression(list_expression*  list);
//	void print();
//private:
//	std::shared_ptr<list_expression> m_arg;
//	std::shared_ptr<expression> m_body;
//};


template <class TCLASS>
TCLASS* instance_of(expression* v){
	return dynamic_cast<TCLASS*>(v);
}

#endif
