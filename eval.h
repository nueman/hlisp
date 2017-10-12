// 后面有重要注释！

#ifndef ______EVAL_____
#define ______EVAL_____

#include "expression.h"
#include "primary_proc.h"


#include <assert.h>

struct call_recover{
public:
	std::shared_ptr<expression> retuen_exp;
	int eval_ok;
	int begin_index; //相当于c中的ebp
	bool funcall=false;
	int now_eval=0; //用于cond case表示现在需要eval哪一个，cond case会因为条件跳过求值

};

struct local_ref{
public:
	int call_index;
	std::string name;
	std::shared_ptr<expression> exp;
	bool is_clear = false;
};


class eval{
public:
	eval():m_stack(41120),m_call_hierarchy(10280),m_local_evn(81920),m_index_call(0),m_index_stack(0),
	m_index_local_evn(0),m_index_funcall(0),m_free_var(1024),m_index_closurecall(0),m_free_len(0),
   m_return_lambda(false)
   {
	}


	eval& operator=(eval& e) {
		this->m_stack.insert(this->m_stack.begin(), e.m_stack.begin(), e.m_stack.end()   );
		this-> m_index_call = e.m_index_call;
		this-> m_index_stack = e.m_index_stack;
		this->  m_index_local_evn=e.m_index_local_evn;

		for(int i=0;i< this->m_index_call ;i++){
			call_recover* nre = new call_recover();
			*nre = *(e.m_call_hierarchy[i]);
			this->m_call_hierarchy[i] = nre;
		}

		for(int j=0;j< this->m_index_local_evn ;j++){
			local_ref* nref= new local_ref();
			*nref = *(e.m_local_evn[j]);
			this->m_local_evn[j] = nref;
		}


		return *this;
	}

	std::shared_ptr<expression> go_on(std::shared_ptr<expression> exp){

		std::shared_ptr<expression>  res;
		std::shared_ptr<expression>  evaled= exp;
		std::vector<std::shared_ptr<expression> > tmpstack(100);

		while(m_index_call > 0){
			res = value_of(evaled.get());
			tmpstack.push_back(res);
			evaled = res;
		}

		return res;
	};


	 std::shared_ptr<expression> value_of(expression* exp);
	 std::shared_ptr<expression> apply_stack(std::shared_ptr<expression> );
	 std::shared_ptr<expression>  loop_value_of(expression* eexp);
	 std::shared_ptr<expression>  loop_apply_stack(std::shared_ptr<expression> evaled_exp);

private:



		bool is_self_eval(expression* exp){
			assert(exp!=NULL);
			int type = exp->get_type();
			return (type==expression_type::BOOL ||
					type==expression_type::DOUBLE ||
					type == expression_type::INT ||type == expression_type::STRING ||
					exp->get_qoute_flag() ==qoute_flag::QOUTED    );
		}

		call_recover* save_call(std::shared_ptr<expression> exp){
			call_recover* re=new call_recover();
			re->begin_index =m_index_stack;
			re->eval_ok=0;
			re->retuen_exp=exp;
			m_call_hierarchy[m_index_call] = re;
			m_index_call++;
			return std::ref(m_call_hierarchy[m_index_call-1]) ;
		}

		bool is_end(){
			return m_index_call ==0;
		}

		call_recover* get_call( )  {
			if(m_index_call == 0)
				return NULL;
			return m_call_hierarchy[m_index_call-1];
		}

		void call_ok(call_recover* re){
			auto iter = m_stack.begin();
			m_stack.erase(iter+re->begin_index  , iter+re->begin_index+re->eval_ok);
			m_index_stack = m_index_stack - re->eval_ok;
			m_index_call --;
			delete m_call_hierarchy[m_index_call]; //实在不觉得有必要用shared_ptr......
			m_call_hierarchy[m_index_call]=NULL;
		}

		void push(call_recover* recover, std::shared_ptr<expression>& results){
			m_stack[m_index_stack] = results;
			m_index_stack++;
			recover->eval_ok++;
		}


		void add_local_ref(std::string  name, std::shared_ptr<expression> exp ,bool not_free =false){
			local_ref* lref =new local_ref();
			lref->name = name;
			lref->call_index = m_index_funcall;
			lref->exp = exp;
			m_local_evn[m_index_local_evn] =  lref;
			m_index_local_evn++;

			if(not_free){
				lref->is_clear =true;
				for(int i=0;i<m_free_len;i++){
					if(m_free_var[i] == name )
						m_free_var[i]="";
				}
			}

		}

		bool is_free(std::string & name){
			for(int i=0; i<m_free_len;i++){
				if(m_free_var[i] == name)
					return true;
			}
			return false;
		}


		std::shared_ptr<expression> find_closure_ref(std::string & name) {

		std::shared_ptr<expression> ret = NULL;
		int end = m_index_local_evn - 1;
		bool first = true;
		int index = -1;
		int   closurecall_index = m_index_closurecall -1;
		while (end >= 0) {
			if (m_local_evn[end]&&m_local_evn[end]->name == name) {
				if (first) {
					ret = m_local_evn[end]->exp;
					index = m_local_evn[end]->call_index - closurecall_index;
					//std::cout<<"index: "<<index<<std::endl;
					first = false;
				}

				if (index == m_local_evn[end]->call_index)
					ret = m_local_evn[end]->exp;

			}

			end--;
		}
		return ret;
	}

