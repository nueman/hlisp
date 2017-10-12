#include "eval.h"

#include <iostream>

using namespace std;

#define DEBUG

//((lambda (a v) (+ a v)) 1 2)
//在fix中会顺着调用每一个list的fix,确定第一个表达式为lambda，
//因为fix时无法确定 函数是参数，或是递归等情况，并且参数列表本身就不需要fix 所以 arg body都不进行fix。
//car大list为lambda所以整个list会被确定为funcall，在eval中求值需要的参数以后会将参数的值和引用与body绑定
//然后展开body


//(let [a 1 ,b 2]
// (+ a b (let [a 3 , b 4]
//              (+ a b  (let [a 4 , b 5]
//                           (+ a b))))))
//所谓变量作用域指的是每一层调用对于a和b都有一个绑定的值，在求值过程中我们需要按照顺序
//去寻找a从最里层到最外层


//(lambda ( le)
//  ((lambda (f ) (f f ) )
//   (lambda (f )
//     ( le (lambda ( x ) ( (f f ) x ))))))

// 调用顺序，闭包，stack,global



std::shared_ptr<expression>  eval::value_of(expression* exp){

//	cout<<"eval: ";
//			exp->print();
//			cout<<endl;

	if(is_self_eval(exp)){
		return apply_stack(exp->shared_from_this());
	}


	atom_expression* atom =  instance_of<atom_expression>(exp);
	if (atom) {
	     	shared_ptr<expression> ret =NULL;

	     	if(!ret){
	     			 ret=find_local_ref(atom->get_value());
	     	}

	     	if(!ret){
	     		//查找全局
	     		ret = evn::get_instance().get_global_ref(atom->get_value());
	     	}
//	     	cout<<atom->get_value()<<" funcall_index  "<<this->m_index_funcall <<"  :  ";
//	     	ret->print();
//	     	cout<<endl;
//	     	if(atom->get_value() == "key"){
//	     			     	cout<<"key:  ";
//	     			     	ret->print();
//	     			     	cout<<endl;
//	     	}

	     	if(!ret){
	     		//GG
	     		cout<<atom->get_value() <<" : NULL"<<endl;
	     		exit(0);
	     	}

			return apply_stack(ret);
	}

	list_expression* list = instance_of<list_expression>(exp);
	if(list){
		if(list->get_qoute_flag() == qoute_flag::QOUTED ){
			//处理解引述
			return apply_stack( exp->shared_from_this() );
		}
		if( list->get_is_lambda()  ){
			return apply_stack( list->shared_from_this() );
		}

		call_recover *re = save_call(list->shared_from_this());

		std::shared_ptr<expression> value;
		if(list->get_need_eval_num() !=0 )
			  value =  value_of(  list->get_expression(re->eval_ok+1).get()  );

		return apply_stack(value);
	}

	return NULL;
}



