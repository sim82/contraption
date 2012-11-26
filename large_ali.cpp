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


#include <QWidget>

#include <string>

#include <QFileDialog>
#include <QScopedPointer>
#include <QScrollArea>
#include "large_ali.h"
#include "ui_large_ali.h"
#include "TextGrid.h"

class largali_model : public TextGridModel {
public:
	largali_model( LargePhylip *large_phy ) : large_phy_(large_phy) {

	}

	virtual QSize size() {
		size_t width = large_phy_->getSeqLen();
		size_t height = large_phy_->size();

		return QSize( width, height );
	}

	virtual QChar data( size_t x, size_t y ) {
		u1_t *sfirst = large_phy_->getSequenceBegin(y);
		assert( sfirst != 0 );
		return sfirst[x];
	}

	virtual QColor color( size_t x, size_t y ) {
		u1_t *sfirst = large_phy_->getSequenceBegin(y);
		assert( sfirst != 0 );

		u1_t c = toupper(sfirst[x]);
        
        const bool protein = true;
        if( !protein ) {
            // nucleotice color mapping
            switch( c ) {
                case 'A':
                    return QColor( 255, 0, 0 );
                case 'C': 
                    return QColor( 0, 255, 0 );
                case 'G':
                    return QColor( 0, 0, 255 );
                case 'T':
                    return QColor( 255, 200, 0 );
                default:
                    return Qt::white;
            }
        } else {
        
            // protein color mapping
            
            switch( c ) {
                case 'A':
                case 'I':   
                case 'L':
                case 'M':
                case 'F':
                case 'W':
                case 'V':
                    return Qt::blue;
                    
                case 'R':
                case 'K':
                    return Qt::red;
                    
                case 'N':
                    return Qt::green;
                    
                case 'C':
                    return Qt::blue;
                    
                case 'Q':
                    return Qt::green;
                    
                case 'E':
                    return Qt::magenta;
                    
                case 'D':
                    return Qt::magenta;
                    
                case 'G':
                    return QColor(255,200,0);
                    
                case 'H':
                case 'Y':
                    return Qt::cyan;
                    
                case 'P':
                    return Qt::yellow;
                    
                case 'S':
                case 'T':
                    return Qt::green;
                    
                default:
                    return Qt::white;
                    
                    
                    
            }   
        }
        
#if 0
		switch( c ) {
		case 'A':
		case 'I':	
		case 'L':
		case 'M':
		case 'F':
		case 'W':
		case 'V':
			return Qt::blue;

		case 'R':
		case 'K':
			return Qt::red;

		case 'N':
			return Qt::green;

		case 'C':
			return Qt::blue;

		case 'Q':
			return Qt::green;

		case 'E':
			return Qt::magenta;

		case 'D':
			return Qt::magenta;

		case 'G':
			return QColor(255,200,0);
			
		case 'H':
		case 'Y':
			return Qt::cyan;

		case 'P':
			return Qt::yellow;

		case 'S':
		case 'T':
			return Qt::green;

		default:
			return Qt::white;



		}
#endif

		
	}

private:
	LargePhylip *large_phy_;
	
};

LargeAli::LargeAli(const char* filename, QWidget* parent) :
	QWidget(parent),
		ui(new Ui::LargaliMain)
{
	ui->setupUi(this);
    
    if( filename != 0 ) {
        open_file( filename );
    }
}
LargeAli::~LargeAli() {}

static std::string de_q_string( QString qs ) {
    std::string s = qs.toStdString();

    return s;
}

void LargeAli::open_file( const std::string &filename ) {
    
    
    try {
        large_phy_.reset( new LargePhylip(filename.c_str() ));
        //std::cout << "name 0: " << large_phy_->getName(0) << "\n";

        grid_model_ = QSharedPointer<TextGridModel> (new largali_model(large_phy_.data()));


        text_grid_.reset(new TextGrid());
        text_grid_->setModel(grid_model_);
    
        
        connect( ui->slZoom, SIGNAL(valueChanged(int)), text_grid_.data(), SLOT(setZoom(int)));
        connect( text_grid_.data(), SIGNAL(zoomChanged(int)), ui->slZoom, SLOT(setValue(int)));
        ui->saAlignment->setWidget(text_grid_.data());
    } catch( boost::interprocess::interprocess_exception x ) {
        std::cerr << "caught: " << x.what() << "\n";
        abort();
    }
}

void LargeAli::on_pbLoad_clicked() {
	
    
    open_file( de_q_string(QFileDialog::getOpenFileName(this)) );
	
}
