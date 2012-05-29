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


#include "FileSelector.h"

void FileSelector::on_pbDialog_clicked() {
    QString fn = QFileDialog::getOpenFileName(this);

    changeFilename(fn);
    
    
}
void FileSelector::changeFilename(QString filename) {

    bool valid = QFile::exists(filename) && (file_validator_.isNull() || file_validator_->is_valid(filename));
    
    
    
    if( !valid ) {
        QString expected_type( "unknown" );
        if( !file_validator_.isNull() ) {
            expected_type = file_validator_->file_type();
        }
        
        icWarning = style()->standardIcon(QStyle::SP_MessageBoxCritical);
//         pmInfo = QMessageBox::standardIcon(QMessageBox::Warning);
        laWarning->setPixmap(icWarning.pixmap(24));
        laWarning->setToolTip( "The file you have selected is either not readable or of the wrong type\nExpected file type: " + expected_type );
    }
    laWarning->setVisible(!valid);
    
    

    leFile->setText(filename);
}
FileSelector::FileSelector( file_validator* validator, QWidget* parent) 
: QWidget( parent ),
  file_validator_(validator)

{

    QBoxLayout *l = new QBoxLayout( QBoxLayout::LeftToRight );



//     icWarning = style()->standardIcon(QStyle::SP_MessageBoxWarning);
    icWarning = style()->standardIcon(QStyle::SP_MessageBoxQuestion);
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
