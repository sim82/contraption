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


#ifndef main_h
#define main_h
#include <QWizard>


class QLabel;
class QLineEdit;
class QCheckBox;
class FileSelector;

class LoadWizardPage : public QWizardPage {
Q_OBJECT
public: 
    LoadWizardPage() ;
    virtual ~LoadWizardPage() ;
    QString getTree() ;

    QString getRef() ;
    
    QString getQuery() ;
    bool isProtein();
    
private:
    FileSelector *fsTree;
    FileSelector *fsRef;
    FileSelector *fsQuery;
    QCheckBox *cbProtein;
};


#endif