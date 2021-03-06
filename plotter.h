#ifndef PLOTTER_H
#define PLOTTER_H

#include <QProgressBar>
#include <QPainter>
#include <QPen>
#include <list>
#include <cmath>

#include "bnf.h"

#define isnan(v) ((v)!=(v))

const double EPS_DV = 1e-8; // for derivative

/*
 * MAX_INSERT_K
 * we can insert 2^CONST points in the worst case
 */
const int MAX_INSERT_K = 12;

double feval(tree *expr , double x);
double derivative(tree *expr, double x);

/*
 * PlotP represent a point on a graph
 *
 * dv - is for derivative
 */
struct PlotP {
    double x, y, dv;

    PlotP() { x = y = dv = NaN; }

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

    void setFunc(tree * f) { func = f; }
    void setSize(double w, double h) { width = w; height = h; }
    void setXRange(double from, double to) { fromX = from; toX = to; }
    void setYRange(double from, double to) { fromY = from; toY = to; autoYRange = false; }
    void setAutoYRange() { autoYRange = true; }
    void setGrid(double x, double y) { gridX = x; gridY = y; }
    void setProgressBar(QProgressBar *bar) { progressBar = bar; }
    void setFindZeros(bool b) { findZeros = b; }

    void doPlot();
    void abort() { needAbort=true; }

    QPen gridPen, coordPen, funcPen;
    QColor backgroundColor;

    QImage imgPlot;

protected:
    void insert_points(const plist::iterator & a, const plist::iterator & b, int k );
    void calculate_factors();
    void drawGrid();

private:
    void plotBegin();
    void plotPoint(const PlotP & p);
    void plotEnd();

    void zerosPoint(const PlotP & p);

    QProgressBar *progressBar;
    tree * func;
    double width, height;
    double fromX, toX;
    double fromY, toY;
    bool autoYRange;

    double gridX, gridY;
    double kx, ky, kdx;

    double left, right, top, bottom;

    bool needAbort;
    plist plot;
    int pointsK;

    //For finding zeros
    bool findZeros;
    PlotP prevP;

    //for plotBegin, plotPoint, plotEnd
    bool breakp;
    bool bad_py;
    QPolygonF part;
    QPainter *painter;
};

#endif // PLOTTER_H
