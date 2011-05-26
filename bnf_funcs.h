#ifndef BNF_FUNCS_H
#define BNF_FUNCS_H

#include <map>
#include <QString>
#include "bnf.h"

extern std::map<QString, opfunc> bnf_funcs;

void bnf_funcs_init();

#endif // BNF_FUNCS_H
