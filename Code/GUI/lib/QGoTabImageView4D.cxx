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

#include "QGoTabImageView4D.h"

#include "QGoImageView3D.h"
#include "QGoLUTDialog.h"
#include "QGoNavigationDockWidget.h"
#include "QGoContourManualSegmentationWidget.h"

#ifdef   ENABLEVIDEORECORD
#include "QGoVideoRecorder.h"
#endif

#include "SnapshotHelper.h"

#include "vtkImageData.h"
#include "vtkLookupTable.h"
#include "vtkImageAppendComponents.h"

#include "vtkContourWidget.h"
#include "vtkOrientedGlyphContourRepresentation.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkImageActorPointPlacer.h"

#include <QLabel>
#include <QDockWidget>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QColorDialog>

#include <algorithm>

//--------------------------------------------------------------------------
QGoTabImageView4D::QGoTabImageView4D(QWidget *iParent):
  QGoTabElementBase(iParent),
  m_XYZImage(0),
  m_XYTImage(0),
  m_BackgroundColor(Qt::black),
  m_TimePoint(-1),
  m_ZSlice(-1),
  m_FirstUpdate(true)
{
  m_Reader1 = itk::MegaCaptureReader::New();
  m_Reader2 = itk::MegaCaptureReader::New();

  m_XYZImage = vtkSmartPointer< vtkImageData >::New();
  m_XYTImage = vtkSmartPointer< vtkImageData >::New();

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
    this->m_ContourWidget.back()->SetInteractor( m_XYZImageView->GetInteractor(i) );
    this->m_ContourWidget.back()->Off();
    }

  CreateVisuDockWidget();

  CreateManualSegmentationdockWidget();

#ifdef   ENABLEVIDEORECORD
  m_VideoRecorderWidget = new QGoVideoRecorder(this);
