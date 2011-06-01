#include "plotter.h"

Plotter::Plotter()
{
    width = height = 100;
    func = NULL;
    fromX = toX = 10;
    fromY = toY = 0;
    autoYRange = true;

    gridX = gridY = 1;
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

void Plotter::fplot() {
    const int MAX_ADD_K = 12;

    plot.clear();
    pointsK = 0;

    int points_count = width*2;
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
}

void Plotter::calculate_factors()
{
    kx = width / (toX - fromX); // for transformation to scene coordinates
    if (!autoYRange)
        ky = height / (toY - fromY);
    else {
        ky = kx;
        fromY = -height/(ky*2);
        toY = -fromY;
        qDebug("%.4lf = fromY", fromY);
    }

    left   = fromX * kx;
    right  = toX * kx;
    top    = -(toY * ky);
    bottom = -(fromY * ky);
}

void Plotter::drawGrid(QPainter *painter) {
    //draw grid
    painter->setPen(gridPen);
    const double stepx = kx * gridX;
    for (double x=0+stepx; x<right; x+=stepx)
        painter->drawLine(QPointF(x, top), QPointF(x, bottom));
    for (double x=0-stepx; x>left; x-=stepx)
        painter->drawLine(QPointF(x, top), QPointF(x, bottom));

    const double stepy = ky * gridY;
    for (double y=0+stepy; y<bottom; y+=stepy)
        painter->drawLine(QPointF(left, y), QPointF(right, y));
    for (double y=0-stepy; y>top; y-=stepy)
        painter->drawLine(QPointF(left, y), QPointF(right, y));

    //draw coordinate system
    painter->setPen(coordPen);
    if (top <= 0.0 && 0.0 <= bottom)
        painter->drawLine(QPointF(left, 0.0), QPointF(right, 0.0)); //OX
    if (left<=0.0 && 0.0 <= right)
        painter->drawLine(QPointF(0.0, bottom), QPointF(0.0, top)); // the Y-coordinate is growing downwards

    //add ZERO
    painter->setPen(Qt::black);
    if ( (left<=0.0 && 0.0 <= right) && (top<=0.0 && 0.0 <= bottom) )
        painter->drawText(0, 0, 20, 20, Qt::AlignLeft || Qt::AlignTop, "0");

}

void Plotter::doPlot(QPainter *painter)
{
    calculate_factors();

    painter->save();
    painter->translate(-left, -top);
    drawGrid(painter);

    fplot();

    //draw Graph
    painter->setPen(funcPen);

    bool breakp = true;
    bool bad_py = false;

    QPolygonF part;

    for (plist::const_iterator p = plot.begin(); p!=plot.end(); ++p) {
        if (isnan(p->y)) {
            breakp = true;
            continue;
        }

        double y = -ky*p->y;
        bool bad_y = (y<top || y>bottom);
        if (bad_y) {
            y = (y<top) ? top : bottom;
        }

        if (breakp || (bad_y && bad_py) ) {
            breakp = false;
            painter->drawPolyline(part);
            part.clear();
        }
        part.append(QPointF(kx*p->x, y));
        bad_py = bad_y;
    }
    painter->drawPolyline(part);
    part.clear();

    //For now, we don`t need it anymore... so free mem
    plot.clear();

    painter->restore();
}