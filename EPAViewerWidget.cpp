#include <iostream>

#include <QScriptEngine>
#include <QFile>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QListView>
#include <QTimer>
#include <QTransform>
#include "ivymike/tree_parser.h"

#include "EPAViewerWidget.h"
#include "PhyloTreeView.h"
#include "ui_epa_viewer_widget.h"


namespace im_tree = ivy_mike::tree_parser_ms;





EPAViewerWidget::EPAViewerWidget(const char* jplace_name, QWidget* parent) 
  : QWidget(parent), 
    ui_(new Ui::EPAViewerWidget()),
    tiTest( new QTimer(this) ),
    cur_taxon_placement_(-1)

{
    tiTest->setObjectName("tiTest");
    
    ui_->setupUi(this);
    
//     QMetaObject::connectSlotsByName(this);
    
    QFile f( jplace_name );
    f.open( QFile::ReadOnly);
    
    
    QByteArray fd_ba = f.readAll();
    
    
    
    QScriptValue sc; 
    QScriptEngine engine;
    
    QString ss;
    ss += "(";
    ss += fd_ba;
    ss += ")";
    sc = engine.evaluate(ss); // In new versions it may need to look like engine.evaluate("(" + QString(result) + ")");
    
//     sc.
    
    
    QScriptValue fields = sc.property("fields");
    
    assert( fields.isArray() );
    qint32 num_fields = fields.property("length").toInt32();
    
    qint32 idx_edge_num = -1;
    qint32 idx_like_weight_ratio = -1;
    
    for( qint32 i = 0; i < num_fields; ++i ) {
        QString name = fields.property(i).toString();
        
//         std::cout << name.toStdString() << std::endl;
        
        if( name == "edge_num" ) {
            idx_edge_num = i;
        } else if( name == "like_weight_ratio" ) {
            idx_like_weight_ratio = i;
        }
    }
    assert( idx_edge_num >= 0 );
    assert( idx_like_weight_ratio >= 0 );
    
    
    QScriptValue placements = sc.property("placements");
    assert( placements.isArray() );
    
    qint32 num_placements = placements.property( "length" ).toInt32();
    
    
    std::cout << "num_placements: " << num_placements << "\n";
    
    for( qint32 i = 0; i < num_placements; ++i ) {
        QScriptValue pm = placements.property( i );
        assert( pm.isValid() );
        
        QScriptValue p = pm.property("p");
        QScriptValue n = pm.property("n");
        assert( n.isArray() );
        
        QString name = n.property(0).toString();
        
        taxon_placements_.push_back( taxon_placement(name.toStdString(), p, idx_edge_num, idx_like_weight_ratio ) );
        
        
    }
    
    
    taxon_list_model_.reset( new TaxonListModel(taxon_placements_) );
    ui_->lvTaxon->setModel( taxon_list_model_.data() );
    
    connect( ui_->lvTaxon->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT( lvTaxon_selectionChanged(QItemSelection,QItemSelection)));
    
    
    std::cout << "isArray: " << sc.property("tree").toString().toStdString() << "\n";
 
    QScriptValue tree_v = sc.property("tree");
    QByteArray tree_ascii = tree_v.toString().toAscii();
    
    im_tree::ln_pool pool;
    
    im_tree::parser p(tree_ascii.begin(), tree_ascii.end(), pool );
    std::copy( tree_ascii.begin(), tree_ascii.end(), std::ostream_iterator<char>(std::cout) );
    std::cout << std::endl;
    
    im_tree::lnode *t = p.parse();
   
    PhyloTreeView *ptv = new PhyloTreeView(t->get_smart_ptr().lock());
    
   
    QGraphicsScene *gs = ptv->initGraphicsScene(&pi_map_);
    
    gvTree = new QGraphicsView( gs, this );
    
    gvTree->setRenderHint(QPainter::Antialiasing, true );
    gvTree->setRenderHint( QPainter::HighQualityAntialiasing, true );
    
    ui_->saTree->setWidget(gvTree);
    gs->setParent(gvTree);
    
    
    
    
    tiTest->setInterval(10);
    tiTest->setSingleShot(false);
//     tiTest->start();
    
    
    
//     qs->setVisible(true);

    
    
 /*
    if (sc.property("result").isArray())
    {
 
            QStringList items;
            qScriptValueToSequence(sc.property("result"), items); 
 
            foreach (QString str, items) {
                 qDebug("value %s",str.toStdString().c_str());
             }
 
    }*/
}
void EPAViewerWidget::on_tiTest_timeout() {
    
    size_t n = pi_map_.size();
    
    size_t r = rand() % n;
    
    std::stringstream ss;
    ss << r + 1;
    
    std::map< std::string, QGraphicsPathItem* >::iterator it = pi_map_.find( ss.str() );
    
    if( it == pi_map_.end() ) {
        return;
    }
    
    QColor rc( rand() % 256, rand() % 256, rand() % 256 );
    
    it->second->setPen( rc );
    
    
    
    gvTree->repaint();
    
    
}