		std::shared_ptr<expression> find_local_ref(std::string & name){
			if(is_free(name)){
						return find_closure_ref(name);
			}

			int end = m_index_local_evn -1;
			while(end>=0){
				if( m_local_evn[end] && m_local_evn[end]->name == name  &&  m_local_evn[end]->call_index ==m_index_funcall ){
					return  m_local_evn[end]->exp;
				}
				end--;
			}
			return NULL;
		}

		void delete_local_evn(){
			int i=0;
			while(m_local_evn[i] !=NULL){
				delete m_local_evn[i];
				m_local_evn[i] =NULL;
				i++;
			}
			m_index_local_evn = 0;
			m_index_closurecall =0;
			m_index_stack =0;
		}

		void add_all_to_closure(list_expression* exp){
			int i=0;
			while(m_local_evn[i]!=NULL){
				exp->add_closure_ref(m_local_evn[i]->name, m_local_evn[i]->exp);
				i++;
			}
		};

		void create_closure(list_expression* ret_fun, int index );
		expression*  bind_funcall_arg(call_recover* recover, list_expression* list);
		std::shared_ptr<expression> create_callcc();
		expression* get_call_exp(call_recover* recover);
		int match_exp(std::shared_ptr<expression> exp, call_recover* recover);
		bool do_match_list(list_expression*  exp_list,  list_expression* match_list );

		void clear_funcall_stack(){
			int end = m_index_local_evn-1;
			while(end>0 && m_local_evn[end]->call_index == m_index_funcall  ){
				if( !m_local_evn[end]->is_clear ){
					end--;
					continue;
				}


				delete m_local_evn[end];
				 m_local_evn[end] =NULL;
				 m_index_local_evn--;
				 end--;
			}

		}

private:




	std::vector<std::shared_ptr<expression>  > m_stack; //中间变量结果需要删除
	std::vector<call_recover*> m_call_hierarchy;             //ret（对应saved eip）
	std::vector<  local_ref* > m_local_evn;                       // 参数
//	每一层调用对应的中间变量，寻找由最里层开始找到global，找不到则gg
// 这里实在不想在套一层shared_ptr
	int m_index_call;
	int m_index_stack;
	int m_index_local_evn;
	int m_index_funcall;
	std::vector<  std::string > m_free_var;
	int m_index_closurecall;
	int m_free_len;
	bool m_return_lambda;
};


class callcc_expression: public  expression{
public:
	callcc_expression(eval* e,std::shared_ptr<expression> & exp):expression(expression_type::CALLCC),
	     m_eval(e),m_list_exp(exp){}
	void print(){
		std::cout<<"callcc"<<std::endl;
	}
	~callcc_expression(){

	}
	std::shared_ptr<eval>& get_eval(){
		return m_eval;
	}

	void set_eval(std::shared_ptr<eval>&  eval){
		m_eval = eval;
	}
private:
	std::shared_ptr<eval> m_eval;
	std::shared_ptr<expression> m_list_exp;

};



#endif

/*

c语言中的栈大致如下
------------------
.
.
.
.  较早的栈
------------------  1
.
.  局部变量
.

arg 1
arg 2   调用者的栈
.
.
.
arg n
ret
------------------
saved ebp 指向1处
.
.
.  局部变量
.
.
-----------------

(if (eq? num 0)
    (num -1))
 对于这样的情况Num是一个局部变量，在c中还是需要copy一份!这里拒绝考虑c艹中的引用的方式
 因为本质上hlisp和java一样全部是通过 “指针”  来寻找变量
 为了方便，此处将参数，ret和中间变量，分开，混在一起不能省一点空间，并且会导致程序更难写

从效率层面考虑，基于AST的解释本身就快不了！
 why？
 1 操作太大,如果编译为vm指令，则在vm解释指令过程中，一个指令只做很小一件事
 2 解释过程中大量的recure！即便把value_of改为循环，需要遍历AST
 3 有太多寻找变量
 4 解释中总有很高的判断这是什么玩意的成本
 5 为了支持cps变换，callcc，闭包，高阶函数，等各种东西需要付出大量额外工作
 6 截止目前似乎没有那种语言是直接通过ast解释的
 7 似乎，大概，也许，好像，可能，应该，说不定，指不准，，，ast的解释可以比较快，但是——臣妾做不到阿！

 guile也是使用vm作为执行 clojure则是jvm
 <unnamed port>:5:10: In procedure fib:
<unnamed port>:5:10: Throw to key `vm-error' with args `(vm-run "VM: Stack overflow" ())'.

Entering a new prompt.  Type `,bt' for a backtrace or `,q' to continue.

 */









