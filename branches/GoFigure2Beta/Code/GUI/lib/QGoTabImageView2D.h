#ifndef __QGoTabImageView2D_h
#define __QGoTabImageView2D_h

class vtkImageData;

class QAction;
class QToolBar;
class QDockWidget;

#include "QGoTabImageViewElementBase.h"

#include "QGoImageView2D.h"

/**
\class QGoTabImageView2D
\brief Element of the QTabWidget to be used to visualized 2D images.
*/
class QGoTabImageView2D : public QGoTabImageViewElementBase
{
  Q_OBJECT
public:
  QGoTabImageView2D( QWidget* parent = 0 );
  virtual ~QGoTabImageView2D();

  GoFigure::TabDimensionType GetTabDimensionType( ) const;

  virtual void Update();

  void setupUi( QWidget* parent );
  void retranslateUi( QWidget *parent );

  virtual std::list< QWidget* > AdditionalWidget();

  virtual void WriteSettings();
  virtual void ReadSettings();

public slots:
  void ChangeLookupTable();
  void ShowScalarBar( const bool& );

protected:
  QGoImageView2D*   m_ImageView;

  void GetBackgroundColorFromImageViewer( );
  void SetBackgroundColorToImageViewer( );

  void SetImageToImageViewer( vtkImageData* image );
  int* GetImageCoordinatesFromWorldCoordinates( double pos[3] );
  std::vector< vtkActor* > AddContour( const int& iId, vtkPolyData* dataset,
      vtkProperty* property = NULL );

  virtual void RemoveActorFromViewer( const int& iId, vtkActor* iActor );
  virtual void DisplayActorInViewer( const int& iId, vtkActor* iActor );

private:
  QGoTabImageView2D( const QGoTabImageView2D& );
  void operator = ( const QGoTabImageView2D& );
};
#endif
