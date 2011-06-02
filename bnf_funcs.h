#ifndef BNF_FUNCS_H
#define BNF_FUNCS_H

#include <map>
#include <QString>
#include "bnf.h"

extern std::map<QString, opfunc> bnf_funcs;
extern std::map<QString, double> bnf_consts;

void bnf_funcs_init();
void bnf_consts_init();

#endif // BNF_FUNCS_H
