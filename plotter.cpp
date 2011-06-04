#include "plotter.h"
#include <QTime>
#include <QCoreApplication>

Plotter::Plotter()
{
    width = height = 100;
    func = NULL;
    fromX = toX = 10;
    fromY = toY = 0;
    autoYRange = true;
    findZeros = false;

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

void Plotter::insert_points(const plist::iterator &a, const plist::iterator &b, int k) {
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
        insert_points(a, t, k-1);
        insert_points(t, b, k-1);
        ++pointsK;
    }
}

void Plotter::calculate_factors()
{
    kdx = (toX - fromX) / (2*width); // for optimized drawing
    kx = width / (toX - fromX); // for transformation to scene coordinates
    if (!autoYRange)
        ky = height / (toY - fromY);
    else {
        ky = kx;
        fromY = -height/(ky*2);
        toY = -fromY;
    }

    left   = fromX * kx;
    right  = toX * kx;
    top    = -(toY * ky);
    bottom = -(fromY * ky);
}

void Plotter::drawGrid() {
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

void Plotter::plotBegin() {
    painter->setPen(funcPen);

    breakp = true;
    bad_py = false;

    part.clear();
    part.reserve(width*2);
}

void Plotter::plotPoint(const PlotP &p) {
    if (isnan(p.y)) {
        breakp = true;
        return;
    }

    double y = -ky*p.y;
    bool bad_y = (y<top || y>bottom);
    if (bad_y) {
        y = (y<top) ? top : bottom;
    }

    if (breakp || (bad_y && bad_py) ) {
        breakp = false;
        painter->drawPolyline(part);
        part.clear();
    }

    const QPointF t3(kx*p.x, y);

    if (part.count() > 1) {
        const QPointF & t1 = *(part.constEnd()-2);
        const QPointF & t2 = *(part.constEnd()-1);
        if ( ( t3.x()-t1.x() <= kdx ) && ( (t2.y()-t1.y())*(t3.y()-t2.y()) >= 0.0 ) ) {
            part.pop_back();
        }
    }
    part.append(t3);

    bad_py = bad_y;
}

void Plotter::plotEnd() {
    painter->drawPolyline(part);
    part.clear();
}


void Plotter::zerosPoint(const PlotP &p) {
    if (isnan(p.y)) {
        prevP = p;
        return;
    }

    if (fabs(p.y)<EPS) {
        qDebug("{%.6lf} - zero", p.x);
    }
    else if (!isnan(prevP.y) && (prevP.y*p.y < 0) ) {
        qDebug("[%.6lf, %.6lf] - one or more zeros ", prevP.x, p.x);
    }
    prevP = p;
}

void Plotter::doPlot()
{
    // measure the time for plotting
    QTime timer;
    timer.start();

    // setup settings and clear old stuff
    needAbort = false;
    calculate_factors();
    prevP = PlotP();
    plot.clear();
    pointsK = 0;

    // Calculations
    const int points_count = width*2;
    const double dx = (toX - fromX) / points_count;

    // setup progressBar
    progressBar->setMinimum(0);
    progressBar->setMaximum(points_count);
    progressBar->reset();

    // setup image and painter
    imgPlot = QImage(width, height, QImage::Format_RGB32);
    imgPlot.fill(backgroundColor.rgb());

    QPainter mypainter;
    painter = &mypainter;
    painter->begin(&imgPlot);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->translate(-left, -top);

    drawGrid();

    // setup plot drawing
    plotBegin();

    // add first point
    plot.push_back(PlotP(func, fromX));

    int i=0;
    for (double x = fromX + dx; x <= toX; x += dx) {
        if (needAbort) break;
        //Don`t freeze the user interface
        QCoreApplication::processEvents();

        plist::iterator prev = --plot.end();
        plist::iterator p = plot.insert(plot.end(), PlotP(func, x));
        ++pointsK;

        insert_points(prev, p, MAX_INSERT_K);
        for (plist::const_iterator t = plot.begin(); t!=p; ++t) {
            plotPoint(*t);
            if (findZeros) zerosPoint(*t);
        }
        plot.erase(plot.begin(), p);

        ++i; progressBar->setValue(i);
    }

    for (plist::const_iterator p = plot.begin(); p!=plot.end(); ++p) {
        plotPoint(*p);
    }
    plot.clear();

    // end of plotting
    plotEnd();

    painter->end();

    qDebug("Totaly added %d points", pointsK);
    qDebug("Time: %d ms", timer.elapsed());
}