std::shared_ptr<expression>  eval::apply_stack(std::shared_ptr<expression> evaled_exp){


	if(this->is_end()){
		delete_local_evn();
		return evaled_exp;
	}

	call_recover* recover = get_call();
	list_expression* list  = instance_of<list_expression>(recover->retuen_exp.get());

	if(list->get_need_eval_num() == recover->eval_ok && m_return_lambda){//在bind_funcall_arg 中遇到返回lambda的list求值 push会增加recover->eval_ok
		return evaled_exp;
	}


		while (recover->funcall && list->get_need_eval_num() == recover->eval_ok  ) {
			m_index_local_evn = m_index_local_evn - recover->eval_ok;
			clear_funcall_stack();

			m_index_call--;

			delete m_call_hierarchy[m_index_call];
			m_call_hierarchy[m_index_call] = NULL;
			m_index_funcall--;
			if(this->is_end()){
					delete_local_evn();
					return evaled_exp;
				}
			recover = get_call();
			list  = instance_of<list_expression>(recover->retuen_exp.get());

		}



	push(recover,evaled_exp);

	if(list->get_proc_name() == proc_int_name::CASE){
		int eval_index = match_exp(evaled_exp , recover);
		this->call_ok(recover);
		if(eval_index != -1 )
			return value_of(  list->get_expression(eval_index).get()  );

		return NULL;
	}

	if(list->get_proc_name() == proc_int_name::COND){
		bool_expression* tmp = instance_of<bool_expression>(evaled_exp.get());
		if(tmp && tmp->get_value()){
			this->call_ok(recover);
			return value_of(  list->get_expression(recover->now_eval+2).get()  );
		}

		atom_expression* atom = instance_of<atom_expression>(evaled_exp.get());
		if(atom && atom->get_value() == "else"){
			this->call_ok(recover);
			return value_of(  list->get_expression(recover->now_eval+2).get()  );
		}

		recover->now_eval = recover->now_eval+2;
		return value_of(  list->get_expression(recover->now_eval+1).get()  );
	}




	if(list->get_proc_name() == proc_int_name::IF){
		bool_expression* tmp = instance_of<bool_expression>(evaled_exp.get());
		this->call_ok(recover);
		if(tmp->get_value())
			return value_of(  list->get_expression(recover->eval_ok+1).get()  );
		else
			return value_of(  list->get_expression(recover->eval_ok+2).get()  );
	}

	if(list->get_need_eval_num() == recover->eval_ok || list->get_need_eval_num() ==0 ){

		if(list->get_proc_name() == proc_int_name::FUNCALL ){
			expression*  ret;
			expression* call= get_call_exp(recover);
			list_expression* lambda = instance_of<list_expression>(call);
			callcc_expression* callcc = instance_of<callcc_expression>(call);
			if(lambda){
				this->m_index_funcall++;
				if(lambda->get_free_var_length() != 0){
					this->m_index_closurecall++;
					auto free = lambda->get_free_var();
					this->m_free_len = lambda->get_free_var_length();
					this->m_free_var.insert(m_free_var.begin(),free.begin() ,free.begin() +m_free_len );
					for(int i=0 ;i<m_free_len;i++){
						auto ret = evn::get_instance().get_global_ref(m_free_var[i]);
						if(ret){
							m_free_var[i]="";
							if(m_free_len == 1)
								this->m_index_closurecall--;
						}
					}
				}
				 ret=bind_funcall_arg(recover,lambda);

//			    this->call_ok(recover);
					auto iter = m_stack.begin();
					m_stack.erase(iter+recover->begin_index  , iter+recover->begin_index+recover->eval_ok);
					m_index_stack = m_index_stack - recover->eval_ok;
				 recover->funcall =true;
				 return value_of(ret );
			}
			if(callcc){
				eval* pre_eval = new eval();
				*pre_eval = *callcc->get_eval().get();
				auto aa = callcc->get_eval()->go_on( m_stack[recover->begin_index]  );
				callcc->get_eval().reset(pre_eval);
				ret=aa.get();
				this->call_ok(recover);
				return value_of(ret );
			}
		}else if(list->get_proc_name() == proc_int_name::SETCC){
			return create_callcc();
		}else{
			auto retfun = primary_proc::get_instance().get_fun(list->get_proc_name());
			//处理闭包，将栈上的参数全部扔进去
			auto  ret_func=m_stack[recover->begin_index+1] ;
			list_expression* list_ret_func = instance_of<list_expression>(ret_func.get());
		   if(list_ret_func && list_ret_func->get_is_lambda()  &&  list->get_proc_name() ==proc_int_name::DEFINE  ){
//			   ( (((A 1) 2) 3) 4) 这样的形式调用闭包所有变量还在栈上，只有在define的时候才需将变量导出
			   create_closure(list_ret_func ,recover->begin_index+1  );
		   }

			auto ret =retfun(recover,m_stack);
			this->call_ok(recover);
			return ret;

		}


	}

	return value_of( list->get_expression(recover->eval_ok+1).get() );

}


