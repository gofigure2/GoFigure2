/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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
#include "QGoVisualizationDockWidget.h"
#include "QGoManualSegmentationDockWidget.h"
#include "GoDBCoordinateRow.h"

#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )

  #include "QGoVideoRecorder.h"

  #ifdef   ENABLEFFMPEG
    #include  "vtkFFMPEGRenderWindowRecorder.h"
  #endif /* ENABLEFFMPEG */

  #ifdef   ENABLEAVI
    #include  "vtkAVIRenderWindowRecorder.h"
  #endif /* ENABLEAVI */

#endif /* ENABLEVIDEORECORD */

#include "SnapshotHelper.h"

#include "GoDBContourRow.h"
#include "GoDBCoordinateRow.h"

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

#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QColorDialog>

#include <set>

//-------------------------------------------------------------------------
/**
 * \brief Default Constructor
 * \param iParent
 */
QGoTabImageView3DwT::
QGoTabImageView3DwT( QWidget* iParent ) :
  QGoTabElementBase( iParent ),
  m_LSMReader( 0 ),
  m_Image( 0 ),
  m_BackgroundColor( Qt::black ),
  m_TimePoint( -1 ),
  m_ContourId( 0 ),
  m_ReEditContourMode( false )
{
  m_Image = vtkSmartPointer< vtkImageData >::New();

  setupUi( this );

  m_DataBaseTables = new QGoPrintDatabase( this );

  m_MegaCaptureReader = itk::MegaCaptureReader::New();

  CreateVisuDockWidget();

  CreateManualSegmentationdockWidget();

  CreateDataBaseTablesConnection();

#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  CreateVideoRecorderWidget();
#endif

  CreateAllViewActions();

  CreateToolsActions();

  CreateBookmarkActions();

  CreateModeActions();

  ReadSettings();

  for( int i = 0; i < 3; i++ )
    {
    this->m_ContourRepresentation.push_back(
      vtkSmartPointer< vtkOrientedGlyphContourRepresentation >::New() );
    this->m_ContourRepresentation.back()->GetProperty()->SetColor( 0., 1., 1. );
    this->m_ContourRepresentation.back()->GetLinesProperty()->SetColor( 1., 0., 1. );
    this->m_ContourRepresentation.back()->GetActiveProperty()->SetColor( 1., 1., 0. );

    this->m_ContourWidget.push_back(
      vtkSmartPointer< vtkContourWidget >::New() );
    this->m_ContourWidget.back()->SetPriority( 10.0 );
    this->m_ContourWidget.back()->SetInteractor( m_ImageView->GetInteractor( i ) );
    this->m_ContourWidget.back()->Off();

    this->m_ContourWidget.back()->SetRepresentation( this->m_ContourRepresentation.back() );
    }

  ChangeContourRepresentationProperty();

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus*, QDockWidget* >(
      new QGoDockWidgetStatus( m_VisuDockWidget, Qt::LeftDockWidgetArea, true, true ), m_VisuDockWidget ) );
  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus*, QDockWidget* >(
      new QGoDockWidgetStatus( m_ManualSegmentationDockWidget, Qt::LeftDockWidgetArea, true, true ), m_ManualSegmentationDockWidget ) );
  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus*, QDockWidget* >(
      new QGoDockWidgetStatus( m_DataBaseTables, Qt::TopDockWidgetArea, true, true ), m_DataBaseTables ) );

#if defined ( ENABLEFFMPEG ) || defined (ENABLEAVI)
  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus*, QDockWidget* >(
      new QGoDockWidgetStatus( m_VideoRecorderWidget, Qt::LeftDockWidgetArea, true, true ), m_VideoRecorderWidget ) );
#endif
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 * \brief Destructor
 */
QGoTabImageView3DwT::
~QGoTabImageView3DwT( )
{
  if( !m_LSMReader.empty() )
    {
    if( m_LSMReader[0] )
      {
      m_LSMReader[0]->Delete();
      }
    }

  vtkImageData* temp = m_MegaCaptureReader->GetOutput();
  if( temp )
    {
    temp->Delete();
    temp = 0;
    }

  ContourMeshStructureMultiIndexContainer::iterator it = m_ContourMeshContainer.begin();
  ContourMeshStructureMultiIndexContainer::iterator end = m_ContourMeshContainer.end();

  std::set< vtkPolyData* > NodeSet;

  while( it != end )
    {
    NodeSet.insert( it->Nodes );
    it->Actor->Delete();
    ++it;
    }

  std::set< vtkPolyData* >::iterator NodeSetIt = NodeSet.begin();
  std::set< vtkPolyData* >::iterator NodeSetEnd = NodeSet.end();

  while( NodeSetIt != NodeSetEnd )
    {
    (*NodeSetIt)->Delete();
    ++NodeSetIt;
    }

#ifdef   ENABLEFFMPEG
  m_FFMPEGWriter->Delete();
#endif /* ENABLEFFMPEG */

#ifdef   ENABLEAVI
  m_AVIWriter->Delete();
#endif /* ENABLEAVI */
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 * \brief
 */
void
QGoTabImageView3DwT::
CreateManualSegmentationdockWidget()
{
  m_ManualSegmentationDockWidget = new QGoManualSegmentationDockWidget( this );

  QObject::connect( m_ManualSegmentationDockWidget, SIGNAL( ValidatePressed() ),
    this, SLOT( ValidateContour() ) );

  QObject::connect( m_ManualSegmentationDockWidget, SIGNAL( ReinitializePressed() ),
    this, SLOT( ReinitializeContour() ) );

  QObject::connect( m_ManualSegmentationDockWidget,
    SIGNAL( ContourRepresentationPropertiesChanged() ),
    this, SLOT( ChangeContourRepresentationProperty() ) );

  QAction* tempaction = m_ManualSegmentationDockWidget->toggleViewAction();

  QObject::connect( this->m_ManualSegmentationDockWidget, SIGNAL( visibilityChanged( bool ) ),
    this->m_ManualSegmentationDockWidget, SLOT( ActivateManualSegmentation( bool ) ) );

  QObject::connect( this->m_ManualSegmentationDockWidget, SIGNAL( visibilityChanged( bool ) ),
    this, SLOT( ActivateManualSegmentationEditor( bool ) ) );

  this->m_SegmentationActions.push_back( tempaction );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
CreateVisuDockWidget()
{
   /**
    * \todo update annotation on current slice (i.e. slice x/23) when moving through
    * x via this dockwidget
    */

  m_VisuDockWidget = new QGoVisualizationDockWidget( this, 4 );

  QObject::connect( m_VisuDockWidget, SIGNAL( XSliceChanged( int ) ),
    this, SLOT( SetSliceViewYZ( int ) ) );

  QObject::connect( this, SIGNAL( SliceViewYZChanged( int ) ),
    m_VisuDockWidget, SLOT( SetXSlice( int ) ) );

  QObject::connect( m_VisuDockWidget, SIGNAL( YSliceChanged( int ) ),
    this, SLOT( SetSliceViewXZ( int ) ) );

  QObject::connect( this, SIGNAL( SliceViewXZChanged( int ) ),
    m_VisuDockWidget, SLOT( SetYSlice( int ) ) );

  QObject::connect( m_VisuDockWidget, SIGNAL( ZSliceChanged( int ) ),
    this, SLOT( SetSliceViewXY( int ) ) );

  QObject::connect( this, SIGNAL( SliceViewXYChanged( int ) ),
    m_VisuDockWidget, SLOT( SetZSlice( int ) ) );

  QObject::connect( m_VisuDockWidget, SIGNAL( TSliceChanged( int ) ),
    this, SLOT( SetTimePoint( int ) ) );

  QObject::connect( this, SIGNAL( TimePointChanged( int ) ),
    m_VisuDockWidget, SLOT( SetTSlice( int ) ) );

  QObject::connect( m_VisuDockWidget, SIGNAL( ShowAllChannelsChanged( bool ) ),
    this, SLOT( ShowAllChannels( bool ) ) );

  QObject::connect( m_VisuDockWidget, SIGNAL( ShowOneChannelChanged( int ) ),
    this, SLOT( ShowOneChannel( int ) ) );
}

void
QGoTabImageView3DwT::
CreateDataBaseTablesConnection()
{
  QObject::connect( this->m_DataBaseTables,
    SIGNAL( PrintExistingColorsFromDB(std::list<std::pair<std::string,std::vector<int> > >) ),
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->ColorComboBox,
    SLOT( setExistingColors(std::list<std::pair<std::string,std::vector<int> > >) ) );

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( PrintExistingCollectionIDsFromDB(std::list<std::pair<std::string,QColor> >) ),
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget, 
    SLOT( SetCollectionID(std::list<std::pair<std::string,QColor> >) ) );

  QObject::connect( 
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->ColorComboBox,
    SIGNAL( NewColorToBeSaved(std::vector<std::string>)),
    this->m_DataBaseTables,
    SLOT( SaveNewColorInDB(std::vector<std::string> ) ) );

  QObject::connect( 
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->ColorIDCollectionComboBox,
    SIGNAL( NewCollectionToBeSaved()),
    this, SLOT( UpdateDBAndCollectionIDComboBoxForANewCreatedCollection() ) );

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( NeedToGetCurrentSelectedColor() ),
    this, SLOT(PassInfoForDBForCurrentSelectedColor()) );

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( NewCreatedCollection(QColor, QString) ),
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->ColorIDCollectionComboBox
    , SLOT( addColor(QColor, QString) ));

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( SelectionContoursToHighLightChanged() ),
    this, SLOT( HighLightContoursFromTable() ) );

 QObject::connect( this->m_DataBaseTables,
    SIGNAL( NeedCurrentSelectedCollectionID() ),
    this, SLOT( PassInfoForCurrentCollectionID() ) );

   QObject::connect( this->m_DataBaseTables,
    SIGNAL( DeletedCollection(unsigned int) ),
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->ColorIDCollectionComboBox,
    SLOT( DeleteCollectionID(unsigned int) ) );

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( TraceToReEdit( unsigned int ) ),
    this, SLOT( ReEditContour( unsigned int ) ) );

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( TracesToDeleteInVisu( std::list< int > ) ),
    this, SLOT( DeleteContoursFromTable( std::list< int > ) ) );

  QObject::connect(this->m_DataBaseTables,
    SIGNAL( ListCellTypesToUpdate(QStringList)),
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget,
    SLOT(SetListCellTypes(QStringList)));

  QObject::connect(
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget,
    SIGNAL( AddANewCellType()),this->m_DataBaseTables,
    SLOT(AddNewCellType()));

  QObject::connect(
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget,
    SIGNAL( DeleteCellType()),this->m_DataBaseTables,
    SLOT(DeleteCellType()));

  QObject::connect(this->m_DataBaseTables,
    SIGNAL( ListSubCellTypesToUpdate(QStringList)),
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget,
    SLOT(SetListSubCellTypes(QStringList)));

   QObject::connect(
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget,
    SIGNAL( AddANewSubCellType()),this->m_DataBaseTables,
    SLOT(AddNewSubCellType()));

  QObject::connect(
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget,
    SIGNAL( DeleteSubCellType()),this->m_DataBaseTables,
    SLOT(DeleteSubCellType()));
}
//-------------------------------------------------------------------------
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
//-------------------------------------------------------------------------

