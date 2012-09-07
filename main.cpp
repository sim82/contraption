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

#include <QPlainTextEdit>
#include <QScrollArea>

#include <QWizard>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <iostream>
#include "main.h"
#include "papara_nt/papara.h"
#include "main_widget.h"
#include "TextGrid.h"
#include "FileSelector.h"

// class TestModel : public TextGridModel {
// public:
//     virtual QSize size() {
//         return QSize( 2000, 2000 );
//     };
//     virtual QChar data( size_t x, size_t y ) {
//         return 'A';
//     }
//     virtual QColor color( size_t x, size_t y ) {
//         return Qt::red;
//         
//     };
// };

namespace papara {
log_stream lout;
}


QString random_text() {
    QString os;
    
    QChar temp[4] = {'A','C','G','T'};
    for( size_t i = 0; i < 2000; ++i ) {
        for(size_t j = 0; j < 2000; ++j ) {
            os.append( temp[(i+j)%4] );
            
        }
        
        os.append('\n');
    }
    
    return os;
}


class MyWidget : public QWidget {
protected:
    void paintEvent( QPaintEvent * e ) {
        QRect r = e->rect();
        
        std::cout << r.x() << " " << r.y() <<  " " << r.width() << " " << r.height() << "\n";
        
    }
private:
    
};


int main( int argc, char *argv[] ) {


    QApplication a(argc, argv);
    
    QWizard wizard;
//     wizard.set

    LoadWizardPage *loadPage = new LoadWizardPage;
    
    
    wizard.addPage(loadPage);
    wizard.setModal(true);
    int res = wizard.exec();
    
    if( res == 0 ) {
        return -1;
    }
    QString treeName = loadPage->getTree();
    QString refName = loadPage->getRef();
    QString queryName = loadPage->getQuery();
    bool is_protein = loadPage->isProtein();
    
    
    QString blastFilename = loadPage->getBlastFile();
    QString partitionName = loadPage->getPartitionFile();
    
    if( !loadPage->isPerPartition() ) {
        // empty filenames are used to indicate that the option is disabled. make sure that they are really emapty.
        blastFilename.clear();
        partitionName.clear();
    }
    
//     std::cout << "tree: " << treeName.toStdString() << "\n";
    
    MainWidget w( treeName, refName, queryName, is_protein, blastFilename, partitionName );

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

    w.post_show_stuff();
    
    return a.exec();

}

#ifdef WIN32
int CALLBACK WinMain(
  __in  HINSTANCE hInstance,
  __in  HINSTANCE hPrevInstance,
  __in  LPSTR lpCmdLine,
  __in  int nCmdShow
)
{
	char *argv[1] = {"test"};
	return main( 1, argv );
}
#endif
LoadWizardPage::LoadWizardPage() : QWizardPage() {
    setTitle("Load Input Files");
    //page->setSubTitle("Please fill both fields.");

    QLabel *laTree = new QLabel( "Reference Tree" );
    fsTree = new FileSelector( new file_validator_newick );

    QLabel *laRef = new QLabel( "Reference Alignment" );
    fsRef = new FileSelector( new file_validator_phylip );

    QLabel *laQuery = new QLabel( "Query Sequences" );
    fsQuery = new FileSelector( new file_validator_fasta );


    QLabel *laProtein = new QLabel( "Protein Data:" );
    //fsQuery = new FileSelector( new file_validator_fasta );
    cbProtein = new QCheckBox();
    
    
    QLabel *laPerPartition = new QLabel( "Per-partition alignment:" );
    //fsQuery = new FileSelector( new file_validator_fasta );
    cbPerPartition = new QCheckBox();
    
    QLabel *laBlast = new QLabel( "Blast output:" );
    //fsQuery = new FileSelector( new file_validator_fasta );
    fsBlast = new FileSelector();
    fsBlast->setEnabled(false);
    
    
    QLabel *laPartition = new QLabel( "Partition file:" );
    //fsQuery = new FileSelector( new file_validator_fasta );
    fsPartition = new FileSelector();
    fsPartition->setEnabled(false);
    
    
    QGridLayout *layout = new QGridLayout;

    connect( cbPerPartition, SIGNAL(toggled(bool)), fsBlast, SLOT(setEnabled(bool)));
    connect( cbPerPartition, SIGNAL(toggled(bool)), fsPartition, SLOT(setEnabled(bool)));
    layout->addWidget(laTree, 0, 0 );
    layout->addWidget(fsTree, 0, 1 );

    layout->addWidget(laRef, 1, 0 );
    layout->addWidget(fsRef, 1, 1 );

    layout->addWidget(laQuery, 2, 0 );
    layout->addWidget(fsQuery, 2, 1 );

    layout->addWidget(laProtein, 3, 0);
    layout->addWidget(cbProtein, 3, 1);
    
    
    layout->addWidget(laPerPartition, 4, 0);
    layout->addWidget(cbPerPartition, 4, 1);
    
    layout->addWidget(laBlast, 5, 0 );
    layout->addWidget(fsBlast, 5, 1 );
    
    layout->addWidget(laPartition, 6, 0 );
    layout->addWidget(fsPartition, 6, 1 );
    
#ifndef WIN32
//     fsTree->changeFilename( "/space/projects/2012_robert_454/RAxML_bestTree.cora_Sanger_reference_alignment.tre" );
//     
//     fsRef->changeFilename( "/space/projects/2012_robert_454/cora_Sanger_reference_alignment.phy" );
//     fsQuery->changeFilename( "/space/projects/2012_robert_454/cluster_52_72_cora_inversa_squamiformis_DIC_148_149.fas" );
    
    fsTree->changeFilename( "/home/sim/src_exelixis/papara_nt/test_1604/RAxML_bestTree.ref_orig" );
    
    fsRef->changeFilename( "/home/sim/src_exelixis/papara_nt/test_1604/orig.phy.1" );
    fsQuery->changeFilename( "/home/sim/src_exelixis/papara_nt/test_1604/qs.fa.200" );
    
    fsBlast->changeFilename("/home/sim/src_exelixis/papara_nt/test.blast");
    fsPartition->changeFilename("/home/sim/src_exelixis/papara_nt/test.model");
    
#else
    fsTree->changeFilename( "C:/2012_robert_454/RAxML_bestTree.cora_Sanger_reference_alignment.tre" );
    
    fsRef->changeFilename( "C:/2012_robert_454/cora_Sanger_reference_alignment.phy" );
    fsQuery->changeFilename( "C:/2012_robert_454/cluster_52_72_cora_inversa_squamiformis_DIC_148_149.fas" );
#endif
    
    setLayout(layout);

}
QString LoadWizardPage::getTree() {
    return fsTree->getFilename();
}
QString LoadWizardPage::getRef() {
    return fsRef->getFilename();

}
QString LoadWizardPage::getQuery() {
    return fsQuery->getFilename();
}

bool LoadWizardPage::isProtein() {
    return cbProtein->isChecked();
}

LoadWizardPage::~LoadWizardPage() {
//     std::cout << "~LoadWizardPage()\n";
}
QString LoadWizardPage::getBlastFile()
{
    return fsBlast->getFilename();
}
QString LoadWizardPage::getPartitionFile()
{
    return fsPartition->getFilename();
}
bool LoadWizardPage::isPerPartition()
{
    return cbPerPartition->isChecked();
}
