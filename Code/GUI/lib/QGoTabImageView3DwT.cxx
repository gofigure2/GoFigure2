/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "QGoTabImageView3DwT.h"
#include "QDebug"
#include "QShortcut"

#include "QGoImageView3D.h"
//#include "QGoLUTDialog.h"
#include "QGoNavigationDockWidget.h"
#include "QGoTransferFunctionDockWidget.h"

#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )

#include "QGoVideoRecorder.h"

#endif /* ENABLEVIDEORECORD */

#include "SnapshotHelper.h"

#include "GoDBContourRow.h"
#include "GoDBCoordinateRow.h"
#include "GoDBMeshRow.h"

#include "vtkLSMReader.h"

#include "vtkImageData.h"
#include "vtkActor.h"
#include "vtkLookupTable.h"
#include "vtkImageAppendComponents.h"
#include "vtkOrientedGlyphContourRepresentation.h"
#include "vtkContourWidget.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkImageActorPointPlacer.h"
#include "SelectQueryDatabaseHelper.h"
#include "ConvertToStringHelper.h"

#include "vtkViewImage2D.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkPolyData.h"

#include "vtkPolyDataMapper.h"
#include "vtkOutlineFilter.h"

//VTK FILTERS
#include "vtkImageExport.h"

//ITK FILTERS
#include "itkvtkPolyDataToGoFigureMeshAttributes.h"
#include "ContourToMeshFilter.h"

#include "GoFigureMeshAttributes.h"

#include <QCursor>

#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QInputDialog>
#include <QProgressDialog>

#include "vtkBox.h"
#include "vtkClipPolyData.h"

#include <set>

// base segmentation dock widgets
#include "QGoMeshEditingWidgetManager.h"
#include "QGoContourEditingWidgetManager.h"

// track dockwidget
#include "QGoTrackViewDockWidget.h"

// lineage dockwidget
#include "QGoLineageViewDockWidget.h"

// lineage viwer
#include "QGoLineageViewerWidget.h"

// image processors
#include "GoMegaImageProcessor.h"
#include "GoLSMImageProcessor.h"

// transfer function editor
#include "GoTransferFunctionEditorWidget.h"

//-------------------------------------------------------------------------
QGoTabImageView3DwT::QGoTabImageView3DwT(QWidget *iParent) :
  QGoTabElementBase(iParent),
  m_ImageProcessor(NULL),
  m_BackgroundColor(Qt::black),
  m_TraceSettingsToolBar(NULL),
  m_IntersectionLineWidth(2.),
  m_PCoord(0),
  m_RCoord(0),
  m_CCoord(0),
  m_XTileCoord(0),
  m_YTileCoord(0),
  m_ZTileCoord(0),
  m_TCoord(-1),
  m_MeshEditingWidget(NULL),
  m_Seeds( 3 )
  //m_TraceWidgetRequiered(false)
{
  //m_Image = vtkImageData::New();

  m_Seeds[0] = vtkPoints::New();
  m_Seeds[1] = vtkPoints::New();
  m_Seeds[2] = vtkPoints::New();

  m_VolumeRenderingEnabled = false;

  m_HighlightedContoursProperty = vtkProperty::New();
  m_HighlightedContoursProperty->SetColor(1., 0., 0.);
  m_HighlightedContoursProperty->SetOpacity(1.);
  m_HighlightedContoursProperty->SetLineWidth(3.);

  m_HighlightedMeshesProperty = vtkProperty::New();
  m_HighlightedMeshesProperty->SetColor(1., 0., 0.);
  m_HighlightedMeshesProperty->SetOpacity(1.);
  m_HighlightedMeshesProperty->SetSpecular(0.6);
  m_HighlightedMeshesProperty->SetLineWidth(3.);
  m_HighlightedMeshesProperty->SetInterpolationToPhong();

  m_IntersectionLineWidth = 2.;

  setupUi(this);

  m_MegaCaptureReader = itk::MegaCaptureReader::New();

  m_ContourContainer = new ContourContainer(this, this->m_ImageView);
  m_ContourContainer->SetHighlightedProperty(m_HighlightedContoursProperty);
  m_ContourContainer->SetIntersectionLineWidth(m_IntersectionLineWidth);

  m_MeshContainer = new MeshContainer(this, this->m_ImageView);
  m_MeshContainer->SetHighlightedProperty(m_HighlightedMeshesProperty);
  m_MeshContainer->SetIntersectionLineWidth(m_IntersectionLineWidth);

  m_TrackContainer = new TrackContainer(this, this->m_ImageView);
  m_TrackContainer->SetHighlightedProperty(m_HighlightedMeshesProperty);
  m_TrackContainer->SetIntersectionLineWidth(m_IntersectionLineWidth);

  m_LineageContainer = new LineageContainer(this, this->m_ImageView);

  CreateVisuDockWidget();

  // create TF dockwidget
  m_TransferFunctionDockWidget =
    new QGoTransferFunctionDockWidget(this);


  // segmentation dockwidgets
  //CreateContourSegmentationDockWidget();
  //CreateMeshSegmentationDockWiget();in setmegacapture/LSM files now

  // track dock widget
  m_TrackViewDockWidget = new QGoTrackViewDockWidget(this);
  this->m_TrackViewDockWidget->setObjectName("TrackViewDockWidget");

  QObject::connect( m_TrackViewDockWidget,
                    SIGNAL( ChangeColorCode(const QString&) ),
                    m_TrackContainer,
                    SLOT( ChangeColorCode(const QString&) ) );

  QObject::connect( m_TrackViewDockWidget,
                    SIGNAL( UpdateTracksRepresentation(const double&, const double&) ),
                    m_TrackContainer,
                    SLOT( UpdateTracksRepresentation(const double&, const double&) ) );

  // lineage dock widget
  m_LineageViewDockWidget = new QGoLineageViewDockWidget(this);
  this->m_LineageViewDockWidget->setObjectName("LineageViewDockWidget");

  QObject::connect( m_LineageViewDockWidget,
                    SIGNAL( ChangeDivisionsColorCode(const QString&) ),
                    m_TrackContainer,
                    SLOT( ChangeDivisionsColorCode(const QString&) ) );

  // lineage viewer
  m_QGoLineageViewerWidget =  new QGoLineageViewerWidget(NULL);
  m_QGoLineageViewerWidget->setObjectName("QGoLineageViewerWidget");

  CreateDataBaseTablesConnection();

#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  CreateVideoRecorderWidget();
#endif

  CreateAllViewActions();

  CreateToolsActions();

  CreateBookmarkActions();

  //CreateModeActions(); in setmegacapture/LSM files now
  //CreateTracesActions();

  ReadSettings();

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(
        m_NavigationDockWidget, Qt::RightDockWidgetArea, false, true, this),
      m_NavigationDockWidget) );

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(
        m_TransferFunctionDockWidget,
        Qt::RightDockWidgetArea, false, true, this),
      m_TransferFunctionDockWidget) );

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(this->m_TrackViewDockWidget,
                              Qt::LeftDockWidgetArea, false, true, this),
      this->m_TrackViewDockWidget) );


  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(this->m_LineageViewDockWidget,
                              Qt::LeftDockWidgetArea, false, true, this),
      this->m_LineageViewDockWidget) );

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
    new QGoDockWidgetStatus(this->m_DataBaseTables,
    Qt::TopDockWidgetArea, false, true, this),
                              this->m_DataBaseTables) );

#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(m_VideoRecorderWidget, Qt::LeftDockWidgetArea, false, true, this),
      m_VideoRecorderWidget) );

#endif

  SetUpShortcuts();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTabImageView3DwT::
