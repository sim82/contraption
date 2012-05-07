#include <QtGui/QApplication>

#include <QPlainTextEdit>
#include <QScrollArea>


#include <iostream>
#include "papara_nt/papara.h"
#include "main_widget.h"
#include "TextGrid.h"


class TestModel : public TextGridModel {
public:
    virtual QSize size() {
        return QSize( 2000, 2000 );
    };
    virtual QChar data( size_t x, size_t y ) {
        return 'A';
    }
    virtual QColor color( size_t x, size_t y ) {
        return Qt::red;
        
    };
};

namespace papara {
log_stream lout;
}


QString random_text() {
    QString os;
    
    QChar temp[4] = {'A','C','G','T'};
    for( size_t i = 0; i < 2000; ++i ) {
        for(size_t j = 0; j < 2000; ++j ) {
            os.append( temp[(i+j)%4] );
            
        }
        
        os.append('\n');
    }
    
    return os;
}


class MyWidget : public QWidget {
protected:
    void paintEvent( QPaintEvent * e ) {
        QRect r = e->rect();
        
        std::cout << r.x() << " " << r.y() <<  " " << r.width() << " " << r.height() << "\n";
        
    }
private:
    
};

int main( int argc, char *argv[] ) {


    QApplication a(argc, argv);
    MainWidget w;

    TestModel tm;
    
    TextGrid *mw = new TextGrid;
    mw->setModel( &tm );
    QScrollArea sa;
    sa.setWidget(mw);
    
//     QScrollArea sa;
    sa.resize(300,200);
    
    
    sa.show();
    
    w.show();

    w.post_show_stuff();
    
    return a.exec();

}
