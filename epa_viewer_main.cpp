/*
 * Copyright (C) 2009-2012 Simon A. Berger
 * 
 * This file is part of epa_viewer.
 * 
 *  epa_viewer is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  epa_viewer is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with epa_viewer.  If not, see <http://www.gnu.org/licenses/>.
 */


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