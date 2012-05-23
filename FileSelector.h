#ifndef FileSelector_h
#define FileSelector_h

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QStyle>
#include <iostream>

class file_validator {
public:
    virtual ~file_validator() {}
    virtual bool is_valid( QString filename ) = 0;
    virtual QString file_type() = 0;
    
};


class file_validator_fasta : public file_validator {
public:
    virtual bool is_valid( QString filename ) {
        
        
        QFile f(filename);
        f.open(QIODevice::ReadOnly);
        
        
//         std::cout << "is_valid: " << f.isOpen() << "\n";
        char c;
        
        if( f.getChar(&c) ) {
            return c == '>';
        }
        
        return false;
    }
    
    virtual QString file_type() {
        return QString( "fasta" );
    }
    
};


class file_validator_newick : public file_validator {
public:
    virtual bool is_valid( QString filename ) {
        
        QFile f(filename);
        f.open(QIODevice::ReadOnly);
        
        
//         std::cout << "is_valid: " << f.isOpen() << "\n";
        char c;
        
        if( f.getChar(&c) ) {
            return c == '(';
        }
        
        return false;
    }
    
    virtual QString file_type() {
        return QString( "newick tree" );
    }
    
};

class file_validator_phylip : public file_validator {
public:
    virtual bool is_valid( QString filename ) {
        return true; // TODO: quick validation for phylip
    }
    
    virtual QString file_type() {
        return QString( "phylip" );
    }
    
};



class FileSelector : public QWidget {
 Q_OBJECT
 
public:
    // WARNING: the file_validator object will be owned by the FileSelector (waiting for c++11 and proper unique_pointer...)
    FileSelector( file_validator *validator = 0, QWidget *parent = 0 ) ;
    QString getFilename() const {
        return leFile->text();
    }
    
public Q_SLOTS:
    void on_pbDialog_clicked() ;
    void changeFilename( QString filename ) ;
    
private:
    QScopedPointer<file_validator> file_validator_;
    QIcon icWarning;
    QLabel *laWarning;
    QPushButton *pbDialog;
    QLineEdit *leFile;
    
};

#endif