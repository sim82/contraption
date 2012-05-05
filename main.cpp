#include <QtGui/QApplication>


#include <iostream>
#include "papara_nt/papara.h"
#include "main_widget.h"


namespace papara {
log_stream lout;
}




int main( int argc, char *argv[] ) {


    QApplication a(argc, argv);
    MainWidget w;
    w.show();

    w.post_show_stuff();
    
    return a.exec();

}
