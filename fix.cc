#include "fix.h"

//car cdr  if cond  + - * / or not eq?  define let
//defn 因为布打算实现宏所以没办法用宏定义之，但是 (define func (lambda....)) 真的很蛋疼



 fix_expression& fix_expression::get_instance(){
	 static fix_expression instance;   // c++ 11 中局部static是线程安全的！！！！
	 return instance;
 }

// (defn sum [a]
//	(if (eq? a 0)
//		0
//		(+ a (sum (-a 1)))))


 void  find_free(list_expression* func , list_expression* body){

	 if(body->get_is_lambda())
		 return;

	 assert(body!=NULL);
	 int length = body->get_length();
	 int i=0;
	 atom_expression* atom = NULL;
	 list_expression* b_list =NULL;
	while(i<length){
		atom = instance_of<atom_expression>( body->get_expression(i) .get());
		if(atom && atom->get_qoute_flag()==qoute_flag::NORMAL &&func->get_arg_index(atom->get_value()) == -1 ){
				func->add_free( atom->get_value() );
		}
		b_list = instance_of<list_expression>( body->get_expression(i) .get());
		if(b_list){
			find_free(func,b_list);
		}
		i++;
	}
 }

 fix_expression:: fix_expression(){
	//(if a b c)
	 auto if_fun = [](list_expression* list)->void {

		 if(list->get_length() != 4){
			 comperro("(if a b c)  a b c is expression  ")
		 }
		 list->set_is_apply(true);
		 list->set_proc_name(proc_int_name::IF);
		 list->set_need_eval_num(-1);

	 };
	 //(eq? 2 3)
	 auto eq_fun=[](list_expression* list)->void {
		 if(list->get_length() != 3){
					 comperro("(eq a b )  a b  is expression  ")
				 }
		 list->set_is_apply(true);
		 list->set_proc_name(proc_int_name::EQ);
		 list->set_need_eval_num(2);
	 };

	 auto calu_fun =[](list_expression* list)->void {
		 if(list->get_length() < 3){
			 comperro("(calu a b .......... )  a b.....  is expression  ")
		 }
		 list->set_is_apply(true);
		 list->set_need_eval_num(list->get_length() -1);
	 };

	 auto add_fun =[calu_fun](list_expression* list)->void {
		 calu_fun(list);
		 list->set_proc_name(proc_int_name::ADD);
	 };

	 auto sub_fun =[calu_fun](list_expression* list)->void {
		 calu_fun(list);
		 list->set_proc_name(proc_int_name::SUB);
	 };

	 auto mul_fun =[calu_fun](list_expression* list)->void {
		 calu_fun(list);
		 list->set_proc_name(proc_int_name::MUL);
	 };

	 auto div_fun =[calu_fun](list_expression* list)->void {
			 calu_fun(list);
			 list->set_proc_name(proc_int_name::DIV);
	 };



	 //	 (lambda (a) (+ 1 a))
	 auto labmda_fun=[](list_expression* list)->void {

		 if(list->get_length() !=3 ){
			 comperro("(lambda (arg ) body )    ")
		 }
		 list_expression* test_arg_list = dynamic_cast<list_expression*>(list->get_expression(1).get());
		 if(!test_arg_list){
			 comperro("args must list ")
		 }

		 atom_expression* test_pre_arg_atom = NULL;
		 for(unsigned int i= 0; i < test_arg_list->get_length(); i++){
			 test_pre_arg_atom = dynamic_cast<atom_expression*> (test_arg_list->get_expression(i).get());
			 if(!test_pre_arg_atom){
				 comperro("pre args must atom ")
			 }
		 }
		 list->set_is_lambda(true);
		 list->set_need_arg(test_arg_list->get_length());

		 find_free(list,   dynamic_cast<list_expression*>(list->get_expression(2).get())  );
	 };


//  (define a b)
	 auto define_fun =[](list_expression* list)->void {
		 if(list->get_length() !=3 ){
					 comperro("(define a b )  a is atom ")
		 }
		 atom_expression*  test_a = dynamic_cast<atom_expression*> (list->get_expression(1).get());
		 if(!test_a){
			 comperro("(define a b )  a is atom ")
		 }

		 list->set_is_apply(true);
		 list->set_proc_name(proc_int_name::DEFINE);
		 list->get_expression(1)->set_qoute_flag(qoute_flag::QOUTED);
		 list->set_need_eval_num(2);
	 };

//	 (if (eq? 1 (callcc aa))  1 2 )
	 auto callcc_fun=[](list_expression* list)->void {
		 if(list->get_length() !=2 ){
			 comperro("(callcc aa)   aa is atom ")
		 }

		 list->set_is_apply(true);
		 list->get_expression(1)->set_qoute_flag(qoute_flag::QOUTED);
		 list->set_proc_name(proc_int_name::SETCC);
		 list->set_need_eval_num(1);
	 };

	 auto id_fun=[](list_expression* list)->void {
		 if(list->get_length() !=2 ){
			 comperro("(id aa)  ")
		 }

		 list->set_is_apply(true);
		 list->get_expression(0)->set_qoute_flag(qoute_flag::QOUTED);
		 list->set_proc_name(proc_int_name::ID);
		 list->set_need_eval_num(1);
	 };

	 auto car_fun=[](list_expression* list)->void {
		 if(list->get_length() !=2 ){
			 comperro("(car aa)  aa must list ")
		 }

		 list->set_is_apply(true);
		 list->get_expression(0)->set_qoute_flag(qoute_flag::QOUTED);
		 list->set_proc_name(proc_int_name::CAR);
		 list->set_need_eval_num(1);
	 };

	 auto ge_fun=[](list_expression* list)->void {
		 if(list->get_length() !=3 ){
			 comperro("(> a b)  aa must list ")
		 }

		 list->set_is_apply(true);
		 list->get_expression(0)->set_qoute_flag(qoute_flag::QOUTED);
		 list->set_proc_name(proc_int_name::GE);
		 list->set_need_eval_num(2);
	 };

	 auto ne_fun=[](list_expression* list)->void {
			 if(list->get_length() !=3 ){
				 comperro("(> a b)  aa must list ")
			 }

			 list->set_is_apply(true);
			 list->get_expression(0)->set_qoute_flag(qoute_flag::QOUTED);
			 list->set_proc_name(proc_int_name::NE);
			 list->set_need_eval_num(2);
	};


	 auto null_fun=[](list_expression* list)->void {
		 if(list->get_length() !=2 ){
			 comperro("(null? aa)  aa must list ")
		 }

		 list->set_is_apply(true);
		 list->get_expression(0)->set_qoute_flag(qoute_flag::QOUTED);
		 list->set_proc_name(proc_int_name::NUL);
		 list->set_need_eval_num(1);
	 };

	 auto cons_fun=[](list_expression* list)->void {
		 if(list->get_length() !=3 ){
			 comperro("(cons a list )  ")
		 }

		 list->set_is_apply(true);
		 list->get_expression(0)->set_qoute_flag(qoute_flag::QOUTED);
		 list->set_proc_name(proc_int_name::CONS);
		 list->set_need_eval_num(2);
	 };

	 auto cdr_fun=[](list_expression* list)->void {
		 if(list->get_length() !=2 ){
			 comperro("(cdr aa ) aa must list  ")
		 }

		 list->set_is_apply(true);
		 list->get_expression(0)->set_qoute_flag(qoute_flag::QOUTED);
		 list->set_proc_name(proc_int_name::CDR);
		 list->set_need_eval_num(1);
	 };

	 auto concat_fun=[](list_expression* list)->void {
		 if(list->get_length() < 3 ){
			 comperro("(cons  aa ...... ) aa must list  ")
		 }

		 list->set_is_apply(true);
		 list->get_expression(0)->set_qoute_flag(qoute_flag::QOUTED);
		 list->set_proc_name(proc_int_name::CONCAT);
		 list->set_need_eval_num( list->get_length() -1 );
	 };

	 auto cond_fun=[](list_expression* list)->void {
		 if(list->get_length() < 3 ){
			 comperro("(cond  aa bb cc dd ) ")
		 }

		 list->set_is_apply(true);
		 list->get_expression(0)->set_qoute_flag(qoute_flag::QOUTED);
		 list->set_proc_name(proc_int_name::COND);
		 list->set_need_eval_num( -1 );
	 };

	 auto case_fun=[](list_expression* list)->void {
		 if(list->get_length() < 3 ){
			 comperro("(case  aa bb cc dd ) ")
		 }

		 list->set_is_apply(true);
		 list->get_expression(0)->set_qoute_flag(qoute_flag::QOUTED);
		 list->set_proc_name(proc_int_name::CASE);
		 list->set_need_eval_num( -1 );
	 };

	 proc_name.push_back("if");
	 proc_fix_fun.push_back(if_fun);

	 proc_name.push_back("eq?");
	 proc_fix_fun.push_back(eq_fun);

	 proc_name.push_back("+");
     proc_fix_fun.push_back(add_fun);
	 proc_name.push_back("-");
     proc_fix_fun.push_back(sub_fun);
	 proc_name.push_back("*");
     proc_fix_fun.push_back(mul_fun);
	 proc_name.push_back("/");
     proc_fix_fun.push_back(div_fun);

	 proc_name.push_back("lambda");
	 proc_fix_fun.push_back(labmda_fun);

	 proc_name.push_back("define");
	 proc_fix_fun.push_back(define_fun);

	 proc_name.push_back("callcc");
	 proc_fix_fun.push_back(callcc_fun);

	 proc_name.push_back("id");
	 proc_fix_fun.push_back(id_fun);

	 proc_name.push_back("car");
	 proc_fix_fun.push_back(car_fun);

	 proc_name.push_back(">");
	 proc_fix_fun.push_back(ge_fun);

	 proc_name.push_back("<");
	 proc_fix_fun.push_back(ne_fun);

	 proc_name.push_back("null?");
	 proc_fix_fun.push_back(null_fun);

	 proc_name.push_back("cons");
	 proc_fix_fun.push_back(cons_fun);

	 proc_name.push_back("cdr");
	 proc_fix_fun.push_back(cdr_fun);

	 proc_name.push_back("concat");
	 proc_fix_fun.push_back(concat_fun);

	 proc_name.push_back("cond");
	 proc_fix_fun.push_back(cond_fun);

	 proc_name.push_back("case");
	 proc_fix_fun.push_back(case_fun);
}

