#include "bnf.h"
#include "fplot.h"
#include <cmath>

inline double feval(tree *expr , double x) {
    if (!expr) return NaN;
    eval(expr, x);
    return expr->value;
}

inline double derivative(tree *expr, double x) {
    return (feval(expr, x+EPS_DV) - feval(expr, x - EPS_DV)) / (2*EPS_DV);
}

/*
 * check_continuous(A, B)
 *
 */
cont_t check_continuous(const PlotP & a, const PlotP & b) {
    const double DV_K1 = 5.0/6.0;
    const double DV_K2 = 1.0 / DV_K1;

    if ( (isnan(a.y) && isnan(b.y)) || (isnan(a.dv) && isnan(b.dv)) )
        return CONT_UNDEFINED;

    if (isnan(a.y) || isnan(b.y) || isnan(a.dv) || isnan(b.dv))
        return CONT_FALSE;

    double dy = b.y - a.y;
    double py = (b.x-a.x) * a.dv; //prediction

    if (dy*py<0) { // if sign is different;
        return CONT_UNKNOWN;
    } else {
        // (DV_K1 <= |py/dy| <= (DV_K2)
        if ( (DV_K1*fabs(dy) <= fabs(py)) && ( fabs(py) <= DV_K2*fabs(dy)) )
            return CONT_TRUE;
        else
            return CONT_UNKNOWN;
    }
}
int fplot_insert_points(tree* expr, plist & plot, const plist::iterator & a, const plist::iterator & b, int k ) {
    double tx = (a->x + b->x) / 2;
    const cont_t cont = check_continuous(*a, *b);

    if (k==0) {
        if ( (!isnan(a->y)) && (!isnan(b->y)) && cont!=CONT_TRUE) {
            plot.insert(b, PlotP(tx, NaN, NaN));
            return 1;
        }
        return 0;
    }

    if ( (a->dv * b->dv < 0.0) || cont==CONT_FALSE || cont==CONT_UNKNOWN ) {
        plist::iterator t = plot.insert(b, PlotP(expr, tx));
        return fplot_insert_points(expr, plot, a, t, k-1)
               + fplot_insert_points(expr, plot, t, b, k-1)
               + 1;
    }
    return 0;
}

plist fplot(tree * expr, double x_from, double x_to, int points_count) {
    const int MAX_ADD_K = 13;

    plist plot;
    double dx = (x_to - x_from) / points_count;

    int pk=1;
    // add first point
    plot.push_back(PlotP(expr, x_from));

    for (double x=x_from+dx; x<=x_to; x+=dx) {
        plist::iterator prev = --plot.end();
        plist::iterator p = plot.insert(plot.end(), PlotP(expr, x));

        pk += fplot_insert_points(expr, plot, prev, p, MAX_ADD_K) + 1;
    }

    qDebug("Totaly added %d points", pk);

    return plot;
}
