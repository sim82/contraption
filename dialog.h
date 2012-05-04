#ifndef DIALOG_H
#define DIALOG_H
#include <QObject>
#include <QDialog>
#include <iostream>
#include <QScopedPointer>
#include <QAbstractTableModel>
#include <string>
#include <cassert>
#include <deque>

class QPlainTextEdit;
class QProgressDialog;
class QTableView;

namespace Ui {
    class Dialog;
}

namespace papara {
    class scoring_results;
}



class output_alignment_store;
class papara_state;



class streambuf_to_q_plain_text_edit : public QObject, public std::streambuf
{
Q_OBJECT
public:
    explicit streambuf_to_q_plain_text_edit( QPlainTextEdit *qpte, std::size_t buff_sz = 80, std::size_t put_back = 8);


private:

    void append( int of, char *first, char *last );
    // overrides base class over()
    int_type overflow(int c);

    int sync();

    // copy ctor and assignment not implemented;
    // copying not allowed
    streambuf_to_q_plain_text_edit(const streambuf_to_q_plain_text_edit &);
    streambuf_to_q_plain_text_edit &operator= (const streambuf_to_q_plain_text_edit &);


Q_SIGNALS:
    void post_text( QString );
private:

    const std::size_t put_back_;
    std::vector<char> buffer_;
};


class state_worker : public QObject
{
    Q_OBJECT

public:
    state_worker( QPlainTextEdit *qpte, const std::string &tree, const std::string &ref, const std::string &qs ) : qpte_(qpte), tree_(tree), ref_(ref), qs_(qs) {}

    virtual ~state_worker() { std::cout << "~state_worker\n";}

public Q_SLOTS:
    void doWork();
Q_SIGNALS:
    void done( papara_state * );

private:

//    std::string log_filename( "contraption.log" );
//    std::ofstream logs( log_filename.c_str());
//    if( !logs ) {
//        std::cout << "could not open logfile for writing: " << log_filename << std::endl;
//        return 0;
//    }


    QPlainTextEdit *qpte_;

    std::string tree_;
    std::string ref_;
    std::string qs_;
};


class scoring_worker : public QObject
{
    Q_OBJECT

public:
    scoring_worker( QPlainTextEdit *qpte, papara_state *state, papara::scoring_results *res, bool ref_gaps ) 
    : qpte_(qpte), state_(state), res_(res), ref_gaps_(ref_gaps) {}

    virtual ~scoring_worker() { std::cout << "~scoring_worker\n";}

public Q_SLOTS:
    void doWork();
Q_SIGNALS:
    void done( output_alignment_store *, papara::scoring_results * );

private:

//    std::string log_filename( "contraption.log" );
//    std::ofstream logs( log_filename.c_str());
//    if( !logs ) {
//        std::cout << "could not open logfile for writing: " << log_filename << std::endl;
//        return 0;
//    }


    QPlainTextEdit *qpte_;

    papara_state *state_;
    papara::scoring_results *res_;
    bool ref_gaps_;
};

// class bg_align_worker : public QObject {
//     Q_OBJECT
// public:
//     bg_align_worker( const papara_state *state, QThread *thread );
//     
//     virtual ~bg_align_worker() {}
//     
// public Q_SLOTS:
//     align( size_t qs_idx ) ;
//     
//     
// public Q_SIGNAL:
//     align_done( size_t qs_idx, std::vector<uint8_t> seq ) ;
// private:
//     const size_t num_qs_;
// //     std::deque<size_t> queue_;
//     const papara_state *state_;
// };

class raw_alignment_table_model : public QAbstractTableModel
{
     Q_OBJECT
public:

    raw_alignment_table_model(QObject *parent );
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const ;

    void set_papara_state( papara_state *pstate ) { papara_state_ = pstate; }
private:
    papara_state *papara_state_;
};

class alignment_table_model : public QAbstractTableModel {
    Q_OBJECT
public:
    alignment_table_model( QObject *parent, bool use_ref = false ) ;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
    void set_oas( const output_alignment_store *oas ) {
        oas_ = oas;
    }
    
private:
    const output_alignment_store *oas_;
    const bool use_ref_;
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
public Q_SLOTS:
    void accept() {
        std::cout << "accept\n";
    }

private Q_SLOTS:
    void on_pb_tree_clicked();

    void on_pb_ref_clicked();

    void on_pb_qs_clicked();

    
    void on_state_ready(papara_state *);
    void on_scoring_done( output_alignment_store *oa, papara::scoring_results *res ) ;
    void on_pbLoad_clicked();
    void on_pbRun_clicked() ;
    void on_cbRefGaps_stateChanged( int s ) ; 
private:
    void resize_rows_columns( QTableView *tv, int row_size, int column_size );
    
    Ui::Dialog *ui;

    QProgressDialog *progress_dialog_;
    
    QScopedPointer<papara_state> papara_;
    QScopedPointer<output_alignment_store> output_alignment_;
    QScopedPointer<papara::scoring_results> scoring_result_;
    
    
    raw_alignment_table_model table_model_;
    alignment_table_model qs_table_model_;
    alignment_table_model ref_table_model_;
    
//     QThread bg_aligner_thread_;
    
    std::string tree_filename_;
    std::string ref_filename_;
    std::string qs_filename_;


};

#endif // DIALOG_H
