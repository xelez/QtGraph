#ifndef QTGRAPH_H
#define QTGRAPH_H

#include <QWidget>
#include <QGraphicsScene>
#include <fplot.h>

namespace Ui {
    class QtGraph;
}

class QtGraph : public QWidget {
    Q_OBJECT
public:
    QtGraph(QWidget *parent = 0);
    ~QtGraph();
    void dbgMsgHandler(QtMsgType type, const char *msg);
    void myPopulateScene(QGraphicsScene * scene, plist & points, double width, double height);

protected:
    void changeEvent(QEvent *e);

private:
    Ui::QtGraph *ui;
    QGraphicsScene scene;
    QImage imgPlot;
    QPen coordPen;
    QPen gridPen;
    QPen funcPen;

private slots:
    void on_sliderXLimits_valueChanged(int value);
    void on_pbBuild_clicked();
};

#endif // QTGRAPH_H
