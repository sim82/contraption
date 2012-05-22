
#include <QPainter>
#include <QPaintEvent>
#include <QStaticText>
#include <QScrollArea>
#include <limits>
#include <cctype>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <cassert>
#include "TextGrid.h"



TextGrid::TextGrid(QWidget* w, int zoom_factor, QSize cell_size ) 
   : QWidget(w), model_(0), cell_size_(cell_size), zoom_factor_(zoom_factor)
   
   
{
    f.setStyleHint(QFont::Monospace);
    f.setPointSize(12);
    
    setFont(f);
    
    QFontMetrics metrics =fontMetrics();
    int fmw = metrics.maxWidth();
    int fmh = metrics.ascent();
    
    cell_size_ = QSize(fmw,fmh);
    
    for( char c = 0; c < std::numeric_limits<char>::max(); ++c ) {
        if( isprint(c) ) {
            stext_.push_back( QStaticText(QChar(c)) );
            
//             stext_.back().
        } else {
            stext_.push_back( QStaticText() );
        }
    }
}

/*
void TextGrid::paintEvent(QPaintEvent* e) {
    QRect r = e->rect();
    
    QPainter p(this);
    p.setFont(f);
    
    
    QSize zsize = cell_size_ * zoom_factor_;
    
    int yfirst = r.top() / zsize.height(); 
    int ylast = r.bottom() / zsize.height(); 
    
    int xfirst = r.left() / zsize.width(); 
    int xlast = r.right() / zsize.width();
    
    
    
    
    for( int y = yfirst; y <= ylast; ++y ) {
        for( int x = xfirst; x <= xlast; ++x ) {
            QRect dr(x * zsize.width(), y * zsize.height(), zsize.width(), zsize.height());
    
            
//             QChar c = '0';//model_->data(x,y);
            QChar c = model_->data(x,y);
//             QStaticText qst(c);
            
            p.fillRect( dr, model_->brush(x,y));
            
            if( zsize.width() > 8 ) {
                p.drawStaticText(dr.topLeft(), stext_.at(size_t(c.toAscii())));
            }
        }
    }
    
    
    
}*/



void TextGrid::paintEvent(QPaintEvent* e) {
    assert( model_ != 0 );
    try {
    
        QRect r = e->rect();
        
        QPainter p(this);
        p.setFont(f);
        
        
        QSize zsize = cell_size_ * zoomf();
        
        QSizeF zsizef = cell_size_;
        zsizef *= zoomf();
        
        
        int yfirst = r.top() / zsizef.height(); 
        int ylast = r.bottom() / zsizef.height(); 
        
        int xfirst = r.left() / zsizef.width(); 
        int xlast = r.right() / zsizef.width();
        
        
        float fw = r.width() / float(zsizef.width());
        float fh = r.height() / float(zsizef.height());
        
        const float tmin = 7;
        const float tmax = 9;
        
        int lighten = -1;
        bool set_tcol = false;
        bool show_text = true;
        QColor tcol( Qt::black);
        
        
        if( zsizef.width() >= tmax ) {
            lighten = 150;
        } else if( zsizef.width() <= tmin ) {
            // nothing
            // lighten stays at -1 and will be ignored
            show_text = false;
        } else {
            // transition
            
            float fw = zsizef.width() - tmin;
            float td = fw / (tmax-tmin);
                       
            tcol.setAlphaF(td);
            set_tcol = true;
            
            lighten = 100 + td * 50;
        }
        
        for( int y = yfirst; y <= ylast; ++y ) {
            for( int x = xfirst; x <= xlast; ++x ) {
                
                QPoint topleft( x * zsizef.width(), y * zsizef.height() );
                QPoint bottomright( (x+1) * zsizef.width(), (y+1) * zsizef.height() );
                QRect drf( topleft, bottomright );
   
   
                //             QChar c = '0';//model_->data(x,y);
                QChar c = model_->data(x,y);
                //             QStaticText qst(c);
                
                QColor col = model_->color(x,y);
                    
                
                if( lighten > 0 ) {
                    col = col.lighter(lighten);
                }
                
                p.fillRect( drf, col);
                
                if( show_text ) {
                    if( set_tcol ) {
                        p.setPen(tcol);
                    }
                    //                     p.setBrush(QBrush(tcol));
                    p.drawStaticText(drf.topLeft(), stext_.at(size_t(c.toAscii())));   
//                     p.drawText(drf.topLeft(), c );   
                
                }
                
            }
        }
    } catch( std::out_of_range x ) {
        std::cout << "out of range exception\n";
        abort();
    }
    
    
}
// void TextGrid::updateSize() {
//     QSize zsize = cell_size_ * zoom_factor_;
//     
//     QSize wsize( model_size_.width() * zsize.width(), model_size_.height() * zsize.height() );
//     
//     resize(wsize);
//     
// }

void TextGrid::updateSize() {
    QSizeF zsizef = cell_size_;
    
    zsizef *= zoomf();
    
    
    QSize wsize( model_size_.width() * zsizef.width(), model_size_.height() * zsizef.height() );
    
    
    std::cout << "zoom: " << zoom_factor_ << " " << zsizef.width() << " " << zsizef.height() << "\n";
    
    
    resize(wsize);
    
}

void TextGrid::setModel(TextGridModel* model) {
    model_ = model;
    model_size_ = model_->size();
    
    updateSize();
}

void TextGrid::setZoom( int factor ) {
    zoom_factor_ = factor;
    
    updateSize();
    
}


void TextGrid::wheelEvent( QWheelEvent *e ) {
//     std::cout << e->x() << " " << e->globalX() << "\n";
//     return;
    if( !e->modifiers().testFlag(Qt::ControlModifier) ) {
        e->ignore();
        return;
    }



    QSize zsize = cell_size_ * zoomf();
    size_t xcell = e->x() / zsize.width();
    size_t ycell = e->y() / zsize.height();
    
    
    
    if( e->delta() < 0 ) {
        zoom_factor_ = std::max( 10, zoom_factor_ - 1 );
    } else {
        zoom_factor_ = std::min( 100, zoom_factor_ + 1 );
    }
    emit zoomChanged(zoom_factor_);
    updateSize();
    
    
    QScrollArea *psa = parentScrollArea();
    
    if( psa != 0 ) {
        
        QPoint ppos = psa->mapToParent(e->pos());
        std::cout << xcell << " " << ycell << "\n";
        QPoint tpoint( xcell * zoomf(), ycell * zoomf() );
        psa->ensureVisible( tpoint.x(), tpoint.y(), ppos.x(), ppos.y() );
    }
    
    e->accept();
    
//     this->
}
QScrollArea* TextGrid::parentScrollArea() {
    QScrollArea *psv = dynamic_cast<QScrollArea*>(parent());

    return psv;
}
// void TextGrid::mousePressEvent(QMouseEvent* e) {
//     std::cout << "down: " << e->pos().x() << " " << e->pos().y() << "\n";
//     ss_ = scroll_state(e->pos());
//     e->accept();
// }
// void TextGrid::mouseReleaseEvent(QMouseEvent* e) {
//     std::cout << "up: " << e->pos().x() << " " << e->pos().y() << "\n";
//     
//     e->accept();
// 
// }
// void TextGrid::mouseMoveEvent(QMouseEvent* e) {
//     QPoint d = ss_.update(e->pos());
//     
//     std::cout << "move: " << d.x() << " " << d.y() << "\n";
//     
//     QScrollArea *psa = parentScrollArea();
//     if( psa != 0 ) {
//         psa->scrollContentsBy(d.x(), d.y());
//     }
//     
//     e->accept();
// 
//     
// }
