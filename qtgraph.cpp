#include "qtgraph.h"
#include "ui_qtgraph.h"
#include "bnf.h"
#include "plotter.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <cmath>

QtGraph::QtGraph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QtGraph)
{
    ui->setupUi(this);

    //Plotter config
    plotter.setProgressBar(ui->progressBar);
    plotter.coordPen = QPen(QBrush(Qt::black), 2);
    plotter.gridPen  = QPen(QBrush(Qt::lightGray), 0);
    plotter.funcPen  = QPen(QBrush(Qt::blue), 1.5, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin);
    plotter.backgroundColor = Qt::white;

    //Set up color pickers
    ui->cpPlot->setColor(Qt::blue);
    ui->cpGrid->setColor(Qt::lightGray);
    ui->cpAxes->setColor(Qt::black);
    ui->cpBackground->setColor(Qt::white);

    //Set scene
    ui->graphView->setScene(&scene);

    //set state
    ui->progressBar->setVisible(false);
    working = false;
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

void QtGraph::setPlotterSettings(tree *func) {
    plotter.setFunc(func);

    plotter.setSize(ui->graphView->width(), ui->graphView->height());
    plotter.setXRange(ui->limitsX->limitFrom(), ui->limitsX->limitTo());
    plotter.setAutoYRange();
    if (ui->limitsY->isChecked())
        plotter.setYRange(ui->limitsY->limitFrom(), ui->limitsY->limitTo());

    plotter.setGrid(ui->sbGridX->value(), ui->sbGridY->value());

    //colors
    plotter.gridPen.setColor(ui->cpGrid->color());
    plotter.coordPen.setColor(ui->cpAxes->color());
    plotter.funcPen.setColor(ui->cpPlot->color());
    plotter.backgroundColor = ui->cpBackground->color();
}

void QtGraph::setWorking(bool b) {
    if (b==working) return;
    if (b) {
        working = true;
        ui->pbBuild->setText("Abort");
        ui->tabView->setEnabled(false);
        ui->edFunc->hide();
        ui->progressBar->show();
    } else {
        working = false;
        ui->pbBuild->setText("Build");
        ui->tabView->setEnabled(true);
        ui->progressBar->hide();
        ui->edFunc->show();
    }
}

void QtGraph::build() {
    ui->teLog->clear();

    const QString str = ui->edFunc->text();
    try {
        if (!ui->limitsX->isValid()) throw QString("Bad X boundaries");
        if (ui->limitsY->isChecked() && !ui->limitsY->isValid()) throw QString("Bad Y boundaries");

        func = build_parse_tree(str);
        setPlotterSettings(func);

        setWorking(true);
        plotter.doPlot();

        buildEnd();
    } catch (QString e) {
        QMessageBox::about(NULL, "Error", e);
    }
}

void QtGraph::buildEnd() {
    scene.clear();
    scene.setSceneRect(0, 0, plotter.imgPlot.width(), plotter.imgPlot.height());
    scene.addPixmap(QPixmap::fromImage(plotter.imgPlot));

    destroy_tree(func);

    setWorking(false);
}

void QtGraph::on_pbBuild_clicked()
{
    if (!working) {
        build();
    } else {
        plotter.abort();
    }
}

void QtGraph::on_pbSave_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save plot"), "",
                       tr("JPEG (*.jpeg, *.jpg);;PNG (*.png);;BMP (*.bmp);;All Files (*)"));

    if (!fileName.isEmpty())
        if (!plotter.imgPlot.save(fileName))
            QMessageBox::warning(this, "Error", "Can`t save file \"" + fileName +"\"");
}
