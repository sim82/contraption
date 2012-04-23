#include "papara.h"

#include "dialog.h"
#include "ui_dialog.h"
#include <QFileDialog>
#include <QProgressDialog>
#include <QThread>
#include <QLabel>
#include <QTableView>
streambuf_to_q_plain_text_edit::streambuf_to_q_plain_text_edit( QPlainTextEdit *qpte, std::size_t buff_sz, std::size_t put_back )
:
  put_back_(std::max(put_back, size_t(1))),
  buffer_(std::max(buff_sz, put_back_) + put_back_)
{

    QObject::connect( this, SIGNAL(post_text(QString)), qpte, SLOT(appendPlainText(const QString &)), Qt::QueuedConnection );
}



void streambuf_to_q_plain_text_edit::append( int of, char *first, char *last ) {
    size_t size = std::distance(first, last);

    assert( size < 100000 );
    QByteArray ba( first, size );

    QString s;
    if( of != 0 ) {

        const char b[2] = {of, 0};
        s.append( b );

    }
    s.append(ba);

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



class papara_state {
public:

    papara_state( QPlainTextEdit *qpte, const std::string &tree_name, const std::string &ref_name, const std::string &qs_name )
        : sbq_(qpte),
          log_file_( "contraption_log.txt"),
          ost_( &sbq_ ),
          ldev( ost_, log_file_ ),
          lout_guard(  papara::lout, ldev ),
          qs_(qs_name),
          refs_(tree_name.c_str(), ref_name.c_str(), &qs_ ) {


        qpte->appendPlainText( "xxx");
        qpte->appendPlainText( "yyy\n");
//        papara::lout << "bla bla bla" << std::endl;
    }

    void do_preprocessing() {

        papara::lout << "qs preprocess" << std::endl;
        qs_.preprocess();


        papara::lout << "ref preprocess" << std::endl;
        refs_.remove_full_gaps();
        refs_.build_ref_vecs();
        papara::lout << "done." << std::endl;
    }

    virtual ~papara_state() {}



    const papara::references<pvec_pgap,papara::tag_dna> &refs() const {
        return refs_;
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
};

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    table_model_(0)
{
    ui->setupUi(this);


   // ui->tv_alignment->setModel( &table_model_ );

    tree_filename_ = "/home/sim/src_exelixis/contraption/test_1604/RAxML_bestTree.ref_orig";
    ref_filename_ = "/home/sim/src_exelixis/contraption/test_1604/orig.phy.1";
    qs_filename_ = "/home/sim/src_exelixis/contraption/test_1604/qs.fa.20";

}

Dialog::~Dialog()
{
    delete ui;
}

// converting from QString to const char* is ridiculusly complicated. so store string as when conversion is necessary std::string...
static std::string de_q_string( QString qs ) {
    std::string s = qs.toStdString();

    return s;
}

void Dialog::on_pb_tree_clicked()
{
    //QFileDialog::exec
    std::cout << "tree\n";
    tree_filename_ = de_q_string(QFileDialog::getOpenFileName(this));
    ui->pte_log->appendPlainText(tree_filename_.c_str());
}

void Dialog::on_pb_ref_clicked()
{
    std::cout << "ref\n";
    ref_filename_ = de_q_string(QFileDialog::getOpenFileName(this));
    ui->pte_log->appendPlainText(ref_filename_.c_str());
}

void Dialog::on_pb_qs_clicked()
{
    std::cout << "qs\n";
    qs_filename_ = de_q_string(QFileDialog::getOpenFileName(this));
    ui->pte_log->appendPlainText(qs_filename_.c_str());
}






void Dialog::on_buttonBox_accepted()
{
    if( tree_filename_.empty() || ref_filename_.empty() || qs_filename_.empty() ) {
        ui->pte_log->appendPlainText( "file missing\n");
        return;
    }


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


void Dialog::on_state_ready(papara_state *state) {
    papara_.reset( state );
    table_model_.set_papara_state( state );

    ui->tv_alignment->setModel(&table_model_);
    ui->tv_alignment->horizontalHeader()->hide();
    
    ui->tv_alignment->resizeColumnsToContents();
    ui->tv_alignment->resizeRowsToContents();
    setEnabled(true);

    ui->pte_log->appendPlainText("papara static state initialized");
}



void state_worker::doWork()
{
   papara_state *ps = new papara_state( qpte_, tree_.data(), ref_.data(), qs_.data());

   ps->do_preprocessing();

   emit done(ps);
}



alignment_table_model::alignment_table_model(QObject *parent )
: papara_state_(0)
{


}

int alignment_table_model::rowCount(const QModelIndex &parent ) const {
    if( papara_state_ == 0 ) {
        return 0;
    } else {
        return papara_state_->refs().num_seqs();
        
    }

}

int alignment_table_model::columnCount(const QModelIndex &parent ) const {
    if( papara_state_ == 0 ) {
        return 0;

    } else {
        return papara_state_->refs().seq_at(0).size();
    }

}

QVariant alignment_table_model::data(const QModelIndex &index, int role ) const {
    
    
    
    if( papara_state_ != 0 ) {
    switch(role){
    case Qt::DisplayRole:
        return QString((char)papara_state_->refs().seq_at(index.row()).at(index.column()));
        
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
QVariant alignment_table_model::headerData(int section, Qt::Orientation orientation, int role) const {
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
