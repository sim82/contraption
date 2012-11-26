/*
 * Copyright (C) 2009-2012 Simon A. Berger
 * 
 * This file is part of visual_papara.
 * 
 *  visual_papara is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  visual_papara is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with visual_papara.  If not, see <http://www.gnu.org/licenses/>.
 */



#include <QPainter>
#include <QPaintEvent>
#include <QStaticText>
#include <QScrollArea>
#include <QScrollBar>
#include <QMessageBox>
#include <limits>
#include <cctype>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <cassert>
#include "TextGrid.h"



TextGrid::TextGrid(QScrollArea* w, int zoom_factor, QSize cell_size ) 
   : QWidget(w), scroll_area_(w), model_(0), cell_size_(cell_size), zoom_factor_(zoom_factor)
   
   
{
    f.setStyleHint(QFont::Monospace);
//     f.setPointSize(10);
    
    setFont(f);
    
    QFontMetrics metrics = fontMetrics();

    QRect max_rect = metrics.boundingRect( 'G' );
    //int fmw = metrics.maxWidth();
    //int fmh = metrics.ascent();
    
    int fmw = max_rect.width() + 2;
    int fmh = metrics.ascent() + 2;

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
        
        //const float tmin = 7;
        //const float tmax = 9;
        const float tmax = cell_size_.width() - 2;
        const float tmin = cell_size_.width() - 4;
                
        int lighten = -1;
        bool set_tcol = false;
        bool show_text = true;
        QColor tcol( Qt::black);
        
        int lighten_min = 130;
	int lighten_max = 170; 
        if( zsizef.width() >= tmax ) {
            lighten = lighten_max;
            set_tcol = true;
        } else if( zsizef.width() <= tmin ) {
            // nothing
            // lighten stays at -1 and will be ignored
            show_text = false;
            lighten = lighten_min;
        } else {
            // transition
            
            float fw = zsizef.width() - tmin;
            float td = fw / (tmax-tmin);
                       
            tcol.setAlphaF(td);
            set_tcol = true;
            
            lighten = lighten_min + td * (lighten_max - lighten_min);
        }
        //         const QPoint text_offset( 1, 1 );
        const QPoint text_offset( 1, -2 );
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
                    
                    p.drawStaticText(drf.topLeft() + text_offset, stext_.at(size_t(c.toAscii())));   
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

void TextGrid::setModel(QSharedPointer<TextGridModel> model) {
    std::cerr << "<<<<<< setModel\n";
    
    model_ = model;
    model_size_ = model_->size();
    
    updateSize();
}

void TextGrid::setZoom( int factor ) {
    zoom_factor_ = factor;
	

	float hs = -1; 
	float vs = -1; 

	QScrollArea *psa = parentScrollArea();

	if( psa != 0 ) {

		float h = parentScrollArea()->horizontalScrollBar()->value();
		float v = parentScrollArea()->verticalScrollBar()->value();
	
		h += width() / 2;
		v += height() / 2;

		hs = float(h) / width();	
		vs = float(v) / height();
	}

	
    updateSize();


	if( psa != 0 ) {
		int hn = hs * width();
		int vn = vs * height();
		
		hn -= width() / 2;
		vn -= height() / 2;

		parentScrollArea()->horizontalScrollBar()->setValue(hn);
		parentScrollArea()->verticalScrollBar()->setValue(vn);
	} else {
		//QMessageBox::critical( this, "no psa", "no psa" );
	}
	
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
    
    if( false && psa != 0 ) {
        
        QPoint ppos = psa->mapToParent(e->pos());
        std::cout << xcell << " " << ycell << "\n";
        QPoint tpoint( xcell * zoomf(), ycell * zoomf() );
        psa->ensureVisible( tpoint.x(), tpoint.y(), ppos.x(), ppos.y() );
    }
    
    e->accept();
    
//     this->
}
QScrollArea* TextGrid::parentScrollArea() {
   // QScrollArea *psv = dynamic_cast<QScrollArea*>(parent());

//	QMessageBox::information( this, "xxx", typeid(parent()).name() );

    //return psv;
	return scroll_area_;
}
TextGridModel::~TextGridModel() { std::cerr << "~TextGridModel" << std::endl; }
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
