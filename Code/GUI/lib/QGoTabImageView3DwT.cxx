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
#include "QGoVisualizationDockWidget.h"
#include "QGoManualSegmentationDockWidget.h"
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
#include "vtkPolyDataMySQLTextReader.h"
#include "vtkImageActorPointPlacer.h"
#include "SelectQueryDatabaseHelper.h"
#include "ConvertToStringHelper.h"

#include "vtkDistanceWidget.h"
#include "vtkAngleWidget.h"
// #include "vtkDistanceRepresentation2D.h"

#include "vtkSphereSource.h"

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
  m_MeshId( 1 ),
  m_ReEditContourMode( false ),
  m_ContourWidget( 3 ),
  m_ContourRepresentation( 3 ),
  m_DistanceWidget( 3 ),
//   m_DistanceRepresentation( 3 )
  m_AngleWidget( 3 )
{
  m_Image = vtkSmartPointer< vtkImageData >::New();

  m_LinesWidth = 1.;

  m_SeedsWorldPosition = NULL;

  setupUi( this );

  m_MegaCaptureReader = itk::MegaCaptureReader::New();

  CreateVisuDockWidget();

  CreateSettingAndDialogSegmentationWidgets();

  CreateManualSegmentationdockWidget();

  CreateOneClickSegmentationDockWidget();

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
    // Contour widget
    this->m_ContourRepresentation[i] =
      vtkSmartPointer< vtkOrientedGlyphContourRepresentation >::New();
    this->m_ContourRepresentation[i]->GetProperty()->SetColor( 0., 1., 1. );
    this->m_ContourRepresentation[i]->GetLinesProperty()->SetColor( 1., 0., 1. );
    this->m_ContourRepresentation[i]->GetActiveProperty()->SetColor( 1., 1., 0. );

    this->m_ContourWidget[i] = vtkSmartPointer< vtkContourWidget >::New();
    this->m_ContourWidget[i]->SetPriority( 10.0 );
    this->m_ContourWidget[i]->SetInteractor( m_ImageView->GetInteractor( i ) );
    this->m_ContourWidget[i]->Off();

    this->m_ContourWidget[i]->SetRepresentation( this->m_ContourRepresentation[i] );

    // distance widget
//     this->m_DistanceRepresentation[i] = vtkSmartPointer< vtkDistanceRepresentation2D >::New();

    this->m_DistanceWidget[i] = vtkSmartPointer< vtkDistanceWidget >::New();
    this->m_DistanceWidget[i]->SetInteractor( m_ImageView->GetInteractor( i ) );
    this->m_DistanceWidget[i]->CreateDefaultRepresentation();

    this->m_DistanceWidget[i]->Off();

    // angle widget
    this->m_AngleWidget[i] = vtkSmartPointer< vtkAngleWidget >::New();
    this->m_AngleWidget[i]->SetInteractor( m_ImageView->GetInteractor( i ) );
    this->m_AngleWidget[i]->CreateDefaultRepresentation();

    this->m_AngleWidget[i]->Off();
    }

  // Generate default color, width and nodes for the contours visualization
  GenerateContourRepresentationProperties();

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus*, QDockWidget* >(
      new QGoDockWidgetStatus( m_NavigationDockWidget, Qt::RightDockWidgetArea, true, true ),
      m_NavigationDockWidget ) );

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus*, QDockWidget* >(
      new QGoDockWidgetStatus( m_ManualSegmentationDockWidget, Qt::LeftDockWidgetArea, true, true ),
      m_ManualSegmentationDockWidget ) );

//   m_DockWidgetList.push_back(
//     std::pair< QGoDockWidgetStatus*, QDockWidget* >(
//       new QGoDockWidgetStatus( m_DataBaseTables, Qt::TopDockWidgetArea, true, true ),
//       m_DataBaseTables ) );

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus*, QDockWidget* >(
      new QGoDockWidgetStatus( m_OneClickSegmentationDockWidget, Qt::LeftDockWidgetArea, true, true ),
      m_OneClickSegmentationDockWidget ) );

  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus*, QDockWidget* >(
      new QGoDockWidgetStatus( m_TraceManualEditingDockWidget, Qt::LeftDockWidgetArea, true, true ),
      m_TraceManualEditingDockWidget ) );


#if defined ( ENABLEFFMPEG ) || defined (ENABLEAVI)
  m_DockWidgetList.push_back(
    std::pair< QGoDockWidgetStatus*, QDockWidget* >(
      new QGoDockWidgetStatus( m_VideoRecorderWidget, Qt::LeftDockWidgetArea, true, true ),
      m_VideoRecorderWidget ) );
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

  unsigned int minch = m_MegaCaptureReader->GetMinChannel();
  unsigned int maxch = m_MegaCaptureReader->GetMaxChannel();

  for( unsigned int i = minch; i < maxch; i++ )
    {
    vtkImageData* temp = m_MegaCaptureReader->GetOutput( i );
    if( temp )
      {
      temp->Delete();
      temp = NULL;
      }
    }

  // If no seeds have been created, don't try do delete it!
  if( m_SeedsWorldPosition )
    {
    m_SeedsWorldPosition->Delete();
    }
