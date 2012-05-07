#include "papara.h"

#include "main_widget.h"
#include "ui_main_widget.h"
#include <QFileDialog>
#include <QProgressDialog>
#include <QThread>
#include <QLabel>
#include <QTableView>
#include <QScrollBar>

#include "ivymike/time.h"

streambuf_to_q_plain_text_edit::streambuf_to_q_plain_text_edit( QPlainTextEdit *qpte, std::size_t buff_sz, std::size_t put_back )
:
  put_back_(std::max(put_back, size_t(1))),
  buffer_(std::max(buff_sz, put_back_) + put_back_)
{

    QObject::connect( this, SIGNAL(post_text(QString)), qpte, SLOT(insertPlainText(const QString &)), Qt::QueuedConnection );
}



void streambuf_to_q_plain_text_edit::append( int of, char *first, char *last ) {
    size_t size = std::distance(first, last);

//     std::cout << "append: " << char(of) << " " << std::distance(first,last) << "\n";
    
    assert( size < 100000 );
    QByteArray ba( first, size );

    QString s;
    s.append(ba);

    if( of > 0 ) {
        const char b[2] = {of, 0};
        s.append( b );
    }
    
    //qpte_->appendPlainText(s);
    emit post_text( s );

}

// overrides base class over()
streambuf_to_q_plain_text_edit::int_type streambuf_to_q_plain_text_edit::overflow(int c) {




//            std::cout << "overflow:";
//            std::copy( pbase(), epptr(), std::ostream_iterator<char>(std::cout));
//            std::cout << (char) c;
//            std::cout << std::endl;

    append( c, pbase(), epptr() );

    setp(&buffer_.front(), (&buffer_.back()) + 1);

    return 1;
}

int streambuf_to_q_plain_text_edit::sync() {
//            std::cout << "sync:";
//            std::copy( pbase(), pptr(), std::ostream_iterator<char>(std::cout));
//            std::cout << std::endl;

    append( 0, pbase(), pptr() );
    setp(&buffer_.front(), (&buffer_.back()) + 1);
    return 0;
}

class output_alignment_store : public papara::output_alignment {
public:
    virtual void push_back( const std::string &name, const out_seq &seq, seq_type t ) {
        if( t == output_alignment::type_qs ) {
            qs_.push_back(seqs_.size());
        } else {
            refs_.push_back(seqs_.size());
        }
        
        names_.push_back(name);
        seqs_.push_back(seq);
        
    }
    virtual void set_max_name_length( size_t len ) {}
    virtual void set_size( size_t num_rows, size_t num_cols ) {}

    const out_seq &qs_at( size_t idx ) const {
        return seqs_.at(qs_.at(idx));
    }
    
    const std::string &qs_name_at( size_t idx ) const {
        return names_.at(qs_.at(idx));
    }

    size_t num_qs() const {
        return qs_.size();
    }
    out_seq ref_at(int idx) const {
        return seqs_.at(refs_.at(idx));
    }
    size_t num_ref() const {
        return refs_.size();
    }
    const std::string &ref_name_at( size_t idx ) const {
        return names_.at(refs_.at(idx)); 
    }
    
private:
    std::vector <std::string> names_;
    std::vector <out_seq> seqs_;
    
    std::vector <size_t> refs_;
    std::vector <size_t> qs_;
//     std::vector <seq_type> refs_;
//     std::vector <seq_type> qs_;
//     

};


class papara_state {
public:

    papara_state( QPlainTextEdit *qpte_dont_use_me, const std::string &tree_name, const std::string &ref_name, const std::string &qs_name )
        : sbq_(qpte_dont_use_me),
          log_file_( "contraption_log.txt"),
          ost_( &sbq_ ),
          ldev( ost_, log_file_ ),
          lout_guard(  papara::lout, ldev ),
          qs_(qs_name),
          refs_(tree_name.c_str(), ref_name.c_str(), &qs_ )
    {


       // qpte->appendPlainText( "xxx");
        //qpte->appendPlainText( "yyy\n");
//        papara::lout << "bla bla bla" << std::endl;
    }

    void do_preprocessing() {

        papara::lout << "qs preprocess" << std::endl;
        qs_.preprocess();


        papara::lout << "ref preprocess" << std::endl;
        refs_.remove_full_gaps();
        refs_.build_ref_vecs();
        papara::lout << "done." << std::endl;
        
//         std::ofstream os( "queries.txt" );
//         
//         qs_.write( os );
//         
    }

