#include "QGoTabImageView3DwT.h"

#include "QGoImageView3D.h"
#include "QGoLUTDialog.h"

#include "vtkLookupTable.h"
#include "vtkImageAppendComponents.h"

#include <QLabel>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QColorDialog>

//--------------------------------------------------------------------------
QGoTabImageView3DwT::QGoTabImageView3DwT( QWidget* parent ) :
  QGoTabElementBase( parent ),
  m_LSMReader( 0 ),
  m_Image( 0 ),
  m_BackgroundColor( Qt::black ),
  m_TimePoint( -1 )
{
  setupUi( this );

  m_MultiFileReader = itk::MultiFileReader::New();

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

  CreateAllViewActions();
  ReadSettings();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::CreateAllViewActions()
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

//   QAction* toggleviewaction = m_VisuDockWidget->toggleViewAction();
//   toggleviewaction->setText( tr( "Slide Location" ) );
//   toggleviewaction->setParent( this );
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
QGoTabImageView3DwT::~QGoTabImageView3DwT( )
{
  if( m_Image )
    {
    m_Image->Delete();
    m_Image = 0;
    }
  int NumberOfChannels = m_LSMReader[0]->GetNumberOfChannels();

  if( NumberOfChannels > 1 )
    {
    for( int i = 1; i < NumberOfChannels; i++ )
      {
      m_LSMReader[i]->Delete();
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::setupUi( QWidget* parent )
{
  if(parent->objectName().isEmpty())
    {
    parent->resize(800, 800);
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

  m_LayOut = new QHBoxLayout( parent );
  m_LayOut->addWidget( m_ImageView  );

  retranslateUi(parent);

  QMetaObject::connectSlotsByName(parent);
} // setupUi
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::retranslateUi(QWidget *parent)
{
  parent->setWindowTitle( tr( "QGoTabImageView3DwT" ) );
  Q_UNUSED(parent);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoFigure::TabDimensionType QGoTabImageView3DwT::GetTabDimensionType( ) const
{
  return GoFigure::THREE_D_WITH_T;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::SetLSMReader( vtkLSMReader* iReader,
  const int& iTimePoint )
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
    m_VisuDockWidget->SetXMinimumAndMaximum( 0, dim[0] - 1 );
    m_VisuDockWidget->SetXSlice( ( dim[0] - 1 ) / 2 );

    m_VisuDockWidget->SetYMinimumAndMaximum( 0, dim[1] - 1 );
    m_VisuDockWidget->SetYSlice( ( dim[1] - 1 ) / 2 );

    m_VisuDockWidget->SetZMinimumAndMaximum( 0, dim[2] - 1 );
    m_VisuDockWidget->SetZSlice( ( dim[2] - 1 ) / 2 );

    m_VisuDockWidget->SetTMinimumAndMaximum( 0, dim[3] - 1 );
    m_VisuDockWidget->SetTSlice( iTimePoint );

    int NumberOfChannels = m_LSMReader[0]->GetNumberOfChannels();
    m_VisuDockWidget->SetNumberOfChannels( NumberOfChannels );

    if( NumberOfChannels > 1 )
      {
      m_VisuDockWidget->SetChannel( 0 );

      for( int i = 1; i < NumberOfChannels; i++ )
        {
        m_VisuDockWidget->SetChannel( i );

        m_LSMReader.push_back( vtkLSMReader::New() );
        m_LSMReader.back()->SetFileName( m_LSMReader[0]->GetFileName() );
        m_LSMReader.back()->SetUpdateChannel( i );
        }
      }

    if( m_TimePoint != iTimePoint )
      {
      SetTimePoint( iTimePoint );
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::SetMultiFiles( FileListType& iFileList,
  const FILETYPE& iFileType,
  const int& iTimePoint )
{
  m_TimePoint = iTimePoint;
  m_FileList = iFileList;
  m_MultiFileReader->SetInput( &m_FileList );

  switch( iFileType ) //IsLSM
    {
    case itk::MultiFileReader::LSM:
      {
      m_MultiFileReader->SetDimensionality( 3 );
      m_MultiFileReader->SetFileType( iFileType );
      m_MultiFileReader->SetChannel( 0 );
      break;
      }
    case itk::MultiFileReader::MHA:
      {
      m_MultiFileReader->SetDimensionality( 3 );
      m_MultiFileReader->SetFileType( iFileType );
      break;
      }
    default:
      {
      std::cout <<"MegaCapture files MUST now use QGoTabImageView4D instead!!!"
        <<std::endl;
      return;
      }
    }
//   if( iSerieType == 1 ) //IsMegaCapture must now use the 4D!!!
//     {
//     m_MultiFileReader->SetDimensionality( 2 );
//     m_MultiFileReader->SetFileType( JPEG );
//     }
  m_MultiFileReader->MultiChannelImagesOff();
  m_MultiFileReader->SetTimeBased( true );
  m_MultiFileReader->SetTimePoint( m_TimePoint );
  m_MultiFileReader->Update();

  m_Image = m_MultiFileReader->GetOutput();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::SetTimePoint( const int& iTimePoint )
{
  if( !m_LSMReader.empty() )
    {
    if( ( iTimePoint == m_TimePoint ) ||
        ( iTimePoint >= m_LSMReader[0]->GetNumberOfTimePoints() ) )
      {
      return;
      }
    m_TimePoint = iTimePoint;
    m_LSMReader[0]->SetUpdateTimePoint( m_TimePoint );

    int NumberOfChannels = m_LSMReader[0]->GetNumberOfChannels();

    if( NumberOfChannels > 1 )
      {
      std::vector< vtkImageData* > temp_image( NumberOfChannels );
      temp_image[0] = vtkImageData::New();
      temp_image[0]->ShallowCopy( m_LSMReader[0]->GetOutput() );

      vtkImageAppendComponents* append_filter =
        vtkImageAppendComponents::New();
      append_filter->AddInput( temp_image[0] );

      for( int i = 1; i < NumberOfChannels; i++ )
        {
        m_LSMReader[i]->SetUpdateTimePoint( m_TimePoint );
        m_LSMReader[i]->Update();

        temp_image[i] = vtkImageData::New();
        temp_image[i]->ShallowCopy( m_LSMReader[i]->GetOutput() );
        append_filter->AddInput( temp_image[i] );
        }
      // This is really stupid!!!
      if( NumberOfChannels < 3 )
        {
        for( int i = NumberOfChannels; i < 3; i++ )
          {
          append_filter->AddInput( temp_image[0] );
          }
        }
      append_filter->Update();

      // Do we really need to delete m_Image?
      if( !m_Image )
        {
        m_Image = vtkImageData::New();
        }

      m_Image->ShallowCopy( append_filter->GetOutput() );
      append_filter->Delete();

      for( int i = 0; i < NumberOfChannels; i++ )
        {
        temp_image[i]->Delete();
        }
      }
    else
      {
      m_LSMReader[0]->Update();

      if( !m_Image )
        {
        m_Image = vtkImageData::New();
        }
      m_Image->ShallowCopy( m_LSMReader[0]->GetOutput() );
      }
    Update();

    emit TimePointChanged( m_TimePoint );
    }
  else
    {
    if( !m_FileList.empty() )
      {
      }
    else
      {
      // no lsm reader, no file list. did you really provide any input?
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::Update()
{
  m_ImageView->SetImage( m_Image );
  m_ImageView->Update();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::ChangeLookupTable()
{
  vtkLookupTable* lut = vtkLookupTable::New();
  lut->DeepCopy( QGoLUTDialog::GetLookupTable( this,
    tr( "Choose one look-up table") ) );
  m_ImageView->SetLookupTable( lut );
  lut->Delete();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::ShowScalarBar( const bool& iShow )
{
  m_ImageView->ShowScalarBar( iShow );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString QGoTabImageView3DwT::SnapshotViewXY(
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotViewXY( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString QGoTabImageView3DwT::SnapshotView2(
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotView2( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString QGoTabImageView3DwT::SnapshotView3(
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotView3( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString QGoTabImageView3DwT::SnapshotViewXYZ(
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName )
{
  return m_ImageView->SnapshotViewXYZ( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::SetSliceViewXY( const int& iS )
{
  m_ImageView->SetSliceViewXY( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::SetSliceViewXZ( const int& iS )
{
  m_ImageView->SetSliceViewXZ( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::SetSliceViewYZ( const int& iS )
{
  m_ImageView->SetSliceViewYZ( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::SetFullScreenView( const int& iS )
{
  m_ImageView->SetFullScreenView( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::Quadview()
{
  m_ImageView->Quadview();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::FullScreenViewXY()
{
  m_ImageView->FullScreenViewXY();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::FullScreenViewXZ()
{
  m_ImageView->FullScreenViewXZ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::FullScreenViewYZ()
{
  m_ImageView->FullScreenViewYZ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::FullScreenViewXYZ()
{
  m_ImageView->FullScreenViewXYZ();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::GetBackgroundColorFromImageViewer( )
{
  double r, g, b;
  m_ImageView->GetBackgroundColor( r, g, b );
  m_BackgroundColor.setRgbF( r, g, b );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::SetBackgroundColorToImageViewer( )
{
  m_ImageView->SetBackgroundColor( m_BackgroundColor );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector< QAction* > QGoTabImageView3DwT::ViewActions()
{
  return m_ViewActions;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< QDockWidget* > QGoTabImageView3DwT::DockWidget()
{
  std::list< QDockWidget* > oList;
  oList.push_back( m_VisuDockWidget );
  return oList;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::ChangeBackgroundColor()
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
    }
}

//--------------------------------------------------------------------------
void QGoTabImageView3DwT::ShowAllChannels( bool iChecked )
{
  if( iChecked )
    {
    int NumberOfChannels = m_LSMReader[0]->GetNumberOfChannels();

    std::vector< vtkImageData* > temp_image( NumberOfChannels );
    temp_image[0] = vtkImageData::New();
    temp_image[0]->ShallowCopy( m_LSMReader[0]->GetOutput() );

    vtkImageAppendComponents* append_filter =
      vtkImageAppendComponents::New();
    append_filter->AddInput( temp_image[0] );

    for( int i = 1; i < NumberOfChannels; i++ )
      {
      m_LSMReader[i]->SetUpdateTimePoint( m_TimePoint );
      m_LSMReader[i]->Update();

      temp_image[i] = vtkImageData::New();
      temp_image[i]->ShallowCopy( m_LSMReader[i]->GetOutput() );
      append_filter->AddInput( temp_image[i] );
      }
    // This is really stupid!!!
    if( NumberOfChannels < 3 )
      {
      for( int i = NumberOfChannels; i < 3; i++ )
        {
        append_filter->AddInput( temp_image[0] );
        }
      }
    append_filter->Update();

    m_Image->ShallowCopy( append_filter->GetOutput() );
    Update();

    append_filter->Delete();

    for( int i = 0; i < NumberOfChannels; i++ )
      {
      temp_image[i]->Delete();
      }
    }
  else
    {
    int ch = this->m_VisuDockWidget->GetCurrentChannel();
    if( ch != -1 )
      {
      m_Image->ShallowCopy( m_LSMReader[ch]->GetOutput() );
      Update();
      }
    }
}

void QGoTabImageView3DwT::
ShowOneChannel( int iChannel )
{
  if( !m_LSMReader.empty() && m_Image )
    {
    if( iChannel != -1 )
      {
      m_Image->ShallowCopy( m_LSMReader[iChannel]->GetOutput() );
      Update();
      }
    }
}
