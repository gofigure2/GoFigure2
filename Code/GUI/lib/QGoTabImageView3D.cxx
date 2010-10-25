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

#include "QGoTabImageView3D.h"

#include "QGoImageView3D.h"
#include "QGoLUTDialog.h"
#include "QGoNavigationDockWidget.h"

#include "vtkImageData.h"
#include "vtkLookupTable.h"
#include "vtkContourWidget.h"
#include "vtkOrientedGlyphContourRepresentation.h"
#include "vtkImageActorPointPlacer.h"
#include "vtkProperty.h"
// #include "vtkQuadricLODActor.h"
#include "vtkActor.h"

#include "vtkImageExtractComponents.h"

#include <QLabel>
#include <QColorDialog>
#include <QDockWidget>
#include <QSpinBox>
#include <QVBoxLayout>

//--------------------------------------------------------------------------
/**
 * \brief Constructor
 * @param iParent
 */
QGoTabImageView3D::QGoTabImageView3D(QWidget *iParent):
  QGoTabImageViewNDBase(iParent)
{
  setupUi(this);

  for ( int i = 0; i < 3; i++ )
    {
    this->m_ContourRepresentation.push_back(
      vtkSmartPointer< vtkOrientedGlyphContourRepresentation >::New() );
    this->m_ContourRepresentation.back()->GetProperty()->SetColor(0., 1., 1.);
    this->m_ContourRepresentation.back()->GetLinesProperty()->SetColor(1., 0., 1.);
    this->m_ContourRepresentation.back()->GetActiveProperty()->SetColor(1., 1., 0.);

    this->m_ContourWidget.push_back(
      vtkSmartPointer< vtkContourWidget >::New() );
    this->m_ContourWidget.back()->SetPriority(10.0);
    this->m_ContourWidget.back()->SetInteractor( m_ImageView->GetInteractor(i) );
    this->m_ContourWidget.back()->Off();
    }

  m_NavigationDockWidget = new QGoNavigationDockWidget(this, GoFigure::THREE_D);
  m_NavigationDockWidget->resize(120, 300);

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

  QObject::connect( m_NavigationDockWidget, SIGNAL( ShowAllChannelsChanged(bool) ),
                    this, SLOT( ShowAllChannels(bool) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( ShowOneChannelChanged(int) ),
                    this, SLOT( ShowOneChannel(int) ) );

  this->m_DockWidgetList.push_front(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(m_NavigationDockWidget, Qt::LeftDockWidgetArea, true, true),
      m_NavigationDockWidget) );

  CreateAllViewActions();

  CreateModeActions();

  ReadSettings();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::CreateAllViewActions()
{
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

  QAction *separator = new QAction(this);
  separator->setSeparator(true);

  this->m_ViewActions.push_back(separator);

  QAction *LookupTableAction = new QAction(tr("Lookup Table"), this);
  LookupTableAction->setStatusTip( tr(" Change the associated lookup table") );

  QIcon luticon;
  luticon.addPixmap(QPixmap( QString::fromUtf8(":/fig/LookupTable.png") ),
                    QIcon::Normal, QIcon::Off);
  LookupTableAction->setIcon(luticon);

  // Here write the connection
  QObject::connect( LookupTableAction, SIGNAL( triggered() ),
                    this, SLOT( ChangeLookupTable() ) );

  this->m_ViewActions.push_back(LookupTableAction);

  QAction *ScalarBarAction = new QAction(tr("Display Scalar Bar"), this);
  ScalarBarAction->setCheckable(true);

  QIcon scalarbaricon;
  scalarbaricon.addPixmap(QPixmap( QString::fromUtf8(":/fig/scalarbar.png") ),
                          QIcon::Normal, QIcon::Off);
  ScalarBarAction->setIcon(scalarbaricon);

  this->m_ViewActions.push_back(ScalarBarAction);

  QObject::connect( ScalarBarAction, SIGNAL( toggled(bool) ),
                    this, SLOT( ShowScalarBar(bool) ) );

  QPixmap Pix(16, 16);
  Pix.fill(Qt::black);
  m_BackgroundColorAction = new QAction(Pix, tr("Set Background Color"), this);
  this->m_ViewActions.push_back(m_BackgroundColorAction);

  QObject::connect( m_BackgroundColorAction, SIGNAL( triggered() ),
                    this, SLOT( ChangeBackgroundColor() ) );

  QAction *separator2 = new QAction(this);
  separator2->setSeparator(true);
  this->m_ViewActions.push_back(separator2);

  this->m_ViewActions.push_back( m_NavigationDockWidget->toggleViewAction() );

  QAction *separator3 = new QAction(this);
  separator3->setSeparator(true);
  this->m_ViewActions.push_back(separator3);

  QAction *DisplayAnnotationsAction = new QAction(tr("Display annotations"), this);
  DisplayAnnotationsAction->setCheckable(true);
  DisplayAnnotationsAction->setChecked(true);
  DisplayAnnotationsAction->setStatusTip( tr(" Display or not annotations in each 2d view") );

  QIcon displayannotationsicon;
  displayannotationsicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/2D_VIEWS_INFOS.png") ),
                                   QIcon::Normal, QIcon::Off);
  DisplayAnnotationsAction->setIcon(displayannotationsicon);

  QObject::connect( DisplayAnnotationsAction, SIGNAL( triggered() ),
                    this, SLOT( DisplayAnnotations() ) );

  this->m_ViewActions.push_back(DisplayAnnotationsAction);

  QAction *DisplaySplinePlanesAction = new QAction(tr("Display spline planes"), this);
  DisplaySplinePlanesAction->setCheckable(true);
  DisplaySplinePlanesAction->setChecked(true);
  DisplaySplinePlanesAction->setStatusTip( tr(" Display or not spline planes on each view") );

  QIcon displaysplineplaneicon;
  displaysplineplaneicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/C_M_L.png") ),
                                   QIcon::Normal, QIcon::Off);
  DisplaySplinePlanesAction->setIcon(displaysplineplaneicon);

  QObject::connect( DisplaySplinePlanesAction, SIGNAL( triggered() ),
                    this, SLOT( DisplaySplinePlanes() ) );

  this->m_ViewActions.push_back(DisplaySplinePlanesAction);

  QAction *DisplayCube3D = new QAction(tr("Display 3D cube"), this);
  DisplayCube3D->setCheckable(true);
  DisplayCube3D->setChecked(true);
  DisplayCube3D->setStatusTip( tr(" Display or not cube in 3d") );

  QIcon cube3dicon;
  cube3dicon.addPixmap(QPixmap( QString::fromUtf8(":/fig/cube.png") ),
                       QIcon::Normal, QIcon::Off);
  DisplayCube3D->setIcon(cube3dicon);

  QObject::connect( DisplayCube3D, SIGNAL( triggered() ),
                    this, SLOT( DisplayCube() ) );

  this->m_ViewActions.push_back(DisplayCube3D);

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
}

