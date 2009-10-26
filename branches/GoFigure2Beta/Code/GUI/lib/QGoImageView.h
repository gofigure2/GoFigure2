#ifndef __QGoImageView_h
#define __QGoImageView_h

#include <QWidget>

class vtkLookupTable;
class vtkImageData;
class vtkViewImage2D;
class vtkViewImage2DCollection;
class vtkImageActor;
class vtkActor;
class vtkDataSet;
class vtkProperty;
class QVTKInteractor;

/**
\class QGoImageView
\brief Abstract class for image visualization.
*/
class QGoImageView : public QWidget
{
  Q_OBJECT

  public:
    explicit QGoImageView( QWidget* parent = 0 );
    virtual ~QGoImageView();

    void SetTag( const QString& iTag );
    QString GetTag() const;

    virtual void SetImage( vtkImageData* iImage ) = 0;
    virtual vtkImageData* GetImage() = 0;

    /** \brief Get Image Coordinates from World Coordinates*/
    int* GetImageCoordinatesFromWorldCoordinates( double pos[3] );

    virtual void Update() = 0;

    virtual vtkImageActor* GetImageActor( const int& ) = 0;
    virtual QVTKInteractor* GetInteractor( const int& ) = 0;

    virtual void setupUi( QWidget* parent ) = 0;
    virtual void retranslateUi(QWidget *parent) = 0;

    void GetBackgroundColor( double& r, double& g, double& b );
    double* GetBackgroundColor();

    vtkViewImage2D* GetImageViewer( const int& iId );
    int GetNumberOfImageViewers();

    virtual std::vector< vtkActor* > AddDataSet( vtkDataSet* dataset,
      vtkProperty* property = NULL,
      const bool& intersection = true,
      const bool& iDataVisibility = true );

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
    QString                           m_Tag;
};

#endif
