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


#include "papara.h"

#include "main_widget.h"
#include "ui_main_widget.h"

#include <QFileDialog>
#include <QProgressDialog>
#include <QThread>
#include <QLabel>
#include <QTableView>
#include <QScrollBar>
#include <QMessageBox>
#include <QGraphicsView>
#include <QGraphicsScene>

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
        const char b[2] = {char(of), '\0'};
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
    virtual ~output_alignment_store() { std::cerr << "<<<<<<<<<<<<<<  output_alignment_store" << std::endl; }
    
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
    
    void write_phylip( const char *filename ) {
        papara::output_alignment_phylip oa(filename);
        size_t max_name_len = 0;
        for( std::vector< std::string >::iterator it = names_.begin(), last = names_.end(); it != last; ++it ) {
            max_name_len = std::max( max_name_len, it->size() );
        }
        max_name_len += 1;
        
        oa.set_max_name_length(max_name_len);
        oa.set_size( names_.size(), seqs_.at(0).size() );
        for( std::vector< size_t >::iterator it = refs_.begin(), last = refs_.end(); it != last; ++it ) {
            oa.push_back( names_.at(*it), seqs_.at(*it), output_alignment::type_ref );
        }
        
        for( std::vector< size_t >::iterator it = qs_.begin(), last = qs_.end(); it != last; ++it ) {
            oa.push_back( names_.at(*it), seqs_.at(*it), output_alignment::type_qs );
        }
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


class alignment_grid_model : public TextGridModel {
public:
    alignment_grid_model( QSharedPointer<output_alignment_store> oas, bool use_ref ) : oas_(oas), use_ref_(use_ref) {}
    
    
    virtual QSize size() {
        if( use_ref_ ) {
            return QSize( oas_->ref_at(0).size(), oas_->num_ref() );
        } else {
            return QSize( oas_->qs_at(0).size(), oas_->num_qs() );
        }
            
    }
    
    virtual QChar data( size_t x, size_t y ) {
        try {
            return data_int(x,y);
        } catch( std::out_of_range ) {
            return 'X';
        }
    }
    inline QChar data_int( size_t x, size_t y ) {
        char ret;
        
       
        
//         std::cout << x << " " << y << " " << use_ref_ << "\n";
        if( use_ref_ ) {
            /*if( y >= oas_->num_ref() ) {
                std::cerr << "meeep y: " << y << "\n";
            
                return 'Y';
            }
            
            
            if( x >= oas_->ref_at(y).size() ) {
                std::cerr << "meeep x: " << x << "\n";
            
                return 'X';
            }*/ 
                
            
            ret = (char)oas_->ref_at(y).at(x);
            
        } else {
//             if( y >= oas_->num_qs() ) {
//                 std::cerr << "qs meeep y: " << y << "\n";
//                 
//                 return 'W';
//             }
//             
//             
//             if( x >= oas_->qs_at(y).size() ) {
//                 std::cerr << "qs meeep x: " << x << "\n";
//                 return 'Z';
//                 
//             }
            
            ret = (char)oas_->qs_at(y).at(x);
            
        }
        
        return ret;
    }
    virtual QColor color( size_t x, size_t y ) {
        char c;
        
        try {
            if( use_ref_ ) {
                c = (char)oas_->ref_at(y).at(x);
            } else {
                c = (char)oas_->qs_at(y).at(x);   
            }
        } catch( std::out_of_range x ) {
            return QColor( 255, 0, 255 );
        }
        
	QColor rcol;
        switch( c ) {
        case 'A':
            return QColor( 255, 0, 0 );
        case 'C': 
            return QColor( 0, 255, 0 );
        case 'G':
            return QColor( 0, 0, 255 );
        case 'T':
            return QColor( 255, 200, 0 );
        default:
            return Qt::white;
        }
    }
private:
    QSharedPointer<output_alignment_store> oas_;
    bool use_ref_;
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
          refs_(tree_name.c_str(), ref_name.c_str(), &qs_ ),
          scoring_parameters_(papara::papara_score_parameters::default_scores())
    {


       // qpte->appendPlainText( "xxx");
        //qpte->appendPlainText( "yyy\n");
//        papara::lout << "bla bla bla" << std::endl;
    }

    void set_scoring_parameters( const papara::papara_score_parameters &sp ) {
        scoring_parameters_ = sp;
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
        
        const size_t num_threads = QThread::idealThreadCount();
        const size_t num_candidates = 1;
        
        
        
      //  papara::papara_score_parameters sp = papara::papara_score_parameters::default_scores();
        papara::scoring_results *res = new papara::scoring_results(qs_.size(), papara::scoring_results::candidates(num_candidates));
        
        
        scoring_parameters_.print(std::cerr);
        papara::driver<pvec_pgap,papara::tag_dna>::calc_scores(num_threads, refs_, qs_, res, scoring_parameters_ );
        
        t1.add_int();
        t1.print( );//papara::lout );
        
        return res;
    }
    
    output_alignment_store *do_scoring( const papara::scoring_results &res, bool ref_gaps ) const {        
        ivy_mike::perf_timer t1;
        
        QScopedPointer<output_alignment_store> oas(new output_alignment_store);
        
        //papara::papara_score_parameters sp = papara::papara_score_parameters::default_scores();
        
        scoring_parameters_.print(std::cerr);
        papara::driver<pvec_pgap,papara::tag_dna>::align_best_scores_oa( oas.data(), qs_, refs_, res, size_t(0), ref_gaps, scoring_parameters_ );
 
        t1.add_int();
        t1.print();//papara::lout);
        
        return oas.take();
    }
    
    virtual ~papara_state() { std::cout << "~papara_state\n"; }



    const papara::references<pvec_pgap,papara::tag_dna> &refs() const {
        return refs_;
    }
    void align_single(size_t qs_idx);
    
    papara::papara_score_parameters scoring_parameters() {
        return scoring_parameters_;
    }
    
    sptr::shared_ptr<ivy_mike::tree_parser_ms::lnode> tree() const {
        return refs_.tree();
        
    }
    
private:

    streambuf_to_q_plain_text_edit sbq_;

    std::ofstream log_file_;


    std::ostream ost_;
    papara::log_device ldev;
    papara::log_stream_guard lout_guard;

    papara_state();
    papara::queries<papara::tag_dna> qs_;
    papara::references<pvec_pgap,papara::tag_dna> refs_;
    
    papara::papara_score_parameters scoring_parameters_;
    
};

// converting from QString to const char* is ridiculusly complicated. so store string as when conversion is necessary std::string...
static std::string de_q_string( QString qs ) {
    std::string s = qs.toStdString();

    return s;
}


static bool is_readable( std::string filename ) {
    std::ifstream is( filename.c_str() );
    
    return is.good();
}


MainWidget::MainWidget(QString treeName, QString refName, QString queryName, QWidget* parent) :
    QWidget(parent),
    ui(new Ui::MainWidget),
    progress_dialog_(0),
    bg_thread_(new QThread()),
    tg_ref_(0),
    tg_qs_(0),
    table_model_(0),
    qs_table_model_(0),
    ref_table_model_(0,true)
    
{
    
    qRegisterMetaType<QSharedPointer<papara_state> >();
    
   
   
   qRegisterMetaType<QSharedPointer<papara::scoring_results> >();
   qRegisterMetaType<QSharedPointer<output_alignment_store> >();
    
    ui->setupUi(this);

    ui->frButtons->setVisible(false);
    ui->pbRun->setPalette(QPalette( Qt::red ));
    
    papara::papara_score_parameters sp = papara::papara_score_parameters::default_scores();
    
    ui->sbOpen->setValue( sp.gap_open );
    ui->sbExt->setValue( sp.gap_extend );
    ui->sbMatch->setValue( sp.match );
    ui->sbCgap->setValue( sp.match_cgap );
    
    
    
//     ui->pte_log->setVisible(false);
    
//     sv_ref_ = new QScrollArea(ui->w_ref);
//     sv_qs_ = new QScrollArea(ui->w_qs);
//     
//     sv_ref_->show();
//     sv_qs_->show();
// //     ui->w_ref->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
//     sv_qs_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
//     sv_ref_->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
//     
    sv_ref_ = new QScrollArea();
    sv_qs_ = new QScrollArea();
    
    ui->splitter->insertWidget(0, sv_ref_);
    ui->splitter->insertWidget(1, sv_qs_);
    
    
    connect(sv_ref_->horizontalScrollBar(), SIGNAL(valueChanged(int)), sv_qs_->horizontalScrollBar(), SLOT(setValue(int)));
    connect(sv_qs_->horizontalScrollBar(), SIGNAL(valueChanged(int)), sv_ref_->horizontalScrollBar(), SLOT(setValue(int)));
   
    
    // ui->tv_alignment->setModel( &table_model_ );

// #ifndef WIN32
//     if( false ) {
//         tree_filename_ = "/home/sim/src_exelixis/contraption/small.tree";
//         ref_filename_ = "/home/sim/src_exelixis/contraption/small.phy";
//         qs_filename_ = "/home/sim/src_exelixis/contraption/small_qs.fa";
//     } else if(false) {
//         tree_filename_ = "/home/sim/src_exelixis/contraption/test_1604/RAxML_bestTree.ref_orig";
//         ref_filename_ = "/home/sim/src_exelixis/contraption/test_1604/orig.phy.1";
//         qs_filename_ = "/home/sim/src_exelixis/contraption/test_1604/qs.fa.20";
//     } else if( false ){
//         tree_filename_ = "/space/projects/2012_robert_454/RAxML_bestTree.cora_Sanger_reference_alignment.tre";
//         ref_filename_ = "/space/projects/2012_robert_454/cora_Sanger_reference_alignment.phy";
//         qs_filename_ = "/space/projects/2012_robert_454/cluster_52_72_cora_inversa_squamiformis_DIC_148_149.fas";
//     } else {
//         tree_filename_ = "/space/projects/2012_robert_454/RAxML_bestTree.cora_Sanger_reference_alignment.tre";
//         ref_filename_ = "/space/projects/2012_robert_454/cora_Sanger_reference_alignment.phy";
//         qs_filename_ = "/space/projects/2012_robert_454/cluster_52_72_cora_inversa_squamiformis_DIC_148_149.fas";
//     }
// #else
// 	tree_filename_ = "C:/2012_robert_454/RAxML_bestTree.cora_Sanger_reference_alignment.tre";
// 	ref_filename_ = "C:/2012_robert_454/cora_Sanger_reference_alignment.phy";
// 	qs_filename_ = "C:/2012_robert_454/cluster_52_72_cora_inversa_squamiformis_DIC_148_149.fas";
// #endif
//     
    tree_filename_ = de_q_string( treeName );
    ref_filename_ = de_q_string( refName );
    qs_filename_ = de_q_string( queryName );
    
    check_filenames();
    
    bg_thread_->start();
    
    
    if( is_readable( tree_filename_ ) && is_readable( ref_filename_ ) && is_readable( qs_filename_ ) ) {
        on_pbLoad_clicked();
    } else {
        QMessageBox::critical(this, "Input Files not readable", "One or more of the input files are not readble.", QMessageBox::Abort );
        
        throw std::runtime_error( "bailing out\n" );
    }
    
}

MainWidget::~MainWidget()
{
    bg_thread_->quit();
    bg_thread_->wait( 1000 );
    if( !bg_thread_->isFinished() ) {
        QMessageBox::critical(this, "Job Running", "A background job is still running. Need to do an unclean program exit, which might look like a crash..." );
        
        // give it one more chance in case it has shut down cleanly in the meantime
        if( !bg_thread_->isFinished() ) { 
            abort();
        }
        
        //bg_thread_->terminate();
    }
    
    delete ui;
}

void MainWidget::post_show_stuff() {
    QList<int> old_sizes = ui->splitter->sizes();
    assert( old_sizes.size() == 3 );
    std::cout << "sizes: " << old_sizes[0] << " " << old_sizes[1] << " " << old_sizes[2] << "\n";
    
    old_sizes[2] = 0;
    ui->splitter->setSizes(old_sizes);   
}

void MainWidget::on_pb_tree_clicked()
{
    //QFileDialog::exec
    std::cout << "tree\n";
    tree_filename_ = de_q_string(QFileDialog::getOpenFileName(this));
    ui->pte_log->appendPlainText(tree_filename_.c_str());
    
    check_filenames();
}

void MainWidget::on_pb_ref_clicked()
{
    std::cout << "ref\n";
    ref_filename_ = de_q_string(QFileDialog::getOpenFileName(this));
    ui->pte_log->appendPlainText(ref_filename_.c_str());
    check_filenames();
}

void MainWidget::on_pb_qs_clicked()
{
    std::cout << "qs\n";
    qs_filename_ = de_q_string(QFileDialog::getOpenFileName(this));
    ui->pte_log->appendPlainText(qs_filename_.c_str());
    check_filenames();
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

    papara_.clear();

    
    setEnabled(false);

   // QThread *thread = new QThread;
    
    
    state_worker_.reset( new state_worker( ui->pte_log, tree_filename_, ref_filename_, qs_filename_ ));
    //obj is a pointer to a QObject that will trigger the work to start. It could just be this

    assert( bg_thread_->isRunning() );
    state_worker_->moveToThread(bg_thread_.data());
//     thread->start();

    
    connect(state_worker_.data(), SIGNAL(done( QSharedPointer<papara_state>, QString)), this, SLOT(on_state_ready(QSharedPointer<papara_state>, QString)));

    QMetaObject::invokeMethod(state_worker_.data(), "doWork", Qt::QueuedConnection);
    //obj will need to emit startWork() to get the work going.

}


void MainWidget::on_pbRun_clicked() {
    //setEnabled(false);
    
    
    
    int sopen = ui->sbOpen->value();
    int sext = ui->sbExt->value();
    int smatch = ui->sbMatch->value();
    int scgap = ui->sbCgap->value();
    
    if( sopen > 0 || sext > 0 || smatch < 1 || scgap > 0 ) {
        QMessageBox::StandardButton b = QMessageBox::warning( this, "Weird Scoring Scheme", "The scoring scheme you selected seems weird\nThe results may be even weirder. Continue?", QMessageBox::Ok|QMessageBox::Cancel );
        
        if( b != QMessageBox::Ok ) {
            return; // bail out
        }
        
    }
    
//     std::cout << "x: " << smatch + scgap << "\n";
    if( smatch + scgap >= 0 ) {
        QMessageBox::StandardButton b = QMessageBox::warning( this, "Scoring Scheme Trouble", "The scoring scheme you selected does not work with the current version of PaPaRa because match+match_cgap < 0. This is no fundamental restriction of the algorithm but necessary for a specific kind of performance optimization.\nPlease chose a smaller match score or a smaller match cgap score.\n\nIf you think that this is not acceptable, please write me at simberger@gmail.com", QMessageBox::Cancel );
        

        return; // bail out

        
    }
    ui->frButtons->setEnabled(false);
    ui->frRun->setEnabled(false);
   
    progress_dialog_ = new QProgressDialog( "Doing the papara", "cancel (not really)", 0, 1 );
    progress_dialog_->setMinimumDuration(0);
    
    
    papara::papara_score_parameters op = papara_->scoring_parameters();
    papara::papara_score_parameters np(sopen, sext, smatch, scgap );
    
    // check if the scoring scheme was changed and clear scoring_results if necessary
    if( !scoring_result_.isNull() && op != np ) {
//         QMessageBox::StandardButton b = QMessageBox::warning( this, "Weird Scoring Scheme", "clear results", QMessageBox::Ok );
        
        scoring_result_.clear();
        
    }
    
    papara_->set_scoring_parameters( np );
    
    
   // QThread *thread = new QThread;
    scoring_worker_.reset( new scoring_worker( ui->pte_log, papara_, scoring_result_, ui->cbRefGaps->isChecked() ));
    //obj is a pointer to a QObject that will trigger the work to start. It could just be this

    
    
    scoring_worker_->moveToThread(bg_thread_.data());
//     thread->start();
    
    connect(scoring_worker_.data(), SIGNAL(done( QSharedPointer<output_alignment_store>, QSharedPointer<papara::scoring_results>, QString)), this, SLOT(on_scoring_done(QSharedPointer<output_alignment_store>, QSharedPointer<papara::scoring_results>, QString)));
    
    QMetaObject::invokeMethod(scoring_worker_.data(), "doWork", Qt::QueuedConnection);
    //obj will need to emit startWork() to get the work going.

    
}

void MainWidget::on_state_ready(QSharedPointer< papara_state > state, QString msg) {
    if( progress_dialog_ != 0 ) {
        delete progress_dialog_;
    }
    
    std::cout << "state_ready: " << state.data() << "\n";
    
    if( state.isNull() ) {
        QMessageBox::critical(this,"Internal Error", msg );
               
        abort();
    }
    
    papara_ = state;
   // table_model_.set_papara_state( state );

   /* ui->tv_alignment->setModel(&table_model_);
    ui->tv_alignment->horizontalHeader()->hide();
    ui->tv_alignment->verticalHeader()->hide();
   */ 
//     ui->tv_alignment->resizeColumnsToContents();
//     ui->tv_alignment->resizeRowsToContents();
    
//     resize_rows_columns(ui->tv_alignment, 12, 12 );
    setEnabled(true);
    
    ui->pte_log->appendPlainText("papara static state initialized");
    
    ui->frRun->setEnabled(true);

    QScrollArea *qs = new QScrollArea();
    
    
    PhyloTreeView *ptv = new PhyloTreeView(papara_->tree());
    
    QGraphicsView *gv = new QGraphicsView( ptv->initGraphicsScene(0) );
    
    qs->setWidget(gv);
    
    qs->setVisible(true);
    
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

void MainWidget::on_scoring_done(QSharedPointer<output_alignment_store> oa, QSharedPointer<papara::scoring_results> res, QString msg ) {
    if( progress_dialog_ != 0 ) {
        delete progress_dialog_;
    }
    
    if( oa.isNull() ) {
        QMessageBox::critical(this,"Internal Error", msg );
               
        abort();
    }
    
    if( scoring_result_.isNull() ) {
        scoring_result_ = res;
    }
    
    
    assert( scoring_result_.data() == res );
    
    output_alignment_ = oa;
//     qs_table_model_.set_oas(oa);
//     ref_table_model_.set_oas(oa);

//     QScopedPointer<TextGridModel> qs_model(new alignment_grid_model(oa, false));
//     QScopedPointer<TextGridModel> ref_model(new alignment_grid_model(oa, true));
    
    qs_grid_model_ = QSharedPointer<TextGridModel> (new alignment_grid_model(oa, false));
    ref_grid_model_ = QSharedPointer<TextGridModel> (new alignment_grid_model(oa, true));
    
//     float zoom_factor = ui->cbZoom->
    
    {
//         int hs = -1;
//         int vs = -1;
        if( tg_ref_ == 0 ) {
            tg_ref_ = new TextGrid(sv_ref_);
            sv_ref_->setWidget(tg_ref_);
        }
        
        
        
//         tg_ref_->setModel(ref_model.data());

        tg_ref_->setModel(ref_grid_model_);
        tg_ref_->repaint();
        
//         sv_qs_->setLayout(new);
    }
    
    {
        
        if( tg_qs_ == 0 ) {
            tg_qs_ = new TextGrid(sv_qs_);
            sv_qs_->setWidget(tg_qs_);
        }
        //tg_qs_->setModel(qs_model.data());
        tg_qs_->setModel(qs_grid_model_);
        tg_qs_->repaint();
    }

    tg_ref_->setZoom(ui->slZoom->value());
    tg_qs_->setZoom(ui->slZoom->value());
    connect(tg_ref_, SIGNAL(zoomChanged(int)), tg_qs_, SLOT(setZoom(int)));
    connect(tg_qs_, SIGNAL(zoomChanged(int)), tg_ref_, SLOT(setZoom(int)));
    connect(tg_ref_, SIGNAL(zoomChanged(int)), ui->slZoom, SLOT(setValue(int)));
    connect(tg_qs_, SIGNAL(zoomChanged(int)), ui->slZoom, SLOT(setValue(int)));
    connect(ui->slZoom, SIGNAL(valueChanged(int)), tg_ref_, SLOT(setZoom(int)));
    connect(ui->slZoom, SIGNAL(valueChanged(int)), tg_qs_, SLOT(setZoom(int)));
    
    int hvalue = -1;
    
    if( sv_qs_->widget() != 0 ) {
//         old_num_cols = ui->tv_qs->model()->columnCount();
    
        hvalue = sv_qs_->horizontalScrollBar()->value();
    }
    
//     ui->tv_qs->setModel(0);
//     ui->tv_qs->setModel(&qs_table_model_);
//     ui->tv_qs->horizontalHeader()->hide();
//     ui->tv_qs->verticalHeader()->hide();
//     ui->tv_qs->resizeColumnsToContents();
//     ui->tv_qs->resizeRowsToContents();
//     
/*
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
    }*/
    
//     if( hvalue >= 0 ) {
//         ui->tv_qs->horizontalScrollBar()->setValue(hvalue);
//     }



    ui->pte_log->appendPlainText("scoring done");
	ui->pbRun->setPalette( QPalette() );

    ui->pbRun->setAutoFillBackground(false);
    ui->frButtons->setEnabled(true);
    ui->frRun->setEnabled(true);
    
    ui->pbSaveAs->setEnabled(true);
    
}


void state_worker::doWork()
{
    
    try {
        std::cout << "doWork\n";
        papara_state *ps = new papara_state( qpte_, tree_.data(), ref_.data(), qs_.data());
        
        ps->do_preprocessing();
        
        emit done(QSharedPointer<papara_state>(ps), QString());
        
        finished_ = true;
    } catch( std::runtime_error x ) {
        QString error_text(x.what());
        QString msg( "Critical error while loading input files:\n" );
        msg += error_text;
        
        
        emit done( QSharedPointer<papara_state>(), msg );
    } 
}

void scoring_worker::doWork() {
    try {
        
        if( res_.isNull() ) {
            res_ = QSharedPointer<papara::scoring_results>(state_->do_scoring_only());
        }
        
        assert( res_ != 0 );
        
        output_alignment_store *oa = state_->do_scoring( *res_, ref_gaps_ );
        
        emit done(QSharedPointer<output_alignment_store>(oa), res_, QString());
        
        finished_ = true;
    } catch( std::runtime_error x ) {
        QString error_text(x.what());
        QString msg( "Critical error during alignment:\n" );
        msg += error_text;
        
        emit done(QSharedPointer<output_alignment_store>(), res_, msg);
    }
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
                 return QBrush( QColor( 128, 128, 255 ) );
             case 'C':
                 return QBrush( QColor( 255, 128, 128 ) );
             case 'G':
                 return QBrush( QColor( 255, 255, 128 ) );
             case 'T':
                 return QBrush( QColor( 128, 255, 128 ) );
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
void MainWidget::on_cbZoom_activated(int idx) {
//     if( tg_qs_ != 0 ) {
//         tg_qs_->setZoom(ui->cbZoom->itemData(idx).toFloat());
//     }
//     
//     if( tg_ref_ != 0 ) {
//         tg_ref_->setZoom(ui->cbZoom->itemData(idx).toFloat());
//     }
    
    
}
void MainWidget::invalidateScores() {
//     std::cout << "invalidate\n";
    
	ui->pbRun->setPalette(QPalette(Qt::red));
    ui->pbRun->setAutoFillBackground(true);
    
}


void MainWidget::check_filenames() { 
    if( is_readable( tree_filename_ ) && is_readable( ref_filename_ ) && is_readable( qs_filename_ ) ) {
        ui->pbLoad->setEnabled(true);
    }
    
}

qt_thread_guard::qt_thread_guard(QThread* thread) : thread_(thread) {}

qt_thread_guard::~qt_thread_guard() {
    thread_->quit();
    thread_->wait();
}
void MainWidget::on_pbSaveAs_clicked() {
    if( output_alignment_.isNull() ) {
        QMessageBox::critical( this, "Internal Error", "SaveAs requested with no valid output alignment" );
        abort();
    }
    
     QString filename = QFileDialog::getSaveFileName(this);
     
     
     if( !filename.isEmpty() ) {
//          std::ofstream os( filename.toStdString() );
         
         std::string t( filename.toStdString() );
         
         output_alignment_->write_phylip( t.c_str() );
         
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
