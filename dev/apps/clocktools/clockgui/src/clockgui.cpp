#pragma ident "$Id$"
#include <qapplication.h>
#include "mainimpl.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    MainImpl widget;
    app.setMainWidget(&widget);
    widget.show();
    
    return app.exec();
}