void fix_expression::do_fix_ref_funcall(list_expression* call) {

	atom_expression* test_atom =dynamic_cast<atom_expression*>(call->get_expression(0).get());
	auto car = evn::get_instance().get_global_ref(test_atom->get_value());
	list_expression* list = dynamic_cast<list_expression*>(car.get());
	if(!list){
			cout<<test_atom->get_value()<<" must be func or callcc"<<endl;
	}

	if(list && !list->get_is_lambda()){
		list->print();
		cout<<" this list must  return  func"<<endl;
	}

	if(list && list->get_is_lambda() ){
		call->set_need_eval_num(list->get_need_arg());
	}else{
		call->set_need_eval_num(call->get_length()-1);
	}

	 call->set_proc_name(proc_int_name::FUNCALL);
 }


  void fix_expression::do_fix_lambda_funcall(list_expression* call){
	  list_expression* list = dynamic_cast<list_expression*>(call->get_expression(0).get());
	  if(!list ){
		  comperro("car of list must be callable")
	  }
	  if(!list->get_is_lambda()){
		  list->print();
		  		 	cout<<" this list musb be return a fun or call cc"<<endl;
	  }
	  if(list->get_is_lambda())
		  call->set_need_eval_num(list->get_need_arg());
	  else
		  call->set_need_eval_num(call->get_length() -1 );
	  call->set_proc_name(proc_int_name::FUNCALL);
  }


