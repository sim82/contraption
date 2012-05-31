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


#ifndef __PhyloTreeView_h
#define __PhyloTreeView_h
#include <map>
#include <QWidget>
#include <QScopedPointer>
#include "ivymike/tree_parser.h"
#include "ivymike/smart_ptr.h"


class QPainter;
class QGraphicsScene;
class QGraphicsPathItem;

class PhyloTreeView : public QWidget {
Q_OBJECT


public:
     
    typedef ivy_mike::tree_parser_ms::lnode lnode;
   
    
    PhyloTreeView( sptr::shared_ptr<lnode> tree, QWidget *parent = 0 ) ;
    virtual ~PhyloTreeView() ;
    
    
    void paintEvent( QPaintEvent *e ) ;
    
    QGraphicsScene *initGraphicsScene(std::map< std::string, QGraphicsPathItem* >* pi_map) ;
    
    static QPen default_pen() ;
    
private:
    
    
    class cache_node {
    public:
        cache_node( lnode *n ) : num_tips_(0), branch_label_(n->backLabel), branch_length_(n->backLen) {
            if( n->next->back != 0 && n->next->next->back != 0 ) {
                c1_.reset( new cache_node( n->next->back ));
                c2_.reset( new cache_node( n->next->next->back ));
               
                num_tips_ = c1_->num_tips() + c2_->num_tips();
                max_depth_ = 1 + std::max( c1_->max_depth(), c2_->max_depth() );
                
                max_branch_length_sum_ = branch_length() + std::max( c1_->max_branch_length_sum(), c2_->max_branch_length_sum() );
                
            } else {
                num_tips_ = 1;
                max_depth_ = 1;
                taxon_ = n->m_data->tipName;
                max_branch_length_sum_ = branch_length();
            }
            
            
        }
        
        size_t num_tips() const {
            return num_tips_;
        }
        
        size_t max_depth() const {
            return max_depth_;
        }
        
        float max_branch_length_sum() const {
            return max_branch_length_sum_;
        }
        
        const cache_node &c1() const {
            return *c1_;
        }
        
        const cache_node &c2() const {
            return *c2_;
        }
        
        const std::string &branch_label() const {
            return branch_label_;
        }
        
        const std::string &taxon() const {
            return taxon_;
        }
        
        float branch_length() const {
            return branch_length_;
        }
    
    
    
    private:
        
        size_t num_tips_;
        size_t max_depth_;
        QScopedPointer<cache_node> c1_;
        QScopedPointer<cache_node> c2_;
        std::string branch_label_;
        std::string taxon_;
        float branch_length_;
        float max_branch_length_sum_;
    };
    
    int draw_childs( const PhyloTreeView::cache_node& cn, int xstart, int ymin, int ymax, bool upper, QPainter* p, QGraphicsScene* gs, std::map< std::string, QGraphicsPathItem*, std::less< std::string >, std::allocator< std::pair< const std::string, QGraphicsPathItem* > > >* pi_map ) ;
    
    
    sptr::shared_ptr<lnode> tree_;
    QScopedPointer<cache_node> ctree_;
    
    
};


#endif