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
QGoTabImageView3D::QGoTabImageView3D( QWidget* iParent ) :
  QGoTabImageViewElementBase( iParent )
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

  QWidget* temp = new QWidget();
  temp->resize( 100, 150 );

  QGridLayout* gridlayout = new QGridLayout( temp );
  gridlayout->setContentsMargins(3, -1, 3, -1);

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
void QGoTabImageView3D::CreateAllViewActions()
{
  QActionGroup* group = new QActionGroup( this );

  QAction* QuadViewAction = new QAction( tr("Quad-View"), this );
  QuadViewAction->setCheckable( true );
  QuadViewAction->setChecked( true );

  group->addAction( QuadViewAction );

  m_ViewActions.push_back( QuadViewAction );

  QObject::connect( QuadViewAction, SIGNAL( triggered() ),
    this, SLOT( Quadview() ) );

  QAction* FullScreenXYAction = new QAction( tr( "Full-Screen XY" ), this );
  FullScreenXYAction->setCheckable( true );

  group->addAction( FullScreenXYAction );

  m_ViewActions.push_back( FullScreenXYAction );

  QObject::connect( FullScreenXYAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewXY() ) );

  QAction* FullScreenXZAction = new QAction( tr( "Full-Screen XZ" ), this );
  FullScreenXZAction->setCheckable( true );

  group->addAction( FullScreenXZAction );

  m_ViewActions.push_back( FullScreenXZAction );

  QObject::connect( FullScreenXZAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewXZ() ) );

  QAction* FullScreenYZAction = new QAction( tr( "Full-Screen YZ" ), this );
  FullScreenYZAction->setCheckable( true );

  group->addAction( FullScreenYZAction );

  m_ViewActions.push_back( FullScreenYZAction );

  QObject::connect( FullScreenYZAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewYZ() ) );

  QAction* FullScreenXYZAction = new QAction( tr( "Full-Screen XYZ" ), this );
  FullScreenXYZAction->setCheckable( true );

  group->addAction( FullScreenXYZAction );

  m_ViewActions.push_back( FullScreenXYZAction );

  QObject::connect( FullScreenXYZAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewXYZ() ) );

  QAction* separator = new QAction( this );
  separator->setSeparator( true );

  m_ViewActions.push_back( separator );

//   QAction* toggleviewaction = m_DockWidget->toggleViewAction();
//   toggleviewaction->setText( tr( "Slide Location" ) );
//   m_ViewActions.push_back( toggleviewaction );

  QAction* LookupTableAction = new QAction( tr( "Lookup Table" ), this );
  LookupTableAction->setStatusTip( tr(" Change the associated lookup table" ) );

  // Here write the connection
  QObject::connect( LookupTableAction, SIGNAL( triggered() ),
    this, SLOT( ChangeLookupTable() ) );

  m_ViewActions.push_back( LookupTableAction );

  QAction* ScalarBarAction = new QAction( tr( "Display Scalar Bar" ), this );
  ScalarBarAction->setCheckable( true );
  m_ViewActions.push_back( ScalarBarAction );

  QObject::connect( ScalarBarAction, SIGNAL( toggled( bool ) ),
    this, SLOT( ShowScalarBar( bool ) ) );

  QAction* BackgroundColorAction = new QAction( tr("Background Color"), this );
  m_ViewActions.push_back( BackgroundColorAction );

  QObject::connect( BackgroundColorAction, SIGNAL( triggered() ),
    this, SLOT( ChangeBackgroundColor() ) );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView3D::~QGoTabImageView3D( )
{
  for( int i = 0; i < 3; i++ )
    {
    this->m_ContourRepresentation[i]->Delete();
    this->m_ContourWidget[i]->Delete();
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
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
void QGoTabImageView3D::retranslateUi(QWidget *iParent)
{
  iParent->setWindowTitle( tr( "QGoTabImageView3D" ) );
  Q_UNUSED(iParent);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoFigure::TabDimensionType QGoTabImageView3D::GetTabDimensionType( ) const
{
  return GoFigure::THREE_D;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
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
void QGoTabImageView3D::ShowScalarBar( const bool& iShow )
{
  m_ImageView->ShowScalarBar( iShow );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString QGoTabImageView3D::SnapshotViewXY(
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotViewXY( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString QGoTabImageView3D::SnapshotView2(
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotView2( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString QGoTabImageView3D::SnapshotView3(
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotView3( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString QGoTabImageView3D::SnapshotViewXYZ(
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotViewXYZ( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::SetSliceViewXY( const int& iS )
{
  m_ImageView->SetSliceViewXY( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::SetSliceViewXZ( const int& iS )
{
  m_ImageView->SetSliceViewXZ( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::SetSliceViewYZ( const int& iS )
{
  m_ImageView->SetSliceViewYZ( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::SetFullScreenView( const int& iS )
{
  m_ImageView->SetFullScreenView( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::Quadview()
{
  m_ImageView->Quadview();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::FullScreenViewXY()
{
  m_ImageView->FullScreenViewXY();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::FullScreenViewXZ()
{
  m_ImageView->FullScreenViewXZ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::FullScreenViewYZ()
{
  m_ImageView->FullScreenViewYZ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::FullScreenViewXYZ()
{
  m_ImageView->FullScreenViewXYZ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::GetBackgroundColorFromImageViewer( )
{
  double r, g, b;
  m_ImageView->GetBackgroundColor( r, g, b );
  this->m_BackgroundColor.setRgbF( r, g, b );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3D::SetBackgroundColorToImageViewer( )
{
  m_ImageView->SetBackgroundColor( this->m_BackgroundColor );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction* > QGoTabImageView3D::ViewActions()
{
  return m_ViewActions;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< QDockWidget* > QGoTabImageView3D::DockWidget()
{
  std::list< QDockWidget* > oList;
  oList.push_back( static_cast< QDockWidget* >( m_VisuDockWidget ) );
  return oList;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
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
