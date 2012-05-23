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
// #include <iostream>

class FileSelector : public QWidget {
 Q_OBJECT
 
public:
    FileSelector( QWidget *parent = 0 ) ;
    QString getFilename() const {
        return leFile->text();
    }
    
public Q_SLOTS:
    void on_pbDialog_clicked() ;
    void changeFilename( QString filename ) ;
    
private:
    
    QIcon icWarning;
    QLabel *laWarning;
    QPushButton *pbDialog;
    QLineEdit *leFile;
    
};

#endif