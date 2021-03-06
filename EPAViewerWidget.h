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


#ifndef __EPAViewerWidget_h
#define __EPAViewerWidget_h

#include <vector>

#include <QWidget>
#include <QScopedPointer>
#include <QAbstractListModel>
#include "ui_EPAViewerWidget.h"


namespace Ui {
    class EPAViewerWidget;
}
class QGraphicsView;
class QGraphicsPathItem;
class QTimer;
class QScriptValue;

class taxon_placement {
public:
    taxon_placement( const std::string &name, const QScriptValue &pos_list, qint32 idx_name, qint32 idx_lhw ) 
      ;
    class position {
    public:
        
        position( const std::string &name, float weight ) 
          : lh_weight_(weight),
            branch_name_(name) 
        {
        }
        
        float lh_weight() const {
            return lh_weight_;
        }
        const std::string &branch_name() const {
            return branch_name_;
        }
    private:
        float lh_weight_;
        std::string branch_name_;
        
    };
    
    const std::string &taxon() const {
        return taxon_;
    }
    
    const position &pos_at( size_t idx ) const {
        return pos_.at(idx);
    }
     
    size_t size() const {
        return pos_.size();
    }
private:
    
    std::string taxon_;
    std::vector<position> pos_;
    
};

class TaxonListModel : public QAbstractListModel {
public:
    TaxonListModel( const std::vector<taxon_placement> &placement_list ) : placement_list_(placement_list ) {}
    
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const ;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const ;;
private:
    const std::vector<taxon_placement> &placement_list_;
};


class PlacementListModel : public QAbstractListModel {
public:
    PlacementListModel( const taxon_placement &p ) : placement_(p) {}
    
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const ;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const ;;
    
private:
    const taxon_placement &placement_;
    
};

class EPAViewerWidget : public QWidget {
Q_OBJECT
public:
    
    EPAViewerWidget( const char *jplace_name, QWidget *parent = 0 ) ;
    
public Q_SLOTS:
    void on_tiTest_timeout();
    void lvTaxon_selectionChanged( const QItemSelection& selected, const QItemSelection& deselected ) ;
    void lvPosition_selectionChanged( const QItemSelection& selected, const QItemSelection& deselected ) ;
    
    void on_slZoom_valueChanged( int value ) ;
    
private:
    
    QScopedPointer<Ui::EPAViewerWidget> ui_;
    QTimer *tiTest;
    
    
    QGraphicsView *gvTree;
    std::map<std::string,QGraphicsPathItem*> pi_map_;
    
    std::vector<taxon_placement> taxon_placements_;
    size_t cur_taxon_placement_;
    
    QScopedPointer<TaxonListModel> taxon_list_model_;
    QScopedPointer<PlacementListModel> placement_list_model_;
};
// #include "ui

#endif
