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
#include "QGoTabImageView2D.h"

#include "QGoImageView2D.h"
#include "QGoLUTDialog.h"
#include "QGoNavigationDockWidget.h"

#include "vtkLookupTable.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkImageData.h"
#include "vtkTextProperty.h"
#include "vtkImageExtractComponents.h"
#include "vtkProperty.h"
#include "vtkContourWidget.h"
#include "vtkOrientedGlyphContourRepresentation.h"
#include "vtkImageActorPointPlacer.h"

#include "QGoImageFilterPluginBase.h"

#include <QCursor>
#include <QMenu>
#include <QColorDialog>
#include <QMessageBox>
#include <QSettings>
#include <QVariant>

#include "GoImageProcessor.h"

//--------------------------------------------------------------------------
QGoTabImageView2D::QGoTabImageView2D(QWidget *iParent) :
  QGoTabImageViewNDBase(iParent)
{
  m_Image = 0;
  setupUi(this);

  this->m_ContourRepresentation.push_back(
    vtkSmartPointer< vtkOrientedGlyphContourRepresentation >::New() );
  this->m_ContourRepresentation.back()->GetProperty()->SetColor(0., 1., 1.);
  this->m_ContourRepresentation.back()->GetLinesProperty()->SetColor(1., 0., 1.);
  this->m_ContourRepresentation.back()->GetActiveProperty()->SetColor(1., 1., 0.);

  this->m_ContourWidget.push_back(
    vtkSmartPointer< vtkContourWidget >::New() );
  this->m_ContourWidget.back()->SetPriority(10.0);
  this->m_ContourWidget.back()->SetInteractor( m_ImageView->GetInteractor() );
  this->m_ContourWidget.back()->Off();

  m_NavigationDockWidget = new QGoNavigationDockWidget(this, GoFigure::TWO_D);
  m_NavigationDockWidget->resize(120, 300);

  this->m_DockWidgetList.push_front(
    std::pair< QGoDockWidgetStatus *, QDockWidget * >(
      new QGoDockWidgetStatus(m_NavigationDockWidget, Qt::LeftDockWidgetArea, true, true),
      m_NavigationDockWidget) );

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

  QObject::connect( m_NavigationDockWidget, SIGNAL( ShowAllChannelsChanged(bool) ),
                    this, SLOT( ShowAllChannels(bool) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( ShowOneChannelChanged(int) ),
                    this, SLOT( ShowOneChannel(int) ) );

  QAction *separator2 = new QAction(this);
  separator2->setSeparator(true);
  this->m_ViewActions.push_back(separator2);

  this->m_ViewActions.push_back( m_NavigationDockWidget->toggleViewAction() );

  //CreateModeActions();

  ReadSettings();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView2D::
~QGoTabImageView2D()
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoFigure::TabDimensionType
QGoTabImageView2D::GetTabDimensionType() const
{
  return GoFigure::TWO_D;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::setupUi(QWidget *iParent)
{
  if ( iParent->objectName().isEmpty() )
    {
    iParent->resize(800, 800);
    }

  m_ImageView = new QGoImageView2D(this);
  m_ImageView->SetBackgroundColor(m_BackgroundColor);

  this->m_LayOut = new QHBoxLayout(iParent);
  this->m_LayOut->addWidget(m_ImageView);

  retranslateUi(iParent);

  QMetaObject::connectSlotsByName(iParent);
} // setupUi

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::retranslateUi(QWidget *iParent)
{
  iParent->setWindowTitle( tr("QGoTabImageView2D") );
  Q_UNUSED(iParent);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::Update()
{
  m_ImageView->Update();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::SetBackgroundColorToImageViewer()
{
  m_ImageView->SetBackgroundColor(this->m_BackgroundColor);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::GetBackgroundColorFromImageViewer()
{
  double r(0.), g(0.), b(0.);

  m_ImageView->GetBackgroundColor(r, g, b);
  this->m_BackgroundColor.setRgbF(r, g, b);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::ChangeLookupTable()
{
  vtkLookupTable *lut = vtkLookupTable::New();

  lut->DeepCopy( QGoLUTDialog::GetLookupTable( this,
                                               tr("Choose one look-up table") ) );
  m_ImageView->SetLookupTable(lut);
  lut->Delete();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::ShowScalarBar(const bool & iShow)
{
  m_ImageView->ShowScalarBar(iShow);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::WriteSettings()
{
  QGoTabImageViewElementBase::WriteSettings();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::ReadSettings()
{
  QGoTabImageViewElementBase::ReadSettings();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::SetImageToImageViewer(vtkImageData *iImage)
{
  m_ImageView->SetImage(iImage);
  m_ImageView->Update();

  vtkImageActorPointPlacer *point_placer = vtkImageActorPointPlacer::New();
  point_placer->SetImageActor( m_ImageView->GetImageActor(0) );

  this->m_ContourRepresentation.back()->SetPointPlacer(point_placer);
  point_placer->Delete();

  this->m_ContourWidget.back()->SetRepresentation(
    this->m_ContourRepresentation.back() );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int *
QGoTabImageView2D::GetImageCoordinatesFromWorldCoordinates(double iPos[3])
{
  return m_ImageView->GetImageCoordinatesFromWorldCoordinates(iPos);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// std::vector< vtkQuadricLODActor* >
std::vector< vtkActor * >
QGoTabImageView2D::AddContour(vtkPolyData *dataset, vtkProperty *iProperty)
{
  return m_ImageView->AddContour(dataset, iProperty);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::RemoveActorFromViewer(const int & iId, vtkActor *iActor)
{
  m_ImageView->RemoveActor(iId, iActor);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::DisplayActorInViewer(const int & iId, vtkActor *iActor)
{
  m_ImageView->AddActor(iId, iActor);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView2D::SetSlice(int iDir, int *iIdx)
{
  (void)iDir;
  (void)iIdx;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView2D::ChangeBackgroundColor()
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

//-------------------------------------------------------------------------
void QGoTabImageView2D::CreateModeToolBar(QMenu*iMenu, QToolBar* iToolBar)
{
  //QActionGroup *group = new QActionGroup(this);

  // Call superclass
  //QGoTabElementBase::CreateModeActions(group);
  QGoTabElementBase::CreateModeToolBar(iMenu, iToolBar);
  this->m_ToolBarList.push_back(this->m_ModeToolBar);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView2D::TakeSnapshot()
{
  m_ImageView->SnapshotViewXY(GoFigure::PNG, "snapshot_");
}

//-------------------------------------------------------------------------
void
QGoTabImageView2D::DefaultInteractorBehavior(bool iEnable)
{
  (void)iEnable;
  m_ImageView->DefaultMode();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView2D::ZoomInteractorBehavior(bool iEnable)
{
  (void)iEnable;
  m_ImageView->ZoomMode();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView2D::PanInteractorBehavior(bool iEnable)
{
  (void)iEnable;
  m_ImageView->PanMode();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView2D::
UpdateImage()
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
