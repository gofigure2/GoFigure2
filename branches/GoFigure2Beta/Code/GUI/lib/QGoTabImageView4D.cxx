#include "QGoTabImageView4D.h"

#include "QGoImageView3D.h"
#include "QGoLUTDialog.h"
#include "QGoVisualizationDockWidget.h"
#include "QGoManualSegmentationDockWidget.h"

#include "vtkLookupTable.h"
#include "vtkImageAppendComponents.h"

#include <QLabel>
#include <QDockWidget>
#include <QSpinBox>
#include <QVBoxLayout>

#include <algorithm>

//--------------------------------------------------------------------------
QGoTabImageView4D::QGoTabImageView4D( QWidget* iParent ) :
  QGoTabElementBase( iParent ),
  m_XYZImage( 0 ),
  m_XYTImage( 0 ),
  m_BackgroundColor( Qt::black ),
  m_TimePoint( -1 ),
  m_ZSlice( -1 ),
  m_FirstUpdate( true )
{
  m_Reader1 = itk::MegaCaptureReader::New();
  m_Reader2 = itk::MegaCaptureReader::New();

  m_XYZImage = vtkSmartPointer< vtkImageData >::New();
  m_XYTImage = vtkSmartPointer< vtkImageData >::New();

  setupUi( this );

  CreateVisuDockWidget();

  CreateManualSegmentationdockWidget();

  CreateAllViewActions();

  ReadSettings();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::CreateAllViewActions()
{
  QActionGroup* group = new QActionGroup( this );

  QAction* OctViewAction = new QAction( tr("Oct-View"), this );
  OctViewAction->setCheckable( true );
  OctViewAction->setChecked( true );

  group->addAction( OctViewAction );

  QObject::connect( OctViewAction, SIGNAL( triggered() ),
    this, SLOT( Octview() ) );

  m_ViewActions.push_back( OctViewAction );

  QAction* XYZQuadViewAction = new QAction( tr("Quad-View XYZ"), this );
  XYZQuadViewAction->setCheckable( true );
  XYZQuadViewAction->setChecked( true );

  group->addAction( XYZQuadViewAction );

  m_ViewActions.push_back( XYZQuadViewAction );

  QObject::connect( XYZQuadViewAction, SIGNAL( triggered() ),
    this, SLOT( QuadviewXYZ() ) );

  QAction* XYTQuadViewAction = new QAction( tr("Quad-View XYT"), this );
  XYTQuadViewAction->setCheckable( true );
  XYTQuadViewAction->setChecked( true );

  group->addAction( XYTQuadViewAction );

  m_ViewActions.push_back( XYTQuadViewAction );

  QObject::connect( XYTQuadViewAction, SIGNAL( triggered() ),
    this, SLOT( QuadviewXYT() ) );

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

  // now FullScreen actions related to the XYT view
  QAction* FullScreenXTAction = new QAction( tr( "Full-Screen XT" ), this );
  FullScreenXTAction->setCheckable( true );

  group->addAction( FullScreenXTAction );

  m_ViewActions.push_back( FullScreenXTAction );

  QObject::connect( FullScreenXTAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewXT() ) );

  QAction* FullScreenYTAction = new QAction( tr( "Full-Screen YT" ), this );
  FullScreenYTAction->setCheckable( true );

  group->addAction( FullScreenYTAction );

  m_ViewActions.push_back( FullScreenYTAction );

  QObject::connect( FullScreenYTAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewYT() ) );

  QAction* FullScreenXYTAction = new QAction( tr( "Full-Screen XYT" ), this );
  FullScreenXYTAction->setCheckable( true );

  group->addAction( FullScreenXYTAction );

  m_ViewActions.push_back( FullScreenXYTAction );

  QObject::connect( FullScreenXYTAction, SIGNAL( triggered() ),
    this, SLOT( FullScreenViewXYT() ) );

  QAction* separator = new QAction( this );
  separator->setSeparator( true );

  m_ViewActions.push_back( separator );

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

//-------------------------------------------------------------------------
/**
 *
 */
