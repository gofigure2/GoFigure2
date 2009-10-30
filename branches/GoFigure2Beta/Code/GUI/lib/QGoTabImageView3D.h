#ifndef __QGoTabImageView3D_h
#define __QGoTabImageView3D_h

#include "QGoTabImageViewElementBase.h"
#include "SnapshotHelper.h"

#include <QHBoxLayout>
#include <QSpinBox>

class vtkImageData;

class QAction;
class QToolBar;
class QDockWidget;

class QGoImageView3D;

class QGoTabImageView3D : public QGoTabImageViewElementBase
{
  Q_OBJECT
  public:
    QGoTabImageView3D( QWidget* parent = 0 );
    virtual ~QGoTabImageView3D();

    GoFigure::TabDimensionType GetTabDimensionType( ) const;

    virtual void Update();

    void setupUi( QWidget* parent );
    void retranslateUi( QWidget *parent );

    virtual std::list< QWidget* > AdditionalWidget()
      {return std::list< QWidget* >(); }

    virtual void WriteSettings() {}
    virtual void ReadSettings() {}

  signals:
    void SliceViewXYChanged( int Slice );
    void SliceViewXZChanged( int Slice );
    void SliceViewYZChanged( int Slice );

    void FullScreenViewChanged( int View );

  public slots:
    QString SnapshotViewXY( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) );
    QString SnapshotView2( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) );
    QString SnapshotView3( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) );
    QString SnapshotViewXYZ( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) );

    void SetSliceViewXY( const int& );
    void SetSliceViewXZ( const int& );
    void SetSliceViewYZ( const int& );

    void SetFullScreenView( const int& iS );
    void Quadview();
    void FullScreenViewXY();
    void FullScreenViewXZ();
    void FullScreenViewYZ();
    void FullScreenViewXYZ();

    void ChangeLookupTable();
    void ShowScalarBar( const bool& );

  protected:
    QGoImageView3D*   m_ImageView;

    void GetBackgroundColorFromImageViewer( );
    void SetBackgroundColorToImageViewer( );
    void CreateAllViewActions();

    void SetImageToImageViewer( vtkImageData* image );
    int* GetImageCoordinatesFromWorldCoordinates( double pos[3] );

    virtual void RemoveActorFromViewer( const int& iId, vtkActor* iActor );
    virtual void DisplayActorInViewer( const int& iId, vtkActor* iActor );

    virtual std::vector< vtkActor* > AddContour( const int& iId,
      vtkPolyData* dataset,
      vtkProperty* property = NULL );

//   private:
};
#endif
