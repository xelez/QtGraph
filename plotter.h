#ifndef PLOTTER_H
#define PLOTTER_H

#include <list>
#include <cmath>

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

enum cont_t {CONT_UNDEFINED, CONT_FALSE, CONT_UNKNOWN, CONT_TRUE};
cont_t check_continuous(const PlotP & a, const PlotP & b);

class Plotter
{
public:
    Plotter(tree * _func, double _width, double _height);

    void setXRange(double from, double to) {
        fromX = from; toX = to;
    }

    void setYRange(double from, double to) {
        fromY = from; toY = to;
    }

    void fplot_insert_points(const plist::iterator & a, const plist::iterator & b, int k );
    plist fplot(int points_count);

private:
    tree * func;
    double width, height;
    double fromX, toX;
    double fromY, toY;
    double kx, ky;

    bool autoYRange;
    plist plot;
    int pointsK;
};

#endif // PLOTTER_H
