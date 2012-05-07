#include <QPainter>
#include <QPaintEvent>
#include <QStaticText>
#include "TextGrid.h"



TextGrid::TextGrid(QWidget* w) : QWidget(w), model_(0), cell_size_(12,16) {}


void TextGrid::paintEvent(QPaintEvent* e) {
    QRect r = e->rect();
    
    QPainter p(this);
    
    int yfirst = r.top() / cell_size_.height(); 
    int ylast = r.bottom() / cell_size_.height(); 
    
    int xfirst = r.left() / cell_size_.width(); 
    int xlast = r.right() / cell_size_.width();
    
    QStaticText qst("A");
    for( int y = yfirst; y <= ylast; ++y ) {
        for( int x = xfirst; x <= xlast; ++x ) {
            QRect dr(x * cell_size_.width(), y * cell_size_.height(), cell_size_.width(), cell_size_.height());
                    
            p.drawRect(dr);
            p.drawStaticText(dr.topLeft(), qst);
            
        }
    }
    
    
    
}
void TextGrid::setModel(TextGridModel* model) {
    model_ = model;
    model_size_ = model_->size();
    
    QSize wsize( model_size_.width() * cell_size_.width(), model_size_.height() * cell_size_.height() );
    
    resize(wsize);
    
}
