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