#include <QtGui/QApplication>
#include "large_ali.h"

int main( int argc, char *argv[] ) {


    QApplication a(argc, argv);
    LargeAli w;

//     if(false) {
//         TestModel tm;
//         
//         TextGrid *mw = new TextGrid;
//         mw->setModel( &tm );
//         QScrollArea sa;
//         sa.setWidget(mw);
//         
//         //     QScrollArea sa;
//         sa.resize(300,200);
//         
//         
//         sa.show();
//     }
    
    w.show();

    
    
    return a.exec();

}
