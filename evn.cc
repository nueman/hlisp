#include "evn.h"
using namespace std;


evn& evn::get_instance(){
	static evn instance;
	return instance;
}

bool evn::add_ref_exp(std::string& name,std::shared_ptr<expression>& exp ){
	bool ret = false;

	{
		std::lock_guard<mutex>gard(this->m_lock);
		auto iter = this->m_global_evn.find(name) ;
		if(iter == this->m_global_evn.end() ){
			ret=true;
			this->m_global_evn[name] = exp;
		}else{
			cout<<name<<"    already defined as: ";
		    iter->second->print();
		    cout<<endl;
		}
	}

	return ret;
}

std::shared_ptr<expression> evn::get_global_ref(string &name ){
	shared_ptr<expression> ret =NULL;
	{
		std::lock_guard<mutex>gard(this->m_lock);
		auto iter =this->m_global_evn.find(name);
		if(iter != m_global_evn.end()){
			ret =iter->second;
		}
	}
	return ret;
}






