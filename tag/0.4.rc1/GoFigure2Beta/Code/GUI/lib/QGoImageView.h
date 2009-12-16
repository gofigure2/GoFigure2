#ifndef __QGoImageView_h
#define __QGoImageView_h

#include <QWidget>

class vtkLookupTable;
class vtkImageData;
class vtkViewImage2D;
class vtkViewImage2DCollection;
class vtkImageActor;
class vtkActor;
class vtkPolyData;
class vtkProperty;
class vtkProp3D;
class QVTKInteractor;

/**
\class QGoImageView
\brief Abstract class for the visualization of 3D Image represented by one vtkImageData*.
*/
class QGoImageView : public QWidget
{
  Q_OBJECT

  public:
    /** \brief Default Constructor. */
    explicit QGoImageView( QWidget* parent = 0 );

    /** \brief Destructor. */
    virtual ~QGoImageView();

    /** \brief Set the image to displaid. */
    virtual void SetImage( vtkImageData* iImage ) = 0;

    /** \brief Get the displaid image.*/
    virtual vtkImageData* GetImage() = 0;

    /** \brief Get Image Coordinates from World Coordinates. */
    int* GetImageCoordinatesFromWorldCoordinates( double pos[3] );

    virtual void Update() = 0;

    /** \brief Returns Image actor rendered on one given view. */
    virtual vtkImageActor* GetImageActor( const int& ) = 0;

    /** \brief Returns the interactor for one given view. */
    virtual QVTKInteractor* GetInteractor( const int& ) = 0;

    virtual void setupUi( QWidget* parent ) = 0;
    virtual void retranslateUi(QWidget *parent) = 0;

    /** \brief Returns used background color by viewers.
        \param[out] r red
        \param[out] g green
        \param[out] b blue    */
    void GetBackgroundColor( double& r, double& g, double& b );
    /** \overload */
    double* GetBackgroundColor();

    vtkViewImage2D* GetImageViewer( const int& iId );
    int GetNumberOfImageViewers();

    virtual void RemoveActor( const int& iId, vtkActor* iActor );
    virtual void AddActor( const int& iId, vtkActor* iActor );


    virtual std::vector< vtkActor* > AddContour( const int& iId,
      vtkPolyData* dataset,
      vtkProperty* property = NULL );

    /** \brief Highlight a given contour (iProp) if iToDo is true. */
    virtual void ChangeActorProperty( vtkProp3D* iActor, vtkProperty* iProperty );

  public slots:
    /** \brief Set background color for all views.
        \param[in] r red
        \param[in] g green
        \param[in] b blue    */
    void SetBackgroundColor( const double& r,
      const double& g,
      const double& b );
    /** \overload */
    void SetBackgroundColor( double rgb[3] );
    /** \overload */
    void SetBackgroundColor( const QColor& iColor );

    virtual void SetLookupTable( vtkLookupTable* iLut ) = 0;
    virtual void ShowScalarBar( const bool& ) = 0;

  protected:
    vtkViewImage2DCollection*         m_Pool;
    vtkImageData*                     m_Image;
    unsigned int                      m_SnapshotId;
};

#endif
