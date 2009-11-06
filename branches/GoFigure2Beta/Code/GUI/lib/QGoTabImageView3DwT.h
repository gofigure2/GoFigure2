#ifndef __QGoTabImageView3DwT_h
#define __QGoTabImageView3DwT_h

#include "QGoTabElementBase.h"

#include "QGoPlugin.h"

#include "GoFigureFileInfoMultiIndexContainerHelper.h"
#include "itkMegaCaptureReader.h"
#include "SnapshotHelper.h"
#include "ContourStructureHelper.h"

#include <QHBoxLayout>
#include <QSpinBox>

class QGoImageView3D;
class QGoVisualizationDockWidget;
class QGoManualSegmentationDockWidget;
class vtkLSMReader;
class vtkContourWidget;
class vtkOrientedGlyphContourRepresentation;

/**
\class QGoTabImageView3DwT
\brief
\example GUI/lib/qgotabimageview3dwt.cxx
*/
class QGoTabImageView3DwT : public QGoTabElementBase
{
  Q_OBJECT
public:
  QGoTabImageView3DwT( QWidget* parent = 0 );
  virtual ~QGoTabImageView3DwT();

  GoFigure::TabDimensionType GetTabDimensionType( ) const;

  void SetLSMReader( vtkLSMReader* iReader, const int& iTimePoint );
  void SetMegaCaptureFile(
    const GoFigureFileInfoHelperMultiIndexContainer& iContainer,
    const GoFigure::FileType& iFileType,
    const std::string& iHeader,
    const unsigned int& iTimePoint  );

  virtual void Update();

  void setupUi( QWidget* parent );
  void retranslateUi( QWidget *parent );

  virtual std::list< QDockWidget* > DockWidget();

  virtual std::list< QWidget* > AdditionalWidget()
    {return std::list< QWidget* >(); }

  virtual void WriteSettings() {}
  virtual void ReadSettings() {}

  virtual void ValidateContour( const int& iId );

signals:
  void TimePointChanged( int TimePoint );
  void SliceViewXYChanged( int Slice );
  void SliceViewXZChanged( int Slice );
  void SliceViewYZChanged( int Slice );
  void FullScreenViewChanged( int FullScreen );

public slots:
  void SetTimePoint( const int& );

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
  void Quadview();
  void FullScreenViewXY();
  void FullScreenViewXZ();
  void FullScreenViewYZ();
  void FullScreenViewXYZ();

  void ChangeLookupTable();
  void ShowScalarBar( const bool& );
  void ChangeBackgroundColor();

  void ShowAllChannels( bool iChecked );
  void ShowOneChannel( int iChannel );

  void ActivateManualSegmentationEditor( const bool& iActivate );
  void ValidateContour();
  void ChangeContourRepresentationProperty();

protected:
  QHBoxLayout*          m_LayOut;
  QGoImageView3D*       m_ImageView;
  std::vector< vtkLSMReader* > m_LSMReader;
  vtkImageData*         m_Image;

  itk::MegaCaptureReader::Pointer           m_MegaCaptureReader;
  GoFigureFileInfoHelperMultiIndexContainer m_FileList;
  GoFigure::FileType    m_FileType;
  QColor                m_BackgroundColor;
  int                   m_TimePoint;
  unsigned int          m_ContourId;

  QGoVisualizationDockWidget*       m_VisuDockWidget;
  QGoManualSegmentationDockWidget*  m_ManualSegmentationDockWidget;

  std::vector< vtkContourWidget* >                      m_ContourWidget;
  std::vector< vtkOrientedGlyphContourRepresentation* > m_ContourRepresentation;
  ContourStructureMultiIndexContainer                   m_ContourContainer;

  void GetBackgroundColorFromImageViewer( );
  void SetBackgroundColorToImageViewer( );
  void CreateAllViewActions();
  void CreateVisuDockWidget();
  void CreateManualSegmentationdockWidget();

  void RemoveActorFromViewer( const int& iId, vtkActor* iActor );
  void DisplayActorInViewer( const int& iId, vtkActor* iActor );

  int* GetImageCoordinatesFromWorldCoordinates( double pos[3] );

  std::vector< vtkActor* > AddContour( const int& iId,
      vtkPolyData* dataset,
      vtkProperty* property = NULL );

  void RemoveAllContoursForPresentTimePoint();
  void LoadAllContoursForGivenTimePoint( const unsigned int& iT );

  void SetTimePointWithLSMReaders( const int& iTimePoint );
  void SetTimePointWithMegaCapture( const int& iTimePoint );
};

#endif