void QGoTabImageView4D::CreateVisuDockWidget()
{
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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 * \brief
 */
void QGoTabImageView4D::CreateManualSegmentationdockWidget()
{
  m_ManualSegmentationDockWidget = new QGoManualSegmentationDockWidget( this );

  QObject::connect( m_ManualSegmentationDockWidget, SIGNAL( ValidatePressed() ),
    this, SLOT( ValidateContour() ) );

  QObject::connect( m_ManualSegmentationDockWidget,
      SIGNAL( ActivateManualSegmentationToggled( bool ) ),
    this, SLOT( ActivateManualSegmentationEditor( bool ) ) );

  QObject::connect( m_ManualSegmentationDockWidget,
    SIGNAL( ContourRepresentationPropertiesChanged() ),
    this, SLOT( ChangeContourRepresentationProperty() ) );

  this->m_SegmentationActions.push_back(
    m_ManualSegmentationDockWidget->toggleViewAction() );
}
//-------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView4D::~QGoTabImageView4D( )
{
  if( m_XYZImageView )
    {
    delete m_XYZImageView;
    m_XYZImageView = 0;
    }
  if( m_XYTImageView )
    {
    delete m_XYTImageView;
    m_XYTImageView = 0;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::setupUi( QWidget* iParent )
{
  if(iParent->objectName().isEmpty())
    {
    iParent->resize(800, 800);
    }

  QList< int > list_size;
  list_size.push_back( 10 );
  list_size.push_back( 10 );

  m_XYZImageView = new QGoImageView3D;
  m_XYZImageView->SetBackgroundColor( m_BackgroundColor );

  m_XYTImageView = new QGoImageView3D;
  m_XYTImageView->SetBackgroundColor( m_BackgroundColor );

  m_Splitter = new QSplitter( Qt::Horizontal, iParent );
  m_Splitter->addWidget( m_XYZImageView );
  m_Splitter->addWidget( m_XYTImageView );
  m_Splitter->setSizes( list_size );
  m_Splitter->resize( 800, 800 );

  retranslateUi(iParent);

  QMetaObject::connectSlotsByName(iParent);
} // setupUi
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::retranslateUi(QWidget *iParent)
{
  iParent->setWindowTitle( tr( "QGoTabImageView4D" ) );
  Q_UNUSED(iParent);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoFigure::TabDimensionType
QGoTabImageView4D::
GetTabDimensionType( ) const
{
  return GoFigure::FOUR_D;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoTabImageView4D::
SetMegaCaptureFile(
  const GoFigureFileInfoHelperMultiIndexContainer& iContainer,
  const GoFigure::FileType& iFileType,
  const std::string& iHeader )
{
  m_FileType = iFileType;
  m_FileList = iContainer;

  m_FirstUpdate = true;

  m_Reader1->SetInput( m_FileList );
  m_Reader1->SetMegaCaptureHeader( iHeader );
  m_Reader1->SetFileType( m_FileType );
  m_Reader1->SetTimeBased( true );
  m_Reader1->SetTimePoint( 0 );
  m_Reader1->SetChannel( 0 );
  m_Reader1->Update();

  unsigned int min_z = m_Reader1->GetMinZSlice();
  unsigned int max_z = m_Reader1->GetMaxZSlice();

  unsigned int zslice = ( min_z + max_z ) / 2;

  m_Reader2->SetInput( m_FileList );
  m_Reader2->SetMegaCaptureHeader( iHeader );
  m_Reader2->SetFileType( m_FileType );
  m_Reader2->SetTimeBased( false );
  m_Reader2->SetZSlice( zslice );
  m_Reader2->SetChannel( 0 );

  unsigned int min_ch = m_Reader1->GetMinChannel();
  unsigned int max_ch = m_Reader1->GetMaxChannel();

  unsigned int NumberOfChannels = max_ch - min_ch + 1;

  vtkImageData* temp = m_Reader1->GetOutput();

  int extent[6];
  temp->GetExtent( extent );

  m_VisuDockWidget->SetXMinimumAndMaximum( extent[0], extent[1] );
  m_VisuDockWidget->SetXSlice( ( extent[0] + extent[1] ) / 2 );

  m_VisuDockWidget->SetYMinimumAndMaximum( extent[2], extent[3] );
  m_VisuDockWidget->SetYSlice( ( extent[2] + extent[3] ) / 2 );

  m_VisuDockWidget->SetZMinimumAndMaximum( extent[4], extent[5] );
  m_VisuDockWidget->SetZSlice( zslice );

  unsigned int min_t = m_Reader1->GetMinTimePoint();
  unsigned int max_t = m_Reader1->GetMaxTimePoint();

  m_VisuDockWidget->SetTMinimumAndMaximum( min_t, max_t );
  m_VisuDockWidget->SetTSlice( 0 );

  m_VisuDockWidget->SetNumberOfChannels( NumberOfChannels );

  if( NumberOfChannels > 1 )
    {
    m_VisuDockWidget->SetChannel( 0 );
    m_XYZInternalImages.resize( NumberOfChannels, NULL );
    m_XYTInternalImages.resize( NumberOfChannels, NULL );

    for( unsigned int i = 1; i < NumberOfChannels; i++ )
      {
      m_VisuDockWidget->SetChannel( i );
      }
    }

  SetTimePoint( 0 );
  SetZSlice( zslice );
  Update();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetTimePoint( const int& iTimePoint )
{
  if( iTimePoint == m_TimePoint )
    {
    return;
    }

  if( !m_FileList.empty() )
    {
    unsigned int t = static_cast< unsigned int >( iTimePoint );
    if( ( t < m_Reader1->GetMinTimePoint() ) ||
        ( t > m_Reader1->GetMaxTimePoint() ) )
      {
      return;
      }
    else
      {
      m_TimePoint = iTimePoint;
      m_Reader1->SetChannel( 0 );
      m_Reader1->SetTimePoint( m_TimePoint );

      unsigned int min_ch = m_Reader1->GetMinChannel();
      unsigned int max_ch = m_Reader1->GetMaxChannel();

      int NumberOfChannels = max_ch - min_ch + 1;

      if( NumberOfChannels > 1 )
        {
        vtkSmartPointer< vtkImageAppendComponents > append_filter =
          vtkSmartPointer< vtkImageAppendComponents >::New();

        for( int i = 0; i < NumberOfChannels; i++ )
          {
          m_Reader1->SetChannel( i );
          m_Reader1->Update();

          if( !m_XYZInternalImages[i] )
            {
            m_XYZInternalImages[i] = vtkSmartPointer< vtkImageData >::New();
            }
          m_XYZInternalImages[i]->ShallowCopy( m_Reader1->GetOutput() );
          append_filter->AddInput( m_XYZInternalImages[i] );
          }
        // This is really stupid!!!
        if( NumberOfChannels < 3 )
          {
          for( int i = NumberOfChannels; i < 3; i++ )
            {
            append_filter->AddInput( m_XYZInternalImages[0] );
            }
          }
        append_filter->Update();

        m_XYZImage->ShallowCopy( append_filter->GetOutput() );
        }
      else
        {
        m_Reader1->Update();

        m_XYZImage->ShallowCopy( m_Reader1->GetOutput() );
        }

      if( !m_FirstUpdate )
        {
        Update();
        }
      emit TimePointChanged( m_TimePoint );
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetZSlice( const int& iZSlice )
{
  if( iZSlice == m_ZSlice )
    {
    return;
    }

  if( !m_FileList.empty() )
    {
    unsigned int z = static_cast< unsigned int >( iZSlice );
    if( ( z < m_Reader2->GetMinZSlice() ) ||
        ( z > m_Reader2->GetMaxZSlice() ) )
      {
      return;
      }
    else
      {
      m_ZSlice = iZSlice;
      m_Reader2->SetChannel( 0 );
      m_Reader2->SetZSlice( m_ZSlice );

      unsigned int min_ch = m_Reader2->GetMinChannel();
      unsigned int max_ch = m_Reader2->GetMaxChannel();

      int NumberOfChannels = max_ch - min_ch + 1;

      if( NumberOfChannels > 1 )
        {
        vtkSmartPointer< vtkImageAppendComponents > append_filter =
          vtkSmartPointer< vtkImageAppendComponents >::New();

        for( int i = 0; i < NumberOfChannels; i++ )
          {
          m_Reader2->SetChannel( i );
          m_Reader2->Update();

          if( !m_XYTInternalImages[i] )
            {
            m_XYTInternalImages[i] = vtkSmartPointer< vtkImageData >::New();
            }
          m_XYTInternalImages[i]->ShallowCopy( m_Reader2->GetOutput() );
          append_filter->AddInput( m_XYTInternalImages[i] );
          }
        // This is really stupid!!!
        if( NumberOfChannels < 3 )
          {
          for( int i = NumberOfChannels; i < 3; i++ )
            {
            append_filter->AddInput( m_XYTInternalImages[0] );
            }
          }
        append_filter->Update();

        m_XYTImage->ShallowCopy( append_filter->GetOutput() );
        }
      else
        {
        m_Reader2->Update();

        m_XYTImage->ShallowCopy( m_Reader2->GetOutput() );
        }

      if( !m_FirstUpdate )
        {
        Update();
        }
      emit ZSliceChanged( m_ZSlice );
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::Update()
{
  m_XYZImageView->SetImage( m_XYZImage );
  m_XYZImageView->Update();

  m_XYTImageView->SetImage( m_XYTImage );
  m_XYTImageView->Update();

  m_FirstUpdate = false;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::ChangeLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->DeepCopy( QGoLUTDialog::GetLookupTable( this,
    tr( "Choose one look-up table") ) );
  m_XYZImageView->SetLookupTable( lut );
  m_XYTImageView->SetLookupTable( lut );
  lut->Delete();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::ShowScalarBar( const bool& iShow )
{
  m_XYZImageView->ShowScalarBar( iShow );
  m_XYTImageView->ShowScalarBar( iShow );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::
SnapshotViewXY( const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  return m_XYZImageView->SnapshotViewXY( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::
SnapshotView2( const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  return m_XYZImageView->SnapshotView2( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::
SnapshotView3( const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  return m_XYZImageView->SnapshotView3( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::
SnapshotViewXYZ( const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  return m_XYZImageView->SnapshotViewXYZ( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::
SnapshotViewXT( const GoFigure::FileType& iType,
  const QString& iBaseName = QString( "snapshot" ) )
{
  return m_XYTImageView->SnapshotView2( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::
SnapshotViewYT( const GoFigure::FileType& iType,
  const QString& iBaseName = QString( "snapshot" ) )
{
  return m_XYTImageView->SnapshotView3( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString
QGoTabImageView4D::SnapshotViewXYT( const GoFigure::FileType& iType,
  const QString& iBaseName = QString( "snapshot" ) )
{
  return m_XYTImageView->SnapshotViewXYZ( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetSliceViewXY( const int& iS )
{
  m_XYZImageView->SetSliceViewXY( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetSliceViewXZ( const int& iS )
{
  m_XYZImageView->SetSliceViewXZ( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetSliceViewYZ( const int& iS )
{
  m_XYZImageView->SetSliceViewYZ( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetSliceViewXT( const int& iS )
{
  m_XYTImageView->SetSliceViewXZ( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetSliceViewYT( const int& iS )
{
  m_XYTImageView->SetSliceViewYZ( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetFullScreenView( const int& iS )
{
  if( iS == 0 )
    {
    m_XYZImageView->show();
    m_XYZImageView->SetFullScreenView( iS );

    m_XYTImageView->show();
    m_XYTImageView->SetFullScreenView( iS );
    }
  else
    {
    if( iS < 4 )
      {
      m_XYZImageView->show();
      m_XYZImageView->SetFullScreenView( iS - 1 );

      m_XYTImageView->hide( );
      }
    else
      {
      m_XYZImageView->show();
      m_XYZImageView->SetFullScreenView( iS - 4 );

      m_XYTImageView->hide( );
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::Octview()
{
  m_XYZImageView->show();
  m_XYZImageView->Quadview();

  m_XYTImageView->show();
  m_XYTImageView->Quadview();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::QuadviewXYZ()
{
  m_XYZImageView->show();
  m_XYZImageView->Quadview();

  m_XYTImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::QuadviewXYT()
{
  m_XYZImageView->hide();

  m_XYTImageView->show();
  m_XYTImageView->Quadview();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXY()
{
  m_XYZImageView->show();
  m_XYZImageView->FullScreenViewXY();

  m_XYTImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXZ()
{
  m_XYZImageView->show();
  m_XYZImageView->FullScreenViewXZ();

  m_XYTImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewYZ()
{
  m_XYZImageView->show();
  m_XYZImageView->FullScreenViewYZ();

  m_XYTImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXYZ()
{
  m_XYZImageView->show();
  m_XYZImageView->FullScreenViewXYZ();

  m_XYTImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXT()
{
  m_XYZImageView->hide();

  m_XYTImageView->show();
  m_XYTImageView->FullScreenViewXZ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewYT()
{
  m_XYZImageView->hide();

  m_XYTImageView->show();
  m_XYTImageView->FullScreenViewYZ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXYT()
{
  m_XYZImageView->hide();

  m_XYTImageView->show();
  m_XYTImageView->FullScreenViewXYZ();
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void QGoTabImageView4D::GetBackgroundColorFromImageViewer( )
{
  double r, g, b;
  m_XYZImageView->GetBackgroundColor( r, g, b );
  m_BackgroundColor.setRgbF( r, g, b );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetBackgroundColorToImageViewer( )
{
  m_XYZImageView->SetBackgroundColor( m_BackgroundColor );
  m_XYTImageView->SetBackgroundColor( m_BackgroundColor );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction* > QGoTabImageView4D::ViewActions()
{
  return m_ViewActions;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< QDockWidget* > QGoTabImageView4D::DockWidget()
{
  std::list< QDockWidget* > oList;
  oList.push_back( m_VisuDockWidget );
  oList.push_back( m_ManualSegmentationDockWidget );
  return oList;
}
//--------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoTabImageView4D::resizeEvent( QResizeEvent* iEvent )
{
  QWidget::resizeEvent( iEvent );
  m_Splitter->resize( iEvent->size() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * \param iChecked
 */
void QGoTabImageView4D::ShowAllChannels( bool iChecked )
{
  if( iChecked )
    {
    vtkSmartPointer< vtkImageAppendComponents > append_filter1 =
      vtkSmartPointer< vtkImageAppendComponents >::New();
    vtkSmartPointer< vtkImageAppendComponents > append_filter2 =
      vtkSmartPointer< vtkImageAppendComponents >::New();

    for( unsigned int i = 0; i < m_XYZInternalImages.size(); i++ )
      {
      append_filter1->AddInput( m_XYZInternalImages[i] );
      append_filter2->AddInput( m_XYTInternalImages[i] );
      }

    // This is really stupid!!!
    if( m_XYZInternalImages.size() < 3 )
      {
      for( int i = m_XYZInternalImages.size(); i < 3; i++ )
        {
        append_filter1->AddInput( m_XYZInternalImages[0] );
        append_filter2->AddInput( m_XYTInternalImages[0] );
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
    int ch = this->m_VisuDockWidget->GetCurrentChannel();
    if( ch != -1 )
      {
      m_XYZImage->ShallowCopy( m_XYZInternalImages[ch] );
      m_XYTImage->ShallowCopy( m_XYTInternalImages[ch] );
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
void QGoTabImageView4D::
ShowOneChannel( int iChannel )
{
  if( ( iChannel != -1 ) && ( !m_XYZInternalImages.empty() ) )
    {
    m_XYZImage->ShallowCopy( m_XYZInternalImages[iChannel] );
    m_XYTImage->ShallowCopy( m_XYTInternalImages[iChannel] );
    Update();
    }
}
//-------------------------------------------------------------------------

