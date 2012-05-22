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
