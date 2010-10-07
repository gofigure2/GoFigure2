/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

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

#include "QGoImageView3D.h"
#include "QGoLUTDialog.h"
#include "QGoNavigationDockWidget.h"
#include "GoDBCoordinateRow.h"
#include "GoDBMeshRow.h"

#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)

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
#include "vtkPolyDataMySQLTextReader.h"
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

#include <set>

// base segmentation dock widgets
#include "QGoContourSegmentationBaseDockWidget.h"
#include "QGoMeshSegmentationBaseDockWidget.h"

// TESTS
#include "vtkPolyDataWriter.h"
//-------------------------------------------------------------------------
QGoTabImageView3DwT::
QGoTabImageView3DwT(QWidget* iParent) :
  QGoTabElementBase(iParent),
  m_LSMReader(0),
  m_Image(0),
  m_BackgroundColor(Qt::black),
  m_PCoord( 0 ),
  m_RCoord( 0 ),
  m_CCoord( 0 ),
  m_XTileCoord( 0 ),
  m_YTileCoord( 0 ),
  m_ZTileCoord( 0 ),
  m_TCoord(-1),
  //m_ContourId(0),
  m_TraceWidgetRequiered(false)
  {
  m_Image = vtkImageData::New();
  m_Seeds = vtkPoints::New();

  m_HighlightedContoursProperty = vtkProperty::New();
  m_HighlightedContoursProperty->SetColor( 1., 0., 0. );
  m_HighlightedContoursProperty->SetOpacity( 1. );
  m_HighlightedContoursProperty->SetLineWidth( 3. );

  m_HighlightedMeshesProperty = vtkProperty::New();
  m_HighlightedMeshesProperty->SetColor( 1., 0., 0. );
  m_HighlightedMeshesProperty->SetOpacity( 1. );
  m_HighlightedMeshesProperty->SetSpecular( 0.6 );
  m_HighlightedMeshesProperty->SetLineWidth( 3. );
  m_HighlightedMeshesProperty->SetInterpolationToPhong();

  setupUi(this);

  m_MegaCaptureReader = itk::MegaCaptureReader::New();

  m_ContourContainer = new ContourMeshContainer( this, this->m_ImageView );
  m_ContourContainer->SetHighlightedProperty( m_HighlightedContoursProperty );

  m_MeshContainer = new ContourMeshContainer( this, this->m_ImageView );
  m_MeshContainer->SetHighlightedProperty( m_HighlightedMeshesProperty );

  m_TrackContainer = new ContourMeshContainer( this, this->m_ImageView );

  CreateVisuDockWidget();

  // segmentation dockwidgets
  CreateContourSegmentationDockWidget();
  CreateMeshSegmentationDockWidget();

  CreateDataBaseTablesConnection();

#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)
  CreateVideoRecorderWidget();
#endif

  CreateAllViewActions();

  CreateToolsActions();

  CreateBookmarkActions();

  CreateModeActions();

  ReadSettings();

  m_DockWidgetList.push_back(
    std::pair<QGoDockWidgetStatus*, QDockWidget*>(
      new QGoDockWidgetStatus(
          m_NavigationDockWidget, Qt::RightDockWidgetArea, true, true),
      m_NavigationDockWidget));

  m_DockWidgetList.push_back(
    std::pair<QGoDockWidgetStatus*, QDockWidget*>(
      new QGoDockWidgetStatus(
          m_ContourSegmentationDockWidget, Qt::LeftDockWidgetArea, true, true),
      m_ContourSegmentationDockWidget));

  m_DockWidgetList.push_back(
    std::pair<QGoDockWidgetStatus*, QDockWidget*>(
      new QGoDockWidgetStatus(
          m_MeshSegmentationDockWidget, Qt::LeftDockWidgetArea, true, true),
      m_MeshSegmentationDockWidget));

//   m_DockWidgetList.push_back(
//     std::pair< QGoDockWidgetStatus*, QDockWidget* >(
//       new QGoDockWidgetStatus( m_DataBaseTables, Qt::TopDockWidgetArea, true, true ),
//       m_DataBaseTables ) );

  m_DockWidgetList.push_back(
    std::pair<QGoDockWidgetStatus*, QDockWidget*>(
      new QGoDockWidgetStatus(this->m_DataBaseTables->GetTraceManualEditingDockWidget(),
      Qt::LeftDockWidgetArea, true, true),
      this->m_DataBaseTables->GetTraceManualEditingDockWidget()));

#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)
  m_DockWidgetList.push_back(
    std::pair<QGoDockWidgetStatus*, QDockWidget*>(
      new QGoDockWidgetStatus(m_VideoRecorderWidget, Qt::LeftDockWidgetArea, true, true),
      m_VideoRecorderWidget));
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

  if (!m_LSMReader.empty())
    {
    if (m_LSMReader[0])
      {
      m_LSMReader[0]->Delete();
      }
    }

  unsigned int minch = m_MegaCaptureReader->GetMinChannel();
  unsigned int maxch = m_MegaCaptureReader->GetMaxChannel();

  for (unsigned int i = minch; i < maxch+1; i++)
    {
    vtkImageData* temp = m_MegaCaptureReader->GetOutput(i);
    if (temp)
      {
      temp->Delete();
      temp = NULL;
      }
    }

  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
UpdateSeeds()
{
  // LEAKS HERE...
  // delete first
  //m_Seeds->Delete();
 m_Seeds->DeepCopy(m_ImageView->GetAllSeeds());
 m_ImageView->GetAllSeeds()->Delete();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateContourSegmentationDockWidget()
{
  //----------------------------------------------------------------
  // Create base  contour segmentation dock widget:
  // basic interactor connections
  //----------------------------------------------------------------

  m_ContourSegmentationDockWidget
      = new QGoContourSegmentationBaseDockWidget(this, m_Seeds, &m_InternalImages);

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL(ReinitializeInteractorActivated(bool)),
                    this,
                    SLOT(DefaultInteractorBehavior(bool)) );

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL(ManualSegmentationActivated(bool)),
                    this,
                    SLOT(ManualInteractorBehavior(bool)) );

  QObject::connect(m_ContourSegmentationDockWidget,
                   SIGNAL(SemiAutoSegmentationActivated(bool)),
                   this,
                   SLOT(SeedInteractorBehavior(bool)));

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL(AutoSegmentationActivated(bool)),
                    this,
                    SLOT(DefaultInteractorBehavior(bool)) );

  // signals from the manual segmentation
  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL(ValidateContour()),
                    this, SLOT(ValidateContour()) );

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL(ReinitializeContourWidget()),
                    m_ImageView,
                    SLOT(ReinitializeContourWidget()) );

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL(UpdateContourRepresentationProperties(float, QColor,
                                                                 QColor, QColor)),
                    m_ImageView,
                    SLOT(UpdateContourRepresentationProperties(float, QColor,
                                                               QColor, QColor)));

  // signals for the semi automated segmentation
  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL(UpdateSeeds()),
                    this,
                    SLOT(UpdateSeeds()) );

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL(SaveAndVisuContour(vtkPolyData*)),
                    this,
                    SLOT(SaveAndVisuContour(vtkPolyData*)));

  QObject::connect( m_ContourSegmentationDockWidget,
                    SIGNAL(ClearAllSeeds()),
                    m_ImageView,
                    SLOT(ClearAllSeeds()) );

  // signals for the automatic segmentation
  // ...

  // intialize the widget
  m_ContourSegmentationDockWidget->Initialize();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateMeshSegmentationDockWidget()
{
  //----------------------------------------------------------------
  // Create base  contour segmentation dock widget:
  // basic interactor connections
  //----------------------------------------------------------------

  m_MeshSegmentationDockWidget
      = new QGoMeshSegmentationBaseDockWidget(this, m_Seeds, &m_InternalImages);

  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL(ReinitializeInteractorActivated(bool)),
                    this,
                    SLOT(DefaultInteractorBehavior(bool)) );

  // we need seeds for the manual mesh segmentation
  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL(ManualSegmentationActivated(bool)),
                    this,
                    SLOT(SeedInteractorBehavior(bool)));

  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL(SemiAutoSegmentationActivated(bool)),
                    this,
                    SLOT(SeedInteractorBehavior(bool)));

  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL(AutoSegmentationActivated(bool)),
                    this,
                    SLOT(DefaultInteractorBehavior(bool)));

  // signals for manual segmentation
  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL(CreateEmptyMesh()),
                    this,
                    SLOT(CreateEmptyMesh()));

  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL(AddContourToCurrentMesh(vtkPolyData* )),
                    this,
                    SLOT(AddContourToCurrentMesh(vtkPolyData* )));

  // signals for the semi automatic segmentation
  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL(UpdateSeeds()),
                    this,
                    SLOT(UpdateSeeds()));

  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL(SaveAndVisuMesh(vtkPolyData*)),
                    this,
                    SLOT(SaveAndVisuMesh(vtkPolyData*)));

  QObject::connect( m_MeshSegmentationDockWidget,
                    SIGNAL(ClearAllSeeds()),
                    m_ImageView,
                    SLOT(ClearAllSeeds()));

  // intialize the widget
  m_MeshSegmentationDockWidget->Initialize();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ManualInteractorBehavior(bool iVisible)
{
  this->m_ImageView->EnableContourWidget(iVisible);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SeedInteractorBehavior(bool iVisible)
{
  this->m_ImageView->EnableSeedWidget(iVisible);
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
DefaultInteractorBehavior(bool iVisible)
{
  if (iVisible)
    {
    this->m_ImageView->DefaultMode();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ZoomInteractorBehavior(bool iVisible)
{
  if (iVisible)
    {
    this->m_ImageView->ZoomMode();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
PanInteractorBehavior(bool iVisible)
{
  if (iVisible)
    {
    this->m_ImageView->PanMode();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ActorPickingInteractorBehavior(bool iVisible)
{
  if (iVisible)
    {
    this->m_ImageView->ActorPickingMode();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
DistanceWidgetInteractorBehavior(bool iActive)
{
  this->m_ImageView->EnableDistanceWidget(iActive);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AngleWidgetInteractorBehavior(bool iActive)
{
  this->m_ImageView->EnableAngleWidget(iActive);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
Box3DPicking(bool iActive)
{
  this->m_ImageView->EnableBoxWidget(iActive);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
PlaneWidgetInteractorBehavior(bool iActive)
{
  this->m_ImageView->EnablePlaneWidget(iActive);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateVisuDockWidget()
{
  /**
   * \todo update annotation on current slice (i.e. slice x/23) when moving through
   * x via this dockwidget
   */

  m_NavigationDockWidget =
      new QGoNavigationDockWidget(this, GoFigure::THREE_D_WITH_T );

  QObject::connect(m_NavigationDockWidget, SIGNAL(XSliceChanged(int)),
                   this, SLOT(SetSliceViewYZ(int)));

  QObject::connect(this, SIGNAL(SliceViewYZChanged(int)),
                   m_NavigationDockWidget, SLOT(SetXSlice(int)));

  QObject::connect(m_NavigationDockWidget, SIGNAL(YSliceChanged(int)),
                   this, SLOT(SetSliceViewXZ(int)));

  QObject::connect(this, SIGNAL(SliceViewXZChanged(int)),
                   m_NavigationDockWidget, SLOT(SetYSlice(int)));

  QObject::connect(m_NavigationDockWidget, SIGNAL(ZSliceChanged(int)),
                   this, SLOT(SetSliceViewXY(int)));

  QObject::connect(this, SIGNAL(SliceViewXYChanged(int)),
                   m_NavigationDockWidget, SLOT(SetZSlice(int)));

  QObject::connect(m_NavigationDockWidget, SIGNAL(TSliceChanged(int)),
                   this, SLOT(SetTimePoint(int)));

  QObject::connect(this, SIGNAL(TimePointChanged(int)),
                   m_NavigationDockWidget, SLOT(SetTSlice(int)));

  QObject::connect(m_NavigationDockWidget, SIGNAL(ShowAllChannelsChanged(bool)),
                   this, SLOT(ShowAllChannels(bool)));

  QObject::connect(m_NavigationDockWidget, SIGNAL(ShowOneChannelChanged(int)),
                   this, SLOT(ShowOneChannel(int)));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateDataBaseTablesConnection()
{
  QObject::connect (this->m_DataBaseTables,
                    SIGNAL(DBVariablesSet()),
                    this,
                    SLOT(SetTheContainersForDB()));

  QObject::connect(this,
                   SIGNAL(TimePointChanged(int)),
                   this->m_DataBaseTables,
                   SLOT(UpdateSelectedTimePoint(int)));

  QObject::connect(this->m_DataBaseTables,
                   SIGNAL(TraceToReEdit(unsigned int)),
                   this, SLOT(ReEditContour(unsigned int)));

  QObject::connect(this->m_DataBaseTables,
                   SIGNAL(NewMeshToGenerate(std::list<unsigned int>,int)),
                   this,
                   SLOT(CreateMeshFromSelectedContours(std::list<unsigned int>,int)));

  QObject::connect(this->m_DataBaseTables, SIGNAL(NeedToGoToTheLocation(int, int, int, int)),
                   this, SLOT(GoToLocation(int, int, int, int)));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CloseTabRequest(bool iTable)
{
  if(iTable)
    {
    m_DataBaseTables->GetTraceManualEditingDockWidget()->show();
    return;
    }

  if(!m_TraceWidgetRequiered)
    {
    m_DataBaseTables->GetTraceManualEditingDockWidget()->hide();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
RequieresTraceWidget(bool iTable)
{
  m_TraceWidgetRequiered = iTable;
}
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetTheContainersForDB()
{
  m_DataBaseTables->SetContoursContainer( m_ContourContainer );
  m_DataBaseTables->SetMeshesContainer( m_MeshContainer );
  m_DataBaseTables->SetTracksContainer( m_TrackContainer );
}
//-------------------------------------------------------------------------
#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)
//-------------------------------------------------------------------------

void
QGoTabImageView3DwT::
CreateVideoRecorderWidget()
{
  m_VideoRecorderWidget = new QGoVideoRecorder(this);

  QObject::connect(this, SIGNAL(FullScreenViewChanged(int)),
                   this, SLOT(SetRendererWindow(int)));

  QObject::connect(m_VideoRecorderWidget, SIGNAL(XSliceChanged(int)),
                   this, SLOT(SetSliceViewYZ(int)));
  QObject::connect(m_VideoRecorderWidget, SIGNAL(YSliceChanged(int)),
                   this, SLOT(SetSliceViewXZ(int)));
  QObject::connect(m_VideoRecorderWidget, SIGNAL(ZSliceChanged(int)),
                   this, SLOT(SetSliceViewXY(int)));
  QObject::connect(m_VideoRecorderWidget, SIGNAL(TSliceChanged(int)),
                   this, SLOT(SetTimePoint(int)));

  QObject::connect(m_VideoRecorderWidget, SIGNAL(GetSliceView()),
                   this, SLOT(SetSliceView()));
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetRendererWindow(int iValue)
{
  if (iValue >= 1)
    {
    m_VideoRecorderWidget->SetRenderingWindow(
      m_ImageView->GetInteractor(iValue - 1)->GetRenderWindow());
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
QGoTabImageView3DwT::
CreateAllViewActions()
{
  QActionGroup* group = new QActionGroup(this);

  QAction* QuadViewAction = new QAction(tr("Quad-View"), this);
  QuadViewAction->setCheckable(true);
  QuadViewAction->setChecked(true);

  QIcon quadviewicon;
  quadviewicon.addPixmap(QPixmap(QString::fromUtf8(":/fig/4views.png")),
                         QIcon::Normal, QIcon::Off);
  QuadViewAction->setIcon(quadviewicon);

  group->addAction(QuadViewAction);

  this->m_ViewActions.push_back(QuadViewAction);

  QObject::connect(QuadViewAction, SIGNAL(triggered()),
                   this, SLOT(Quadview()));

  QAction* FullScreenXYAction = new QAction(tr("Full-Screen XY"), this);
  FullScreenXYAction->setCheckable(true);

  QIcon xyicon;
  xyicon.addPixmap(QPixmap(QString::fromUtf8(":/fig/xy.png")),
                   QIcon::Normal, QIcon::Off);
  FullScreenXYAction->setIcon(xyicon);

  group->addAction(FullScreenXYAction);

  this->m_ViewActions.push_back(FullScreenXYAction);

  QObject::connect(FullScreenXYAction, SIGNAL(triggered()),
                   this, SLOT(FullScreenViewXY()));

  QAction* FullScreenXZAction = new QAction(tr("Full-Screen XZ"), this);
  FullScreenXZAction->setCheckable(true);

  QIcon xzicon;
  xzicon.addPixmap(QPixmap(QString::fromUtf8(":/fig/zx.png")),
                   QIcon::Normal, QIcon::Off);
  FullScreenXZAction->setIcon(xzicon);

  group->addAction(FullScreenXZAction);

  this->m_ViewActions.push_back(FullScreenXZAction);

  QObject::connect(FullScreenXZAction, SIGNAL(triggered()),
                   this, SLOT(FullScreenViewXZ()));

  QAction* FullScreenYZAction = new QAction(tr("Full-Screen YZ"), this);
  FullScreenYZAction->setCheckable(true);

  QIcon yzicon;
  yzicon.addPixmap(QPixmap(QString::fromUtf8(":/fig/yz.png")),
                   QIcon::Normal, QIcon::Off);
  FullScreenYZAction->setIcon(yzicon);

  group->addAction(FullScreenYZAction);

  this->m_ViewActions.push_back(FullScreenYZAction);

  QObject::connect(FullScreenYZAction, SIGNAL(triggered()),
                   this, SLOT(FullScreenViewYZ()));

  QAction* FullScreenXYZAction = new QAction(tr("Full-Screen XYZ"), this);
  FullScreenXYZAction->setCheckable(true);

  QIcon xyzicon;
  xyzicon.addPixmap(QPixmap(QString::fromUtf8(":/fig/xyz.png")),
                    QIcon::Normal, QIcon::Off);
  FullScreenXYZAction->setIcon(xyzicon);

  group->addAction(FullScreenXYZAction);

  this->m_ViewActions.push_back(FullScreenXYZAction);

  QObject::connect(FullScreenXYZAction, SIGNAL(triggered()),
                   this, SLOT(FullScreenViewXYZ()));

  QAction* separator5 = new QAction(this);
  separator5->setSeparator(true);
  this->m_ViewActions.push_back(separator5);

  QAction* ActionDisplayAnnotations =
    new QAction(tr("Display annotations"), this);
  ActionDisplayAnnotations->setCheckable(true);
  ActionDisplayAnnotations->setChecked(true);
  ActionDisplayAnnotations->setStatusTip(tr(" Display or not annotations in each 2d view"));

  QIcon displayannotationsicon;
  displayannotationsicon.addPixmap(QPixmap(QString::fromUtf8(":/fig/2D_VIEWS_INFOS.png")),
                                   QIcon::Normal, QIcon::Off);
  ActionDisplayAnnotations->setIcon(displayannotationsicon);

  QObject::connect(ActionDisplayAnnotations, SIGNAL(triggered()),
                   this->m_ImageView, SLOT(ShowAnnotations()));

  this->m_ViewActions.push_back(ActionDisplayAnnotations);

  QAction* ActionDisplaySplinePlanes =
    new QAction(tr("Display spline planes"), this);
  ActionDisplaySplinePlanes->setCheckable(true);
  ActionDisplaySplinePlanes->setChecked(true);
  ActionDisplaySplinePlanes->setStatusTip(tr(" Display or not spline planes on each view"));

  QIcon displaysplineplaneicon;
  displaysplineplaneicon.addPixmap(QPixmap(QString::fromUtf8(":/fig/C_M_L.png")),
                                   QIcon::Normal, QIcon::Off);
  ActionDisplaySplinePlanes->setIcon(displaysplineplaneicon);

  QObject::connect(ActionDisplaySplinePlanes, SIGNAL(triggered()),
                   this->m_ImageView, SLOT(ShowSplinePlane()));

  this->m_ViewActions.push_back(ActionDisplaySplinePlanes);

  QAction* DisplayCube3D = new QAction(tr("Display 3D cube"), this);
  DisplayCube3D->setCheckable(true);
  DisplayCube3D->setChecked(true);
  DisplayCube3D->setStatusTip(tr(" Display or not cube in 3d"));

  QIcon cube3dicon;
  cube3dicon.addPixmap(QPixmap(QString::fromUtf8(":/fig/cube.png")),
                       QIcon::Normal, QIcon::Off);
  DisplayCube3D->setIcon(cube3dicon);

  QObject::connect(DisplayCube3D, SIGNAL(triggered()),
      this->m_ImageView, SLOT(ShowCube3D()));

  this->m_ViewActions.push_back(DisplayCube3D);

  QAction* separator = new QAction(this);
  separator->setSeparator(true);

  this->m_ViewActions.push_back(separator);

  QAction* LookupTableAction = new QAction(tr("Lookup Table"), this);
  LookupTableAction->setStatusTip(tr(" Change the associated lookup table"));

  //take
  QIcon luticon;
  luticon.addPixmap(QPixmap(QString::fromUtf8(":/fig/LookupTable.png")),
                    QIcon::Normal, QIcon::Off);
  LookupTableAction->setIcon(luticon);

  // Here write the connection
  QObject::connect(LookupTableAction, SIGNAL(triggered()),
                   this, SLOT(ChangeLookupTable()));

  this->m_ViewActions.push_back(LookupTableAction);

  QAction* ScalarBarAction = new QAction(tr("Display Scalar Bar"), this);
  ScalarBarAction->setCheckable(true);

  QIcon scalarbaricon;
  scalarbaricon.addPixmap(QPixmap(QString::fromUtf8(":/fig/scalarbar.png")),
                          QIcon::Normal, QIcon::Off);
  ScalarBarAction->setIcon(scalarbaricon);

  this->m_ViewActions.push_back(ScalarBarAction);

  QObject::connect(ScalarBarAction, SIGNAL(toggled(bool)),
                   this, SLOT(ShowScalarBar(bool)));

  QPixmap Pix(16, 16);
  Pix.fill(Qt::black);
  m_BackgroundColorAction = new QAction(Pix, tr("Set Background Color"), this);
  this->m_ViewActions.push_back(m_BackgroundColorAction);

  QObject::connect(m_BackgroundColorAction, SIGNAL(triggered()),
                   this, SLOT(ChangeBackgroundColor()));

  QAction* separator2 = new QAction(this);
  separator2->setSeparator(true);
  this->m_ViewActions.push_back(separator2);

  this->m_ViewActions.push_back(m_NavigationDockWidget->toggleViewAction());

  QAction* separator3 = new QAction(this);
  separator3->setSeparator(true);
  this->m_ViewActions.push_back(separator3);

  this->m_ViewActions.push_back(m_DataBaseTables->toggleViewAction());

  // to know if we should close the trace widget as well
  QObject::connect( m_DataBaseTables->toggleViewAction(), SIGNAL(toggled(bool)),
                   this, SLOT(CloseTabRequest(bool)));

  QAction* separator4 = new QAction(this);
  separator4->setSeparator(true);
  this->m_ViewActions.push_back(separator4);

  ///TODO create group actions for views changing
  QAction* Change3DPerspectiveToAxialAction =
    new QAction(tr("Change 3D view to Posterior "), this);
  this->m_ViewActions.push_back(Change3DPerspectiveToAxialAction);

  QIcon axialicon;
  axialicon.addPixmap(QPixmap(QString::fromUtf8(":/fig/PosteriorView.png")),
                      QIcon::Normal, QIcon::Off);
  Change3DPerspectiveToAxialAction->setIcon(axialicon);

  QObject::connect(Change3DPerspectiveToAxialAction, SIGNAL(triggered()),
                   this, SLOT(Change3DPerspectiveToAxial()));

  QAction* Change3DPerspectiveToCoronalAction =
    new QAction(tr("Change 3D view to Dorsal "), this);
  this->m_ViewActions.push_back(Change3DPerspectiveToCoronalAction);

  QIcon coronalicon;
  coronalicon.addPixmap(QPixmap(QString::fromUtf8(":/fig/DorsalView.png")),
                        QIcon::Normal, QIcon::Off);
  Change3DPerspectiveToCoronalAction->setIcon(coronalicon);

  QObject::connect(Change3DPerspectiveToCoronalAction, SIGNAL(triggered()),
                   this, SLOT(Change3DPerspectiveToCoronal()));

  QAction* Change3DPerspectiveToSagittalAction =
    new QAction(tr("Change 3D view to Left "), this);
  this->m_ViewActions.push_back(Change3DPerspectiveToSagittalAction);

  QIcon sagittalicon;
  sagittalicon.addPixmap(QPixmap(QString::fromUtf8(":/fig/LeftView.png")),
                         QIcon::Normal, QIcon::Off);
  Change3DPerspectiveToSagittalAction->setIcon(sagittalicon);

  QObject::connect(Change3DPerspectiveToSagittalAction, SIGNAL(triggered()),
                   this, SLOT(Change3DPerspectiveToSagittal()));

  QAction* separator6 = new QAction(this);
  separator6->setSeparator(true);
  this->m_ViewActions.push_back(separator6);

  // Enable volume rendering
  QAction* VolumeRenderingAction =
    new QAction(tr("Enable the volume rendering for the current channel(s)"), this);
  VolumeRenderingAction->setCheckable(true);
  VolumeRenderingAction->setChecked(false);
  this->m_ViewActions.push_back(VolumeRenderingAction);

  QIcon volumerenderingicon;
  volumerenderingicon.addPixmap(QPixmap(QString::fromUtf8(":/fig/VolumeRendering1.png")),
                         QIcon::Normal, QIcon::Off);
  VolumeRenderingAction->setIcon(volumerenderingicon);

  QObject::connect(VolumeRenderingAction, SIGNAL(toggled(bool)),
                    this->m_ImageView, SLOT(EnableVolumeRendering(bool)));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateToolsActions()
{
#if defined(ENABLEFFMPEG) || defined(ENABLEAVI)
  this->m_ToolsActions.push_back(m_VideoRecorderWidget->toggleViewAction());
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(false);
#endif

  m_TakeSnapshotAction = new QAction(tr("Take Snapshot"), this);
  QIcon snapshoticon;
  snapshoticon.addPixmap(QPixmap(QString::fromUtf8(":/fig/camera-photo.png")),
                         QIcon::Normal, QIcon::Off);
  m_TakeSnapshotAction->setIcon(snapshoticon);
  m_TakeSnapshotAction->setStatusTip(tr("You have to be in full screen view to use the snapshot"));
  m_TakeSnapshotAction->setEnabled(false);
  QObject::connect(m_TakeSnapshotAction, SIGNAL(triggered()),
                   this, SLOT(TakeSnapshot()));

  this->m_ToolsActions.push_back(m_TakeSnapshotAction);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::CreateModeActions()
{
  QActionGroup* group = new QActionGroup(this);

  // Call superclass
  QGoTabElementBase::CreateModeActions(group);

  //--------------------------------//
  //  Contour segmentation mode     //
  //--------------------------------//

  QAction* ContourSegmentationAction =
      m_ContourSegmentationDockWidget->toggleViewAction();

  group->addAction(ContourSegmentationAction);

  this->m_ModeActions.push_back(ContourSegmentationAction);

  QObject::connect( ContourSegmentationAction,
                    SIGNAL(toggled(bool)),
                    m_ContourSegmentationDockWidget,
                    SLOT(interactorBehavior(bool)));

  QObject::connect( ContourSegmentationAction,
                    SIGNAL(toggled(bool)),
                    this,
                    SLOT(RequieresTraceWidget(bool)));

  QObject::connect( ContourSegmentationAction,
                    SIGNAL(toggled(bool)),
                    this,
                    SLOT(ShowTraceDockWidgetForContour(bool)));

  //---------------------------------//
  //        Mesh segmentation        //
  //---------------------------------//

  QAction* MeshSegmentationAction =
      m_MeshSegmentationDockWidget->toggleViewAction();

  group->addAction(MeshSegmentationAction);

  this->m_ModeActions.push_back(MeshSegmentationAction);

  QObject::connect( MeshSegmentationAction,
                    SIGNAL(toggled(bool)),
                    m_MeshSegmentationDockWidget,
                    SLOT(interactorBehavior(bool)));

  QObject::connect( MeshSegmentationAction,
                    SIGNAL(toggled(bool)),
                    this,
                    SLOT(RequieresTraceWidget(bool)));

  QObject::connect( MeshSegmentationAction,
                    SIGNAL(toggled(bool)),
                    this,
                    SLOT(ShowTraceDockWidgetForMesh(bool)) );

  //---------------------------------//
  //       Actor picking  mode       //
  //---------------------------------//
  QAction* ActorPickingAction = new QAction(tr("Actor Picking"), this);
  ActorPickingAction->setCheckable(true);
  ActorPickingAction->setChecked(false);

  QIcon ActorPickingIcon;
  ActorPickingIcon.addPixmap(QPixmap(QString::fromUtf8(":/fig/MeshPicking.png")),
                               QIcon::Normal, QIcon::Off);
  ActorPickingAction->setIcon(ActorPickingIcon);

  group->addAction(ActorPickingAction);

  this->m_ModeActions.push_back(ActorPickingAction);
  // it also updates the interactor behaviour
  QObject::connect(ActorPickingAction, SIGNAL(toggled(bool)),
                   this, SLOT(ActorPickingInteractorBehavior(bool)));

  //---------------------------------//
  //       Box 3D picking  mode      //
  //---------------------------------//
  QAction* Box3DPickingAction = new QAction(tr("Box 3D Picking"), this);
  Box3DPickingAction->setCheckable(true);
  Box3DPickingAction->setChecked(false);

  QIcon Box3DPickingIcon;
  Box3DPickingIcon.addPixmap(QPixmap(QString::fromUtf8(":/fig/Box3DPicking.png")),
                             QIcon::Normal, QIcon::Off);
  Box3DPickingAction->setIcon(Box3DPickingIcon);

  group->addAction(Box3DPickingAction);

  this->m_ModeActions.push_back(Box3DPickingAction);
  // it also updates the interactor behaviour
  QObject::connect(Box3DPickingAction, SIGNAL(toggled(bool)),
                   this, SLOT(Box3DPicking(bool)));

  //---------------------------------//
  //        Plane  widget  mode      //
  //---------------------------------//
  QAction* PlaneWidgetAction = new QAction(tr("Plane Widget"), this);
  PlaneWidgetAction->setCheckable(true);
  PlaneWidgetAction->setChecked(false);

  QIcon PlaneWidgetIcon;
  PlaneWidgetIcon.addPixmap(QPixmap(QString::fromUtf8(":/fig/PlaneWidget.png")),
                             QIcon::Normal, QIcon::Off);
  PlaneWidgetAction->setIcon(PlaneWidgetIcon);

  group->addAction(PlaneWidgetAction);

  this->m_ModeActions.push_back(PlaneWidgetAction);
  // it also updates the interactor behaviour
  QObject::connect(PlaneWidgetAction, SIGNAL(toggled(bool)),
                   this, SLOT(PlaneWidgetInteractorBehavior(bool)));


  // NOT A MODE: ANNOTATION

  QAction* separator = new QAction(this);
  separator->setSeparator(true);
  this->m_ModeActions.push_back(separator);

  //---------------------------------//
  //         Distance    mode        //
  //---------------------------------//
  QAction* DistanceAction = new QAction(tr("Measure Distance"), this);

  DistanceAction->setCheckable(true);
  DistanceAction->setChecked(false);

  QIcon DistanceIcon;
  DistanceIcon.addPixmap(QPixmap(QString::fromUtf8(":/fig/Distance.png")),
                         QIcon::Normal, QIcon::Off);
  DistanceAction->setIcon(DistanceIcon);

  this->m_ModeActions.push_back(DistanceAction);

  QObject::connect(DistanceAction, SIGNAL(toggled(bool)),
                   this, SLOT(DistanceWidgetInteractorBehavior(bool)));

  // NOT A MODE: ANNOTATION

  //---------------------------------//
  //           Angle     mode        //
  //---------------------------------//
  QAction* AngleAction = new QAction(tr("Measure Angle"), this);
  AngleAction->setCheckable(true);
  AngleAction->setChecked(false);

  QIcon AngleIcon;
  AngleIcon.addPixmap(QPixmap(QString::fromUtf8(":/fig/Angle.png")),
                      QIcon::Normal, QIcon::Off);
  AngleAction->setIcon(AngleIcon);

  this->m_ModeActions.push_back(AngleAction);

  QObject::connect(AngleAction, SIGNAL(toggled(bool)),
                   this, SLOT(AngleWidgetInteractorBehavior(bool)));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::CreateBookmarkActions()
{
  QAction* AddBookmarkAction = new QAction(tr("Add a bookmark"), this);
  this->m_BookmarkActions.push_back(AddBookmarkAction);
  QObject::connect(AddBookmarkAction, SIGNAL (triggered()),
                   this, SLOT (AddBookmark()));
  QAction* DeleteBookmarkAction = new QAction(tr("Delete a bookmark"), this);
  this->m_BookmarkActions.push_back(DeleteBookmarkAction);
  QObject::connect(DeleteBookmarkAction, SIGNAL (triggered()),
                   this->m_DataBaseTables, SLOT(DeleteBookmarks()));
  QObject::connect(this->m_DataBaseTables, SIGNAL (PrintDBReady()),
                   this, SLOT(GetTheRelatedToDBActions()));
  QObject::connect(this->m_DataBaseTables, SIGNAL(OpenBookmarksToUpdate()),
                   this, SLOT(GetTheOpenBookmarksActions()));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::GetTheRelatedToDBActions()
{
  this->GetTheOpenBookmarksActions();
  QMenu*   ImportMenu = new QMenu(tr("Import"), this);
  QAction* ImportContoursAction = new QAction(tr("Contours"), this);
  QAction* ImportMeshesAction = new QAction(tr("3DMeshes"), this);
  ImportMenu->addAction(ImportContoursAction);
  ImportMenu->addAction(ImportMeshesAction);
  QMenu*   ExportMenu = new QMenu(tr("Export"), this);
  QAction* ExportContoursAction = new QAction(tr("Contours"), this);
  QAction* ExportMeshesAction = new QAction(tr("3DMeshes"), this);
  ExportMenu->addAction(ExportContoursAction);
  ExportMenu->addAction(ExportMeshesAction);
  this->m_ToolsActions.push_back(ImportMenu->menuAction());
  this->m_ToolsActions.push_back(ExportMenu->menuAction());
  QObject::connect(ExportContoursAction, SIGNAL(triggered()),
                   this->m_DataBaseTables, SLOT(ExportContours ()));
  QObject::connect(ImportContoursAction, SIGNAL(triggered()),
                   this, SLOT(ImportContours()));
  QObject::connect(ExportMeshesAction, SIGNAL(triggered()),
                   this->m_DataBaseTables, SLOT(ExportMeshes()));
  QObject::connect(ImportMeshesAction, SIGNAL (triggered()),
                   this, SLOT (ImportMeshes()));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::GetTheOpenBookmarksActions()
{
  bool UpdateOpenBookmarks = false;
  if (this->m_BookmarkActions.size() > 2)
    {
    this->m_BookmarkActions.erase(
      this->m_BookmarkActions.begin() + this->m_BookmarkActions.size() - 1
      );
    UpdateOpenBookmarks = true;
    }
  NamesDescrContainerType ListBookmarks =
    this->m_DataBaseTables->GetListBookmarks();
  size_t NumberBookmarks = ListBookmarks.size();
  QMenu* OpenBookmarkMenu = new QMenu(tr("Open a bookmark"), this);
  for (size_t i = 0; i < NumberBookmarks; i++)
    {
    QAction* OpenBookmarkAction
      = new QAction(ListBookmarks[i].first.c_str(), this);
    std::string TextStatusTip = "Description of the bookmark: ";
    TextStatusTip += ListBookmarks[i].second;
    OpenBookmarkAction->setStatusTip(TextStatusTip.c_str());
    OpenBookmarkMenu->addAction(OpenBookmarkAction);
    QObject::connect(
      OpenBookmarkAction, SIGNAL(triggered()),
      this, SLOT(OpenExistingBookmark())
      );
    }
  this->m_BookmarkActions.push_back(OpenBookmarkMenu->menuAction());
  if (UpdateOpenBookmarks)
    {
    emit UpdateBookmarkOpenActions(this->m_BookmarkActions);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::OpenExistingBookmark()
{
  QAction*          taction = qobject_cast<QAction*>(sender());
  std::string       BookmarkName = taction->text().toStdString();
  GoDBCoordinateRow Coord = this->m_DataBaseTables->GetCoordinateForBookmark(BookmarkName);

  int tt = atoi(Coord.GetMapValue("TCoord").c_str());
  int tz = atoi(Coord.GetMapValue("ZCoord").c_str());
  int ty = atoi(Coord.GetMapValue("YCoord").c_str());
  int tx = atoi(Coord.GetMapValue("XCoord").c_str());

  this->SetTimePoint( tt );
  this->SetSliceViewXY( tz );
  this->SetSliceViewXZ( ty );
  this->SetSliceViewYZ( tx );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
TakeSnapshot()
{
  // Get the current view displayed in full screen
  int FullScreenView = m_ImageView->GetFullScreenView();

  // TODO enhance the name of the files
  QString filename = QDir::toNativeSeparators(QDir::homePath());
  filename.append("snapshot_");

  switch (FullScreenView)
    {
    case 1:
      // X Slice
      m_ImageView->SnapshotViewXY(GoFigure::PNG, filename);
      break;

    case 2:
      // Y Slice
      m_ImageView->SnapshotView2(GoFigure::PNG, filename);
      break;

    case 3:
      // Z Slice
      m_ImageView->SnapshotView3(GoFigure::PNG, filename);
      break;

    default:
      // 3D view
      m_ImageView->SnapshotViewXYZ(GoFigure::PNG, filename);
      break;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
setupUi(QWidget* iParent)
{
  if (iParent->objectName().isEmpty())
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

  QObject::connect(m_ImageView, SIGNAL(SliceViewXYChanged(int)),
                   this, SIGNAL(SliceViewXYChanged(int)));

  QObject::connect(m_ImageView, SIGNAL(SliceViewXZChanged(int)),
                   this, SIGNAL(SliceViewXZChanged(int)));

  QObject::connect(m_ImageView, SIGNAL(SliceViewYZChanged(int)),
                   this, SIGNAL(SliceViewYZChanged(int)));

  QObject::connect(m_ImageView, SIGNAL(FullScreenViewChanged(int)),
                   this, SIGNAL(FullScreenViewChanged(int)));
/*
  // connect the contours selection connection
  QObject::connect(m_ImageView, SIGNAL(ContoursSelectionChanged()),
                   this, SLOT(HighLightContours()));

  QObject::connect(m_ImageView, SIGNAL(ContoursSelectionChanged()),
                   this, SLOT(SelectContoursInTable()));*/

  // connect the contours selection connection
  /*QObject::connect(m_ImageView, SIGNAL(ContoursSelectionXYChanged()),
                   this, SLOT(HighlightContoursXY()));*/
  QObject::connect(m_ImageView, SIGNAL(SelectionXYChanged()),
                     this, SLOT(HighlightXY()));
  // connect the contours selection connection
  QObject::connect(m_ImageView, SIGNAL(SelectionXZChanged()),
                   this, SLOT(HighlightXZ()));
  // connect the contours selection connection
  QObject::connect(m_ImageView, SIGNAL(SelectionYZChanged()),
                   this, SLOT(HighlightYZ()));
  // connect the contours selection connection
  QObject::connect(m_ImageView, SIGNAL(SelectionXYZChanged()),
                   this, SLOT(HighlightXYZ()));

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
  QObject::connect(m_ImageView, SIGNAL(VisibilityXYZChanged()),
                   this, SLOT(VisibilityXYZ()));

  m_HBoxLayout = new QHBoxLayout(iParent);
  m_HBoxLayout->addWidget(m_VSplitter);

  retranslateUi(iParent);

  QMetaObject::connectSlotsByName(iParent);
} // setupUi
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
retranslateUi(QWidget *iParent)
{
  iParent->setWindowTitle(tr("QGoTabImageView3DwT"));
  Q_UNUSED(iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoFigure::TabDimensionType
QGoTabImageView3DwT::
GetTabDimensionType() const
{
  return GoFigure::THREE_D_WITH_T;
}
//-------------------------------------------------------------------------

//#########################################################################
//#########################################################################
// Set Inputs

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetLSMReader(vtkLSMReader* iReader, const int& iTimePoint)
{
  if (iReader)
    {
    if (m_LSMReader.empty())
      {
      m_LSMReader.push_back(iReader);
      }
    else
      {
      if (iReader != m_LSMReader[0])
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

    if (NumberOfChannels > 1)
      {
      m_NavigationDockWidget->SetChannel(0);
      m_ContourSegmentationDockWidget->SetChannel(0);
      m_MeshSegmentationDockWidget->SetChannel(0);
      m_InternalImages.resize(NumberOfChannels);

      for (int i = 1; i < NumberOfChannels; i++)
        {
        m_NavigationDockWidget->SetChannel(i);
        m_ContourSegmentationDockWidget->SetChannel(i);
        m_MeshSegmentationDockWidget->SetChannel(i);

        m_LSMReader.push_back(vtkSmartPointer<vtkLSMReader>::New());
        m_LSMReader.back()->SetFileName(m_LSMReader[0]->GetFileName());
        m_LSMReader.back()->SetUpdateChannel(i);
        }
      }

    m_NavigationDockWidget->SetXMinimumAndMaximum(0, dim[0] - 1);
    m_NavigationDockWidget->SetXSlice((dim[0] - 1) / 2);

    m_NavigationDockWidget->SetYMinimumAndMaximum(0, dim[1] - 1);
    m_NavigationDockWidget->SetYSlice((dim[1] - 1) / 2);

    m_NavigationDockWidget->SetZMinimumAndMaximum(0, dim[2] - 1);
    m_NavigationDockWidget->SetZSlice((dim[2] - 1) / 2);

    m_NavigationDockWidget->SetTMinimumAndMaximum(0, dim[3] - 1);
    m_NavigationDockWidget->SetTSlice(iTimePoint);
    if (m_TCoord != iTimePoint)
      {
      SetTimePoint(iTimePoint);
      }

#if defined(ENABLEFFMPEG) || defined(ENABLEAVI)
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
QGoTabImageView3DwT::
SetMegaCaptureFile(
  const GoFigureFileInfoHelperMultiIndexContainer& iContainer,
  const GoFigure::FileType& iFileType,
  const std::string& iHeader,
  const unsigned int& iTimePoint)
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

  vtkImageData* temp = m_MegaCaptureReader->GetOutput(min_ch);

  int extent[6];
  temp->GetExtent(extent);

  // Initialize the widgets with the good number of channels
  // it will update the size of the related combobox
  m_NavigationDockWidget->SetNumberOfChannels(NumberOfChannels);

  // Set up QSpinBox in m_VideoRecorderWidget
  if (NumberOfChannels > 1)
    {
    m_NavigationDockWidget->SetChannel(0);
    m_ContourSegmentationDockWidget->SetChannel(0);
    m_MeshSegmentationDockWidget->SetChannel(0);
    m_InternalImages.resize(NumberOfChannels, NULL);

    for (unsigned int i = 1; i < NumberOfChannels; i++)
      {
      m_NavigationDockWidget->SetChannel(i);
      m_ContourSegmentationDockWidget->SetChannel(i);
      m_MeshSegmentationDockWidget->SetChannel(i);
      }
    }

  m_NavigationDockWidget->SetXMinimumAndMaximum(extent[0], extent[1]);
  m_NavigationDockWidget->SetXSlice((extent[0] + extent[1]) / 2);

  m_NavigationDockWidget->SetYMinimumAndMaximum(extent[2], extent[3]);
  m_NavigationDockWidget->SetYSlice((extent[2] + extent[3]) / 2);

  m_NavigationDockWidget->SetZMinimumAndMaximum(extent[4], extent[5]);
  m_NavigationDockWidget->SetZSlice((extent[4] + extent[5]) / 2);

  m_NavigationDockWidget->SetTMinimumAndMaximum(min_t, max_t);
  m_NavigationDockWidget->SetTSlice(iTimePoint);

  if (static_cast<unsigned int>(m_TCoord) != iTimePoint)
    {
    SetTimePoint(iTimePoint);
    }

  // Set up QSpinBox in m_VideoRecorderWidget
#if defined(ENABLEFFMPEG) || defined(ENABLEAVI)
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
QGoTabImageView3DwT::
SetTimePointWithMegaCapture(const int& iTimePoint)
{
  m_MegaCaptureReader->SetTimePoint(m_TCoord);

  unsigned int min_ch = m_MegaCaptureReader->GetMinChannel();
  unsigned int max_ch = m_MegaCaptureReader->GetMaxChannel();
  m_MegaCaptureReader->Update();

  if (max_ch != min_ch)
    {
    vtkSmartPointer<vtkImageAppendComponents> append_filter =
      vtkSmartPointer<vtkImageAppendComponents>::New();

    for (unsigned int i = min_ch; i <= max_ch; i++)
      {
      m_InternalImages[i] = m_MegaCaptureReader->GetOutput(i);
      append_filter->AddInput(m_InternalImages[i]);
      }
    // This is really stupid!!!
    if (max_ch < 2)
      {
      for (unsigned int i = max_ch + 1; i < 3; i++)
        {
        append_filter->AddInput(m_InternalImages[0]);
        }
      }
    append_filter->Update();

    if (this->m_NavigationDockWidget->ShowAllChannels())
      {
      m_Image->ShallowCopy(append_filter->GetOutput());
      }
    else
      {
      int ch = this->m_NavigationDockWidget->GetCurrentChannel();
      if (ch != -1)
        {
        m_Image->ShallowCopy(m_InternalImages[ch]);
        }
      }
    }
  else
    {
    m_Image->ShallowCopy(m_MegaCaptureReader->GetOutput(min_ch));
    m_Image->SetNumberOfScalarComponents(1);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetTimePointWithLSMReaders(const int& iTimePoint)
{
  m_LSMReader[0]->SetUpdateTimePoint(m_TCoord);

  int NumberOfChannels = m_LSMReader[0]->GetNumberOfChannels();

  if (NumberOfChannels > 1)
    {
    m_InternalImages[0] = m_LSMReader[0]->GetOutput();

    vtkSmartPointer<vtkImageAppendComponents> append_filter =
      vtkSmartPointer<vtkImageAppendComponents>::New();
    append_filter->AddInput(m_InternalImages[0]);

    for (int i = 1; i < NumberOfChannels; i++)
      {
      m_LSMReader[i]->SetUpdateTimePoint(m_TCoord);
      m_LSMReader[i]->Update();

      m_InternalImages[i] = m_LSMReader[i]->GetOutput();
      append_filter->AddInput(m_InternalImages[i]);
      }
    // This is really stupid!!!
    if (NumberOfChannels < 3)
      {
      for (int i = NumberOfChannels; i < 3; i++)
        {
        append_filter->AddInput(m_InternalImages[0]);
        }
      }
    append_filter->Update();

    m_Image->ShallowCopy(append_filter->GetOutput());
    }
  else
    {
    m_LSMReader[0]->Update();

    m_Image->ShallowCopy(m_LSMReader[0]->GetOutput());
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetTimePoint(const int& iTimePoint)
{
  if (iTimePoint == m_TCoord)
    {
    return;
    }

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  // this->m_ContourContainer->RemoveActorsWithGivenTimePoint( m_TCoord );
  // this->m_MeshContainer->RemoveActorsWithGivenTimePoint( m_TCoord );

  if (!m_LSMReader.empty())
    {
    if (iTimePoint >= m_LSMReader[0]->GetNumberOfTimePoints())
      {
      return;
      }
    else
      {
      m_TCoord = iTimePoint;
      SetTimePointWithLSMReaders(iTimePoint);
      emit TimePointChanged(m_TCoord);
      }
    }
  else
    {
    if (!m_FileList.empty())
      {
      unsigned int t = static_cast<unsigned int>(iTimePoint);
      if ((t < m_MegaCaptureReader->GetMinTimePoint()) ||
          (t > m_MegaCaptureReader->GetMaxTimePoint()))
        {
        return;
        }
      else
        {
        m_TCoord = iTimePoint;
        SetTimePointWithMegaCapture(iTimePoint);
        emit TimePointChanged(m_TCoord);
        }
      }
    else
      {
      // no lsm reader, no file list. did you really provide any input?
      std::cerr << "No lsm reader. No file list" << std::endl;
      }
    }

  //this->m_ContourContainer->AddActorsWithGivenTimePoint( m_TCoord );
  //this->m_MeshContainer->AddActorsWithGivenTimePoint( m_TCoord );
  //this->ShowTraceDockWidgetForContour(false);
  this->m_ContourContainer->ShowActorsWithGivenTimePoint( m_TCoord );
  this->m_MeshContainer->ShowActorsWithGivenTimePoint( m_TCoord );
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
QGoTabImageView3DwT::
ChangeLookupTable()
{
  vtkImageData* image = m_ImageView->GetImage();

  if (image->GetNumberOfScalarComponents() == 1)
    {
    /// \todo there may be one memory leak in here!
    vtkLookupTable* lut = QGoLUTDialog::GetLookupTable(this,
                                                       tr("Choose one look-up table"));
    m_ImageView->SetLookupTable(lut);

    // free memory since it is not freed in the QGoLUTDialog
    lut->Delete();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ShowScalarBar(const bool& iShow)
{
  vtkImageData* image = m_ImageView->GetImage();

  if (image->GetNumberOfScalarComponents() == 1)
    {
    m_ImageView->ShowScalarBar(iShow);
    }
}
//------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString
QGoTabImageView3DwT::
SnapshotViewXY(
  const GoFigure::FileType& iType,
  const QString& iBaseName)
{
  return m_ImageView->SnapshotViewXY(iType, iBaseName);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString
QGoTabImageView3DwT::
SnapshotView2(
  const GoFigure::FileType& iType,
  const QString& iBaseName)
{
  return m_ImageView->SnapshotView2(iType, iBaseName);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString
QGoTabImageView3DwT::
SnapshotView3(
  const GoFigure::FileType& iType,
  const QString& iBaseName)
{
  return m_ImageView->SnapshotView3(iType, iBaseName);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString
QGoTabImageView3DwT::
SnapshotViewXYZ(
  const GoFigure::FileType& iType,
  const QString& iBaseName)
{
  return m_ImageView->SnapshotViewXYZ(iType, iBaseName);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetSliceViewXY(const int& iS)
{
  m_ImageView->SetSliceViewXY(iS);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetSliceViewXZ(const int& iS)
{
  m_ImageView->SetSliceViewXZ(iS);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetSliceViewYZ(const int& iS)
{
  m_ImageView->SetSliceViewYZ(iS);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetFullScreenView(const int& iS)
{
  m_ImageView->SetFullScreenView(iS);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
Quadview()
{
  m_ImageView->SetFullScreenView(0);
  m_TakeSnapshotAction->setEnabled(false);
#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(false);
#endif
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
FullScreenViewXY()
{
  m_ImageView->SetFullScreenView(1);
  m_TakeSnapshotAction->setEnabled(true);
#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
#endif
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
FullScreenViewXZ()
{
  m_ImageView->SetFullScreenView(2);
  m_TakeSnapshotAction->setEnabled(true);
#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
#endif
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
FullScreenViewYZ()
{
  m_ImageView->SetFullScreenView(3);
  m_TakeSnapshotAction->setEnabled(true);
#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
#endif
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
FullScreenViewXYZ()
{
  m_ImageView->SetFullScreenView(4);
  m_TakeSnapshotAction->setEnabled(true);
#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
#endif
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
GetBackgroundColorFromImageViewer()
{
  double r, g, b;
  m_ImageView->GetBackgroundColor(r, g, b);
  m_BackgroundColor.setRgbF(r, g, b);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetBackgroundColorToImageViewer()
{
  m_ImageView->SetBackgroundColor(m_BackgroundColor);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ChangeBackgroundColor()
{
  double r, g, b;
  m_ImageView->GetBackgroundColor(r, g, b);
  m_BackgroundColor.setRgbF(r, g, b);

  QColor temp = QColorDialog::getColor(m_BackgroundColor,
                                       this, tr("Choose Background Color"));

  if (temp != m_BackgroundColor)
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
QGoTabImageView3DwT::
ShowAllChannels(bool iChecked)
{
  if (iChecked)
    {
    // Reset the window level
    m_ImageView->ResetWindowLevel();

    vtkSmartPointer<vtkImageAppendComponents> append_filter =
      vtkSmartPointer<vtkImageAppendComponents>::New();

    for (unsigned int i = 0; i < m_InternalImages.size(); i++)
      {
      append_filter->AddInput(m_InternalImages[i]);
      }

    // This is really stupid!!!
    if (m_InternalImages.size() < 3)
      {
      for (size_t i = m_InternalImages.size(); i < 3; i++)
        {
        append_filter->AddInput(m_InternalImages[0]);
        }
      }
    append_filter->Update();

    m_Image->ShallowCopy(append_filter->GetOutput());
    Update();
    }
  else
    {
    int ch = this->m_NavigationDockWidget->GetCurrentChannel();
    if (ch != -1)
      {
      m_Image->ShallowCopy(m_InternalImages[ch]);
      Update();
      }
    }
}
//------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ShowOneChannel(int iChannel)
{
  if ((iChannel != -1) && (!m_InternalImages.empty()))
    {
    m_Image->ShallowCopy(m_InternalImages[iChannel]);
    Update();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//QGoTabImageView3DwT::IDWithColorData
void
QGoTabImageView3DwT::
SaveContour(vtkPolyData* contour, vtkPolyData* contour_nodes)
{
  //IDWithColorData ContourData = IDWithColorData(-1, QColor(Qt::white));
  if ((contour->GetNumberOfPoints() > 2) && (m_TCoord >= 0))
    {
    // Compute Bounding Box
    int* bounds = GetBoundingBox(contour);
    // Save contour in database :
    this->m_DataBaseTables->SaveContoursFromVisuInDB( bounds[0],
                                                      bounds[2],
                                                      bounds[4],
                                                      m_TCoord,
                                                      bounds[1],
                                                      bounds[3],
                                                      bounds[5],
                                                      contour_nodes);
    //delete bounds;
    }
  //else
  //  {
  //  std::cerr << "(contour->GetNumberOfPoints() < 2) or  (m_TCoord < 0)" << std::endl;
  //  }
  //useful ??
  //return ContourData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int*
QGoTabImageView3DwT::GetBoundingBox(vtkPolyData* iElement)
{
  if(iElement)
  {
  // Compute Bounding Box
  double bounds[6];
  iElement->GetBounds(bounds);

  // Extract Min and Max from bounds
  double       Min[3], Max[3];
  int          k = 0;
  unsigned int i;
  for (i = 0; i < 3; i++)
    {
    Min[i] = bounds[k++];
    Max[i] = bounds[k++];
    }

  int* min_idx = this->GetImageCoordinatesFromWorldCoordinates(Min);
  int* max_idx = this->GetImageCoordinatesFromWorldCoordinates(Max);

  int* boundingBox = new int;
  boundingBox[0] = min_idx[0];
  boundingBox[1] = max_idx[0];

  boundingBox[2] = min_idx[1];
  boundingBox[3] = max_idx[1];

  boundingBox[4] = min_idx[2];
  boundingBox[5] = max_idx[2];

  delete min_idx;
  delete max_idx;
  return boundingBox;
  }
  return NULL;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkActor*> QGoTabImageView3DwT::
VisualizeContour( vtkPolyData* contour )
{
  std::vector<vtkActor*> oActors;

  if( contour->GetNumberOfPoints() > 2 )
    {
    //m_ContourId = iContourID;
    double* RGBA = this->m_ContourContainer->m_CurrentElement.rgba;

    vtkProperty* contour_property = vtkProperty::New();
    contour_property->SetColor(RGBA[0], RGBA[1], RGBA[2]);
    contour_property->SetOpacity(RGBA[3]);

    /// TODO shallow copy...?
    // get corresponding actor from visualization
    vtkPolyData* contour_copy = vtkPolyData::New();
    contour_copy->ShallowCopy(contour);

    oActors =
      this->AddContour(contour_copy, contour_property);

    contour_property->Delete();
    }

  m_ImageView->UpdateRenderWindows();

  return oActors;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<vtkActor*>
QGoTabImageView3DwT::
VisualizeMesh( vtkPolyData* iMesh )
{
  std::vector<vtkActor*> oActors;

  //m_ContourId = iContourID;
  double* RGBA = this->m_MeshContainer->m_CurrentElement.rgba;

  vtkProperty* mesh_property = vtkProperty::New();
  mesh_property->SetColor(RGBA[0], RGBA[1], RGBA[2]);
  mesh_property->SetOpacity(RGBA[3]);

  /// TODO shallow copy...?
  // get corresponding actor from visualization
  vtkPolyData* mesh_copy = vtkPolyData::New();
  mesh_copy->ShallowCopy(iMesh);

  oActors = this->AddContour(mesh_copy, mesh_property);

  mesh_property->Delete();

  m_ImageView->UpdateRenderWindows();

  return oActors;
}
//-------------------------------------------------------------------------


//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ValidateContour()
{
  // to make sure that m_ContourId is set to the right value
  //int          ContourID = -1;
  //double rgba[4] = {0., 0., 0., 0.};
  IDWithColorData test;
  bool re_edit = m_ContourSegmentationDockWidget->GetReeditMode();

  for ( int i = 0; i < m_ImageView->GetNumberOfImageViewers(); i++)
    {
    vtkPolyData* nodes = m_ImageView->GetContourRepresentationNodePolydata(i);

    if( nodes->GetNumberOfPoints() > 2 )
      {
      vtkPolyData* contour =  m_ImageView->GetContourRepresentationAsPolydata(i);

      SaveContour( contour, nodes );

      std::vector<vtkActor*> actors = VisualizeContour( contour );

      m_ContourContainer->UpdateCurrentElementFromVisu(actors,
                                          nodes,
                                          m_TCoord,
                                          re_edit,//highlighted
                                          true);//visible

      m_ContourContainer->InsertCurrentElement();
      }
    }

  if (re_edit)
    {
    //std::list<int> listofrowstobeselected;
    //listofrowstobeselected.push_back(m_ContourId);

    //m_DataBaseTables->ChangeContoursToHighLightInfoFromVisu(listofrowstobeselected,
                                                          //  true); // reedit mode = true
    this->m_DataBaseTables->GetTraceManualEditingDockWidget()->setEnabled(true);
    m_ContourSegmentationDockWidget->SetReeditMode(false);
    m_ImageView->ReinitializeContourWidget();
    //m_ContourSegmentationDockWidget->setEnabled( false );
    m_ContourSegmentationDockWidget->hide();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int*
QGoTabImageView3DwT::
GetImageCoordinatesFromWorldCoordinates(double iPos[3])
{
  return m_ImageView->GetImageCoordinatesFromWorldCoordinates(iPos);
}

//-------------------------------------------------------------------------
// std::vector< vtkQuadricLODActor* >
std::vector<vtkActor*>
QGoTabImageView3DwT::
AddContour(vtkPolyData* dataset, vtkProperty* iProperty)
{
  return this->m_ImageView->AddContour(dataset, iProperty);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ReEditContour(const unsigned int& iId)
{
  vtkPolyData* nodes = NULL;

  if ( this->m_ContourContainer
       ->RemoveElementFromVisualizationWithGivenTraceID( iId ) )
    {
    nodes = this->m_ContourContainer->m_CurrentElement.Nodes;

    int* bounds = this->GetBoundingBox( nodes );

    int dir = ContourMeshContainer::ComputeDirectionFromBounds<int>( bounds );

    if( dir != -1 )
      {
      //this->m_ReEditContourMode = true;

      int idx[3];
      idx[0] = ( bounds[0] + bounds[1] ) / 2;
      idx[1] = ( bounds[2] + bounds[3] ) / 2;
      idx[2] = ( bounds[4] + bounds[5] ) / 2;

      this->GoToLocation( idx[0], idx[1], idx[2], m_TCoord );

      //this->m_ManualSegmentationDockWidget->show();
      this->m_ModeActions[0]->setChecked(true);

      m_ImageView->InitializeContourWidgetNodes( dir , nodes );

      this->m_ContourSegmentationDockWidget->show();
      //this->m_ContourSegmentationDockWidget->setEnabled(true);
      this->m_ContourSegmentationDockWidget->SegmentationMethod(0);
      this->m_ContourSegmentationDockWidget->SetReeditMode( true );
          //m_ManualSegmentationDockWidget->setEnabled(true);
      //this->m_DataBaseTables->GetTraceManualEditingDockWidget()->setEnabled(false);
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighlightXY()
{
  vtkActor* temp_actor = m_ImageView->GetCurrentActor();
  if( temp_actor )
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
            temp_actor );
       /* }
      else
        {*/
        m_MeshContainer->UpdateElementHighlightingWithGivenActor< ActorXY >(
            temp_actor );
        /*}
      }*/
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighlightXZ()
{
  vtkActor* temp_actor = m_ImageView->GetCurrentActor();
  if( temp_actor )
    {
    m_ContourContainer->UpdateElementHighlightingWithGivenActor< ActorXZ >(
        temp_actor );
    m_MeshContainer->UpdateElementHighlightingWithGivenActor< ActorXZ >(
        temp_actor );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighlightYZ()
{
  vtkActor* temp_actor = m_ImageView->GetCurrentActor();
  if( temp_actor )
    {
    m_ContourContainer->UpdateElementHighlightingWithGivenActor< ActorYZ >(
        temp_actor );
    m_MeshContainer->UpdateElementHighlightingWithGivenActor< ActorYZ >(
        temp_actor );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighlightXYZ()
{
  vtkActor* temp_actor = m_ImageView->GetCurrentActor();
  if( temp_actor )
    {
    m_ContourContainer->UpdateElementHighlightingWithGivenActor< ActorXYZ >(
        temp_actor );
    m_MeshContainer->UpdateElementHighlightingWithGivenActor< ActorXYZ >(
        temp_actor );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
VisibilityXYZ()
{
  vtkActor* temp_actor = m_ImageView->GetCurrentActor();
  if( temp_actor )
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
std::list<int>
QGoTabImageView3DwT::
SelectTraceInTable(ContourMeshStructureMultiIndexContainer& iContainer,
                   std::list<vtkProp3D*>                  iActorList)
{
  std::list<int>                  listofrowstobeselected;
  std::list<vtkProp3D*>::iterator it = iActorList.begin();

  vtkActor* t_actor( NULL );

  while (it != iActorList.end())
    {
    t_actor = static_cast<vtkActor*>( *it );

//    // Change the corresponding highlighted value in the container
//    ContourMeshStructureMultiIndexContainer::index<Actor>::type::iterator
//    actor_it = iContainer.get<Actor>().find();
//
//    if (actor_it != iContainer.get<Actor>().end())
//      {
//      if (actor_it->TCoord ==  static_cast<unsigned int>(m_TCoord))
//        {
//        int trace_id = static_cast<int>(actor_it->TraceID);
//        listofrowstobeselected.push_back(trace_id);
//        }
//      }
//    ++it;
    }

  return listofrowstobeselected;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void
QGoTabImageView3DwT::
DeleteTracesFromTableManager(const std::list<int>& iList)
{
  // In which table are we?
  std::string currentTrace = this->m_DataBaseTables->InWhichTableAreWe();

  // If we are in contour
  if (currentTrace.compare("contour") == 0)
    {
    DeleteTracesFromTable(m_ContourContainer, iList);
    }
  // If we are in mesh
  if (currentTrace.compare("mesh") == 0)
    {
    DeleteTracesFromTable(m_MeshContainer, iList);
    }

  // Update the visualization
  this->m_ImageView->UpdateRenderWindows();
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void
QGoTabImageView3DwT::
DeleteTracesFromTable(ContourMeshStructureMultiIndexContainer& iContainer,
                      const std::list<int>& iList)
{
  std::list<int>::const_iterator traceid_it = iList.begin();

  ContourMeshStructureMultiIndexContainer::index<TraceID>::type::iterator it;

  while (traceid_it != iList.end())
    {
    it = iContainer.get<TraceID>().find(*traceid_it);
    if (it != iContainer.get<TraceID>().end())
      {
      this->m_ImageView->RemoveActor( 0, it->ActorXY );
      this->m_ImageView->RemoveActor( 1, it->ActorXZ );
      this->m_ImageView->RemoveActor( 2, it->ActorYZ );
      this->m_ImageView->RemoveActor( 3, it->ActorXYZ );

      it->Nodes->Delete();
      it->ActorXY->Delete();
      it->ActorXZ->Delete();
      it->ActorYZ->Delete();
      it->ActorXYZ->Delete();

      iContainer.get<TraceID>().erase(it);
      }
    ++traceid_it;
    }
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int
QGoTabImageView3DwT::
GetSliceViewXY() const
{
  return m_ImageView->GetSliceViewXY();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoTabImageView3DwT::
GetSliceViewXZ() const
{
  return m_ImageView->GetSliceViewXZ();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoTabImageView3DwT::
GetSliceViewYZ() const
{
  return m_ImageView->GetSliceViewYZ();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoTabImageView3DwT::
GetTimePoint() const
{
  return m_TCoord;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::AddBookmark()
{
  this->m_DataBaseTables->AddBookmark(this->GetSliceViewYZ(),
                                      this->GetSliceViewXZ(),
                                      this->GetSliceViewXY(),
                                      this->GetTimePoint());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
Change3DPerspectiveToAxial()
{
  m_ImageView->SetCamera(1);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
Change3DPerspectiveToCoronal()
{
  m_ImageView->SetCamera(2);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
Change3DPerspectiveToSagittal()
{
  m_ImageView->SetCamera(3);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetSliceView()
{
#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)
  m_VideoRecorderWidget->SetCurrentX(this->GetSliceViewYZ());
  m_VideoRecorderWidget->SetCurrentY(this->GetSliceViewXZ());
  m_VideoRecorderWidget->SetCurrentZ(this->GetSliceViewXY());
  m_VideoRecorderWidget->SetCurrentT(this->GetTimePoint());
#endif
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int
QGoTabImageView3DwT::
SaveAndVisuContour(vtkPolyData* iView)
{
  if(!m_DataBaseTables->IsDatabaseUsed())
    {
    std::cerr << "Problem with DB" << std::endl;
    return -1;
    }

  // if no data, we are using the contour widget
  if( !iView )
    {
    std::cerr << "Input contour is NULL" <<std::endl;
    return -1;
    }

  vtkPolyData* contour_nodes = vtkPolyData::New();
  CreateContour( contour_nodes, iView );

  SaveContour(iView, contour_nodes);

  std::vector<vtkActor*> actors = VisualizeContour( iView );

  // update the container
  m_ContourContainer->UpdateCurrentElementFromVisu(actors,
                                    contour_nodes,
                                    m_TCoord,
                                    false,//highlighted
                                    true);//visible

  m_ContourContainer->InsertCurrentElement();

  return 0;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateContour(vtkPolyData* contour_nodes, vtkPolyData* iView)
{
  //generate contour
  vtkOrientedGlyphContourRepresentation* contourRep =
    vtkOrientedGlyphContourRepresentation::New();

  vtkContourWidget* contourWidget = vtkContourWidget::New();

  contourWidget->SetInteractor(this->m_ImageView->GetImageViewer(0)
                               ->GetInteractor());
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
QGoTabImageView3DwT::
SaveMesh(vtkPolyData* iView )//, const int& iMeshID, double iRgba[4], bool NewMesh)
{
  // Compute Bounding Box
  int* bounds = GetBoundingBox(iView);

  // Save mesh in database
  //don't use m_ContourId
  GoFigureMeshAttributes MeshAttributes = ComputeMeshAttributes(iView);

  //if the mesh needs to be updated in the Database, the NewMesh will be false
  // and the iMeshID will be = 0 if the meshID needs to be gotten from the
  // trace manual editing widget, if it is a new mesh, the NewMesh will be
  // true and iMeshID = 0:
  //IDWithColorData MeshData =
  this->m_DataBaseTables->SaveMeshFromVisuInDB( bounds[0], bounds[2], bounds[4],
                                              m_TCoord,
                                              bounds[1], bounds[3], bounds[5],
                                              iView,
                                              &MeshAttributes);

  //delete bounds;
  //return MeshData;

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SaveAndVisuMesh(vtkPolyData* iView)
{
  SaveAndVisuMesh(iView, m_TCoord);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SaveAndVisuMesh(vtkPolyData* iView, unsigned int iTCoord)
{
  if(!m_DataBaseTables->IsDatabaseUsed())
    {
    std::cerr << "Problem with DB" << std::endl;
    return;
    }

  if( !iView )
    {
    std::cerr << "Input Mesh is NULL" << std::endl;
    return;
    }

  SaveMesh( iView );

  std::vector<vtkActor*> actors =  VisualizeMesh( iView );

  // update container since a new mesh is created
  m_MeshContainer->UpdateCurrentElementFromVisu( actors,
                                                 iView,
                                                 iTCoord,
                                                 false, // highlighted
                                                 true ); // visible
  m_MeshContainer->InsertCurrentElement();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateEmptyMesh()
{
  m_DataBaseTables->SaveNewMeshWithNoPointsInDB();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AddContourToCurrentMesh(vtkPolyData* iInput)
{
  //IDWithColorData ContourData = IDWithColorData(-1, QColor(Qt::white));
  if ((iInput->GetNumberOfPoints() > 2) && (m_TCoord >= 0))
    {
    // Compute Bounding Box
    int* bounds = GetBoundingBox(iInput);
    // Save contour in database :
    this->m_DataBaseTables
      ->SaveNewContourForContoursToSphere( bounds[0],
                                           bounds[2],
                                           bounds[4],
                                           bounds[1],
                                           bounds[3],
                                           bounds[5],
                                           iInput,
                                           m_MeshContainer->m_CurrentElement.TraceID
                                           );

    // AND VISU!!!
    std::vector<vtkActor*> actors = VisualizeContour( iInput );

    // update the container
    m_ContourContainer->UpdateCurrentElementFromVisu( actors,
                                                      iInput,
                                                      m_TCoord,
                                                      false,//highlighted
                                                      true);//visible

    m_ContourContainer->InsertCurrentElement();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::ShowTraceDockWidgetForContour(
  bool ManualSegVisible)
{
  if (ManualSegVisible)
    {
    if (this->m_DataBaseTables->IsDatabaseUsed())
      {
      this->m_DataBaseTables->UpdateWidgetsForCorrespondingTrace("contour","mesh");
      }
    }
  else
    {
    if(m_DataBaseTables->toggleViewAction()->isChecked())
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
  if (MeshSegmentationVisible)
    {
    if (this->m_DataBaseTables->IsDatabaseUsed())
      {
      this->m_DataBaseTables->UpdateWidgetsForCorrespondingTrace("mesh","track");
      }
    }
  else
    {
    if(m_DataBaseTables->toggleViewAction()->isChecked())
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
  if(iEnable)
    {
    this->m_ModeActions.at(0)->setChecked(true);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoFigureMeshAttributes
QGoTabImageView3DwT::
ComputeMeshAttributes(vtkPolyData* iMesh)
{
  typedef unsigned char PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image<PixelType, Dimension> ImageType;

  itk::vtkPolyDataToGoFigureMeshAttributes<ImageType>::Pointer
  calculator = itk::vtkPolyDataToGoFigureMeshAttributes<ImageType>::New();
  calculator->SetPolyData(iMesh);

  GoFigureMeshAttributes oAttributes;

  for (size_t i = 0; i < m_InternalImages.size(); i++)
    {
    vtkSmartPointer<vtkImageExport> vtk_exporter =
      vtkSmartPointer<vtkImageExport>::New();
    itk::VTKImageImport<ImageType>::Pointer itk_importer =
      itk::VTKImageImport<ImageType>::New();
    vtk_exporter->SetInput(m_InternalImages[i]);

    ConnectPipelines<vtkImageExport, itk::VTKImageImport<ImageType>::Pointer>(
        vtk_exporter, itk_importer );
    calculator->SetImage(itk_importer->GetOutput());
    calculator->Update();

    QString     q_channelname = this->m_NavigationDockWidget->GetChannelName(i);
    std::string channelname = q_channelname.toStdString();

    oAttributes.m_TotalIntensityMap[channelname] =
        static_cast<int>(calculator->GetSumIntensity());
    oAttributes.m_MeanIntensityMap[channelname] = calculator->GetMeanIntensity();
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
  if (this->m_DataBaseTables->IsDatabaseUsed())
    {
    this->m_DataBaseTables->ImportContours(this->m_TCoord);

    this->GoToDefaultMenu();

    //as in the import contours file, there are data such as colors,celltype
    //and subcelltype, the lists may have been updated in the database:
    this->m_DataBaseTables->InitializeTheComboboxesNotTraceRelated();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*
void
QGoTabImageView3DwT::
ChangeColorOfSelectedTraces(ContourMeshStructureMultiIndexContainer& ioContainer,
                            ContourMeshStructureMultiIndexContainer* iTbContainer,
                            QColor iSelectedColor)
{
  ContourMeshStructureMultiIndexContainer::iterator
    it = iTbContainer->begin();

  unsigned int trace_id = 0;
  double       rgba[4];
  rgba[0] = iSelectedColor.redF();
  rgba[1] = iSelectedColor.greenF();
  rgba[2] = iSelectedColor.blueF();
  rgba[3] = iSelectedColor.alphaF();

  while (it != iTbContainer->end())
    {
    trace_id = it->TraceID;

    ContourMeshStructureMultiIndexContainer::index<TraceID>::type::iterator
      traceid_it = ioContainer.get<TraceID>().find(trace_id);

    if( traceid_it != ioContainer.get<TraceID>().end() )
      {
      if (it->Highlighted)
        {
        ContourMeshStructure temp(*traceid_it);
        temp.Highlighted = it->Highlighted;
        temp.rgba[0] = rgba[0];
        temp.rgba[1] = rgba[1];
        temp.rgba[2] = rgba[2];
        temp.rgba[3] = rgba[3];
        ioContainer.get<TraceID>().replace(traceid_it, temp);
        }
      }
    ++it;
    }
}*/
//-------------------------------------------------------------------------

/// TODO Bug iMeshID is not the one it is supposed to be
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateMeshFromSelectedContours(
    std::list<unsigned int> iListContourIDs,
    int iMeshID )
{
  std::list<unsigned int>::iterator contourid_it = iListContourIDs.begin();

  std::vector<vtkPolyData*> list_contours;

  // get the time point
  unsigned int tcoord;

  while (contourid_it != iListContourIDs.end())
    {
    ContourMeshContainer::MultiIndexContainerTraceIDIterator
      traceid_it = m_ContourContainer->m_Container.get< TraceID >().find( *contourid_it );

    if( traceid_it != m_ContourContainer->m_Container.get< TraceID >().end() )
      {
      tcoord = traceid_it->TCoord;

      list_contours.push_back(
          vtkPolyData::SafeDownCast(
              traceid_it->ActorXYZ->GetMapper()->GetInput() ) );
      }
    ++contourid_it;
    }

  if( !list_contours.empty() )
    {
    // make the mesh from the list of contours
    typedef itk::ContourToMeshFilter<std::vector<vtkPolyData*> > FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->ProcessContours(list_contours);

    //as the element is already in the container we need to delete it in order
    //to update it in the SaveAndVisuMesh:
    this->m_MeshContainer->RemoveElementFromVisualizationWithGivenTraceID(iMeshID);
    SaveAndVisuMesh( filter->GetOutput(), tcoord);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::ImportMeshes()
{
  if (this->m_DataBaseTables->IsDatabaseUsed())
    {
    m_DataBaseTables->ImportMeshes(this->m_MeshContainer,this->m_TCoord );

    GoToDefaultMenu();

    this->m_DataBaseTables->InitializeTheComboboxesNotTraceRelated();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
GoToLocation(int iX, int iY, int iZ, int iT)
{
  this->SetTimePoint(iT);
  this->SetSliceViewXY(iZ);
  this->SetSliceViewXZ(iY);
  this->SetSliceViewYZ(iX);
}
//-------------------------------------------------------------------------
