/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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

#include "QGoImageView3D.h"
#include "QGoLUTDialog.h"
#include "QGoNavigationDockWidget.h"
#include "GoDBCoordinateRow.h"
#include "GoDBMeshRow.h"

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
#include "QGoTraceManualEditingWidget.h"

#include <QCursor>

#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QInputDialog>

#include <set>

// base segmentation dock widgets
#include "QGoContourSegmentationBaseDockWidget.h"
#include "QGoMeshSegmentationBaseDockWidget.h"

// TESTS
#include "vtkPolyDataWriter.h"
#include "vtkViewImage3D.h"

//-------------------------------------------------------------------------
QGoTabImageView3DwT::QGoTabImageView3DwT(QWidget *iParent):
  QGoTabElementBase(iParent),
  m_LSMReader(0),
  m_Image(0),
  m_BackgroundColor(Qt::black),
  m_PCoord(0),
  m_RCoord(0),
  m_CCoord(0),
  m_XTileCoord(0),
  m_YTileCoord(0),
  m_ZTileCoord(0),
  m_TCoord(-1),
  m_TraceWidgetRequiered(false)
{
  m_Image = vtkImageData::New();
  m_Seeds = vtkPoints::New();

  m_ChannelClassicMode = true;
  m_ChannelOfInterest = 0;

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

  setupUi(this);

  m_MegaCaptureReader = itk::MegaCaptureReader::New();

  m_ContourContainer = new ContourMeshContainer(this, this->m_ImageView);
  m_ContourContainer->SetHighlightedProperty(m_HighlightedContoursProperty);

  m_MeshContainer = new ContourMeshContainer(this, this->m_ImageView);
  m_MeshContainer->SetHighlightedProperty(m_HighlightedMeshesProperty);

  m_TrackContainer = new TrackContainer(this, this->m_ImageView);
  m_TrackContainer->SetHighlightedProperty(m_HighlightedMeshesProperty);

  CreateVisuDockWidget();

  // segmentation dockwidgets
  CreateContourSegmentationDockWidget();
  CreateMeshSegmentationDockWidget();

  CreateDataBaseTablesConnection();

#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  CreateVideoRecorderWidget();
#endif

  CreateAllViewActions();

  CreateToolsActions();

  CreateBookmarkActions();

  CreateModeActions();

  ReadSettings();

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(
        m_NavigationDockWidget, Qt::RightDockWidgetArea, true, true),
      m_NavigationDockWidget) );

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(
        m_ContourSegmentationDockWidget, Qt::LeftDockWidgetArea, true, true),
      m_ContourSegmentationDockWidget) );

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(
        m_MeshSegmentationDockWidget, Qt::LeftDockWidgetArea, true, true),
      m_MeshSegmentationDockWidget) );

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(this->m_DataBaseTables->GetTraceManualEditingDockWidget(),
                              Qt::LeftDockWidgetArea, true, true),
      this->m_DataBaseTables->GetTraceManualEditingDockWidget() ) );

#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(m_VideoRecorderWidget, Qt::LeftDockWidgetArea, true, true),
      m_VideoRecorderWidget) );
