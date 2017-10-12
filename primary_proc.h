#ifndef ____PRIMARY__PROC___
#define ____PRIMARY__PROC___

#include "expression.h"
#include "evn.h"
#include "eval.h"
#include <map>
#include <functional>

struct call_recover;
struct local_ref;

class primary_proc{
public:
	static primary_proc& get_instance(){
		static primary_proc instance;
		return instance;
	}

	std::function<std::shared_ptr<expression> (call_recover* recover, std::vector<std::shared_ptr<expression>  >&) > &
			get_fun(int name){
		return m_proc_map[name];
	}


private:
	primary_proc();
	std::map<int,std::function<std::shared_ptr<expression> (call_recover* recover , std::vector<std::shared_ptr<expression>  >&) >  >m_proc_map;

};



//primary_proc& get_instance(){
//	static primary_proc instance;
//	return instance;
//}
//
//
//private:
//primary_proc();

#endif
