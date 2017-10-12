#include "primary_proc.h"

using namespace std;
primary_proc::primary_proc(){

	auto add_fun =[]( call_recover* recover ,std::vector<std::shared_ptr<expression>  >&  stack )->std::shared_ptr<expression>{
		int sum =0;
		int begin = recover->begin_index;
		int end = recover->eval_ok;
		int_expression * tmp;
		int realend = begin+end;
		while(begin<realend){
			tmp = instance_of<int_expression> (stack[begin].get());
			sum+=tmp->get_value();
			begin++;
		}

		tmp = new int_expression(sum);
		return shared_ptr<expression>( static_cast<expression*> (tmp) );
	};

	auto sub_fun =[]( call_recover* recover , std::vector<std::shared_ptr<expression>  >& stack )->std::shared_ptr<expression>{
		int sum =0;
		int begin = recover->begin_index;
		int end = recover->eval_ok;
		int_expression * tmp=instance_of<int_expression> (stack[begin].get());
		int realend = begin+end;
		begin++;
		sum = tmp->get_value();
		while(begin<realend){
			tmp = instance_of<int_expression> (stack[begin].get());
			sum-=tmp->get_value();
			begin++;
		}

		tmp = new int_expression(sum);

		return shared_ptr<expression>( static_cast<expression*> (tmp) );
	};


	auto mul_fun =[]( call_recover* recover ,std::vector<std::shared_ptr<expression>  >& stack)->std::shared_ptr<expression>{
		int sum =1;
		int begin = recover->begin_index;
		int end = recover->eval_ok;
		int_expression * tmp;
		int realend = begin+end;
		while(begin<realend){
			tmp = instance_of<int_expression> (stack[begin].get());
			sum*=tmp->get_value();
			begin++;
		}

		tmp = new int_expression(sum);
		return shared_ptr<expression>( static_cast<expression*> (tmp) );
	};

	auto div_fun =[]( call_recover* recover , std::vector<std::shared_ptr<expression>  >& stack )->std::shared_ptr<expression>{
		int sum =0;
		int begin = recover->begin_index;
		int end = recover->eval_ok;
		int_expression * tmp=instance_of<int_expression> (stack[begin].get());
		int realend = begin+end;
		begin++;
		sum = tmp->get_value();

		while(begin<realend){
			tmp = instance_of<int_expression> (stack[begin].get());
			sum /= tmp->get_value();
			begin++;
		}

		tmp = new int_expression(sum);
		return shared_ptr<expression>( static_cast<expression*> (tmp) );
	};

	auto eq_fun= []( call_recover* recover ,std::vector<std::shared_ptr<expression>  >& stack )->std::shared_ptr<expression>{
		int begin = recover->begin_index;

		int_expression * tmp1=instance_of<int_expression> (stack[begin].get());
		int_expression * tmp2=instance_of<int_expression> (stack[begin+1].get());
		bool tmp = tmp1->get_value() == tmp2->get_value() ;
		bool_expression *ret = new  bool_expression(tmp);
		return shared_ptr<expression>( static_cast<expression*> (ret) );
	};

	auto define_fun= []( call_recover* recover , std::vector<std::shared_ptr<expression>  >& stack )->std::shared_ptr<expression>{
		int begin = recover->begin_index;

		atom_expression * atom=instance_of<atom_expression> (stack[begin].get());
		bool ok= evn::get_instance().add_ref_exp(atom->get_value(),stack[begin+1]);
		bool_expression *ret = new  bool_expression(ok);
		return shared_ptr<expression>( static_cast<expression*> (ret) );
	};

	auto id_fun= []( call_recover* recover , std::vector<std::shared_ptr<expression>  >& stack )->std::shared_ptr<expression>{
//		int begin = recover->begin_index;

		return stack[0];
	};

	auto car_fun= []( call_recover* recover , std::vector<std::shared_ptr<expression>  >& stack )->std::shared_ptr<expression>{
		int begin = recover->begin_index;
		list_expression* list= instance_of<list_expression> (stack[begin].get());
		if(list->get_qoute_flag() != qoute_flag::QOUTED){
			cout<<"must QOUTED list";
			exit(0);
		}
		if(list->get_length()==0)
			return NULL;
		return list->get_expression(0);
	};

	auto ge_fun= []( call_recover* recover ,std::vector<std::shared_ptr<expression>  >& stack )->std::shared_ptr<expression>{
		int begin = recover->begin_index;

		int_expression * tmp1=instance_of<int_expression> (stack[begin].get());
		int_expression * tmp2=instance_of<int_expression> (stack[begin+1].get());
		bool tmp = tmp1->get_value() > tmp2->get_value() ;
		bool_expression *ret = new  bool_expression(tmp);
		return shared_ptr<expression>( static_cast<expression*> (ret) );
	};

	auto ne_fun= []( call_recover* recover ,std::vector<std::shared_ptr<expression>  >& stack )->std::shared_ptr<expression>{
		int begin = recover->begin_index;

		int_expression * tmp1=instance_of<int_expression> (stack[begin].get());
		int_expression * tmp2=instance_of<int_expression> (stack[begin+1].get());
		bool tmp = tmp1->get_value() < tmp2->get_value() ;
		bool_expression *ret = new  bool_expression(tmp);
		return shared_ptr<expression>( static_cast<expression*> (ret) );
	};

	auto null_fun= []( call_recover* recover , std::vector<std::shared_ptr<expression>  >& stack )->std::shared_ptr<expression>{
		int begin = recover->begin_index;
		list_expression* list= instance_of<list_expression> (stack[begin].get());
		if(list->get_qoute_flag() != qoute_flag::QOUTED){
			cout<<"must QOUTED list";
			exit(0);
		}
		bool_expression *ret = new  bool_expression(list->get_length() == 0);
		return shared_ptr<expression>( static_cast<expression*> (ret) );
	};

	auto cons_fun= []( call_recover* recover , std::vector<std::shared_ptr<expression>  >& stack )->std::shared_ptr<expression>{
		int begin = recover->begin_index;
		list_expression* list= instance_of<list_expression> (stack[begin+1].get());

		if(list->get_qoute_flag() != qoute_flag::QOUTED){
			cout<<"must QOUTED list";
			exit(0);
		}
		list_expression* ret=new list_expression();
		*ret = *list;
		stack[begin]->set_qoute_flag(qoute_flag::QOUTED);
		ret->cons_exp( stack[begin]);

		return shared_ptr<expression>( static_cast<expression*> (ret) );

	};

	auto cdr_fun= []( call_recover* recover , std::vector<std::shared_ptr<expression>  >& stack )->std::shared_ptr<expression>{
		int begin = recover->begin_index;
		list_expression* list= instance_of<list_expression> (stack[begin].get());
		if(list->get_qoute_flag() != qoute_flag::QOUTED){
			cout<<"must QOUTED list";
			exit(0);
		}
		list_expression* ret=new list_expression();
		*ret = *list;
		ret->remove_fistof_list();

		return shared_ptr<expression>( static_cast<expression*> (ret) );
	};

	auto concat_fun= []( call_recover* recover , std::vector<std::shared_ptr<expression>  >& stack )->std::shared_ptr<expression>{
		int begin = recover->begin_index;
		int end =  recover->begin_index + recover->eval_ok;

		list_expression* list= instance_of<list_expression> (stack[begin].get());
		if(list->get_qoute_flag() != qoute_flag::QOUTED){
			cout<<"must QOUTED list";
			exit(0);
		}
		list_expression* ret=new list_expression();
		*ret = *list;

		for(int i = recover->begin_index+1;i<end;i++ ){
			ret->append(stack[i]);
		}
		return shared_ptr<expression>( static_cast<expression*> (ret) );

	};



	this->m_proc_map[proc_int_name::ADD] = add_fun;
	this->m_proc_map[proc_int_name::SUB] = sub_fun;
	this->m_proc_map[proc_int_name::MUL] = mul_fun;
	this->m_proc_map[proc_int_name::DIV] = div_fun;
	this->m_proc_map[proc_int_name::EQ] = eq_fun;
	this->m_proc_map[proc_int_name::DEFINE] = define_fun;
	this->m_proc_map[proc_int_name::ID] = id_fun;
	this->m_proc_map[proc_int_name::CAR] = car_fun;
	this->m_proc_map[proc_int_name::GE] = ge_fun;
	this->m_proc_map[proc_int_name::NE] = ne_fun;
	this->m_proc_map[proc_int_name::NUL] = null_fun;

	this->m_proc_map[proc_int_name::CONS] = cons_fun;
	this->m_proc_map[proc_int_name::CDR] = cdr_fun;

	this->m_proc_map[proc_int_name::CONCAT] = concat_fun;



//	let  call/cc
}

