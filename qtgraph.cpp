#include "qtgraph.h"
#include "ui_qtgraph.h"
#include "bnf.h"
#include "fplot.h"

#include <QMessageBox>
#include <QDebug>
#include <cmath>

QtGraph::QtGraph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QtGraph)
{
    ui->setupUi(this);

    //Brushes and Pens for drawing
    coordPen = QPen(QBrush(Qt::black), 2);
    gridPen  = QPen(QBrush(Qt::lightGray), 0);
    funcPen  = QPen(QBrush(Qt::blue), 1.5, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin);

    //Set scene
    ui->graphView->setScene(&scene);

    //Set edit`s validators
    ui->edXFrom->setValidator(new QDoubleValidator(ui->edXFrom));
    ui->edXTo->setValidator(new QDoubleValidator(ui->edXTo));

}

QtGraph::~QtGraph()
{
    delete ui;
}

void QtGraph::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void QtGraph::dbgMsgHandler(QtMsgType type, const char *msg)
 {
    if (type==QtDebugMsg) {
        ui->teLog->append(msg);
    } else {
        fprintf(stderr, "%s\n", msg);
    }
 }

void QtGraph::myPopulateScene(QGraphicsScene * scene, plist & points, double width, double height) {
    const double k = width / (points.back().x - points.front().x); // for transformation to scene coordinates
    const double left = points.front().x*k, right = points.back().x*k, top = -height/2, bottom = height/2;

    scene->clear();
    scene->setSceneRect(0, 0, width, height);

    imgPlot = QImage(width, height, QImage::Format_RGB32);
    imgPlot.fill(QColor(Qt::white).rgb());

    QPainter painter;
    painter.begin(&imgPlot);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.translate(-left, -top);

    //draw grid
    painter.setPen(gridPen);
    const double step = k;
    for (double x=0+step; x<right; x+=step)
        painter.drawLine(QPointF(x, top), QPointF(x, bottom));
    for (double x=0-step; x>left; x-=step)
        painter.drawLine(QPointF(x, top), QPointF(x, bottom));

    for (double y=0+step; y<bottom; y+=step) {
        painter.drawLine(QPointF(left, y), QPointF(right, y));
        painter.drawLine(QPointF(left, -y), QPointF(right, -y));
    }

    //draw coordinate system
    painter.setPen(coordPen);
    painter.drawLine(QPointF(left, 0.0), QPointF(right, 0.0)); //OX
    if (left<=0.0 && 0.0 <= right)
        painter.drawLine(QPointF(0.0, bottom), QPointF(0.0, top)); // the Y-coordinate is growing downwards

    //add ZERO
    painter.setPen(Qt::black);
    if ( (left<=0.0 && 0.0 <= right) && (top<=0.0 && 0.0 <= bottom) )
        painter.drawText(0, 0, 20, 20, Qt::AlignLeft || Qt::AlignTop, "0");

    //draw Graph
    painter.setPen(funcPen);

    bool breakp = true;
    bool bad_py = false;

    QPolygonF part;

    for (plist::iterator p = points.begin(); p!=points.end(); ++p) {
        if (isnan(p->y)) {
            breakp = true;
            continue;
        }

        double y = -k*p->y;
        bool bad_y = (y<top || y>bottom);
        if (bad_y) {
            y = (y<top) ? top : bottom;
        }

        if (breakp || (bad_y && bad_py) ) {
            breakp = false;
            painter.drawPolyline(part);
            part.clear();
        }
        part.append(QPointF(k*p->x, y));
        bad_py = bad_y;
    }
    painter.drawPolyline(part);
    part.clear();

    painter.end();
    scene->addPixmap(QPixmap::fromImage(imgPlot));
}

void QtGraph::on_pbBuild_clicked()
{
    ui->teLog->clear();

    QString func = ui->edFunc->text();
    tree *t;

    try {
        bool ok;
        double x_from = ui->edXFrom->text().toDouble(&ok);
        if (!ok) throw QString("Bad left x boundary");
        double x_to = ui->edXTo->text().toDouble(&ok);
        if (!ok) throw QString("Bad right x boundary");

        t = build_parse_tree(func);
        plist l = fplot(t, x_from, x_to, ui->graphView->width()*2);
        destroy_tree(t);

        myPopulateScene(&scene, l, ui->graphView->width(), ui->graphView->height());
        l.clear();
    } catch (QString e) {
        QMessageBox::about(NULL, "Error", "'"+e+"'");
    }
}

void QtGraph::on_sliderXLimits_valueChanged(int value)
{
    ui->edXFrom->setText(QString().number(-value));
    ui->edXTo->setText(QString().number(value));
}
