#ifndef PLOTTER_H
#define PLOTTER_H

#include <QPainter>
#include <QPen>
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
    Plotter();

    void setFunc(tree * f) { // temp
        func = f;
    }

    void setSize(double w, double h) {
        width = w; height = h;
    }

    void setXRange(double from, double to) {
        fromX = from; toX = to;
    }

    void setYRange(double from, double to) {
        fromY = from; toY = to;
        autoYRange = false;
    }

    void setAutoYRange() {
        autoYRange = true;
    }

    void setGrid(double x, double y) {
        gridX = x; gridY = y;
    }

    void doPlot(QPainter * painter);

    QPen gridPen, coordPen, funcPen;

protected:
    void fplot_insert_points(const plist::iterator & a, const plist::iterator & b, int k );
    void fplot();
    void calculate_factors();
    void drawGrid(QPainter *painter);

private:
    void plotBegin();
    void plotPoint(const PlotP & p);
    void plotEnd();

    tree * func;
    double width, height;
    double fromX, toX;
    double fromY, toY;

    double gridX, gridY;
    double kx, ky, kdx;

    double left, right, top, bottom;

    bool autoYRange;

    plist plot;
    int pointsK;

    //for plotBegin, plotPoint, plotEnd
    bool breakp;
    bool bad_py;
    QPolygonF part;
    QPainter *painter;
};

#endif // PLOTTER_H
