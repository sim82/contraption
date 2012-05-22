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
    FileSelector( QWidget *parent = 0 ) : QWidget( parent ) {
        
        QBoxLayout *l = new QBoxLayout( QBoxLayout::LeftToRight );
        
       
        
        icWarning = style()->standardIcon(QStyle::SP_MessageBoxWarning);
//         pmInfo = QMessageBox::standardIcon(QMessageBox::Warning);
        laWarning = new QLabel;
        laWarning->setPixmap(icWarning.pixmap(24));
        laWarning->setToolTip( "File not readable" );
        leFile = new QLineEdit;
        leFile->setReadOnly(true);
        
        pbDialog = new QPushButton(QString("..."));
        pbDialog->setObjectName(QString::fromUtf8("pbDialog"));
//         QSizePolicy sp1( QSizePolicy::Expanding, QSizePolicy::Preferred );
//         QSizePolicy sp2( QSizePolicy::Preferred, QSizePolicy::Preferred );
//         leFile->setSizePolicy(sp1);
//         pbDialog->setSizePolicy(sp2);
        l->addWidget( laWarning );
        l->addWidget( leFile );
        l->addWidget( pbDialog );
        
        setLayout(l);
        QMetaObject::connectSlotsByName(this);
    }
    QString getFilename() {
//         std::cout << "text: " << leFile->text().toStdString() << "\n";
        
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