//   DeleteContourMeshStructureElement( m_ContourContainer );
//   DeleteContourMeshStructureElement( m_MeshContainer );

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateSettingAndDialogSegmentationWidgets()
{
  this->m_TraceManualEditingDockWidget = 
    new QGoTraceManualEditingDockWidget(this );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
GenerateContourRepresentationProperties()
{
  bool haschanged = false;

  double temp = m_ManualSegmentationDockWidget->m_SettingsDialog->GetLineWidth();

  if( m_LinesWidth != temp )
    {
    m_LinesWidth = temp;
    haschanged = true;
    }

  QColor temp_color = m_ManualSegmentationDockWidget->m_SettingsDialog->GetLineColor();

  if( m_LinesColor != temp_color )
    {
    m_LinesColor = temp_color;
    haschanged = true;
    }

  temp_color = m_ManualSegmentationDockWidget->m_SettingsDialog->GetNodeColor();

  if( m_NodesColor != temp_color )
    {
    m_NodesColor = temp_color;
    haschanged = true;
    }
  temp_color = m_ManualSegmentationDockWidget->m_SettingsDialog->GetActivatedNodeColor();

  if( m_ActiveNodesColor != temp_color )
    {
    m_ActiveNodesColor = temp_color;
    haschanged = true;
    }

  if( haschanged )
    {
    emit ContourRepresentationPropertiesChanged();
    }
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
  m_ManualSegmentationDockWidget->setEnabled( false );

  // Manual segmentation

  QObject::connect( m_ManualSegmentationDockWidget, SIGNAL( ValidatePressed() ),
    this, SLOT( ValidateContour() ) );

  QObject::connect( m_ManualSegmentationDockWidget, SIGNAL( ReinitializePressed() ),
    this, SLOT( ReinitializeContour() ) );

  QObject::connect( m_ManualSegmentationDockWidget, SIGNAL(visibilityChanged(bool) ),
    this, SLOT (ShowTraceDockWidgetForContour(bool)));

  QObject::connect( m_ManualSegmentationDockWidget, SIGNAL( UpdateContourRepresentationProperties() ),
    this, SLOT( GenerateContourRepresentationProperties( ) ) );

  QObject::connect( this, SIGNAL( ContourRepresentationPropertiesChanged() ),
    this, SLOT( ChangeContourRepresentationProperty() ) );

  // Connect semi auto segmentation

  QObject::connect( m_ManualSegmentationDockWidget, SIGNAL( ApplyFilterPressed( ) ),
      this, SLOT( ApplyContourSemiAutoSegmentation( ) ) );

  // Cursor interaction

  QObject::connect( m_ManualSegmentationDockWidget, SIGNAL( UpdateInteractorBehavior( bool ) ),
   this, SLOT( ContourInteractorBehavior( bool ) ) );

  // Create action

  QAction* tempaction = m_ManualSegmentationDockWidget->toggleViewAction();

  this->m_SegmentationActions.push_back( tempaction );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ContourInteractorBehavior( bool iVisible)
{
  // Mode 0 = manual
  // Mode 1 = semi auto
  // Mode 2 = auto
  int mode = m_ManualSegmentationDockWidget->GetMode();
  switch ( mode )
    {
    case 0 :
      // Manual
      ActivateManualSegmentationEditor( iVisible );
      break;

    case 1 :
      // Semi auto
      ActivateSemiAutoSegmentationEditor( iVisible );
      break;

    case 2 :
      // Auto
      break;

    default :
      break;
      }
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
  // Initializae cursor behaviour
  this->m_ImageView->DefaultMode();

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
 * \param iActivate
 */
void
QGoTabImageView3DwT::
ActivateSemiAutoSegmentationEditor( const bool& iActivate )
{
  ActivateManualSegmentationEditor( false );
  // Initializae cursor behaviour
  this->m_ImageView->DefaultMode();

  if( iActivate )
    {
    this->m_ImageView->OneClickMode();
    }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
MeshInteractorBehavior( bool iVisible)
{
  // if the widget is visible
  // check in which mode we are
  if( iVisible )
    {
    this->m_ImageView->OneClickMode();
    }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
DefaultInteractorBehavior( bool iVisible)
{
  // if the widget is visible
  // check in which mode we are
  if( iVisible )
    {
    this->m_ImageView->DefaultMode();
    }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ZoomInteractorBehavior( bool iVisible)
{
  // if the widget is visible
  // check in which mode we are
  if( iVisible )
    {
    this->m_ImageView->ZoomMode();
    }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
PanInteractorBehavior( bool iVisible)
{
  // if the widget is visible
  // check in which mode we are
  if( iVisible )
    {
    this->m_ImageView->PanMode();
    }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ContourPickingInteractorBehavior( bool iVisible)
{
  // if the widget is visible
  // check in which mode we are
  if( iVisible )
    {
    this->m_ImageView->ContourPickingMode();
    }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
MeshPickingInteractorBehavior( bool iVisible)
{
  // if the widget is visible
  // check in which mode we are
  if( iVisible )
    {
    std::cout << "calls meshpicking mode..." << std::endl;
    this->m_ImageView->MeshPickingMode();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
DistanceWidgetInteractorBehavior( bool iActive )
{
  if( iActive )
    {
    for( int i = 0; i < 3; i++ )
      {
      this->m_DistanceWidget[i]->On();
      }
    }
  else
    {
    for( int i = 0; i < 3; i++ )
      {
      this->m_DistanceWidget[i]->Off();
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AngleWidgetInteractorBehavior( bool iActive )
{
  if( iActive )
    {
    for( int i = 0; i < 3; i++ )
      {
      this->m_AngleWidget[i]->On();
      }
    }
  else
    {
    for( int i = 0; i < 3; i++ )
      {
      this->m_AngleWidget[i]->Off();
      }
    }
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
Box3DPicking( bool iActive )
{
  this->m_ImageView->SetBox3DPicking( iActive );
}


//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
/**
 * \brief
 */
void
QGoTabImageView3DwT::
CreateOneClickSegmentationDockWidget()
{
  m_OneClickSegmentationDockWidget = new QGoOneClickSegmentationDockWidget( this );
  m_OneClickSegmentationDockWidget->setEnabled( false );

  QAction* tempaction = m_OneClickSegmentationDockWidget->toggleViewAction();

  this->m_SegmentationActions.push_back( tempaction );

  QObject::connect( m_OneClickSegmentationDockWidget, SIGNAL( ApplyFilterPressed( ) ),
      this, SLOT( ApplyOneClickSegmentationFilter( ) ) );

   QObject::connect( m_OneClickSegmentationDockWidget, SIGNAL( visibilityChanged(bool) ),
      this, SLOT( ShowTraceDockWidgetForMesh(bool))); 
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

  m_NavigationDockWidget = new QGoVisualizationDockWidget( this, 4 );

  QObject::connect( m_NavigationDockWidget, SIGNAL( XSliceChanged( int ) ),
    this, SLOT( SetSliceViewYZ( int ) ) );

  QObject::connect( this, SIGNAL( SliceViewYZChanged( int ) ),
    m_NavigationDockWidget, SLOT( SetXSlice( int ) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( YSliceChanged( int ) ),
    this, SLOT( SetSliceViewXZ( int ) ) );

  QObject::connect( this, SIGNAL( SliceViewXZChanged( int ) ),
    m_NavigationDockWidget, SLOT( SetYSlice( int ) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( ZSliceChanged( int ) ),
    this, SLOT( SetSliceViewXY( int ) ) );

  QObject::connect( this, SIGNAL( SliceViewXYChanged( int ) ),
    m_NavigationDockWidget, SLOT( SetZSlice( int ) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( TSliceChanged( int ) ),
    this, SLOT( SetTimePoint( int ) ) );

  QObject::connect( this, SIGNAL( TimePointChanged( int ) ),
    m_NavigationDockWidget, SLOT( SetTSlice( int ) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( ShowAllChannelsChanged( bool ) ),
    this, SLOT( ShowAllChannels( bool ) ) );

  QObject::connect( m_NavigationDockWidget, SIGNAL( ShowOneChannelChanged( int ) ),
    this, SLOT( ShowOneChannel( int ) ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void
QGoTabImageView3DwT::
CreateDataBaseTablesConnection()
{
  QObject::connect( this->m_DataBaseTables,
    SIGNAL( PrintExistingColorsFromDB(std::list<std::pair<std::string,std::vector<int> > >) ),
    this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorComboBox,
    SLOT( setExistingColors(std::list<std::pair<std::string,std::vector<int> > >) ) );

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( PrintExistingCollectionIDsFromDB(std::list<std::pair<std::string,QColor> >) ),
    this->m_TraceManualEditingDockWidget->m_TraceWidget,
    SLOT( SetCollectionID(std::list<std::pair<std::string,QColor> >) ) );

  QObject::connect(
    this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorComboBox,
    SIGNAL( NewColorToBeSaved(std::vector<std::string>)),
    this->m_DataBaseTables,
    SLOT( SaveNewColorInDB(std::vector<std::string> ) ) );

  QObject::connect(
    this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorIDCollectionComboBox,
    SIGNAL( NewCollectionToBeSaved()),
    this, SLOT( UpdateDBAndCollectionIDComboBoxForANewCreatedCollection() ) );

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( NeedToGetCurrentSelectedColor() ),
    this, SLOT(PassInfoForDBForCurrentSelectedColor()) );

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( NewCreatedCollection(QColor, QString) ),
    this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorIDCollectionComboBox
    , SLOT( addColor(QColor, QString) ));

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( SelectionContoursToHighLightChanged() ),
    this, SLOT( HighLightTracesFromTableManager() ) );

  QObject::connect( this->m_DataBaseTables,
      SIGNAL( SelectionMeshesToHighLightChanged() ),
      this, SLOT( HighLightTracesFromTableManager() ) );

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( NeedCurrentSelectedCollectionID() ),
    this, SLOT( PassInfoForCurrentCollectionID() ) );

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( DeletedCollection(unsigned int) ),
    this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorIDCollectionComboBox,
    SLOT( DeleteCollectionID(unsigned int) ) );

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( TraceToReEdit( unsigned int ) ),
    this, SLOT( ReEditContour( unsigned int ) ) );

  QObject::connect( this->m_DataBaseTables,
    SIGNAL( TracesToDeleteInVisu( std::list< int > ) ),
    this, SLOT( DeleteTracesFromTableManager( std::list< int > ) ) );

  QObject::connect(this->m_DataBaseTables,
    SIGNAL( ListCellTypesToUpdate(QStringList)),
    this->m_TraceManualEditingDockWidget->m_TraceWidget,
    SLOT(SetListCellTypes(QStringList)));

  QObject::connect(
    this->m_TraceManualEditingDockWidget->m_TraceWidget,
    SIGNAL( AddANewCellType()),this->m_DataBaseTables,
    SLOT(AddNewCellType()));

  QObject::connect(
    this->m_TraceManualEditingDockWidget->m_TraceWidget,
    SIGNAL( DeleteCellType()),this->m_DataBaseTables,
    SLOT(DeleteCellType()));

  QObject::connect(this->m_DataBaseTables,
    SIGNAL( ListSubCellTypesToUpdate(QStringList)),
    this->m_TraceManualEditingDockWidget->m_TraceWidget,
    SLOT(SetListSubCellTypes(QStringList)));

   QObject::connect(
    this->m_TraceManualEditingDockWidget->m_TraceWidget,
    SIGNAL( AddANewSubCellType()),this->m_DataBaseTables,
    SLOT(AddNewSubCellType()));

  QObject::connect(
    this->m_TraceManualEditingDockWidget->m_TraceWidget,
    SIGNAL( DeleteSubCellType()),this->m_DataBaseTables,
    SLOT(DeleteSubCellType()));

  QObject::connect(
    this->m_DataBaseTables,SIGNAL(TheColorNameAlreadyExits()),
    this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorComboBox,
    SLOT(DontAddTheColor()));
  
  QObject::connect(
    this->m_TraceManualEditingDockWidget->m_TraceWidget,SIGNAL(ListCellTypesReady()),
    this, SLOT(SetTheCurrentCellType()));

  QObject::connect(
    this->m_TraceManualEditingDockWidget->m_TraceWidget,SIGNAL(ListSubCellTypesReady()),
    this, SLOT(SetTheCurrentSubCellType()));

  QObject::connect(
    this->m_DataBaseTables, SIGNAL(TableWidgetTableChanged(std::string,std::string)),
    this, SLOT(GoToDefaultMenu(std::string, std::string)));

  QObject::connect(
    this->m_DataBaseTables, SIGNAL(ColorChangedForSelectedTraces(std::pair<std::list<int>,QColor>)),
    this, SLOT(ChangeColorOfSelectedTracesManager(std::pair<std::list<int>,QColor>)));
}
//-------------------------------------------------------------------------
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
//-------------------------------------------------------------------------

void
QGoTabImageView3DwT::
CreateVideoRecorderWidget()
{
  m_VideoRecorderWidget = new QGoVideoRecorder( this );

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

  QObject::connect( m_VideoRecorderWidget, SIGNAL( GetSliceView() ),
          this, SLOT( SetSliceView() ) );
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

  this->m_ViewActions.push_back( m_NavigationDockWidget->toggleViewAction() );

  QAction* separator3 = new QAction( this );
  separator3->setSeparator( true );
  this->m_ViewActions.push_back( separator3 );

  this->m_ViewActions.push_back( m_DataBaseTables->toggleViewAction() );

  QAction* LoadContoursPerTimePointAction =
    new QAction( tr( "Load All Contours For Current Time Point" ), this );
  this->m_ViewActions.push_back( LoadContoursPerTimePointAction );
  LoadContoursPerTimePointAction->setVisible(false);

  QObject::connect( LoadContoursPerTimePointAction, SIGNAL( triggered() ),
    this, SLOT( LoadAllTracesForCurrentTimePointManager() ) );

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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
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

  //---------------------------------//
  //            Manual mode          //
  //---------------------------------//

  //////// Manual is becoming contour segmentation

  QAction* ManualEditingAction = new QAction( tr("Manual-Editing"), this );
  ManualEditingAction->setCheckable( true );
  QIcon ManualEditingIcon;
  ManualEditingIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/manual-editing.png")),
    QIcon::Normal, QIcon::Off );
  ManualEditingAction->setIcon(ManualEditingIcon);

  group->addAction( ManualEditingAction );

  this->m_ModeActions.push_back( ManualEditingAction );

  // When click on button, it updates widget in visu
  QObject::connect( ManualEditingAction, SIGNAL( toggled( bool ) ),
      this->m_ManualSegmentationDockWidget, SLOT( setEnabled( bool ) ) );
  QObject::connect( ManualEditingAction, SIGNAL( toggled( bool ) ),
      this->m_ManualSegmentationDockWidget, SLOT( setVisible( bool ) ) );

  // it also updates the interactor behaviour
  QObject::connect( ManualEditingAction, SIGNAL( toggled( bool ) ),
       this, SLOT( ContourInteractorBehavior( bool ) ) );

  //---------------------------------//
  //          one click mode         //
  //---------------------------------//

  ////////  is becoming mesh segmentation

  // Create/initialize the manual editing action
  QAction* OneClickAction = new QAction( tr("One Click"), this );
  OneClickAction->setCheckable( true );
  OneClickAction->setChecked(false);

  // Definition of the associated icon
  QIcon OneClickIcon;
  OneClickIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/seedIcon.png")),
    QIcon::Normal, QIcon::Off );
  OneClickAction->setIcon(OneClickIcon);

  // When click on button, it updates widget in visu
  QObject::connect( OneClickAction, SIGNAL( toggled( bool ) ),
      m_OneClickSegmentationDockWidget, SLOT( setEnabled( bool ) ) );
  QObject::connect( OneClickAction, SIGNAL( toggled( bool ) ),
      m_OneClickSegmentationDockWidget, SLOT( setVisible( bool ) ) );

  // it also updates the interactor behaviour
  QObject::connect( OneClickAction, SIGNAL( toggled( bool ) ),
    this, SLOT( MeshInteractorBehavior( bool ) ) );

  // Add the action to m_ModeActions and to group
  this->m_ModeActions.push_back( OneClickAction );
  group->addAction( OneClickAction );


  //---------------------------------//
  //           default mode          //
  //---------------------------------//

  // Create/initialize the default action
  QAction* DefaultAction = new QAction( tr( "Default" ), this );
  DefaultAction->setCheckable( true );
  DefaultAction->setChecked(true);

  QIcon DefaultIcon;
  DefaultIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/mouse-cursor.png")),
    QIcon::Normal, QIcon::Off );
  DefaultAction->setIcon( DefaultIcon );

  group->addAction( DefaultAction );

  // it also updates the interactor behaviour
  QObject::connect( DefaultAction, SIGNAL( toggled( bool ) ),
    this, SLOT( DefaultInteractorBehavior( bool ) ) );

  this->m_ModeActions.push_back( DefaultAction );

  //---------------------------------//
  //            Zoom mode            //
  //---------------------------------//

  QAction* ZoomAction = new QAction( tr( "Zoom" ), this );
  ZoomAction->setCheckable( true );
  ZoomAction->setChecked(false);

  QIcon ZoomIcon;
  ZoomIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/zoom.png")),
    QIcon::Normal, QIcon::Off );
  ZoomAction->setIcon( ZoomIcon );

  group->addAction( ZoomAction );

  this->m_ModeActions.push_back( ZoomAction );
  // it also updates the interactor behaviour
  QObject::connect( ZoomAction, SIGNAL( toggled( bool ) ),
    this, SLOT( ZoomInteractorBehavior( bool ) ) );

  //---------------------------------//
  //            Pan  mode            //
  //---------------------------------//

  QAction* PanAction = new QAction( tr( "Pan" ), this );
  PanAction->setCheckable( true );
  PanAction->setChecked(false);

  QIcon PanIcon;
  PanIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/Hand.png")),
    QIcon::Normal, QIcon::Off );
  PanAction->setIcon( PanIcon );

  group->addAction( PanAction );

  this->m_ModeActions.push_back( PanAction );
  // it also updates the interactor behaviour
  QObject::connect( PanAction, SIGNAL( toggled( bool ) ),
    this, SLOT( PanInteractorBehavior( bool ) ) );

  //---------------------------------//
  //     Contour picking  mode       //
  //---------------------------------//

  QAction* ContourPickingAction = new QAction( tr( "Contour Picking" ), this );
  ContourPickingAction->setCheckable( true );
  ContourPickingAction->setChecked(false);
  
  QIcon ContourPickingIcon;
  ContourPickingIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/ContourPicking.png")),
    QIcon::Normal, QIcon::Off );
  ContourPickingAction->setIcon( ContourPickingIcon );

  group->addAction( ContourPickingAction );

  this->m_ModeActions.push_back( ContourPickingAction );
  // it also updates the interactor behaviour
  QObject::connect( ContourPickingAction, SIGNAL( toggled( bool ) ),
    this, SLOT( ContourPickingInteractorBehavior( bool ) ) );

  //---------------------------------//
  //       Mesh picking  mode        //
  //---------------------------------//

  QAction* MeshPickingAction = new QAction( tr( "Mesh Picking" ), this );
  MeshPickingAction->setCheckable( true );
  MeshPickingAction->setChecked(false);

  QIcon MeshPickingIcon;
  MeshPickingIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/MeshPicking.png")),
    QIcon::Normal, QIcon::Off );
  MeshPickingAction->setIcon( MeshPickingIcon );

  group->addAction( MeshPickingAction );

  this->m_ModeActions.push_back( MeshPickingAction );
  // it also updates the interactor behaviour
  QObject::connect( MeshPickingAction, SIGNAL( toggled( bool ) ),
    this, SLOT( MeshPickingInteractorBehavior( bool ) ) );

  // Distance measurement mode
  QAction* DistanceAction = new QAction( tr("Measure Distance"), this );

  DistanceAction->setCheckable( true );
  DistanceAction->setChecked( false );
  
  QIcon DistanceIcon;
  DistanceIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/Distance.png")),
    QIcon::Normal, QIcon::Off );
  DistanceAction->setIcon( DistanceIcon );

  group->addAction( DistanceAction );

  this->m_ModeActions.push_back( DistanceAction );

  QObject::connect( DistanceAction, SIGNAL( toggled( bool ) ),
    this, SLOT( DistanceWidgetInteractorBehavior( bool ) ) );

  // angle measurement mode
  QAction* AngleAction = new QAction( tr("Measure Angle"), this );
  AngleAction->setCheckable( true );
  AngleAction->setChecked( false );

  QIcon AngleIcon;
  AngleIcon.addPixmap( QPixmap(QString::fromUtf8(":/fig/Angle.png")),
    QIcon::Normal, QIcon::Off );
  AngleAction->setIcon( AngleIcon );

  group->addAction( AngleAction );

  this->m_ModeActions.push_back( AngleAction );

  QObject::connect( AngleAction, SIGNAL( toggled( bool ) ),
    this, SLOT( AngleWidgetInteractorBehavior( bool ) ) );

  //---------------------------------//
  //       Box 3D picking  mode        //
  //---------------------------------//

  QAction* Box3DPickingAction = new QAction( tr( "Box 3D Picking" ), this );
  Box3DPickingAction->setCheckable( true );
  Box3DPickingAction->setChecked(false);

  group->addAction( Box3DPickingAction );

  this->m_ModeActions.push_back( Box3DPickingAction );
  // it also updates the interactor behaviour
  QObject::connect( Box3DPickingAction, SIGNAL( toggled( bool ) ),
    this, SLOT( Box3DPicking( bool ) ) );
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
    this, SLOT(GetTheRelatedToDBActions()));
  QObject::connect(this->m_DataBaseTables, SIGNAL(OpenBookmarksToUpdate()),
    this, SLOT(GetTheOpenBookmarksActions()));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::GetTheRelatedToDBActions()
{
  this->GetTheOpenBookmarksActions();
  QMenu* ImportMenu = new QMenu(tr("Import"), this);
  QAction* ImportContoursAction = new QAction( tr("Contours"), this );
  QAction* ImportMeshesAction = new QAction( tr("3DMeshes"), this );
  ImportMenu->addAction(ImportContoursAction);
  ImportMenu->addAction(ImportMeshesAction);
  QMenu* ExportMenu = new QMenu(tr("Export"), this);
  QAction* ExportContoursAction = new QAction( tr("Contours"), this );
  QAction* ExportMeshesAction = new QAction( tr("3DMeshes"), this );
  ExportMenu->addAction(ExportContoursAction);
  ExportMenu->addAction(ExportMeshesAction);
  this->m_ToolsActions.push_back( ImportMenu->menuAction() );
  this->m_ToolsActions.push_back( ExportMenu->menuAction() );
  QObject::connect( ExportContoursAction, SIGNAL( triggered() ),
    this->m_DataBaseTables, SLOT(ExportContours () ) );
  QObject::connect( ImportContoursAction, SIGNAL( triggered() ),
    this, SLOT(ImportContours() ) );
  QObject::connect( ExportMeshesAction, SIGNAL( triggered() ),
    this->m_DataBaseTables, SLOT(ExportMeshes() ) );
  QObject::connect( ImportMeshesAction, SIGNAL (triggered() ),
    this, SLOT (ImportMeshes() ) );
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
    break;
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

  m_VSplitter  = new QSplitter( Qt::Vertical, iParent );
  m_DataBaseTables = new QGoPrintDatabase;
  m_VSplitter->addWidget( m_DataBaseTables );
  m_DataBaseTables->hide();
  
  m_ImageView = new QGoImageView3D;
  m_ImageView->SetBackgroundColor( m_BackgroundColor );
  m_VSplitter->addWidget( m_ImageView );

  QObject::connect( m_ImageView, SIGNAL( SliceViewXYChanged( int ) ),
    this, SIGNAL( SliceViewXYChanged( int ) ) );

  QObject::connect( m_ImageView, SIGNAL( SliceViewXZChanged( int ) ),
    this, SIGNAL( SliceViewXZChanged( int ) ) );

  QObject::connect( m_ImageView, SIGNAL( SliceViewYZChanged( int ) ),
    this, SIGNAL( SliceViewYZChanged( int ) ) );

  QObject::connect( m_ImageView, SIGNAL( FullScreenViewChanged( int ) ),
    this, SIGNAL( FullScreenViewChanged( int ) ) );

  // connect the contours selection connection
  QObject::connect( m_ImageView, SIGNAL( ContoursSelectionChanged( ) ),
      this, SLOT( HighLightContours( ) ) );

  QObject::connect( m_ImageView, SIGNAL( ContoursSelectionChanged( ) ),
    this, SLOT( SelectContoursInTable() ) );

  // connect the meshes selection connection
  QObject::connect( m_ImageView, SIGNAL( MeshesSelectionChanged( ) ),
      this, SLOT( ListHighLightMeshes( ) ) );

  // connect the meshes selection connection
  QObject::connect( m_ImageView, SIGNAL( MeshesSelectionChanged( ) ),
      this, SLOT( ListSelectMeshesInTable( ) ) );

  m_HBoxLayout = new QHBoxLayout( iParent );
  m_HBoxLayout->addWidget( m_VSplitter );

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

    // Initialize the widgets with the good number of channels
    // it will update the size of the related combobox
    m_NavigationDockWidget->SetNumberOfChannels( NumberOfChannels );
    m_OneClickSegmentationDockWidget->SetNumberOfChannels( NumberOfChannels );
    m_ManualSegmentationDockWidget->SetNumberOfChannels( NumberOfChannels );

    if( NumberOfChannels > 1 )
      {
      m_NavigationDockWidget->SetChannel( 0 );
      m_InternalImages.resize( NumberOfChannels );

      for( int i = 1; i < NumberOfChannels; i++ )
        {
        m_NavigationDockWidget->SetChannel( i );

        m_LSMReader.push_back( vtkSmartPointer< vtkLSMReader >::New() );
        m_LSMReader.back()->SetFileName( m_LSMReader[0]->GetFileName() );
        m_LSMReader.back()->SetUpdateChannel( i );
        }
      }

    m_NavigationDockWidget->SetXMinimumAndMaximum( 0, dim[0] - 1 );
    m_NavigationDockWidget->SetXSlice( ( dim[0] - 1 ) / 2 );

    m_NavigationDockWidget->SetYMinimumAndMaximum( 0, dim[1] - 1 );
    m_NavigationDockWidget->SetYSlice( ( dim[1] - 1 ) / 2 );

    m_NavigationDockWidget->SetZMinimumAndMaximum( 0, dim[2] - 1 );
    m_NavigationDockWidget->SetZSlice( ( dim[2] - 1 ) / 2 );

    m_NavigationDockWidget->SetTMinimumAndMaximum( 0, dim[3] - 1 );
    m_NavigationDockWidget->SetTSlice( iTimePoint );
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
  m_MegaCaptureReader->Update();

  unsigned int min_t = m_MegaCaptureReader->GetMinTimePoint();
  unsigned int max_t = m_MegaCaptureReader->GetMaxTimePoint();

  unsigned int min_ch = m_MegaCaptureReader->GetMinChannel();
  unsigned int max_ch = m_MegaCaptureReader->GetMaxChannel();

  unsigned int NumberOfChannels = max_ch - min_ch + 1;

  vtkImageData* temp = m_MegaCaptureReader->GetOutput( min_ch );

  int extent[6];
  temp->GetExtent( extent );

  // Initialize the widgets with the good number of channels
  // it will update the size of the related combobox
  m_NavigationDockWidget->SetNumberOfChannels( NumberOfChannels );
  m_OneClickSegmentationDockWidget->SetNumberOfChannels( NumberOfChannels );
  m_ManualSegmentationDockWidget->SetNumberOfChannels( NumberOfChannels );

  // Set up QSpinBox in m_VideoRecorderWidget
  if( NumberOfChannels > 1 )
    {
    m_NavigationDockWidget->SetChannel( 0 );
    m_InternalImages.resize( NumberOfChannels, NULL );

    for( unsigned int i = 1; i < NumberOfChannels; i++ )
      {
      m_NavigationDockWidget->SetChannel( i );
      }
    }

  m_NavigationDockWidget->SetXMinimumAndMaximum( extent[0], extent[1] );
  m_NavigationDockWidget->SetXSlice( ( extent[0] + extent[1] ) / 2 );

  m_NavigationDockWidget->SetYMinimumAndMaximum( extent[2], extent[3] );
  m_NavigationDockWidget->SetYSlice( ( extent[2] + extent[3] ) / 2 );

  m_NavigationDockWidget->SetZMinimumAndMaximum( extent[4], extent[5] );
  m_NavigationDockWidget->SetZSlice( ( extent[4] + extent[5] ) / 2 );

  m_NavigationDockWidget->SetTMinimumAndMaximum( min_t, max_t );
  m_NavigationDockWidget->SetTSlice( iTimePoint );

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
  QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

  RemoveAllTracesForPresentTimePointManager();

  m_TimePoint = iTimePoint;
  m_MegaCaptureReader->SetTimePoint( m_TimePoint );

  unsigned int min_ch = m_MegaCaptureReader->GetMinChannel();
  unsigned int max_ch = m_MegaCaptureReader->GetMaxChannel();
  m_MegaCaptureReader->Update();
  
  if( max_ch != min_ch )
    {
    vtkSmartPointer< vtkImageAppendComponents > append_filter =
      vtkSmartPointer< vtkImageAppendComponents >::New();

    for( unsigned int i = min_ch; i <= max_ch; i++ )
      {
      m_InternalImages[i] = m_MegaCaptureReader->GetOutput( i );
      append_filter->AddInput( m_InternalImages[i] );
      }
    // This is really stupid!!!
    if( max_ch < 2 )
      {
      for( unsigned int i = max_ch + 1; i < 3; i++ )
        {
        append_filter->AddInput( m_InternalImages[0] );
        }
      }
    append_filter->Update();

    if( this->m_NavigationDockWidget->ShowAllChannels() )
      {
      m_Image->ShallowCopy( append_filter->GetOutput() );
      }
    else
      {
      int ch = this->m_NavigationDockWidget->GetCurrentChannel();
      if( ch != -1 )
        {
        m_Image->ShallowCopy( m_InternalImages[ch] );
        }
      }
    }
  else
    {
    m_Image->ShallowCopy( m_MegaCaptureReader->GetOutput( min_ch ) );
    m_Image->SetNumberOfScalarComponents( 1 );
    }


  LoadAllTracesForCurrentTimePointManager();
  Update();

  QApplication::restoreOverrideCursor();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetTimePointWithLSMReaders( const int& iTimePoint )
{
  QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );

  RemoveAllTracesForPresentTimePointManager();

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

  LoadAllTracesForCurrentTimePointManager();
  Update();

  QApplication::restoreOverrideCursor();
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
        if (this->m_DataBaseTables->IsDatabaseUsed())
          {
          this->m_DataBaseTables->UpdateListMeshes(iTimePoint);
          }
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
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(false);
#endif
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
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
   this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
#endif
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
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
#endif
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
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
#endif
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
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  this->m_VideoRecorderWidget->toggleViewAction()->setEnabled(true);
#endif
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
    int ch = this->m_NavigationDockWidget->GetCurrentChannel();
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

    if( iHighlighted )
      {
      contour_property->SetColor( 1., 0., 0. );
      contour_property->SetLineWidth( 3. );
      }
    else
      {
      contour_property->SetColor( iR, iG, iB );
      }

    // get corresponding actor from visualization
    vtkPolyData* contour_copy = vtkPolyData::New();
    contour_copy->ShallowCopy( contour );

//     std::vector< vtkQuadricLODActor* > contour_actor =
    std::vector< vtkActor* > contour_actor =
      this->AddContour( iDir, contour_copy,
        contour_property );

    // get meshid from the visu dock widget (SpinBox)
    //unsigned int meshid = m_ManualSegmentationDockWidget->GetMeshId();
    unsigned int meshid = this->m_TraceManualEditingDockWidget
      ->m_TraceWidget->GetCurrentCollectionID();

    if( iSaveInDataBase )
      {
      if( !m_ReEditContourMode )
        {
        std::pair< std::string, QColor > ColorData =
          this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorComboBox->GetCurrentColorData();

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
       this->m_TraceManualEditingDockWidget->setEnabled( true );
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
      ContourMeshStructure temp( m_ContourId, contour_actor[i],
        contour_nodes, meshid, iTCoord, iHighlighted, iR, iG, iB, iA, i );
      m_ContourContainer.insert( temp );
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
  if( this->m_TraceManualEditingDockWidget->m_TraceWidget->GetCurrentCollectionID() == -1 )
    {
    r = 0.1;
    g = 0.5;
    b = 0.7;
    }
  else
    {
    QColor color =
      this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorComboBox->GetCurrentColorData().second;
    color.getRgbF( &r, &g, &b );
    }
  
  bool highlighted;
  if (m_ReEditContourMode)
    {
    highlighted = true;
    }
  else
    {
    highlighted = false;
    }

  // get from m_DataBaseTables if user is using one gofiguredatabase or not.
  // In such case contours are saved in the database, else they are not!
  bool saveindatabase = m_DataBaseTables->IsDatabaseUsed();

  // to make sure that m_ContourId is set to the right value
  int ContourID = -1;
  unsigned int i;

  if( m_ReEditContourMode )
    {
    ContourID = m_ContourId;

    ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
      it = m_ContourContainer.get< TraceID >().find( m_ContourId );

    if( it->TraceID == m_ContourId )
      {
      // save color
      r = it->rgba[0];
      g = it->rgba[1];
      b = it->rgba[2];
//       a = it->rgba[3];

      // We have to remove the polydata from the container too
      m_ContourContainer.get< TraceID >().erase( m_ContourId );
      }
    }

  for( i = 0; i < m_ContourWidget.size(); i++ )
    {
    ValidateContour( ContourID, i, r, g, b, a, highlighted, m_TimePoint, saveindatabase );
    }

  if( m_ReEditContourMode )
    {
    m_ManualSegmentationDockWidget->setEnabled( false );

    for( i = 0; i < m_ContourWidget.size(); i++ )
      {
      m_ContourWidget[i]->Initialize( NULL );
      }

    std::list< int > listofrowstobeselected;
    listofrowstobeselected.push_back( m_ContourId );

    m_DataBaseTables->ChangeContoursToHighLightInfoFromVisu( listofrowstobeselected,
      m_ReEditContourMode );

    m_ManualSegmentationDockWidget->setEnabled( true );
    this->m_TraceManualEditingDockWidget->setEnabled( true );
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
  float linewidth = static_cast< float >( m_LinesWidth );
  QColor linecolor = m_LinesColor;
  QColor nodecolor = m_NodesColor;
  QColor activenodecolor = m_ActiveNodesColor;

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
void
QGoTabImageView3DwT::
RemoveAllTracesForPresentTimePointManager( )
{
  if( m_TimePoint >= 0 )
    {
    // Removes contours from visu
    RemoveAllTracesForGivenTimePoint( m_TimePoint, m_ContourContainer );
    // Removes meshes from visu
    RemoveAllTracesForGivenTimePoint( m_TimePoint, m_MeshContainer );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
RemoveAllTracesForGivenTimePoint( const unsigned int& iT,
      const std::string& iTraceName )
{
  if( iTraceName.compare("contour") == 0 )
    {
    // Removes contours from visu
    RemoveAllTracesForGivenTimePoint( iT, m_ContourContainer );
    }
  if( iTraceName.compare( "mesh" ) == 0 )
    {
    // Removes meshes from visu
    RemoveAllTracesForGivenTimePoint( iT, m_MeshContainer );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
RemoveAllTracesForGivenTimePoint( const unsigned int& iT,
    ContourMeshStructureMultiIndexContainer& iContainer )
{
  if( iContainer.size() > 0 )
    {
    ContourMeshStructureMultiIndexContainer::index< TCoord >::type::iterator it0, it1;
    boost::tuples::tie(it0,it1) =
        iContainer.get< TCoord >().equal_range( iT );

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
LoadAllTracesForCurrentTimePointManager()
{
  if( m_TimePoint >= 0 )
    {
    // Loads contours for visu
    LoadAllTracesForGivenTimePoint( m_TimePoint, m_ContourContainer );
    // Loads meshes for visu
    LoadAllTracesForGivenTimePoint( m_TimePoint, m_MeshContainer );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
LoadAllTracesForGivenTimePoint( const unsigned int& iT,
    ContourMeshStructureMultiIndexContainer& iContainer )
{
  if( iContainer.size() > 0 )
    {
    ContourMeshStructureMultiIndexContainer::index< TCoord >::type::iterator it0, it1;
    boost::tuples::tie(it0,it1) =
        iContainer.get< TCoord >().equal_range( iT );

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
/*
void
QGoTabImageView3DwT::
AddPolyData( vtkPolyData* iMesh )
{
  m_ImageView->AddMesh( iMesh );
//   this->AddContour( 0, iMesh, 0 );
}
*/
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AddTraceFromNodesManager( const unsigned int& iContourID,
    vtkPolyData* iNodes,
    const double iRgba[4],
    const bool& iHighlighted,
    const unsigned int& iTCoord,
    const bool& iSaveInDataBase,
    std::string iTrace)
{
  // If we want to add a contour
  if( iTrace.compare( "contour" ) == 0 )
    {
    AddContourFromNodes(iContourID, iNodes, iRgba, iHighlighted, iTCoord,
        iSaveInDataBase);
    }
  // If we want to add a mesh
  if( iTrace.compare( "mesh" ) == 0 )
    {
    AddMeshFromNodes(iContourID, iNodes, iRgba, iHighlighted, iTCoord,
        iSaveInDataBase);
    }
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AddContourFromNodes( const unsigned int& iContourID,
  vtkPolyData* iNodes,
  const double iRgba[4],
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
AddMeshFromNodes( const unsigned int& iContourID,
  vtkPolyData* iNodes,
  const double iRgba[4],
  const bool& iHighlighted,
  const unsigned int& iTCoord,
  const bool& iSaveInDataBase )
{
  AddMeshFromNodes( iContourID, iNodes, iRgba[0], iRgba[1], iRgba[2], iRgba[3],
    iHighlighted, iTCoord, iSaveInDataBase );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
AddMeshFromNodes( const unsigned int& iContourID,
  vtkPolyData* iNodes,
  const double& iR, const double& iG, const double& iB, const double& iA,
  const bool& iHighlighted, const unsigned int& iTCoord, const bool& iSaveInDataBase )
{
  this->SavePolyDataAsMeshInDB(  iNodes, iContourID, 0, iR,  iG,  iB,
     iA,  iHighlighted,  iTCoord, iSaveInDataBase );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
UpdateDBAndCollectionIDComboBoxForANewCreatedCollection()
{
  //first, save in the database:
  std::string TraceName =  this->m_TraceManualEditingDockWidget->m_TraceWidget->TraceName->text().toStdString();
  std::string CellType = "";
  std::string SubCellType = "";
  if ( TraceName == "contour") //for a mesh, collection of contour;
    { 
    CellType = this->m_TraceManualEditingDockWidget->m_TraceWidget->GetCurrentCellType();
    SubCellType = this->m_TraceManualEditingDockWidget->m_TraceWidget->GetCurrentSubCellType();
    }
  std::pair<std::string,QColor> NewCollectionToAddInComboBox =

  this->m_DataBaseTables->SaveNewCollectionInDB(
  this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorComboBox->GetCurrentColorData(),
  this->m_TraceManualEditingDockWidget->m_TraceWidget->TraceName->text().toStdString(),
  this->GetTimePoint(),CellType,SubCellType);

  //second, update the ColorIDCollectionComboBox with the new created ID:
  this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorIDCollectionComboBox->addColor(
    NewCollectionToAddInComboBox.second,NewCollectionToAddInComboBox.first.c_str());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
PassInfoForDBForCurrentSelectedColor()
{
  this->m_DataBaseTables->UpdateCurrentColorData(
    this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorComboBox->GetCurrentColorData());

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
PassInfoForCurrentCollectionID()
{
  this->m_DataBaseTables->SetCurrentCollectionID(
    this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorIDCollectionComboBox->GetCurrentColorData());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ReEditContour( const unsigned int& iId )
{
  if( !m_ContourContainer.empty() )
    {
    ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
      it = m_ContourContainer.get< TraceID >().find( iId );

    if( it != m_ContourContainer.get< TraceID >().end() )
      {
      int c_dir;
      vtkActor* c_actor;
      vtkPolyData* c_nodes = NULL;

      while( it != m_ContourContainer.get< TraceID >().end() )
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

      m_ContourContainer.erase( iId );

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
        this->m_ManualSegmentationDockWidget->show();
        this->m_ModeActions[0]->setChecked(true);
        m_ContourWidget[dir]->Initialize( c_nodes );
        m_ManualSegmentationDockWidget->setEnabled( true );
        this->m_TraceManualEditingDockWidget->setEnabled( false );
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
  /// Modifiy "picked" to "modified"
  std::list< vtkProp3D* > listofpicked = m_ImageView->GetListOfPickedContours();
  std::list< vtkProp3D* >::iterator it = listofpicked.begin();

  while( it != listofpicked.end() )
    {
    // Mode 0: One click selection
    HighLightContainer(m_ContourContainer, static_cast< vtkActor* >( *it ));
    ++it;
    }
}
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ListHighLightMeshes()
{
  std::list< vtkProp3D* > listofpicked = m_ImageView->GetListOfModifiedActors3D();
  std::list< vtkProp3D* >::iterator it = listofpicked.begin();

  while( it != listofpicked.end() )
    {
    HighLightContainer(m_MeshContainer, static_cast< vtkActor* >( *it ));
    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighLightContainer( ContourMeshStructureMultiIndexContainer& iContainer, vtkActor* iActor)
{
  vtkProperty* select_property = vtkProperty::New();
  select_property->SetColor( 1., 0., 0. );
  select_property->SetLineWidth( 3. );

  // Change the corresponding highlighted value in the container
  ContourMeshStructureMultiIndexContainer::index< Actor >::type::iterator
    actor_it = iContainer.get< Actor >().find( iActor );

  if( actor_it != iContainer.get< Actor >().end() )
    {
    unsigned int trace_id = actor_it->TraceID;

    ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
      traceid_it = iContainer.get< TraceID >().find( trace_id );

    if( traceid_it != iContainer.get< TraceID >().end() )
      {
      while( ( traceid_it != iContainer.get< TraceID >().end() )
          && ( (*traceid_it).TraceID == trace_id ) )
        {
        if( !traceid_it->Highlighted )
          {
          m_ImageView->ChangeActorProperty( traceid_it->Direction,
            traceid_it->Actor, select_property );

          ContourMeshStructure temp( *traceid_it );
          temp.Highlighted = true;
          iContainer.get< TraceID >().replace( traceid_it, temp );
          }
        else
          {
          vtkProperty* temp_property = vtkProperty::New();
          temp_property->SetColor( traceid_it->rgba[0], traceid_it->rgba[1], traceid_it->rgba[2] );
          temp_property->SetLineWidth( 1. );

          m_ImageView->ChangeActorProperty( traceid_it->Direction,
            traceid_it->Actor, temp_property );
          temp_property->Delete();

          ContourMeshStructure temp( *traceid_it );
          temp.Highlighted = false;
          iContainer.get< TraceID >().replace( traceid_it, temp );
          }
        ++traceid_it;
        }
      }
    }
  select_property->Delete();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighLightTracesFromTableManager( )
{
  // In which table are we?
  std::string currentTrace = this->m_DataBaseTables->InWhichTableAreWe();

  // If we are in contour
  if( currentTrace.compare( "contour" ) == 0 )
    {
    HighLightTracesFromTable( m_ContourContainer, currentTrace );
    }
  // If we are in mesh
  if( currentTrace.compare( "mesh" ) == 0 )
    {
    HighLightTracesFromTable( m_MeshContainer, currentTrace );
    }
}
//-------------------------------------------------------------------------

// Maybe we should add the "highlight property" as a parameter

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
HighLightTracesFromTable( ContourMeshStructureMultiIndexContainer& iContainer,
    std::string iCurrentTrace)
{
  ContourMeshStructureMultiIndexContainer::iterator
  it = this->m_DataBaseTables->GetTracesInfoListForVisu( iCurrentTrace.c_str() )
                             ->begin();
  unsigned int trace_id = 0;

  vtkProperty* select_property = vtkProperty::New();
  select_property->SetColor( 1., 0., 0. );
  select_property->SetLineWidth( 3. );

  while( it != this->m_DataBaseTables
                   ->GetTracesInfoListForVisu( iCurrentTrace.c_str() )->end() )
    {
    trace_id = it->TraceID;

    ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
    traceid_it = iContainer.get< TraceID >().find( trace_id );

    if( traceid_it != iContainer.get< TraceID >().end() )
      {
      while( ( traceid_it != iContainer.get< TraceID >().end() )
          && ( (*traceid_it).TraceID == trace_id ) )
        {
        if( it->Highlighted != traceid_it->Highlighted )
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
          ContourMeshStructure temp( *traceid_it );
          temp.Highlighted = it->Highlighted;

          iContainer.get< TraceID >().replace( traceid_it, temp );
          }
        ++traceid_it;
        }
      }

    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*void
//QGoTabImageView3DwT::
//ChangeContourColorFromTable( const QColor& iColor )
{
  // 1-get a container of modified contours
  //this->m_DataBaseTables->

  // 2-iterate on the container
  //    for each element
  //      - get the new color
  //      - update m_ContourContainer
  //      - update contour color in the visualization
  
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SelectContoursInTable( )
{
  if( this->m_DataBaseTables->IsDatabaseUsed() )
    {
    std::list< vtkProp3D* > listofpicked = m_ImageView->GetListOfPickedContours();

    if( !listofpicked.empty() )
      {
      this->m_DataBaseTables->ChangeContoursToHighLightInfoFromVisu(
          SelectTraceInTable( m_ContourContainer, listofpicked ), false );
      }
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ListSelectMeshesInTable( )
{
  if( this->m_DataBaseTables->IsDatabaseUsed() )
    {
    std::list< vtkProp3D* > listofpicked = m_ImageView->GetListOfModifiedActors3D();

    if( !listofpicked.empty() )
      {
      this->m_DataBaseTables->ChangeMeshesToHighLightInfoFromVisu(
          SelectTraceInTable( m_MeshContainer, listofpicked ) );
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< int >
QGoTabImageView3DwT::
SelectTraceInTable( ContourMeshStructureMultiIndexContainer& iContainer ,
                    std::list< vtkProp3D* >                  iActorList)
{
  std::list< int > listofrowstobeselected;
  std::list< vtkProp3D* >::iterator it = iActorList.begin();

  while( it != iActorList.end() )
    {
    // Change the corresponding highlighted value in the container
    ContourMeshStructureMultiIndexContainer::index< Actor >::type::iterator
      actor_it = iContainer.get< Actor >().find( static_cast< vtkActor* >( *it ) );

    if( actor_it != iContainer.get< Actor >().end() )
      {
      int trace_id = static_cast< int >( actor_it->TraceID );
      listofrowstobeselected.push_back( trace_id );
      }
    ++it;
    }

  return listofrowstobeselected;
}

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
DeleteTracesFromTableManager( const std::list< int >& iList )
{
  // In which table are we?
  std::string currentTrace = this->m_DataBaseTables->InWhichTableAreWe();

  // If we are in contour
  if( currentTrace.compare( "contour" ) == 0 )
    {
    DeleteTracesFromTable( m_ContourContainer, iList );
    }
  // If we are in mesh
  if( currentTrace.compare( "mesh" ) == 0 )
    {
    DeleteTracesFromTable( m_MeshContainer, iList );
    }

  // Update the visualization
  this->m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
DeleteTracesFromTable( ContourMeshStructureMultiIndexContainer& iContainer,
    const std::list< int >& iList )
{
  std::list< int >::const_iterator traceid_it = iList.begin();
  ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator it;

  while( traceid_it != iList.end() )
    {
    it = iContainer.get< TraceID >().find( *traceid_it );
    if( it != iContainer.get< TraceID >().end() )
      {
      int c_dir;
      vtkActor* c_actor;
      vtkPolyData* c_nodes;
      while( it != iContainer.get< TraceID >().end() )
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
      iContainer.erase( *traceid_it );
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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
SetSliceView()
{
#if defined ( ENABLEFFMPEG ) || defined ( ENABLEAVI )
  m_VideoRecorderWidget->SetCurrentX( this->GetSliceViewYZ() );
  m_VideoRecorderWidget->SetCurrentY( this->GetSliceViewXZ() );
  m_VideoRecorderWidget->SetCurrentZ( this->GetSliceViewXY() );
  m_VideoRecorderWidget->SetCurrentT( this->GetTimePoint() );
#endif
}
//-------------------------------------------------------------------------
QGoManualSegmentationDockWidget* QGoTabImageView3DwT::
  GetManualSegmentationWidget()
{
  return this->m_ManualSegmentationDockWidget;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoTraceManualEditingWidget* QGoTabImageView3DwT::
  GetTraceManualEditingWidget()
{
  return this->m_TraceManualEditingDockWidget->m_TraceWidget;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::SetTheCurrentCellType()
{
  this->m_TraceManualEditingDockWidget->m_TraceWidget
    ->SetCurrentCellType(this->m_DataBaseTables->GetNameNewCellType());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::SetTheCurrentSubCellType()
{
  this->m_TraceManualEditingDockWidget->m_TraceWidget
    ->SetCurrentSubCellType(this->m_DataBaseTables->GetNameNewSubCellType());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ApplyContourSemiAutoSegmentation()
{
  this->LevelSetSegmentation2D();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ApplyOneClickSegmentationFilter()
{
  // Check the filter to be applied
  // 0 = circle contours (sphere aspect)
  // 1 = sphere ( 3D volume - no contours)
  // 2 = levelset in 3D
  int filterToBeApplied = this->m_OneClickSegmentationDockWidget->GetFilter();

  switch ( filterToBeApplied )
    {
    case 0 :
    // circle contours creation (sphere aspect)
    this->OneClickSphereContours();
    break;

    case 1 :
    // sphere (3D volume creation)
    this->OneClickSphereMeshes();
    break;

    case 2 :
    // 3d level set
    this->LevelSetSegmentation3D();
    break;

    default :
    break;
    }
}
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
OneClickSphereContours()
{
  // Get seeds
  m_SeedsWorldPosition = this->m_ImageView->GetAllSeeds();

  // Get informations about contour to be saved

  // pos[] will contain position of a seed
  double seed_pos[3];

  // Initialize the segmentation
  QGoSeedsSegmentation seedsSegmentation;
  seedsSegmentation.setRadius(
      m_OneClickSegmentationDockWidget->GetRadius() );
  seedsSegmentation.setOriginImageInformation(
      this->m_ImageView->GetImageViewer(0));

  // Apply filter for each seed
  for( int i = 0; i < m_SeedsWorldPosition->GetNumberOfPoints(); i++ )
    {

    std::list<int> listContoursIDs;
    // Put position of each seed "i" in "seed_pos[3]"
    m_SeedsWorldPosition->GetPoint( i, seed_pos );
    seedsSegmentation.setSeedsPosition( seed_pos );

    std::vector< vtkSmartPointer<vtkPolyData> > ContoursForOnePoint;

    // Each segmentation metho returns the appropriate output
    ContoursForOnePoint = seedsSegmentation.SphereContoursSegmentation();

    // Save polydatas (=contours) in DB
    for( unsigned int j = 1; j < ContoursForOnePoint.size(); j++)
      {
      listContoursIDs.push_back(this->SavePolyDataAsContourInDB( ContoursForOnePoint[j] ));
      }
    // assign contours to mesh
    // will increment mesh ID automatically
    if( m_DataBaseTables->IsDatabaseUsed() )
      {
      m_DataBaseTables->CreateMeshFromOneClickSegmentation(listContoursIDs);
      }
    }

  // Erase seeds once everything is stored in DB
  this->m_ImageView->ClearAllSeeds();

  // Update visualization
  this->m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
OneClickSphereMeshes()
{
  // Get seeds
  m_SeedsWorldPosition = this->m_ImageView->GetAllSeeds();

  // pos[] will contain position of a seed
  double seed_pos[3];

  // Initialize the segmentation
  QGoSeedsSegmentation seedsSegmentation;
  seedsSegmentation.setRadius(
      m_OneClickSegmentationDockWidget->GetRadius() );

  // Create "spheres" for all the points
  for( int i = 0; i < m_SeedsWorldPosition->GetNumberOfPoints(); i++ )
    {
    // Put position of each seed "i" in "pos[3]"
    m_SeedsWorldPosition->GetPoint( i, seed_pos );
    seedsSegmentation.setSeedsPosition( seed_pos );

    // Save polydatas/mesh (=volume) in DB
    this->SavePolyDataAsMeshInDB(
        seedsSegmentation.SphereVolumeSegmentation() );
    }

  // Erase seeds once everything is stored in DB
  this->m_ImageView->ClearAllSeeds();

  // Update visualization
  this->m_ImageView->UpdateRenderWindows();
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
LevelSetSegmentation2D()
{
  // Get seeds (centers for level sets)
  m_SeedsWorldPosition = this->m_ImageView->GetAllSeeds();
  // pos[] will contain position of a seed
  double seed_pos[3];

  // Get input volume, according to selected channel
  vtkSmartPointer<vtkImageData> inputVolume = vtkSmartPointer<vtkImageData>::New();
  if( ( !m_InternalImages.empty() ) )
    {
    inputVolume->ShallowCopy(
        m_InternalImages[ m_ManualSegmentationDockWidget->GetChannel() ] );
    }

  // Initialize the segmentation
  QGoSeedsSegmentation seedsSegmentation;
  seedsSegmentation.setInputVolume( inputVolume );
  seedsSegmentation.setRadius(
      m_ManualSegmentationDockWidget->GetRadius() );
  seedsSegmentation.setNumberOfIterations(
      m_ManualSegmentationDockWidget->GetNumberOfIterations() );
  seedsSegmentation.setCurvatureWeight(
      m_ManualSegmentationDockWidget->GetCurvatureWeight() );

  // Apply filter for each seed
  for( int i = 0; i < m_SeedsWorldPosition->GetNumberOfPoints(); i++ )
    {
    // Put position of each seed "i" in "seed_pos[3]"
    m_SeedsWorldPosition->GetPoint( i, seed_pos );
    seedsSegmentation.setSeedsPosition( seed_pos );
    //
    int orientation = 0;
    // Save contour in database
    SavePolyDataAsContourInDB(
        seedsSegmentation.LevelSetSegmentation2D( orientation ) );
    }

  // Erase seeds once everything is stored in DB
  this->m_ImageView->ClearAllSeeds();

  // Update visualization
  this->m_ImageView->UpdateRenderWindows();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
LevelSetSegmentation3D()
{
  // Get seeds (centers for level sets)
  m_SeedsWorldPosition = this->m_ImageView->GetAllSeeds();
  // pos[] will contain position of a seed
  double seed_pos[3];

  // Get input volume, according to selected channel
  vtkSmartPointer<vtkImageData> inputVolume = vtkSmartPointer<vtkImageData>::New();
  if( ( !m_InternalImages.empty() ) )
    {
    inputVolume->ShallowCopy(
        m_InternalImages[ m_OneClickSegmentationDockWidget->GetChannel() ] );
    }

  // Initialize the segmentation
  QGoSeedsSegmentation seedsSegmentation;
  seedsSegmentation.setInputVolume( inputVolume );
  seedsSegmentation.setRadius(
      m_OneClickSegmentationDockWidget->GetRadius() );
  seedsSegmentation.setNumberOfIterations(
      m_OneClickSegmentationDockWidget->GetNumberOfIterations() );
  seedsSegmentation.setCurvatureWeight(
      m_OneClickSegmentationDockWidget->GetCurvatureWeight() );

  // Apply filter for each seed
  for( int i = 0; i < m_SeedsWorldPosition->GetNumberOfPoints(); i++ )
    {
    // Put position of each seed "i" in "seed_pos[3]"
    m_SeedsWorldPosition->GetPoint( i, seed_pos );
    seedsSegmentation.setSeedsPosition( seed_pos );

    SavePolyDataAsMeshInDB(
        seedsSegmentation.LevelSetSegmentation3D() );
  }

  // Erase seeds once everything is stored in DB
  this->m_ImageView->ClearAllSeeds();

  // Update visualization
  this->m_ImageView->UpdateRenderWindows();
}

//-------------------------------------------------------------------------
int
QGoTabImageView3DwT::
SavePolyDataAsContourInDB( vtkPolyData* iView, const int& iContourID,
    const int& iDir, const double& iR, const double& iG, const double& iB,
    const double& iA, const bool& iHighlighted, const unsigned int& iTCoord,
    const bool& iSaveInDataBase )
{
  //generate contour
  vtkSmartPointer<vtkOrientedGlyphContourRepresentation> contourRep =
      vtkSmartPointer<vtkOrientedGlyphContourRepresentation>::New();
  //contourRep->GetLinesProperty()->SetColor(1, 0, 0); //set color to red

  vtkSmartPointer<vtkContourWidget> contourWidget =
      vtkSmartPointer<vtkContourWidget>::New();
  contourWidget->SetInteractor(this->m_ImageView->GetImageViewer(0)
      ->GetInteractor());
  contourWidget->SetRepresentation(contourRep);
  contourWidget->On();

  contourWidget->Initialize(iView);
  contourWidget->CloseLoop();

  vtkPolyData* contour =
  contourRep->GetContourRepresentationAsPolyData();

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
    contourRep->GetNodePolyData( contour_nodes );


    vtkProperty* contour_property = vtkProperty::New();
    contour_property->SetColor( iR, iG, iB );

    // get corresponding actor from visualization
    vtkPolyData* contour_copy = vtkPolyData::New();
    contour_copy->ShallowCopy( contour );

    std::vector< vtkActor* > contour_actor =
    this->AddContour( iDir, contour_copy, contour_property );

    // get meshid from the visu dock widget (SpinBox)
    // mesh ID will be assigned once all contours are already created in database
    // using a special method which will also create tracks
    unsigned int meshid = 0;

  if( iSaveInDataBase )
    {
    std::pair< std::string, QColor > ColorData =
        this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorComboBox->GetCurrentColorData();

    // Save contour in database!
    m_ContourId = m_DataBaseTables->SaveContoursFromVisuInDB( min_idx[0],
        min_idx[1], min_idx[2], iTCoord, max_idx[0],
        max_idx[1], max_idx[2], contour_nodes, ColorData, meshid );

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
    ContourMeshStructure temp( m_ContourId,
        reinterpret_cast< vtkActor* >( contour_actor[i] ), contour_nodes,
        meshid, iTCoord, iHighlighted, iR, iG, iB, iA, i );
    m_ContourContainer.insert( temp );
    }

  if( ( !iSaveInDataBase ) && ( iContourID != -1 ) )
    {
    ++m_ContourId;
    }
  }

  contourWidget->Off();

  return m_ContourId;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
int
QGoTabImageView3DwT::
SavePolyDataAsContourInDB( vtkPolyData* iView )
{
  // get color from the dock widget
  double r, g, b, a( 1. );
  //QColor color = m_ManualSegmentationDockWidget->GetValidatedColor();
  if( this->m_TraceManualEditingDockWidget->m_TraceWidget->GetCurrentCollectionID() == -1 )
    {
    r = 0.1;
    g = 0.5;
    b = 0.7;
    }
  else
    {
    QColor color =
      this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorComboBox->GetCurrentColorData().second;
    color.getRgbF( &r, &g, &b );
    }

  bool highlighted( false );

  // get from m_DataBaseTables if user is using one gofiguredatabase or not.
  // In such a case contours are saved in the database, else they are not!
  bool saveindatabase = m_DataBaseTables->IsDatabaseUsed();

  // to make sure that m_ContourId is set to the right value
  int ContourID = -1;

  return SavePolyDataAsContourInDB( iView, ContourID, 0, r, g, b, a, highlighted,
      m_TimePoint, saveindatabase );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int
QGoTabImageView3DwT::
SavePolyDataAsMeshInDB( vtkPolyData* iView, const int& iContourID,
    const int& iDir, const double& iR, const double& iG, const double& iB,
    const double& iA, const bool& iHighlighted, const unsigned int& iTCoord,
    const bool& iSaveInDataBase )
{
  // map to graphics library
  vtkPolyDataMapper *map = vtkPolyDataMapper::New();
  map->SetInput( iView );

  // Compute Bounding Box
  double bounds[6];
  map->GetBounds( bounds );

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

  vtkProperty*  mesh_property = vtkProperty::New();
  mesh_property->SetColor( iR, iG, iB );

  std::vector< vtkActor* > contour_actor;

  // dont't create actors if there is no polydata to be displayed
  if ( iView )
    {
    contour_actor =
        this->AddContour( iDir, iView, mesh_property );
    }

  // get meshid from the visu dock widget (SpinBox)
  unsigned int trackid = this->m_TraceManualEditingDockWidget->m_TraceWidget->GetCurrentCollectionID();

  if( iSaveInDataBase )
    {
    std::pair< std::string, QColor > ColorData =
        this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorComboBox->GetCurrentColorData();

    // Save mesh in database
    //don't use m_ContourId
    GoFigureMeshAttributes MeshAttributes = ComputeMeshAttributes( iView );
  
    m_MeshId = m_DataBaseTables->SaveMeshFromVisuInDB( min_idx[0],
        min_idx[1], min_idx[2], iTCoord, max_idx[0],
        max_idx[1], max_idx[2], iView, &MeshAttributes );
    }
  else
    {
    m_MeshId = iContourID;
    }

  // fill the container
  for( i = 0; i < contour_actor.size(); i++ )
    {
    ContourMeshStructure temp( m_MeshId, contour_actor[i], iView,
        trackid, iTCoord, iHighlighted, iR, iG, iB, iA, i );
    m_MeshContainer.insert( temp );
    }

  ++m_MeshId;

  return 0;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
int
QGoTabImageView3DwT::
SavePolyDataAsMeshInDB( vtkPolyData* iView )
{
  // get color from the dock widget
  double r, g, b, a( 1. );
  //QColor color = m_ManualSegmentationDockWidget->GetValidatedColor();
  if( this->m_TraceManualEditingDockWidget->m_TraceWidget->GetCurrentCollectionID() == -1 )
    {
    r = 0.1;
    g = 0.5;
    b = 0.7;
    }
  else
    {
    QColor color =
      this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorComboBox->GetCurrentColorData().second;
    color.getRgbF( &r, &g, &b );
    }

  bool highlighted( false );

  // get from m_DataBaseTables if user is using one gofiguredatabase or not.
  // In such a case contours are saved in the database, else they are not!
  bool saveindatabase = m_DataBaseTables->IsDatabaseUsed();

  // to make sure that m_ContourId is set to the right value
  int MeshID = -1;

  /// TODO check iDir
  return SavePolyDataAsMeshInDB( iView, MeshID, 0, r, g, b, a, highlighted,
      m_TimePoint, saveindatabase );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::ShowTraceDockWidgetForContour(
  bool ManualSegVisible)
{
  if (ManualSegVisible)
    {
    if(this->m_DataBaseTables->IsDatabaseUsed())
      {
      this->m_TraceManualEditingDockWidget->ShowAndUpdate("contour","mesh");
      this->m_TraceManualEditingDockWidget->m_TraceWidget->SetCollectionID(
          this->m_DataBaseTables->GetListExistingCollectionIDFromDB(
              "contour", this->GetTimePoint() ) );
      this->m_DataBaseTables->blockSignals(true);
      this->m_DataBaseTables->SetTable("contour");
      this->m_DataBaseTables->blockSignals(false);
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
  if(OneClickVisible)
    {
    if(this->m_DataBaseTables->IsDatabaseUsed())
      {
      this->m_TraceManualEditingDockWidget->ShowAndUpdate("mesh","track");
      this->m_TraceManualEditingDockWidget->m_TraceWidget->SetCollectionID(
          this->m_DataBaseTables->GetListExistingCollectionIDFromDB("mesh"));
      this->m_DataBaseTables->blockSignals(true);
      this->m_DataBaseTables->SetTable("mesh");
      this->m_DataBaseTables->blockSignals(false);
      }
    this->m_ManualSegmentationDockWidget->setDisabled(true);
    this->m_ManualSegmentationDockWidget->hide();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::GoToDefaultMenu(std::string iTracename,
  std::string iCollectionName)
{
  this->m_ManualSegmentationDockWidget->setDisabled(true);
  this->m_ManualSegmentationDockWidget->hide();
  this->m_OneClickSegmentationDockWidget->setDisabled(true);
  this->m_OneClickSegmentationDockWidget->hide();
  this->m_ModeActions.at(2)->setChecked(true);
  this->m_TraceManualEditingDockWidget->ShowAndUpdate(iTracename,
    iCollectionName);
  if(this->m_DataBaseTables->IsDatabaseUsed())
    {
    if (iTracename == "contour")
      {
      this->m_TraceManualEditingDockWidget->m_TraceWidget->SetCollectionID(
      this->m_DataBaseTables->GetListExistingCollectionIDFromDB(
          "contour",this->GetTimePoint()));
      }
    else
      {
      this->m_TraceManualEditingDockWidget->m_TraceWidget->SetCollectionID(
        this->m_DataBaseTables->GetListExistingCollectionIDFromDB(iTracename));
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoFigureMeshAttributes
QGoTabImageView3DwT::
ComputeMeshAttributes( vtkPolyData* iMesh )
{
  typedef unsigned char PixelType;
  const unsigned int Dimension = 3;
  typedef itk::Image< PixelType, Dimension > ImageType;

  itk::vtkPolyDataToGoFigureMeshAttributes< ImageType >::Pointer
    calculator = itk::vtkPolyDataToGoFigureMeshAttributes< ImageType >::New();
  calculator->SetPolyData( iMesh );

  GoFigureMeshAttributes oAttributes;

  for( size_t i = 0; i < m_InternalImages.size(); i++ )
    {
    vtkSmartPointer<vtkImageExport> vtk_exporter =
      vtkSmartPointer<vtkImageExport>::New();
    itk::VTKImageImport< ImageType >::Pointer itk_importer =
      itk::VTKImageImport< ImageType >::New();
    vtk_exporter->SetInput( m_InternalImages[i] );
    ConnectPipelines< vtkImageExport, itk::VTKImageImport< ImageType >::Pointer >( vtk_exporter, itk_importer );
    calculator->SetImage( itk_importer->GetOutput() );
    calculator->Update();

    QString q_channelname = this->m_NavigationDockWidget->GetChannelName( i );
    std::string channelname = q_channelname.toStdString();

    oAttributes.m_TotalIntensityMap[channelname] = static_cast< int >( calculator->GetSumIntensity() );
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
  if( this->m_DataBaseTables->IsDatabaseUsed() )
    {
    ContourMeshStructureMultiIndexContainer* ContourToAdd =
      this->m_DataBaseTables->ImportContours(this->GetTimePoint());
    ContourMeshStructureMultiIndexContainer::iterator c_it = ContourToAdd->begin();
    while( c_it != ContourToAdd->end() )
      {
      ContourMeshStructure Contour = *c_it;
      this->AddContourFromNodes(Contour.TraceID,Contour.Nodes,Contour.rgba,
        Contour.Highlighted,Contour.TCoord,false);
      ++c_it;
      }
    //update the TraceManualEditingWidget 
    this->GoToDefaultMenu("contour","mesh");
    this->GetTraceManualEditingWidget()->ColorComboBox->setExistingColors(
      this->m_DataBaseTables->GetColorComboBoxInfofromDB());
    this->GetTraceManualEditingWidget()->SetListCellTypes(
      this->m_DataBaseTables->GetQStringListCellTypes());
    this->GetTraceManualEditingWidget()->SetListSubCellTypes(
      this->m_DataBaseTables->GetQStringListSubCellTypes());
    }
}
//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ChangeColorOfSelectedTracesManager(
  std::pair<std::list<int>,QColor> iSelectedTraces)
{
  // In which table are we?
  std::string currentTrace = this->m_DataBaseTables->InWhichTableAreWe();
  QColor selectedColor = iSelectedTraces.second;

  // If we are in contour
  if( currentTrace.compare( "contour" ) == 0 )
    {
    ChangeColorOfSelectedTraces( m_ContourContainer, currentTrace, selectedColor );
    }
  // If we are in mesh
  if( currentTrace.compare( "mesh" ) == 0 )
    {
    ChangeColorOfSelectedTraces( m_MeshContainer, currentTrace, selectedColor );
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
ChangeColorOfSelectedTraces( ContourMeshStructureMultiIndexContainer& ioContainer,
    std::string iCurrentTrace, QColor iSelectedColor)
{
  ContourMeshStructureMultiIndexContainer::iterator
  it = this->m_DataBaseTables->GetTracesInfoListForVisu( iCurrentTrace.c_str() )
                             ->begin();
  unsigned int trace_id = 0;
  double rgba[4];
  rgba[0] = iSelectedColor.redF();
  rgba[1] = iSelectedColor.greenF();
  rgba[2] = iSelectedColor.blueF();
  rgba[3] = 1;

  while( it != this->m_DataBaseTables
                     ->GetTracesInfoListForVisu( iCurrentTrace.c_str() )->end() )
    {
    trace_id = it->TraceID;
    ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
      traceid_it = ioContainer.get< TraceID >().find( trace_id );
    if( traceid_it != ioContainer.get< TraceID >().end() )
      {
      while( ( traceid_it != ioContainer.get< TraceID >().end() )
        && ( (*traceid_it).TraceID == trace_id ) )
        {
        if ( it->Highlighted )
          {
          ContourMeshStructure temp( *traceid_it );
          temp.Highlighted = it->Highlighted;
          temp.rgba[0] = rgba[0];
          temp.rgba[1] = rgba[1];
          temp.rgba[2] = rgba[2];
          temp.rgba[3] = rgba[3];
          ioContainer.get< TraceID >().replace( traceid_it, temp );
          }
        ++traceid_it;
        }
      }
    ++it;
    }
}

//-------------------------------------------------------------------------
void
QGoTabImageView3DwT::
CreateMeshFromSelectedContours( ContourMeshStructureMultiIndexContainer& iContainer )
{
/*  ContourMeshStructureMultiIndexContainer::iterator it = iContainer.begin();

  // get the time point
  unsigned int tcoord = it->TCoord;

  std::vector< vtkPolyData* > list_contours( container.size(), NULL );
  size_t id = 0;
  
  while( it != iContainer.end() )
    {
    list_contours[id] = it->Nodes;
    ++id;
    ++it;
    }

  // iterate on the container and fill a list of contours
  std::vector< ContourMeshStructure >::iterator it = container.begin();

  // make the mesh from the list of contours
  typedef itk::ContourToMeshFilter< std::vector< vtkPolyData* > > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->ProcessContours( list_contours );

  // get the color from the Trace Editing Widget
  double rgba[] = { 1., 1., 1., 1. };

  QColor color =
    this->m_TraceManualEditingDockWidget->m_TraceWidget->ColorComboBox->GetCurrentColorData().second;
  color.getRgbF( &rgba[0], &rgba[1], &rgba[2] );

  unsigned int meshid =
    this->m_TraceManualEditingDockWidget->m_TraceWidget->GetCurrentCollectionID();

  bool highlighted = false;

  // visualize of the generated mesh
  // save into the database
  bool saveindatabase = true;
  this->AddMeshFromNodes( meshid, filter->GetOutput(), rgba, highlighted,
                          tcoord, saveindatabase );*/
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView3DwT::ImportMeshes()
{
  if( this->m_DataBaseTables->IsDatabaseUsed() )
    {
    ContourMeshStructureMultiIndexContainer* MeshToAdd =
      this->m_DataBaseTables->ImportMeshes(this->GetTimePoint());
    ContourMeshStructureMultiIndexContainer::iterator c_it = MeshToAdd->begin();
    while( c_it != MeshToAdd->end() )
      {
      ContourMeshStructure Mesh = *c_it;
      this->AddMeshFromNodes(Mesh.TraceID, Mesh.Nodes,Mesh.rgba, Mesh.Highlighted,
        Mesh.TCoord,false);
      ++c_it;
      }
    //update the TraceManualEditingWidget 
    this->GoToDefaultMenu("mesh","track");
    this->GetTraceManualEditingWidget()->ColorComboBox->setExistingColors(
      this->m_DataBaseTables->GetColorComboBoxInfofromDB());
    this->GetTraceManualEditingWidget()->SetListCellTypes(
      this->m_DataBaseTables->GetQStringListCellTypes());
    this->GetTraceManualEditingWidget()->SetListSubCellTypes(
      this->m_DataBaseTables->GetQStringListSubCellTypes());
    }
}
