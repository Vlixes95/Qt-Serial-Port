#include <QApplication>
#include "serialportWidget.h"

/*
 * TODO: Add drive size
 * TODO: Add free bytes of the drive
 */

int main ( int argc, char *argv[] ) {

    QApplication a( argc, argv );

    SerialPortWidget widget;
    widget.show();

    return QApplication::exec( );

}