std::shared_ptr<expression>eval:: create_callcc(){
	auto re=get_call();
	eval* neweval = new eval();
	auto atom =instance_of<atom_expression>( m_stack[re->begin_index].get());
	call_ok(re); //去掉callcc本身


	*neweval = *this;
	expression* cc = new callcc_expression(neweval , m_call_hierarchy[0]->retuen_exp);

	shared_ptr<expression>s_cc(cc);
	bool is= evn::get_instance().add_ref_exp( atom->get_value()  ,s_cc );

	while(m_index_call>0){
		call_ok(get_call());
	}

	return std::shared_ptr<expression>(new bool_expression(is));

}

void eval::create_closure(list_expression* ret_fun, int index ){

	list_expression* closure = new list_expression();
	*closure = *ret_fun; //copy默认构造函数

	 add_all_to_closure( closure );
	 this->m_stack[index].reset(closure);


}

//(case a
//		1 (+ 1 1)
//     2 (+ 2 2)
//		3 (+ 3 3))


bool eval::do_match_list(list_expression*  exp_list,  list_expression* match_list ){

	bool matchrest=false;
	int jumped = 0;
	int match_num = 0;
	if(exp_list->get_length() == 0  ){
		if( match_list->get_length() == 0)
			return true;
		return false;
	}


	for(int i=0;i<match_list->get_length();i++){
		auto match_son_inum =  instance_of<int_expression>(match_list->get_expression(i).get());
		auto match_son_list = instance_of<list_expression>(match_list->get_expression(i).get());
		auto match_son_atom = instance_of<atom_expression>(match_list->get_expression(i).get());
		if(match_son_atom && match_son_atom->get_value() ==":"  ){
			matchrest = true;
			jumped++;
			continue;
		}

		if(i<exp_list->get_length()  + jumped){
			auto exp_son_inum =  instance_of<int_expression>(exp_list->get_expression(i - jumped).get());
			auto exp_son_list = instance_of<list_expression>(exp_list->get_expression(i - jumped ).get());
			if(match_son_atom){
				if(matchrest){
					auto restlist = new list_expression();
					*restlist = *exp_list;
					int remove_num = i-jumped;
					while(remove_num){
						restlist->remove_fistof_list();
						remove_num--;
					}
					shared_ptr<expression> ref( restlist );
					add_local_ref(match_son_atom->get_value(), ref,true);
					match_num = match_list->get_length() ;
				}else{
					add_local_ref(match_son_atom->get_value(), exp_list->get_expression(i) ,true);
					match_num++;
				}
			}

			if(match_son_inum){
				if(!exp_son_inum )
					return false;
				if(match_son_inum->get_value() !=exp_son_inum->get_value())
					return false;
				match_num++;
			}
			if(exp_son_list && match_son_list){
				bool match_son = do_match_list(exp_son_list, match_son_list);
				if(!match_son)
					return false;
				match_num++;
			}
		}else{
			if(match_son_inum || match_son_list)
				return false;
			if(match_son_atom){
				auto ref_list =new list_expression();
				ref_list->set_qoute_flag(qoute_flag::QOUTED);
				shared_ptr<expression> ref(  ref_list);
				add_local_ref(match_son_atom->get_value(), ref ,true);
				match_num = match_list->get_length()  ;
			}
		}

	}


	if(match_num == match_list->get_length() &&  match_list->get_length()!=0 )
		return true;
	return false;
}


