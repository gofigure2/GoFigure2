#ifndef __QGoImageView_h
#define __QGoImageView_h

#include <QWidget>

class vtkLookupTable;
class vtkImageData;
class vtkViewImage2DCollection;

class QGoImageView : public QWidget
{
  Q_OBJECT

  public:
    QGoImageView( QWidget* parent = 0 );
    virtual ~QGoImageView();

    virtual void SetImage( vtkImageData* iImage ) = 0;
    virtual void Update() = 0;

    virtual void setupUi( QWidget* parent ) = 0;
    virtual void retranslateUi(QWidget *parent) = 0;

    void GetBackgroundColor( double& r, double& g, double& b );
    double* GetBackgroundColor();

  public slots:
    void SetBackgroundColor( const double& r,
      const double& g,
      const double& b );
    void SetBackgroundColor( double rgb[3] );
    void SetBackgroundColor( const QColor& iColor );

    virtual void SetLookupTable( vtkLookupTable* iLut ) = 0;
    virtual void ShowScalarBar( const bool& ) = 0;

  protected:
    vtkViewImage2DCollection*         m_Pool;
    vtkImageData*                     m_Image;
    unsigned int                      m_SnapshotId;
};

#endif
