#include "reader.h"
#include "tools.hpp"
#include "parse.h"
#include <string>
#include <list>
#include <vector>
#include <memory>
#include <regex>
#include <mutex>
#include <thread>
#include <atomic>
#include <stack>

#include "fix.h"
#include "eval.h"


//
//(+ 2 (+ 2 3))
//(atom :+   int :2   (atom :+   int :2   int :3   )  )   res:  destory expression
//int :7   destory expression
//
//destory expression
//destory expression
//destory expression
//destory expression
//destory expression
//destory expression
//destory expression
//其中一共有 7个表达式
//+ 2 + 2 3 和两个List
//在eval过程中  2 2 3是求值表达式
//所以扔入栈中的中间变量是shared_from_this，clean_stack不需delete
//（+ 2 3）产生一个中间变量需要delete
//返回一个最终结果需要delete
//一共调用 7+2 = 9 次delete




void repl(){
	reader r;
	eval* e =new eval();
	while(1){
		shared_ptr<expression> a;
		a=parse::hread(r);
		a->fix();

		if(a){
			a->print();
			cout<<endl;
			shared_ptr<expression> b = e->value_of(a.get());
			cout<<" result :  ";
			b->print();
		}
		else
			cout<<"no input"<<endl;

		cout<<endl;
	}

}




int main(){
//	test();
	repl();
}



