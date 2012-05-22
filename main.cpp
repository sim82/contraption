#include <QtGui/QApplication>

#include <QPlainTextEdit>
#include <QScrollArea>

#include <QWizard>
#include <QLabel>
#include <QGridLayout>
#include <QLineEdit>
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
  
//     std::cout << "tree: " << treeName.toStdString() << "\n";
    
    MainWidget w( treeName, refName, queryName );

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

    QLabel *nameLabel = new QLabel("Name:");
    QLineEdit *nameLineEdit = new QLineEdit;

    QLabel *emailLabel = new QLabel("Email address:");
    QLineEdit *emailLineEdit = new QLineEdit;

    QLabel *laTree = new QLabel( "Reference Tree" );
    fsTree = new FileSelector;

    QLabel *laRef = new QLabel( "Reference Alignment" );
    fsRef = new FileSelector;

    QLabel *laQuery = new QLabel( "Query Sequences" );
    fsQuery = new FileSelector;


    QGridLayout *layout = new QGridLayout;


    layout->addWidget(laTree, 0, 0 );
    layout->addWidget(fsTree, 0, 1 );

    layout->addWidget(laRef, 1, 0 );
    layout->addWidget(fsRef, 1, 1 );

    layout->addWidget(laQuery, 2, 0 );
    layout->addWidget(fsQuery, 2, 1 );

    
#ifndef WIN32
    fsTree->changeFilename( "/space/projects/2012_robert_454/RAxML_bestTree.cora_Sanger_reference_alignment.tre" );
    
    fsRef->changeFilename( "/space/projects/2012_robert_454/cora_Sanger_reference_alignment.phy" );
    fsQuery->changeFilename( "/space/projects/2012_robert_454/cluster_52_72_cora_inversa_squamiformis_DIC_148_149.fas" );
    
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
LoadWizardPage::~LoadWizardPage() {
    std::cout << "~LoadWizardPage()\n";
}
