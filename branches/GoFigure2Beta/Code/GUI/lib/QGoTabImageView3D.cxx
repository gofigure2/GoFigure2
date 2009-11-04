#include "QGoTabImageView3D.h"

#include "QGoImageView3D.h"
#include "QGoLUTDialog.h"

#include "vtkLookupTable.h"
#include "vtkContourWidget.h"
#include "vtkOrientedGlyphContourRepresentation.h"
#include "vtkImageActorPointPlacer.h"
#include "vtkProperty.h"

#include "vtkImageExtractComponents.h"

#include <QLabel>
#include <QDockWidget>
#include <QSpinBox>
#include <QVBoxLayout>

//--------------------------------------------------------------------------
/**
 * \brief Constructor
 * @param iParent
 */
QGoTabImageView3D::
QGoTabImageView3D( QWidget* iParent )
  : QGoTabImageViewElementBase( iParent )
{
  setupUi( this );

  for( int i = 0; i < 3; i++ )
    {
    this->m_ContourRepresentation.push_back( vtkOrientedGlyphContourRepresentation::New() );
    this->m_ContourRepresentation.back()->GetProperty()->SetColor( 0., 1., 1. );
    this->m_ContourRepresentation.back()->GetLinesProperty()->SetColor( 1., 0., 1. );
    this->m_ContourRepresentation.back()->GetActiveProperty()->SetColor( 1., 1., 0. );

    this->m_ContourWidget.push_back( vtkContourWidget::New() );
    this->m_ContourWidget.back()->SetPriority( 10.0 );
    this->m_ContourWidget.back()->SetInteractor( m_ImageView->GetInteractor( i ) );
    this->m_ContourWidget.back()->Off();
    }

  m_VisuDockWidget = new QGoVisualizationDockWidget( this, 3 );
  m_VisuDockWidget->resize( 120, 300 );

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

  QObject::connect( m_VisuDockWidget, SIGNAL( ShowAllChannelsChanged( bool ) ),
    this, SLOT( ShowAllChannels( bool ) ) );

  CreateAllViewActions();

  ReadSettings();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::CreateAllViewActions()
{
  QActionGroup* group = new QActionGroup( this );

  QAction* QuadViewAction = new QAction( tr("Quad-View"), this );
  QuadViewAction->setCheckable( true );
  QuadViewAction->setChecked( true );

  group->addAction( QuadViewAction );

  this->m_ViewActions.push_back( QuadViewAction );

  QObject::connect( QuadViewAction, SIGNAL( triggered() ),
    this, SLOT( Quadview() ) );

  QAction* FullScreenXYAction = new QAction( tr( "Full-Screen XY" ), this );
  FullScreenXYAction->setCheckable( true );

  group->addAction( FullScreenXYAction );

  this->m_ViewActions.push_back( FullScreenXYAction );

  QObject::connect( FullScreenXYAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewXY() ) );

  QAction* FullScreenXZAction = new QAction( tr( "Full-Screen XZ" ), this );
  FullScreenXZAction->setCheckable( true );

  group->addAction( FullScreenXZAction );

  this->m_ViewActions.push_back( FullScreenXZAction );

  QObject::connect( FullScreenXZAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewXZ() ) );

  QAction* FullScreenYZAction = new QAction( tr( "Full-Screen YZ" ), this );
  FullScreenYZAction->setCheckable( true );

  group->addAction( FullScreenYZAction );

  this->m_ViewActions.push_back( FullScreenYZAction );

  QObject::connect( FullScreenYZAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewYZ() ) );

  QAction* FullScreenXYZAction = new QAction( tr( "Full-Screen XYZ" ), this );
  FullScreenXYZAction->setCheckable( true );

  group->addAction( FullScreenXYZAction );

  this->m_ViewActions.push_back( FullScreenXYZAction );

  QObject::connect( FullScreenXYZAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewXYZ() ) );

  QAction* separator = new QAction( this );
  separator->setSeparator( true );

  this->m_ViewActions.push_back( separator );

  QAction* LookupTableAction = new QAction( tr( "Lookup Table" ), this );
  LookupTableAction->setStatusTip( tr(" Change the associated lookup table" ) );

  // Here write the connection
  QObject::connect( LookupTableAction, SIGNAL( triggered() ),
    this, SLOT( ChangeLookupTable() ) );

  this->m_ViewActions.push_back( LookupTableAction );

  QAction* ScalarBarAction = new QAction( tr( "Display Scalar Bar" ), this );
  ScalarBarAction->setCheckable( true );
  this->m_ViewActions.push_back( ScalarBarAction );

  QObject::connect( ScalarBarAction, SIGNAL( toggled( bool ) ),
    this, SLOT( ShowScalarBar( bool ) ) );

  QAction* BackgroundColorAction = new QAction( tr("Background Color"), this );
  this->m_ViewActions.push_back( BackgroundColorAction );

  QObject::connect( BackgroundColorAction, SIGNAL( triggered() ),
    this, SLOT( ChangeBackgroundColor() ) );

  QAction* separator2 = new QAction( this );
  separator2->setSeparator( true );
  this->m_ViewActions.push_back( separator2 );

  this->m_ViewActions.push_back( m_VisuDockWidget->toggleViewAction() );

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Destructor
 */
