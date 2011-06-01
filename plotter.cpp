#include "plotter.h"

Plotter::Plotter(tree *_func, double _width, double _height) :
        func(_func), width(_width), height(_height), autoYRange(true)
{
}

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

void Plotter::fplot_insert_points(const plist::iterator &a, const plist::iterator &b, int k) {
    double tx = (a->x + b->x) / 2;
    const cont_t cont = check_continuous(*a, *b);

    if (k==0) {
        if ( (!isnan(a->y)) && (!isnan(b->y)) && cont!=CONT_TRUE) {
            plot.insert(b, PlotP(tx, NaN, NaN));
            ++pointsK;
        }
        return;
    }

    if ( (a->dv * b->dv < 0.0) || cont==CONT_FALSE || cont==CONT_UNKNOWN ) {
        plist::iterator t = plot.insert(b, PlotP(func, tx));
        fplot_insert_points(a, t, k-1);
        fplot_insert_points(t, b, k-1);
        pointsK += 2;
    }
}

plist Plotter::fplot(int points_count) {
    const int MAX_ADD_K = 12;

    plot.clear();
    pointsK = 0;

    double dx = (toX - fromX) / points_count;

    // add first point
    plot.push_back(PlotP(func, fromX));

    for (double x = fromX + dx; x <= toX; x += dx) {
        plist::iterator prev = --plot.end();
        plist::iterator p = plot.insert(plot.end(), PlotP(func, x));

        fplot_insert_points(prev, p, MAX_ADD_K);
        ++pointsK;
    }

    qDebug("Totaly added %d points", pointsK);

    return plot;
}

