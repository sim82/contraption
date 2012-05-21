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
    explicit LargeAli(QWidget *parent = 0);
    ~LargeAli();

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