void
QGoTabImageView3DwT::
CreateVideoRecorderWidget()
{
  m_VideoRecorderWidget = new QGoVideoRecorder( this );

#ifdef ENABLEFFMPEG
  m_FFMPEGWriter = vtkFFMPEGRenderWindowRecorder::New();
  m_VideoRecorderWidget->SetMovieRecorder( m_FFMPEGWriter );
#endif /* ENABLEFFMPEG */

#ifdef ENABLEAVI
  m_AVIWriter = vtkAVIRenderWindowRecorder::New();
  m_VideoRecorderWidget->SetMovieRecorder( m_AVIWriter );
#endif /* ENABLEAVI */

  QObject::connect( this, SIGNAL( FullScreenViewChanged( int ) ),
        this,SLOT( SetRendererWindow( int )));

  QObject::connect( m_VideoRecorderWidget, SIGNAL( XSliceChanged( int ) ),
      this, SLOT( SetSliceViewYZ( int ) ) );
  QObject::connect( m_VideoRecorderWidget, SIGNAL( YSliceChanged( int ) ),
      this, SLOT( SetSliceViewXZ( int ) ) );
  QObject::connect( m_VideoRecorderWidget, SIGNAL( ZSliceChanged( int ) ),
      this, SLOT( SetSliceViewXY( int ) ) );
  QObject::connect( m_VideoRecorderWidget, SIGNAL( TSliceChanged( int ) ),
      this, SLOT( SetTimePoint( int ) ) );

  QObject::connect( m_VideoRecorderWidget, SIGNAL( FrameRateChanged( int ) ),
        this, SLOT( SetSpecificParametersFrameRate( int ) ) );
  QObject::connect( m_VideoRecorderWidget, SIGNAL( QualityChanged( int ) ),
        this, SLOT( SetSpecificParametersQuality( int ) ) );
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetRendererWindow(int iValue)
{

  if (iValue >= 1)
    {
    m_VideoRecorderWidget->SetRenderingWindow(
    m_ImageView->GetInteractor( iValue -1 )->GetRenderWindow() );
    }
  else
    {
    m_VideoRecorderWidget->SetRenderingWindow( NULL );
    }
}
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetSpecificParametersFrameRate(int iValue)
{

#ifdef ENABLEFFMPEG
  m_FFMPEGWriter->SetFrameRate( iValue );
  m_FFMPEGWriter->SetSpecificParameters();
#endif /* ENABLEFFMPEG */

#ifdef ENABLEAVI
  m_AVIWriter->SetFrameRate( iValue );
  m_AVIWriter->SetSpecificParameters();
#endif /* ENABLEAVI */

}
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetSpecificParametersQuality(int iValue)
{
#ifdef ENABLEFFMPEG
  m_FFMPEGWriter->SetVideoQuality( iValue );
  m_FFMPEGWriter->SetSpecificParameters();
#endif /* ENABLEFFMPEG */

#ifdef ENABLEAVI
  m_AVIWriter->SetVideoQuality( iValue );
  m_AVIWriter->SetSpecificParameters();
#endif /* ENABLEAVI */
}
#endif /* ENABLEVIDEORECORD */
//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
CreateAllViewActions()
{
  QActionGroup* group = new QActionGroup( this );

  QAction* QuadViewAction = new QAction( tr("Quad-View"), this );
  QuadViewAction->setCheckable( true );
  QuadViewAction->setChecked( true );

  QIcon quadviewicon;
  quadviewicon.addPixmap( QPixmap(QString::fromUtf8(":/fig/4views.png")),
    QIcon::Normal, QIcon::Off );
  QuadViewAction->setIcon(quadviewicon);

  group->addAction( QuadViewAction );

  this->m_ViewActions.push_back( QuadViewAction );

  QObject::connect( QuadViewAction, SIGNAL( triggered() ),
    this, SLOT( Quadview() ) );

  QAction* FullScreenXYAction = new QAction( tr( "Full-Screen XY" ), this );
  FullScreenXYAction->setCheckable( true );

  QIcon xyicon;
  xyicon.addPixmap( QPixmap(QString::fromUtf8(":/fig/xy.png")),
    QIcon::Normal, QIcon::Off );
  FullScreenXYAction->setIcon( xyicon );

  group->addAction( FullScreenXYAction );

  this->m_ViewActions.push_back( FullScreenXYAction );

  QObject::connect( FullScreenXYAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewXY() ) );

  QAction* FullScreenXZAction = new QAction( tr( "Full-Screen XZ" ), this );
  FullScreenXZAction->setCheckable( true );

  QIcon xzicon;
  xzicon.addPixmap( QPixmap(QString::fromUtf8(":/fig/zx.png")),
    QIcon::Normal, QIcon::Off );
  FullScreenXZAction->setIcon( xzicon );

  group->addAction( FullScreenXZAction );

  this->m_ViewActions.push_back( FullScreenXZAction );

  QObject::connect( FullScreenXZAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewXZ() ) );

  QAction* FullScreenYZAction = new QAction( tr( "Full-Screen YZ" ), this );
  FullScreenYZAction->setCheckable( true );

  QIcon yzicon;
  yzicon.addPixmap( QPixmap(QString::fromUtf8(":/fig/yz.png")),
    QIcon::Normal, QIcon::Off );
  FullScreenYZAction->setIcon( yzicon );

  group->addAction( FullScreenYZAction );

  this->m_ViewActions.push_back( FullScreenYZAction );

  QObject::connect( FullScreenYZAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewYZ() ) );

  QAction* FullScreenXYZAction = new QAction( tr( "Full-Screen XYZ" ), this );
  FullScreenXYZAction->setCheckable( true );

  QIcon xyzicon;
  xyzicon.addPixmap( QPixmap(QString::fromUtf8(":/fig/xyz.png")),
    QIcon::Normal, QIcon::Off );
  FullScreenXYZAction->setIcon( xyzicon );

  group->addAction( FullScreenXYZAction );

  this->m_ViewActions.push_back( FullScreenXYZAction );

  QObject::connect( FullScreenXYZAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewXYZ() ) );

  QAction* separator5 = new QAction( this );
  separator5->setSeparator( true );
  this->m_ViewActions.push_back( separator5 );

  QAction* ActionDisplayAnnotations =
    new QAction( tr( "Display annotations" ), this );
  ActionDisplayAnnotations->setCheckable( true );
  ActionDisplayAnnotations->setChecked( true );
  ActionDisplayAnnotations->setStatusTip( tr(" Display or not annotations in each 2d view" ) );

  QIcon displayannotationsicon;
  displayannotationsicon.addPixmap( QPixmap(QString::fromUtf8(":/fig/2D_VIEWS_INFOS.png")),
    QIcon::Normal, QIcon::Off );
  ActionDisplayAnnotations->setIcon(displayannotationsicon);

  QObject::connect( ActionDisplayAnnotations, SIGNAL( triggered() ),
    this, SLOT( DisplayAnnotations() ) );

  this->m_ViewActions.push_back( ActionDisplayAnnotations );

  QAction* ActionDisplaySplinePlanes =
    new QAction( tr( "Display spline planes" ), this );
  ActionDisplaySplinePlanes->setCheckable( true );
  ActionDisplaySplinePlanes->setChecked( true );
  ActionDisplaySplinePlanes->setStatusTip( tr(" Display or not spline planes on each view" ) );

  QIcon displaysplineplaneicon;
  displaysplineplaneicon.addPixmap( QPixmap(QString::fromUtf8(":/fig/C_M_L.png")),
      QIcon::Normal, QIcon::Off );
  ActionDisplaySplinePlanes->setIcon(displaysplineplaneicon);

  QObject::connect( ActionDisplaySplinePlanes, SIGNAL( triggered() ),
    this, SLOT( DisplaySplinePlanes() ) );

  this->m_ViewActions.push_back( ActionDisplaySplinePlanes );

  QAction* DisplayCube3D = new QAction( tr( "Display 3D cube" ), this );
  DisplayCube3D->setCheckable( true );
  DisplayCube3D->setChecked( true );
  DisplayCube3D->setStatusTip( tr(" Display or not cube in 3d" ) );

  QIcon cube3dicon;
  cube3dicon.addPixmap( QPixmap(QString::fromUtf8(":/fig/cube.png")),
    QIcon::Normal, QIcon::Off );
  DisplayCube3D->setIcon( cube3dicon );

  QObject::connect( DisplayCube3D, SIGNAL( triggered() ),
    this, SLOT( DisplayCube() ) );

  this->m_ViewActions.push_back( DisplayCube3D );

  QAction* separator = new QAction( this );
  separator->setSeparator( true );

  this->m_ViewActions.push_back( separator );

  QAction* LookupTableAction = new QAction( tr( "Lookup Table" ), this );
  LookupTableAction->setStatusTip( tr(" Change the associated lookup table" ) );

	//take
  QIcon luticon;
  luticon.addPixmap( QPixmap(QString::fromUtf8(":/fig/LookupTable.png")),
    QIcon::Normal, QIcon::Off );
  LookupTableAction->setIcon( luticon );

  // Here write the connection
  QObject::connect( LookupTableAction, SIGNAL( triggered() ),
    this, SLOT( ChangeLookupTable() ) );

  this->m_ViewActions.push_back( LookupTableAction );

  QAction* ScalarBarAction = new QAction( tr( "Display Scalar Bar" ), this );
  ScalarBarAction->setCheckable( true );

  QIcon scalarbaricon;
  scalarbaricon.addPixmap( QPixmap(QString::fromUtf8(":/fig/scalarbar.png")),
    QIcon::Normal, QIcon::Off );
  ScalarBarAction->setIcon( scalarbaricon );

  this->m_ViewActions.push_back( ScalarBarAction );

  QObject::connect( ScalarBarAction, SIGNAL( toggled( bool ) ),
    this, SLOT( ShowScalarBar( bool ) ) );

  QPixmap Pix(16, 16);
  Pix.fill(Qt::black);
  m_BackgroundColorAction = new QAction(Pix, tr("Set Background Color"), this );
  this->m_ViewActions.push_back( m_BackgroundColorAction );

  QObject::connect( m_BackgroundColorAction, SIGNAL( triggered() ),
    this, SLOT( ChangeBackgroundColor() ) );

  QAction* separator2 = new QAction( this );
  separator2->setSeparator( true );
  this->m_ViewActions.push_back( separator2 );

  this->m_ViewActions.push_back( m_VisuDockWidget->toggleViewAction() );

  QAction* separator3 = new QAction( this );
  separator3->setSeparator( true );
  this->m_ViewActions.push_back( separator3 );

  this->m_ViewActions.push_back( m_DataBaseTables->toggleViewAction() );

  QAction* LoadContoursPerTimePointAction =
    new QAction( tr( "Load All Contours For Current Time Point" ), this );
  this->m_ViewActions.push_back( LoadContoursPerTimePointAction );

  QObject::connect( LoadContoursPerTimePointAction, SIGNAL( triggered() ),
    this, SLOT( LoadAllContoursForCurrentTimePoint() ) );

  QAction* separator4 = new QAction( this );
    separator4->setSeparator( true );
    this->m_ViewActions.push_back( separator4 );

    QAction* Change3DPerspectiveToAxialAction =
      new QAction( tr( "Change 3D view to Posterior " ), this );
    this->m_ViewActions.push_back( Change3DPerspectiveToAxialAction );

    QIcon axialicon;
    axialicon.addPixmap( QPixmap(QString::fromUtf8(":/fig/PosteriorView.png")),
        QIcon::Normal, QIcon::Off );
      Change3DPerspectiveToAxialAction->setIcon( axialicon );

    QObject::connect( Change3DPerspectiveToAxialAction, SIGNAL( triggered() ),
      this, SLOT( Change3DPerspectiveToAxial( ) ) );


    QAction* Change3DPerspectiveToCoronalAction =
      new QAction( tr( "Change 3D view to Dorsal " ), this );
    this->m_ViewActions.push_back( Change3DPerspectiveToCoronalAction );

    QIcon coronalicon;
    coronalicon.addPixmap( QPixmap(QString::fromUtf8(":/fig/DorsalView.png")),
            QIcon::Normal, QIcon::Off );
    Change3DPerspectiveToCoronalAction->setIcon( coronalicon );

    QObject::connect( Change3DPerspectiveToCoronalAction, SIGNAL( triggered() ),
      this, SLOT( Change3DPerspectiveToCoronal( ) ) );


    QAction* Change3DPerspectiveToSagittalAction =
      new QAction( tr( "Change 3D view to Left " ), this );
    this->m_ViewActions.push_back( Change3DPerspectiveToSagittalAction );

    QIcon sagittalicon;
    sagittalicon.addPixmap( QPixmap(QString::fromUtf8(":/fig/LeftView.png")),
                QIcon::Normal, QIcon::Off );
    Change3DPerspectiveToSagittalAction->setIcon( sagittalicon );

    QObject::connect( Change3DPerspectiveToSagittalAction, SIGNAL( triggered() ),
      this, SLOT( Change3DPerspectiveToSagittal( ) ) );
}

