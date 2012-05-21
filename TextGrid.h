#ifndef __TextGrid_h
#define __TextGrid_h


#include <QWidget>
#include <QStaticText>
#include <QSharedPointer>
#include <vector>
#include <iostream>

class QScrollArea;

class TextGridModel {
  
public:
    virtual ~TextGridModel() ;
    
    virtual QSize size() = 0;
    virtual QChar data( size_t x, size_t y ) = 0;
    //virtual QBrush brush( size_t x, size_t y ) = 0;
    virtual QColor color( size_t x, size_t y ) = 0;
    
};

class TextGrid : public QWidget {
    Q_OBJECT
    
    class scroll_state {
    public:
        scroll_state( const QPoint &p ) : sp_(p) {}
        scroll_state() : sp_(0,0) {}
        QPoint update( const QPoint &p ) {
            QPoint d = p - sp_;
            sp_ = p;
            
            return d;
        }
        
    private:
        QPoint sp_;
    };
    
    class modifier_state {
    public:
        modifier_state() : ctrl_(false) {}
        
        void press( QKeyEvent * e ) ;
        
        void release( QKeyEvent * e ) ;
        
        bool ctrl() const {
            return ctrl_;
        }
        
    private:
        bool ctrl_;
    };
    
public:
    TextGrid( QWidget* w = 0, int zoom_factor = 100, QSize cell_size = QSize( 11, 14 )) ; 
    
    virtual ~TextGrid() {
        std::cout << "~TextGrid\n";
    }
    
    void setModel( QSharedPointer<TextGridModel> model ) ;
    
    void paintEvent( QPaintEvent *e ) ;
    
    
public Q_SLOTS:
    void setZoom( int zoom );
    
Q_SIGNALS:
    void zoomChanged( int factor );
    
protected:
    void wheelEvent( QWheelEvent *e );
      
private:
    void updateSize();
    
    inline float zoomf() const { return zoom_factor_ * 0.01; }
    
    QScrollArea *parentScrollArea() ;
//     QPoint cellByPos( const QPoint &p ) {
//         
//     }
    
    
    //TextGridModel *model_;
    QSharedPointer<TextGridModel> model_;
    QSize model_size_;
    const QSize cell_size_; // base cell size
    int zoom_factor_;
    
    std::vector<QStaticText> stext_;
    
    QFont f;
    scroll_state ss_;
    modifier_state ms_;
};


#endif