//--------------------------------------------------------------------------
//-------------------------------------------------------------------------
void QGoTabImageView3D::CreateModeActions()
{
  QActionGroup *group = new QActionGroup(this);

  // Call superclass
  QGoTabElementBase::CreateModeActions(group);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView3D::~QGoTabImageView3D()
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iParent
 */
void QGoTabImageView3D::setupUi(QWidget *iParent)
{
  if ( iParent->objectName().isEmpty() )
    {
    iParent->resize(800, 800);
    }

  m_ImageView = new QGoImageView3D(this);
  m_ImageView->SetBackgroundColor(m_BackgroundColor);

  QObject::connect( m_ImageView, SIGNAL( SliceViewXYChanged(int) ),
                    this, SIGNAL( SliceViewXYChanged(int) ) );

  QObject::connect( m_ImageView, SIGNAL( SliceViewXZChanged(int) ),
                    this, SIGNAL( SliceViewXZChanged(int) ) );

  QObject::connect( m_ImageView, SIGNAL( SliceViewYZChanged(int) ),
                    this, SIGNAL( SliceViewYZChanged(int) ) );

  QObject::connect( m_ImageView, SIGNAL( FullScreenViewChanged(int) ),
                    this, SIGNAL( FullScreenViewChanged(int) ) );

  this->m_LayOut = new QHBoxLayout(iParent);
  this->m_LayOut->addWidget(m_ImageView);

  retranslateUi(iParent);

  QMetaObject::connectSlotsByName(iParent);
} // setupUi

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iParent
 */
void QGoTabImageView3D::retranslateUi(QWidget *iParent)
{
  iParent->setWindowTitle( tr("QGoTabImageView3D") );
  Q_UNUSED(iParent);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @return
 */
GoFigure::TabDimensionType QGoTabImageView3D::GetTabDimensionType() const
{
  return GoFigure::THREE_D;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::Update()
{
  m_ImageView->Update();
  int extent[6];
  m_Image->GetExtent(extent);

  m_NavigationDockWidget->SetXSlice( ( extent[0] + extent[1] ) / 2 );
  m_NavigationDockWidget->SetYSlice( ( extent[2] + extent[3] ) / 2 );
  m_NavigationDockWidget->SetZSlice( ( extent[4] + extent[5] ) / 2 );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::ChangeLookupTable()
{
  vtkLookupTable *lut = vtkLookupTable::New();

  lut->DeepCopy( QGoLUTDialog::GetLookupTable( this,
                                               tr("Choose one look-up table") ) );
  m_ImageView->SetLookupTable(lut);
  lut->Delete();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iShow
 */
void QGoTabImageView3D::ShowScalarBar(const bool & iShow)
{
  m_ImageView->ShowScalarBar(iShow);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iType
 * @param iBaseName
 * @return
 */
QString QGoTabImageView3D::SnapshotViewXY(
  const GoFigure::FileType & iType,
  const QString & iBaseName)
{
  return m_ImageView->SnapshotViewXY(iType, iBaseName);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iType
 * @param iBaseName
 * @return
 */
QString QGoTabImageView3D::SnapshotView2(
  const GoFigure::FileType & iType,
  const QString & iBaseName)
{
  return m_ImageView->SnapshotView2(iType, iBaseName);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iType
 * @param iBaseName
 * @return
 */
QString QGoTabImageView3D::SnapshotView3(
  const GoFigure::FileType & iType,
  const QString & iBaseName)
{
  return m_ImageView->SnapshotView3(iType, iBaseName);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iType
 * @param iBaseName
 * @return
 */
QString QGoTabImageView3D::SnapshotViewXYZ(
  const GoFigure::FileType & iType,
  const QString & iBaseName)
{
  return m_ImageView->SnapshotViewXYZ(iType, iBaseName);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iS
 */
void QGoTabImageView3D::SetSliceViewXY(const int & iS)
{
  m_ImageView->SetSliceViewXY(iS);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iS
 */
void QGoTabImageView3D::SetSliceViewXZ(const int & iS)
{
  m_ImageView->SetSliceViewXZ(iS);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iS
 */
void QGoTabImageView3D::SetSliceViewYZ(const int & iS)
{
  m_ImageView->SetSliceViewYZ(iS);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iS
 */
void QGoTabImageView3D::SetFullScreenView(const int & iS)
{
  m_ImageView->SetFullScreenView(iS);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::Quadview()
{
  m_ImageView->SetFullScreenView(0);
  m_TakeSnapshotAction->setEnabled(false);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::FullScreenViewXY()
{
  m_ImageView->SetFullScreenView(1);
  m_TakeSnapshotAction->setEnabled(true);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::FullScreenViewXZ()
{
  m_ImageView->SetFullScreenView(2);
  m_TakeSnapshotAction->setEnabled(true);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::FullScreenViewYZ()
{
  m_ImageView->SetFullScreenView(3);
  m_TakeSnapshotAction->setEnabled(true);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::FullScreenViewXYZ()
{
  m_ImageView->SetFullScreenView(4);
  m_TakeSnapshotAction->setEnabled(true);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::GetBackgroundColorFromImageViewer()
{
  double r, g, b;

  m_ImageView->GetBackgroundColor(r, g, b);
  this->m_BackgroundColor.setRgbF(r, g, b);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::SetBackgroundColorToImageViewer()
{
  m_ImageView->SetBackgroundColor(this->m_BackgroundColor);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param image
 */
void QGoTabImageView3D::SetImageToImageViewer(vtkImageData *image)
{
  m_ImageView->SetImage(image);
  m_ImageView->Update();

  for ( unsigned int i = 0; i < this->m_ContourWidget.size(); i++ )
    {
    vtkImageActorPointPlacer *point_placer = vtkImageActorPointPlacer::New();
    point_placer->SetImageActor( m_ImageView->GetImageActor(i) );

    this->m_ContourRepresentation[i]->SetPointPlacer(point_placer);
    point_placer->Delete();

    this->m_ContourWidget[i]->SetRepresentation(this->m_ContourRepresentation[i]);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int * QGoTabImageView3D::GetImageCoordinatesFromWorldCoordinates(double iPos[3])
{
  return m_ImageView->GetImageCoordinatesFromWorldCoordinates(iPos);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// std::vector< vtkQuadricLODActor* >
std::vector< vtkActor * >
QGoTabImageView3D::AddContour(vtkPolyData *dataset,
                              vtkProperty *iProperty)
{
  return this->m_ImageView->AddContour(dataset, iProperty);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView3D::RemoveActorFromViewer(const int & iId, vtkActor *iActor)
{
  m_ImageView->RemoveActor(iId, iActor);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::DisplayActorInViewer(const int & iId, vtkActor *iActor)
{
  m_ImageView->AddActor(iId, iActor);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView3D::SetSlice(int iDir, int *iIdx)
{
  switch ( iDir )
    {
    default:
    case 0:
      {
      this->SetSliceViewXY(iIdx[2]);
      break;
      }
    case 1:
      {
      this->SetSliceViewXZ(iIdx[1]);
      break;
      }
    case 2:
      {
      this->SetSliceViewXY(iIdx[0]);
      break;
      }
    }
}

//--------------------------------------------------------------------------

void
QGoTabImageView3D::ChangeBackgroundColor()
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
QGoTabImageView3D::DisplayAnnotations()
{
  this->m_ImageView->ShowAnnotations();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3D::DisplaySplinePlanes()
{
  this->m_ImageView->ShowSplinePlane();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3D::DisplayCube()
{
  this->m_ImageView->ShowCube3D();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3D::Change3DPerspectiveToAxial()
{
  this->m_ImageView->SetCamera(1);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3D::Change3DPerspectiveToCoronal()
{
  this->m_ImageView->SetCamera(2);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3D::Change3DPerspectiveToSagittal()
{
  this->m_ImageView->SetCamera(3);
}

//-------------------------------------------------------------------------
void
QGoTabImageView3D::DefaultInteractorBehavior(bool iEnable)
{
  (void)iEnable;
  this->m_ImageView->DefaultMode();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3D::ZoomInteractorBehavior(bool iEnable)
{
  (void)iEnable;
  this->m_ImageView->ZoomMode();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3D::PanInteractorBehavior(bool iEnable)
{
  (void)iEnable;
  this->m_ImageView->PanMode();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void
QGoTabImageView3D::TakeSnapshot()
{
  // Get the current view displayed in full screen
  int FullScreenView = m_ImageView->GetFullScreenView();

  // TODO enhance the name of the files

  switch ( FullScreenView )
    {
    case 1:
      // X Slice
      SnapshotViewXY(GoFigure::PNG, "snapshot_");
      break;

    case 2:
      // Y Slice
      SnapshotView2(GoFigure::PNG, "snapshot_");
      break;

    case 3:
      // Z Slice
      SnapshotView3(GoFigure::PNG, "snapshot_");
      break;

    default:
      // 3D view
      SnapshotViewXYZ(GoFigure::PNG, "snapshot_");
      break;
    }
}

//-------------------------------------------------------------------------