/**
 *
 */
void
QGoTabImageView3DwT::
CreateToolsActions()
{
#if defined( ENABLEFFMPEG ) || defined( ENABLEAVI )
  this->m_ToolsActions.push_back( m_VideoRecorderWidget->toggleViewAction() );
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(false);
#endif

  m_TakeSnapshotAction = new QAction( tr( "Take Snapshot" ), this );
  QIcon snapshoticon;
  snapshoticon.addPixmap( QPixmap(QString::fromUtf8(":/fig/camera-photo.png")),
    QIcon::Normal, QIcon::Off );
  m_TakeSnapshotAction->setIcon( snapshoticon );
  m_TakeSnapshotAction->setStatusTip(tr("You have to be in full screen view to use the snapshot"));
  m_TakeSnapshotAction->setEnabled(false);
  QObject::connect( m_TakeSnapshotAction, SIGNAL( triggered() ),
                    this, SLOT( TakeSnapshot() ) );

  this->m_ToolsActions.push_back( m_TakeSnapshotAction );
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void QGoTabImageView3DwT::CreateModeActions()
{
  QActionGroup* group = new QActionGroup( this );

  QAction* ManualEditingAction = new QAction( tr("Manual-Editing"), this );
  ManualEditingAction->setCheckable( true );
  QIcon ManualEditingIcon;
  ManualEditingIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/manual-editing.png")),
    QIcon::Normal, QIcon::Off );
  ManualEditingAction->setIcon(ManualEditingIcon);

  group->addAction( ManualEditingAction );

  this->m_ModeActions.push_back( ManualEditingAction );
  /** \todo implement the manual editing mode*/
  //QObject::connect( ManualEditingAction, SIGNAL( triggered() ),
  //  this, SLOT( ManualEditingMode() ) );

  QAction* DefaultAction = new QAction( tr( "Default" ), this );
  DefaultAction->setCheckable( true );
  DefaultAction->setChecked(true);
  
  QIcon DefaultIcon;
  DefaultIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/mouse-cursor.png")),
    QIcon::Normal, QIcon::Off );
  DefaultAction->setIcon( DefaultIcon );

  group->addAction( DefaultAction );

  this->m_ModeActions.push_back( DefaultAction );
  /** \todo implement default mode*/
  //QObject::connect( DefaultAction, SIGNAL( triggered() ),
   // this, SLOT( DefaultMode() ) );
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void QGoTabImageView3DwT::CreateBookmarkActions()
{
  QAction* AddBookmarkAction = new QAction(tr("Add a bookmark"),this);
  this->m_BookmarkActions.push_back(AddBookmarkAction);
  QObject::connect(AddBookmarkAction, SIGNAL ( triggered() ),
    this, SLOT (AddBookmark()));
  QAction* DeleteBookmarkAction = new QAction(tr("Delete a bookmark"),this);
  this->m_BookmarkActions.push_back(DeleteBookmarkAction);
  QObject::connect(DeleteBookmarkAction,SIGNAL ( triggered() ),
    this->m_DataBaseTables,SLOT( DeleteBookmarks()));
  QObject::connect(this->m_DataBaseTables, SIGNAL (PrintDBReady()),
    this, SLOT(GetTheOpenBookmarksActions()));
  QObject::connect(this->m_DataBaseTables, SIGNAL(OpenBookmarksToUpdate()),
    this, SLOT(GetTheOpenBookmarksActions()));
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
  for( size_t i = 0; i < NumberBookmarks; i++ )
    {
    QAction* OpenBookmarkAction 
      = new QAction( ListBookmarks[i].first.c_str(), this );
    std::string TextStatusTip = "Description of the bookmark: ";
    TextStatusTip += ListBookmarks[i].second;
    OpenBookmarkAction->setStatusTip( TextStatusTip.c_str() );
    OpenBookmarkMenu->addAction( OpenBookmarkAction );
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
  QAction* taction = qobject_cast< QAction* >( sender() );
  std::string BookmarkName = taction->text().toStdString();
  GoDBCoordinateRow Coord = this->m_DataBaseTables->GetCoordinateForBookmark(BookmarkName);
  this->SetTimePoint(atoi(Coord.GetMapValue("TCoord").c_str()));
  this->SetSliceViewXY(atoi(Coord.GetMapValue("ZCoord").c_str()));
  this->SetSliceViewXZ(atoi(Coord.GetMapValue("YCoord").c_str()));
  this->SetSliceViewYZ(atoi(Coord.GetMapValue("XCoord").c_str()));
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void
QGoTabImageView3DwT::
TakeSnapshot()
{
  // Get the current view displayed in full screen
  int FullScreenView = m_ImageView->GetFullScreenView();

  // TODO enhance the name of the files

  switch ( FullScreenView )
    {
    case 1 :
    // X Slice
    m_ImageView->SnapshotViewXY( GoFigure::PNG , "snapshot_" );
    break;

    case 2 :
    // Y Slice
    m_ImageView->SnapshotView2( GoFigure::PNG , "snapshot_" );
    break;

    case 3 :
    // Z Slice
    m_ImageView->SnapshotView3( GoFigure::PNG , "snapshot_" );
    break;

    default :
    // 3D view
    m_ImageView->SnapshotViewXYZ( GoFigure::PNG , "snapshot_" );
    }
}

//-------------------------------------------------------------------------
/**
 * \brief
 * \param iParent
 */
void
QGoTabImageView3DwT::
setupUi( QWidget* iParent )
{
  if(iParent->objectName().isEmpty())
    {
    iParent->resize(800, 800);
    }

  m_ImageView = new QGoImageView3D( this );
  m_ImageView->SetBackgroundColor( m_BackgroundColor );

  QObject::connect( m_ImageView, SIGNAL( SliceViewXYChanged( int ) ),
    this, SIGNAL( SliceViewXYChanged( int ) ) );

  QObject::connect( m_ImageView, SIGNAL( SliceViewXZChanged( int ) ),
    this, SIGNAL( SliceViewXZChanged( int ) ) );

  QObject::connect( m_ImageView, SIGNAL( SliceViewYZChanged( int ) ),
    this, SIGNAL( SliceViewYZChanged( int ) ) );

  QObject::connect( m_ImageView, SIGNAL( FullScreenViewChanged( int ) ),
    this, SIGNAL( FullScreenViewChanged( int ) ) );

  QObject::connect( m_ImageView, SIGNAL( ActorsSelectionChanged( ) ),
    this, SLOT( HighLightContours() ) );
  QObject::connect( m_ImageView, SIGNAL( ActorsSelectionChanged( ) ),
    this, SLOT( SelectContoursInTable() ) );

  m_LayOut = new QHBoxLayout( iParent );
  m_LayOut->addWidget( m_ImageView  );

  retranslateUi(iParent);

  QMetaObject::connectSlotsByName(iParent);
} // setupUi
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param iParent
 */
void
QGoTabImageView3DwT::
retranslateUi(QWidget *iParent)
{
  iParent->setWindowTitle( tr( "QGoTabImageView3DwT" ) );
  Q_UNUSED(iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \return
 */
GoFigure::TabDimensionType
QGoTabImageView3DwT::
GetTabDimensionType( ) const
{
  return GoFigure::THREE_D_WITH_T;
}
//-------------------------------------------------------------------------

//#########################################################################
//#########################################################################
// Set Inputs

//-------------------------------------------------------------------------
/**
 * \brief
 * \param[in] iReader
 * \param[in] iTimePoint
 */
void
QGoTabImageView3DwT::
SetLSMReader( vtkLSMReader* iReader, const int& iTimePoint )
{
  if( iReader )
    {
    if( m_LSMReader.empty() )
      {
      m_LSMReader.push_back( iReader );
      }
    else
      {
      if( iReader != m_LSMReader[0] )
        {
        m_LSMReader[0] = iReader;
        }
      }

    m_LSMReader[0]->Update();

    int dim[5];
    m_LSMReader[0]->GetDimensions( dim );

    int NumberOfChannels = m_LSMReader[0]->GetNumberOfChannels();

    m_VisuDockWidget->SetNumberOfChannels( NumberOfChannels );

    if( NumberOfChannels > 1 )
      {
      m_VisuDockWidget->SetChannel( 0 );
      m_InternalImages.resize( NumberOfChannels );

      for( int i = 1; i < NumberOfChannels; i++ )
        {
        m_VisuDockWidget->SetChannel( i );

        m_LSMReader.push_back( vtkSmartPointer< vtkLSMReader >::New() );
        m_LSMReader.back()->SetFileName( m_LSMReader[0]->GetFileName() );
        m_LSMReader.back()->SetUpdateChannel( i );
        }
      }

    m_VisuDockWidget->SetXMinimumAndMaximum( 0, dim[0] - 1 );
    m_VisuDockWidget->SetXSlice( ( dim[0] - 1 ) / 2 );

    m_VisuDockWidget->SetYMinimumAndMaximum( 0, dim[1] - 1 );
    m_VisuDockWidget->SetYSlice( ( dim[1] - 1 ) / 2 );

    m_VisuDockWidget->SetZMinimumAndMaximum( 0, dim[2] - 1 );
    m_VisuDockWidget->SetZSlice( ( dim[2] - 1 ) / 2 );

    m_VisuDockWidget->SetTMinimumAndMaximum( 0, dim[3] - 1 );
    m_VisuDockWidget->SetTSlice( iTimePoint );
    if( m_TimePoint != iTimePoint ) 
      { 
      SetTimePoint( iTimePoint ); 
      } 

#if defined( ENABLEFFMPEG ) || defined( ENABLEAVI )
      m_VideoRecorderWidget->SetXMinAndMax( 0, dim[0] - 1 );
      m_VideoRecorderWidget->SetYMinAndMax( 0, dim[1] - 1 );
      m_VideoRecorderWidget->SetZMinAndMax( 0, dim[2] - 1 );
      m_VideoRecorderWidget->SetTMinAndMax( 0, dim[3] - 1 );
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
  const unsigned int& iTimePoint  )
{
  m_FileList = iContainer;
  m_FileType = iFileType;

  m_MegaCaptureReader->SetInput( m_FileList );
  m_MegaCaptureReader->SetMegaCaptureHeader( iHeader );
  m_MegaCaptureReader->SetFileType( m_FileType );
  m_MegaCaptureReader->SetTimeBased( true );
  m_MegaCaptureReader->SetTimePoint( iTimePoint );
  m_MegaCaptureReader->SetChannel( 0 );
  m_MegaCaptureReader->Update();

  unsigned int min_t = m_MegaCaptureReader->GetMinTimePoint();
  unsigned int max_t = m_MegaCaptureReader->GetMaxTimePoint();

  unsigned int min_ch = m_MegaCaptureReader->GetMinChannel();
  unsigned int max_ch = m_MegaCaptureReader->GetMaxChannel();

  unsigned int NumberOfChannels = max_ch - min_ch + 1;

  vtkImageData* temp = m_MegaCaptureReader->GetOutput();

  int extent[6];
  temp->GetExtent( extent );

  m_VisuDockWidget->SetNumberOfChannels( NumberOfChannels );

  // Set up QSpinBox in m_VideoRecorderWidget
  if( NumberOfChannels > 1 )
    {
    m_VisuDockWidget->SetChannel( 0 );
    m_InternalImages.resize( NumberOfChannels, NULL );

    for( unsigned int i = 1; i < NumberOfChannels; i++ )
      {
      m_VisuDockWidget->SetChannel( i );
      }
    }

  m_VisuDockWidget->SetXMinimumAndMaximum( extent[0], extent[1] );
  m_VisuDockWidget->SetXSlice( ( extent[0] + extent[1] ) / 2 );

  m_VisuDockWidget->SetYMinimumAndMaximum( extent[2], extent[3] );
  m_VisuDockWidget->SetYSlice( ( extent[2] + extent[3] ) / 2 );

  m_VisuDockWidget->SetZMinimumAndMaximum( extent[4], extent[5] );
  m_VisuDockWidget->SetZSlice( ( extent[4] + extent[5] ) / 2 );

  m_VisuDockWidget->SetTMinimumAndMaximum( min_t, max_t );
  m_VisuDockWidget->SetTSlice( iTimePoint );

  if( static_cast< unsigned int >( m_TimePoint ) != iTimePoint ) 
    { 
	 	    SetTimePoint( iTimePoint ); 
    } 

  // Set up QSpinBox in m_VideoRecorderWidget
#if defined( ENABLEFFMPEG ) || defined( ENABLEAVI )
  m_VideoRecorderWidget->SetXMinAndMax( extent[0], extent[1] );
  m_VideoRecorderWidget->SetYMinAndMax( extent[2], extent[3] );
  m_VideoRecorderWidget->SetZMinAndMax( extent[4], extent[5] );
  m_VideoRecorderWidget->SetTMinAndMax( min_t, max_t );
#endif /* ENABLEVIDEORECORD */
}
//-------------------------------------------------------------------------


//#########################################################################
//#########################################################################

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetTimePointWithMegaCapture( const int& iTimePoint )
{
  RemoveAllContoursForPresentTimePoint();

  m_TimePoint = iTimePoint;
  m_MegaCaptureReader->SetChannel( 0 );
  m_MegaCaptureReader->SetTimePoint( m_TimePoint );

  unsigned int min_ch = m_MegaCaptureReader->GetMinChannel();
  unsigned int max_ch = m_MegaCaptureReader->GetMaxChannel();

  int NumberOfChannels = max_ch - min_ch + 1;

  if( NumberOfChannels > 1 )
    {
    m_MegaCaptureReader->Update();

    vtkSmartPointer< vtkImageAppendComponents > append_filter =
      vtkSmartPointer< vtkImageAppendComponents >::New();

    for( int i = 0; i < NumberOfChannels; i++ )
      {
      m_MegaCaptureReader->SetChannel( i );
      m_MegaCaptureReader->Update();

      if( !m_InternalImages[i] )
        {
        m_InternalImages[i] = vtkSmartPointer< vtkImageData >::New();
        }
      m_InternalImages[i]->ShallowCopy( m_MegaCaptureReader->GetOutput() );
      append_filter->AddInput( m_InternalImages[i] );
      }
    // This is really stupid!!!
    if( NumberOfChannels < 3 )
      {
      for( int i = NumberOfChannels; i < 3; i++ )
        {
        append_filter->AddInput( m_InternalImages[0] );
        }
        }
    append_filter->Update();

    if( this->m_VisuDockWidget->ShowAllChannels() )
      {
      m_Image->ShallowCopy( append_filter->GetOutput() );
      }
    else
      {
      int ch = this->m_VisuDockWidget->GetCurrentChannel();
      if( ch != -1 )
        {
    	m_Image->ShallowCopy( m_InternalImages[ch] );
    	}
      }
    }
  else
    {
    m_MegaCaptureReader->Update();

    m_Image->ShallowCopy( m_MegaCaptureReader->GetOutput() );
    }


  LoadAllContoursForGivenTimePoint( m_TimePoint );
  Update();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetTimePointWithLSMReaders( const int& iTimePoint )
{
  RemoveAllContoursForPresentTimePoint();

  m_TimePoint = iTimePoint;
  m_LSMReader[0]->SetUpdateTimePoint( m_TimePoint );

  int NumberOfChannels = m_LSMReader[0]->GetNumberOfChannels();

  if( NumberOfChannels > 1 )
    {
    m_InternalImages[0] = m_LSMReader[0]->GetOutput();

    vtkSmartPointer< vtkImageAppendComponents > append_filter =
      vtkSmartPointer< vtkImageAppendComponents >::New();
    append_filter->AddInput( m_InternalImages[0] );

    for( int i = 1; i < NumberOfChannels; i++ )
      {
      m_LSMReader[i]->SetUpdateTimePoint( m_TimePoint );
      m_LSMReader[i]->Update();

      m_InternalImages[i] = m_LSMReader[i]->GetOutput();
      append_filter->AddInput( m_InternalImages[i] );
      }
    // This is really stupid!!!
    if( NumberOfChannels < 3 )
      {
      for( int i = NumberOfChannels; i < 3; i++ )
        {
        append_filter->AddInput( m_InternalImages[0] );
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

  LoadAllContoursForGivenTimePoint( m_TimePoint );
  Update();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param iTimePoint
 */
void
QGoTabImageView3DwT::
SetTimePoint( const int& iTimePoint )
{
  if( iTimePoint == m_TimePoint )
    {
    return;
    }

  if( !m_LSMReader.empty() )
    {
    if( iTimePoint >= m_LSMReader[0]->GetNumberOfTimePoints() )
      {
      return;
      }
    else
      {
      SetTimePointWithLSMReaders( iTimePoint );
      emit TimePointChanged( m_TimePoint );
      }
    }
  else
    {
    if( !m_FileList.empty() )
      {
      unsigned int t = static_cast< unsigned int >( iTimePoint );
      if( ( t < m_MegaCaptureReader->GetMinTimePoint() ) ||
          ( t > m_MegaCaptureReader->GetMaxTimePoint() ) )
        {
        return;
        }
      else
        {
        SetTimePointWithMegaCapture( iTimePoint );
        emit TimePointChanged( m_TimePoint );
        }
      }
    else
      {
      // no lsm reader, no file list. did you really provide any input?
      std::cerr <<"No lsm reader. No file list" <<std::endl;
      return;
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3DwT::Update()
{
  m_ImageView->SetImage( m_Image );
  m_ImageView->Update();

  for( unsigned int i = 0; i < this->m_ContourWidget.size(); i++ )
    {
    vtkSmartPointer< vtkImageActorPointPlacer > point_placer =
      vtkSmartPointer< vtkImageActorPointPlacer >::New();
    point_placer->SetImageActor( m_ImageView->GetImageActor( i ) );

    this->m_ContourRepresentation[i]->SetPointPlacer( point_placer );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
ChangeLookupTable()
{
  vtkImageData* image = m_ImageView->GetImage();

  if( image->GetNumberOfScalarComponents() == 1 )
    {
    /// \todo there may be one memory leak in here!
    vtkSmartPointer< vtkLookupTable > lut =
      vtkSmartPointer< vtkLookupTable >::New();
    lut->DeepCopy( QGoLUTDialog::GetLookupTable( this,
      tr( "Choose one look-up table") ) );
    m_ImageView->SetLookupTable( lut );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param[in] iShow
 */
void
QGoTabImageView3DwT::
ShowScalarBar( const bool& iShow )
{
  vtkImageData* image = m_ImageView->GetImage();

  if( image->GetNumberOfScalarComponents() == 1 )
    {
    m_ImageView->ShowScalarBar( iShow );
    }
}
//------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param[in] iType
 * \param[in] iBaseName
 * \return
 */
QString
QGoTabImageView3DwT::
SnapshotViewXY(
  const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotViewXY( iType, iBaseName );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param[in] iType
 * \param[in] iBaseName
 * \return
 */
QString
QGoTabImageView3DwT::
SnapshotView2(
  const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotView2( iType, iBaseName );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param[in] iType
 * \param[in] iBaseName
 * \return
 */
QString
QGoTabImageView3DwT::
SnapshotView3(
  const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotView3( iType, iBaseName );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param[in] iType
 * \param[in] iBaseName
 * \return
 */
QString
QGoTabImageView3DwT::
SnapshotViewXYZ(
  const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotViewXYZ( iType, iBaseName );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param[in] iS
 */
void
QGoTabImageView3DwT::
SetSliceViewXY( const int& iS )
{
  m_ImageView->SetSliceViewXY( iS );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param[in] iS
 */
void
QGoTabImageView3DwT::
SetSliceViewXZ( const int& iS )
{
  m_ImageView->SetSliceViewXZ( iS );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param[in] iS
 */
void
QGoTabImageView3DwT::
SetSliceViewYZ( const int& iS )
{
  m_ImageView->SetSliceViewYZ( iS );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param[in] iS
 */
void
QGoTabImageView3DwT::
SetFullScreenView( const int& iS )
{
  m_ImageView->SetFullScreenView( iS );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
Quadview()
{
  m_ImageView->SetFullScreenView(0);
  m_TakeSnapshotAction->setEnabled(false);
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(false);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
FullScreenViewXY()
{
  m_ImageView->SetFullScreenView(1);
  m_TakeSnapshotAction->setEnabled(true);
   this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
FullScreenViewXZ()
{
  m_ImageView->SetFullScreenView(2);
  m_TakeSnapshotAction->setEnabled(true);
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
FullScreenViewYZ()
{
  m_ImageView->SetFullScreenView(3);
  m_TakeSnapshotAction->setEnabled(true);
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
FullScreenViewXYZ()
{
  m_ImageView->SetFullScreenView(4);
  m_TakeSnapshotAction->setEnabled(true);
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
GetBackgroundColorFromImageViewer( )
{
  double r, g, b;
  m_ImageView->GetBackgroundColor( r, g, b );
  m_BackgroundColor.setRgbF( r, g, b );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
SetBackgroundColorToImageViewer( )
{
  m_ImageView->SetBackgroundColor( m_BackgroundColor );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
ChangeBackgroundColor()
{
  double r, g, b;
  m_ImageView->GetBackgroundColor( r, g, b );
  m_BackgroundColor.setRgbF( r, g, b );

  QColor temp = QColorDialog::getColor( m_BackgroundColor,
    this, tr( "Choose Background Color" ) );

  if( temp != m_BackgroundColor )
    {
    m_BackgroundColor = temp;
    m_ImageView->SetBackgroundColor( m_BackgroundColor );
    QPixmap Pix(16, 16);
    Pix.fill(temp);
    m_BackgroundColorAction->setIcon(Pix);
    }
}

//-------------------------------------------------------------------------
/**
 *
 * \param iChecked
 */
void
QGoTabImageView3DwT::
ShowAllChannels( bool iChecked )
{
  if( iChecked )
    {
    vtkSmartPointer< vtkImageAppendComponents > append_filter =
      vtkSmartPointer< vtkImageAppendComponents >::New();

    for( unsigned int i = 0; i < m_InternalImages.size(); i++ )
      {
      append_filter->AddInput( m_InternalImages[i] );
      }

    // This is really stupid!!!
    if( m_InternalImages.size() < 3 )
      {
      for( size_t i = m_InternalImages.size(); i < 3; i++ )
        {
        append_filter->AddInput( m_InternalImages[0] );
        }
      }
    append_filter->Update();

    m_Image->ShallowCopy( append_filter->GetOutput() );
    Update();
    }
  else
    {
    int ch = this->m_VisuDockWidget->GetCurrentChannel();
    if( ch != -1 )
      {
      m_Image->ShallowCopy( m_InternalImages[ch] );
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
QGoTabImageView3DwT::
ShowOneChannel( int iChannel )
{
  if( ( iChannel != -1 ) && ( !m_InternalImages.empty() ) )
    {
    m_Image->ShallowCopy( m_InternalImages[iChannel] );
    Update();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param iId
 */
void
QGoTabImageView3DwT::
ValidateContour( const int& iContourID, const int& iDir,
  const double& iR, const double& iG, const double& iB, const double& iA,
  const bool& iHighlighted, const unsigned int& iTCoord,
  const bool& iSaveInDataBase )
{
  vtkPolyData* contour =
    m_ContourRepresentation[iDir]->GetContourRepresentationAsPolyData();

  if( ( contour->GetNumberOfPoints() > 2 ) && ( m_TimePoint >= 0 ) )
    {
    // Compute Bounding Box
    double bounds[6];
    contour->GetBounds( bounds );

    // Extract Min and Max from bounds
    double Min[3], Max[3];
    int k = 0;
    unsigned int i;
    for( i = 0; i < 3; i++ )
      {
      Min[i] = bounds[k++];
      Max[i] = bounds[k++];
      }

    int* min_idx = this->GetImageCoordinatesFromWorldCoordinates( Min );
    int* max_idx = this->GetImageCoordinatesFromWorldCoordinates( Max );

    vtkPolyData* contour_nodes = vtkPolyData::New();
    m_ContourRepresentation[iDir]->GetNodePolyData( contour_nodes );

    vtkProperty* contour_property = vtkProperty::New();
    contour_property->SetColor( iR, iG, iB );

    // get corresponding actor from visualization
    vtkPolyData* contour_copy = vtkPolyData::New();
    contour_copy->ShallowCopy( contour );

//     std::vector< vtkQuadricLODActor* > contour_actor =
    std::vector< vtkActor* > contour_actor =
      this->AddContour( iDir, contour_copy,
        contour_property );

    // get meshid from the visu dock widget (SpinBox)
    //unsigned int meshid = m_ManualSegmentationDockWidget->GetMeshId();
    unsigned int meshid = 
      this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->GetCurrentCollectionID();

    if( iSaveInDataBase )
      {
      if( !m_ReEditContourMode )
        {
        std::pair< std::string, QColor > ColorData =
          this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->ColorComboBox->GetCurrentColorData();

        // Save contour in database!
        m_ContourId = m_DataBaseTables->SaveContoursFromVisuInDB( min_idx[0],
          min_idx[1], min_idx[2], iTCoord, max_idx[0],
          max_idx[1], max_idx[2], contour_nodes, ColorData, meshid );
        }
      else
        {
        m_ContourId = iContourID;

        m_DataBaseTables->UpdateContourFromVisuInDB( min_idx[0],
          min_idx[1], min_idx[2], iTCoord, max_idx[0],
          max_idx[1], max_idx[2], contour_nodes, m_ContourId );
        this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->SetEnableTraceCollectionColorBoxes(true);
        }
      }
    else
      {
      if( iContourID != -1 )
        {
        m_ContourId = iContourID;
        }
      }

    contour_copy->Delete();
    contour_property->Delete();

    // fill the container
    for( i = 0; i < contour_actor.size(); i++ )
      {
      ContourMeshStructure temp( m_ContourId, reinterpret_cast< vtkActor* >( contour_actor[i] ),
        contour_nodes, meshid, iTCoord, iHighlighted, iR, iG, iB, iA, i );
      m_ContourMeshContainer.insert( temp );
      }

    if( ( !iSaveInDataBase ) && ( iContourID != -1 ) )
      {
      ++m_ContourId;
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
ValidateContour( )
{
  // get color from the dock widget
  double r, g, b, a( 1. );
  //QColor color = m_ManualSegmentationDockWidget->GetValidatedColor();
  if( this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->GetCurrentCollectionID()
    == -1 )
    {
    r = 0.1;
    g = 0.5;
    b = 0.7;
    }
  else
    {
    QColor color = 
      this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->ColorComboBox->GetCurrentColorData().second;
    color.getRgbF( &r, &g, &b );
    }

  bool highlighted( false );

  // get from m_DataBaseTables if user is using one gofiguredatabase or not.
  // In such a case contours are saved in the database, else they are not!
  bool saveindatabase = m_DataBaseTables->IsDatabaseUsed();

  // to make sure that m_ContourId is set to the right value
  int ContourID = -1;
  unsigned int i;

  if( m_ReEditContourMode )
    {
    ContourID = m_ContourId;

    ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
      it = m_ContourMeshContainer.get< TraceID >().find( m_ContourId );

    if( it->TraceID == m_ContourId )
      {
      r = it->rgba[0];
      g = it->rgba[1];
      b = it->rgba[2];
//       a = it->rgba[3];
      }
    }

  for( i = 0; i < m_ContourWidget.size(); i++ )
    {
    ValidateContour( ContourID, i, r, g, b, a, highlighted, m_TimePoint, saveindatabase );
    }

  if( m_ReEditContourMode )
    {
    m_ManualSegmentationDockWidget->ActivateManualSegmentation( false );

    for( i = 0; i < m_ContourWidget.size(); i++ )
      {
      m_ContourWidget[i]->Initialize( NULL );
      }

    std::list< int > listofrowstobeselected;
    listofrowstobeselected.push_back( m_ContourId );

    m_DataBaseTables->ChangeContoursToHighLightInfoFromVisu( listofrowstobeselected,
      m_ReEditContourMode );

    m_ReEditContourMode = false;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ReinitializeContour()
{
  for( unsigned int i = 0; i < m_ContourWidget.size(); i++ )
    {
	  // Enable then initialize the widget
	  m_ContourWidget[i]->SetEnabled(1);
    m_ContourWidget[i]->Initialize(NULL);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
ChangeContourRepresentationProperty()
{
  double linewidth = m_ManualSegmentationDockWidget->GetLinesWidth();
  QColor linecolor = m_ManualSegmentationDockWidget->GetLinesColor();
  QColor nodecolor = m_ManualSegmentationDockWidget->GetNodesColor();
  QColor activenodecolor = m_ManualSegmentationDockWidget->GetActiveNodesColor();

  double rl, gl, bl;
  linecolor.getRgbF( &rl, &gl, &bl );

  double rn, gn, bn;
  nodecolor.getRgbF( &rn, &gn, &bn );

  double ra, ga, ba;
  activenodecolor.getRgbF( &ra, &ga, &ba );

  for( unsigned int i = 0; i < m_ContourRepresentation.size(); i++ )
    {
    m_ContourRepresentation[i]->GetLinesProperty()->SetLineWidth( linewidth );
    m_ContourRepresentation[i]->GetLinesProperty()->SetColor( rl, gl, bl );

    m_ContourRepresentation[i]->GetProperty()->SetColor( rn, gn, bn );
    m_ContourRepresentation[i]->GetActiveProperty()->SetColor( ra, ga, ba );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param[in] pos[]
 * \return
 */
int*
QGoTabImageView3DwT::
GetImageCoordinatesFromWorldCoordinates( double iPos[3] )
{
  return m_ImageView->GetImageCoordinatesFromWorldCoordinates( iPos );
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
std::vector< vtkActor* >
QGoTabImageView3DwT::
AddContour( const int& iId,
  vtkPolyData* dataset,
  vtkProperty* iProperty )
{
  return this->m_ImageView->AddContour( iId, dataset, iProperty );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param iActivate
 */
void
QGoTabImageView3DwT::
ActivateManualSegmentationEditor( const bool& iActivate )
{
  std::vector< vtkSmartPointer< vtkContourWidget > >::iterator
    it = m_ContourWidget.begin();
  while( it != m_ContourWidget.end() )
    {
    if( iActivate )
      {
      (*it)->On();
      }
    else
      {
      (*it)->Off();
      }
    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param iId
 * \param iActor
 */
void
QGoTabImageView3DwT::
RemoveActorFromViewer( const int& iId, vtkActor* iActor )
{
  m_ImageView->RemoveActor( iId, iActor );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param iId
 * \param iActor
 */
void
QGoTabImageView3DwT::
DisplayActorInViewer( const int& iId, vtkActor* iActor )
{
  m_ImageView->AddActor( iId, iActor );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
RemoveAllContoursForPresentTimePoint( )
{
  if( m_TimePoint >= 0 )
    {
    RemoveAllContoursForGivenTimePoint( m_TimePoint );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
RemoveAllContoursForGivenTimePoint( const unsigned int& iT )
{
  if( m_ContourMeshContainer.size() > 0 )
    {
    ContourMeshStructureMultiIndexContainer::index< TCoord >::type::iterator it0, it1;
    boost::tuples::tie(it0,it1) =
      m_ContourMeshContainer.get< TCoord >().equal_range( iT );

    int c_dir;
    vtkActor* c_actor;

    while( it0 != it1 )
      {
      c_dir = (*it0).Direction;
      c_actor = (*it0).Actor;

      RemoveActorFromViewer( c_dir, c_actor );
      ++it0;
      }

    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
LoadAllContoursForCurrentTimePoint()
{
  if( m_TimePoint >= 0 )
    {
    LoadAllContoursForGivenTimePoint( static_cast< unsigned int >( m_TimePoint ) );
    }
}
//-------------------------------------------------------------------------
/**
 *
 * \param iT
 */
void
QGoTabImageView3DwT::LoadAllContoursForGivenTimePoint( const unsigned int& iT )
{
  if( m_ContourMeshContainer.size() > 0 )
    {
    ContourMeshStructureMultiIndexContainer::index< TCoord >::type::iterator it0, it1;
    boost::tuples::tie(it0,it1) =
      m_ContourMeshContainer.get< TCoord >().equal_range( iT );

    int c_dir;
    vtkActor* c_actor;

    while( it0 != it1 )
      {
      c_dir = (*it0).Direction;
      c_actor = (*it0).Actor;

      DisplayActorInViewer( c_dir, c_actor );
      ++it0;
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AddPolyData( vtkPolyData* iMesh )
{
  m_ImageView->AddMesh( iMesh );
//   this->AddContour( 0, iMesh, 0 );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AddContourFromNodes( const unsigned int& iContourID,
  vtkPolyData* iNodes,
  double iRgba[4],
  const bool& iHighlighted,
  const unsigned int& iTCoord,
  const bool& iSaveInDataBase )
{
  AddContourFromNodes( iContourID, iNodes, iRgba[0], iRgba[1], iRgba[2], iRgba[3],
    iHighlighted, iTCoord, iSaveInDataBase );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AddContourFromNodes( const unsigned int& iContourID,
  vtkPolyData* iNodes,
  const double& iR, const double& iG, const double& iB, const double& iA,
  const bool& iHighlighted, const unsigned int& iTCoord, const bool& iSaveInDataBase )
{
  if( iNodes->GetNumberOfPoints() > 2 )
    {
    int dir = ComputeDirectionFromContour( iNodes );

    if( dir != -1 )
      {
      /// \todo m_ContourWidget needs to be correctly set up,
      /// before turning it on
      m_ContourWidget[dir]->On();
      m_ContourWidget[dir]->Initialize( iNodes );
      this->ValidateContour( iContourID, dir, iR, iG, iB, iA, iHighlighted,
        iTCoord, iSaveInDataBase );
      m_ContourWidget[dir]->Off();
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
UpdateDBAndCollectionIDComboBoxForANewCreatedCollection()
{
  //first, save in the database:
  std::pair<std::string,QColor> NewCollectionToAddInComboBox =
    this->m_DataBaseTables->SaveNewCollectionInDB(
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->ColorComboBox->GetCurrentColorData(),
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->TraceName->text().toStdString());
  //second, update the ColorIDCollectionComboBox with the new created ID:
  this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->ColorIDCollectionComboBox->addColor(
    NewCollectionToAddInComboBox.second,NewCollectionToAddInComboBox.first.c_str());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
PassInfoForDBForCurrentSelectedColor()
{
  this->m_DataBaseTables->UpdateCurrentColorData(
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->ColorComboBox->GetCurrentColorData());

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
PassInfoForCurrentCollectionID()
{
  this->m_DataBaseTables->SetCurrentCollectionID(
    this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->ColorIDCollectionComboBox->GetCurrentColorData());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ReEditContour( const unsigned int& iId )
{
  if( !m_ContourMeshContainer.empty() )
    {
    ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
      it = m_ContourMeshContainer.get< TraceID >().find( iId );

    if( it != m_ContourMeshContainer.get< TraceID >().end() )
      {
      int c_dir;
      vtkActor* c_actor;
      vtkPolyData* c_nodes = (vtkPolyData*)(0);

      while( it != m_ContourMeshContainer.get< TraceID >().end() )
        {
        if( it->TraceID == iId )
          {
          c_dir = (*it).Direction;
          c_actor = (*it).Actor;
          c_nodes = (*it).Nodes;

          RemoveActorFromViewer( c_dir, c_actor );
          }
        else
          {
          break;
          }

        ++it;
        }

      m_ContourMeshContainer.erase( iId );

      int dir = ComputeDirectionFromContour( c_nodes );

      if( dir != -1 )
        {
        m_ReEditContourMode = true;
        m_ContourId = iId;

        double p[3];
        c_nodes->GetPoint( 0, p );
        int* idx = GetImageCoordinatesFromWorldCoordinates( p );

        switch( dir )
          {
          default:
          case 0:
            {
            this->SetSliceViewXY( idx[2] );
            break;
            }
          case 1:
            {
            this->SetSliceViewXZ( idx[1] );
            break;
            }
          case 2:
            {
            this->SetSliceViewXY( idx[0] );
            break;
            }
          }
        m_ContourWidget[dir]->Initialize( c_nodes );
        m_ManualSegmentationDockWidget->ActivateManualSegmentation( true );
        this->m_ManualSegmentationDockWidget->TraceManualEditingWidget->
          SetEnableTraceCollectionColorBoxes(false);
        }
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighLightContours()
{
  std::list< vtkProp3D* > listofpicked = m_ImageView->GetListOfPickedActors();

  std::list< vtkProp3D* >::iterator it = listofpicked.begin();

  vtkProperty* select_property = vtkProperty::New();
  select_property->SetColor( 1., 0., 0. );
  select_property->SetLineWidth( 3. );

  while( it != listofpicked.end() )
    {
    // Change the corresponding highlighted value in the container
    ContourMeshStructureMultiIndexContainer::nth_index< 1 >::type::iterator
      actor_it = m_ContourMeshContainer.get< 1 >().find( static_cast< vtkActor* >( *it ) );

    if( actor_it != m_ContourMeshContainer.get< 1 >().end() )
      {
      unsigned int trace_id = actor_it->TraceID;

      ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
        traceid_it = m_ContourMeshContainer.get< TraceID >().find( trace_id );

      if( traceid_it != m_ContourMeshContainer.get< TraceID >().end() )
        {
        while( ( traceid_it != m_ContourMeshContainer.get< TraceID >().end() )
            && ( (*traceid_it).TraceID == trace_id ) )
          {
          m_ImageView->ChangeActorProperty( traceid_it->Direction,
            traceid_it->Actor, select_property );

//           traceid_it->Highlighted = true;

          ++traceid_it;
          }
        }
      }

    ++it;
    }

  select_property->Delete();

  std::list< vtkProp3D* > listofunpicked = m_ImageView->GetListOfUnPickedActors();
  it = listofunpicked.begin();

  while( it != listofunpicked.end() )
    {
    // find corresponding color from the container
//     ChangeActorProperty( *it, m_ActorsPropertyMap[*it] );
    // Change the corresponding highlighted value in the container
        // Change the corresponding highlighted value in the container
    ContourMeshStructureMultiIndexContainer::nth_index< 1 >::type::iterator
      actor_it = m_ContourMeshContainer.get< 1 >().find( static_cast< vtkActor* >( *it ) );

    if( actor_it != m_ContourMeshContainer.get< 1 >().end() )
      {
      unsigned int trace_id = actor_it->TraceID;

      ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
        traceid_it = m_ContourMeshContainer.get< TraceID >().find( trace_id );

      if( traceid_it != m_ContourMeshContainer.get< TraceID >().end() )
        {
        while( ( traceid_it != m_ContourMeshContainer.get< TraceID >().end() )
            && ( (*traceid_it).TraceID == trace_id ) )
          {
          vtkProperty* temp_property = vtkProperty::New();
          temp_property->SetColor( traceid_it->rgba[0], traceid_it->rgba[1], traceid_it->rgba[2] );
          temp_property->SetLineWidth( 1. );

          m_ImageView->ChangeActorProperty( traceid_it->Direction,
            traceid_it->Actor, temp_property );

//           traceid_it->Highlighted = false;

          temp_property->Delete();
          ++traceid_it;
          }
        }
      }

    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighLightContoursFromTable( )
{
 // std::vector< ContourMeshStructure >::iterator
 //   it = this->m_DataBaseTables->m_ContoursInfo.begin();
  std::vector< ContourMeshStructure >::iterator
    it = this->m_DataBaseTables->GetTracesInfoListForVisu("contour")->begin();
  unsigned int trace_id = 0;

  vtkProperty* select_property = vtkProperty::New();
  select_property->SetColor( 1., 0., 0. );
  select_property->SetLineWidth( 3. );

  while( it != this->m_DataBaseTables->GetTracesInfoListForVisu("contour")->end() )
    {
    trace_id = it->TraceID;
//     std::cout <<it->TCoord <<std::endl;

    ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
        traceid_it = m_ContourMeshContainer.get< TraceID >().find( trace_id );

    if( traceid_it != m_ContourMeshContainer.get< TraceID >().end() )
      {
      while( ( traceid_it != m_ContourMeshContainer.get< TraceID >().end() )
        && ( (*traceid_it).TraceID == trace_id ) )
        {
        if( !it->Highlighted )
          {
          vtkProperty* temp_property = vtkProperty::New();
          temp_property->SetColor( traceid_it->rgba[0], traceid_it->rgba[1], traceid_it->rgba[2] );
          temp_property->SetLineWidth( 1. );

          m_ImageView->ChangeActorProperty( traceid_it->Direction,
            traceid_it->Actor, temp_property );

          temp_property->Delete();
          }
        else
          {
          m_ImageView->ChangeActorProperty( traceid_it->Direction,
            traceid_it->Actor, select_property );
          }
        ++traceid_it;
        }
      }

    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SelectContoursInTable( )
{
  if( this->m_DataBaseTables->IsDatabaseUsed() )
    {
    std::list< vtkProp3D* > listofpicked = m_ImageView->GetListOfPickedActors();

    if( !listofpicked.empty() )
      {
      std::list< int > listofrowstobeselected;

      std::list< vtkProp3D* >::iterator it = listofpicked.begin();

      while( it != listofpicked.end() )
        {
        // Change the corresponding highlighted value in the container
        ContourMeshStructureMultiIndexContainer::nth_index< 1 >::type::iterator
          actor_it = m_ContourMeshContainer.get< 1 >().find( static_cast< vtkActor* >( *it ) );

        if( actor_it != m_ContourMeshContainer.get< 1 >().end() )
          {
          int trace_id = static_cast< int >( actor_it->TraceID );
          listofrowstobeselected.push_back( trace_id );
          }
        ++it;
        }

      this->m_DataBaseTables->ChangeContoursToHighLightInfoFromVisu(
        listofrowstobeselected, false );
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
DisplayAnnotations( )
{
  this->m_ImageView->ShowAnnotations();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
DisplaySplinePlanes( )
{
  this->m_ImageView->ShowSplinePlane();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
DisplayCube( )
{
  this->m_ImageView->ShowCube3D();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
DeleteContoursFromTable( const std::list< int >& iList )
{
  std::list< int >::const_iterator traceid_it = iList.begin();
  ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator it;

  while( traceid_it != iList.end() )
    {
    it = m_ContourMeshContainer.get< TraceID >().find( *traceid_it );

    if( it != m_ContourMeshContainer.get< TraceID >().end() )
      {
      int c_dir;
      vtkActor* c_actor;
      vtkPolyData* c_nodes;

      while( it != m_ContourMeshContainer.get< TraceID >().end() )
        {
        if( static_cast< int >( it->TraceID ) == *traceid_it )
          {
          c_dir = (*it).Direction;
          c_actor = (*it).Actor;
          c_nodes = (*it).Nodes;

          RemoveActorFromViewer( c_dir, c_actor );
          }
        else
          {
          break;
          }

        ++it;
        }

      m_ContourMeshContainer.erase( *traceid_it );
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
  return m_TimePoint;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::AddBookmark()
{
  this->m_DataBaseTables->AddBookmark(this->GetSliceViewYZ(),
    this->GetSliceViewXZ(),this->GetSliceViewXY(),this->GetTimePoint());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
Change3DPerspectiveToAxial()
{
  m_ImageView->SetCamera( 1 );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
Change3DPerspectiveToCoronal()
{
  m_ImageView->SetCamera( 2 );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
Change3DPerspectiveToSagittal()
{
  m_ImageView->SetCamera( 3 );
}
