#ifndef ____FIX__EXPRESSION___
#define ____FIX__EXPRESSION___

#include "expression.h"
#include "evn.h"
#include <functional>
#include <regex>
#include <assert.h>



#define comperro(x)  fprintf(stderr,"comp error:  ");fprintf(stderr, x); exit(0);

using namespace std;
class fix_expression{
public:


	 static fix_expression& get_instance();
	 std::list<std::string>& get_proc_name(){
		 return proc_name;
	 }

	 static  void do_fix_ref_funcall(list_expression* call);
	 static  void do_fix_lambda_funcall(list_expression* call);
	 std::list< function<void  ( list_expression*   ) > >&  get_fix_func(){
		 return proc_fix_fun;
	 }

private:
	 fix_expression ( );

	 std::list<std::string> proc_name;
	 std::list< function<void  ( list_expression*   ) > > proc_fix_fun;

};

#endif
