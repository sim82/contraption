#include <QApplication>
#include <QFileDialog>
#include "EPAViewerWidget.h"
#include "PhyloTreeView.h"



int main( int argc, char *argv[] ) {
    
    QApplication app(argc, argv);
        
    std::string filename = QFileDialog::getOpenFileName(0,0,"/space/projects/2011_12_micah", "*.jplace").toStdString();
    
    
    if( filename.empty() ) {
        return 0;
    }
    //     const char *filename = "/space/projects/2011_12_micah/RAxML_portableTree.colpodea.jplace";
    
    EPAViewerWidget evw(filename.c_str());
    
    evw.show();
    
    app.exec();
   
}