~QGoTabImageView3DwT()
{
  m_HighlightedContoursProperty->Delete();
  m_HighlightedMeshesProperty->Delete();

  m_Seeds[0]->Delete();
  m_Seeds[1]->Delete();
  m_Seeds[2]->Delete();
  // new
  /*
  std::vector<vtkPoints*>::iterator it = m_OrderedSeeds.begin();
  while(it != m_OrderedSeeds.end())
  {
    (*it)->Delete();
    (*it) = NULL;
    ++it;
  }

  */
  unsigned int minch = m_MegaCaptureReader->GetMinChannel();
  unsigned int maxch = m_MegaCaptureReader->GetMaxChannel();

  for( unsigned int i = minch; i < maxch + 1; i++ )
    {
    vtkImageData *temp = m_MegaCaptureReader->GetOutput(i);
    if ( temp )
      {
      temp->Delete();
      temp = NULL;
      }
    }

  if ( m_ContourContainer )
    {
    delete m_ContourContainer;
    }
  if ( m_MeshContainer )
    {
    delete m_MeshContainer;
    }
  if ( m_TrackContainer )
    {
    delete m_TrackContainer;
    }
  if ( m_LineageContainer)
    {
    delete m_LineageContainer;
    }

  if(m_ImageProcessor)
    {
    delete m_ImageProcessor;
    }

  if( m_QGoLineageViewerWidget)
    {
    delete m_QGoLineageViewerWidget;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::UpdateSeeds()
{
  for( size_t id = 0; id < m_Seeds.size(); id++ )
    {
    m_Seeds[id]->Initialize();
    }

  m_ImageView->GetSeeds( m_Seeds );
}

//-------------------------------------------------------------------------

std::vector< QString >
QGoTabImageView3DwT::GetChannelNames()
{
  // build channel vector
  unsigned int numberOfChannels = m_ImageProcessor->getNumberOfChannels();

  std::vector< QString > channelNames;
  channelNames.resize( numberOfChannels );

  for(unsigned int i =0; i<numberOfChannels; ++i)
    {
    channelNames[i] = QString::fromStdString( m_ImageProcessor->getChannelName(i));
    }

  return channelNames;
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateContourEditingDockWidget(
  int iTimeMin, int iTimeMax)
{
  //----------------------------------------------------------------
  // Create base  contour segmentation dock widget:
  // basic interactor connections
  //----------------------------------------------------------------

  std::vector< QString > channelNames = this->GetChannelNames();

  this->m_ContourEditingWidget = new QGoContourEditingWidgetManager(
    channelNames, iTimeMin, iTimeMax, &m_Seeds,
    m_ImageProcessor, &m_TCoord, this);

  this->CreateConnectionsTraceEditingWidget<QGoContourEditingWidgetManager>(
    iTimeMin, iTimeMax, this->m_ContourEditingWidget);

  // signals from the manual segmentation
  QObject::connect( m_ContourEditingWidget,
                    SIGNAL( ManualSegmentationActivated(bool) ),
                    this,
                    SLOT( ManualInteractorBehavior(bool) ) );

  QObject::connect( this->m_ContourEditingWidget,
                    SIGNAL( ContourValidated(int) ),
                    this, SLOT( ValidateContour(int) ) );

  QObject::connect( this->m_ContourEditingWidget,
                    SIGNAL( reinitializeContour() ),
                    m_ImageView,
                    SLOT( ReinitializeContourWidget() ) );

  QObject::connect( this->m_ContourEditingWidget,
                    SIGNAL( changeContourRepresentationProperty(float, QColor,
                                                                  QColor, QColor) ),
                    m_ImageView,
                    SLOT( UpdateContourRepresentationProperties(float, QColor,
                                                                QColor, QColor) ) );

  QObject::connect( this->m_ContourEditingWidget,
                    SIGNAL(TracesCreatedFromAlgo(std::vector<vtkPolyData *>, int) ),
                    this,
                    SLOT( SaveInDBAndRenderContourForVisu(std::vector<vtkPolyData *>, int) ) );


  this->m_ContourEditingWidget->InitializeSettingsForManualMode();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateMeshEditingDockWidget(int iTimeMin, int iTimeMax)
{
  //----------------------------------------------------------------
  // Create base  contour segmentation dock widget:
  // basic interactor connections
  //----------------------------------------------------------------

  // build channel vector
  std::vector< QString > channelNames = this->GetChannelNames();

  this->m_MeshEditingWidget = new QGoMeshEditingWidgetManager(
    channelNames, iTimeMin, iTimeMax, &m_Seeds,
    m_ImageProcessor, &m_TCoord, this);

  this->CreateConnectionsTraceEditingWidget<QGoMeshEditingWidgetManager>(
    iTimeMin, iTimeMax, this->m_MeshEditingWidget);

  QObject::connect( this->m_MeshEditingWidget,
                    SIGNAL(TracesCreatedFromAlgo(std::vector<vtkPolyData *>, int) ),
                    this,
                    SLOT( SaveInDBAndRenderMeshForVisu(std::vector<vtkPolyData *>, int) ) );

  QObject::connect( this->m_MeshEditingWidget,
                    SIGNAL(TracesSplittedFromAlgo(std::vector<vtkPolyData *>) ),
                    this,
                    SLOT( SplitInDBAndRenderMeshForVisu(std::vector<vtkPolyData *>) ) );

  QObject::connect( this->m_MeshEditingWidget,
                    SIGNAL(TracesMergedFromAlgo(vtkPolyData *)),
                    this,
                    SLOT( MergeInDBAndRenderMeshForVisu(vtkPolyData * ) ));

  /** \todo connect the signal, reimplement the slot*/
  QObject::connect( this->m_MeshEditingWidget,
                    SIGNAL(SetOfContoursFromAlgo(std::vector<std::vector<vtkPolyData*> >, int) ),
                    this,
                    SLOT(SaveInDBAndRenderSetOfContoursForVisu(std::vector<std::vector<vtkPolyData*> >, int)));

  QObject::connect( this,
                    SIGNAL( TimePointChanged(int) ),
                    this,
                    SLOT(UpdateTracesEditingWidget() ) );

  QObject::connect( this->m_MeshEditingWidget,
                    SIGNAL(RequestPolydatas() ),
                    this,
                    SLOT( PolydatasRequested() ) );

  QObject::connect( this,
                    SIGNAL( RequestedPolydatas(std::list< vtkPolyData* >) ),
                    this->m_MeshEditingWidget,
                    SLOT( RequestedPolydatas(std::list< vtkPolyData* >) ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::ManualInteractorBehavior(bool iVisible)
{
  DefaultInteractorBehavior(iVisible);
  this->m_ImageView->EnableContourWidget(iVisible);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SeedInteractorBehavior(bool iVisible)
{
  this->m_ImageView->EnableSeedWidget(iVisible);
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::DefaultInteractorBehavior(bool iVisible)
{
  if ( iVisible )
    {
    this->m_ImageView->DefaultMode();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::ZoomInteractorBehavior(bool iVisible)
{
  if ( iVisible )
    {
    this->m_ImageView->ZoomMode();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::TranslateInteractorBehavior(bool iVisible)
{
  if ( iVisible )
    {
    this->m_ImageView->PanMode();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::ActorPickingInteractorBehavior(bool iVisible)
{
  if ( iVisible )
    {
    this->m_ImageView->ActorPickingMode();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::DistanceWidgetInteractorBehavior(bool iActive)
{
  this->m_ImageView->EnableDistanceWidget(iActive);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::AngleWidgetInteractorBehavior(bool iActive)
{
  this->m_ImageView->EnableAngleWidget(iActive);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::Box3DPicking(bool iActive)
{
  this->m_ImageView->EnableBoxWidget(iActive);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::PlaneWidgetInteractorBehavior(bool iActive)
{
  this->m_ImageView->EnablePlaneWidget(iActive);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateVisuDockWidget()
{
  /**
   * \todo update annotation on current slice (i.e. slice x/23) when moving through
   * x via this dockwidget
   */

  m_NavigationDockWidget =
    new QGoNavigationDockWidget(this, GoFigure::THREE_D_WITH_T);

  QObject::connect( m_NavigationDockWidget, SIGNAL( XSliceChanged(int) ),
                    this, SLOT( SetSliceViewYZ(int) ) );

  QObject::connect( this, SIGNAL( SliceViewYZChanged(int) ),
                    m_NavigationDockWidget, SLOT( SetXSlice(int) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( YSliceChanged(int) ),
                    this, SLOT( SetSliceViewXZ(int) ) );

  QObject::connect( this, SIGNAL( SliceViewXZChanged(int) ),
                    m_NavigationDockWidget, SLOT( SetYSlice(int) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( ZSliceChanged(int) ),
                    this, SLOT( SetSliceViewXY(int) ) );

  QObject::connect( this, SIGNAL( SliceViewXYChanged(int) ),
                    m_NavigationDockWidget, SLOT( SetZSlice(int) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( TSliceChanged(int) ),
                    this, SLOT( SetTimePoint(int) ) );

  QObject::connect( this, SIGNAL( TimePointChanged(int) ),
                    m_NavigationDockWidget, SLOT( SetTSlice(int) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( ModeChanged(int) ),
                    this, SLOT( ModeChanged(int) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( StepChanged(int) ),
                    this, SLOT( StepChanged(int) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( DopplerSizeChanged(int) ),
                    this, SLOT( DopplerSizeChanged(int) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( visibilityChanged(QString, bool) ),
                    this, SLOT( visibilityChanged(QString, bool) ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateDataBaseTablesConnection()
{
  QObject::connect ( this->m_DataBaseTables,
                     SIGNAL( DBVariablesSet() ),
                     this,
                     SLOT( SetDatabaseContainersAndDelayedConnections() ) );
  //in order also, to update the list of meshes in the collectionIDs when
  //the trace is contour and to update the show/hide contours and meshes
  QObject::connect( this,
                    SIGNAL( TimePointChanged(int) ),
                    this->m_DataBaseTables,
                    SLOT( UpdateSelectedTimePoint(int) ) );

  QObject::connect( this->m_DataBaseTables,
                    SIGNAL( TraceToReEdit(unsigned int) ),
                    this, SLOT( ReEditContour(unsigned int) ) );

  QObject::connect( this->m_DataBaseTables,
                    SIGNAL( NewMeshToGenerate(std::list< unsigned int >, int) ),
                    this,
                    SLOT( CreateMeshFromSelectedContours(std::list< unsigned int >, int) ) );

  QObject::connect( this->m_DataBaseTables, SIGNAL( NeedToGoToTheLocation(int, int, int, int) ),
                    this, SLOT( GoToLocation(int, int, int, int) ) );

  QObject::connect( this->m_DataBaseTables->toggleViewAction(), SIGNAL (toggled(bool) ),
                    this, SLOT(SetTraceSettingsToolBarVisible(bool) ) );

  this->m_TraceSettingsWidget = this->m_DataBaseTables->GetTraceSettingsWidget();
  //this->m_TraceSettingsWidgetForToolBar = this->m_DataBaseTables->GetTraceSettingsWidgetForToolBar();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::SetDatabaseContainersAndDelayedConnections()
{
  this->SetTheContainersForDB();
  QObject::connect( this->m_DataBaseTables,
                    SIGNAL( PrintMessage(QString, int) ),
                    this->m_StatusBar,
                    SLOT( showMessage(QString, int) ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetTheContainersForDB()
{
  m_DataBaseTables->SetContoursContainer(m_ContourContainer);
  m_DataBaseTables->SetMeshesContainer(m_MeshContainer);
  m_DataBaseTables->SetTracksContainer(m_TrackContainer);
  m_DataBaseTables->SetLineagesContainers(m_LineageContainer, m_TrackContainer);
}

//-------------------------------------------------------------------------
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
//-------------------------------------------------------------------------

void
QGoTabImageView3DwT::CreateVideoRecorderWidget()
{
  m_VideoRecorderWidget = new QGoVideoRecorder(this);

  QObject::connect( this, SIGNAL( FullScreenViewChanged(int) ),
                    this, SLOT( SetRendererWindow(int) ) );

  QObject::connect( m_VideoRecorderWidget, SIGNAL( XSliceChanged(int) ),
                    this, SLOT( SetSliceViewYZ(int) ) );
  QObject::connect( m_VideoRecorderWidget, SIGNAL( YSliceChanged(int) ),
                    this, SLOT( SetSliceViewXZ(int) ) );
  QObject::connect( m_VideoRecorderWidget, SIGNAL( ZSliceChanged(int) ),
                    this, SLOT( SetSliceViewXY(int) ) );
  QObject::connect( m_VideoRecorderWidget, SIGNAL( TSliceChanged(int) ),
                    this, SLOT( SetTimePoint(int) ) );

  QObject::connect( m_VideoRecorderWidget, SIGNAL( GetSliceView() ),
                    this, SLOT( SetSliceView() ) );
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetRendererWindow(int iValue)
{
  if ( iValue >= 1 )
    {
    m_VideoRecorderWidget->SetRenderingWindow(
      m_ImageView->GetInteractor(iValue - 1)->GetRenderWindow() );
    }
  else
    {
    m_VideoRecorderWidget->SetRenderingWindow(NULL);
    }
}

#endif /* ENABLEVIDEORECORD */
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateAllViewActions()
{
  QAction *separator1 = new QAction(this);
  separator1->setSeparator(true);
  this->m_ViewNoToolBarActions.push_back(separator1);

  QActionGroup *group = new QActionGroup(this);

  QAction *QuadViewAction = new QAction(tr("Quad-View"), this);

  QuadViewAction->setCheckable(true);
  QuadViewAction->setChecked(true);

  QIcon quadviewicon;
  quadviewicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/4views.png") ),
                         QIcon::Normal, QIcon::Off);
  QuadViewAction->setIcon(quadviewicon);

  group->addAction(QuadViewAction);

  this->m_ViewActions.push_back(QuadViewAction);

  QObject::connect( QuadViewAction, SIGNAL( triggered() ),
                    this, SLOT( Quadview() ) );

  QAction *FullScreenXYAction = new QAction(tr("Full-Screen XY"), this);
  FullScreenXYAction->setCheckable(true);

  QIcon xyicon;
  xyicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/xy.png") ),
                   QIcon::Normal, QIcon::Off);
  FullScreenXYAction->setIcon(xyicon);

  group->addAction(FullScreenXYAction);

  this->m_ViewActions.push_back(FullScreenXYAction);

  QObject::connect( FullScreenXYAction, SIGNAL( triggered() ),
                    this, SLOT( FullScreenViewXY() ) );

  QAction *FullScreenXZAction = new QAction(tr("Full-Screen XZ"), this);
  FullScreenXZAction->setCheckable(true);

  QIcon xzicon;
  xzicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/zx.png") ),
                   QIcon::Normal, QIcon::Off);
  FullScreenXZAction->setIcon(xzicon);

  group->addAction(FullScreenXZAction);

  this->m_ViewActions.push_back(FullScreenXZAction);

  QObject::connect( FullScreenXZAction, SIGNAL( triggered() ),
                    this, SLOT( FullScreenViewXZ() ) );

  QAction *FullScreenYZAction = new QAction(tr("Full-Screen YZ"), this);
  FullScreenYZAction->setCheckable(true);

  QIcon yzicon;
  yzicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/yz.png") ),
                   QIcon::Normal, QIcon::Off);
  FullScreenYZAction->setIcon(yzicon);

  group->addAction(FullScreenYZAction);

  this->m_ViewActions.push_back(FullScreenYZAction);

  QObject::connect( FullScreenYZAction, SIGNAL( triggered() ),
                    this, SLOT( FullScreenViewYZ() ) );

  QAction *FullScreenXYZAction = new QAction(tr("Full-Screen XYZ"), this);
  FullScreenXYZAction->setCheckable(true);

  QIcon xyzicon;
  xyzicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/xyz.png") ),
                    QIcon::Normal, QIcon::Off);
  FullScreenXYZAction->setIcon(xyzicon);

  group->addAction(FullScreenXYZAction);

  this->m_ViewActions.push_back(FullScreenXYZAction);

  QObject::connect( FullScreenXYZAction, SIGNAL( triggered() ),
                    this, SLOT( FullScreenViewXYZ() ) );

  QAction *separator2 = new QAction(this);
  separator2->setSeparator(true);
  this->m_ViewActions.push_back(separator2);

  QAction *ActionDisplayAnnotations =
    new QAction(tr("Display annotations"), this);
  ActionDisplayAnnotations->setCheckable(true);
  ActionDisplayAnnotations->setChecked(true);
  ActionDisplayAnnotations->setStatusTip( tr(" Display annotations in each 2d view") );

  QIcon displayannotationsicon;
  displayannotationsicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/2D_VIEWS_INFOS.png") ),
                                   QIcon::Normal, QIcon::Off);
  ActionDisplayAnnotations->setIcon(displayannotationsicon);

  QObject::connect( ActionDisplayAnnotations, SIGNAL( triggered() ),
                    this->m_ImageView, SLOT( ShowAnnotations() ) );

  this->m_ViewNoToolBarActions.push_back(ActionDisplayAnnotations);

  QAction *ActionDisplaySplinePlanes =
    new QAction(tr("Display spline planes"), this);
  ActionDisplaySplinePlanes->setCheckable(true);
  ActionDisplaySplinePlanes->setChecked(true);
  ActionDisplaySplinePlanes->setStatusTip( tr(" Display spline planes on each view") );

  QIcon displaysplineplaneicon;
  displaysplineplaneicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/C_M_L.png") ),
                                   QIcon::Normal, QIcon::Off);
  ActionDisplaySplinePlanes->setIcon(displaysplineplaneicon);
  this->m_ViewNoToolBarActions.push_back(ActionDisplaySplinePlanes);

  QObject::connect( ActionDisplaySplinePlanes, SIGNAL( triggered() ),
                    this->m_ImageView, SLOT( ShowSplinePlane() ) );

  QAction *DisplayCube3D = new QAction(tr("Display 3D cube"), this);
  DisplayCube3D->setCheckable(true);
  DisplayCube3D->setChecked(true);
  DisplayCube3D->setStatusTip( tr(" Display or not cube in 3d") );

  QIcon cube3dicon;
  cube3dicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/cube.png") ),
                       QIcon::Normal, QIcon::Off);
  DisplayCube3D->setIcon(cube3dicon);

  QObject::connect( DisplayCube3D, SIGNAL( triggered() ),
                    this->m_ImageView, SLOT( ShowCube3D() ) );

  this->m_ViewNoToolBarActions.push_back(DisplayCube3D);

  QAction *LookupTableAction = new QAction(tr("Lookup Table"), this);
  LookupTableAction->setObjectName("LUT");
  LookupTableAction->setEnabled(false);
  LookupTableAction->setStatusTip( tr(" Change the associated lookup table") );

  //take
  /*QIcon luticon;
  luticon.addPixmap(QPixmap( QString::fromUtf8(":/fig/LookupTable.png") ),
                    QIcon::Normal, QIcon::Off);
  LookupTableAction->setIcon(luticon);

  // Here write the connection
  QObject::connect( LookupTableAction, SIGNAL( triggered() ),
                    this, SLOT( ChangeLookupTable() ) );

  this->m_ViewActions.push_back(LookupTableAction);*/

  QAction *ScalarBarAction = new QAction(tr("Display Scalar Bar"), this);
  ScalarBarAction->setEnabled(false);
  ScalarBarAction->setCheckable(true);
  ScalarBarAction->setObjectName("ScalarBar");

  QIcon scalarbaricon;
  scalarbaricon.addPixmap(QPixmap( QString::fromUtf8(":/fig/scalarbar.png") ),
                          QIcon::Normal, QIcon::Off);
  ScalarBarAction->setIcon(scalarbaricon);

  this->m_ViewActions.push_back(ScalarBarAction);

  QObject::connect( ScalarBarAction, SIGNAL( toggled(bool) ),
                    m_ImageView, SLOT( ShowScalarBar(bool) ) );

  QPixmap Pix(16, 16);
  Pix.fill(Qt::black);
  m_BackgroundColorAction = new QAction(Pix, tr("Set Background Color"), this);
  this->m_ViewActions.push_back(m_BackgroundColorAction);

  QObject::connect( m_BackgroundColorAction, SIGNAL( triggered() ),
                    this, SLOT( ChangeBackgroundColor() ) );

  QAction *separator4 = new QAction(this);
  separator4->setSeparator(true);
  this->m_ViewActions.push_back(separator4);

  this->m_ViewActions.push_back( m_NavigationDockWidget->toggleViewAction() );

  this->m_ViewActions.push_back( m_TransferFunctionDockWidget->toggleViewAction() );

  this->m_ViewActions.push_back( m_DataBaseTables->toggleViewAction() );

  /// \todo create group actions for views changing
  QAction *Change3DPerspectiveToAxialAction =
    new QAction(tr("Change 3D view to Posterior "), this);
  QIcon axialicon;
  axialicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/PosteriorView.png") ),
                      QIcon::Normal, QIcon::Off);
  Change3DPerspectiveToAxialAction->setIcon(axialicon);
  this->m_ViewNoToolBarActions.push_back(Change3DPerspectiveToAxialAction);

  QObject::connect( Change3DPerspectiveToAxialAction, SIGNAL( triggered() ),
                    this, SLOT( Change3DPerspectiveToAxial() ) );

  QAction *Change3DPerspectiveToCoronalAction =
    new QAction(tr("Change 3D view to Dorsal "), this);
  QIcon coronalicon;
  coronalicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/DorsalView.png") ),
                        QIcon::Normal, QIcon::Off);
  Change3DPerspectiveToCoronalAction->setIcon(coronalicon);

  this->m_ViewNoToolBarActions.push_back(Change3DPerspectiveToCoronalAction);

  QObject::connect( Change3DPerspectiveToCoronalAction, SIGNAL( triggered() ),
                    this, SLOT( Change3DPerspectiveToCoronal() ) );

  QAction *Change3DPerspectiveToSagittalAction =
    new QAction(tr("Change 3D view to Left "), this);
  QIcon sagittalicon;
  sagittalicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/LeftView.png") ),
                         QIcon::Normal, QIcon::Off);
  Change3DPerspectiveToSagittalAction->setIcon(sagittalicon);

  this->m_ViewNoToolBarActions.push_back(Change3DPerspectiveToSagittalAction);

  QObject::connect( Change3DPerspectiveToSagittalAction, SIGNAL( triggered() ),
                    this, SLOT( Change3DPerspectiveToSagittal() ) );

  QAction *separator7 = new QAction(this);
  separator7->setSeparator(true);
  this->m_ViewActions.push_back(separator7);

  // Enable volume rendering
  QAction *VolumeRenderingAction =
    new QAction(tr("Enable the volume rendering for the current channel(s)"), this);
  VolumeRenderingAction->setCheckable(true);
  VolumeRenderingAction->setChecked(false);
  this->m_ViewActions.push_back(VolumeRenderingAction);

  QIcon volumerenderingicon;
  volumerenderingicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/VolumeRendering1.png") ),
                                QIcon::Normal, QIcon::Off);
  VolumeRenderingAction->setIcon(volumerenderingicon);

  QObject::connect( VolumeRenderingAction, SIGNAL( toggled(bool) ),
                    this, SLOT( EnableVolumeRendering(bool) ) );

  // Enable synchronization
  QAction *SynchronizeViewsAction =
    new QAction(tr("synchronize the different views"), this);
  SynchronizeViewsAction->setCheckable(true);
  SynchronizeViewsAction->setChecked(true);
  this->m_ViewActions.push_back(SynchronizeViewsAction);

  QIcon synchronizeicon;
  synchronizeicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/synchronize.png") ),
                                QIcon::Normal, QIcon::Off);
  SynchronizeViewsAction->setIcon(synchronizeicon);

  QObject::connect( SynchronizeViewsAction, SIGNAL( toggled(bool) ),
                    this->m_ImageView, SLOT( SynchronizeViews(bool) ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateTracesActions()
{
   // Track Color Coding
 // this->m_TracesActions->m_VectorAction.push_back( m_TrackViewDockWidget->toggleViewAction() );

  // Lineage Color Coding
 // this->m_TracesActions->m_VectorAction.push_back( m_LineageViewDockWidget->toggleViewAction() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::StartDopplerView()
{
  bool         ok;
  QStringList  channel;
  unsigned int* boundChannel = m_ImageProcessor->getBoundsChannel();

  for( unsigned int i = boundChannel[0]; i < boundChannel[1]+1; ++i )
    {
    channel << QString::number(i, 10);
    }

  QString item =
      QInputDialog::getItem(this,
                            tr("Channel selection"),
                            tr("Please select the channel you want to track"),
                            channel, 0, false, &ok);

  if ( ok )
    {
    m_ImageProcessor->setDopplerMode(ok, // bool: true/false
                                     item.toInt(&ok, 10)); // selected channel ID
    // update image
    m_ImageProcessor->setDoppler(m_TCoord, 0); // 0 is for optimization later on...

    // update widget
    // hide channels
    this->m_NavigationDockWidget->VisibilityListChannels(false);
    //update values - show requiered widgets
    std::vector<int> time = m_ImageProcessor->getDopplerTime(m_TCoord);
    // get number of items in container
    // requiereds to call this method since the number of items varies
    for(unsigned int i=0; i<m_ImageProcessor->getDopplerSize(); ++i)
      {
      if(time[i]>=0)
        {
        std::string name = m_ImageProcessor->getChannelName(time[i]);
        // channel color
        std::vector<double> color = m_ImageProcessor->getColor(name);
        // update navigation dockwidget
        m_NavigationDockWidget->AddDoppler(
              QString::fromStdString(name),
              QColor(color[0],
                     color[1],
                     color[2],
                     color[3]),
              time[i],
              true); // all checkboxes are check edwhen we start
        }
      }
    // set channel name
    this->m_NavigationDockWidget->setChannelName(
          QString("Channel %1").arg(m_ImageProcessor->getDopplerChannel()));
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateToolsActions()
{
#if defined( ENABLEFFMPEG ) || defined( ENABLEAVI )
  this->m_ToolsActions.push_back( m_VideoRecorderWidget->toggleViewAction() );
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(false);
#endif

  m_TakeSnapshotAction = new QAction(tr("Take Snapshot"), this);
  QIcon snapshoticon;
  snapshoticon.addPixmap(QPixmap( QString::fromUtf8(":/fig/camera-photo.png") ),
                         QIcon::Normal, QIcon::Off);
  m_TakeSnapshotAction->setIcon(snapshoticon);
  m_TakeSnapshotAction->setStatusTip( tr("You have to be in full screen view to use the snapshot") );
  m_TakeSnapshotAction->setEnabled(false);
  QObject::connect( m_TakeSnapshotAction, SIGNAL( triggered() ),
                    this, SLOT( TakeSnapshot() ) );

  this->m_ToolsActions.push_back(m_TakeSnapshotAction);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void QGoTabImageView3DwT::CreateModeActions()
{
  QActionGroup *group = new QActionGroup(this);
  group->setObjectName("ModeGroup");
  // Call superclass
  QGoTabElementBase::CreateModeActions(group);

  QAction *separator1 = new QAction(this);
  separator1->setSeparator(true);
  this->m_ModeActions.push_back(separator1);

  //---------------------------------//
  //       Actor picking  mode       //
  //---------------------------------//
  QAction *ActorPickingAction = new QAction(tr("Object Picking"), this);
  ActorPickingAction->setCheckable(true);
  ActorPickingAction->setChecked(false);

  QIcon ActorPickingIcon;
  ActorPickingIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/ObjectPicking.png") ),
                             QIcon::Normal, QIcon::Off);
  ActorPickingAction->setIcon(ActorPickingIcon);
  ActorPickingAction->setStatusTip( tr(
                                      "Select a contour or a mesh (left click when the bounding box of the object of interest is visible)") );

  group->addAction(ActorPickingAction);

  this->m_ModeActions.push_back(ActorPickingAction);
  // it also updates the interactor behaviour
  QObject::connect( ActorPickingAction, SIGNAL( toggled(bool) ),
                    this, SLOT( ActorPickingInteractorBehavior(bool) ) );

  //---------------------------------//
  //       Box 3D picking  mode      //
  //---------------------------------//
  QAction *Box3DPickingAction = new QAction(tr("Show/hide objects using Box"), this);
  Box3DPickingAction->setCheckable(true);
  Box3DPickingAction->setChecked(false);

  QIcon Box3DPickingIcon;
  Box3DPickingIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/Box3DPicking.png") ),
                             QIcon::Normal, QIcon::Off);
  Box3DPickingAction->setIcon(Box3DPickingIcon);
  Box3DPickingAction->setStatusTip( tr("Show only the objects in the box") );

  group->addAction(Box3DPickingAction);

  this->m_ModeActions.push_back(Box3DPickingAction);
  // it also updates the interactor behaviour
  QObject::connect( Box3DPickingAction, SIGNAL( toggled(bool) ),
                    this, SLOT( Box3DPicking(bool) ) );

  //---------------------------------//
  //        Plane  widget  mode      //
  //---------------------------------//
  QAction *PlaneWidgetAction = new QAction(tr("Show/hide objects using Plane"), this);
  PlaneWidgetAction->setCheckable(true);
  PlaneWidgetAction->setChecked(false);

  QIcon PlaneWidgetIcon;
  PlaneWidgetIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/PlaneSelection.png") ),
                            QIcon::Normal, QIcon::Off);
  PlaneWidgetAction->setIcon(PlaneWidgetIcon);
  PlaneWidgetAction->setStatusTip( tr("Show only the objects located in front of the plane") );

  group->addAction(PlaneWidgetAction);

  this->m_ModeActions.push_back(PlaneWidgetAction);
  // it also updates the interactor behaviour
  QObject::connect( PlaneWidgetAction, SIGNAL( toggled(bool) ),
                    this, SLOT( PlaneWidgetInteractorBehavior(bool) ) );

  // NOT A MODE: ANNOTATION

  QAction *separator3 = new QAction(this);
  separator3->setSeparator(true);
  this->m_ModeActions.push_back(separator3);

  //---------------------------------//
  //         Distance    mode        //
  //---------------------------------//
  QAction *DistanceAction = new QAction(tr("Measure a Distance"), this);

  DistanceAction->setCheckable(true);
  DistanceAction->setChecked(false);

  QIcon DistanceIcon;
  DistanceIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/Distance.png") ),
                         QIcon::Normal, QIcon::Off);
  DistanceAction->setIcon(DistanceIcon);
  DistanceAction->setStatusTip( tr("Measure a distance between 2 points (left click to place/drag the points)") );

  group->addAction(DistanceAction);

  this->m_ModeActions.push_back(DistanceAction);

  QObject::connect( DistanceAction, SIGNAL( toggled(bool) ),
                    this, SLOT( DistanceWidgetInteractorBehavior(bool) ) );

  // NOT A MODE: ANNOTATION

  //---------------------------------//
  //           Angle     mode        //
  //---------------------------------//
  QAction *AngleAction = new QAction(tr("Measure an Angle"), this);
  AngleAction->setCheckable(true);
  AngleAction->setChecked(false);

  QIcon AngleIcon;
  AngleIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/Angle.png") ),
                      QIcon::Normal, QIcon::Off);
  AngleAction->setIcon(AngleIcon);
  AngleAction->setStatusTip( tr("Measure an angle between 3 points (left click to place/drag the points)") );

  group->addAction(AngleAction);

  this->m_ModeActions.push_back(AngleAction);

  QObject::connect( AngleAction, SIGNAL( toggled(bool) ),
                    this, SLOT( AngleWidgetInteractorBehavior(bool) ) );

  //--------------------------------//
  //  Contour segmentation mode     //
  //--------------------------------//

  QAction *ContourSegmentationAction =
    m_ContourSegmentationDockWidget->toggleViewAction();

  group->addAction(ContourSegmentationAction);
  //group->addAction(this->m_ContourSegmentationDockWidget->GetActionForToggle() );

  this->m_TracesActions->m_VectorAction.push_back(ContourSegmentationAction);
  //this->m_TracesActions.push_back(this->m_ContourSegmentationDockWidget->GetActionForToggle());

  QObject::connect( ContourSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    m_ContourSegmentationDockWidget,
                    SLOT( interactorBehavior(bool) ) );

  QObject::connect( ContourSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    this,
                    SLOT( SetTraceSettingsToolBarVisible(bool) ) );

  //---------------------------------//
  //        Mesh segmentation        //
  //---------------------------------//

  QAction *MeshSegmentationAction =
    m_MeshSegmentationDockWidget->toggleViewAction();

  group->addAction(MeshSegmentationAction);

  this->m_TracesActions->m_VectorAction.push_back(MeshSegmentationAction);

  QObject::connect( MeshSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    m_MeshSegmentationDockWidget,
                    SLOT( interactorBehavior(bool) ) );

  QObject::connect( MeshSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    this,
                    SLOT( SetTraceSettingsToolBarVisible(bool) ) );
}*/

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::CreateBookmarkActions()
{
  QAction *AddBookmarkAction = new QAction(tr("Add a bookmark"), this);

  this->m_BookmarkActions.push_back(AddBookmarkAction);
  QObject::connect( AddBookmarkAction, SIGNAL ( triggered() ),
                    this, SLOT ( AddBookmark() ) );
  QAction *DeleteBookmarkAction = new QAction(tr("Delete a bookmark"), this);
  this->m_BookmarkActions.push_back(DeleteBookmarkAction);
  QObject::connect( DeleteBookmarkAction, SIGNAL ( triggered() ),
                    this->m_DataBaseTables, SLOT( DeleteBookmarks() ) );
  QObject::connect( this->m_DataBaseTables, SIGNAL ( PrintDBReady() ),
                    this, SLOT( GetTheRelatedToDBActions() ) );
  QObject::connect( this->m_DataBaseTables, SIGNAL( OpenBookmarksToUpdate() ),
                    this, SLOT( GetTheOpenBookmarksActions() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::GetTheRelatedToDBActions()
{
  this->GetTheOpenBookmarksActions();
  QMenu *  ImportMenu = new QMenu(tr("Import"), this);
  QAction *ImportContoursAction = new QAction(tr("Contours"), this);
  ImportContoursAction->setStatusTip(
    tr("Import the data of the contours from the text file into the GoFigure database" ) );
  QAction *ImportMeshesAction = new QAction(tr("3DMeshes"), this);
  ImportMeshesAction->setStatusTip(
    tr("Import the data of the meshes from the text file into the GoFigure database" ) );
  QAction *ImportTracksAction = new QAction(tr("Tracks"), this);
  ImportTracksAction->setStatusTip(
    tr("Import the data of the tracks from the text file into the GoFigure database" ) );

  ImportMenu->addAction(ImportContoursAction);
  ImportMenu->addAction(ImportMeshesAction);
  ImportMenu->addAction(ImportTracksAction);

  QMenu *  ExportMenu = new QMenu(tr("Export"), this);
  QAction *ExportContoursAction = new QAction(tr("Contours"), this);
  ExportContoursAction->setStatusTip(
    tr("Export all the data related to all contours of the imagingsession from the database into a .txt file") );

  QAction *ExportMeshesAction = new QAction(tr("3DMeshes"), this);
  ExportMeshesAction->setStatusTip(
    tr("Export all the data related to all meshes of the imagingsession from the database into a .txt file") );

  QAction *ExportLineagesAction = new QAction(tr("Lineages For Lineage Viewer"), this);
  ExportLineagesAction->setStatusTip(
    tr("Export each lineage into a vtkFile in order to be visualized separately into the lineage viewer outside of Gofigure") );

  ExportMenu->addAction(ExportContoursAction);
  ExportMenu->addAction(ExportMeshesAction);
  ExportMenu->addAction(ExportLineagesAction);

  this->m_ToolsActions.push_back( ImportMenu->menuAction() );
  this->m_ToolsActions.push_back( ExportMenu->menuAction() );

  QObject::connect( ExportContoursAction, SIGNAL( triggered() ),
                    this->m_DataBaseTables, SLOT( ExportContours () ) );
  QObject::connect( ImportContoursAction, SIGNAL( triggered() ),
                    this, SLOT( ImportContours() ) );
  QObject::connect( ExportMeshesAction, SIGNAL( triggered() ),
                    this->m_DataBaseTables, SLOT( ExportMeshes() ) );
  QObject::connect( ImportMeshesAction, SIGNAL ( triggered() ),
                    this, SLOT ( ImportMeshes() ) );
  QObject::connect( ImportTracksAction, SIGNAL ( triggered() ),
                    this, SLOT ( ImportTracks() ) );
  QObject::connect( ExportLineagesAction, SIGNAL ( triggered() ),
                    m_LineageContainer,
                    SIGNAL( ExportLineages() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::GetTheOpenBookmarksActions()
{
  bool UpdateOpenBookmarks = false;

  if ( this->m_BookmarkActions.size() > 2 )
    {
    this->m_BookmarkActions.erase(
      this->m_BookmarkActions.begin() + this->m_BookmarkActions.size() - 1
      );
    UpdateOpenBookmarks = true;
    }
  NamesDescrContainerType ListBookmarks =
    this->m_DataBaseTables->GetListBookmarks();
  size_t NumberBookmarks = ListBookmarks.size();
  QMenu *OpenBookmarkMenu = new QMenu(tr("Open a bookmark"), this);
  for( size_t i = 0; i < NumberBookmarks; i++ )
    {
    QAction *OpenBookmarkAction =
      new QAction(ListBookmarks[i].first.c_str(), this);
    std::string TextStatusTip = "Description of the bookmark: ";
    TextStatusTip += ListBookmarks[i].second;
    OpenBookmarkAction->setStatusTip( TextStatusTip.c_str() );
    OpenBookmarkMenu->addAction(OpenBookmarkAction);
    QObject::connect(
      OpenBookmarkAction, SIGNAL( triggered() ),
      this, SLOT( OpenExistingBookmark() )
      );
    }
  this->m_BookmarkActions.push_back( OpenBookmarkMenu->menuAction() );
  if ( UpdateOpenBookmarks )
    {
    emit UpdateBookmarkOpenActions(this->m_BookmarkActions);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::OpenExistingBookmark()
{
  QAction *         taction = qobject_cast< QAction * >( sender() );
  std::string       BookmarkName = taction->text().toStdString();
  GoDBCoordinateRow Coord = this->m_DataBaseTables->GetCoordinateForBookmark(BookmarkName);

  int tt = Coord.GetMapValue<int>("TCoord");
  int tz = Coord.GetMapValue<int>("ZCoord");
  int ty = Coord.GetMapValue<int>("YCoord");
  int tx = Coord.GetMapValue<int>("XCoord");

  this->SetTimePoint(tt);
  this->SetSliceViewXY(tz);
  this->SetSliceViewXZ(ty);
  this->SetSliceViewYZ(tx);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::TakeSnapshot()
{
  // Get the current view displayed in full screen
  int FullScreenView = m_ImageView->GetFullScreenView();

  QString filename = QDir::toNativeSeparators( QDir::homePath() );

  filename.append( QString("%1").arg("/") );

  switch ( FullScreenView )
    {
    case 1:
      filename.append("snapshot-xy-");
      m_ImageView->SnapshotViewXY(GoFigure::PNG, filename);
      break;

    case 2:
      filename.append("snapshot-xz-");
      m_ImageView->SnapshotViewXZ(GoFigure::PNG, filename);
      break;

    case 3:
      filename.append("snapshot-yz-");
      m_ImageView->SnapshotViewYZ(GoFigure::PNG, filename);
      break;

    default:
      filename.append("snapshot-xyz-");
      m_ImageView->SnapshotViewXYZ(GoFigure::PNG, filename);
      break;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::setupUi(QWidget *iParent)
{
  if ( iParent->objectName().isEmpty() )
    {
    iParent->resize(800, 800);
    }

  m_ImageView = new QGoImageView3D( this );
  this->setCentralWidget(m_ImageView);
  m_DataBaseTables = new QGoPrintDatabase( this );
  this->addDockWidget(Qt::TopDockWidgetArea, m_DataBaseTables);
  m_DataBaseTables->hide();


  m_ImageView->SetIntersectionLineWidth(this->m_IntersectionLineWidth);
  m_ImageView->SetBackgroundColor(m_BackgroundColor);

  QObject::connect( m_ImageView, SIGNAL( SliceViewXYChanged(int) ),
                    this, SIGNAL( SliceViewXYChanged(int) ) );

  QObject::connect( m_ImageView, SIGNAL( SliceViewXZChanged(int) ),
                    this, SIGNAL( SliceViewXZChanged(int) ) );

  QObject::connect( m_ImageView, SIGNAL( SliceViewYZChanged(int) ),
                    this, SIGNAL( SliceViewYZChanged(int) ) );

  QObject::connect( m_ImageView, SIGNAL( FullScreenViewChanged(int) ),
                    this, SIGNAL( FullScreenViewChanged(int) ) );
  // connect the contours selection connection
  QObject::connect( m_ImageView, SIGNAL( SelectionChanged() ),
                    this, SLOT( HighlightPickedActor() ) );

  // connect the contours selection connection
  QObject::connect( m_ImageView, SIGNAL( VisibilityChanged() ),
                    this, SLOT( VisibilityPickedActor() ) );

  retranslateUi(iParent);

  QMetaObject::connectSlotsByName(iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::retranslateUi(QWidget *iParent)
{
  iParent->setWindowTitle( tr("QGoTabImageView3DwT") );
  Q_UNUSED(iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoFigure::TabDimensionType
QGoTabImageView3DwT::GetTabDimensionType() const
{
  return GoFigure::THREE_D_WITH_T;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetLSMReader(vtkLSMReader *iReader, const int & iTimePoint)
{
  GoLSMImageProcessor* processor = new GoLSMImageProcessor;
  processor->setReader(iReader);
  m_ImageProcessor = processor;

  m_TCoord = iTimePoint;
  // update image processor
  m_ImageProcessor->initTimePoint(m_TCoord);
  //update images
  UpdateImage();
  // update actors
  this->ShowTraces(m_TCoord);
  // update widgets on image loading
  InitializeImageRelatedWidget();
  // render
  m_ImageView->Update();

  // for the trace widget, navigation widget and table widget
  // should not be requiered since we just initialize it before
  emit TimePointChanged(m_TCoord);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetMegaCaptureFile(
  const GoFigureFileInfoHelperMultiIndexContainer & iContainer,
  const GoFigure::FileType & iFileType,
  const std::string & iHeader,
  const unsigned int & iTimePoint)
{
  m_FileList = iContainer;
  m_FileType = iFileType;

  // setup megacapture reader
  m_MegaCaptureReader->SetInput(m_FileList);
  m_MegaCaptureReader->SetMegaCaptureHeader(iHeader);
  m_MegaCaptureReader->SetFileType(m_FileType);
  m_MegaCaptureReader->SetTimeBased(true);
  m_MegaCaptureReader->SetTimePoint(iTimePoint);
  m_MegaCaptureReader->Update();

  GoMegaImageProcessor* processor = new GoMegaImageProcessor;
  processor->setReader(m_MegaCaptureReader);
  m_ImageProcessor = processor;

  m_TCoord = iTimePoint;
  // update image processor
  m_ImageProcessor->initTimePoint(m_TCoord);
  //update images
  UpdateImage();
  // update widgets on image loading
  InitializeImageRelatedWidget();
  // render
  m_ImageView->Update();

  // for the trace widget, navigation widget and table widget
  // should not be requiered since we just initialize it before
  emit TimePointChanged(m_TCoord);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
InitializeImageRelatedWidget()
{
  unsigned int*  boundTime    = m_ImageProcessor->getBoundsTime();
  unsigned int NumberOfChannels = m_ImageProcessor->getNumberOfChannels();
  int* extent = m_ImageProcessor->getExtent();

  // Initialize the widgets with the good number of channels
  // it will update the size of the related combobox
  m_NavigationDockWidget->blockSignals(true);

  for( unsigned int i = 0; i < NumberOfChannels; i++ )
    {
    std::string name = m_ImageProcessor->getChannelName(i);
    // channel color
    std::vector<double> color = m_ImageProcessor->getColor(name);
    // update navigation dockwidget
    m_NavigationDockWidget->AddChannel(
          QString::fromStdString(name),
          QColor(color[0],
                 color[1],
                 color[2],
                 color[3]),
          i,
          true); // all checkboxes are check edwhen we start
    // create TF editor
    // add it in the vector
    GoTransferFunctionEditorWidget* widget =
        createTransferFunctionEditor(QString::fromStdString(name));
    }

  m_NavigationDockWidget->SetXMinimumAndMaximum(extent[0], extent[1]);
  m_NavigationDockWidget->SetXSlice( ( extent[0] + extent[1] ) / 2 );

  m_NavigationDockWidget->SetYMinimumAndMaximum(extent[2], extent[3]);
  m_NavigationDockWidget->SetYSlice( ( extent[2] + extent[3] ) / 2 );

  m_NavigationDockWidget->SetZMinimumAndMaximum(extent[4], extent[5]);
  m_NavigationDockWidget->SetZSlice( ( extent[4] + extent[5] ) / 2 );

  m_NavigationDockWidget->SetTMinimumAndMaximum(boundTime[0], boundTime[1]);
  m_NavigationDockWidget->SetTSlice(boundTime[0]);
  m_NavigationDockWidget->blockSignals(false);

  CreateContourEditingDockWidget(boundTime[0], boundTime[1]);
  CreateMeshEditingDockWidget(boundTime[0], boundTime[1]);
    m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(
        this->m_ContourEditingWidget->GetDockWidget(),
        Qt::LeftDockWidgetArea, false, true, this),
        this->m_ContourEditingWidget->GetDockWidget()) );

          m_DockWidgetList.push_back(
  std::pair< QGoDockWidgetStatus *, QDockWidget * >(
    new QGoDockWidgetStatus(
      this->m_MeshEditingWidget->GetDockWidget(),
      Qt::LeftDockWidgetArea, false, true, this),
      this->m_MeshEditingWidget->GetDockWidget()) );

  // Set up QSpinBox in m_VideoRecorderWidget
#if defined( ENABLEFFMPEG ) || defined( ENABLEAVI )
  m_VideoRecorderWidget->SetXMinAndMax(extent[0], extent[1]);
  m_VideoRecorderWidget->SetYMinAndMax(extent[2], extent[3]);
  m_VideoRecorderWidget->SetZMinAndMax(extent[4], extent[5]);
  m_VideoRecorderWidget->SetTMinAndMax(boundTime[0], boundTime[1]);
#endif /* ENABLEVIDEORECORD */
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::UpdateImage()
{
  // get number of visible channels instead
  unsigned int NumberOfVisibleChannels = m_ImageProcessor->getNumberOfVisibleChannels();

  if ( NumberOfVisibleChannels>1 )
    {
    m_ImageView->SetImage(m_ImageProcessor->getVisibleImages());
    }
  else if( NumberOfVisibleChannels == 1)
    {
    //update Image
    m_ImageView->SetImage(m_ImageProcessor->getImageBW());
    // update LUT
    m_ImageView->SetLookupTable(m_ImageProcessor->getLookuptable());
    // CONFIGURE LUT
    this->findChild<QAction*>("LUT")->setEnabled(true);
    this->findChild<QAction*>("ScalarBar")->setEnabled(true);
    }
  else
    {
    // Start by creating a black lookup table.
    vtkSmartPointer<vtkLookupTable> bwLut =
      vtkSmartPointer<vtkLookupTable>::New();
    bwLut->SetTableRange (0, 1);
    bwLut->SetSaturationRange (0, 0);
    bwLut->SetHueRange (0, 0);
    bwLut->SetValueRange (0, 0);
    bwLut->Build();
    m_ImageView->SetLookupTable(bwLut);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::BuildDopplerWidget()
{
  // update widget
  //update values - show requiered widgets
  std::vector<int> time = m_ImageProcessor->getDopplerTime(m_TCoord);
  // get number of items in container
  // requiereds to call this method since the number of items varies
  for(unsigned int i=0; i<m_ImageProcessor->getDopplerSize(); ++i)
    {
    if(time[i]>=0)
      {
      std::string name = m_ImageProcessor->getChannelName(time[i]);
      // channel color
      std::vector<double> color = m_ImageProcessor->getColor(name);
      // update navigation dockwidget
      m_NavigationDockWidget->AddDoppler(
            QString::fromStdString(name),
            QColor(color[0],
                   color[1],
                   color[2],
                   color[3]),
            time[i],
            true); // all checkboxes are check edwhen we start
      }
    }
  // set channel name
  this->m_NavigationDockWidget->setChannelName(
        QString("Channel %1").arg(m_ImageProcessor->getDopplerChannel()));
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetTimePoint(const int & iTimePoint)
{
  if ( iTimePoint == m_TCoord )
    {
    return;
    }

  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

  m_TCoord = iTimePoint;

  if (!m_ImageProcessor->getDopplerMode())
    {
    // update image processor
    m_ImageProcessor->setTimePoint(m_TCoord);
    }
  else
    {
    // delete previous doppler widget
    this->m_NavigationDockWidget->DeleteDopplerWidgets();
    // update the image processor
    m_ImageProcessor->setDoppler(m_TCoord, 0); // 0 is for optimization later on...
    //rebuild navigation widget
    BuildDopplerWidget();
    }

  // update image container
  UpdateImage();

  // update TF editor (histogram and max value)
  UpdateTFEditor();

  EnableVolumeRendering( this->m_VolumeRenderingEnabled );

  // for the trace widget, navigation widget and table widget
  emit TimePointChanged(m_TCoord);

  // clean table widget and container
  // then load new traces in TW and put polydatas in container
  std::list<unsigned int> timePoints =
          this->m_DataBaseTables->
          UpdateTableWidgetAndContainersForGivenTimePoint(
          m_TCoord);

  // create actors
  // function has to be splitted-> remove duplications
  this->CreateContoursActorsFromVisuContainer(timePoints);
  this->CreateMeshesActorsFromVisuContainer(timePoints);

  //show time specific actors
  this->ShowTraces(m_TCoord);

  m_ImageView->Update();

  QApplication::restoreOverrideCursor();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void
QGoTabImageView3DwT::ChangeLookupTable()
{
  vtkImageData *image = m_ImageView->GetImage();

  if ( image->GetNumberOfScalarComponents() == 1 )
    {
    vtkLookupTable *lut = QGoLUTDialog::GetLookupTable( this,
                                                        tr("Choose one look-up table") );
    if ( lut )
      {
      m_ImageView->SetLookupTable(lut);

      // free memory since it is not freed in the QGoLUTDialog
      lut->Delete();
      }
    }
}*/

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString
QGoTabImageView3DwT::SnapshotViewXY(
  const GoFigure::FileType & iType,
  const QString & iBaseName)
{
  return m_ImageView->SnapshotViewXY(iType, iBaseName);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString
QGoTabImageView3DwT::SnapshotViewXZ(
  const GoFigure::FileType & iType,
  const QString & iBaseName)
{
  return m_ImageView->SnapshotViewXZ(iType, iBaseName);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString
QGoTabImageView3DwT::SnapshotViewYZ(
  const GoFigure::FileType & iType,
  const QString & iBaseName)
{
  return m_ImageView->SnapshotViewYZ(iType, iBaseName);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString
QGoTabImageView3DwT::SnapshotViewXYZ(
  const GoFigure::FileType & iType,
  const QString & iBaseName)
{
  return m_ImageView->SnapshotViewXYZ(iType, iBaseName);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetSliceViewXY(const int & iS)
{
  m_ImageView->SetSliceViewXY(iS);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetSliceViewXZ(const int & iS)
{
  m_ImageView->SetSliceViewXZ(iS);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetSliceViewYZ(const int & iS)
{
  m_ImageView->SetSliceViewYZ(iS);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetFullScreenView(const int & iS)
{
  m_ImageView->SetFullScreenView(iS);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::Quadview()
{
  m_ImageView->SetFullScreenView(0);
  m_TakeSnapshotAction->setEnabled(false);
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(false);
#endif
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::FullScreenViewXY()
{
  m_ImageView->SetFullScreenView(1);
  m_TakeSnapshotAction->setEnabled(true);
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
#endif
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::FullScreenViewXZ()
{
  m_ImageView->SetFullScreenView(2);
  m_TakeSnapshotAction->setEnabled(true);
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
#endif
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::FullScreenViewYZ()
{
  m_ImageView->SetFullScreenView(3);
  m_TakeSnapshotAction->setEnabled(true);
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
#endif
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::FullScreenViewXYZ()
{
  m_ImageView->SetFullScreenView(4);
  m_TakeSnapshotAction->setEnabled(true);
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
#endif
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::GetBackgroundColorFromImageViewer()
{
  double r(0.), g(0.), b(0.);

  m_ImageView->GetBackgroundColor(r, g, b);
  m_BackgroundColor.setRgbF(r, g, b);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetBackgroundColorToImageViewer()
{
  m_ImageView->SetBackgroundColor(m_BackgroundColor);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::ChangeBackgroundColor()
{
  double r(0.), g(0.), b(0.);

  m_ImageView->GetBackgroundColor(r, g, b);
  m_BackgroundColor.setRgbF(r, g, b);

  QColor temp = QColorDialog::getColor( m_BackgroundColor,
                                        this, tr("Choose Background Color") );

  if ( temp != m_BackgroundColor )
    {
    m_BackgroundColor = temp;
    m_ImageView->SetBackgroundColor(m_BackgroundColor);
    QPixmap Pix(16, 16);
    Pix.fill(temp);
    m_BackgroundColorAction->setIcon(Pix);
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::ModeChanged(int iChannel)
{
  if ( iChannel == 1 )
    {
    // set image processor and build navigation widget if we click on ok
    StartDopplerView();
    }
  else
    {
    this->m_NavigationDockWidget->DeleteDopplerWidgets();
    m_ImageProcessor->setDopplerMode(false, 0);
    // update image processor
    m_ImageProcessor->initTimePoint(m_TCoord);
    // change visibility
    this->m_NavigationDockWidget->VisibilityListChannels(true);
    }

  //update images
  UpdateImage();
  // update visualization
  m_ImageView->Update();
  //update the trace editing widget
  UpdateTracesEditingWidget();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::StepChanged(int iStep)
{
  // delete previous doppler widget
  this->m_NavigationDockWidget->DeleteDopplerWidgets();
  // set the new doppler step
  m_ImageProcessor->setDopplerStep(iStep);
  // update the image processor
  m_ImageProcessor->setDoppler(m_TCoord, 0); // 0 is for optimization later on...
  //rebuild navigation widget
  BuildDopplerWidget();
  // build new image
  UpdateImage();
  //update
  m_ImageView->Update();
  //update the trace editing widget
  UpdateTracesEditingWidget();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SaveContour(vtkPolyData *contour, vtkPolyData *contour_nodes,
  int iTCoord)
{
  if ( ( contour->GetNumberOfPoints() > 2 ) && ( iTCoord >= 0 ) )
    {
    // Compute Bounding Box
    std::vector< int > bounds = GetBoundingBox(contour);
    // Save contour in database :
    this->m_DataBaseTables->SaveContoursFromVisuInDB(bounds[0],
                                                     bounds[2],
                                                     bounds[4],
                                                     iTCoord,
                                                     bounds[1],
                                                     bounds[3],
                                                     bounds[5],
                                                     contour_nodes);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< int >
QGoTabImageView3DwT::GetBoundingBox(vtkPolyData *iElement)
{
  std::vector< int > boundingBox(6, 0);

  if ( iElement )
    {
    // Compute Bounding Box
    double bounds[6];
    iElement->GetBounds(bounds);

    // Extract Min and Max from bounds
    double       Min[3], Max[3];
    int          k = 0;
    unsigned int i;
    for( i = 0; i < 3; i++ )
      {
      Min[i] = bounds[k++];
      Max[i] = bounds[k++];
      }

    int *min_idx = this->GetImageCoordinatesFromWorldCoordinates(Min);
    int *max_idx = this->GetImageCoordinatesFromWorldCoordinates(Max);

    int extent[6];
    m_ImageProcessor->getImageBW()->GetExtent(extent);

    for( i = 0; i < 3; i++ )
      {
      if ( min_idx[i] > extent[2 * i] )
        {
        boundingBox[2 * i] = min_idx[i];
        }
      else
        {
        boundingBox[2 * i] = extent[2 * i];
        }
      if ( max_idx[i] < extent[2 * i + 1] )
        {
        boundingBox[2 * i + 1] = max_idx[i];
        }
      else
        {
        boundingBox[2 * i + 1] = extent[2 * i + 1];
        }
      }

    delete[] min_idx;
    delete[] max_idx;
    }
  return boundingBox;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< vtkActor * >
QGoTabImageView3DwT::VisualizeTrace(vtkPolyData *iTrace, double *iRGBA)
{
  std::vector< vtkActor * > oActors;

  if ( iTrace->GetNumberOfPoints() > 2 )
    {
    vtkProperty *trace_property = vtkProperty::New();
    trace_property->SetColor(iRGBA[0], iRGBA[1], iRGBA[2]);
    trace_property->SetOpacity(iRGBA[3]);

    vtkPolyData *trace_copy = vtkPolyData::New();
    trace_copy->DeepCopy(iTrace);

    oActors =
      this->AddContour(trace_copy, trace_property);

    trace_copy->Delete();
    trace_property->Delete();
    }

  // add actor to renderer and to Prop3d
  m_ImageView->AddActor(3, oActors[3]);

  m_ImageView->UpdateRenderWindows();

  return oActors;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::ValidateContour(int iTCoord)
{
  bool re_edit = this->m_ContourEditingWidget->GetReeditMode();

#pragma omp for
  for( int i = 0; i < m_ImageView->GetNumberOfImageViewers(); i++ )
    {
    vtkPolyData *nodes = m_ImageView->GetContourRepresentationNodePolydata(i);

    if ( nodes->GetNumberOfPoints() > 2 )
      {
      /// \todo Nicolas - Fix leaks generated by
      // m_ImageView->GetContourRepresentationAsPolydata(i)
      vtkPolyData *contour = vtkPolyData::New();
      contour->DeepCopy( m_ImageView->GetContourRepresentationAsPolydata(i) );

      //polydata for bounding box, node for points
      SaveContour(contour, nodes, iTCoord);

      // polydata
      //ADD TRACE ID IN POLYDATA
      AddTraceIDIntoPolydata(contour, this->m_ContourContainer->m_CurrentElement.TraceID, "CONTOUR");

      std::vector< vtkActor * > actors =
        VisualizeTrace(contour,
                       this->m_ContourContainer->m_CurrentElement.rgba);
      contour->Delete();

      //nodes
      m_ContourContainer->UpdateCurrentElementFromVisu(actors,
                                                       nodes,
                                                      // m_TCoord,
                                                       iTCoord,
                                                       re_edit, //highlighted
                                                       true);   //visible

      m_ContourContainer->InsertCurrentElement();
      }
    }

  if ( re_edit ) //need to set the widgets to a normal mode
    {
    this->m_TraceSettingsToolBar->setEnabled(true);
    this->m_TraceSettingsWidget->setEnabled(true);
    this->m_ContourEditingWidget->SetReeditMode(false);
    m_ImageView->ReinitializeContourWidget();
    //m_ContourSegmentationDockWidget->hide();
    this->m_ContourEditingWidget->GetDockWidget()->hide();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int *
QGoTabImageView3DwT::GetImageCoordinatesFromWorldCoordinates(double iPos[3])
{
  return m_ImageView->GetImageCoordinatesFromWorldCoordinates(iPos);
}

//-------------------------------------------------------------------------
// std::vector< vtkQuadricLODActor* >
std::vector< vtkActor * >
QGoTabImageView3DwT::AddContour(vtkPolyData *dataset, vtkProperty *iProperty)
{
  return this->m_ImageView->AddContour(dataset, iProperty);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::ReEditContour(const unsigned int & iId)
{
  vtkPolyData *nodes = NULL;

  if ( this->m_ContourContainer
       ->RemoveElementFromVisualizationWithGivenTraceID(iId) )
    {
    nodes = this->m_ContourContainer->m_CurrentElement.Nodes;

    std::vector< int > bounds = this->GetBoundingBox(nodes);

    int dir = ContourMeshContainer::ComputeDirectionFromBounds< int >(bounds);

    if ( dir != -1 )
      {
      int idx[3];
      idx[0] = ( bounds[0] + bounds[1] ) / 2;
      idx[1] = ( bounds[2] + bounds[3] ) / 2;
      idx[2] = ( bounds[4] + bounds[5] ) / 2;

      this->GoToLocation(idx[0], idx[1], idx[2], m_TCoord);

      //this->m_ModeActions[0]->setChecked(true);
      QAction* action = this->findChild< QAction* >("ContourEditingMode");//->setChecked(true);
      action->setChecked(true);
      m_ImageView->InitializeContourWidgetNodes(dir, nodes);

      this->m_TraceSettingsToolBar->setEnabled(false);
      this->m_TraceSettingsWidget->setEnabled(false);
    //this->m_ContourSegmentationDockWidget->show();
      //this->m_ContourSegmentationDockWidget->SegmentationMethod(0);
      //this->m_ContourSegmentationDockWidget->SetReeditMode(true);
      this->m_ContourEditingWidget->SetReeditMode(true);
      this->m_ContourEditingWidget->GetDockWidget()->show();

      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::HighlightPickedActor()
{
  vtkActor *temp_actor = m_ImageView->GetCurrentActor();
  // mesh ID first - higher probability?
  if( temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("MESH") )
  {
    m_MeshContainer->UpdateElementHighlighting(
        temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("MESH")->GetTuple1(0));
    return;
  }

  if( temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("TRACK") )
  {
    m_TrackContainer->UpdateElementHighlighting(
        temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("TRACK")->GetTuple1(0));
    return;
  }

  if( temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("DIVISION") )
  {
    m_TrackContainer->UpdateCollectionHighlighting(
        temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("DIVISION")->GetTuple1(0));
    return;
  }

  if( temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("CONTOUR") )
  {
    m_ContourContainer->UpdateElementHighlighting(
        temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("CONTOUR")->GetTuple1(0));
    return;
  }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::VisibilityPickedActor()
{
  vtkActor *temp_actor = m_ImageView->GetCurrentActor();

  if( temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("MESH") )
  {
    m_MeshContainer->UpdateElementVisibility(
        temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("MESH")->GetTuple1(0),
        m_ImageView->GetCurrentState() );
    return;
  }

  if( temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("TRACK") )
  {
    m_TrackContainer->UpdateElementVisibility(
        temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("TRACK")->GetTuple1(0),
        m_ImageView->GetCurrentState() );
    return;
  }

  if( temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("CONTOUR") )
  {
    m_ContourContainer->UpdateElementVisibility(
        temp_actor->GetMapper()->GetInput()->GetFieldData()->GetArray("CONTOUR")->GetTuple1(0),
        m_ImageView->GetCurrentState() );
    return;
  }

}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int
QGoTabImageView3DwT::GetSliceViewXY() const
{
  return m_ImageView->GetSliceViewXY();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoTabImageView3DwT::GetSliceViewXZ() const
{
  return m_ImageView->GetSliceViewXZ();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoTabImageView3DwT::GetSliceViewYZ() const
{
  return m_ImageView->GetSliceViewYZ();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoTabImageView3DwT::GetTimePoint() const
{
  return m_TCoord;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoTabImageView3DwT::GetTimeInterval() const
{
  return m_ImageProcessor->getTimeInterval();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::AddBookmark()
{
  this->m_DataBaseTables->AddBookmark( this->GetSliceViewYZ(),
                                       this->GetSliceViewXZ(),
                                       this->GetSliceViewXY(),
                                       this->GetTimePoint() );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::Change3DPerspectiveToAxial()
{
  m_ImageView->SetCamera(1);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::Change3DPerspectiveToCoronal()
{
  m_ImageView->SetCamera(2);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::Change3DPerspectiveToSagittal()
{
  m_ImageView->SetCamera(3);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetSliceView()
{
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  m_VideoRecorderWidget->SetCurrentX( this->GetSliceViewYZ() );
  m_VideoRecorderWidget->SetCurrentY( this->GetSliceViewXZ() );
  m_VideoRecorderWidget->SetCurrentZ( this->GetSliceViewXY() );
  m_VideoRecorderWidget->SetCurrentT( this->GetTimePoint() );
#endif
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int
QGoTabImageView3DwT::SaveAndVisuContour(int iTCoord, vtkPolyData *iView)
{
  if ( !m_DataBaseTables->IsDatabaseUsed() )
    {
    std::cerr << "Problem with DB" << std::endl;
    return -1;
    }

  // if no data, we are using the contour widget
  if ( !iView )
    {
    std::cerr << "Input contour is NULL" << std::endl;
    return -1;
    }

  vtkPolyData *contour_nodes = vtkPolyData::New();
  CreateContour(contour_nodes, iView);

  // polydata for bounding box, nodes for db
  SaveContour(iView, contour_nodes, iTCoord);

  AddTraceIDIntoPolydata(iView, this->m_ContourContainer->m_CurrentElement.TraceID, "CONTOUR");

  // should be polydata
  std::vector< vtkActor * > actors =
    VisualizeTrace(iView,
                   this->m_ContourContainer->m_CurrentElement.rgba);
  iView->Delete();

  // should be nodes
  // update the container
  m_ContourContainer->UpdateCurrentElementFromVisu(actors,
                                                   contour_nodes,
                                                   //m_TCoord,
                                                   iTCoord,
                                                   false, //highlighted
                                                   true); //visible

  m_ContourContainer->InsertCurrentElement();

  return 0;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateContour(vtkPolyData *contour_nodes, vtkPolyData *iView)
{
  //generate contour
  vtkOrientedGlyphContourRepresentation *contourRep =
    vtkOrientedGlyphContourRepresentation::New();

  vtkContourWidget *contourWidget = vtkContourWidget::New();

  contourWidget->SetInteractor( this->m_ImageView->GetImageViewer(0)
                                ->GetInteractor() );
  contourWidget->SetRepresentation(contourRep);
  contourWidget->On();

  contourWidget->Initialize(iView);
  contourWidget->CloseLoop();

  contourRep->GetNodePolyData(contour_nodes);

  contourWidget->Delete();
  contourRep->Delete();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SaveMesh(vtkPolyData *iView, int iTCoord, int iCollectionID)
{
  // Compute Bounding Box
  std::vector< int > bounds = this->GetBoundingBox(iView);

  // Save mesh in database
  GoFigureMeshAttributes
      MeshAttributes =
        ComputeMeshAttributes( iView, true, static_cast< unsigned int >( iTCoord ) );

  this->m_DataBaseTables->SaveMeshFromVisuInDB(bounds[0], bounds[2], bounds[4],
                                               bounds[1], bounds[3], bounds[5],
                                               iTCoord,
                                               iView,
                                               &MeshAttributes,
                                               iCollectionID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void
QGoTabImageView3DwT::SaveAndVisuMeshFromSegmentation(vtkPolyData *iView, int iTCoord)
{
  SaveAndVisuMesh(iView, m_TCoord, iTCoord);
}*/
void
QGoTabImageView3DwT::SaveInDBAndRenderMeshForVisu(
  std::vector<vtkPolyData *> iVectPolydata, int iTCoord)
{
  std::vector<vtkPolyData *>::iterator iter = iVectPolydata.begin();
  while(iter != iVectPolydata.end())
    {
    SaveAndVisuMesh(*iter, iTCoord);
    ++iter;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SplitInDBAndRenderMeshForVisu(
  std::vector<vtkPolyData *> iVectPolydata)
{
  size_t N = iVectPolydata.size();

  if( N == 0 )
    {
    return;
    }

  // get mesh trace ID
  std::list< unsigned int > traceID =
      this->m_MeshContainer-> GetHighlightedElementsTraceID();
  // get mesh track ID
  std::list< unsigned int > collectionID =
      this->m_MeshContainer-> GetHighlightedElementsCollectionID();
  // get mesh time point
  std::list< unsigned int > tCoord =
      this->m_MeshContainer-> GetHighlightedElementsTCoord();

  // uncheck this element
  // need it so when we split next mesh, time point and track ID will be accurate
  m_MeshContainer->UpdateElementHighlighting(traceID.front());

  unsigned int timePoint = tCoord.front();

  // Save mesh first mesh, provide track ID
  SaveAndVisuMesh(iVectPolydata[0], timePoint, collectionID.front());

  if( N > 1)
    {
#pragma omp for
    for( size_t i = 1; i<N; ++i )
      {
      SaveAndVisuMesh( iVectPolydata[i], timePoint, 0 );
      }
    }

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::MergeInDBAndRenderMeshForVisu(
  vtkPolyData * iVectPolydata)
{
  if( iVectPolydata == NULL )
    return;

  // get mesh trace ID
  std::list< unsigned int > traceID =
      this->m_MeshContainer-> GetHighlightedElementsTraceID();
  // get mesh track ID
  std::list< unsigned int > collectionID =
      this->m_MeshContainer-> GetHighlightedElementsCollectionID();
  // get mesh time point
  std::list< unsigned int > tCoord =
      this->m_MeshContainer-> GetHighlightedElementsTCoord();

  // uncheck all
  std::list< unsigned int >::iterator iterator = traceID.begin();
  while(iterator != traceID.end())
    {
    m_MeshContainer->UpdateElementHighlighting(*iterator);
    ++iterator;
    }

  // Save mesh first mesh, provide track ID
  SaveAndVisuMesh(iVectPolydata, tCoord.front(), collectionID.front());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SaveInDBAndRenderSetOfContoursForVisu(
  std::vector<std::vector<vtkPolyData*> > iVectorSetOfContours, int iTCoord)
{
  // just add to visu as of now
  std::vector<std::vector<vtkPolyData*> >::iterator it1 =
      iVectorSetOfContours.begin();
  while(it1!=iVectorSetOfContours.end())
    {
    std::vector<vtkPolyData*>::iterator it2 = (*it1).begin();
    while(it2!=(*it1).end())
      {
      vtkPolyData* data = vtkPolyData::New();
      data->DeepCopy(*it2);
      this->AddContour(data);
      ++it2;
      }
    ++it1;
    }
}

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::SaveInDBAndRenderContourForVisu(
  std::vector<vtkPolyData *> iVectPolydata, int iTCoord)
{
  std::vector<vtkPolyData *>::iterator iter = iVectPolydata.begin();
  while(iter != iVectPolydata.end())
    {
      std::cout << "receive polydata" << std::endl;
    vtkPolyData* data = vtkPolyData::New();
    data->DeepCopy(*iter);
    this->AddContour(data);
    //SaveAndVisuContour(*iter, m_TCoord, iTCoord);
    ++iter;
    }

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SaveAndVisuMesh(vtkPolyData *iView,
                                     unsigned int iTCoord,
                                     int iCollectionID)
{
  if ( !iView )
    {
    std::cerr << "Input Mesh is NULL" << std::endl;
    return;
    }

  SaveMesh(iView, iTCoord, iCollectionID);

  // should be done in the mesh manager, from goprintdatabase

  //ADD TRACE ID IN POLYDATA
  AddTraceIDIntoPolydata(iView, this->m_MeshContainer->m_CurrentElement.TraceID, "MESH");

  std::vector< vtkActor * > actors =
    VisualizeTrace(iView,
                   this->m_MeshContainer->m_CurrentElement.rgba);

  // update container since a new mesh is created
  m_MeshContainer->UpdateCurrentElementFromVisu(actors,
                                                iView,
                                                iTCoord,
                                                false,  // highlighted
                                                true);  // visible
  m_MeshContainer->InsertCurrentElement();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::AddContourForMeshToContours(vtkPolyData *iInput)
{
  if ( !m_DataBaseTables->IsDatabaseUsed() )
    {
    std::cerr << "Problem with DB" << std::endl;
    return;
    }

  vtkPolyData *contour_nodes = vtkPolyData::New();
  CreateContour(contour_nodes, iInput);

  //std::list<unsigned int> ListContourIDs;
  if ( ( iInput->GetNumberOfPoints() > 2 ) && ( m_TCoord >= 0 ) )
    {
    // Compute Bounding Box
    std::vector< int > bounds = GetBoundingBox(iInput);
    // Save contour in database :
    //ListContourIDs.push_back(
    this->m_DataBaseTables->SaveNewContourForMeshToContours  (bounds[0],
                                                              bounds[2],
                                                              bounds[4],
                                                              bounds[1],
                                                              bounds[3],
                                                              bounds[5],
                                                              contour_nodes);
    //);

    // AND VISU!!!
    AddTraceIDIntoPolydata(iInput, this->m_ContourContainer->m_CurrentElement.TraceID, "MESH");

    std::vector< vtkActor * > actors =
      VisualizeTrace(iInput,
                     this->m_ContourContainer->m_CurrentElement.rgba);
    iInput->Delete();

    // update the container
    m_ContourContainer->UpdateCurrentElementFromVisu(actors,
                                                     contour_nodes,
                                                     m_TCoord,
                                                     false, //highlighted
                                                     true); //visible

    m_ContourContainer->InsertCurrentElement();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::GoToDefaultMenu(bool iEnable)
{
  if ( iEnable )
    {
    //this->m_ModeActions.at(0)->setChecked(true);
    this->findChild< QAction* >("DefaultMode")->setChecked(true);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoFigureMeshAttributes
QGoTabImageView3DwT::
ComputeMeshAttributes(vtkPolyData *iMesh,
                      const bool & iIntensity,
                      const unsigned int& iTCoord )
{
  typedef unsigned char PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;

  itk::vtkPolyDataToGoFigureMeshAttributes< ImageType >::Pointer
    calculator = itk::vtkPolyDataToGoFigureMeshAttributes< ImageType >::New();
  calculator->SetPolyData(iMesh);
  calculator->SetIntensityBasedComputation(iIntensity);

  GoFigureMeshAttributes oAttributes;
  if(!m_ImageProcessor->getDopplerMode())
    {
    size_t NumberOfChannels = m_ImageProcessor->getNumberOfChannels();

    if( iIntensity )
      {
#pragma omp for
      for( size_t i = 0; i < NumberOfChannels; i++ )
        {
        vtkSmartPointer< vtkImageExport > vtk_exporter =
          vtkSmartPointer< vtkImageExport >::New();
        itk::VTKImageImport< ImageType >::Pointer itk_importer =
          itk::VTKImageImport< ImageType >::New();

        vtk_exporter->SetInput(m_ImageProcessor->getImageBW(i));

        ConnectPipelines< vtkImageExport, itk::VTKImageImport< ImageType >::Pointer >(
          vtk_exporter, itk_importer);
        calculator->SetImage( itk_importer->GetOutput() );
        calculator->Update();

        oAttributes.m_Volume = calculator->GetPhysicalSize();
        oAttributes.m_Area = calculator->GetArea();
        oAttributes.m_Size = calculator->GetSize();

        std::string channelname = m_ImageProcessor->getChannelName(i);

        oAttributes.m_TotalIntensityMap[channelname] =
          static_cast< int >( calculator->GetSumIntensity() );
        oAttributes.m_MeanIntensityMap[channelname] =
          calculator->GetMeanIntensity();
        }
      }
    else
      {
      vtkSmartPointer< vtkImageExport > vtk_exporter =
          vtkSmartPointer< vtkImageExport >::New();
      itk::VTKImageImport< ImageType >::Pointer itk_importer =
        itk::VTKImageImport< ImageType >::New();

      vtk_exporter->SetInput( m_ImageProcessor->getImageBW( 0 ) );

      ConnectPipelines< vtkImageExport, itk::VTKImageImport< ImageType >::Pointer >(
            vtk_exporter, itk_importer);
      calculator->SetImage( itk_importer->GetOutput() );
      calculator->Update();

      oAttributes.m_Volume = calculator->GetPhysicalSize();
      oAttributes.m_Area = calculator->GetArea();
      oAttributes.m_Size = calculator->GetSize();
      }
    }
  else
    {
    unsigned int* boundChannel = m_ImageProcessor->getBoundsChannel();
    unsigned int NumberOfChannels = m_ImageProcessor->getNumberOfChannels();

    std::vector< vtkSmartPointer< vtkImageData > > temp_image( NumberOfChannels );

    // load real image
    m_ImageProcessor->initTimePoint(iTCoord);

    if( iIntensity )
      {
#pragma omp for
      for( unsigned int i = boundChannel[0]; i <= boundChannel[1]; i++ )
        {
        temp_image[i] = m_ImageProcessor->getImageBW(i);

        vtkSmartPointer< vtkImageExport > vtk_exporter =
          vtkSmartPointer< vtkImageExport >::New();
        itk::VTKImageImport< ImageType >::Pointer itk_importer =
          itk::VTKImageImport< ImageType >::New();

        vtk_exporter->SetInput(temp_image[i]);

        ConnectPipelines< vtkImageExport, itk::VTKImageImport< ImageType >::Pointer >(
          vtk_exporter, itk_importer);
        calculator->SetImage( itk_importer->GetOutput() );
        calculator->Update();

        oAttributes.m_Volume = calculator->GetPhysicalSize();
        oAttributes.m_Area = calculator->GetArea();
        oAttributes.m_Size = calculator->GetSize();

        std::string channelname = m_ImageProcessor->getChannelName(i);

        oAttributes.m_TotalIntensityMap[channelname] =
            static_cast< int >( calculator->GetSumIntensity() );
        oAttributes.m_MeanIntensityMap[channelname] =
            calculator->GetMeanIntensity();
        }
      }
    else
      {
      temp_image[0] = m_ImageProcessor->getImageBW(0);

      vtkSmartPointer< vtkImageExport > vtk_exporter =
        vtkSmartPointer< vtkImageExport >::New();
      itk::VTKImageImport< ImageType >::Pointer itk_importer =
        itk::VTKImageImport< ImageType >::New();

      vtk_exporter->SetInput(temp_image[0]);

      ConnectPipelines< vtkImageExport, itk::VTKImageImport< ImageType >::Pointer >(
        vtk_exporter, itk_importer);
      calculator->SetImage( itk_importer->GetOutput() );
      calculator->Update();

      oAttributes.m_Volume = calculator->GetPhysicalSize();
      oAttributes.m_Area = calculator->GetArea();
      oAttributes.m_Size = calculator->GetSize();
      }
    // load doppler image
    // visibility should be updated...
    m_ImageProcessor->setDoppler(m_TCoord, 0);
    }
  return oAttributes;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::ImportContours()
{
  if ( this->m_DataBaseTables->IsDatabaseUsed() )
    {
    this->m_DataBaseTables->ImportContours();

    this->GoToDefaultMenu();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateMeshFromSelectedContours(
  std::list< unsigned int > iListContourIDs,
  int iMeshID)
{
  /// \todo Bug iMeshID is not the one it is supposed to be
  std::list< unsigned int >::const_iterator contourid_it  = iListContourIDs.begin();
  std::list< unsigned int >::const_iterator contourid_end = iListContourIDs.end();

  std::vector< vtkPolyData * > list_contours;

  // get the time point
  unsigned int tcoord = std::numeric_limits< unsigned int >::max();

  while ( contourid_it != contourid_end )
    {
    ContourMeshContainer::MultiIndexContainerTraceIDIterator it0, it1;

    boost::tuples::tie(it0, it1) =
      m_ContourContainer->m_Container.get< TraceID >().equal_range(*contourid_it);

    if ( it0 != it1 )
      {
      if ( tcoord == std::numeric_limits< unsigned int >::max() )
        {
        tcoord = it0->TCoord;
        }
      else
        {
        if ( it0->TCoord != tcoord )
          {
          QMessageBox::warning(
                NULL,
                tr("Generate Mesh From Checked Contours"),
                tr("Selected contours are at different time point: %1 != %2").arg(tcoord).arg( it0->TCoord) );
          return;
          }
        }

      list_contours.push_back(
        vtkPolyData::SafeDownCast(
          it0->ActorXYZ->GetMapper()->GetInput() ) );
      }
    ++contourid_it;
    }

  if ( !list_contours.empty() )
    {
    // make the mesh from the list of contours
    typedef itk::ContourToMeshFilter< std::vector< vtkPolyData * > > FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->ProcessContours(list_contours);

    vtkPolyData *mesh = filter->GetOutput();

    vtkBox *implicitFunction = vtkBox::New();
    // get bounds from the first visible image
    implicitFunction->SetBounds( m_ImageProcessor->getImageBW()->GetBounds() );

    vtkClipPolyData *cutter = vtkClipPolyData::New();
    cutter->SetInput(mesh);
    cutter->InsideOutOn();
    cutter->SetClipFunction(implicitFunction);
    cutter->Update();

    vtkPolyData *temp = vtkPolyData::New();
    temp->DeepCopy( cutter->GetOutput() );

    cutter->Delete();
    implicitFunction->Delete();
    mesh->Delete();

    //as the element is already in the container we need to delete it in order
    //to update it in the SaveAndVisuMesh:
    this->m_MeshContainer->RemoveElementFromVisualizationWithGivenTraceID(iMeshID);
    SaveAndVisuMesh(temp, tcoord);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::ImportMeshes()
{
  if ( this->m_DataBaseTables->IsDatabaseUsed() )
    {
    m_DataBaseTables->ImportMeshes();

    GoToDefaultMenu();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::ImportTracks()
{
  if ( this->m_DataBaseTables->IsDatabaseUsed() )
    {
    std::vector< int > NewTrackIDs =
      m_DataBaseTables->ImportTracks();
    //call the method of the trackContainer to update the points :argument
    // NewTrackIDs
    this->m_TrackContainer->UpdateTracksStrings(NewTrackIDs);
    GoToDefaultMenu();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::GoToLocation(int iX, int iY, int iZ, int iT)
{
  this->SetTimePoint(iT);
  this->SetSliceViewXY(iZ);
  this->SetSliceViewXZ(iY);
  this->SetSliceViewYZ(iX);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AddTraceIDIntoPolydata( vtkPolyData* iPolydata, unsigned int iTraceID, const char* iTrace)
{
  vtkSmartPointer<vtkIntArray> trackIDArray = vtkSmartPointer<vtkIntArray>::New();
  trackIDArray->SetNumberOfComponents(1);
  trackIDArray->SetNumberOfValues(1);
  trackIDArray->SetName(iTrace);
  trackIDArray->SetValue(0, iTraceID);

  iPolydata->GetFieldData()->AddArray(trackIDArray);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetTraceSettingsToolBarVisible(bool IsVisible)
{
  if (!IsVisible)
    {
    if (!this->m_ContourEditingWidget->GetToggleViewAction()->isChecked() &&
        !this->m_MeshEditingWidget->GetToggleViewAction()->isChecked() &&
        !this->m_DataBaseTables->toggleViewAction()->isChecked() )
      {
      this->m_TraceSettingsToolBar->setVisible(IsVisible);
      }
    }
  else
    {
    if (this->m_ContourEditingWidget->GetToggleViewAction()->isChecked())
      {
      this->m_DataBaseTables->SetTraceNameForTableWidget("contour");
      }
    if (this->m_MeshEditingWidget->GetToggleViewAction()->isChecked() )
      {
      this->m_DataBaseTables->SetTraceNameForTableWidget("mesh");
      }
    if(this->m_DataBaseTables->NeedTraceSettingsToolBarVisible() )
      // if the dockwidget is not on floating mode
      // or if the trace settings widget is not already visible in the dockwidget
      // or if the TW is not visible
      {
      this->m_TraceSettingsToolBar->setVisible(IsVisible);
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::InitializeToolsForTracesToolBar(
QMenu* iMenu, QToolBar* iToolBar)
{
  //iToolBar->clear();
  QGoToolBarStatus* TracesToolBar =
    new QGoToolBarStatus(iToolBar, iMenu, Qt::TopToolBarArea, true, true, this);

  QActionGroup* group = this->findChild< QActionGroup* >("ModeGroup");

  //Contour Editing
  QAction *ContourSegmentationAction =
    this->m_ContourEditingWidget->GetToggleViewAction();
  ContourSegmentationAction->setShortcut(tr("2", "Contour Editing Mode"));
  ContourSegmentationAction->setParent(this);
  ContourSegmentationAction->setObjectName("ContourEditingMode");

  group->addAction(ContourSegmentationAction);
  //group->addAction(this->m_ContourSegmentationDockWidget->GetActionForToggle() );

  TracesToolBar->m_VectorAction.push_back(ContourSegmentationAction);
  //this->m_TracesActions.push_back(this->m_ContourSegmentationDockWidget->GetActionForToggle());

  //Mesh Editing
  QAction *MeshSegmentationAction =
    this->m_MeshEditingWidget->GetToggleViewAction();
  MeshSegmentationAction->setShortcut(tr("3", "Mesh Editing Mode"));

  group->addAction(MeshSegmentationAction);

  TracesToolBar->m_VectorAction.push_back(MeshSegmentationAction);

  // Track Color Coding
  TracesToolBar->m_VectorAction.push_back(
    m_TrackViewDockWidget->toggleViewAction() );

  // Lineage Color Coding
  TracesToolBar->m_VectorAction.push_back(
    m_LineageViewDockWidget->toggleViewAction() );

  // Lineage Viewer
  TracesToolBar->m_VectorAction.push_back(
    m_QGoLineageViewerWidget->toggleViewAction() );

  QObject::connect( ContourSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    m_ContourEditingWidget,
                    SLOT( SetVisible(bool) ) );

  QObject::connect( ContourSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    this,
                    SLOT( SetTraceSettingsToolBarVisible(bool) ) );

  QObject::connect( MeshSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    m_MeshEditingWidget,
                    SLOT( SetVisible(bool) ) );

  QObject::connect( MeshSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    this,
                    SLOT( SetTraceSettingsToolBarVisible(bool) ) );

  this->m_ToolBarList.push_back(TracesToolBar);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::InitializeTraceSettingsToolBar(QToolBar* iToolBar)
{
  m_TraceSettingsToolBar = iToolBar;
  QGoToolBarStatus* TraceSettingsToolBar = new QGoToolBarStatus(
    iToolBar, 0, Qt::TopToolBarArea,false,true,this,
    this->m_DataBaseTables->GetTraceSettingsWidgetForToolBar() );

  this->m_ToolBarList.push_back(TraceSettingsToolBar);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::UpdateTracesEditingWidget()
{
  if (this->m_MeshEditingWidget != NULL && this->m_ContourEditingWidget != NULL)
    {
    if (!this->m_ImageProcessor->getDopplerMode())
      {
      this->m_MeshEditingWidget->SetTSliceForClassicView();
      this->m_ContourEditingWidget->SetTSliceForClassicView();
      }
    else
      {
      std::map<QString, QColor> ListTimePoints;
      std::vector<int> dopplerT =
          this->m_ImageProcessor->getDopplerTime(this->m_TCoord);

      for(unsigned int i=0; i<this->m_ImageProcessor->getDopplerSize(); ++i)
      {
        if (dopplerT[i] >= 0 )
          {
          double* rgb = vtkMath::HSVToRGB(
                static_cast<double>(i) /
                static_cast<double>(
                  this->m_ImageProcessor->getDopplerSize()),1,1);
          QColor color(rgb[0]*255, rgb[1]*255, rgb[2]*255);
          ListTimePoints[tr("%1").arg(dopplerT[i])] = color;
          }
      }

      this->m_MeshEditingWidget->SetTSliceForDopplerView(ListTimePoints,
        m_ImageProcessor->getDopplerChannel());
      this->m_ContourEditingWidget->SetTSliceForDopplerView(ListTimePoints,
        m_ImageProcessor->getDopplerChannel());
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateModeToolBar(QMenu* iMenu, QToolBar* iToolBar)
{
  QGoTabElementBase::CreateModeToolBar(iMenu, iToolBar);

  QActionGroup* group = this->findChild<QActionGroup*>("ModeGroup");
  QAction *separator1 = new QAction(this);
  separator1->setSeparator(true);
  this->m_ModeToolBar->m_VectorAction.push_back(separator1);

  //---------------------------------//
  //       Actor picking  mode       //
  //---------------------------------//
  QAction *ActorPickingAction = new QAction(tr("Object Picking"), this);
  ActorPickingAction->setShortcut(tr("P", "Object Picking Mode"));
  ActorPickingAction->setCheckable(true);
  ActorPickingAction->setChecked(false);

  QIcon ActorPickingIcon;
  ActorPickingIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/ObjectPicking.png") ),
                             QIcon::Normal, QIcon::Off);
  ActorPickingAction->setIcon(ActorPickingIcon);
  ActorPickingAction->setStatusTip( tr(
                                      "Select a contour or a mesh (left click when the bounding box of the object of interest is visible)") );

  group->addAction(ActorPickingAction);

  this->m_ModeToolBar->m_VectorAction.push_back(ActorPickingAction);
  // it also updates the interactor behaviour
  QObject::connect( ActorPickingAction, SIGNAL( toggled(bool) ),
                    this, SLOT( ActorPickingInteractorBehavior(bool) ) );

  //---------------------------------//
  //       Box 3D picking  mode      //
  //---------------------------------//
  QAction *Box3DPickingAction = new QAction(tr("Show/hide objects using Box"), this);
  Box3DPickingAction->setShortcut(tr("B", "Box Widget Mode"));
  Box3DPickingAction->setCheckable(true);
  Box3DPickingAction->setChecked(false);

  QIcon Box3DPickingIcon;
  Box3DPickingIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/Box3DPicking.png") ),
                             QIcon::Normal, QIcon::Off);
  Box3DPickingAction->setIcon(Box3DPickingIcon);
  Box3DPickingAction->setStatusTip( tr("Show only the objects in the box") );

  group->addAction(Box3DPickingAction);

  this->m_ModeToolBar->m_VectorAction.push_back(Box3DPickingAction);
  // it also updates the interactor behaviour
  QObject::connect( Box3DPickingAction, SIGNAL( toggled(bool) ),
                    this, SLOT( Box3DPicking(bool) ) );

  //---------------------------------//
  //        Plane  widget  mode      //
  //---------------------------------//
  QAction *PlaneWidgetAction = new QAction(tr("Show/hide objects using Plane"), this);
  PlaneWidgetAction->setCheckable(true);
  PlaneWidgetAction->setChecked(false);

  QIcon PlaneWidgetIcon;
  PlaneWidgetIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/PlaneSelection.png") ),
                            QIcon::Normal, QIcon::Off);
  PlaneWidgetAction->setIcon(PlaneWidgetIcon);
  PlaneWidgetAction->setStatusTip( tr("Show only the objects located in front of the plane") );

  group->addAction(PlaneWidgetAction);

  this->m_ModeToolBar->m_VectorAction.push_back(PlaneWidgetAction);
  // it also updates the interactor behaviour
  QObject::connect( PlaneWidgetAction, SIGNAL( toggled(bool) ),
                    this, SLOT( PlaneWidgetInteractorBehavior(bool) ) );

  // NOT A MODE: ANNOTATION

  QAction *separator3 = new QAction(this);
  separator3->setSeparator(true);
  this->m_ModeToolBar->m_VectorAction.push_back(separator3);

  //---------------------------------//
  //         Distance    mode        //
  //---------------------------------//
  QAction *DistanceAction = new QAction(tr("Measure a Distance"), this);
  DistanceAction->setCheckable(true);
  DistanceAction->setChecked(false);

  QIcon DistanceIcon;
  DistanceIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/Distance.png") ),
                         QIcon::Normal, QIcon::Off);
  DistanceAction->setIcon(DistanceIcon);
  DistanceAction->setStatusTip( tr("Measure a distance between 2 points (left click to place/drag the points)") );

  group->addAction(DistanceAction);

  this->m_ModeToolBar->m_VectorAction.push_back(DistanceAction);

  QObject::connect( DistanceAction, SIGNAL( toggled(bool) ),
                    this, SLOT( DistanceWidgetInteractorBehavior(bool) ) );

  // NOT A MODE: ANNOTATION

  //---------------------------------//
  //           Angle     mode        //
  //---------------------------------//
  QAction *AngleAction = new QAction(tr("Measure an Angle"), this);
  AngleAction->setCheckable(true);
  AngleAction->setChecked(false);

  QIcon AngleIcon;
  AngleIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/Angle.png") ),
                      QIcon::Normal, QIcon::Off);
  AngleAction->setIcon(AngleIcon);
  AngleAction->setStatusTip( tr("Measure an angle between 3 points (left click to place/drag the points)") );

  group->addAction(AngleAction);

  this->m_ModeToolBar->m_VectorAction.push_back(AngleAction);

  QObject::connect( AngleAction, SIGNAL( toggled(bool) ),
                    this, SLOT( AngleWidgetInteractorBehavior(bool) ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
visibilityChanged(QString iName, bool iVisibility)
{
  m_ImageProcessor->visibilityChanged(iName.toStdString(), iVisibility);
  UpdateImage();
  //if we are in volume rendering
  EnableVolumeRendering( this->m_VolumeRenderingEnabled );

  // update visu
  m_ImageView->Update();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
DopplerSizeChanged(int iDopplerSize)
{
  // delete previous doppler widget
  this->m_NavigationDockWidget->DeleteDopplerWidgets();
  // set the new doppler step
  m_ImageProcessor->setDopplerSize(iDopplerSize);
  // update the image processor
  m_ImageProcessor->setDoppler(m_TCoord, 0); // 0 is for optimization later on...
  //rebuild navigation widget
  BuildDopplerWidget();
  // build new image
  UpdateImage();
  //update
  m_ImageView->Update();
  //update the trace editing widget
  UpdateTracesEditingWidget();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoTransferFunctionEditorWidget*
QGoTabImageView3DwT::
createTransferFunctionEditor(QString iName)
{
  // create editor
  // get LUT parameters (gamma, min, max)
  std::vector<int> lutParameters =
      m_ImageProcessor->getLUTParameters(iName.toStdString());

  GoTransferFunctionEditorWidget* editor =
      new GoTransferFunctionEditorWidget(NULL,
                                         iName,
                                         m_ImageProcessor->getColor(iName.toStdString()),
                                         lutParameters,
                                         m_ImageProcessor->getImageBW(iName.toStdString())->GetScalarRange()[1]);
  // connect signals

  QObject::connect( editor,
                    SIGNAL( updateVisualization() ),
                    this,
                    SLOT( updateSlot()) );

  QObject::connect( editor,
                    SIGNAL( UpdateImageStructure(QString,
                                         std::map< unsigned int, unsigned int>,
                                         QColor,
                                         int,
                                         int,
                                         int) ),
                    this,
                    SLOT( updatePoints(QString,
                                       std::map< unsigned int, unsigned int>,
                                       QColor,
                                       int,
                                       int,
                                       int)) );

  QObject::connect(this->m_ImageView, SIGNAL(NewWindowLevel(double, double)),
                   this, SLOT(AdjustWindowLevel(double, double)));

  // show editor - to have consistent geomerty to add the points
  editor->show();
  // add points
  editor->AddPoints(m_ImageProcessor->getAlpha(iName.toStdString()));
  // add LUT
  editor->AddLookupTable(m_ImageProcessor->getLookuptable(iName.toStdString()));
  // add Opacity TF
  editor->AddOpacityTransferFunction(
        m_ImageProcessor->getOpacityTransferFunction(iName.toStdString()));
  // add histogram - should not recalculate all the time...
  editor->AddHistogram(m_ImageProcessor->getHistogram(iName.toStdString()));
  // hide editor
  editor->hide();

  //editor->setParent(m_TransferFunctionDockWidget);
  m_TransferFunctionDockWidget->AddTransferFunction(iName, editor);

  return editor;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// color changed
void
QGoTabImageView3DwT::updateSlot()
{
  UpdateImage();
  EnableVolumeRendering( this->m_VolumeRenderingEnabled );

  m_ImageView->Update();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
updatePoints(QString iName,
             std::map< unsigned int, unsigned int> iPoints,
             QColor iColor,
             int iMin,
             int iMax,
             int iGamma)
{
  // update opacity TF points
  m_ImageProcessor->updatePoints(iName.toStdString(), iPoints);

  //color
  std::vector<double> color;
  color.push_back(iColor.redF()*255);
  color.push_back(iColor.greenF()*255);
  color.push_back(iColor.blueF()*255);

  m_ImageProcessor->setColor(iName.toStdString(), color);

  //LUT parameters
  m_ImageProcessor->setLUTParameters(iName.toStdString(), iGamma, iMin, iMax);

  // color of button in navigation widget
  m_NavigationDockWidget->ModifyChannel(iName, iColor);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
EnableVolumeRendering(bool iEnable)
{
  if(iEnable)
    {
    m_VolumeRenderingEnabled = true;
    m_ImageView->EnableVolumeRendering(m_ImageProcessor->getColoredImages(),
                                       m_ImageProcessor->getOpacityTransferFunctions());
    }
  else
    {
    m_VolumeRenderingEnabled = false;
    m_ImageView->DisableVolumeRendering();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateContoursActorsFromVisuContainer()
{
  //check mode from print db
  std::list<unsigned int> tp = m_DataBaseTables->GetVisibleTimePoints();

  // load all tp
  if(tp.size() == 0)
    {
    unsigned int* time = m_ImageProcessor->getBoundsTime();
    for(unsigned int i = time[0]; i<time[1]+1; i++)
      {
      tp.push_back(i);
      }
    }

  CreateContoursActorsFromVisuContainer(tp);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateContoursActorsFromVisuContainer(std::list<unsigned int> iTPointToLoad)
{
  typedef ContourContainer::MultiIndexContainerType::index< TCoord >::type::iterator
    ContourContainerTCoordIterator;

  if ( this->m_ContourContainer )
    {
    // load everything if no list given
    if ( !iTPointToLoad.empty() )
      {
      std::list<unsigned int>::const_iterator it  = iTPointToLoad.begin();
      std::list<unsigned int>::const_iterator end = iTPointToLoad.end();

      while(it != end)
        {
        ContourContainerTCoordIterator it0, it1;
        // let's iterate on the container with increasing TraceID
        boost::tuples::tie(it0, it1)
          = this->m_ContourContainer->m_Container.get< TCoord >().equal_range(*it);

        // we don't need here to save this contour in the database,
        // since they have just been extracted from it!
        while ( it0 != it1 )
          {
          this->AddContourFromNodes< TCoord >( it0 );
          ++it0;
          }
        ++it;
        }
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateMeshesActorsFromVisuContainer()
{
  //check mode from print db
  std::list<unsigned int> tp = m_DataBaseTables->GetVisibleTimePoints();

  // load all tp
  if( tp.empty() )
    {
    unsigned int* time = m_ImageProcessor->getBoundsTime();
    for(unsigned int i = time[0]; i<time[1]+1; i++)
      {
      tp.push_back(i);
      }
    }

  CreateMeshesActorsFromVisuContainer(tp);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateMeshesActorsFromVisuContainer(std::list<unsigned int> iTPointToLoad)
{
  typedef MeshContainer::MultiIndexContainerType::index< TCoord >::type::iterator
    MeshContainerTCoordIterator;

  int lastTP = m_ImageProcessor->getBoundsTime()[1];
  std::stringstream lastTimePoint;
  lastTimePoint << lastTP;

  if( this->m_MeshContainer)
    {
    // load everything if no list given
    if ( !iTPointToLoad.empty() )
      {
      std::list<unsigned int>::const_iterator it = iTPointToLoad.begin();
      std::list<unsigned int>::const_iterator end = iTPointToLoad.end();

      while( it != end )
        {
        MeshContainerTCoordIterator it0, it1;

        boost::tuples::tie(it0, it1) =
          this->m_MeshContainer->m_Container.get< TCoord >().equal_range(*it);

        // progress bar
        int size = this->m_DataBaseTables->
                   GetNumberOfElementForTraceAndTimePoint("mesh", *it);
        std::stringstream time;
        time << *it;

        QProgressDialog progress( QString::fromStdString("Loading Meshes from T=" + time.str() +"/" + lastTimePoint.str()),
                                  QString(),
                                  0,
                                  size);
        size_t i = 0;
        progress.setValue( i );

        // we don't need here to save this mesh in the database,
        // since they have just been extracted from it!
        while ( it0 != it1 )
          {
          if ( it0->Nodes )
            {
            // bug here, don't use TCoord
            GoFigureMeshAttributes attributes =
              this->ComputeMeshAttributes(
                it0->Nodes, // mesh
                false, // do not need to compute intensity based measure
                it0->TCoord
                );
            this->m_DataBaseTables->PrintVolumeAreaForMesh(
              &attributes, it0->TraceID);
            }
          this->AddMeshFromNodes< TCoord >( it0 );
          ++it0;

          ++i;
          progress.setValue( i );
          }

        progress.setValue(size);
        ++it;
        }
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ShowTraces(const unsigned int& iTimePoint)
{
  // only contours and meshes will be modified since tracks and lineage contain
  // several time points
  this->m_ContourContainer->ShowActorsWithGivenTimePoint(iTimePoint);
  this->m_MeshContainer->ShowActorsWithGivenTimePoint(iTimePoint);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AdjustWindowLevel(double iMin, double iMax)
{
  int index = m_NavigationDockWidget->GetFirstVisibleChannel();
  // if one channel is checked
  // (if no channel selected, index == -1, we don't do anything)
  if(index >= 0)
    {
    this->m_TransferFunctionDockWidget->SetCurrentWidget(index);
    GoTransferFunctionEditorWidget* widget =
        dynamic_cast<GoTransferFunctionEditorWidget*>(
        this->m_TransferFunctionDockWidget->GetWidget(index));
    widget->AdjustWindowLevel(iMin, iMax);
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void
QGoTabImageView3DwT::
UpdateTFEditor()
{
  unsigned int NumberOfChannels = m_ImageProcessor->getNumberOfChannels();

  for( unsigned int i = 0; i < NumberOfChannels; i++ )
    {
    std::string name = m_ImageProcessor->getChannelName(i);
    m_TransferFunctionDockWidget->SetCurrentWidget(i);
    GoTransferFunctionEditorWidget* widget =
      dynamic_cast<GoTransferFunctionEditorWidget*>(
        m_TransferFunctionDockWidget->GetWidget(i) );

    // update histogram
    widget->AddHistogram(m_ImageProcessor->getHistogram(name) );

    // update max value
    widget->SetMaximumValue(
        m_ImageProcessor->getImageBW(name)->GetScalarRange()[1]);
    }
}
//------------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
PolydatasRequested(){
  std::list< vtkPolyData* > elements =
      this->m_MeshContainer-> GetHighlightedElements();
  emit RequestedPolydatas(elements);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetUpShortcuts(){
  // move to next time point
  (void)new QShortcut(
      QKeySequence(
          tr("Ctrl+C", "Next Time Point")),
      this, SLOT(MoveToNextTimePoint()));
  (void)new QShortcut(
      QKeySequence(
          tr("Right Arrow", "Next Time Point")),
      this, SLOT(MoveToNextTimePoint()));

  // move to previous time point
  (void)new QShortcut(
      QKeySequence(
          tr("Ctrl+Z", "Previous Time Point")),
      this, SLOT(MoveToPreviousTimePoint()));
  (void)new QShortcut(
      QKeySequence(
          tr("Left Arrow", "Next Time Point")),
      this, SLOT(MoveToPreviousTimePoint()));

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
MoveToNextTimePoint()
{
  m_NavigationDockWidget->MoveToNextTimePoint();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
MoveToPreviousTimePoint()
{
  m_NavigationDockWidget->MoveToPreviousTimePoint();
}
