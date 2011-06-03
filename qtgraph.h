#ifndef QTGRAPH_H
#define QTGRAPH_H

#include <QWidget>
#include <QGraphicsScene>

#include "plotter.h"

namespace Ui {
    class QtGraph;
}

class QtGraph : public QWidget {
    Q_OBJECT
public:
    QtGraph(QWidget *parent = 0);
    ~QtGraph();
    void dbgMsgHandler(QtMsgType type, const char *msg);
    void setPlotterSettings(tree *func);

protected:
    void changeEvent(QEvent *e);
    void setWorking(bool b);

private:
    Ui::QtGraph *ui;
    QGraphicsScene scene;

    bool working;
    tree * func;
    Plotter plotter;

public slots:
    void build();
    void buildEnd();

private slots:
    void on_pbSave_clicked();
    void on_pbBuild_clicked();
};

#endif // QTGRAPH_H