QGoTabImageView3D::~QGoTabImageView3D( )
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iParent
 */
void QGoTabImageView3D::setupUi( QWidget* iParent )
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

  this->m_LayOut = new QHBoxLayout( iParent );
  this->m_LayOut->addWidget( m_ImageView  );

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
  iParent->setWindowTitle( tr( "QGoTabImageView3D" ) );
  Q_UNUSED(iParent);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @return
 */
GoFigure::TabDimensionType QGoTabImageView3D::GetTabDimensionType( ) const
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
  m_Image->GetExtent( extent );

  m_VisuDockWidget->SetXSlice( ( extent[0] + extent[1] ) / 2 );
  m_VisuDockWidget->SetYSlice( ( extent[2] + extent[3] ) / 2 );
  m_VisuDockWidget->SetZSlice( ( extent[4] + extent[5] ) / 2 );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::ChangeLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->DeepCopy( QGoLUTDialog::GetLookupTable( this,
    tr( "Choose one look-up table") ) );
  m_ImageView->SetLookupTable( lut );
  lut->Delete();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iShow
 */
void QGoTabImageView3D::ShowScalarBar( const bool& iShow )
{
  m_ImageView->ShowScalarBar( iShow );
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
  const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotViewXY( iType, iBaseName );
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
  const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotView2( iType, iBaseName );
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
  const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotView3( iType, iBaseName );
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
  const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotViewXYZ( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iS
 */
void QGoTabImageView3D::SetSliceViewXY( const int& iS )
{
  m_ImageView->SetSliceViewXY( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iS
 */
void QGoTabImageView3D::SetSliceViewXZ( const int& iS )
{
  m_ImageView->SetSliceViewXZ( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iS
 */
void QGoTabImageView3D::SetSliceViewYZ( const int& iS )
{
  m_ImageView->SetSliceViewYZ( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iS
 */
void QGoTabImageView3D::SetFullScreenView( const int& iS )
{
  m_ImageView->SetFullScreenView( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::Quadview()
{
  m_ImageView->Quadview();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::FullScreenViewXY()
{
  m_ImageView->FullScreenViewXY();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::FullScreenViewXZ()
{
  m_ImageView->FullScreenViewXZ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::FullScreenViewYZ()
{
  m_ImageView->FullScreenViewYZ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::FullScreenViewXYZ()
{
  m_ImageView->FullScreenViewXYZ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::GetBackgroundColorFromImageViewer( )
{
  double r, g, b;
  m_ImageView->GetBackgroundColor( r, g, b );
  this->m_BackgroundColor.setRgbF( r, g, b );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView3D::SetBackgroundColorToImageViewer( )
{
  m_ImageView->SetBackgroundColor( this->m_BackgroundColor );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param image
 */
void QGoTabImageView3D::SetImageToImageViewer( vtkImageData* image )
{
  m_ImageView->SetImage( image );
  m_ImageView->Update();

  for( int i = 0; i < 3; i++ )
    {
    vtkImageActorPointPlacer* point_placer = vtkImageActorPointPlacer::New();
    point_placer->SetImageActor( m_ImageView->GetImageActor( i ) );

    this->m_ContourRepresentation[i]->SetPointPlacer( point_placer );
    point_placer->Delete();

    this->m_ContourWidget[i]->SetRepresentation( this->m_ContourRepresentation[i] );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int* QGoTabImageView3D::
GetImageCoordinatesFromWorldCoordinates( double iPos[3] )
{
  return m_ImageView->GetImageCoordinatesFromWorldCoordinates( iPos );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< vtkActor* >
QGoTabImageView3D::
AddContour( const int& iId,
  vtkPolyData* dataset,
  vtkProperty* iProperty )
{
  return this->m_ImageView->AddContour( iId, dataset, iProperty );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView3D::
RemoveActorFromViewer( const int& iId, vtkActor* iActor )
{
  m_ImageView->RemoveActor( iId, iActor );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::
DisplayActorInViewer( const int& iId, vtkActor* iActor )
{
  m_ImageView->AddActor( iId, iActor );
}
//--------------------------------------------------------------------------
