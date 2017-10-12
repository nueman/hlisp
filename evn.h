#ifndef ___EVN___
#define ___EVN___
#include "expression.h"
#include <map>
#include <mutex>



struct local_ref;

class expression;
class evn{
public:
	static evn& get_instance();
	std::shared_ptr<expression> get_global_ref(std::string &name);
	bool add_ref_exp(std::string& name,std::shared_ptr<expression>& exp );

private:
	evn(){};
	std::map<std::string, std::shared_ptr<expression> > m_global_evn;
	std::mutex m_lock;

};






#endif
