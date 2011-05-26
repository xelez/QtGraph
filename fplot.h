#ifndef FPLOT_H
#define FPLOT_H

#include <list>
#include "bnf.h"

const double EPS_DV = 1e-8; // for derivative

double feval(tree *expr , double x);
double derivative(tree *expr, double x);

/*
 * PlotP represent a point on a graph
 *
 * dv - is for derivative
 */
struct PlotP {
    double x, y, dv;

    PlotP(double _x, double _y, double _dv) {
        x=_x; y=_y; dv=_dv;
    }

    PlotP(tree *expr, double _x) {
        x = _x; y = feval(expr, x); dv = derivative(expr, x);
    }
};

typedef std::list<PlotP> plist;

enum cont_t {CONT_FALSE, CONT_UNKNOWN, CONT_TRUE};
cont_t check_continuous(const PlotP & a, const PlotP & b);
plist fplot(tree * expr, double x_from, double x_to, int points_count);

#endif // FPLOT_H
