#ifndef BNF_H
#define BNF_H

#include <QString>

const double NaN = 0.0/0.0; //Not a Number
const double EPS = 1e-10; // for comparing with null

struct tree {
    QChar type; //'n'==number, 'x'==x, '+','-','*','/','^'==соответственно, 'f' - function
    double value;
    void *f;
    tree *op1, *op2;

    tree () {
        type = 'n'; value = 0.0; f = op1 = op2 = NULL;
    }

    tree (QChar _type, tree * _op1, tree * _op2) {
        type = _type; value = 0.0; f = NULL;
        op1 = _op1; op2 = _op2;
    }

    tree (double val) {
        type = 'n'; value = val;
        f = op1 = op2 = NULL;
    }

    tree (QChar _type) {
        type = _type; value = 0.0;
        f = op1 = op2 = NULL;
    }
};

typedef double (*opfunc)(tree*);

void error(QString s) throw(QString);
QString peek_lexem(QString & str);
tree * build_parse_tree(QString str);
void destroy_tree(tree * t);

void eval(tree *t, double x);

#endif // BNF_H