#endif

  CreateAllViewActions();
  CreateModeActions();

  ReadSettings();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::CreateAllViewActions()
{
  QActionGroup *group = new QActionGroup(this);

  QAction *OctoViewAction = new QAction(tr("Octo-View"), this);

  OctoViewAction->setCheckable(true);
  OctoViewAction->setChecked(true);

  group->addAction(OctoViewAction);

  QObject::connect( OctoViewAction, SIGNAL( triggered() ),
                    this, SLOT( Octview() ) );

  m_ViewActions.push_back(OctoViewAction);

  QAction *XYZQuadViewAction = new QAction(tr("Quad-View XYZ"), this);
  XYZQuadViewAction->setCheckable(true);
  XYZQuadViewAction->setChecked(true);

  group->addAction(XYZQuadViewAction);

  m_ViewActions.push_back(XYZQuadViewAction);

  QObject::connect( XYZQuadViewAction, SIGNAL( triggered() ),
                    this, SLOT( QuadviewXYZ() ) );

  QAction *XYTQuadViewAction = new QAction(tr("Quad-View XYT"), this);
  XYTQuadViewAction->setCheckable(true);
  XYTQuadViewAction->setChecked(true);

  group->addAction(XYTQuadViewAction);

  m_ViewActions.push_back(XYTQuadViewAction);

  QObject::connect( XYTQuadViewAction, SIGNAL( triggered() ),
                    this, SLOT( QuadviewXYT() ) );

  QAction *FullScreenXYAction = new QAction(tr("Full-Screen XY"), this);
  FullScreenXYAction->setCheckable(true);

  group->addAction(FullScreenXYAction);

  m_ViewActions.push_back(FullScreenXYAction);

  QObject::connect( FullScreenXYAction, SIGNAL( triggered() ),
                    this, SLOT( FullScreenViewXY() ) );

  QAction *FullScreenXZAction = new QAction(tr("Full-Screen XZ"), this);
  FullScreenXZAction->setCheckable(true);

  group->addAction(FullScreenXZAction);

  m_ViewActions.push_back(FullScreenXZAction);

  QObject::connect( FullScreenXZAction, SIGNAL( triggered() ),
                    this, SLOT( FullScreenViewXZ() ) );

  QAction *FullScreenYZAction = new QAction(tr("Full-Screen YZ"), this);
  FullScreenYZAction->setCheckable(true);

  group->addAction(FullScreenYZAction);

  m_ViewActions.push_back(FullScreenYZAction);

  QObject::connect( FullScreenYZAction, SIGNAL( triggered() ),
                    this, SLOT( FullScreenViewYZ() ) );

  QAction *FullScreenXYZAction = new QAction(tr("Full-Screen XYZ"), this);
  FullScreenXYZAction->setCheckable(true);

  group->addAction(FullScreenXYZAction);

  m_ViewActions.push_back(FullScreenXYZAction);

  QObject::connect( FullScreenXYZAction, SIGNAL( triggered() ),
                    this, SLOT( FullScreenViewXYZ() ) );

  // now FullScreen actions related to the XYT view
  QAction *FullScreenXTAction = new QAction(tr("Full-Screen XT"), this);
  FullScreenXTAction->setCheckable(true);

  group->addAction(FullScreenXTAction);

  m_ViewActions.push_back(FullScreenXTAction);

  QObject::connect( FullScreenXTAction, SIGNAL( triggered() ),
                    this, SLOT( FullScreenViewXT() ) );

  QAction *FullScreenYTAction = new QAction(tr("Full-Screen YT"), this);
  FullScreenYTAction->setCheckable(true);

  group->addAction(FullScreenYTAction);

  m_ViewActions.push_back(FullScreenYTAction);

  QObject::connect( FullScreenYTAction, SIGNAL( triggered() ),
                    this, SLOT( FullScreenViewYT() ) );

  QAction *FullScreenXYTAction = new QAction(tr("Full-Screen XYT"), this);
  FullScreenXYTAction->setCheckable(true);

  group->addAction(FullScreenXYTAction);

  m_ViewActions.push_back(FullScreenXYTAction);

  QObject::connect( FullScreenXYTAction, SIGNAL( triggered() ),
                    this, SLOT( FullScreenViewXYT() ) );

  QAction *separator = new QAction(this);
  separator->setSeparator(true);

  m_ViewActions.push_back(separator);

  QAction *LookupTableAction = new QAction(tr("Lookup Table"), this);
  LookupTableAction->setStatusTip( tr(" Change the associated lookup table") );

  // Here write the connection
  QObject::connect( LookupTableAction, SIGNAL( triggered() ),
                    this, SLOT( ChangeLookupTable() ) );

  m_ViewActions.push_back(LookupTableAction);

  QAction *ScalarBarAction = new QAction(tr("Display Scalar Bar"), this);
  ScalarBarAction->setCheckable(true);
  m_ViewActions.push_back(ScalarBarAction);

  QObject::connect( ScalarBarAction, SIGNAL( toggled(bool) ),
                    this, SLOT( ShowScalarBar(bool) ) );

  QAction *BackgroundColorAction = new QAction(tr("Background Color"), this);
  m_ViewActions.push_back(BackgroundColorAction);

  QObject::connect( BackgroundColorAction, SIGNAL( triggered() ),
                    this, SLOT( ChangeBackgroundColor() ) );
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void QGoTabImageView4D::CreateModeActions()
{
  QActionGroup *group = new QActionGroup(this);

  QAction *ManualEditingAction = new QAction(tr("Manual-Editing"), this);

  ManualEditingAction->setCheckable(true);
  QIcon ManualEditingIcon;
  ManualEditingIcon.addPixmap(QPixmap( QString::fromUtf8(":/fig/manual-editing.png") ),
                              QIcon::Normal, QIcon::Off);
  ManualEditingAction->setIcon(ManualEditingIcon);

  group->addAction(ManualEditingAction);

  this->m_ModeActions.push_back(ManualEditingAction);
  /** \todo implement the manual editing mode*/
  //QObject::connect( ManualEditingAction, SIGNAL( triggered() ),
  //  this, SLOT( ManualEditingMode() ) );

  QAction *DefaultAction = new QAction(tr("Default"), this);
  DefaultAction->setCheckable(true);
  DefaultAction->setChecked(true);

  /** \todo add an icon for default*/
  /* QIcon DefaultIcon;
   DefaultIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/xy.png")),
     QIcon::Normal, QIcon::Off );
   DefaultAction->setIcon( DefaultIcon );*/

  group->addAction(DefaultAction);

  this->m_ModeActions.push_back(DefaultAction);
  /** \todo implement default mode*/
  //QObject::connect( DefaultAction, SIGNAL( triggered() ),
  // this, SLOT( DefaultMode() ) );
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView4D::CreateVisuDockWidget()
{
  m_NavigationDockWidget = new QGoNavigationDockWidget(this, GoFigure::FOUR_D);

  QObject::connect( m_NavigationDockWidget, SIGNAL( XSliceChanged(int) ),
                    this, SLOT( SetXSlice(int) ) );

  QObject::connect( this, SIGNAL( XSliceChanged(int) ),
                    m_NavigationDockWidget, SLOT( SetXSlice(int) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( YSliceChanged(int) ),
                    this, SLOT( SetYSlice(int) ) );

  QObject::connect( this, SIGNAL( YSliceChanged(int) ),
                    m_NavigationDockWidget, SLOT( SetYSlice(int) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( ZSliceChanged(int) ),
                    this, SLOT( SetZSlice(int) ) );

  QObject::connect( this, SIGNAL( ZSliceChanged(int) ),
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
/**
 * \brief
 */
void QGoTabImageView4D::CreateManualSegmentationdockWidget()
{
  m_ManualSegmentationWidget = new QGoContourManualSegmentationWidget(this);

  QObject::connect( m_ManualSegmentationWidget, SIGNAL( ValidatePressed() ),
                    this, SLOT( ValidateContour() ) );

  QObject::connect( m_ManualSegmentationWidget,
                    SIGNAL( ActivateManualSegmentationToggled(bool) ),
                    this, SLOT( ActivateManualSegmentationEditor(bool) ) );

  QObject::connect( m_ManualSegmentationWidget,
                    SIGNAL( ContourRepresentationPropertiesChanged() ),
                    this, SLOT( ChangeContourRepresentationProperty() ) );
/// TODO fix it is not a dock widget anymore
//  this->m_SegmentationActions.push_back(
//    m_ManualSegmentationWidget->toggleViewAction());
}

//-------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView4D::~QGoTabImageView4D()
{
  if ( m_XYZImageView )
    {
    delete m_XYZImageView;
    m_XYZImageView = 0;
    }
  if ( m_XYTImageView )
    {
    delete m_XYTImageView;
    m_XYTImageView = 0;
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::setupUi(QWidget *iParent)
{
  if ( iParent->objectName().isEmpty() )
    {
    iParent->resize(800, 800);
    }

  QList< int > list_size;
  list_size.push_back(10);
  list_size.push_back(10);

  m_XYZImageView = new QGoImageView3D;
  m_XYZImageView->SetBackgroundColor(m_BackgroundColor);

  m_XYTImageView = new QGoImageView3D;
  m_XYTImageView->SetBackgroundColor(m_BackgroundColor);

  m_Splitter = new QSplitter(Qt::Horizontal, iParent);
  m_Splitter->addWidget(m_XYZImageView);
  m_Splitter->addWidget(m_XYTImageView);
  m_Splitter->setSizes(list_size);
  m_Splitter->resize(800, 800);

  QObject::connect( m_XYZImageView, SIGNAL( SliceViewXYChanged(int) ),
                    this, SIGNAL( ZSliceChanged(int) ) );

  QObject::connect( m_XYZImageView, SIGNAL( SliceViewXZChanged(int) ),
                    this, SIGNAL( YSliceChanged(int) ) );

  QObject::connect( m_XYZImageView, SIGNAL( SliceViewYZChanged(int) ),
                    this, SIGNAL( XSliceChanged(int) ) );

  QObject::connect( m_XYTImageView, SIGNAL( SliceViewXYChanged(int) ),
                    this, SIGNAL( TimePointChanged(int) ) );

  QObject::connect( m_XYTImageView, SIGNAL( SliceViewXZChanged(int) ),
                    this, SIGNAL( YSliceChanged(int) ) );

  QObject::connect( m_XYTImageView, SIGNAL( SliceViewYZChanged(int) ),
                    this, SIGNAL( XSliceChanged(int) ) );

//   QObject::connect( m_ImageView, SIGNAL( FullScreenViewChanged( int ) ),
//     this, SIGNAL( FullScreenViewChanged( int ) ) );

  retranslateUi(iParent);

  QMetaObject::connectSlotsByName(iParent);
} // setupUi

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::retranslateUi(QWidget *iParent)
{
  iParent->setWindowTitle( tr("QGoTabImageView4D") );
  Q_UNUSED(iParent);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoFigure::TabDimensionType
QGoTabImageView4D::GetTabDimensionType() const
{
  return GoFigure::FOUR_D;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView4D::SetMegaCaptureFile(
  const GoFigureFileInfoHelperMultiIndexContainer & iContainer,
  const GoFigure::FileType & iFileType,
  const std::string & iHeader)
{
  m_FileType = iFileType;
  m_FileList = iContainer;

  m_FirstUpdate = true;

  m_Reader1->SetInput(m_FileList);
  m_Reader1->SetMegaCaptureHeader(iHeader);
  m_Reader1->SetFileType(m_FileType);
  m_Reader1->SetTimeBased(true);
  m_Reader1->SetTimePoint(0);
  m_Reader1->Update();

  unsigned int min_z = m_Reader1->GetMinZSlice();
  unsigned int max_z = m_Reader1->GetMaxZSlice();

  unsigned int zslice = ( min_z + max_z ) / 2;

  m_Reader2->SetInput(m_FileList);
  m_Reader2->SetMegaCaptureHeader(iHeader);
  m_Reader2->SetFileType(m_FileType);
  m_Reader2->SetTimeBased(false);
  m_Reader2->SetZSlice(zslice);

  unsigned int min_ch = m_Reader1->GetMinChannel();
  unsigned int max_ch = m_Reader1->GetMaxChannel();

  unsigned int NumberOfChannels = max_ch - min_ch + 1;

  vtkImageData *temp = m_Reader1->GetOutput(min_ch);

  int extent[6];
  temp->GetExtent(extent);

  m_NavigationDockWidget->SetXMinimumAndMaximum(extent[0], extent[1]);
  m_NavigationDockWidget->SetYMinimumAndMaximum(extent[2], extent[3]);
  m_NavigationDockWidget->SetZMinimumAndMaximum(extent[4], extent[5]);

  unsigned int min_t = m_Reader1->GetMinTimePoint();
  unsigned int max_t = m_Reader1->GetMaxTimePoint();

  m_NavigationDockWidget->SetTMinimumAndMaximum(min_t, max_t);
  m_NavigationDockWidget->SetTSlice(0);

  m_NavigationDockWidget->SetNumberOfChannels(NumberOfChannels);

  if ( NumberOfChannels > 1 )
    {
    m_NavigationDockWidget->SetChannel(0);
    m_XYZInternalImages.resize(NumberOfChannels, NULL);
    m_XYTInternalImages.resize(NumberOfChannels, NULL);

    for ( unsigned int i = 1; i < NumberOfChannels; i++ )
      {
      m_NavigationDockWidget->SetChannel(i);
      }
    }

  SetTimePoint(0);
  SetZSlice(zslice);
  Update();

  m_NavigationDockWidget->SetXSlice( ( extent[0] + extent[1] ) / 2 );
  m_NavigationDockWidget->SetYSlice( ( extent[2] + extent[3] ) / 2 );
  m_NavigationDockWidget->SetZSlice(zslice);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetTimePoint(const int & iTimePoint)
{
  if ( iTimePoint == m_TimePoint )
    {
    return;
    }

  if ( !m_FileList.empty() )
    {
    unsigned int t = static_cast< unsigned int >( iTimePoint );
    if ( ( t < m_Reader1->GetMinTimePoint() )
         || ( t > m_Reader1->GetMaxTimePoint() ) )
      {
      return;
      }
    else
      {
      m_TimePoint = iTimePoint;
      m_Reader1->SetTimePoint(m_TimePoint);
      m_Reader1->Update();

      unsigned int min_ch = m_Reader1->GetMinChannel();
      unsigned int max_ch = m_Reader1->GetMaxChannel();

      int NumberOfChannels = max_ch - min_ch + 1;

      if ( NumberOfChannels > 1 )
        {
        vtkSmartPointer< vtkImageAppendComponents > append_filter =
          vtkSmartPointer< vtkImageAppendComponents >::New();

        for ( unsigned int i = min_ch; i < max_ch; i++ )
          {
          if ( !m_XYZInternalImages[i] )
            {
            m_XYZInternalImages[i] = vtkSmartPointer< vtkImageData >::New();
            }
          m_XYZInternalImages[i]->ShallowCopy( m_Reader1->GetOutput(i) );
          append_filter->AddInput(m_XYZInternalImages[i]);
          }
        // This is really stupid!!!
        if ( NumberOfChannels < 3 )
          {
          for ( int i = NumberOfChannels; i < 3; i++ )
            {
            append_filter->AddInput(m_XYZInternalImages[0]);
            }
          }
        append_filter->Update();

        m_XYZImage->ShallowCopy( append_filter->GetOutput() );
        }
      else
        {
        m_XYZImage->ShallowCopy( m_Reader1->GetOutput(min_ch) );
        }

      if ( !m_FirstUpdate )
        {
        Update();
        }
      emit TimePointChanged(m_TimePoint);
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetZSlice(const int & iZSlice)
{
  if ( iZSlice == m_ZSlice )
    {
    return;
    }

  if ( !m_FileList.empty() )
    {
    unsigned int z = static_cast< unsigned int >( iZSlice );
    if ( ( z < m_Reader2->GetMinZSlice() )
         || ( z > m_Reader2->GetMaxZSlice() ) )
      {
      return;
      }
    else
      {
      m_ZSlice = iZSlice;
      m_Reader2->SetZSlice(m_ZSlice);

      unsigned int min_ch = m_Reader2->GetMinChannel();
      unsigned int max_ch = m_Reader2->GetMaxChannel();
      m_Reader2->Update();

      int NumberOfChannels = max_ch - min_ch + 1;

      if ( NumberOfChannels > 1 )
        {
        vtkSmartPointer< vtkImageAppendComponents > append_filter =
          vtkSmartPointer< vtkImageAppendComponents >::New();

        for ( unsigned int i = min_ch; i < max_ch; i++ )
          {
          if ( !m_XYTInternalImages[i] )
            {
            m_XYTInternalImages[i] = vtkSmartPointer< vtkImageData >::New();
            }
          m_XYTInternalImages[i]->ShallowCopy( m_Reader2->GetOutput(i) );
          append_filter->AddInput(m_XYTInternalImages[i]);
          }
        // This is really stupid!!!
        if ( NumberOfChannels < 3 )
          {
          for ( int i = NumberOfChannels; i < 3; i++ )
            {
            append_filter->AddInput(m_XYTInternalImages[0]);
            }
          }
        append_filter->Update();

        m_XYTImage->ShallowCopy( append_filter->GetOutput() );
        }
      else
        {
        m_XYTImage->ShallowCopy( m_Reader2->GetOutput(min_ch) );
        }

      if ( !m_FirstUpdate )
        {
        Update();
        }
      emit ZSliceChanged(m_ZSlice);
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::Update()
{
  m_XYZImageView->SetImage(m_XYZImage);
  m_XYZImageView->Update();

  m_XYTImageView->SetImage(m_XYTImage);
  m_XYTImageView->Update();

  m_XYZImageView->SetSliceViewXY(m_ZSlice);
  m_XYTImageView->SetSliceViewXY(m_TimePoint);

  for ( int i = 0; i < 3; i++ )
    {
    vtkSmartPointer< vtkImageActorPointPlacer > point_placer =
      vtkSmartPointer< vtkImageActorPointPlacer >::New();
    point_placer->SetImageActor( m_XYZImageView->GetImageActor(i) );

    this->m_ContourRepresentation[i]->SetPointPlacer(point_placer);

    this->m_ContourWidget[i]->SetRepresentation(this->m_ContourRepresentation[i]);
    }

  m_FirstUpdate = false;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::ChangeLookupTable()
{
  vtkLookupTable *lut = vtkLookupTable::New();

  lut->DeepCopy( QGoLUTDialog::GetLookupTable( this,
                                               tr("Choose one look-up table") ) );
  m_XYZImageView->SetLookupTable(lut);
  m_XYTImageView->SetLookupTable(lut);
  lut->Delete();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::ShowScalarBar(const bool & iShow)
{
  m_XYZImageView->ShowScalarBar(iShow);
  m_XYTImageView->ShowScalarBar(iShow);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::SnapshotViewXY(const GoFigure::FileType & iType,
                                  const QString & iBaseName)
{
  return m_XYZImageView->SnapshotViewXY(iType, iBaseName);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::SnapshotView2(const GoFigure::FileType & iType,
                                 const QString & iBaseName)
{
  return m_XYZImageView->SnapshotView2(iType, iBaseName);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::SnapshotView3(const GoFigure::FileType & iType,
                                 const QString & iBaseName)
{
  return m_XYZImageView->SnapshotView3(iType, iBaseName);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::SnapshotViewXYZ(const GoFigure::FileType & iType,
                                   const QString & iBaseName)
{
  return m_XYZImageView->SnapshotViewXYZ(iType, iBaseName);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::SnapshotViewXT(const GoFigure::FileType & iType,
                                  const QString & iBaseName)
{
  return m_XYTImageView->SnapshotView2(iType, iBaseName);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::SnapshotViewYT(const GoFigure::FileType & iType,
                                  const QString & iBaseName)
{
  return m_XYTImageView->SnapshotView3(iType, iBaseName);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::SnapshotViewXYT(const GoFigure::FileType & iType,
                                   const QString & iBaseName)
{
  return m_XYTImageView->SnapshotViewXYZ(iType, iBaseName);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetYSlice(const int & iS)
{
  int  slice = m_XYZImageView->GetSliceViewXZ();
  bool tobesignaled = false;

  if ( slice != iS )
    {
    m_XYZImageView->SetSliceViewXZ(iS);
    tobesignaled = true;
    }
  slice = m_XYTImageView->GetSliceViewXZ();

  if ( slice != iS )
    {
    m_XYTImageView->SetSliceViewXZ(iS);
    tobesignaled = true;
    }

  if ( tobesignaled )
    {
    emit YSliceChanged(iS);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetXSlice(const int & iS)
{
  int  slice = m_XYZImageView->GetSliceViewYZ();
  bool tobesignaled = false;

  if ( slice != iS )
    {
    m_XYZImageView->SetSliceViewYZ(iS);
    tobesignaled = true;
    }
  slice = m_XYTImageView->GetSliceViewYZ();

  if ( slice != iS )
    {
    m_XYTImageView->SetSliceViewYZ(iS);
    tobesignaled = true;
    }

  if ( tobesignaled )
    {
    emit XSliceChanged(iS);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetFullScreenView(const int & iS)
{
  if ( iS == 0 )
    {
    m_XYZImageView->show();
    m_XYZImageView->SetFullScreenView(iS);

    m_XYTImageView->show();
    m_XYTImageView->SetFullScreenView(iS);
    }
  else
    {
    if ( iS < 4 )
      {
      m_XYZImageView->show();
      m_XYZImageView->SetFullScreenView(iS - 1);

      m_XYTImageView->hide();
      }
    else
      {
      m_XYZImageView->show();
      m_XYZImageView->SetFullScreenView(iS - 4);

      m_XYTImageView->hide();
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::Octview()
{
  m_XYZImageView->show();
  m_XYZImageView->SetFullScreenView(0);

  m_XYTImageView->show();
  m_XYTImageView->SetFullScreenView(0);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::QuadviewXYZ()
{
  m_XYZImageView->show();
  m_XYZImageView->SetFullScreenView(0);

  m_XYTImageView->hide();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::QuadviewXYT()
{
  m_XYZImageView->hide();

  m_XYTImageView->show();
  m_XYTImageView->SetFullScreenView(0);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXY()
{
  m_XYZImageView->show();
  m_XYZImageView->SetFullScreenView(1);

  m_XYTImageView->hide();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXZ()
{
  m_XYZImageView->show();
  m_XYZImageView->SetFullScreenView(2);

  m_XYTImageView->hide();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewYZ()
{
  m_XYZImageView->show();
  m_XYZImageView->SetFullScreenView(3);

  m_XYTImageView->hide();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXYZ()
{
  m_XYZImageView->show();
  m_XYZImageView->SetFullScreenView(4);

  m_XYTImageView->hide();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXT()
{
  m_XYZImageView->hide();

  m_XYTImageView->show();
  m_XYTImageView->SetFullScreenView(2);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewYT()
{
  m_XYZImageView->hide();

  m_XYTImageView->show();
  m_XYTImageView->SetFullScreenView(3);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXYT()
{
  m_XYZImageView->hide();

  m_XYTImageView->show();
  m_XYTImageView->SetFullScreenView(4);
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void QGoTabImageView4D::GetBackgroundColorFromImageViewer()
{
  double r, g, b;

  m_XYZImageView->GetBackgroundColor(r, g, b);
  m_BackgroundColor.setRgbF(r, g, b);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetBackgroundColorToImageViewer()
{
  m_XYZImageView->SetBackgroundColor(m_BackgroundColor);
  m_XYTImageView->SetBackgroundColor(m_BackgroundColor);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction * > QGoTabImageView4D::ViewActions()
{
  return m_ViewActions;
}

//--------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView4D::resizeEvent(QResizeEvent *iEvent)
{
  QWidget::resizeEvent(iEvent);
  m_Splitter->resize( iEvent->size() );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param iChecked
 */
void QGoTabImageView4D::ShowAllChannels(bool iChecked)
{
  if ( iChecked )
    {
    vtkSmartPointer< vtkImageAppendComponents > append_filter1 =
      vtkSmartPointer< vtkImageAppendComponents >::New();
    vtkSmartPointer< vtkImageAppendComponents > append_filter2 =
      vtkSmartPointer< vtkImageAppendComponents >::New();

    for ( unsigned int i = 0; i < m_XYZInternalImages.size(); i++ )
      {
      append_filter1->AddInput(m_XYZInternalImages[i]);
      append_filter2->AddInput(m_XYTInternalImages[i]);
      }

    // This is really stupid!!!
    if ( m_XYZInternalImages.size() < 3 )
      {
      for ( size_t i = m_XYZInternalImages.size(); i < 3; i++ )
        {
        append_filter1->AddInput(m_XYZInternalImages[0]);
        append_filter2->AddInput(m_XYTInternalImages[0]);
        }
      }
    append_filter1->Update();
    append_filter2->Update();

    m_XYZImage->ShallowCopy( append_filter1->GetOutput() );
    m_XYTImage->ShallowCopy( append_filter2->GetOutput() );
    Update();
    }
  else
    {
    int ch = this->m_NavigationDockWidget->GetCurrentChannel();
    if ( ch != -1 )
      {
      m_XYZImage->ShallowCopy(m_XYZInternalImages[ch]);
      m_XYTImage->ShallowCopy(m_XYTInternalImages[ch]);
      Update();
      }
    }
}

//------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 * \brief
 * \param[in] iChannel
 */
void
QGoTabImageView4D::ShowOneChannel(int iChannel)
{
  if ( ( iChannel != -1 ) && ( !m_XYZInternalImages.empty() ) )
    {
    m_XYZImage->ShallowCopy(m_XYZInternalImages[iChannel]);
    m_XYTImage->ShallowCopy(m_XYTInternalImages[iChannel]);
    Update();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param iActivate
 */
void
QGoTabImageView4D::ActivateManualSegmentationEditor(const bool & iActivate)
{
  std::vector< vtkSmartPointer< vtkContourWidget > >::iterator
    it = m_ContourWidget.begin();
  while ( it != m_ContourWidget.end() )
    {
    if ( iActivate )
      {
      ( *it )->On();
      }
    else
      {
      ( *it )->Off();
      }
    ++it;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView4D::ChangeContourRepresentationProperty()
{
  double linewidth = 0.0; // = m_ManualSegmentationWidget->GetLinesWidth();
  QColor linecolor;       // = m_ManualSegmentationWidget->GetLinesColor();
  QColor nodecolor;       // = m_ManualSegmentationWidget->GetNodesColor();
  QColor activenodecolor; // =
                          // m_ManualSegmentationWidget->GetActiveNodesColor();

  double rl, gl, bl;

  linecolor.getRgbF(&rl, &gl, &bl);

  double rn, gn, bn;
  nodecolor.getRgbF(&rn, &gn, &bn);

  double ra, ga, ba;
  activenodecolor.getRgbF(&ra, &ga, &ba);

  for ( unsigned int i = 0; i < m_ContourRepresentation.size(); i++ )
    {
    m_ContourRepresentation[i]->GetLinesProperty()->SetLineWidth(linewidth);
    m_ContourRepresentation[i]->GetLinesProperty()->SetColor(rl, gl, bl);

    m_ContourRepresentation[i]->GetProperty()->SetColor(rn, gn, bn);
    m_ContourRepresentation[i]->GetActiveProperty()->SetColor(ra, ga, ba);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView4D::ValidateContour()
{
  for ( unsigned int i = 0; i < m_ContourWidget.size(); i++ )
    {
    ValidateContour(i);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param iId
 */
void
QGoTabImageView4D::ValidateContour(const int & iId)
{
  vtkPolyData *contour =
    m_ContourRepresentation[iId]->GetContourRepresentationAsPolyData();

  if ( ( contour->GetNumberOfPoints() > 2 ) && ( m_TimePoint >= 0 ) )
    {
    // get color from the dock widget
    double r(1.), g(1.), b(1.);
    /// \todo to be fixed!
    //QColor color; //= m_ManualSegmentationWidget->GetValidatedColor();
    //iColor.getRgbF(&r, &g, &b);

    vtkProperty *contour_property = vtkProperty::New();
    contour_property->SetRepresentationToWireframe();
    /** \todo: how to get the color*/
    contour_property->SetColor(r, g, b);

    // Compute Bounding Box
    double bounds[6];
    contour->GetBounds(bounds);

    std::cout << bounds[0] << " " << bounds[1] << std::endl;
    std::cout << bounds[2] << " " << bounds[3] << std::endl;
    std::cout << bounds[4] << " " << bounds[5] << std::endl;

    // Extract Min and Max from bounds
    double       Min[3], Max[3];
    int          k = 0;
    unsigned int i;
    for ( i = 0; i < 3; i++ )
      {
      Min[i] = bounds[k++];
      Max[i] = bounds[k++];
      }

    // Unused?
    // int* min_idx = this->GetImageCoordinatesFromWorldCoordinates( Min );
    // int* max_idx = this->GetImageCoordinatesFromWorldCoordinates( Max );

    vtkPolyData *contour_nodes = vtkPolyData::New();
    m_ContourRepresentation[iId]->GetNodePolyData(contour_nodes);

    // get corresponding actor from visualization
    vtkPolyData *contour_copy = vtkPolyData::New();
    contour_copy->ShallowCopy(contour);

//     std::vector< vtkQuadricLODActor* > contour_actor =
    std::vector< vtkActor * > contour_actor =
      this->AddContour(contour_copy,
                       contour_property);

    // Save contour in database!
//       {
//       m_DataBaseTables->SaveContoursFromVisuInDB(min_idx[0],
//         min_idx[1],min_idx[2],m_TimePoint,max_idx[0],
//         max_idx[1],max_idx[2], contour_nodes);
//       }

    contour_copy->Delete();
    contour_property->Delete();

    // get meshid from the dock widget (SpinBox)
    // unsigned int meshid = m_ManualSegmentationWidget->GetMeshId();
    // unused?
    // unsigned int meshid =
    // this->m_NavigationDockWidget->GetCurrentCollectionID();
    // unsigned int timepoint = static_cast< unsigned int >( m_TimePoint );
    // bool highlighted = false;

    // fill the container
//     for( i = 0; i < contour_actor.size(); i++ )
//       {
//       ContourStructure temp( m_ContourId, contour_actor[i], contour_nodes,
//          meshid, timepoint, highlighted, r, g, b, i );
//       m_ContourContainer.insert( temp );
//       }

    m_ContourId++;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param[in] pos[]
 * \return
 */
int *
QGoTabImageView4D::GetImageCoordinatesFromWorldCoordinates(double iPos[3])
{
  return m_XYZImageView->GetImageCoordinatesFromWorldCoordinates(iPos);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param iId
 * \param dataset
 * \param property
 * \return
 */
// std::vector< vtkQuadricLODActor* >
std::vector< vtkActor * >
QGoTabImageView4D::AddContour(vtkPolyData *dataset,
                              vtkProperty *iProperty)
{
// Adding contour in xyt is not straightforward (it is not the same coordinates)
//  vtkViewImage2D* viewer = this->m_XYTImageView->GetImageViewer( 0 );
//  vtkViewImage3D* viewer3D = this->m_XYTImageView->GetImageViewer3D();

//   std::vector< vtkQuadricLODActor* > oActorVector =
// this->m_XYZImageView->AddContour( iId, dataset, iProperty );
  std::vector< vtkActor * > oActorVector = this->m_XYZImageView->AddContour(dataset, iProperty);

//  viewer->GetRenderer()->AddViewProp( oActorVector[0] );
//  viewer->Render();

//  viewer3D->GetRenderer()->AddViewProp( oActorVector[3] );
//  viewer3D->Render();

  return oActorVector;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView4D::ChangeBackgroundColor()
{
  double r, g, b;

  m_XYZImageView->GetBackgroundColor(r, g, b);
  m_BackgroundColor.setRgbF(r, g, b);

  QColor temp = QColorDialog::getColor( m_BackgroundColor,
                                        this, tr("Choose Background Color") );

  if ( temp != m_BackgroundColor )
    {
    m_BackgroundColor = temp;
    m_XYZImageView->SetBackgroundColor(m_BackgroundColor);
    m_XYTImageView->SetBackgroundColor(m_BackgroundColor);
    }
}

//-------------------------------------------------------------------------
