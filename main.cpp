#include <QtGui/QApplication>
#include <QPointer>
#include "qtgraph.h"
#include "bnf_funcs.h"

QPointer<QtGraph> w;

void myMessageOutput(QtMsgType type, const char *msg) {
    if (w)
     w->dbgMsgHandler(type, msg);
}

int main(int argc, char *argv[])
{
    bnf_funcs_init();
    QApplication a(argc, argv);
    w = new QtGraph;
    qInstallMsgHandler(myMessageOutput);

    w->show();
    return a.exec();
}
