#include "FileSelector.h"

void FileSelector::on_pbDialog_clicked() {
    QString fn = QFileDialog::getOpenFileName(this);

    changeFilename(fn);
    
    
}
void FileSelector::changeFilename(QString filename) {

    if( QFile::exists(filename) ) {
        laWarning->setVisible(false);
    }

    leFile->setText(filename);
}
FileSelector::FileSelector(QWidget* parent) : QWidget( parent ) {

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
