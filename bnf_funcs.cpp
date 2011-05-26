#include <bnf.h>
#include <bnf_funcs.h>
#include <cmath>

std::map<QString, opfunc> bnf_funcs;

double bnf_sin(tree * t) {
    return sin(t->op1->value);
}

double bnf_cos(tree * t) {
    return cos(t->op1->value);
}

double bnf_tg(tree * t) {
    if (fabs(cos(t->op1->value)) < EPS) return NaN;
    return tan(t->op1->value);
}

double bnf_ctg(tree * t) {
    double tmp = sin(t->op1->value);
    if (fabs(tmp) < EPS) return NaN;
    return cos(t->op1->value) / tmp;
}

double bnf_floor(tree * t) {
    return floor(t->op1->value);
}

double bnf_sqrt(tree * t) {
    return sqrt(t->op1->value);
}

double bnf_arcsin(tree *t) {
    return asin(t->op1->value);
}

double bnf_arccos(tree *t) {
    return acos(t->op1->value);
}

double bnf_arctg(tree *t) {
    return atan(t->op1->value);
}

void bnf_funcs_init() {
    bnf_funcs["sin"]    = bnf_sin;
    bnf_funcs["cos"]    = bnf_cos;
    bnf_funcs["tg"]     = bnf_tg;
    bnf_funcs["ctg"]    = bnf_ctg;
    bnf_funcs["floor"]  = bnf_floor;
    bnf_funcs["sqrt"]   = bnf_sqrt;
    bnf_funcs["arcsin"] = bnf_arcsin;
    bnf_funcs["arccos"] = bnf_arccos;
    bnf_funcs["arctg"] = bnf_arctg;
}