    papara::scoring_results *do_scoring_only() const {
        ivy_mike::perf_timer t1;
        
        const size_t num_threads = 2;
        const size_t num_candidates = 1;
        papara::papara_score_parameters sp = papara::papara_score_parameters::default_scores();
        papara::scoring_results *res = new papara::scoring_results(qs_.size(), papara::scoring_results::candidates(num_candidates));
        papara::driver<pvec_pgap,papara::tag_dna>::calc_scores(num_threads, refs_, qs_, res, sp );
        
        t1.add_int();
        t1.print( );//papara::lout );
        
        return res;
    }
    
    output_alignment_store *do_scoring( const papara::scoring_results &res, bool ref_gaps ) const {        
        ivy_mike::perf_timer t1;
        
        QScopedPointer<output_alignment_store> oas(new output_alignment_store);
        
        papara::papara_score_parameters sp = papara::papara_score_parameters::default_scores();
        papara::driver<pvec_pgap,papara::tag_dna>::align_best_scores_oa( oas.data(), qs_, refs_, res, size_t(0), ref_gaps, sp );
 
        t1.add_int();
        t1.print();//papara::lout);
        
        return oas.take();
    }
    
    virtual ~papara_state() {}



    const papara::references<pvec_pgap,papara::tag_dna> &refs() const {
        return refs_;
    }
    void align_single(size_t qs_idx);
private:

    streambuf_to_q_plain_text_edit sbq_;

    std::ofstream log_file_;


    std::ostream ost_;
    papara::log_device ldev;
    papara::log_stream_guard lout_guard;

    papara_state();
    papara::queries<papara::tag_dna> qs_;
    papara::references<pvec_pgap,papara::tag_dna> refs_;
    
};



MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    table_model_(0),
    qs_table_model_(0),
    ref_table_model_(0,true),
    progress_dialog_(0)
{
   
    
    ui->setupUi(this);

//     ui->pte_log->setVisible(false);
    
    
    
    connect(ui->tv_alignment->horizontalScrollBar(), SIGNAL(valueChanged(int)), ui->tv_qs->horizontalScrollBar(), SLOT(setValue(int)));
    connect(ui->tv_qs->horizontalScrollBar(), SIGNAL(valueChanged(int)), ui->tv_alignment->horizontalScrollBar(), SLOT(setValue(int)));
   // ui->tv_alignment->setModel( &table_model_ );

#ifndef WIN32
	if( false ) {
        tree_filename_ = "/home/sim/src_exelixis/contraption/small.tree";
        ref_filename_ = "/home/sim/src_exelixis/contraption/small.phy";
        qs_filename_ = "/home/sim/src_exelixis/contraption/small_qs.fa";
    } else if(false) {
        tree_filename_ = "/home/sim/src_exelixis/contraption/test_1604/RAxML_bestTree.ref_orig";
        ref_filename_ = "/home/sim/src_exelixis/contraption/test_1604/orig.phy.1";
        qs_filename_ = "/home/sim/src_exelixis/contraption/test_1604/qs.fa.20";
    } else {
        tree_filename_ = "/space/projects/2012_robert_454/RAxML_bestTree.cora_Sanger_reference_alignment.tre";
        ref_filename_ = "/space/projects/2012_robert_454/cora_Sanger_reference_alignment.phy";
        qs_filename_ = "/space/projects/2012_robert_454/cluster_52_72_cora_inversa_squamiformis_DIC_148_149.fas";
    }
#else
	tree_filename_ = "C:/2012_robert_454/RAxML_bestTree.cora_Sanger_reference_alignment.tre";
	ref_filename_ = "C:/2012_robert_454/cora_Sanger_reference_alignment.phy";
	qs_filename_ = "C:/2012_robert_454/cluster_52_72_cora_inversa_squamiformis_DIC_148_149.fas";
#endif
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::post_show_stuff() {
    QList<int> old_sizes = ui->splitter->sizes();
    assert( old_sizes.size() == 3 );
    std::cout << "sizes: " << old_sizes[0] << " " << old_sizes[1] << " " << old_sizes[2] << "\n";
    
    old_sizes[2] = 0;
    ui->splitter->setSizes(old_sizes);   
}

// converting from QString to const char* is ridiculusly complicated. so store string as when conversion is necessary std::string...
static std::string de_q_string( QString qs ) {
    std::string s = qs.toStdString();

    return s;
}

void MainWidget::on_pb_tree_clicked()
{
    //QFileDialog::exec
    std::cout << "tree\n";
    tree_filename_ = de_q_string(QFileDialog::getOpenFileName(this));
    ui->pte_log->appendPlainText(tree_filename_.c_str());
}

void MainWidget::on_pb_ref_clicked()
{
    std::cout << "ref\n";
    ref_filename_ = de_q_string(QFileDialog::getOpenFileName(this));
    ui->pte_log->appendPlainText(ref_filename_.c_str());
}

void MainWidget::on_pb_qs_clicked()
{
    std::cout << "qs\n";
    qs_filename_ = de_q_string(QFileDialog::getOpenFileName(this));
    ui->pte_log->appendPlainText(qs_filename_.c_str());
}






void MainWidget::on_pbLoad_clicked()
{
    if( tree_filename_.empty() || ref_filename_.empty() || qs_filename_.empty() ) {
        ui->pte_log->appendPlainText( "file missing\n");
        return;
    }


    
    progress_dialog_ = new QProgressDialog( "Initializing papara static data", "cancel (not really)", 0, 1 );
//    QProgressDialog *pd = new QProgressDialog( "Initializing papara static data", "cancel (not really)", 0, 1 );
//    pd->show();

    papara_.reset();

    setEnabled(false);

    QThread *thread = new QThread;
    state_worker *worker = new state_worker( ui->pte_log, tree_filename_, ref_filename_, qs_filename_ );
    //obj is a pointer to a QObject that will trigger the work to start. It could just be this


    worker->moveToThread(thread);
    thread->start();
    QMetaObject::invokeMethod(worker, "doWork", Qt::QueuedConnection);
    //obj will need to emit startWork() to get the work going.

    connect(worker, SIGNAL(done( papara_state *)), this, SLOT(on_state_ready(papara_state *)));

}


void MainWidget::on_pbRun_clicked() {
    setEnabled(false);
   
    progress_dialog_ = new QProgressDialog( "Doing the papara", "cancel (not really)", 0, 1 );
    progress_dialog_->setMinimumDuration(0);
    QThread *thread = new QThread;
    scoring_worker *worker = new scoring_worker( ui->pte_log, papara_.data(), scoring_result_.data(), ui->cbRefGaps->isChecked() );
    //obj is a pointer to a QObject that will trigger the work to start. It could just be this


    worker->moveToThread(thread);
    thread->start();
    QMetaObject::invokeMethod(worker, "doWork", Qt::QueuedConnection);
    //obj will need to emit startWork() to get the work going.

    connect(worker, SIGNAL(done( output_alignment_store *, papara::scoring_results *)), this, SLOT(on_scoring_done(output_alignment_store *, papara::scoring_results *)));

}

void MainWidget::on_state_ready(papara_state *state) {
    if( progress_dialog_ != 0 ) {
        delete progress_dialog_;
    }
    
    papara_.reset( state );
    table_model_.set_papara_state( state );

    ui->tv_alignment->setModel(&table_model_);
    ui->tv_alignment->horizontalHeader()->hide();
    ui->tv_alignment->verticalHeader()->hide();
    
//     ui->tv_alignment->resizeColumnsToContents();
//     ui->tv_alignment->resizeRowsToContents();
    
    resize_rows_columns(ui->tv_alignment, 12, 12 );
    setEnabled(true);

    ui->pte_log->appendPlainText("papara static state initialized");
}

void MainWidget::resize_rows_columns( QTableView *tv, int row_size, int column_size ) {
    const int num_rows = tv->model()->rowCount();
    const int num_cols = tv->model()->columnCount();
    
    
    for( int i = 0; i < num_rows; ++i ) {
        tv->setRowHeight(i, row_size);
    }
    for( int i = 0; i < num_cols; ++i ) {
        tv->setColumnWidth(i, column_size);
    }
}

void MainWidget::on_scoring_done(output_alignment_store* oa, papara::scoring_results *res) {
    if( progress_dialog_ != 0 ) {
        delete progress_dialog_;
    }
    
    if( scoring_result_.isNull() ) {
        scoring_result_.reset(res);
    }
    
    assert( scoring_result_.data() == res );
    
    output_alignment_.reset(oa);
    qs_table_model_.set_oas(oa);
    ref_table_model_.set_oas(oa);

  
    int hvalue = -1;
    
    if( ui->tv_qs->model() != 0 ) {
//         old_num_cols = ui->tv_qs->model()->columnCount();
    
        hvalue = ui->tv_qs->horizontalScrollBar()->value();
    }
    
    ui->tv_qs->setModel(0);
    ui->tv_qs->setModel(&qs_table_model_);
    ui->tv_qs->horizontalHeader()->hide();
    ui->tv_qs->verticalHeader()->hide();
//     ui->tv_qs->resizeColumnsToContents();
//     ui->tv_qs->resizeRowsToContents();
//     
    resize_rows_columns(ui->tv_qs, 12, 12);
    
    if(true) {
        int vvalue = -1;
        if( ui->tv_alignment->model() != 0 ) {
            vvalue = ui->tv_alignment->verticalScrollBar()->value();
        }
        ui->tv_alignment->setModel(0);
        ui->tv_alignment->setModel(&ref_table_model_);
        ui->tv_alignment->horizontalHeader()->hide();
        ui->tv_alignment->verticalHeader()->hide();
//         ui->tv_alignment->resizeColumnsToContents();
//         ui->tv_alignment->resizeRowsToContents();
//     
        resize_rows_columns(ui->tv_alignment, 12, 12); 
        
        if( vvalue >= 0 ) {
            ui->tv_alignment->horizontalScrollBar()->setValue(vvalue);
        }
    }
    
    if( hvalue >= 0 ) {
        ui->tv_qs->horizontalScrollBar()->setValue(hvalue);
    }
    ui->pte_log->appendPlainText("scoring done");
    setEnabled(true);

    
}


void state_worker::doWork()
{
   papara_state *ps = new papara_state( qpte_, tree_.data(), ref_.data(), qs_.data());

   ps->do_preprocessing();

   emit done(ps);
}

void scoring_worker::doWork() {
    if( res_ == 0 ) {
        res_ = state_->do_scoring_only();
    }
    
    assert( res_ != 0 );
    
    output_alignment_store *oa = state_->do_scoring( *res_, ref_gaps_ );
    
    emit done(oa, res_);
}

raw_alignment_table_model::raw_alignment_table_model(QObject *parent )
: papara_state_(0)
{


}

int raw_alignment_table_model::rowCount(const QModelIndex &parent ) const {
    if( papara_state_ == 0 ) {
        return 0;
    } else {
        return papara_state_->refs().num_seqs();
        
    }

}

int raw_alignment_table_model::columnCount(const QModelIndex &parent ) const {
    if( papara_state_ == 0 ) {
        return 0;

    } else {
        return papara_state_->refs().seq_at(0).size();
    }

}

QVariant raw_alignment_table_model::data(const QModelIndex &index, int role ) const {
    
    
    
    if( papara_state_ != 0 ) {
    switch(role){
    case Qt::DisplayRole:
        //return QString((char)papara_state_->refs().seq_at(index.row()).at(index.column()));
        return (char)papara_state_->refs().seq_at(index.row()).at(index.column());
//      case Qt::FontRole:
//          if (row == 0 && col == 0) //change font only for cell(0,0)
//          {
//              QFont boldFont;
//              boldFont.setBold(true);
//              return boldFont;
//          }
//          break;
     case Qt::BackgroundRole:

         
         {
            
             char c = (char)papara_state_->refs().seq_at(index.row()).at(index.column());
             
             
             switch( c ) {
             case 'A':
                 return QBrush( QColor( 255, 128, 128 ) );
             case 'C':
                 return QBrush( QColor( 128, 255, 128 ) );
             case 'G':
                 return QBrush( QColor( 128, 128, 255 ) );
             case 'T':
                 return QBrush( QColor( 255, 255, 128 ) );
             default:
                 return QVariant();
             }
             
             
         }
         break;
     case Qt::TextAlignmentRole:
         return Qt::AlignRight + Qt::AlignVCenter;
         
//      case Qt::CheckStateRole:
// 
//          if (row == 1 && col == 0) //add a checkbox to cell(1,0)
//          {
//              return Qt::Checked;
//          }
         
     default:
         return QVariant();
     }

    } else {
        return QVariant();
    }

}
QVariant raw_alignment_table_model::headerData(int section, Qt::Orientation orientation, int role) const {
    if( orientation == Qt::Vertical ) {
        if( role == Qt::DisplayRole ) {
            return QString(papara_state_->refs().name_at(section).c_str());
        } else {
            
        
            return QAbstractTableModel::headerData( section, orientation, role );
        }
    } else {
        return QVariant();
    }

}

alignment_table_model::alignment_table_model(QObject* parent, bool use_ref) : use_ref_(use_ref) {
    
}

int alignment_table_model::rowCount(const QModelIndex &parent ) const {
    if( oas_ == 0 ) {
        return 0;
    } 
    
    if( use_ref_ ) {
        return oas_->num_ref();
    } else {
        return oas_->num_qs();
    }

}

int alignment_table_model::columnCount(const QModelIndex &parent ) const {
    if( oas_ == 0 ) {
        return 0;
    }
    
    if( use_ref_ ) {
        return oas_->ref_at(0).size();
    } else {
        return oas_->qs_at(0).size();
    }

}

static size_t g_count = 0;

QVariant alignment_table_model::data(const QModelIndex &index, int role ) const {
    
    
    
    if( oas_ == 0 ) {
        return QVariant();
    }
    
    
    
    
//     ++g_count;
//     
//     if( g_count % 100000 == 0 ) {
//         std::cout << "meeeeep " << g_count << "\n";
//     }
    
    
    switch(role){
    case Qt::DisplayRole:
    {
//         return QVariant();
        char ret;
        if( use_ref_ ) {
            //ret = (char)oas_->ref_at(index.row()).at(index.column());
            ret = (char)oas_->ref_at(index.row())[index.column()];
        } else {
//             ret = (char)oas_->qs_at(index.row()).at(index.column());
            ret = (char)oas_->qs_at(index.row())[index.column()];
        }
        
        char x[2] = {ret, 0};
        return QVariant(x);
//         return QVariant();
    }
    case Qt::BackgroundRole:
        {

            char c;
             
             if( use_ref_ ) {
                 c = (char)oas_->ref_at(index.row()).at(index.column());
             } else {
                 c = (char)oas_->qs_at(index.row()).at(index.column());   
             }
             
             switch( c ) {
             case 'A':
                 return QBrush( QColor( 255, 128, 128 ) );
             case 'C':
                 return QBrush( QColor( 128, 255, 128 ) );
             case 'G':
                 return QBrush( QColor( 128, 128, 255 ) );
             case 'T':
                 return QBrush( QColor( 255, 255, 128 ) );
             default:
                 return QVariant();
             }
             
             
         }
         break;
     case Qt::TextAlignmentRole:
         return Qt::AlignRight + Qt::AlignVCenter;
         
//      case Qt::CheckStateRole:
// 
//          if (row == 1 && col == 0) //add a checkbox to cell(1,0)
//          {
//              return Qt::Checked;
//          }
         
     default:
         return QVariant();
     }

    

}
QVariant alignment_table_model::headerData(int section, Qt::Orientation orientation, int role) const {
    if( orientation == Qt::Vertical ) {
        if( role == Qt::DisplayRole ) {
            
            if( use_ref_ ) {
                return QString(oas_->ref_name_at(section).c_str() );
            } else {
                return QString(oas_->qs_name_at(section).c_str() );
            }
        } else {
            return QAbstractTableModel::headerData( section, orientation, role );
        }
    } else {
        return QVariant();
    }

}

void MainWidget::on_cbRefGaps_stateChanged(int s) {
    
    if( !scoring_result_.isNull() ) {
        // TODO: make the forwarding dependent on expected time of alignment generation.
        on_pbRun_clicked(); // trigger 'run' iff scoring results are already available
    }
    
}

// bg_align_worker::bg_align_worker( const papara_state *state, QThread* thread) : state_(state) {
//     moveToThread(thread);
// }
// 
// bg_align_worker::align_done(size_t qs_idx, std::vector< uint8_t > seq) {
//     
// }
// 
// bg_align_worker::align(size_t qs_idx) {
//     state_->align_single( qs_idx );
// }