QVariant TaxonListModel::data(const QModelIndex& index, int role) const {
    if( role == Qt::DisplayRole ) {
        return QString(placement_list_.at( index.row() ).taxon().c_str());
        
    }
    
    return QVariant();
    
}
int TaxonListModel::rowCount(const QModelIndex& parent) const {
    return placement_list_.size();
    
}


QVariant PlacementListModel::data(const QModelIndex& index, int role) const {
    if( role == Qt::DisplayRole ) {
        return placement_.pos_at(index.row() ).lh_weight();
    }
    
    return QVariant();
}
int PlacementListModel::rowCount(const QModelIndex& parent) const {
    return placement_.size();
    
}
taxon_placement::taxon_placement(const std::string& name, const QScriptValue& pos_list, qint32 idx_name, qint32 idx_lhw) : taxon_(name)
{
    assert( pos_list.isArray() );

    qint32 len = pos_list.property( "length" ).toInt32();

    std::cout << "taxon: " << name << "\n";
    for( qint32 i = 0; i < len; ++i ) {
        QScriptValue qspos = pos_list.property(i);
        assert( qspos.isArray() );

        QScriptValue name = qspos.property(idx_name).toString();
        qsreal lhw = qspos.property(idx_lhw).toNumber();

        std::cout << "pos: " << name.toString().toStdString() << " " << float(lhw) << std::endl;
        pos_.push_back(position( name.toString().toStdString(), float(lhw) ));
    }
}
void EPAViewerWidget::lvTaxon_selectionChanged( const QItemSelection & selected, const QItemSelection & deselected) {
//     size_t row = idx.row();
    
    
//     assert(taxon_placements_.size() > row );
    size_t row = selected.front().top();
    
    std::cout << "row: " << row << "\n";
    
    
    if( ui_->lvPosition->model() != 0 ) {
        ui_->lvPosition->selectionModel()->clear();
    }
    cur_taxon_placement_ = row;
    
    placement_list_model_.reset( new PlacementListModel( taxon_placements_.at(cur_taxon_placement_) ) );
    ui_->lvPosition->setModel( placement_list_model_.data());
    
    
    
    connect( ui_->lvPosition->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT( lvPosition_selectionChanged(QItemSelection,QItemSelection)));
    ui_->lvPosition->setSelectionMode( QListView::ExtendedSelection );
    ui_->lvPosition->selectAll();
    
}

void EPAViewerWidget::lvPosition_selectionChanged(const QItemSelection & selected, const QItemSelection & deselected) {
    const taxon_placement &p = taxon_placements_.at(cur_taxon_placement_);
    
    
    
    
    QPen pen1 = PhyloTreeView::default_pen();
    
//     pen1.setColor( QColor(Qt::red) );
//     pen1.setWidth(2);
//     pen1.setCapStyle(Qt::RoundCap);
//     pen1.setJoinStyle(Qt::RoundJoin);
//     
    
    
    QColor c0( Qt::red );
    QColor c1( Qt::blue );
    
    qreal rd = c1.redF() - c0.redF();
    qreal gd = c1.greenF() - c0.greenF();
    qreal bd = c1.blueF() - c0.blueF();
    
    
//     qreal hue0 = c0.hsvHueF();
//     qreal hue1 = c1.hsvHueF();
    
    
    for( QList< QItemSelectionRange >::const_iterator its = deselected.begin(); its != deselected.end(); ++its ) {
        for( int i = its->top(); i <= its->bottom(); ++ i ) {
            const taxon_placement::position &ppos = p.pos_at(i);
            const std::string &branch_name = ppos.branch_name();
        
            std::map< std::string, QGraphicsPathItem* >::iterator it = pi_map_.find( branch_name );
            
            assert( it != pi_map_.end() );
            
            it->second->setPen(PhyloTreeView::default_pen());
            
        }
    }
    
    
    QRectF br_all;
    bool have_item = false;
    for( QList< QItemSelectionRange >::const_iterator its = selected.begin(); its != selected.end(); ++its ) {
        for( int i = its->top(); i <= its->bottom(); ++ i ) {
            const taxon_placement::position &ppos = p.pos_at(i);
            const std::string &branch_name = ppos.branch_name();
        
            std::map< std::string, QGraphicsPathItem* >::iterator it = pi_map_.find( branch_name );
    
//             it->second->
            
            assert( it != pi_map_.end() );
            
            br_all = br_all.united(it->second->boundingRect());
            have_item = true;
           
            float lhw = ppos.lh_weight();
            
            QColor col;
            col.setRedF(c0.redF() + rd * lhw );
            col.setGreenF(c0.greenF() + gd * lhw );
            col.setBlueF(c0.blueF() + bd * lhw );
            
            
            pen1.setColor(col);
            it->second->setPen(pen1);
            
        }
    }
    
    if( have_item ) {
//         QRectF br = main_item->boundingRect();
//         gvTree->ensureVisible(br_all);
        gvTree->centerOn(br_all.center());
        
//         gvTree->ens`
//         gvTree->ensureVisible( main_item );
    }
    
    gvTree->repaint();
    
}
void EPAViewerWidget::on_slZoom_valueChanged(int value) {
    float scale = value / 100.0;
    
    QTransform trans;
    trans.scale(scale,scale);
    gvTree->setTransform(trans);
}