int eval::match_exp(std::shared_ptr<expression> exp, call_recover* recover){

	int ret = -1;
	auto caselist = instance_of<list_expression>(recover->retuen_exp.get());
	auto exp_inum =  instance_of<int_expression>(exp.get());
	auto exp_list =  instance_of<list_expression>(exp.get());

	for(int i = 2;i< caselist->get_length();i++ ){
		if((i % 2) != 0)
			continue;
		auto match_inum =  instance_of<int_expression>(caselist->get_expression(i).get());
		auto match_list = instance_of<list_expression>(caselist->get_expression(i).get());
		auto match_atom = instance_of<atom_expression>(caselist->get_expression(i).get());

		if(match_atom && match_atom->get_value() == "all" ){
			ret =i+1;
			break;
		}

		if(exp_inum && match_inum  ){
			if( match_inum->get_value() == exp_inum->get_value() ){
				ret =i+1;
				break;
			}
		}

		if(exp_list && match_list){

			bool matchson = do_match_list(exp_list,match_list );
			if(matchson ){
				ret =i+1;
				break;
			}
		}
	}
	return ret;
}




expression* eval::get_call_exp(call_recover* recover){

	list_expression* l  = instance_of<list_expression>(recover->retuen_exp.get());

	auto func =l->get_expression(0).get();

		//直接push函数的调用 ((lambda (x) (+ x 1)) 3)
	   //唯一callcc和function不一样的地方
		list_expression* list =NULL;
		list=instance_of<list_expression>(func);

		atom_expression* atom =instance_of<atom_expression>(func);

		if(!list){	//		使用的define定义的函数或者参数的是函数 (foo x)

			list =instance_of<list_expression>(find_local_ref(atom->get_value()).get());//寻找栈
			if(!list){
				auto ref = evn::get_instance().get_global_ref(atom->get_value()).get(); //callcc 只可能在全局！！！
				list =instance_of<list_expression>(ref);
				if(list)
					return list;
				auto cc = instance_of<callcc_expression>(ref);
				if(cc)
					return cc;
			}

		}


		if(list && !list->get_is_lambda()){
	//		 返回函数的调用 ( (return_func 3) 3)
			m_return_lambda =true;
			list = instance_of<list_expression> ( value_of(list).get());
			m_return_lambda =false;

		}

		assert(list!=NULL);

		return list;
}


expression* eval::bind_funcall_arg(call_recover* recover, list_expression* list){


//处理闭包变量
	auto closure_ref = list->get_closure_map();
	auto iter = closure_ref.begin();
	while(iter != closure_ref.end() ){
		add_local_ref(iter->first , iter->second );
		iter++;
	}


	auto body = list->get_expression(2);
	assert(body!=NULL);

//处理参数
	int begin = recover->begin_index;
	int end = recover->eval_ok+ recover->begin_index;
	int i=0;
	while(begin<end){
		string arg_name = list->get_arg_name(i) ;
		add_local_ref(list->get_arg_name(i) , m_stack[begin] );

		i++;
		begin++;
	}

	return body.get();
}


//std::cout<<atom->get_value()<<" : ";
// iter->second->print();
// std::cout<<std::endl;
//在求值atom时确定参数的打印

//std::cout<<"value_of_body  : ";
//ret->print();
//std::cout<<std::endl;
//在funcall完成是打印需要展开的body

