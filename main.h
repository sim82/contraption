#ifndef main_h
#define main_h
#include <QWizard>


class QLabel;
class QLineEdit;
class FileSelector;

class LoadWizardPage : public QWizardPage {
Q_OBJECT
public: 
    LoadWizardPage() ;
    virtual ~LoadWizardPage() ;
    QString getTree() ;

    QString getRef() ;
    
    QString getQuery() ;
    
private:
    FileSelector *fsTree;
    FileSelector *fsRef;
    FileSelector *fsQuery;
};


#endif