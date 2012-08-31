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


#ifndef LARGE_ALI_H
#define LARGE_ALI_H

#include <QObject>
#include <QWidget>
#include <iostream>
#include <QSharedPointer>
#include "TextGrid.h"
#include "ivymike/LargePhylip.h"


namespace Ui {
    class LargaliMain;
}



class LargeAli : public QWidget
{
    Q_OBJECT

public:
    explicit LargeAli(const char *filename = 0, QWidget *parent = 0);
    ~LargeAli();
    void open_file( const std::string &filename );
public Q_SLOTS:
	void on_pbLoad_clicked();

private Q_SLOTS:
    
private:
    QScopedPointer<Ui::LargaliMain> ui;
   
    QScopedPointer<TextGrid> text_grid_;
    
  
    
    
    QSharedPointer<TextGridModel> grid_model_;
	QScopedPointer<LargePhylip> large_phy_;
  
};



#endif