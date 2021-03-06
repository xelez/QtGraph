#include <bnf.h>
#include <bnf_funcs.h>
#include <cmath>

std::map<QString, opfunc> bnf_funcs;
std::map<QString, double> bnf_consts;

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

double bnf_abs(tree *t) {
    return fabs(t->op1->value);
}

double bnf_boobs(tree *t) { //just for fun =)
 const double x = t->op1->value;
 return 0.7
        * (3.2 - fabs((fabs(1.8-fabs(0.49*x))-fabs(0.46*x)+1.9)-1.1) + fabs(1.9-fabs(0.55*x)) - fabs(0.52*x))
        * (0.5*fabs(sin(2*(fabs(x)-0.8))-0.9) + 0.5*sin(1.9*(fabs(x)-0.78)) + fabs(sin(fabs(x))));
}

void bnf_funcs_init() {
    bnf_consts_init();

    bnf_funcs["abs"]    = bnf_abs;
    bnf_funcs["sin"]    = bnf_sin;
    bnf_funcs["cos"]    = bnf_cos;
    bnf_funcs["tan"]    = bnf_tg;
    bnf_funcs["tg"]     = bnf_tg;
    bnf_funcs["ctan"]   = bnf_ctg;
    bnf_funcs["ctg"]    = bnf_ctg;
    bnf_funcs["floor"]  = bnf_floor;
    bnf_funcs["sqrt"]   = bnf_sqrt;
    bnf_funcs["arcsin"] = bnf_arcsin;
    bnf_funcs["arccos"] = bnf_arccos;
    bnf_funcs["arctan"] = bnf_arctg;
    bnf_funcs["arctg"]  = bnf_arctg;
    bnf_funcs["boobs"]  = bnf_boobs;
}

void bnf_consts_init() {
    bnf_consts["pi"] = 3.1415926535897932384626433832795;
}
