#include <QtGui/QApplication>


#include <iostream>
#include "papara_nt/papara.h"
#include "dialog.h"


namespace papara {
log_stream lout;
}




int main( int argc, char *argv[] ) {


    QApplication a(argc, argv);
    Dialog w;
    w.show();

    return a.exec();

}
