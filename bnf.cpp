#include <QString>
#include <cmath>
#include "bnf.h"
#include "bnf_funcs.h"

#define is_op(a) ( (a)=='(' || (a)==')' || (a)=='+' || (a)=='-' || (a)=='*' || (a)=='/' || (a)=='^' )

QString peek_lexem(QString & str) {
    int pos = 0;
    while (pos < str.length() && str[pos].isSpace()) ++pos;
    str.remove(0, pos);

    if (str.isEmpty()) return "";

    pos = 0;
    if ( is_op(str[0]) ) return QString(str[0]);

    while ( pos < str.length() && (str[pos].isLetterOrNumber() || str[pos]=='.') )
        ++pos;

    return str.left(pos);
}

QString pop_lexem(QString & str) {
    QString lexem = peek_lexem(str);
    str.remove(0, lexem.length());
    return lexem;
}

/*
 * BNF-like
 *
 * <expr1> ::= <expr1> + <expr2> | <expr1> - <expr2> | <expr2>
 * <expr2> ::= <expr2> * <expr3> | <expr2> / <expr3> | <expr3>
 * <expr3> ::= <expr4> ^ <expr3> | <expr4>
 * <expr4> ::= x | <number> | -<expr4> | +<expr4> | <func_name>(<expr1>) | (<expr1>)
 */

void error(QString s) throw(QString) {
    throw s;
}

tree* parse1(QString & str);
tree* parse2(QString & str);
tree* parse3(QString & str);
tree* parse4(QString & str);

tree* parse1(QString & str) {
    tree *t;
    QString op = peek_lexem(str);
    t = parse2(str);

    while (1) {
        op = peek_lexem(str);
        if (op==")" || op.isEmpty()) break;
        if (op!="+" && op!="-") error("1");
        pop_lexem(str);

        tree *r = parse2(str);
        t = new tree(op[0], t, r);
    }

    return t;
}

tree* parse2(QString & str) {
    tree *t;
    t = parse3(str);

    while (1) {
        QString op = peek_lexem(str);
        if (op==")" || op.isEmpty()) break;
        if (op=="+" || op=="-") break;
        if (op!="*" && op!="/") error("2");
        pop_lexem(str);

        tree *r = parse3(str);
        t = new tree(op[0], t, r);
    }

    return t;
}

tree* parse3(QString & str) {
    tree *t;
    t = parse4(str);

    QString op = peek_lexem(str);
    if (op==")" || op.isEmpty()) return t;
    if (op=="+" || op=="-" || op=="/" || op=="*") return t;
    if (op!="^") error("3");
    pop_lexem(str);

    tree *r = parse3(str);
    return new tree(op[0], t, r);
}

tree* parse4(QString & str) {
    QString lex = pop_lexem(str);

    if (lex=="-" || lex=="+") {
        return new tree(lex[0], parse4(str), NULL);
    }

    if (lex=="x") {
        return new tree(QChar('x'));
    }

    if (lex=="(") {
        tree * t = parse1(str);
        if (pop_lexem(str)!=")") error(") Expected");
        return t;
    }

    bool ok;
    double val = lex.toDouble(&ok);
    if (ok) {
        return new tree(val);
    }

    if (bnf_funcs.find(lex)!=bnf_funcs.end()) {
        if (pop_lexem(str)!="(") error("( Expected");
        tree * p = parse1(str);
        if (pop_lexem(str)!=")") error(") Expected");
        tree * t = new tree(QChar('f'));
        t->f = (void*) bnf_funcs[lex];
        t->op1 = p;
        return t;
    }

    error(lex);
    return NULL;
}


tree * build_parse_tree(QString str) {
    str = str.toLower();
    tree * t = parse1(str);
    if (!str.isEmpty()) throw str;
    return t;
}

void destroy_tree(tree * t) {
    if (!t) return;
    if (t->op1) destroy_tree(t->op1);
    if (t->op2) destroy_tree(t->op2);
    delete t;
}

void eval(tree *t, double x) {
    if (!t) return;
    if (t->op1) eval(t->op1, x);
    if (t->op2) eval(t->op2, x);

    switch (t->type.toAscii()) {
    case 'n':
        break;
    case 'x':
        t->value = x;
        break;
    case 'f':
        t->value = opfunc(t->f)(t);
        break;
    case '-':
        if (t->op2)
            t->value = t->op1->value - t->op2->value;
        else
            t->value = - t->op1->value;
        break;
    case '+':
        t->value = ( (t->op1)?(t->op1->value):(0) ) + ( (t->op2)?(t->op2->value):(0) );
        break;
    case '*':
        t->value = (t->op1->value) * (t->op2->value);
        break;
    case '/':
        if (fabs(t->op2->value) < EPS )
            t->value = NaN;
        else
            t->value = (t->op1->value) / (t->op2->value);
        break;
    case '^':
        t->value = pow(t->op1->value, t->op2->value);
        break;

    }
}
