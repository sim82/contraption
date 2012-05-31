/*
 * Copyright (C) 2009-2012 Simon A. Berger
 * 
 * This file is part of epa_viewer.
 * 
 *  epa_viewer is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  epa_viewer is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with epa_viewer.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <QPaintEvent>
#include <QPainter>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QGraphicsItem>
#include "PhyloTreeView.h"


void PhyloTreeView::paintEvent(QPaintEvent* e) {
    QPainter p(this);
    
    
    if( ctree_.isNull() ) {
        ctree_.reset( new cache_node( tree_.get() ));
    }
    
    const int yinc = 16;
    
    draw_childs( *ctree_, 0, 0, height() - yinc, true, &p, 0, 0 );
    
}

// int PhyloTreeView::draw_childs(const cache_node &cn, int xstart, int ymin, int ymax, QPainter* p, QGraphicsScene *gs) {
//     
//     int xinc = 32;
//     
//     if( cn.num_tips() == 1 ) {
//         int ymid = 0.5 * (ymin + ymax);
//         
//         
//         if( p != 0 ) {
//             p->drawLine( xstart, ymid, xstart + xinc, ymid );
//         }
//         
//         if( gs != 0 ) {
//             gs->addLine( xstart, ymid, xstart + xinc, ymid );
//             
//         }
//         
//         return ymid;
//     } else {
//         
//         size_t num_c1 = cn.c1().num_tips();
//         size_t num_c2 = cn.c2().num_tips();
//         
//         float splitf = num_c1 / float( num_c1 + num_c2 );
//         
//         int ymid = ymin + (ymax - ymin) * splitf;
// //         int ymid_outer = ymin + (ymax - ymin) * 0.5;
//     
//         
//         int xstart_c = xstart + xinc;
//         
//         int ymid1 = draw_childs( cn.c1(), xstart_c, ymin, ymid, p, gs );
//         int ymid2 = draw_childs( cn.c2(), xstart_c, ymid, ymax, p, gs );
//         
//         
//         if( p != 0 ) {
//             p->drawLine( xstart, ymid, xstart_c, ymid );
//             p->drawLine( xstart_c, ymid1, xstart_c, ymid2 );
//         }
//         
//         if( gs != 0 ) {
//             gs->addLine( xstart, ymid, xstart_c, ymid );
//             gs->addLine( xstart_c, ymid1, xstart_c, ymid2 );
//             
//             
//         }
//         
//         
//         return ymid;
//     }
//     
//     
//     
//     
//     
// }


int PhyloTreeView::draw_childs(const cache_node &cn, int xstart, int ymin, int ymax, bool upper, QPainter* p, QGraphicsScene *gs, std::map<std::string,QGraphicsPathItem *> *pi_map) {
    int xinc = 3000 * cn.branch_length();
    
    if( cn.num_tips() == 1 ) {
        int ymid = 0.5 * (ymin + ymax);
        
        
//         if( p != 0 ) {
//             p->drawLine( xstart, ymid, xstart + xinc, ymid );
//         }
    
        
    
        
        if( gs != 0 ) {
            QPainterPath path;
            
            if( !upper ) {
            
                path.moveTo( xstart, ymin );
                
                
            } else {
                path.moveTo( xstart, ymax );
            }
            
//             QFont font("fixed");
//             font.set
            
           
            
            path.lineTo( xstart, ymid );
            path.lineTo( xstart + xinc, ymid );
        
            
//             gs->addLine( xstart, ymid, xstart + xinc, ymid );
            QGraphicsPathItem *pi = gs->addPath(path, default_pen());
            
            
            QGraphicsSimpleTextItem *gsst = gs->addSimpleText( QString(cn.taxon().c_str()) );
           
            
            gsst->setPos( xstart + xinc + 10, ymid - gsst->boundingRect().height() * 0.5 );
//             pi->setPen(QPen(Qt::red));
//             std::cout << "path tip: " << cn.branch_label() << std::endl;
            if( pi_map != 0 ) {
                pi_map->insert( std::make_pair( cn.branch_label(), pi ));
            }
            
        }
        
        return ymid;
    } else {
        
        size_t num_c1 = cn.c1().num_tips();
        size_t num_c2 = cn.c2().num_tips();
        
        float splitf = num_c1 / float( num_c1 + num_c2 );
        
        int ymid = ymin + (ymax - ymin) * splitf;
        int ymid_outer = ymid; //ymin + (ymax - ymin) * 0.5;
    
        
        int xstart_c = xstart + xinc;
        
        int ymid1 = draw_childs( cn.c1(), xstart_c, ymin, ymid, true, p, gs, pi_map );
        int ymid2 = draw_childs( cn.c2(), xstart_c, ymid, ymax, false, p, gs, pi_map );
        
        
//         if( p != 0 ) {
//             p->drawLine( xstart, ymid, xstart_c, ymid );
//             p->drawLine( xstart_c, ymid1, xstart_c, ymid2 );
//         }
        
        if( gs != 0 ) {
            QPainterPath path;
            
            
            if( !upper ) {
                path.moveTo( xstart, ymin );
            } else {
                path.moveTo( xstart, ymax );
            }
            
            path.lineTo( xstart, ymid_outer );
            path.lineTo( xstart_c, ymid_outer );
            QGraphicsPathItem *pi = gs->addPath( path, default_pen() );
            
//             std::cout << "path inner: " << cn.branch_label() << std::endl;
            
            if( pi_map != 0 ) {
                pi_map->insert( std::make_pair( cn.branch_label(), pi ));
            }
            
//             gs->addLine( xstart_c, ymid1, xstart_c, ymid2 );
            
            
        }
        
        
        return ymid;
    }
    
    
    
    
    
}


PhyloTreeView::PhyloTreeView(sptr::shared_ptr< PhyloTreeView::lnode > tree, QWidget* parent) : QWidget(parent), tree_(tree) {
    assert( tree_.get() != 0 );
    
    ctree_.reset( new cache_node( tree_.get() ));
    
    const int xinc = 32;
    const int yinc = 16;
    
    QSize size( ctree_->max_depth() * xinc, ctree_->num_tips() * yinc );
    
    
    resize( size );

}

QGraphicsScene* PhyloTreeView::initGraphicsScene( std::map<std::string, QGraphicsPathItem*> *pi_map ) {

    QGraphicsScene *gs = new QGraphicsScene();
    
    
    if( ctree_.isNull() ) {
        ctree_.reset( new cache_node( tree_.get() ));
    }
    
    const int yinc = 16;
    
    draw_childs( *ctree_, 0, 0, height() - yinc, true, 0, gs, pi_map );
    
    return gs;
}
PhyloTreeView::~PhyloTreeView() {}
QPen PhyloTreeView::default_pen() {
    QPen pen0;
    pen0.setColor( QColor(Qt::black) );
    pen0.setWidth(2.0);
    pen0.setCapStyle(Qt::RoundCap);
    pen0.setJoinStyle(Qt::RoundJoin);

    return pen0;
}