//  (define Y
//   (lambda ( le)
//    ((lambda (f ) (f f ) )
//     (lambda (f )
//       ( le (lambda ( x ) ( (f f ) x )))))))

//  (define   `Y
//    (LAMBDA  (le   )
//            (call  need 1 params (LAMBDA  (f   )  (call  need 1 params f   f   )  )
//                  (LAMBDA  (f   )
//                         (call  need 1 params le
//                               (LAMBDA  (x   )  (call  need 1 params (call  need 1 params f   f   )  x   )  )  )  )  )  )  )

 void list_expression::fix(  ){

		if(this->get_qoute_flag() == qoute_flag::QOUTED  )
			return ;

		atom_expression* test_atom = dynamic_cast<atom_expression*>(this->get_expression(0).get());

		if(test_atom && test_atom->get_value()=="lambda"){
			 list_expression* test_arg_list = dynamic_cast<list_expression*>(this->get_expression(1).get());
			 if(!test_arg_list){
				 comperro("args must list ")
			 }
			 test_arg_list->set_fix_flag(fix_flag::UNFIX);
			 test_arg_list = dynamic_cast<list_expression*>(this->get_expression(2).get());
			 if(test_arg_list)
				 test_arg_list->set_fix_flag(fix_flag::LAMBDABODY);
		}

		int len = this->get_length();
		list_expression* list =NULL;
		for(int i=0;i<len;i++){
			list =  dynamic_cast<list_expression*>(this->get_expression(i).get());
			if(list && list->get_fix_flag() != fix_flag::UNFIX ){
				if(list->get_fix_flag() == fix_flag::LAMBDABODY ){
					list_expression* sunlist =NULL;
					int listlen = list->get_length();
					for(int j=0;j<listlen;j++){
						sunlist =  dynamic_cast<list_expression*>(list->get_expression(j).get());
						if(sunlist){
							sunlist->set_fix_flag(fix_flag::LAMBDABODY);
						}
					}
				}
//				cout<<"yanzhen: ";
//				list->print();
//				cout<<endl;
				list->fix();
//				this->merge_free(list );
			}
		}


		if(!test_atom){
			//确定funcall case 1  ( (lambda (a b) (+ a b)) 1 2)
			fix_expression::do_fix_lambda_funcall(this);
			return;
		}

		fix_expression instance = 	fix_expression::get_instance();

		auto iter_name = instance.get_proc_name().begin();
		auto end_name = instance.get_proc_name().end();
		auto iter_func = instance.get_fix_func().begin();

		while(iter_name != end_name){
// 确定基本操作
				if( *(iter_name)  == test_atom->get_value() ){
					test_atom->set_qoute_flag(qoute_flag::QOUTED);
					 (*iter_func)(this);
					 return;
				}
				iter_func++;
				iter_name++;
		}

//确定funcall case2 (aa 1 2 ) aa为函数
		fix_expression::do_fix_ref_funcall(this);
 }

