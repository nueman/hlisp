#ifndef ___PARSE___
#define ___PARSE___
#include "reader.h"
#include "expression.h"
#include <regex>
#include <string>
#include "fix.h"

typedef enum{
	var,
	lp, //left parentheses
	rp,//right parentheses
	num,
	qoute,
	unqoute,
	hstring,
}token_type;


class parse{
public:
	static std::shared_ptr<expression> hread(reader& reader,list_expression* end=NULL);
	static std::shared_ptr<expression> match_and_create_token(char *token);
	static std::shared_ptr<expression> create_list();

};



















#endif
