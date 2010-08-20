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
#include "QGoManualSegmentationDockWidget.h"
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

#include "QGoOneClickSegmentationDockWidget.h"
#include "vtkViewImage2D.h"
#include "vtkCellArray.h"
#include "vtkMath.h"
#include "vtkPolyData.h"

#include "vtkPolyDataMapper.h"
#include "vtkOutlineFilter.h"

//VTK FILTERS
#include "vtkImageExport.h"

// VTK/ITK
#include "vtkitkAdaptor.h"
#include "itkImage.h"
#include "itkVTKImageImport.h"
#include "itkVTKImageToImageFilter.h"

//ITK FILTERS
#include "itkvtkPolyDataToGoFigureMeshAttributes.h"
#include "ContourToMeshFilter.h"

#include "GoFigureMeshAttributes.h"
#include "QGoManualSegmentationSettingsDialog.h"
#include "QGoTraceManualEditingWidget.h"
#include "QGoSeedsSegmentation.h"

#include <QCursor>

#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QColorDialog>

#include <set>

#include <QDebug>
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
  m_ContourId(0),
  m_ReEditContourMode(false)
  {
  m_Image = vtkImageData::New();

  m_LinesWidth = 1.;

  setupUi(this);

  m_MegaCaptureReader = itk::MegaCaptureReader::New();

  CreateVisuDockWidget();

  //CreateSettingAndDialogSegmentationWidgets();

  m_SegmentationOutput = vtkPolyData::New();
  m_SeedsSegmentation = new QGoSeedsSegmentation(0, m_ImageView->GetImageViewer(0),
      m_ImageView->GetAllSeeds(), m_InternalImages, m_SegmentationOutput);

  CreateManualSegmentationdockWidget();

  CreateOneClickSegmentationDockWidget();

  CreateDataBaseTablesConnection();

#if defined (ENABLEFFMPEG) || defined (ENABLEAVI)
  CreateVideoRecorderWidget();
#endif

  CreateAllViewActions();

  CreateToolsActions();

  CreateBookmarkActions();

  CreateModeActions();

  ReadSettings();

  // Generate default color, width and nodes for the contours visualization
  GenerateContourRepresentationProperties();

  m_DockWidgetList.push_back(
    std::pair<QGoDockWidgetStatus*, QDockWidget*>(
      new QGoDockWidgetStatus(m_NavigationDockWidget, Qt::RightDockWidgetArea, true, true),
      m_NavigationDockWidget));

  m_DockWidgetList.push_back(
    std::pair<QGoDockWidgetStatus*, QDockWidget*>(
      new QGoDockWidgetStatus(m_ManualSegmentationDockWidget, Qt::LeftDockWidgetArea, true, true),
      m_ManualSegmentationDockWidget));

//   m_DockWidgetList.push_back(
//     std::pair< QGoDockWidgetStatus*, QDockWidget* >(
//       new QGoDockWidgetStatus( m_DataBaseTables, Qt::TopDockWidgetArea, true, true ),
//       m_DataBaseTables ) );

  m_DockWidgetList.push_back(
    std::pair<QGoDockWidgetStatus*, QDockWidget*>(
      new QGoDockWidgetStatus(m_OneClickSegmentationDockWidget, Qt::LeftDockWidgetArea, true, true),
      m_OneClickSegmentationDockWidget));

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
  m_Image->Delete();

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

  delete m_SeedsSegmentation;
  m_SegmentationOutput->Delete();
//   DeleteContourMeshStructureElement( m_ContourContainer );
//   DeleteContourMeshStructureElement( m_MeshContainer );
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void
QGoTabImageView3DwT::
CreateSettingAndDialogSegmentationWidgets()
{
  this->m_TraceManualEditingDockWidget =
    new QGoTraceManualEditingDockWidget(this);
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
GenerateContourRepresentationProperties()
{
  bool haschanged = false;

  double temp = m_ManualSegmentationDockWidget->m_SettingsDialog->GetLineWidth();

  if (m_LinesWidth != temp)
    {
    m_LinesWidth = temp;
    haschanged = true;
    }

  QColor temp_color = m_ManualSegmentationDockWidget->m_SettingsDialog->GetLineColor();

  if (m_LinesColor != temp_color)
    {
    m_LinesColor = temp_color;
    haschanged = true;
    }

  temp_color = m_ManualSegmentationDockWidget->m_SettingsDialog->GetNodeColor();

  if (m_NodesColor != temp_color)
    {
    m_NodesColor = temp_color;
    haschanged = true;
    }
  temp_color = m_ManualSegmentationDockWidget->m_SettingsDialog->GetActivatedNodeColor();

  if (m_ActiveNodesColor != temp_color)
    {
    m_ActiveNodesColor = temp_color;
    haschanged = true;
    }

  if (haschanged)
    {
    emit ContourRepresentationPropertiesChanged();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateManualSegmentationdockWidget()
{
  m_ManualSegmentationDockWidget = new QGoManualSegmentationDockWidget(this);
  m_ManualSegmentationDockWidget->setEnabled(false);

  // Manual segmentation

  QObject::connect(m_ManualSegmentationDockWidget, SIGNAL(ValidatePressed()),
                   this, SLOT(ValidateContour()));

  QObject::connect(m_ManualSegmentationDockWidget, SIGNAL(ReinitializePressed()),
                   this, SLOT(ReinitializeContour()));

  QObject::connect(m_ManualSegmentationDockWidget, SIGNAL(visibilityChanged(bool)),
                   this, SLOT (ShowTraceDockWidgetForContour(bool)));

  QObject::connect(m_ManualSegmentationDockWidget, SIGNAL(UpdateContourRepresentationProperties()),
                   this, SLOT(GenerateContourRepresentationProperties()));

  QObject::connect(this, SIGNAL(ContourRepresentationPropertiesChanged()),
                   this, SLOT(ChangeContourRepresentationProperty()));

  // Connect semi auto segmentation
  QObject::connect(m_ManualSegmentationDockWidget, SIGNAL(ApplyFilterPressed()),
                     m_SeedsSegmentation,
                     SLOT(ApplySeedContourSegmentationFilter()));

  // Cursor interaction
  QObject::connect(m_ManualSegmentationDockWidget, SIGNAL(UpdateInteractorBehavior(bool)),
                   this, SLOT(ContourInteractorBehavior(bool)));

  // Create action
  QAction* tempaction = m_ManualSegmentationDockWidget->toggleViewAction();

  this->m_SegmentationActions.push_back(tempaction);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ContourInteractorBehavior(bool iVisible)
{
  // Mode 0 = manual
  // Mode 1 = semi auto
  // Mode 2 = auto
  int mode = m_ManualSegmentationDockWidget->GetMode();
  switch (mode)
    {
    case 0:
      // Manual
      this->m_ImageView->EnableSeedWidget(!iVisible);
      this->m_ImageView->EnableContourWidget(iVisible);
      break;

    case 1:
      // Semi auto
      this->m_ImageView->EnableContourWidget(!iVisible);
      this->m_ImageView->EnableSeedWidget(iVisible);
      break;

    case 2:
      // Auto
      break;

    default:
      break;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
MeshInteractorBehavior(bool iVisible)
{
  this->m_ImageView->EnableSeedWidget(iVisible);
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
DefaultInteractorBehavior(bool iVisible)
{
  // if the widget is visible
  // check in which mode we are
  if (iVisible)
    {
    this->m_ImageView->DefaultMode();
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ZoomInteractorBehavior(bool iVisible)
{
  // if the widget is visible
  // check in which mode we are
  if (iVisible)
    {
    this->m_ImageView->ZoomMode();
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
PanInteractorBehavior(bool iVisible)
{
  // if the widget is visible
  // check in which mode we are
  if (iVisible)
    {
    this->m_ImageView->PanMode();
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ActorPickingInteractorBehavior(bool iVisible)
{
  // if the widget is visible
  // check in which mode we are
  if (iVisible)
    {
    this->m_ImageView->ActorPickingMode();
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
DistanceWidgetInteractorBehavior(bool iActive)
{
  this->m_ImageView->EnableDistanceWidget(iActive);
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AngleWidgetInteractorBehavior(bool iActive)
{
  this->m_ImageView->EnableAngleWidget(iActive);
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
Box3DPicking(bool iActive)
{
  this->m_ImageView->EnableBoxWidget(iActive);
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateOneClickSegmentationDockWidget()
{
  m_OneClickSegmentationDockWidget = new QGoOneClickSegmentationDockWidget(this);
  m_OneClickSegmentationDockWidget->setEnabled(false);

  QAction* tempaction = m_OneClickSegmentationDockWidget->toggleViewAction();

  this->m_SegmentationActions.push_back(tempaction);

  QObject::connect(m_OneClickSegmentationDockWidget, SIGNAL(visibilityChanged(bool)),
                   this, SLOT(ShowTraceDockWidgetForMesh(bool)));

  QObject::connect(m_OneClickSegmentationDockWidget, SIGNAL(ApplyFilterPressed()),
                   m_SeedsSegmentation,
                   SLOT(ApplySeedMeshSegmentationFilter()));

  QObject::connect(m_OneClickSegmentationDockWidget, SIGNAL(UpdateSegmentationMethod(int)),
                   m_SeedsSegmentation, SLOT(UpdateSegmentationMethod(int)));

  QObject::connect(m_OneClickSegmentationDockWidget, SIGNAL(RadiusChanged(double)),
                     m_SeedsSegmentation, SLOT(RadiusChanged(double)));

  QObject::connect(m_OneClickSegmentationDockWidget, SIGNAL(ChannelChanged(int)),
                     m_SeedsSegmentation, SLOT(ChannelChanged(int)));

  QObject::connect(m_OneClickSegmentationDockWidget, SIGNAL(NbOfIterationsChanged(int)),
                       m_SeedsSegmentation, SLOT(NbOfIterationsChanged(int)));

  QObject::connect(m_OneClickSegmentationDockWidget, SIGNAL(CurvatureWeightChanged(int)),
                       m_SeedsSegmentation, SLOT(CurvatureWeightChanged(int)));
  /// TODO CONNECT SIGNAL TO GET POLYDATA BACK
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateVisuDockWidget()
{
  /**
   * \todo update annotation on current slice (i.e. slice x/23) when moving through
   * x via this dockwidget
   */

  m_NavigationDockWidget = new QGoNavigationDockWidget(this, 4);

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
void
QGoTabImageView3DwT::
CreateDataBaseTablesConnection()
{
  QObject::connect(this,
                   SIGNAL(TimePointChanged(int)),
                   this->m_DataBaseTables,
                   SLOT(UpdateSelectedTimePoint(int)));

  QObject::connect(this->m_DataBaseTables,
                   SIGNAL(SelectionContoursToHighLightChanged()),
                   this, SLOT(HighLightTracesFromTableManager()));

  QObject::connect(this->m_DataBaseTables,
                   SIGNAL(SelectionMeshesToHighLightChanged()),
                   this, SLOT(HighLightTracesFromTableManager()));

  QObject::connect(this->m_DataBaseTables,
                   SIGNAL(TraceToReEdit(unsigned int)),
                   this, SLOT(ReEditContour(unsigned int)));

  QObject::connect(this->m_DataBaseTables,
                   SIGNAL(TracesToDeleteInVisu(std::list<int> )),
                   this, SLOT(DeleteTracesFromTableManager(std::list<int> )));

  QObject::connect(
    this->m_DataBaseTables, SIGNAL(TableWidgetTabChanged()),
    this, SLOT(GoToDefaultMenu()));

  QObject::connect(
    this->m_DataBaseTables, SIGNAL(ColorChangedForSelectedTraces(QColor)),
    this, SLOT(ChangeColorOfSelectedTracesManager(QColor)));

  QObject::connect(this->m_DataBaseTables, SIGNAL(NewMeshToGenerate(std::list<int>,int)),
                   this, SLOT(CreateMeshFromSelectedContours(std::list<int>,int)));

  QObject::connect(this->m_DataBaseTables, SIGNAL(NeedToGoToTheLocation(int, int, int, int)),
                   this, SLOT(GoToLocation(int, int, int, int)));

  QObject::connect(this->m_DataBaseTables, SIGNAL(ShowCheckedTracesActivated()),
                   this, SLOT(ChangeSelectedMeshesVisibility()));

  // Show/hide
  QObject::connect(this->m_DataBaseTables,
                   SIGNAL(SelectionContoursToShowChanged()),
                   this, SLOT(ModifyTracesVisibilityFromTableManager()));

  QObject::connect(this->m_DataBaseTables,
                   SIGNAL(SelectionMeshesToShowChanged()),
                   this, SLOT(ModifyTracesVisibilityFromTableManager()));
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

  QAction* LoadContoursPerTimePointAction =
    new QAction(tr("Load All Contours For Current Time Point"), this);
  this->m_ViewActions.push_back(LoadContoursPerTimePointAction);
  LoadContoursPerTimePointAction->setVisible(false);

  QObject::connect(LoadContoursPerTimePointAction, SIGNAL(triggered()),
                   this, SLOT(LoadAllTracesForCurrentTimePointManager()));

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
void QGoTabImageView3DwT::CreateModeActions()
{
  QActionGroup* group = new QActionGroup(this);

  // Call superclass
  QGoTabElementBase::CreateModeActions(group);

  //---------------------------------//
  //            Manual mode          //
  //---------------------------------//

  //////// Manual is becoming contour segmentation

  QAction* ManualEditingAction = new QAction(tr("Manual-Editing"), this);
  ManualEditingAction->setCheckable(true);
  QIcon ManualEditingIcon;
  ManualEditingIcon.addPixmap(QPixmap(QString::fromUtf8(":/fig/manual-editing.png")),
                              QIcon::Normal, QIcon::Off);
  ManualEditingAction->setIcon(ManualEditingIcon);

  group->addAction(ManualEditingAction);

  this->m_ModeActions.push_back(ManualEditingAction);

  // When click on button, it updates widget in visu
  QObject::connect(ManualEditingAction, SIGNAL(toggled(bool)),
                   this->m_ManualSegmentationDockWidget, SLOT(setEnabled(bool)));
  QObject::connect(ManualEditingAction, SIGNAL(toggled(bool)),
                   this->m_ManualSegmentationDockWidget, SLOT(setVisible(bool)));

  // it also updates the interactor behaviour
  QObject::connect(ManualEditingAction, SIGNAL(toggled(bool)),
                   this, SLOT(ContourInteractorBehavior(bool)));

  //---------------------------------//
  //          one click mode         //
  //---------------------------------//

  ////////  is becoming mesh segmentation

  // Create/initialize the manual editing action
  QAction* OneClickAction = new QAction(tr("One Click"), this);
  OneClickAction->setCheckable(true);
  OneClickAction->setChecked(false);

  // Definition of the associated icon
  QIcon OneClickIcon;
  OneClickIcon.addPixmap(QPixmap(QString::fromUtf8(":/fig/seedIcon.png")),
                         QIcon::Normal, QIcon::Off);
  OneClickAction->setIcon(OneClickIcon);

  // When click on button, it updates widget in visu
  QObject::connect(OneClickAction, SIGNAL(toggled(bool)),
                   m_OneClickSegmentationDockWidget, SLOT(setEnabled(bool)));
  QObject::connect(OneClickAction, SIGNAL(toggled(bool)),
                   m_OneClickSegmentationDockWidget, SLOT(setVisible(bool)));

  // it also updates the interactor behaviour
  QObject::connect(OneClickAction, SIGNAL(toggled(bool)),
                   this, SLOT(MeshInteractorBehavior(bool)));

  // Add the action to m_ModeActions and to group
  this->m_ModeActions.push_back(OneClickAction);
  group->addAction(OneClickAction);

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
  //         Distance    mode        //
  //---------------------------------//
  QAction* DistanceAction = new QAction(tr("Measure Distance"), this);

  DistanceAction->setCheckable(true);
  DistanceAction->setChecked(false);

  QIcon DistanceIcon;
  DistanceIcon.addPixmap(QPixmap(QString::fromUtf8(":/fig/Distance.png")),
                         QIcon::Normal, QIcon::Off);
  DistanceAction->setIcon(DistanceIcon);

  group->addAction(DistanceAction);

  this->m_ModeActions.push_back(DistanceAction);

  QObject::connect(DistanceAction, SIGNAL(toggled(bool)),
                   this, SLOT(DistanceWidgetInteractorBehavior(bool)));

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

  group->addAction(AngleAction);

  this->m_ModeActions.push_back(AngleAction);

  QObject::connect(AngleAction, SIGNAL(toggled(bool)),
                   this, SLOT(AngleWidgetInteractorBehavior(bool)));

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

  int t = atoi(Coord.GetMapValue("TCoord").c_str());
  int z = atoi(Coord.GetMapValue("TCoord").c_str());
  int y = atoi(Coord.GetMapValue("TCoord").c_str());
  int x = atoi(Coord.GetMapValue("TCoord").c_str());

  this->SetTimePoint( t );
  this->SetSliceViewXY( z );
  this->SetSliceViewXZ( y );
  this->SetSliceViewYZ( x );
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

  // connect the meshes selection connection
  QObject::connect(m_ImageView, SIGNAL(MeshesSelectionChanged()),
                   this, SLOT(HighlightMeshXYZ()));

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
    m_OneClickSegmentationDockWidget->SetNumberOfChannels(NumberOfChannels);
    m_ManualSegmentationDockWidget->SetNumberOfChannels(NumberOfChannels);

    if (NumberOfChannels > 1)
      {
      m_NavigationDockWidget->SetChannel(0);
      m_InternalImages.resize(NumberOfChannels);

      for (int i = 1; i < NumberOfChannels; i++)
        {
        m_NavigationDockWidget->SetChannel(i);

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
  m_OneClickSegmentationDockWidget->SetNumberOfChannels(NumberOfChannels);
  m_ManualSegmentationDockWidget->SetNumberOfChannels(NumberOfChannels);

  // Set up QSpinBox in m_VideoRecorderWidget
  if (NumberOfChannels > 1)
    {
    m_NavigationDockWidget->SetChannel(0);
    m_InternalImages.resize(NumberOfChannels, NULL);

    for (unsigned int i = 1; i < NumberOfChannels; i++)
      {
      m_NavigationDockWidget->SetChannel(i);
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
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  RemoveAllTracesForPresentTimePointManager();

  m_TCoord = iTimePoint;
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

  LoadAllTracesForCurrentTimePointManager();
  //this->ShowTraceDockWidgetForContour(false);
  Update();

  QApplication::restoreOverrideCursor();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetTimePointWithLSMReaders(const int& iTimePoint)
{
  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  RemoveAllTracesForPresentTimePointManager();

  m_TCoord = iTimePoint;
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

  LoadAllTracesForCurrentTimePointManager();
  Update();

  QApplication::restoreOverrideCursor();
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

  if (!m_LSMReader.empty())
    {
    if (iTimePoint >= m_LSMReader[0]->GetNumberOfTimePoints())
      {
      return;
      }
    else
      {
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
        SetTimePointWithMegaCapture(iTimePoint);
        emit TimePointChanged(m_TCoord);
        }
      }
    else
      {
      // no lsm reader, no file list. did you really provide any input?
      std::cerr << "No lsm reader. No file list" << std::endl;
      return;
      }
    }
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
QGoTabImageView3DwT::IDWithColorData
QGoTabImageView3DwT::
SaveContour(vtkPolyData* contour, vtkPolyData* contour_nodes)
{
  IDWithColorData ContourData = IDWithColorData(-1, QColor(Qt::white));

  if ((contour->GetNumberOfPoints() > 2) && (m_TCoord >= 0))
    {
    // Compute Bounding Box
    int* bounds = GetBoundingBox(contour);

    // Save contour in database!
    ContourData = m_DataBaseTables->SaveContoursFromVisuInDB(bounds[0],
                                                             bounds[1],
                                                             bounds[2],
                                                             m_TCoord,
                                                             bounds[3],
                                                             bounds[4],
                                                             bounds[5],
                                                             contour_nodes);
    delete bounds;
    }
  else
    {
    std::cerr << "(contour->GetNumberOfPoints() < 2) or  (m_TCoord < 0)" << std::endl;
    }
  return ContourData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTabImageView3DwT::IDWithColorData
QGoTabImageView3DwT::
UpdateContour(vtkPolyData* contour, vtkPolyData* contour_nodes)
{
  IDWithColorData ContourData( -1, QColor(Qt::white) );

  if ((contour->GetNumberOfPoints() > 2) && (m_TCoord >= 0))
    {
    // Compute Bounding Box
    int* bounds = GetBoundingBox(contour);

    /// TODO Fix bug here, returned color is wrong
    // update contour in database!
    ContourData = m_DataBaseTables->UpdateContourFromVisuInDB(
        bounds[0], bounds[2], bounds[4],
        m_TCoord,
        bounds[1], bounds[3], bounds[5], contour_nodes, m_ContourId);
    this->m_DataBaseTables->GetTraceManualEditingDockWidget()->setEnabled(true);

    delete bounds;
    }
  else
    {
    std::cerr << "(contour->GetNumberOfPoints() < 2) or  (m_TCoord < 0)" << std::endl;
    }
  return ContourData;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int*
QGoTabImageView3DwT::GetBoundingBox(vtkPolyData* iElement)
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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoTabImageView3DwT::
VisualizeContour(const int& iContourID, const unsigned int& iTCoord,
    vtkPolyData* contour, vtkPolyData* contour_nodes, const double iRGBA[4])
{
  if ((contour->GetNumberOfPoints() > 2) && (m_TCoord >= 0))
    {
    m_ContourId = iContourID;

    vtkProperty* contour_property = vtkProperty::New();

    contour_property->SetColor(iRGBA[0], iRGBA[1], iRGBA[2]);
    contour_property->SetOpacity(iRGBA[3]);

    /// TODO shallow copy...?
    // get corresponding actor from visualization
    vtkPolyData* contour_copy = vtkPolyData::New();
    contour_copy->ShallowCopy(contour);

    std::vector<vtkActor*> contour_actor =
      this->AddContour(contour_copy, contour_property);

    contour_property->Delete();

    // fill the container
    ContourMeshStructure temp( m_ContourId, contour_actor,
                               contour_nodes, iTCoord,
                               false, // highlighted
                               true, // Visible
                               iRGBA[0], iRGBA[1], iRGBA[2], iRGBA[3] );
    m_ContourContainer.insert(temp);

    // to increase accurately
    ++m_ContourId;
    }

  return iContourID;
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ValidateContour()
{
  // to make sure that m_ContourId is set to the right value
  int          ContourID = -1;
  IDWithColorData test;

  for ( int i = 0; i < m_ImageView->GetNumberOfImageViewers(); i++)
    {
    if( m_ReEditContourMode )
      {
      ContourID = m_ContourId;

      test = UpdateContour( m_ImageView->GetContourRepresentationAsPolydata(i),
                            m_ImageView->GetContourRepresentationNodePolydata(i));
      }
    else
      {
      test = SaveContour( m_ImageView->GetContourRepresentationAsPolydata(i),
                          m_ImageView->GetContourRepresentationNodePolydata(i) );
      ContourID = test.first;
      }


    double rgba[4] = {0., 0., 0., 0.};

    test.second.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);
    std::cout <<"RGBA: " << rgba[0] << " " << rgba[1] << " " << rgba[2] << std::endl;

    // visu
    VisualizeContour(ContourID,
                     m_TCoord,
                     m_ImageView->GetContourRepresentationAsPolydata(i),
                     m_ImageView->GetContourRepresentationNodePolydata(i),
                     rgba);
    }

  if (m_ReEditContourMode)
    {
    std::list<int> listofrowstobeselected;
    listofrowstobeselected.push_back(m_ContourId);

    m_DataBaseTables->ChangeContoursToHighLightInfoFromVisu(listofrowstobeselected,
                                                            m_ReEditContourMode);
    m_ReEditContourMode = false;
    }
}

/// TODO temporary classes
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ReinitializeContour()
{
  m_ImageView->ReinitializeContourWidget();
}

//-------------------------------------------------------------------------
/// TODO temporary class
void
QGoTabImageView3DwT::
ChangeContourRepresentationProperty()
{
  m_ImageView->UpdateContourRepresentationProperties(
      static_cast<float>(m_LinesWidth),
      m_LinesColor, m_NodesColor, m_ActiveNodesColor);
}

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
void
QGoTabImageView3DwT::
RemoveAllTracesForPresentTimePointManager()
{
  if (m_TCoord >= 0)
    {
    // Removes contours from visu
    RemoveAllTracesForGivenTimePoint(m_TCoord, m_ContourContainer);
    // Removes meshes from visu
    RemoveAllTracesForGivenTimePoint(m_TCoord, m_MeshContainer);
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
RemoveAllTracesForGivenTimePoint(const unsigned int& iT,
                                 const std::string& iTraceName)
{
  if (iTraceName.compare("contour") == 0)
    {
    // Removes contours from visu
    RemoveAllTracesForGivenTimePoint(iT, m_ContourContainer);
    }
  if (iTraceName.compare("mesh") == 0)
    {
    // Removes meshes from visu
    RemoveAllTracesForGivenTimePoint(iT, m_MeshContainer);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
RemoveAllTracesForGivenTimePoint(const unsigned int& iT,
                                 ContourMeshStructureMultiIndexContainer& iContainer)
{
  if (iContainer.size() > 0)
    {
    ContourMeshStructureMultiIndexContainer::index<TCoord>::type::iterator
        it0, it1;

    boost::tuples::tie(it0, it1) =
      iContainer.get<TCoord>().equal_range(iT);

    while (it0 != it1)
      {
      this->m_ImageView->RemoveActor( 0, it0->ActorXY );
      this->m_ImageView->RemoveActor( 1, it0->ActorXZ );
      this->m_ImageView->RemoveActor( 2, it0->ActorYZ );
      this->m_ImageView->RemoveActor( 3, it0->ActorXYZ );
      ++it0;
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
LoadAllTracesForCurrentTimePointManager()
{
  if (m_TCoord >= 0)
    {
    // Loads contours for visu
    LoadAllTracesForGivenTimePoint(m_TCoord, m_ContourContainer);
    // Loads meshes for visu
    LoadAllTracesForGivenTimePoint(m_TCoord, m_MeshContainer);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
LoadAllTracesForGivenTimePoint(const unsigned int& iT,
                               ContourMeshStructureMultiIndexContainer& iContainer)
{
  if (iContainer.size() > 0)
    {
    ContourMeshStructureMultiIndexContainer::index<TCoord>::type::iterator
        it0, it1;

    boost::tuples::tie(it0, it1) =
      iContainer.get<TCoord>().equal_range(iT);

    while (it0 != it1)
      {
      this->m_ImageView->AddActor( 0, it0->ActorXY );
      this->m_ImageView->AddActor( 1, it0->ActorXZ );
      this->m_ImageView->AddActor( 2, it0->ActorYZ );
      this->m_ImageView->AddActor( 3, it0->ActorXYZ );

      ++it0;
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AddTraceFromNodesManager(const unsigned int& iContourID,
                         vtkPolyData* iNodes,
                         double iRgba[4],
                         const unsigned int& iTCoord,
                         std::string iTrace)
{
  // If we want to add a contour
  if (iTrace.compare("contour") == 0)
    {
    AddContourFromNodes(iContourID, iNodes, iRgba, iTCoord);
    }
  // If we want to add a mesh
  if (iTrace.compare("mesh") == 0)
    {
    AddMeshFromNodes(iContourID, iNodes, iRgba, iTCoord);
    }
}


//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AddContourFromNodes(const unsigned int& iContourID, vtkPolyData* iNodes,
                    const double iRgba[4], const unsigned int& iTCoord)
{
  if (iNodes->GetNumberOfPoints() > 2)
    {
    int dir = ComputeDirectionFromContour(iNodes);

    if (dir != -1)
      {
      m_ImageView->EnableContourWidget( true );
      m_ImageView->InitializeContourWidgetNodes( dir, iNodes );
      VisualizeContour(iContourID, iTCoord,
                      m_ImageView->GetContourRepresentationAsPolydata(dir),
                      m_ImageView->GetContourRepresentationNodePolydata(dir),
                      iRgba);
       m_ImageView->ReinitializeContourWidget();
       m_ImageView->EnableContourWidget( false );
      }
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AddMeshFromNodes(const unsigned int& iMeshID, vtkPolyData* iNodes,
                 const double iRgba[4],
                 const unsigned int& iTCoord)
{
  this->VisualizeMesh(iNodes, iMeshID, iTCoord, iRgba);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ReEditContour(const unsigned int& iId)
{
  if (!m_ContourContainer.empty())
    {
    ContourMeshStructureMultiIndexContainer::index<TraceID>::type::iterator
    it = m_ContourContainer.get<TraceID>().find(iId);

    if (it != m_ContourContainer.get<TraceID>().end())
      {
      vtkPolyData* nodes = it->Nodes;
      int* bounds = this->GetBoundingBox( nodes );

      int dir = ComputeDirectionFromBounds<int>( bounds );

      if( dir != -1 )
        {
        this->m_ReEditContourMode = true;
        this->m_ContourId = iId;

        this->m_ImageView->RemoveActor( 0, it->ActorXY );
        this->m_ImageView->RemoveActor( 1, it->ActorXZ );
        this->m_ImageView->RemoveActor( 2, it->ActorYZ );
        this->m_ImageView->RemoveActor( 3, it->ActorXYZ );

        m_ContourContainer.get<TraceID>().erase(it);

        int idx[3];
        idx[0] = ( bounds[0] + bounds[1] ) / 2;
        idx[1] = ( bounds[2] + bounds[3] ) / 2;
        idx[2] = ( bounds[4] + bounds[5] ) / 2;

        this->GoToLocation( idx[0], idx[1], idx[2], m_TCoord );

        this->m_ManualSegmentationDockWidget->show();
        this->m_ModeActions[0]->setChecked(true);

        m_ImageView->InitializeContourWidgetNodes( dir , nodes );

        m_ManualSegmentationDockWidget->setEnabled(true);
        this->m_DataBaseTables->GetTraceManualEditingDockWidget()->setEnabled(false);
        }

      delete bounds;
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighlightXY()
{
  HighLightActorsInContainer< ActorXY >( m_ContourContainer,
          m_ImageView->GetCurrentActor() );
  HighLightActorsInContainer< ActorXY >( m_MeshContainer,
            m_ImageView->GetCurrentActor() );
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighlightXZ()
{
  HighLightActorsInContainer< ActorXZ >( m_ContourContainer,
            m_ImageView->GetCurrentActor() );
  HighLightActorsInContainer< ActorXZ >( m_MeshContainer,
            m_ImageView->GetCurrentActor() );
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighlightYZ()
{
  HighLightActorsInContainer< ActorYZ >( m_ContourContainer,
            m_ImageView->GetCurrentActor() );
  HighLightActorsInContainer< ActorYZ >( m_MeshContainer,
            m_ImageView->GetCurrentActor() );
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighlightXYZ()
{
  HighLightActorsInContainer< ActorXYZ >( m_ContourContainer,
            m_ImageView->GetCurrentActor() );
  HighLightActorsInContainer< ActorXYZ >( m_MeshContainer,
            m_ImageView->GetCurrentActor() );
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighlightContoursXY()
{
  HighLightContours<ActorXY>();
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighlightContoursXZ()
{

  HighLightContours<ActorXZ>();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighlightContoursYZ()
{
  HighLightContours<ActorYZ>();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighlightMeshXYZ()
{
  // Fill container + send sth to visu
  /// Modifiy "picked" to "modified"
  HighLightMeshes<ActorXYZ>();
  // Fill table widget
  //ListSelectMeshesInTable();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighLightTracesFromTableManager()
{
  // In which table are we?
  std::string currentTrace = this->m_DataBaseTables->InWhichTableAreWe();

  // If we are in contour
  if (currentTrace.compare("contour") == 0)
    {
    HighLightTracesFromTable(m_ContourContainer, currentTrace);
    }
  // If we are in mesh
  if (currentTrace.compare("mesh") == 0)
    {
    HighLightTracesFromTable(m_MeshContainer, currentTrace);
    }
}
//-------------------------------------------------------------------------

// Maybe we should add the "highlight property" as a parameter

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighLightTracesFromTable(ContourMeshStructureMultiIndexContainer& iContainer,
                         std::string iCurrentTrace)
{
  ContourMeshStructureMultiIndexContainer::iterator
  it = this->m_DataBaseTables->GetTracesInfoListForVisu(iCurrentTrace.c_str())
       ->begin();
  unsigned int trace_id = 0;

  vtkProperty* select_property = vtkProperty::New();
  select_property->SetColor(1., 0., 0.);
  select_property->SetLineWidth(3.);
  select_property->SetOpacity(1.);

  ContourMeshStructure tempStructure;
  ContourMeshStructureMultiIndexContainer::index<TraceID>::type::iterator
  traceid_it;
  ContourMeshStructureMultiIndexContainer::index<TraceID>::type::iterator
  test_it = iContainer.get<TraceID>().end();

  while (it != this->m_DataBaseTables
         ->GetTracesInfoListForVisu(iCurrentTrace.c_str())->end())
    {
    trace_id = it->TraceID;

    traceid_it = iContainer.get<TraceID>().find(trace_id);

    if( traceid_it != iContainer.get<TraceID>().end() )
      {
      if (it->Highlighted != traceid_it->Highlighted)
        {
        if (!it->Highlighted)
          {
          vtkProperty* temp_property = vtkProperty::New();
          temp_property->SetColor( traceid_it->rgba[0],
                                   traceid_it->rgba[1],
                                   traceid_it->rgba[2]);
          temp_property->SetOpacity( traceid_it->rgba[3] );
          temp_property->SetLineWidth(1.);

          m_ImageView->ChangeActorProperty( 0,
                                            traceid_it->ActorXY,
                                            temp_property);
          m_ImageView->ChangeActorProperty( 1,
                                            traceid_it->ActorXZ,
                                            temp_property);
          m_ImageView->ChangeActorProperty( 2,
                                            traceid_it->ActorYZ,
                                            temp_property);
          m_ImageView->ChangeActorProperty( 3,
                                            traceid_it->ActorXYZ,
                                            temp_property);
          temp_property->Delete();
          }
        else
          {
          m_ImageView->ChangeActorProperty( 0,
                                            traceid_it->ActorXY,
                                            select_property);
          m_ImageView->ChangeActorProperty( 1,
                                            traceid_it->ActorXZ,
                                            select_property);
          m_ImageView->ChangeActorProperty( 2,
                                            traceid_it->ActorYZ,
                                            select_property);
          m_ImageView->ChangeActorProperty( 3,
                                            traceid_it->ActorXYZ,
                                            select_property);
          }
        tempStructure = *traceid_it;
        tempStructure.Highlighted = it->Highlighted;

        iContainer.get<TraceID>().replace(traceid_it, tempStructure);
        }
      }

    ++it;
    }

  select_property->Delete();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SelectContoursInTable()
{
  if (this->m_DataBaseTables->IsDatabaseUsed())
    {
  /*
    std::list<vtkProp3D*> listofpicked = m_ImageView->GetListOfPickedContours();

    if (!listofpicked.empty())
      {
      this->m_DataBaseTables->ChangeContoursToHighLightInfoFromVisu(
        SelectTraceInTable(m_ContourContainer, listofpicked), false);
      }*/
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ListSelectMeshesInTable()
{
  /// TODO Fix bug here
  /*
  if (this->m_DataBaseTables->IsDatabaseUsed())
    {
    std::list<vtkProp3D*> listofpicked = m_ImageView->GetListOfModifiedActors3D();

    if (!listofpicked.empty())
      {
      this->m_DataBaseTables->ChangeMeshesToHighLightInfoFromVisu(
        SelectTraceInTable(m_MeshContainer, listofpicked));
      }
    }
    */
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
void
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
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
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
}
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
QGoManualSegmentationDockWidget*
QGoTabImageView3DwT::
GetManualSegmentationWidget()
{
  return this->m_ManualSegmentationDockWidget;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
MeshSphereContours(QGoSeedsSegmentation iSeedsSegmentation)
{
  std::vector<vtkSmartPointer<vtkPolyData> > ContoursForOnePoint;
  std::list<int> listContoursIDs;

  // Each segmentation metho returns the appropriate output
  ContoursForOnePoint = iSeedsSegmentation.SphereContoursSegmentation();

  // Save polydatas (=contours) in DB and update visualization
  for (unsigned int j = 1; j < ContoursForOnePoint.size(); j++)
    {
    //save in db
    listContoursIDs.push_back(SaveAndVisuContour(ContoursForOnePoint[j]));
    //update visu
    }

  // Create new mesh for the new contours
  m_DataBaseTables->CreateMeshFromOneClickSegmentation(listContoursIDs);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
InitializeSeedSegmentationFilter(QGoSeedsSegmentation& ioSeedsSegmentation,
    double iRadius, vtkImageData* inputVolume, int iNbOfIterations,
    int iCurvatureWeight, vtkViewImage2D* iInformations)
{
  ioSeedsSegmentation.setRadius(iRadius);
  ioSeedsSegmentation.setInputVolume(inputVolume);
  ioSeedsSegmentation.setNumberOfIterations(iNbOfIterations);
  ioSeedsSegmentation.setCurvatureWeight(iCurvatureWeight);
  ioSeedsSegmentation.setOriginImageInformation(iInformations);
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
    }
  // Create Node for the associated contour (useful for reedit mode)
  vtkPolyData* contour_nodes = vtkPolyData::New();
  CreateContour( contour_nodes, iView );

  // Check if there is a DB cause now we assume there is one
  // Save in DB
  IDWithColorData test =  SaveContour(iView, contour_nodes);

  double rgba[4] = {0., 0., 0., 0.};
  int ID = test.first;
  test.second.getRgbF(&rgba[0], &rgba[1], &rgba[2], &rgba[3]);
  // visu
  return VisualizeContour(ID, m_TCoord, iView, contour_nodes, rgba);
}

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
QGoTabImageView3DwT::IDWithColorData
QGoTabImageView3DwT::
SaveMesh(vtkPolyData* iView, const int& iMeshID, double iRgba[4], bool NewMesh)
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
  IDWithColorData MeshData =
      m_DataBaseTables->SaveMeshFromVisuInDB( bounds[0], bounds[2], bounds[4],
                                              m_TCoord,
                                              bounds[1], bounds[3], bounds[5],
                                              iView,
                                              &MeshAttributes,
                                              NewMesh,
                                              iMeshID);
  delete bounds;

  return MeshData;
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
VisualizeMesh(vtkPolyData* iView, const int& iMeshID,
              const unsigned int& iTCoord, const double iRgba[4])
{
  vtkProperty* mesh_property = vtkProperty::New();
  mesh_property->SetColor(iRgba[0], iRgba[1], iRgba[2]);
  mesh_property->SetOpacity(iRgba[3]);

  /// TODO fix bug, shouldn't be required
  std::vector<vtkActor*> mesh_actor;
  if (iView)
    {
    mesh_actor = this->AddContour(iView, mesh_property);
    }
  mesh_property->Delete();

  // get meshid from the visu dock widget (SpinBox)
  /// \todo: this is not the track id when creating
  //mesh from the context menu: this is the selected mesh in the combolist !!!
  //unsigned int trackid =
  // this->m_TraceManualEditingDockWidget->m_TraceWidget->GetCurrentCollectionID();

  // fill the container
  ContourMeshStructure temp( iMeshID, mesh_actor, iView,
                             iTCoord,
                             false, //highlighted
                             true, // visible
                             iRgba[0], iRgba[1], iRgba[2], iRgba[3]);
  m_MeshContainer.insert(temp);
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SaveAndVisuMesh(vtkPolyData* iView)
{
  if(!m_DataBaseTables->IsDatabaseUsed())
    {
    std::cerr << "Problem with DB" << std::endl;
    }
  double RGBA[4] = {0., 0., 0., 0.};
  int iID(0);
  // save in DB
  IDWithColorData MeshData = SaveMesh(iView, -1,RGBA, true);
  iID = MeshData.first;
  MeshData.second.getRgbF(&RGBA[0],&RGBA[1],&RGBA[2],&RGBA[3]);
  // visu
  VisualizeMesh(iView, iID, m_TCoord ,RGBA);
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
    this->m_OneClickSegmentationDockWidget->setDisabled(true);
    this->m_OneClickSegmentationDockWidget->hide();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::ShowTraceDockWidgetForMesh(
  bool OneClickVisible)
{
  if (OneClickVisible)
    {
    if (this->m_DataBaseTables->IsDatabaseUsed())
      {
      this->m_DataBaseTables->UpdateWidgetsForCorrespondingTrace("mesh","track");
      }
    this->m_ManualSegmentationDockWidget->setDisabled(true);
    this->m_ManualSegmentationDockWidget->hide();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::GoToDefaultMenu()
{
  this->m_ManualSegmentationDockWidget->setDisabled(true);
  this->m_ManualSegmentationDockWidget->hide();
  this->m_OneClickSegmentationDockWidget->setDisabled(true);
  this->m_OneClickSegmentationDockWidget->hide();
  this->m_ModeActions.at(0)->setChecked(true);
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
    ContourMeshStructureMultiIndexContainer* ContourToAdd =
      this->m_DataBaseTables->ImportContours( this->m_TCoord );

    ContourMeshStructureMultiIndexContainer::iterator
        c_it = ContourToAdd->begin();

    while (c_it != ContourToAdd->end())
      {
      // NOT ENOUGH (ONLY VISU YET)
      this->AddContourFromNodes( c_it->TraceID, c_it->Nodes,
                                 c_it->rgba, c_it->TCoord );
      ++c_it;
      }
    this->GoToDefaultMenu();
    //as in the import contours file, there are data such as colors,celltype
    //and subcelltype, the lists may have been updated in the database:
    this->m_DataBaseTables->InitializeTheComboboxesNotTraceRelated();
    }
}
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ChangeColorOfSelectedTracesManager(QColor iSelectedColor)
  //std::pair<std::list<int>, QColor> iSelectedTraces)
{
  // In which table are we?
  std::string currentTrace = this->m_DataBaseTables->InWhichTableAreWe();

  ContourMeshStructureMultiIndexContainer* tb_container =
    this->m_DataBaseTables->GetTracesInfoListForVisu( currentTrace );
  //QColor      selectedColor = iSelectedTraces.second;

  // If we are in contour
  if (currentTrace.compare("contour") == 0)
    {
    ChangeColorOfSelectedTraces(m_ContourContainer, tb_container, iSelectedColor);
    }
  // If we are in mesh
  if (currentTrace.compare("mesh") == 0)
    {
    ChangeColorOfSelectedTraces(m_MeshContainer, tb_container, iSelectedColor);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
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
}
//-------------------------------------------------------------------------

/// TODO Bug iMeshID is not the one it is supposed to be
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateMeshFromSelectedContours(
    std::list<int> iListContourIDs,
    int iMeshID )
{
  std::list<int>::iterator contourid_it = iListContourIDs.begin();

  std::vector<vtkPolyData*> list_contours(iListContourIDs.size(), NULL);
  size_t                    id = 0;

  // get the time point
  unsigned int tcoord;

  while (contourid_it != iListContourIDs.end())
    {
    ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
      traceid_it = m_ContourContainer.get< TraceID >().find( *contourid_it );

    if( traceid_it != m_ContourContainer.get< TraceID >().end() )
      {
      tcoord = traceid_it->TCoord;
      list_contours[id++] =
          vtkPolyData::SafeDownCast(traceid_it->ActorXYZ->GetMapper()->GetInput());
      }
    ++contourid_it;
    }

  // make the mesh from the list of contours
  typedef itk::ContourToMeshFilter<std::vector<vtkPolyData*> > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->ProcessContours(list_contours);

  double RGBA[4] = {0., 0., 0., 0.};

  int iID(0);
  // save in DB
  IDWithColorData MeshData = this->SaveMesh(filter->GetOutput(), iMeshID, RGBA, false);
  iID = MeshData.first;
  MeshData.second.getRgbF(&RGBA[0],&RGBA[1],&RGBA[2],&RGBA[3]);
  // visu
  this->VisualizeMesh(filter->GetOutput(), iID, m_TCoord, RGBA);

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::ImportMeshes()
{
  if (this->m_DataBaseTables->IsDatabaseUsed())
    {
    ContourMeshStructureMultiIndexContainer* MeshToAdd =
      m_DataBaseTables->ImportMeshes( this->m_TCoord );

    ContourMeshStructureMultiIndexContainer::iterator c_it = MeshToAdd->begin();

    while (c_it != MeshToAdd->end())
      {
      // save + visu....
      // NOT ENOUGH ONLY VISU YET
      AddMeshFromNodes(c_it->TraceID, c_it->Nodes, c_it->rgba, c_it->TCoord);

      ++c_it;
      }
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

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ChangeSelectedMeshesVisibility()
{
  // In which table are we?
  std::string currentTrace = m_DataBaseTables->InWhichTableAreWe();

  ContourMeshStructureMultiIndexContainer* tb_container =
    this->m_DataBaseTables->GetTracesInfoListForVisu( currentTrace );

  // If we are in contour
  if (currentTrace.compare("contour") == 0)
    {
    ShowTracesFromTable(m_ContourContainer, tb_container);
    }
  // If we are in mesh
  if (currentTrace.compare("mesh") == 0)
    {
    ShowTracesFromTable(m_MeshContainer, tb_container);
    }

}
//-------------------------------------------------------------------------
// should update new container
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ShowTracesFromTable(ContourMeshStructureMultiIndexContainer& iContainer,
                    ContourMeshStructureMultiIndexContainer* iTbContainer )
{
  ContourMeshStructureMultiIndexContainer::iterator
      it = iTbContainer->begin();
  unsigned int trace_id = 0;
  vtkActor* tempActor = NULL;
  bool tempHighL = NULL;

  ContourMeshStructure  temp;

  ContourMeshStructureMultiIndexContainer::index<TraceID>::type::iterator
      traceid_it;

  while (it != iTbContainer->end())
    {
    trace_id = it->TraceID;
    tempHighL = it->Highlighted;

    traceid_it = iContainer.get<TraceID>().find(trace_id);

    if( traceid_it != iContainer.get<TraceID>().end() )
      {
      tempActor->SetVisibility(tempHighL);

      m_ImageView->ChangeActorProperty( 0,
                                        traceid_it->ActorXY,
                                        traceid_it->ActorXY->GetProperty() );

      m_ImageView->ChangeActorProperty( 1,
                                        traceid_it->ActorXZ,
                                        traceid_it->ActorXZ->GetProperty() );

      m_ImageView->ChangeActorProperty( 2,
                                        traceid_it->ActorYZ,
                                        traceid_it->ActorYZ->GetProperty() );

      m_ImageView->ChangeActorProperty( 3,
                                        traceid_it->ActorXYZ,
                                        traceid_it->ActorXYZ->GetProperty() );

      temp = *traceid_it;
      temp.Highlighted = tempHighL;

      iContainer.get<TraceID>().replace(traceid_it, temp);
      }
    ++it;
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ModifyTracesVisibilityFromTableManager()
{
  // In which table are we?
  std::string currentTrace = this->m_DataBaseTables->InWhichTableAreWe();

  ContourMeshStructureMultiIndexContainer* tb_container =
    this->m_DataBaseTables->GetTracesInfoListForVisu( currentTrace );

  // If we are in contour
  if (currentTrace.compare("contour") == 0)
    {
    ModifyTracesVisibilityFromTable(m_ContourContainer, tb_container);
    }
  // If we are in mesh
  if (currentTrace.compare("mesh") == 0)
    {
    ModifyTracesVisibilityFromTable(m_MeshContainer, tb_container);
    }
}
//-------------------------------------------------------------------------

// Maybe we should add the "highlight property" as a parameter

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ModifyTracesVisibilityFromTable(
    ContourMeshStructureMultiIndexContainer& iContainer,
    ContourMeshStructureMultiIndexContainer* iTbContainer )
{
  ContourMeshStructureMultiIndexContainer::iterator
  it = iTbContainer->begin();
  unsigned int trace_id = 0;

  ContourMeshStructure tempStructure;
  ContourMeshStructureMultiIndexContainer::index<TraceID>::type::iterator
  traceid_it;
  ContourMeshStructureMultiIndexContainer::index<TraceID>::type::iterator
  test_it;

  test_it = iContainer.get<TraceID>().end();

  while (it != iTbContainer->end())
    {
    trace_id = it->TraceID;

    traceid_it = iContainer.get<TraceID>().find(trace_id);

    if( traceid_it != iContainer.get<TraceID>().end() )
      {
      if (it->Visible != traceid_it->Visible)
        {
        traceid_it->ActorXY->SetVisibility(it->Visible);
        traceid_it->ActorXZ->SetVisibility(it->Visible);
        traceid_it->ActorYZ->SetVisibility(it->Visible);
        traceid_it->ActorXYZ->SetVisibility(it->Visible);

        tempStructure = *traceid_it;
        tempStructure.Visible = it->Visible;

        iContainer.get<TraceID>().replace(traceid_it, tempStructure);
        }
      }

    ++it;
    }

  // update view
  m_ImageView->UpdateRenderWindows();
}
