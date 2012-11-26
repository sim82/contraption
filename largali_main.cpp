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


#include <QtGui/QApplication>
#include "LargeAli.h"

int main( int argc, char *argv[] ) {


    QApplication a(argc, argv);
    const char *filename = 0;
    if( argc == 2 ) {
        filename = argv[1];
    }
    LargeAli w(filename);

//     if(false) {
//         TestModel tm;
//         
//         TextGrid *mw = new TextGrid;
//         mw->setModel( &tm );
//         QScrollArea sa;
//         sa.setWidget(mw);
//         
//         //     QScrollArea sa;
//         sa.resize(300,200);
//         
//         
//         sa.show();
//     }
    
    w.show();

    
    
    return a.exec();

}