std::shared_ptr<expression>  eval::loop_apply_stack(std::shared_ptr<expression> evaled_exp){
	static int a =0 ;
	a++;
	cout<<"cout apply: "<<a<<endl;


	call_recover* recover = get_call();
	list_expression* list  = instance_of<list_expression>(recover->retuen_exp.get());

	if(list->get_need_eval_num() == recover->eval_ok) //在bind_funcall_arg 中遇到返回lambda的list求值 push会增加recover->eval_ok
		return evaled_exp;

	push(recover,evaled_exp);

	if(list->get_proc_name() == proc_int_name::IF){
		bool_expression* tmp = instance_of<bool_expression>(evaled_exp.get());
		this->call_ok(recover);
		if(tmp->get_value())
			return loop_value_of(  list->get_expression(recover->eval_ok+1).get()  );
		else
			return loop_value_of(  list->get_expression(recover->eval_ok+2).get()  );
	}

	if(list->get_need_eval_num() == recover->eval_ok || list->get_need_eval_num() ==0 ){

		if(list->get_proc_name() == proc_int_name::FUNCALL ){
			expression*  ret;
			expression* call= get_call_exp(recover);
			list_expression* lambda = instance_of<list_expression>(call);
			callcc_expression* callcc = instance_of<callcc_expression>(call);
			if(lambda){
				this->m_index_funcall++;
				if(lambda->get_free_var_length() != 0){
					this->m_index_closurecall++;
					auto free = lambda->get_free_var();
					this->m_free_len = lambda->get_free_var_length();
					this->m_free_var.insert(m_free_var.begin(),free.begin() ,free.begin() +m_free_len );
					for(int i=0 ;i<m_free_len;i++){
						auto ret = evn::get_instance().get_global_ref(m_free_var[i]);
						if(ret){
							m_free_var[i]="";
							if(m_free_len == 1)
								this->m_index_closurecall--;
						}
					}
				}
				 ret=bind_funcall_arg(recover,lambda);
				 this->call_ok(recover);
				 return loop_value_of(ret );
			}
			if(callcc){
				eval* pre_eval = new eval();
				*pre_eval = *callcc->get_eval().get();
				auto aa = callcc->get_eval()->go_on( m_stack[recover->begin_index]  );
				callcc->get_eval().reset(pre_eval);
				ret=aa.get();
				this->call_ok(recover);
				return loop_value_of(ret );
			}
		}else if(list->get_proc_name() == proc_int_name::SETCC){
			return create_callcc();
		}else{
			auto retfun = primary_proc::get_instance().get_fun(list->get_proc_name());
			//处理闭包，将栈上的参数全部扔进去
			auto  ret_func=m_stack[recover->begin_index+1] ;
			list_expression* list_ret_func = instance_of<list_expression>(ret_func.get());
		   if(list_ret_func && list_ret_func->get_is_lambda()  &&  list->get_proc_name() ==proc_int_name::DEFINE  ){
//			   ( (((A 1) 2) 3) 4) 这样的形式调用闭包所有变量还在栈上，只有在define的时候才需将变量导出
			   create_closure(list_ret_func ,recover->begin_index+1  );
		   }

			auto ret =retfun(recover,m_stack);
			this->call_ok(recover);
			if(this->is_end()){
				delete_local_evn();
				return ret;
			}

			return loop_value_of(ret.get());

		}
	}

	return loop_value_of( list->get_expression(recover->eval_ok+1).get() );

}

std::shared_ptr<expression>  eval::loop_value_of(expression* eexp){

	static int a =0 ;
	a++;
	cout<<"cout value: "<<a<<endl;
	auto exp = eexp;
	while(exp){
		if(is_self_eval(exp)){
				return loop_apply_stack(exp->shared_from_this());
			}


			atom_expression* atom =  instance_of<atom_expression>(exp);
			if (atom) {
			     	shared_ptr<expression> ret =NULL;

			     	if(!ret){
			     			 ret=find_local_ref(atom->get_value());
			     	}

			     	if(!ret){
			     		//查找全局
			     		ret = evn::get_instance().get_global_ref(atom->get_value());
			     	}

//			     	cout<<atom->get_value()<<" : ";
//			     	ret->print();
//			     	cout<<endl;

			     	if(!ret){
			     		//GG
			     		cout<<atom->get_value() <<" : NULL"<<endl;
			     		exit(0);
			     	}

					return loop_apply_stack(ret);
			}

			list_expression* list = instance_of<list_expression>(exp);
			if(list){
				if(list->get_qoute_flag() == qoute_flag::QOUTED ){
					//处理解引述
					return loop_apply_stack( exp->shared_from_this() );
				}
				if( list->get_is_lambda()  ){
					return loop_apply_stack( list->shared_from_this() );
				}
				call_recover *re = save_call(list->shared_from_this());
				std::shared_ptr<expression> value;
				if(list->get_need_eval_num() !=0 ){
					 exp=  list->get_expression(re->eval_ok+1).get()  ;
					continue;
				}
			//	return apply_stack(value);
			}
	}
	return NULL;
}
