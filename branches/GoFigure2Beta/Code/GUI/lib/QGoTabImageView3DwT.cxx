#include "QGoTabImageView3DwT.h"

#include "QGoImageView3D.h"
#include "QGoLUTDialog.h"

#include "vtkLookupTable.h"
#include "vtkImageAppendComponents.h"

#include <QLabel>
#include <QDockWidget>
#include <QSpinBox>
#include <QVBoxLayout>

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

  m_DockWidget = new QDockWidget( tr( "Slice" ) );
  m_DockWidget->resize( 120, 300 );

  QWidget* temp = new QWidget();
  temp->resize( 100, 150 );

  QGridLayout* layout = new QGridLayout( temp );
  layout->setContentsMargins(3, -1, 3, -1);

  QLabel* SliceX = new QLabel( "X Slice" );
  layout->addWidget( SliceX, 0, 0 );
  m_XSliceSpinBox = new QSpinBox();
  layout->addWidget( m_XSliceSpinBox, 0, 1 );

  QObject::connect( m_XSliceSpinBox, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSliceViewYZ( int ) ) );

  QLabel* SliceY = new QLabel( "Y Slice" );
  layout->addWidget( SliceY, 1, 0 );
  m_YSliceSpinBox = new QSpinBox( );
  layout->addWidget( m_YSliceSpinBox, 1, 1 );

  QObject::connect( m_YSliceSpinBox, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSliceViewXZ( int ) ) );

  QLabel* SliceZ = new QLabel( "Z Slice" );
  layout->addWidget( SliceZ, 2, 0 );
  m_ZSliceSpinBox = new QSpinBox( );
  layout->addWidget( m_ZSliceSpinBox, 2, 1 );

  QObject::connect( m_ZSliceSpinBox, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSliceViewXY( int ) ) );

  QLabel* SliceT = new QLabel( "T Time" );
  layout->addWidget( SliceT, 3, 0 );
  m_TSliceSpinBox = new QSpinBox( );
  layout->addWidget( m_TSliceSpinBox, 3, 1 );

  QObject::connect( m_TSliceSpinBox, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetTimePoint( int ) ) );

  m_DockWidget->layout()->addWidget( temp );

  ReadSettings();
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
    m_XSliceSpinBox->setMinimum( 0 );
    m_XSliceSpinBox->setMaximum( dim[0] - 1 );

    m_YSliceSpinBox->setMinimum( 0 );
    m_YSliceSpinBox->setMaximum( dim[1] - 1 );

    m_ZSliceSpinBox->setMinimum( 0 );
    m_ZSliceSpinBox->setMaximum( dim[2] - 1 );

    m_TSliceSpinBox->setMinimum( 0 );
    m_TSliceSpinBox->setMaximum( dim[3] - 1 );
    m_TSliceSpinBox->setValue( iTimePoint );

    int NumberOfChannels = m_LSMReader[0]->GetNumberOfChannels();

    if( NumberOfChannels > 1 )
      {
      for( int i = 1; i < NumberOfChannels; i++ )
        {
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
  const int& iSerieType,
  const int& iTimePoint )
{
  m_TimePoint = iTimePoint;
  m_FileList = iFileList;
  m_MultiFileReader->SetInput( &m_FileList );

  if( iSerieType == 0 ) //IsLSM
    {
    m_MultiFileReader->SetDimensionality( 3 );
    m_MultiFileReader->SetFileType( LSM );
    m_MultiFileReader->SetChannel( 0 );
    }
//   if( iSerieType == 1 ) //IsMegaCapture must now use the 4D!!!
//     {
//     m_MultiFileReader->SetDimensionality( 2 );
//     m_MultiFileReader->SetFileType( JPEG );
//     }
  m_MultiFileReader->SetMultiChannelImagesON();
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

      vtkImageAppendComponents* append_filter = vtkImageAppendComponents::New();
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
  oList.push_back( m_DockWidget );
  return oList;
}
//--------------------------------------------------------------------------
