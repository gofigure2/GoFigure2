#ifndef __QGoImageView3D_h
#define __QGoImageView3D_h

#include "QGoImageView.h"
#include "SnapshotHelper.h"

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QSplitter>
#include <QSlider>
#include <QSpacerItem>

#include <map>


class QVTKWidget;
class vtkEventQtSlotConnect;
class QSplitterChild;

class vtkProp3D;
class vtkProperty;
class vtkImageData;
class vtkViewImage2D;
class vtkViewImage3D;
class vtkViewImage2DCollection;

/**
\class QGoImageView3D
\brief class for the visualization of 3D Image represented by one vtkImageData*.
\example GUI/lib/qgoimageview3d.cxx
*/
class QGoImageView3D : public QGoImageView
{
  Q_OBJECT
public:
  /** \brief Constructor by default */
  explicit QGoImageView3D( QWidget* parent = 0 );

  /** \brief Destructor. */
  virtual ~QGoImageView3D();

  /** \brief Set the image to be displaid. */
  virtual void SetImage( vtkImageData* iImage );

  /** \brief Get the displaid image.*/
  virtual vtkImageData* GetImage();

  virtual void Update();

  vtkViewImage3D* GetImageViewer3D();

  vtkImageActor* GetImageActor( const int& );
  QVTKInteractor* GetInteractor( const int& );

  virtual void RemoveActor( const int& iId, vtkActor* iActor );
  virtual void AddActor( const int& iId, vtkActor* iActor );

  void AddMesh( vtkPolyData* iMesh );

  std::vector< vtkActor* > AddContour( const int& iId,
      vtkPolyData* dataset,
      vtkProperty* property = NULL );

  virtual void setupUi( QWidget* parent );
  virtual void retranslateUi(QWidget *parent);

  int GetFullScreenView( ) const;

  void SaveStateSplitters();

  int GetSliceViewXY() const;
  int GetSliceViewXZ() const;
  int GetSliceViewYZ() const;

  virtual void ChangeActorProperty( vtkProp3D* iActor, vtkProperty* iProperty );
  virtual void ChangeActorProperty( int iDir, vtkProp3D* iActor, vtkProperty* iProperty );

  std::list< vtkProp3D* > GetListOfPickedActors();
  std::list< vtkProp3D* > GetListOfUnPickedActors();

  void TestPool();

signals:
  void SliceViewXYChanged( int Slice );
  void SliceViewXZChanged( int Slice );
  void SliceViewYZChanged( int Slice );

  void FullScreenViewChanged( int View );
  void ActorsSelectionChanged();

public slots:
  QString SnapshotViewXY( const GoFigure::FileType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QString SnapshotView2( const GoFigure::FileType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QString SnapshotView3( const GoFigure::FileType& iType,
    const QString& iBaseName = QString( "snapshot" ) );
  QString SnapshotViewXYZ( const GoFigure::FileType& iType,
    const QString& iBaseName = QString( "snapshot" ) );

  void SetSliceViewXY( const int& );
  void SetSliceViewXZ( const int& );
  void SetSliceViewYZ( const int& );

  void SetFullScreenView( const int& iS );

//   void HighLightContours();

  /**
   *
   * @param
   */
  virtual void SetLookupTable( vtkLookupTable* );

  virtual void ShowScalarBar( const bool& );

protected:
  QSplitter*          VSplitter;
  QSplitterChild*     HtSplitter;
  QSplitterChild*     HbSplitter;

  QWidget*      LayOutWidget1;
  QHBoxLayout*  LayOut1;
  QSlider*      SliderXY;
  QVTKWidget*   QvtkWidget_XY;

  QWidget*      LayOutWidget2;
  QHBoxLayout*  LayOut2;
  QSlider*      SliderXZ;
  QVTKWidget*   QvtkWidget_XZ;

  QWidget*      LayOutWidget3;
  QHBoxLayout*  LayOut3;
  QSlider*      SliderYZ;
  QVTKWidget*   QvtkWidget_YZ;

  QWidget*      LayOutWidget4;
  QHBoxLayout*  LayOut4;
  QVTKWidget*   QvtkWidget_XYZ;
  QSpacerItem*  Spacer;

  vtkViewImage3D* View3D;

  vtkEventQtSlotConnect*    VtkEventQtConnector;
  int                       IsFullScreen;
  bool                      m_FirstRender;

  vtkProperty* m_HighlightedContourProperty;

  virtual void resizeEvent( QResizeEvent* event );

  void SetupVTKtoQtConnections();
  std::map< vtkProp3D*, vtkProperty* > m_ActorsPropertyMap;

  void Quadview();
  void FullScreenViewXY();
  void FullScreenViewXZ();
  void FullScreenViewYZ();
  void FullScreenViewXYZ();

protected slots:
  void MoveSliderXY();
  void MoveSliderXZ();
  void MoveSliderYZ();

};
#endif
