#ifndef __TextGrid_h
#define __TextGrid_h


#include <QWidget>


class TextGridModel {
  
public:
    virtual QSize size() = 0;
    virtual QChar data( size_t x, size_t y ) = 0;
    virtual QColor color( size_t x, size_t y ) = 0;
    
};

class TextGrid : public QWidget {
public:
    TextGrid( QWidget * w = 0 ) ; 
    
    void setModel( TextGridModel *model ) ;
    
    void paintEvent( QPaintEvent *e ) ;
private:
    
    TextGridModel *model_;
    QSize model_size_;
    QSize cell_size_;
};


#endif