//(define [atom? exp]
// (not (list? exp)))
//
//(define [take  n lot]
// (if (eq? 0 n)
//     (car lot)
//     (take (- n 1) (cdr lot))))
//
//(take 3 `(1 2 3 4))
//
//(define [cadddr exp]
// (take 3 exp))
//
//(define [add? exp]
// (and (list? exp)
//      (eq? 3 (length exp))
//      (eq? '+ (car exp))))
//
//(define [if? exp]
// (and (list? exp)
//      (eq? 4 (length exp))
//      (eq? 'if (car exp))))
//
//(define [eqq? exp]
// (and (list? exp)
//      (eq? 3 (length exp))
//      (eq? 'eqq (car exp))))
//
//(define [callccc? exp]
// (and (list? exp)
//      (eq? 2 (length exp))
//      (eq? 'callccc (car exp))))
//
//(define [fn? exp]
//
//
//(define [value_of exp]
//  (cond
//   ((atom? exp) exp)
//   ((add?  exp)
//    (+ (value_of (cadr exp)) (value_of (caddr exp)) ) )
//   ((if? exp)
//    (let ( [f_o_t (value_of (cadr exp))] )
//     (if f_o_t (value_of (caddr exp)) (value_of (cadddr exp)))))
//   ((eqq? exp)
//    (eq? (value_of (cadr exp)) (value_of (caddr exp)) ))
//   (else 'caonima)))
//
//
//(define global_k 'null)
//
//(define [value_of_cps exp stack]
//  (cond
//    ((atom? exp)
//     (apply_stack  stack exp))
//    ((add? exp)
//     (value_of_cps (cadr exp) `(add1 ,(caddr exp) ,stack)))
//    ((eqq? exp)
//     (value_of_cps (cadr exp) `(eqq1 ,(caddr exp) ,stack)))
//    ((if? exp)
//     (value_of_cps (cadr exp) `(if ,(caddr exp) ,(cadddr exp) ,stack)))
//    ((callccc? exp)
//     (begin (set! global_k stack) (apply_stack  stack (cadr exp))))
//    (else 'caonima)))
//
//(define [apply_stack stack var]
//  (cond
//    ((null? stack) var)
//    ((eq? (car stack) 'add1)
//     (value_of_cps (cadr stack) `(add2  ,var ,(caddr stack))))   ;(caddr stack) saved stack
//    ((eq? (car stack) 'add2)
//     (let ((num (+ var (cadr stack))))
//        (apply_stack (caddr stack) num)))
//    ((eq? (car stack) 'eqq1)
//     (value_of_cps (cadr stack) `(eqq2  ,var ,(caddr stack))))   ;(caddr stack) saved stack
//    ((eq? (car stack) 'eqq2)
//     (let ((res (= var (cadr stack))))
//        (apply_stack (caddr stack) res)))
//    ((eq? (car stack) 'if)
//     (if var (value_of_cps (cadr stack) (cadddr stack))
//                (value_of_cps (caddr stack) (cadddr stack))))
//    (else 'caonima)))
//
//(define [value_of_k exp]
//  (value_of_cps exp `()))
//
//
//(value_of `(if (eqq (+ 2 3) (+ 2 3)) 3 4)) ;->3
//
//(value_of_k `(if (eqq (+ 2 3) (+ 2 (callccc 3))) 3 4)) ;-> 3
//(apply_stack global_k 2) ;-> 4

//只支持if eq add的小解释器，

//第一种方式通过传统的要求值某表达式就必须先求值其参数直到
//成为不能再求值的atom为止

//第二个方式为eopl式的cps风格求值，当求值一个表达式，先求值其第一个参数
//其余部分放入栈中，因为list不区分数据和代码，事实上所有语言翻译为语法解析树都如此
//所以操作和数据可以一起塞，而等在被求值对象外的stack其实就是这一小段的 延续
// 此处简单实现scheme call/cc
// (value_of_k `(if (eqq (+ 2 3) (+ 2 (callccc 3))) 3 4)) 例如等在 callccc 3 外的就是那
//(eqq (+ 2 3) (+ 2  ？ ) 3 4)
//当求值到不可再求值的对象时我们对栈进行求值 [apply_stack stack var]
//由于在value_of_cps中我们总是将一个参数留下求值，剩余部分入栈，
//所以栈中的东西就是被扣掉一块的表达式 stack
//而 var 这是被扣掉那一块的求值结果
//当栈空了说明求值完成返回var 最终结果
//否则继续求值
//如果有需要求值的东西调用value_of_cps
//如果所有参数求值完成则调用apply_stack将栈求值


//此处的stack和c语言的stack区别在于 c语言编译后的汇编并不将操作和数据混在一起
//而是分为代码段，和栈
//栈中只用来存放需要保存的东西入某些寄存器返回地址，参数，局部变量等
//汇编中通过eip来指示需要执行的指令
//当需要call函数的时，先将参数入栈 ，再将返回地址入栈 入栈eip，将ebp变为栈底
//通过ebp找到参数，计算之，
//最后通过保存的eip返回代码出，返回值保存在eax中






