#endif
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTabImageView3DwT::
~QGoTabImageView3DwT()
{
  m_HighlightedContoursProperty->Delete();
  m_HighlightedMeshesProperty->Delete();

  m_Image->Delete();
  m_Seeds->Delete();

  if ( !m_LSMReader.empty() )
    {
    if ( m_LSMReader[0] )
      {
      m_LSMReader[0]->Delete();
      }
    }

  // clean the LSMReader vector
  /*while(!m_LSMReader.empty())
    {
    m_LSMReader.back()->Delete();
    m_LSMReader.pop_back();
    }
*/
  unsigned int minch = m_MegaCaptureReader->GetMinChannel();
  unsigned int maxch = m_MegaCaptureReader->GetMaxChannel();

  for ( unsigned int i = minch; i < maxch + 1; i++ )
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
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::UpdateSeeds()
{
  vtkPoints *temp = m_ImageView->GetAllSeeds();

  m_Seeds->DeepCopy(temp);
  temp->Delete();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateContourSegmentationDockWidget()
{
  //----------------------------------------------------------------
  // Create base  contour segmentation dock widget:
  // basic interactor connections
  //----------------------------------------------------------------

  m_ContourSegmentationDockWidget =
    new QGoContourSegmentationBaseDockWidget(this, m_Seeds, &m_InternalImages);

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL( ReinitializeInteractorActivated(bool) ),
                    this,
                    SLOT( DefaultInteractorBehavior(bool) ) );

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL( ManualSegmentationActivated(bool) ),
                    this,
                    SLOT( ManualInteractorBehavior(bool) ) );

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL( SemiAutoSegmentationActivated(bool) ),
                    this,
                    SLOT( SeedInteractorBehavior(bool) ) );

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL( AutoSegmentationActivated(bool) ),
                    this,
                    SLOT( DefaultInteractorBehavior(bool) ) );

  // signals from the manual segmentation
  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL( ValidateContour() ),
                    this, SLOT( ValidateContour() ) );

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL( ReinitializeContourWidget() ),
                    m_ImageView,
                    SLOT( ReinitializeContourWidget() ) );

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL( UpdateContourRepresentationProperties(float, QColor,
                                                                  QColor, QColor) ),
                    m_ImageView,
                    SLOT( UpdateContourRepresentationProperties(float, QColor,
                                                                QColor, QColor) ) );

  // signals for the semi automated segmentation
  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL( UpdateSeeds() ),
                    this,
                    SLOT( UpdateSeeds() ) );

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL( SaveAndVisuContour(vtkPolyData *) ),
                    this,
                    SLOT( SaveAndVisuContour(vtkPolyData *) ) );

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL( ClearAllSeeds() ),
                    m_ImageView,
                    SLOT( ClearAllSeeds() ) );

  // signals for the automatic segmentation
  // ...

  // intialize the widget
  m_ContourSegmentationDockWidget->Initialize();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateMeshSegmentationDockWidget()
{
  //----------------------------------------------------------------
  // Create base  contour segmentation dock widget:
  // basic interactor connections
  //----------------------------------------------------------------

  m_MeshSegmentationDockWidget =
    new QGoMeshSegmentationBaseDockWidget(this, m_Seeds, &m_InternalImages);

  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL( ReinitializeInteractorActivated(bool) ),
                    this,
                    SLOT( DefaultInteractorBehavior(bool) ) );

  // we need seeds for the manual mesh segmentation
  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL( ManualSegmentationActivated(bool) ),
                    this,
                    SLOT( SeedInteractorBehavior(bool) ) );

  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL( SemiAutoSegmentationActivated(bool) ),
                    this,
                    SLOT( SeedInteractorBehavior(bool) ) );

  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL( AutoSegmentationActivated(bool) ),
                    this,
                    SLOT( DefaultInteractorBehavior(bool) ) );

  // signals for manual segmentation
  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL ( CreateCorrespondingMesh(int) ),
                    this->m_DataBaseTables,
                    SLOT ( SaveNewMeshForMeshToContours(int) ) );

  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL( AddContourForMeshToContours(vtkPolyData *) ),
                    this,
                    SLOT( AddContourForMeshToContours(vtkPolyData *) ) );

  // signals for the semi automatic segmentation
  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL( UpdateSeeds() ),
                    this,
                    SLOT( UpdateSeeds() ) );

  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL( SaveAndVisuMesh(vtkPolyData *) ),
                    this,
                    SLOT( SaveAndVisuMesh(vtkPolyData *) ) );

  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL( ClearAllSeeds() ),
                    m_ImageView,
                    SLOT( ClearAllSeeds() ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::ManualInteractorBehavior(bool iVisible)
{
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
QGoTabImageView3DwT::PanInteractorBehavior(bool iVisible)
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

  QObject::connect( m_NavigationDockWidget, SIGNAL( ShowAllChannelsChanged(bool) ),
                    this, SLOT( ShowAllChannels(bool) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( ShowOneChannelChanged(int) ),
                    this, SLOT( ShowOneChannel(int) ) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CreateDataBaseTablesConnection()
{
  QObject::connect ( this->m_DataBaseTables,
                     SIGNAL( DBVariablesSet() ),
                     this,
                     SLOT( SetTheContainersForDB() ) );

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
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::CloseTabRequest(bool iTable)
{
  if ( iTable )
    {
    m_DataBaseTables->GetTraceManualEditingDockWidget()->show();
    return;
    }

  if ( !m_TraceWidgetRequiered )
    {
    m_DataBaseTables->GetTraceManualEditingDockWidget()->hide();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::RequieresTraceWidget(bool iTable)
{
  m_TraceWidgetRequiered = iTable;
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetTheContainersForDB()
{
  m_DataBaseTables->SetContoursContainer(m_ContourContainer);
  m_DataBaseTables->SetMeshesContainer(m_MeshContainer);
  m_DataBaseTables->SetTracksContainer(m_TrackContainer);
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
  /*
   * COMMENTED FROM HOTFIXES
   *
  QActionGroup *groupMode = new QActionGroup(this);

  QAction *ChannelClassic = new QAction(tr("Classic-View"), this);

  ChannelClassic->setCheckable(true);
  ChannelClassic->setChecked(true);

  QIcon ChannelClassicIcon;
  ChannelClassicIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/VisuClassic.png") ),
                         QIcon::Normal, QIcon::Off);
  ChannelClassic->setIcon(ChannelClassicIcon);

  groupMode->addAction(ChannelClassic);

  this->m_ViewActions.push_back(ChannelClassic);

  //-------------------------
  //
  //-------------------------

  QAction *ChannelTime = new QAction(tr("Time-View"), this);

  ChannelTime->setCheckable(true);
  ChannelTime->setChecked(false);

  QIcon ChannelTimeIcon;
  ChannelTimeIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/VisuTime.png") ),
                         QIcon::Normal, QIcon::Off);
  ChannelTime->setIcon(ChannelTimeIcon);

  groupMode->addAction(ChannelTime);

  this->m_ViewActions.push_back(ChannelTime);

  QObject::connect( ChannelTime, SIGNAL( triggered() ),
                    this, SLOT( LoadChannelTime() ) );

  QObject::connect( ChannelTime, SIGNAL( toggled( bool ) ),
                    this, SLOT( ChannelTimeMode( bool ) ) );

  //-------------------------
  //
  //-------------------------

  QAction *separator1 = new QAction(this);
  separator1->setSeparator(true);
  this->m_ViewActions.push_back(separator1);

  */

  //-------------------------
  //
  //-------------------------

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

  this->m_ViewActions.push_back(ActionDisplayAnnotations);

  QAction *ActionDisplaySplinePlanes =
    new QAction(tr("Display spline planes"), this);
  ActionDisplaySplinePlanes->setCheckable(true);
  ActionDisplaySplinePlanes->setChecked(true);
  ActionDisplaySplinePlanes->setStatusTip( tr(" Display spline planes on each view") );

  QIcon displaysplineplaneicon;
  displaysplineplaneicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/C_M_L.png") ),
                                   QIcon::Normal, QIcon::Off);
  ActionDisplaySplinePlanes->setIcon(displaysplineplaneicon);

  QObject::connect( ActionDisplaySplinePlanes, SIGNAL( triggered() ),
                    this->m_ImageView, SLOT( ShowSplinePlane() ) );

  this->m_ViewActions.push_back(ActionDisplaySplinePlanes);

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

  this->m_ViewActions.push_back(DisplayCube3D);

  QAction *separator3 = new QAction(this);
  separator3->setSeparator(true);

  this->m_ViewActions.push_back(separator3);

  QAction *LookupTableAction = new QAction(tr("Lookup Table"), this);
  LookupTableAction->setEnabled(false);
  LookupTableAction->setStatusTip( tr(" Change the associated lookup table") );

  //take
  QIcon luticon;
  luticon.addPixmap(QPixmap( QString::fromUtf8(":/fig/LookupTable.png") ),
                    QIcon::Normal, QIcon::Off);
  LookupTableAction->setIcon(luticon);

  // Here write the connection
  QObject::connect( LookupTableAction, SIGNAL( triggered() ),
                    this, SLOT( ChangeLookupTable() ) );

  this->m_ViewActions.push_back(LookupTableAction);

  QAction *ScalarBarAction = new QAction(tr("Display Scalar Bar"), this);
  ScalarBarAction->setEnabled(false);
  ScalarBarAction->setCheckable(true);

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

  QAction *separator5 = new QAction(this);
  separator5->setSeparator(true);
  this->m_ViewActions.push_back(separator5);

  this->m_ViewActions.push_back( m_DataBaseTables->toggleViewAction() );

  // to know if we should close the trace widget as well
  QObject::connect( m_DataBaseTables->toggleViewAction(), SIGNAL( toggled(bool) ),
                    this, SLOT( CloseTabRequest(bool) ) );

  QAction *separator6 = new QAction(this);
  separator6->setSeparator(true);
  this->m_ViewActions.push_back(separator6);

  /// \todo create group actions for views changing
  QAction *Change3DPerspectiveToAxialAction =
    new QAction(tr("Change 3D view to Posterior "), this);
  this->m_ViewActions.push_back(Change3DPerspectiveToAxialAction);

  QIcon axialicon;
  axialicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/PosteriorView.png") ),
                      QIcon::Normal, QIcon::Off);
  Change3DPerspectiveToAxialAction->setIcon(axialicon);

  QObject::connect( Change3DPerspectiveToAxialAction, SIGNAL( triggered() ),
                    this, SLOT( Change3DPerspectiveToAxial() ) );

  QAction *Change3DPerspectiveToCoronalAction =
    new QAction(tr("Change 3D view to Dorsal "), this);
  this->m_ViewActions.push_back(Change3DPerspectiveToCoronalAction);

  QIcon coronalicon;
  coronalicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/DorsalView.png") ),
                        QIcon::Normal, QIcon::Off);
  Change3DPerspectiveToCoronalAction->setIcon(coronalicon);

  QObject::connect( Change3DPerspectiveToCoronalAction, SIGNAL( triggered() ),
                    this, SLOT( Change3DPerspectiveToCoronal() ) );

  QAction *Change3DPerspectiveToSagittalAction =
    new QAction(tr("Change 3D view to Left "), this);
  this->m_ViewActions.push_back(Change3DPerspectiveToSagittalAction);

  QIcon sagittalicon;
  sagittalicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/LeftView.png") ),
                         QIcon::Normal, QIcon::Off);
  Change3DPerspectiveToSagittalAction->setIcon(sagittalicon);

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
                    this->m_ImageView, SLOT( EnableVolumeRendering(bool) ) );
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ChannelTimeMode( bool iEnable )
{
  m_ChannelClassicMode = !iEnable;

  // if we leave the time mode, go back to the classic mode automatically
  if(m_ChannelClassicMode)
    {
    unsigned int min_ch = m_MegaCaptureReader->GetMinChannel();
    unsigned int max_ch = m_MegaCaptureReader->GetMaxChannel();
    unsigned int NumberOfChannels = max_ch - min_ch + 1;
    // resize internal and update the internal image
    m_InternalImages.resize(NumberOfChannels, NULL);
    SetTimePointWithMegaCapture();

    // Update navigation widget
    // Initialize the widgets with the good number of channels
    // it will update the size of the related combobox
    m_NavigationDockWidget->blockSignals(true);
    m_NavigationDockWidget->SetNumberOfChannels(NumberOfChannels);
    m_ContourSegmentationDockWidget->SetNumberOfChannels(NumberOfChannels);
    m_MeshSegmentationDockWidget->SetNumberOfChannels(NumberOfChannels);


    if ( NumberOfChannels > 1 )
      {
      m_NavigationDockWidget->SetChannel(0);
      m_ContourSegmentationDockWidget->SetChannel(0);
      m_MeshSegmentationDockWidget->SetChannel(0);
      for ( unsigned int i = 1; i < NumberOfChannels; i++ )
        {
        m_NavigationDockWidget->SetChannel(i);
        m_ContourSegmentationDockWidget->SetChannel(i);
        m_MeshSegmentationDockWidget->SetChannel(i);
        }
      }
    m_NavigationDockWidget->blockSignals(false);
    // update visualization
    Update();
    }
}

//-------------------------------------------------------------------------

void QGoTabImageView3DwT::LoadChannelTime()
{
  bool ok;
  QStringList channel;
  unsigned int minch = m_MegaCaptureReader->GetMinChannel();
  unsigned int maxch = m_MegaCaptureReader->GetMaxChannel();
  maxch += maxch;

  for(unsigned int i = minch; i < maxch; ++i)
    {
    channel << QString::number(i, 10);
    }

  QString item = QInputDialog::getItem(this,
      tr("Channel selection"),
      tr("Please select the channel you want to track"),
      channel, 0, false, &ok);

  if ( ok )
    {
     // qDebug() << "user selected an item and pressed OK";
      // use the item
      int value = item.toInt(&ok, 10);
      //qDebug() << "value:" << value;
      // emit with channel...
      // keep track of channel of interest when we move through time
      m_ChannelOfInterest = value;
      SetTimePointWithMegaCaptureTimeChannels( value );
      Update();
    }
  else
    {
    //qDebug() << "user selected an item and pressed CANCEL";
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
void QGoTabImageView3DwT::CreateModeActions()
{
  QActionGroup *group = new QActionGroup(this);

  // Call superclass
  QGoTabElementBase::CreateModeActions(group);

  QAction *separator1 = new QAction(this);
  separator1->setSeparator(true);
  this->m_ModeActions.push_back(separator1);

  //--------------------------------//
  //  Contour segmentation mode     //
  //--------------------------------//

  QAction *ContourSegmentationAction =
    m_ContourSegmentationDockWidget->toggleViewAction();

  group->addAction(ContourSegmentationAction);

  this->m_ModeActions.push_back(ContourSegmentationAction);

  QObject::connect( ContourSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    m_ContourSegmentationDockWidget,
                    SLOT( interactorBehavior(bool) ) );

  QObject::connect( ContourSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    this,
                    SLOT( RequieresTraceWidget(bool) ) );

  QObject::connect( ContourSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    this,
                    SLOT( ShowTraceDockWidgetForContour(bool) ) );

  //---------------------------------//
  //        Mesh segmentation        //
  //---------------------------------//

  QAction *MeshSegmentationAction =
    m_MeshSegmentationDockWidget->toggleViewAction();

  group->addAction(MeshSegmentationAction);

  this->m_ModeActions.push_back(MeshSegmentationAction);

  QObject::connect( MeshSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    m_MeshSegmentationDockWidget,
                    SLOT( interactorBehavior(bool) ) );

  QObject::connect( MeshSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    this,
                    SLOT( RequieresTraceWidget(bool) ) );

  QObject::connect( MeshSegmentationAction,
                    SIGNAL( toggled(bool) ),
                    this,
                    SLOT( ShowTraceDockWidgetForMesh(bool) ) );

  QAction *separator2 = new QAction(this);
  separator2->setSeparator(true);
  this->m_ModeActions.push_back(separator2);

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
}

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
  QAction *ImportMeshesAction = new QAction(tr("3DMeshes"), this);
  ImportMenu->addAction(ImportContoursAction);
  ImportMenu->addAction(ImportMeshesAction);
  QMenu *  ExportMenu = new QMenu(tr("Export"), this);
  QAction *ExportContoursAction = new QAction(tr("Contours"), this);
  QAction *ExportMeshesAction = new QAction(tr("3DMeshes"), this);
  ExportMenu->addAction(ExportContoursAction);
  ExportMenu->addAction(ExportMeshesAction);
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
  for ( size_t i = 0; i < NumberBookmarks; i++ )
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

  int tt = atoi( Coord.GetMapValue("TCoord").c_str() );
  int tz = atoi( Coord.GetMapValue("ZCoord").c_str() );
  int ty = atoi( Coord.GetMapValue("YCoord").c_str() );
  int tx = atoi( Coord.GetMapValue("XCoord").c_str() );

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

  switch ( FullScreenView )
    {
    case 1:
      filename.append( "snapshot-xy-" );
      m_ImageView->SnapshotViewXY(GoFigure::PNG, filename);
      break;

    case 2:
      filename.append( "snapshot-xz-" );
      m_ImageView->SnapshotViewXZ(GoFigure::PNG, filename);
      break;

    case 3:
      filename.append( "snapshot-yz-" );
      m_ImageView->SnapshotViewYZ(GoFigure::PNG, filename);
      break;

    default:
      filename.append( "snapshot-xyz-" );
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

  m_VSplitter  = new QSplitter(Qt::Vertical, iParent);
  m_DataBaseTables = new QGoPrintDatabase;
  m_VSplitter->addWidget(m_DataBaseTables);
  m_DataBaseTables->hide();

  m_ImageView = new QGoImageView3D;
  m_ImageView->SetBackgroundColor(m_BackgroundColor);
  m_VSplitter->addWidget(m_ImageView);

  QObject::connect( m_ImageView, SIGNAL( SliceViewXYChanged(int) ),
                    this, SIGNAL( SliceViewXYChanged(int) ) );

  QObject::connect( m_ImageView, SIGNAL( SliceViewXZChanged(int) ),
                    this, SIGNAL( SliceViewXZChanged(int) ) );

  QObject::connect( m_ImageView, SIGNAL( SliceViewYZChanged(int) ),
                    this, SIGNAL( SliceViewYZChanged(int) ) );

  QObject::connect( m_ImageView, SIGNAL( FullScreenViewChanged(int) ),
                    this, SIGNAL( FullScreenViewChanged(int) ) );
/*
  // connect the contours selection connection
  QObject::connect(m_ImageView, SIGNAL(ContoursSelectionChanged()),
                   this, SLOT(HighLightContours()));

  QObject::connect(m_ImageView, SIGNAL(ContoursSelectionChanged()),
                   this, SLOT(SelectContoursInTable()));*/

  // connect the contours selection connection
  /*QObject::connect(m_ImageView, SIGNAL(ContoursSelectionXYChanged()),
                   this, SLOT(HighlightContoursXY()));*/
  QObject::connect( m_ImageView, SIGNAL( SelectionXYChanged() ),
                    this, SLOT( HighlightXY() ) );
  // connect the contours selection connection
  QObject::connect( m_ImageView, SIGNAL( SelectionXZChanged() ),
                    this, SLOT( HighlightXZ() ) );
  // connect the contours selection connection
  QObject::connect( m_ImageView, SIGNAL( SelectionYZChanged() ),
                    this, SLOT( HighlightYZ() ) );
  // connect the contours selection connection
  QObject::connect( m_ImageView, SIGNAL( SelectionXYZChanged() ),
                    this, SLOT( HighlightXYZ() ) );

  /*
  QObject::connect(m_ImageView, SIGNAL(VisibilityXYChanged()),
                     this, SLOT(VisibilityXY()));
  // connect the contours selection connection
  QObject::connect(m_ImageView, SIGNAL(VisibilityXZChanged()),
                   this, SLOT(VisibilityXZ()));
  // connect the contours selection connection
  QObject::connect(m_ImageView, SIGNAL(VisibilityYZChanged()),
                   this, SLOT(VisibilityYZ()));*/
  // connect the contours selection connection
  QObject::connect( m_ImageView, SIGNAL( VisibilityXYZChanged() ),
                    this, SLOT( VisibilityXYZ() ) );

  m_HBoxLayout = new QHBoxLayout(iParent);
  m_HBoxLayout->addWidget(m_VSplitter);

  retranslateUi(iParent);

  QMetaObject::connectSlotsByName(iParent);
} // setupUi

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

//#########################################################################
//#########################################################################
// Set Inputs

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetLSMReader(vtkLSMReader *iReader, const int & iTimePoint)
{
  if ( iReader )
    {
    if ( m_LSMReader.empty() )
      {
      m_LSMReader.push_back(iReader);
      }
    else
      {
      if ( iReader != m_LSMReader[0] )
        {
        m_LSMReader[0] = iReader;
        }
      }

    m_LSMReader[0]->Update();

    int dim[5];
    m_LSMReader[0]->GetDimensions(dim);

    int NumberOfChannels = m_LSMReader[0]->GetNumberOfChannels();

    // Initialize the widgets with the good number of channels
    // it will update the size of the related combobox
    m_NavigationDockWidget->SetNumberOfChannels(NumberOfChannels);
    m_ContourSegmentationDockWidget->SetNumberOfChannels(NumberOfChannels);
    m_MeshSegmentationDockWidget->SetNumberOfChannels(NumberOfChannels);

    if ( NumberOfChannels > 1 )
      {
      m_NavigationDockWidget->SetChannel(0);
      m_ContourSegmentationDockWidget->SetChannel(0);
      m_MeshSegmentationDockWidget->SetChannel(0);
      m_InternalImages.resize(NumberOfChannels);

      for ( int i = 1; i < NumberOfChannels; i++ )
        {
        m_NavigationDockWidget->SetChannel(i);
        m_ContourSegmentationDockWidget->SetChannel(i);
        m_MeshSegmentationDockWidget->SetChannel(i);

        m_LSMReader.push_back( vtkSmartPointer< vtkLSMReader >::New() );
        m_LSMReader.back()->SetFileName( m_LSMReader[0]->GetFileName() );
        m_LSMReader.back()->SetUpdateChannel(i);
        }
      }

    m_NavigationDockWidget->SetXMinimumAndMaximum(0, dim[0] - 1);
    m_NavigationDockWidget->SetXSlice( ( dim[0] - 1 ) / 2 );

    m_NavigationDockWidget->SetYMinimumAndMaximum(0, dim[1] - 1);
    m_NavigationDockWidget->SetYSlice( ( dim[1] - 1 ) / 2 );

    m_NavigationDockWidget->SetZMinimumAndMaximum(0, dim[2] - 1);
    m_NavigationDockWidget->SetZSlice( ( dim[2] - 1 ) / 2 );

    m_NavigationDockWidget->SetTMinimumAndMaximum(0, dim[3] - 1);
    m_NavigationDockWidget->SetTSlice(iTimePoint);
    if ( m_TCoord != iTimePoint )
      {
      SetTimePoint(iTimePoint);
      }

#if defined( ENABLEFFMPEG ) || defined( ENABLEAVI )
    m_VideoRecorderWidget->SetXMinAndMax(0, dim[0] - 1);
    m_VideoRecorderWidget->SetYMinAndMax(0, dim[1] - 1);
    m_VideoRecorderWidget->SetZMinAndMax(0, dim[2] - 1);
    m_VideoRecorderWidget->SetTMinAndMax(0, dim[3] - 1);
#endif /* ENABLEVIDEORECORD */
    }
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

  m_MegaCaptureReader->SetInput(m_FileList);
  m_MegaCaptureReader->SetMegaCaptureHeader(iHeader);
  m_MegaCaptureReader->SetFileType(m_FileType);
  m_MegaCaptureReader->SetTimeBased(true);
  m_MegaCaptureReader->SetTimePoint(iTimePoint);
  m_MegaCaptureReader->Update();

  unsigned int min_t = m_MegaCaptureReader->GetMinTimePoint();
  unsigned int max_t = m_MegaCaptureReader->GetMaxTimePoint();

  unsigned int min_ch = m_MegaCaptureReader->GetMinChannel();
  unsigned int max_ch = m_MegaCaptureReader->GetMaxChannel();

  unsigned int NumberOfChannels = max_ch - min_ch + 1;

  vtkImageData *temp = m_MegaCaptureReader->GetOutput(min_ch);

  int extent[6];
  temp->GetExtent(extent);

  // Initialize the widgets with the good number of channels
  // it will update the size of the related combobox
  m_NavigationDockWidget->SetNumberOfChannels(NumberOfChannels);
  m_ContourSegmentationDockWidget->SetNumberOfChannels(NumberOfChannels);
  m_MeshSegmentationDockWidget->SetNumberOfChannels(NumberOfChannels);

  // Set up QSpinBox in m_VideoRecorderWidget
  if ( NumberOfChannels > 1 )
    {
    m_NavigationDockWidget->SetChannel(0);
    m_ContourSegmentationDockWidget->SetChannel(0);
    m_MeshSegmentationDockWidget->SetChannel(0);
    m_InternalImages.resize(NumberOfChannels, NULL);

    for ( unsigned int i = 1; i < NumberOfChannels; i++ )
      {
      m_NavigationDockWidget->SetChannel(i);
      m_ContourSegmentationDockWidget->SetChannel(i);
      m_MeshSegmentationDockWidget->SetChannel(i);
      }
    }

  m_NavigationDockWidget->SetXMinimumAndMaximum(extent[0], extent[1]);
  m_NavigationDockWidget->SetXSlice( ( extent[0] + extent[1] ) / 2 );

  m_NavigationDockWidget->SetYMinimumAndMaximum(extent[2], extent[3]);
  m_NavigationDockWidget->SetYSlice( ( extent[2] + extent[3] ) / 2 );

  m_NavigationDockWidget->SetZMinimumAndMaximum(extent[4], extent[5]);
  m_NavigationDockWidget->SetZSlice( ( extent[4] + extent[5] ) / 2 );

  m_NavigationDockWidget->SetTMinimumAndMaximum(min_t, max_t);
  m_NavigationDockWidget->SetTSlice(iTimePoint);

  if ( static_cast< unsigned int >( m_TCoord ) != iTimePoint )
    {
    SetTimePoint(iTimePoint);
    }

  // Set up QSpinBox in m_VideoRecorderWidget
#if defined( ENABLEFFMPEG ) || defined( ENABLEAVI )
  m_VideoRecorderWidget->SetXMinAndMax(extent[0], extent[1]);
  m_VideoRecorderWidget->SetYMinAndMax(extent[2], extent[3]);
  m_VideoRecorderWidget->SetZMinAndMax(extent[4], extent[5]);
  m_VideoRecorderWidget->SetTMinAndMax(min_t, max_t);
#endif /* ENABLEVIDEORECORD */
}

//-------------------------------------------------------------------------

//#########################################################################
//#########################################################################

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetTimePointWithMegaCapture()
{
  m_MegaCaptureReader->SetTimePoint(m_TCoord);

  unsigned int min_ch = m_MegaCaptureReader->GetMinChannel();
  unsigned int max_ch = m_MegaCaptureReader->GetMaxChannel();

  unsigned int NumberOfChannels = max_ch - min_ch + 1;
  // delete the preivous pointers
  // and reassign it
  m_InternalImages.resize(NumberOfChannels, NULL);

  m_MegaCaptureReader->Update();

  if ( max_ch != min_ch )
    {
    vtkSmartPointer< vtkImageAppendComponents > append_filter =
      vtkSmartPointer< vtkImageAppendComponents >::New();

    for ( unsigned int i = min_ch; i <= max_ch; i++ )
      {
      vtkSmartPointer< vtkImageData > input = vtkSmartPointer< vtkImageData >::New();
      input->ShallowCopy(m_MegaCaptureReader->GetOutput(i));
      m_InternalImages[i] = input;
      append_filter->AddInput(m_InternalImages[i]);
      }
    // This is really stupid!!!
    if ( max_ch < 2 )
      {
      for ( unsigned int i = max_ch + 1; i < 3; i++ )
        {
        append_filter->AddInput(m_InternalImages[0]);
        }
      }
    append_filter->Update();

    if ( this->m_NavigationDockWidget->ShowAllChannels() )
      {
      m_Image->ShallowCopy( append_filter->GetOutput() );

      // LUT DISABLED
      m_ViewActions[11]->setEnabled(false);
      m_ViewActions[10]->setEnabled(false);
      }
    else
      {
      int ch = this->m_NavigationDockWidget->GetCurrentChannel();
      if ( ch != -1 )
        {
        m_Image->ShallowCopy(m_InternalImages[ch]);
        }
      // LUT ENABLED
      m_ViewActions[11]->setEnabled(true);
      m_ViewActions[10]->setEnabled(true);
      }
    }
  else
    {
    m_Image->ShallowCopy( m_MegaCaptureReader->GetOutput(min_ch) );
    m_Image->SetNumberOfScalarComponents(1);

    // LUT ENABLED
    m_ViewActions[11]->setEnabled(true);
    m_ViewActions[10]->setEnabled(true);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetTimePointWithMegaCaptureTimeChannels( int iChannel )
{
  int min_t = m_MegaCaptureReader->GetMinTimePoint();
  int max_t = m_MegaCaptureReader->GetMaxTimePoint();

  int t0 = m_TCoord - 1;
  int t1 = m_TCoord;
  int t2 = m_TCoord + 1;

  // special case if we are at the borders
  if(m_TCoord == min_t)
    {
    t0 = t2;
    }

  if(m_TCoord == max_t)
    {
    t2 = t0;
    }

  // resize internal image
  // clean the vector since is is a vector of smartpointers
  m_InternalImages.resize(3, NULL);

  vtkSmartPointer< vtkImageAppendComponents > append_filter =
    vtkSmartPointer< vtkImageAppendComponents >::New();

  vtkSmartPointer<vtkImageData> i0 = vtkSmartPointer<vtkImageData>::New();
  i0->ShallowCopy(m_MegaCaptureReader->GetImage(iChannel, t0));
  m_InternalImages[0] = i0;
  append_filter->AddInput(m_InternalImages[0]);

  vtkSmartPointer<vtkImageData> i1 = vtkSmartPointer<vtkImageData>::New();
  i1->ShallowCopy(m_MegaCaptureReader->GetImage(iChannel, t1));
  m_InternalImages[1] = i1;
  append_filter->AddInput(m_InternalImages[1]);

  vtkSmartPointer<vtkImageData> i2 = vtkSmartPointer<vtkImageData>::New();
  i2->ShallowCopy(m_MegaCaptureReader->GetImage(iChannel, t2));
  m_InternalImages[2] = i2;
  append_filter->AddInput(m_InternalImages[2]);

  append_filter->Update();

  if ( this->m_NavigationDockWidget->ShowAllChannels() )
    {
    m_Image->ShallowCopy( append_filter->GetOutput() );

    // LUT DISABLED
    m_ViewActions[11]->setEnabled(false);
    m_ViewActions[10]->setEnabled(false);
    }
  else
    {
    int ch = this->m_NavigationDockWidget->GetCurrentChannel();
    if ( ch != -1 )
      {
      m_Image->ShallowCopy(m_InternalImages[ch]);
      }
    // LUT ENABLED
    m_ViewActions[11]->setEnabled(true);
    m_ViewActions[10]->setEnabled(true);
    }

  // update channels in navigation DockWidget
  m_NavigationDockWidget->SetNumberOfChannels(3);
  m_NavigationDockWidget->blockSignals(true);
  m_NavigationDockWidget->SetChannel(0, "t-1");
  m_NavigationDockWidget->SetChannel(1, "t");
  m_NavigationDockWidget->SetChannel(2, "t+1");
  m_NavigationDockWidget->blockSignals(false);

  //update channels in segmentation widgets
  m_ContourSegmentationDockWidget->SetNumberOfChannels(3);
  m_ContourSegmentationDockWidget->SetChannel(0, "t-1");
  m_ContourSegmentationDockWidget->SetChannel(1, "t");
  m_ContourSegmentationDockWidget->SetChannel(2, "t+1");

  m_MeshSegmentationDockWidget->SetNumberOfChannels(3);
  m_MeshSegmentationDockWidget->SetChannel(0, "t-1");
  m_MeshSegmentationDockWidget->SetChannel(1, "t");
  m_MeshSegmentationDockWidget->SetChannel(2, "t+1");
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SetTimePointWithLSMReaders()
{
  m_LSMReader[0]->SetUpdateTimePoint(m_TCoord);

  int NumberOfChannels = m_LSMReader[0]->GetNumberOfChannels();

  if ( NumberOfChannels > 1 )
    {
    m_InternalImages[0] = m_LSMReader[0]->GetOutput();

    vtkSmartPointer< vtkImageAppendComponents > append_filter =
      vtkSmartPointer< vtkImageAppendComponents >::New();
    append_filter->AddInput(m_InternalImages[0]);

    for ( int i = 1; i < NumberOfChannels; i++ )
      {
      m_LSMReader[i]->SetUpdateTimePoint(m_TCoord);
      m_LSMReader[i]->Update();

      m_InternalImages[i] = m_LSMReader[i]->GetOutput();
      append_filter->AddInput(m_InternalImages[i]);
      }
    // This is really stupid!!!
    if ( NumberOfChannels < 3 )
      {
      for ( int i = NumberOfChannels; i < 3; i++ )
        {
        append_filter->AddInput(m_InternalImages[0]);
        }
      }
    append_filter->Update();

    m_Image->ShallowCopy( append_filter->GetOutput() );
    }
  else
    {
    m_LSMReader[0]->Update();

    m_Image->ShallowCopy( m_LSMReader[0]->GetOutput() );
    }
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

  if ( !m_LSMReader.empty() )
    {
    if ( iTimePoint >= m_LSMReader[0]->GetNumberOfTimePoints() )
      {
      return;
      }
    else
      {
      m_TCoord = iTimePoint;
      SetTimePointWithLSMReaders();
      emit TimePointChanged(m_TCoord);
      }
    }
  else
    {
    if ( !m_FileList.empty() )
      {
      unsigned int t = static_cast< unsigned int >( iTimePoint );
      if ( ( t < m_MegaCaptureReader->GetMinTimePoint() )
           || ( t > m_MegaCaptureReader->GetMaxTimePoint() ) )
        {
        return;
        }
      else
        {
        m_TCoord = iTimePoint;
        if(m_ChannelClassicMode)
          {
          //qDebug() << "CLASSIC mode";
          SetTimePointWithMegaCapture();
          }
        else
          {
          //qDebug() << "TRACK mode";
         // qDebug() << "CHANNEL: " << m_ChannelOfInterest;
          SetTimePointWithMegaCaptureTimeChannels( m_ChannelOfInterest );
          }
        emit TimePointChanged(m_TCoord);
        }
      }
    else
      {
      // no lsm reader, no file list. did you really provide any input?
      qWarning() << "No lsm reader. No file list";
      }
    }

  this->m_ContourContainer->ShowActorsWithGivenTimePoint(m_TCoord);
  this->m_MeshContainer->ShowActorsWithGivenTimePoint(m_TCoord);
  this->m_TrackContainer->ShowActorsWithGivenTimePoint(m_TCoord);

  Update();

  QApplication::restoreOverrideCursor();
}

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::Update()
{
  m_ImageView->SetImage(m_Image);
  m_ImageView->Update();
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::ChangeLookupTable()
{
  vtkImageData *image = m_ImageView->GetImage();

  if ( image->GetNumberOfScalarComponents() == 1 )
    {
    vtkLookupTable *lut = QGoLUTDialog::GetLookupTable( this,
                                                        tr("Choose one look-up table") );
    m_ImageView->SetLookupTable(lut);

    // free memory since it is not freed in the QGoLUTDialog
    lut->Delete();
    }
}

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
  double r, g, b;

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
  double r, g, b;

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
QGoTabImageView3DwT::ShowAllChannels(bool iChecked)
{
  if ( iChecked )
    {
    vtkSmartPointer< vtkImageAppendComponents > append_filter =
      vtkSmartPointer< vtkImageAppendComponents >::New();

    for ( unsigned int i = 0; i < m_InternalImages.size(); i++ )
      {
      append_filter->AddInput(m_InternalImages[i]);
      }

    // This is really stupid!!!
    if ( m_InternalImages.size() < 3 )
      {
      for ( size_t i = m_InternalImages.size(); i < 3; i++ )
        {
        append_filter->AddInput(m_InternalImages[0]);
        }
      }

    append_filter->Update();

    m_Image->ShallowCopy( append_filter->GetOutput() );
    Update();

    // Update LUT
    m_ViewActions[11]->setEnabled(false);
    m_ViewActions[10]->setEnabled(false);
    }
  else
    {
    int ch = this->m_NavigationDockWidget->GetCurrentChannel();
    if ( ch != -1 )
      {
      m_Image->ShallowCopy(m_InternalImages[ch]);
      Update();
      }

    // Update LUT
    m_ViewActions[11]->setEnabled(true);
    m_ViewActions[10]->setEnabled(true);

    // show the scalarbar automatically if the button is checked
    bool showScalarBar = m_ViewActions[11]->isChecked();
    m_ImageView->ShowScalarBar(showScalarBar);
    }
}

//------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::ShowOneChannel(int iChannel)
{
  if ( ( iChannel != -1 ) && ( !m_InternalImages.empty() ) )
    {
    // Update lut
    m_ViewActions[11]->setEnabled(true);
    m_ViewActions[10]->setEnabled(true);

    m_Image->ShallowCopy(m_InternalImages[iChannel]);
    Update();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SaveContour(vtkPolyData *contour, vtkPolyData *contour_nodes)
{
  if ( ( contour->GetNumberOfPoints() > 2 ) && ( m_TCoord >= 0 ) )
    {
    // Compute Bounding Box
    std::vector< int > bounds = GetBoundingBox(contour);
    // Save contour in database :
    this->m_DataBaseTables->SaveContoursFromVisuInDB(bounds[0],
                                                     bounds[2],
                                                     bounds[4],
                                                     m_TCoord,
                                                     bounds[1],
                                                     bounds[3],
                                                     bounds[5],
                                                     contour_nodes);
    }
  //else
  //  {
  //  std::cerr << "(contour->GetNumberOfPoints() < 2) or  (m_TCoord < 0)" <<
  // std::endl;
  //  }
  //useful ??
  //return ContourData;
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
    for ( i = 0; i < 3; i++ )
      {
      Min[i] = bounds[k++];
      Max[i] = bounds[k++];
      }

    int *min_idx = this->GetImageCoordinatesFromWorldCoordinates(Min);
    int *max_idx = this->GetImageCoordinatesFromWorldCoordinates(Max);

    boundingBox[0] = min_idx[0];
    boundingBox[1] = max_idx[0];

    boundingBox[2] = min_idx[1];
    boundingBox[3] = max_idx[1];

    boundingBox[4] = min_idx[2];
    boundingBox[5] = max_idx[2];

    delete[] min_idx;
    delete[] max_idx;
    }
  return boundingBox;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< vtkActor * > QGoTabImageView3DwT::VisualizeContour(vtkPolyData *contour)
{
  std::vector< vtkActor * > oActors;

  if ( contour->GetNumberOfPoints() > 2 )
    {
    double *RGBA = this->m_ContourContainer->m_CurrentElement.rgba;

    vtkProperty *contour_property = vtkProperty::New();
    contour_property->SetColor(RGBA[0], RGBA[1], RGBA[2]);
    contour_property->SetOpacity(RGBA[3]);

    vtkPolyData *contour_copy = vtkPolyData::New();
    contour_copy->DeepCopy(contour);

    oActors =
      this->AddContour(contour_copy, contour_property);

    contour_copy->Delete();
    contour_property->Delete();
    }

  m_ImageView->UpdateRenderWindows();

  return oActors;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< vtkActor * >
QGoTabImageView3DwT::VisualizeMesh(vtkPolyData *iMesh)
{
  std::vector< vtkActor * > oActors;

  double *RGBA = this->m_MeshContainer->m_CurrentElement.rgba;

  vtkProperty *mesh_property = vtkProperty::New();
  mesh_property->SetColor(RGBA[0], RGBA[1], RGBA[2]);
  mesh_property->SetOpacity(RGBA[3]);

  /// \todo shallow copy...?
  // get corresponding actor from visualization
  vtkPolyData *mesh_copy = vtkPolyData::New();
  mesh_copy->DeepCopy(iMesh);

  oActors = this->AddContour(mesh_copy, mesh_property);

  mesh_copy->Delete();
  mesh_property->Delete();

  m_ImageView->UpdateRenderWindows();

  return oActors;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< vtkActor * >
QGoTabImageView3DwT::
VisualizeTrack(vtkPolyData *iMesh)
{
  double *RGBA = this->m_TrackContainer->m_CurrentElement.rgba;
  std::vector< vtkActor * > mesh_actor;

  if ( iMesh )
    {
    //bool visibility = true;

    vtkProperty *mesh_property = vtkProperty::New();
    mesh_property->SetColor(RGBA[0], RGBA[1], RGBA[2]);
    mesh_property->SetOpacity(RGBA[3]);

    /// \todo fix bug, shouldn't be required

    mesh_actor.resize(4);
    mesh_actor = this->AddContour(iMesh, mesh_property);

    mesh_property->Delete();

    /*m_TrackContainer->UpdateVisualizationForGivenElement< TIndex >(iIt,
                                                                  mesh_actor,
                                                                  false,
                                                                  visibility);
    */
    }

  return mesh_actor;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::ValidateContour()
{
  bool re_edit = m_ContourSegmentationDockWidget->GetReeditMode();

  for ( int i = 0; i < m_ImageView->GetNumberOfImageViewers(); i++ )
    {
    vtkPolyData *nodes = m_ImageView->GetContourRepresentationNodePolydata(i);

    if ( nodes->GetNumberOfPoints() > 2 )
      {
      /// \todo Fix leaks generated by
      // m_ImageView->GetContourRepresentationAsPolydata(i)
      vtkPolyData *contour = vtkPolyData::New();
      contour->DeepCopy( m_ImageView->GetContourRepresentationAsPolydata(i) );

      //polydata for bounding box, node for points
      SaveContour(contour, nodes);

      // polydata
      std::vector< vtkActor * > actors = VisualizeContour(contour);
      contour->Delete();

      //nodes
      m_ContourContainer->UpdateCurrentElementFromVisu(actors,
                                                       nodes,
                                                       m_TCoord,
                                                       re_edit, //highlighted
                                                       true);   //visible

      m_ContourContainer->InsertCurrentElement();
      }
    }
  /** \todo useful ?? */
  if ( re_edit )
    {
    this->m_DataBaseTables->GetTraceManualEditingDockWidget()->setEnabled(true);
    m_ContourSegmentationDockWidget->SetReeditMode(false);
    m_ImageView->ReinitializeContourWidget();
    m_ContourSegmentationDockWidget->hide();
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

      this->m_ModeActions[0]->setChecked(true);

      m_ImageView->InitializeContourWidgetNodes(dir, nodes);

      this->m_ContourSegmentationDockWidget->show();
      this->m_ContourSegmentationDockWidget->SegmentationMethod(0);
      this->m_ContourSegmentationDockWidget->SetReeditMode(true);
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::HighlightXY()
{
  vtkActor *temp_actor = m_ImageView->GetCurrentActor();

  if ( temp_actor )
    {
    // CODE COMMENTED AS REMINDER
    // FIND A SOLUTION TO AVOID CHECKING THE 2 CONTAINERS
    // FOR VISIBILITY AND HIGHLIGHT
    /*vtkPolyData* pd =
        dynamic_cast< vtkPolyData* >( temp_actor->GetMapper()->GetInput() );

    if( pd )
      {
      if( ContourMeshContainer::ComputeDirectionFromContour( pd ) != -1 )
        {*/
    m_ContourContainer->UpdateElementHighlightingWithGivenActor< ActorXY >(
      temp_actor);
    /* }
   else
     {*/
    m_MeshContainer->UpdateElementHighlightingWithGivenActor< ActorXY >(
      temp_actor);
    /*}
  }*/
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::HighlightXZ()
{
  vtkActor *temp_actor = m_ImageView->GetCurrentActor();

  if ( temp_actor )
    {
    m_ContourContainer->UpdateElementHighlightingWithGivenActor< ActorXZ >(
      temp_actor);
    m_MeshContainer->UpdateElementHighlightingWithGivenActor< ActorXZ >(
      temp_actor);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::HighlightYZ()
{
  vtkActor *temp_actor = m_ImageView->GetCurrentActor();

  if ( temp_actor )
    {
    m_ContourContainer->UpdateElementHighlightingWithGivenActor< ActorYZ >(
      temp_actor);
    m_MeshContainer->UpdateElementHighlightingWithGivenActor< ActorYZ >(
      temp_actor);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::HighlightXYZ()
{
  vtkActor *temp_actor = m_ImageView->GetCurrentActor();

  if ( temp_actor )
    {
    m_ContourContainer->UpdateElementHighlightingWithGivenActor< ActorXYZ >(
      temp_actor);
    m_MeshContainer->UpdateElementHighlightingWithGivenActor< ActorXYZ >(
      temp_actor);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::VisibilityXYZ()
{
  vtkActor *temp_actor = m_ImageView->GetCurrentActor();

  if ( temp_actor )
    {
    m_ContourContainer->UpdateElementVisibilityWithGivenActor< ActorXYZ >(
      temp_actor,
      m_ImageView->GetCurrentState()
      );
    m_MeshContainer->UpdateElementVisibilityWithGivenActor< ActorXYZ >(
      temp_actor,
      m_ImageView->GetCurrentState() );
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
QGoTabImageView3DwT::SaveAndVisuContour(vtkPolyData *iView)
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
  SaveContour(iView, contour_nodes);

  // should be polydata
  std::vector< vtkActor * > actors = VisualizeContour(iView);
  iView->Delete();

  // should be nodes
  // update the container
  m_ContourContainer->UpdateCurrentElementFromVisu(actors,
                                                   contour_nodes,
                                                   m_TCoord,
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
QGoTabImageView3DwT::SaveMesh(vtkPolyData *iView)
{
  // Compute Bounding Box
  std::vector< int > bounds = this->GetBoundingBox(iView);

  // Save mesh in database
  GoFigureMeshAttributes MeshAttributes = ComputeMeshAttributes(iView, true );

  this->m_DataBaseTables->SaveMeshFromVisuInDB(bounds[0], bounds[2], bounds[4],
                                               bounds[1], bounds[3], bounds[5],
                                               iView,
                                               &MeshAttributes);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SaveAndVisuMesh(vtkPolyData *iView)
{
  SaveAndVisuMesh(iView, m_TCoord);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::SaveAndVisuMesh(vtkPolyData *iView, unsigned int iTCoord)
{
  if ( !m_DataBaseTables->IsDatabaseUsed() )
    {
    std::cerr << "Problem with DB" << std::endl;
    return;
    }

  if ( !iView )
    {
    std::cerr << "Input Mesh is NULL" << std::endl;
    return;
    }

  SaveMesh(iView);

  std::vector< vtkActor * > actors =  VisualizeMesh(iView);

  // update container since a new mesh is created
  m_MeshContainer->UpdateCurrentElementFromVisu(actors,
                                                iView,
                                                iTCoord,
                                                false,  // highlighted
                                                true);  // visible
  m_MeshContainer->InsertCurrentElement();

  // UPDATE THE TRACKS IN THE CONTAINER

  // get the center of the mesh
  // pointer to double is deleted in AddPointToCurrentElement
  //double * point = new double[4];
  //std::vector< int > boundingBox = GetBoundingBox(iView);
  //for(int i = 0; i<3; ++i)
  //  {
  //  point[i] = (boundingBox[2*i] + boundingBox[2*i+1])/2;
  //  }
  //point[3] = iTCoord;

  // Clean the actors (remove from visu + delete)
  // Update the track polydata with the new center
  //m_TrackContainer->AddPointToCurrentElement( point );
  //vtkPolyData* track = m_TrackContainer->GetCurrentElementNodes();
  // Create new actors and visu it if there is more than one point
  //if(track->GetNumberOfPoints() > 1)
  //  {
  //  std::vector< vtkActor * > trackActors =  VisualizeTrack(track);
    // Add new actors in the container
  //  m_TrackContainer->UpdateCurrentElementActorsFromVisu(trackActors);
  //  }
 // m_TrackContainer->InsertCurrentElement();
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
    std::vector< vtkActor * > actors = VisualizeContour(iInput);
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
void QGoTabImageView3DwT::ShowTraceDockWidgetForContour(
  bool ManualSegVisible)
{
  if ( ManualSegVisible )
    {
    if ( this->m_DataBaseTables->IsDatabaseUsed() )
      {
      this->m_DataBaseTables->UpdateWidgetsForCorrespondingTrace("contour", "mesh");
      }
    }
  else
    {
    if ( m_DataBaseTables->toggleViewAction()->isChecked() )
      {
      // do nothing
      }
    else
      {
      //
      m_DataBaseTables->GetTraceManualEditingDockWidget()->hide();
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::ShowTraceDockWidgetForMesh(
  bool MeshSegmentationVisible)
{
  if ( MeshSegmentationVisible )
    {
    if ( this->m_DataBaseTables->IsDatabaseUsed() )
      {
      this->m_DataBaseTables->UpdateWidgetsForCorrespondingTrace("mesh", "track");
      }
    }
  else
    {
    if ( m_DataBaseTables->toggleViewAction()->isChecked() )
      {
      // do nothing
      }
    else
      {
      //
      m_DataBaseTables->GetTraceManualEditingDockWidget()->hide();
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::GoToDefaultMenu(bool iEnable)
{
  if ( iEnable )
    {
    this->m_ModeActions.at(0)->setChecked(true);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoFigureMeshAttributes
QGoTabImageView3DwT::ComputeMeshAttributes( vtkPolyData *iMesh,
                                            const bool& iIntensity)
{
  typedef unsigned char PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;

  itk::vtkPolyDataToGoFigureMeshAttributes< ImageType >::Pointer
    calculator = itk::vtkPolyDataToGoFigureMeshAttributes< ImageType >::New();
  calculator->SetPolyData(iMesh);
  calculator->SetIntensityBasedComputation( iIntensity );

  GoFigureMeshAttributes oAttributes;

  if( iIntensity )
    {
    for ( size_t i = 0; i < m_InternalImages.size(); i++ )
      {
      vtkSmartPointer< vtkImageExport > vtk_exporter =
        vtkSmartPointer< vtkImageExport >::New();
      itk::VTKImageImport< ImageType >::Pointer itk_importer =
        itk::VTKImageImport< ImageType >::New();
      vtk_exporter->SetInput(m_InternalImages[i]);

      ConnectPipelines< vtkImageExport, itk::VTKImageImport< ImageType >::Pointer >(
        vtk_exporter, itk_importer);
      calculator->SetImage( itk_importer->GetOutput() );
      calculator->Update();

      QString     q_channelname = this->m_NavigationDockWidget->GetChannelName(i);
      std::string channelname = q_channelname.toStdString();

      oAttributes.m_TotalIntensityMap[channelname] =
        static_cast< int >( calculator->GetSumIntensity() );
      oAttributes.m_MeanIntensityMap[channelname] = calculator->GetMeanIntensity();
      oAttributes.m_Volume = calculator->GetPhysicalSize();
      oAttributes.m_Area = calculator->GetArea();
      oAttributes.m_Size = calculator->GetSize();
      }
    }
  else
    {
    vtkSmartPointer< vtkImageExport > vtk_exporter =
      vtkSmartPointer< vtkImageExport >::New();
    itk::VTKImageImport< ImageType >::Pointer itk_importer =
      itk::VTKImageImport< ImageType >::New();
    vtk_exporter->SetInput(m_InternalImages[0]);

    ConnectPipelines< vtkImageExport, itk::VTKImageImport< ImageType >::Pointer >(
      vtk_exporter, itk_importer);
    calculator->SetImage( itk_importer->GetOutput() );
    calculator->Update();

    oAttributes.m_Volume = calculator->GetPhysicalSize();
    oAttributes.m_Area = calculator->GetArea();
    oAttributes.m_Size = calculator->GetSize();
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
  std::list< unsigned int >::iterator contourid_it = iListContourIDs.begin();

  std::vector< vtkPolyData * > list_contours;

  // get the time point
  int tcoord = this->m_TCoord;

  while ( contourid_it != iListContourIDs.end() )
    {
    ContourMeshContainer::MultiIndexContainerTraceIDIterator
      traceid_it = m_ContourContainer->m_Container.get< TraceID >().find(*contourid_it);

    if ( traceid_it != m_ContourContainer->m_Container.get< TraceID >().end() )
      {
      tcoord = traceid_it->TCoord;

      list_contours.push_back(
        vtkPolyData::SafeDownCast(
          traceid_it->ActorXYZ->GetMapper()->GetInput() ) );
      }
    ++contourid_it;
    }

  if ( !list_contours.empty() )
    {
    // make the mesh from the list of contours
    typedef itk::ContourToMeshFilter< std::vector< vtkPolyData * > > FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->ProcessContours(list_contours);

    //as the element is already in the container we need to delete it in order
    //to update it in the SaveAndVisuMesh:
    this->m_MeshContainer->RemoveElementFromVisualizationWithGivenTraceID(iMeshID);
    SaveAndVisuMesh(filter->GetOutput(), tcoord);
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
void
QGoTabImageView3DwT::GoToLocation(int iX, int iY, int iZ, int iT)
{
  this->SetTimePoint(iT);
  this->SetSliceViewXY(iZ);
  this->SetSliceViewXZ(iY);
  this->SetSliceViewYZ(iX);
}

//-------------------------------------------------------------------------
