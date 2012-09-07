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


#ifndef DIALOG_H
#define DIALOG_H
#include <QObject>
#include <QDialog>
#include <iostream>
#include <QScopedPointer>
#include <QAbstractTableModel>
#include <QScrollArea>
#include <string>
#include <cassert>
#include <deque>
#include "TextGrid.h"
#include "PhyloTreeView.h"


class QPlainTextEdit;
class QProgressDialog;
class QTableView;
class QThread;

namespace Ui {
    class MainWidget;
}

namespace papara {
    class scoring_results;
}


    
class output_alignment_store;
class papara_state;

Q_DECLARE_METATYPE(QSharedPointer<papara_state>)
Q_DECLARE_METATYPE(QSharedPointer<papara::scoring_results> )
Q_DECLARE_METATYPE(QSharedPointer<output_alignment_store> )


class qt_thread_guard {
public:
    qt_thread_guard( QThread *thread ) ;
    ~qt_thread_guard() ;
    
    QThread *data() {
        return thread_.data();
    }
    
    QThread *operator->() { return thread_.operator->(); }
    
private:
    qt_thread_guard();
    qt_thread_guard( const qt_thread_guard &);
    qt_thread_guard &operator=( qt_thread_guard &);
    
    //QThread * const thread_;
    QScopedPointer<QThread> thread_;
};



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
    state_worker( QPlainTextEdit *qpte, const std::string &tree, const std::string &ref, const std::string &qs, bool is_protein, const std::string &pg_blast, const std::string &pg_partitions ) 
    : qpte_(qpte),
    tree_(tree), 
    ref_(ref), 
    qs_(qs), 
    pg_blast_(pg_blast),
    pg_partitions_(pg_partitions),
    finished_(false), 
    is_protein_(is_protein) 
    {}

    virtual ~state_worker() { 
        std::cout << "~state_worker\n";
        if( !finished_ ) {qWarning("~state_worker: not finished!\n");}
        
    }

public Q_SLOTS:
    void doWork();
Q_SIGNALS:
    void done( QSharedPointer<papara_state>, QString );

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
    
    std::string pg_blast_;
    std::string pg_partitions_;
    
    bool finished_;
    bool is_protein_;
};


class scoring_worker : public QObject
{
    Q_OBJECT

public:
    scoring_worker( QPlainTextEdit *qpte, QSharedPointer<papara_state> state, QSharedPointer<papara::scoring_results> res, bool ref_gaps ) 
    : qpte_(qpte), state_(state), res_(res), ref_gaps_(ref_gaps), finished_(false) {}

    virtual ~scoring_worker() { 
        std::cout << "~scoring_worker\n";
        if( !finished_ ) {qWarning("~scoring_woker: not finished!\n");}
    }

public Q_SLOTS:
    void doWork();
Q_SIGNALS:
    void done( QSharedPointer<output_alignment_store>, QSharedPointer<papara::scoring_results>, QString msg );

private:

//    std::string log_filename( "contraption.log" );
//    std::ofstream logs( log_filename.c_str());
//    if( !logs ) {
//        std::cout << "could not open logfile for writing: " << log_filename << std::endl;
//        return 0;
//    }


    QPlainTextEdit *qpte_;

    QSharedPointer<papara_state> state_;
    QSharedPointer<papara::scoring_results> res_;
    bool ref_gaps_;
    
    bool finished_;
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

#if 0
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
    QVariant x_;
};
#endif
class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget( QString treeName, QString refName, QString queryName, bool is_protein, QString pgBlastName, QString pgPartitionsName, QWidget* parent = 0 );
    ~MainWidget();

    void post_show_stuff();

public Q_SLOTS:
    void accept() {
        std::cout << "accept\n";
    }

private Q_SLOTS:
    void on_pb_tree_clicked();

    void on_pb_ref_clicked();

    void on_pb_qs_clicked();

    
    void on_state_ready(QSharedPointer< papara_state > state, QString msg );
    void on_scoring_done( QSharedPointer< output_alignment_store > oa, QSharedPointer< papara::scoring_results > res, QString msg ) ;
    void on_pbLoad_clicked();
    void on_pbRun_clicked() ;
    void on_cbRefGaps_stateChanged( int s ) ; 
    void on_cbZoom_activated( int idx );
    
    void on_sbOpen_valueChanged( int v ) { invalidateScores(); }
    void on_sbExt_valueChanged( int v ) { invalidateScores(); }
    void on_sbMatch_valueChanged( int v ) { invalidateScores(); }
    void on_sbCgap_valueChanged( int v ) { invalidateScores(); }
    
    void on_pbSaveAs_clicked() ;
private:
    void showLog( bool v );
    void check_filenames() ;
    void invalidateScores() ;
    
    void resize_rows_columns( QTableView *tv, int row_size, int column_size );
    
    Ui::MainWidget *ui;

    QProgressDialog *progress_dialog_;
    
    QSharedPointer<papara_state> papara_;
    QSharedPointer<output_alignment_store> output_alignment_;
    QSharedPointer<papara::scoring_results> scoring_result_;
    
    
    qt_thread_guard bg_thread_;
    QScopedPointer<scoring_worker> scoring_worker_;
    QScopedPointer<state_worker> state_worker_;
    
//     TextGrid *tg_ref_;
//     TextGrid *tg_qs_;
//     
    TextGrid *tg_ref_;
    TextGrid *tg_qs_;
    
    
    QScrollArea *sv_ref_;
    QScrollArea *sv_qs_;
    
    
    QSharedPointer<TextGridModel> ref_grid_model_;
    QSharedPointer<TextGridModel> qs_grid_model_;
    
    
//     raw_alignment_table_model table_model_;
//     alignment_table_model qs_table_model_;
//     alignment_table_model ref_table_model_;
    
//     QThread bg_aligner_thread_;
    
    std::string tree_filename_;
    std::string ref_filename_;
    std::string qs_filename_;
    const bool is_protein_;
    int log_size_;
    std::string pg_blast_name_;
    std::string pg_partitions_name_;
};

#endif // DIALOG_H
