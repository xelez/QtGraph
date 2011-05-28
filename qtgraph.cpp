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
    double kx = width / (points.back().x - points.front().x); // for transformation to scene coordinates
    const double left = points.front().x * kx, right = points.back().x * kx;

    double ky, top, bottom;
    if (ui->limitsY->isChecked()) {
        ky = height / (ui->limitsY->limitTo() - ui->limitsY->limitFrom());
        top = -(ui->limitsY->limitTo() * ky);
        bottom = -(ui->limitsY->limitFrom() * ky);
    } else {
        ky = kx;
        top = -height/2;
        bottom = height/2;
    }

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
    const double stepx = kx;
    for (double x=0+stepx; x<right; x+=stepx)
        painter.drawLine(QPointF(x, top), QPointF(x, bottom));
    for (double x=0-stepx; x>left; x-=stepx)
        painter.drawLine(QPointF(x, top), QPointF(x, bottom));

    const double stepy = ky;
    for (double y=0+stepy; y<bottom; y+=stepy)
        painter.drawLine(QPointF(left, y), QPointF(right, y));
    for (double y=0-stepy; y>top; y-=stepy)
        painter.drawLine(QPointF(left, y), QPointF(right, y));


    //draw coordinate system
    painter.setPen(coordPen);
    if (top <= 0.0 && 0.0 <= bottom)
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

        double y = -ky*p->y;
        bool bad_y = (y<top || y>bottom);
        if (bad_y) {
            y = (y<top) ? top : bottom;
        }

        if (breakp || (bad_y && bad_py) ) {
            breakp = false;
            painter.drawPolyline(part);
            part.clear();
        }
        part.append(QPointF(kx*p->x, y));
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
        if (!ui->limitsX->isValid()) throw QString("Bad X boundaries");
        if (ui->limitsY->isChecked() && (!ui->limitsY->isValid())) throw QString("Bad Y boundaries");

        t = build_parse_tree(func);
        plist l = fplot(t, ui->limitsX->limitFrom(), ui->limitsX->limitTo(), ui->graphView->width()*2);
        destroy_tree(t);

        myPopulateScene(&scene, l, ui->graphView->width(), ui->graphView->height());
        l.clear();
    } catch (QString e) {
        QMessageBox::about(NULL, "Error", "'"+e+"'");
    }
}
