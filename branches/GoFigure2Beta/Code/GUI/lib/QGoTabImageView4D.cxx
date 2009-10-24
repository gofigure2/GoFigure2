#include "QGoTabImageView4D.h"

#include "QGoImageView3D.h"
#include "QGoLUTDialog.h"

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
  m_BackgroundColor( Qt::black )
{
  m_Reader1 = itk::MultiFileReader::New();
  m_Reader2 = itk::MultiFileReader::New();

  m_XYZFileList = new FileListType;
  m_XYTFileList = new FileListType;
//   m_ThreadedReader1 = new QGoThreadedMultiFileReader( this );
//   m_ThreadedReader2 = new QGoThreadedMultiFileReader( this );

  setupUi( this );

//   QObject::connect( m_ThreadedReader1, SIGNAL( finished() ),
//     this, SLOT( ReceiveXYZImage() ) );

//   QObject::connect( m_ThreadedReader1, SIGNAL( finished() ),
//     m_ThreadedReader2, SLOT( start() ) );
//   QObject::connect( m_ThreadedReader2, SIGNAL( finished() ),
//     this, SLOT( ReceiveXYTImage() ) );

  CreateAllViewActions();

  m_DockWidget = new QDockWidget( tr( "Slice" ) );
  m_DockWidget->resize( 120, 300 );

  QWidget* temp = new QWidget();
  temp->resize( 100, 150 );

  QGridLayout* gridlayout = new QGridLayout( temp );
  gridlayout->setContentsMargins(3, -1, 3, -1);

  QLabel* SliceX = new QLabel( "X Slice" );
  gridlayout->addWidget( SliceX, 0, 0 );
  QSpinBox* XSliceSpinBox = new QSpinBox();
  gridlayout->addWidget( XSliceSpinBox, 0, 1 );

  QLabel* SliceY = new QLabel( "Y Slice" );
  gridlayout->addWidget( SliceY, 1, 0 );
  QSpinBox* YSliceSpinBox = new QSpinBox( );
  gridlayout->addWidget( YSliceSpinBox, 1, 1 );

  QLabel* SliceZ = new QLabel( "Z Slice" );
  gridlayout->addWidget( SliceZ, 2, 0 );
  QSpinBox* ZSliceSpinBox = new QSpinBox( );
  gridlayout->addWidget( ZSliceSpinBox, 2, 1 );

  QLabel* SliceT = new QLabel( "T Time" );
  gridlayout->addWidget( SliceT, 3, 0 );
  QSpinBox* TSliceSpinBox = new QSpinBox( );
  gridlayout->addWidget( TSliceSpinBox, 3, 1 );

  m_DockWidget->layout()->addWidget( temp );

  ReadSettings();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::CreateAllViewActions()
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
  if( m_XYZImage )
    {
    m_XYZImage->Delete();
    m_XYZImage = 0;
    }
  if( m_XYTImage )
    {
    m_XYTImage->Delete();
    m_XYTImage = 0;
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
GoFigure::TabDimensionType QGoTabImageView4D::GetTabDimensionType( ) const
{
  return GoFigure::FOUR_D;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetMultiFiles( FileListType* iFileList,
  const int& iTimePoint )
{
  m_TimePoint = iTimePoint;

  m_Reader1->SetFileType( itk::MultiFileReader::JPEG );
  m_Reader1->SetTimeBased( true );
  m_Reader1->SetDimensionality( 2 );
  m_Reader1->MultiChannelImagesOn();

  m_Reader2->SetFileType( itk::MultiFileReader::JPEG );
  m_Reader2->SetTimeBased( false );
  m_Reader2->SetDimensionality( 2 );
  m_Reader2->MultiChannelImagesOn();

  m_XYZFileList->resize( iFileList->size() );
  m_XYTFileList->resize( iFileList->size() );

  std::copy( iFileList->begin(), iFileList->end(), m_XYZFileList->begin() );
  std::copy( iFileList->begin(), iFileList->end(), m_XYTFileList->begin() );

  std::sort( m_XYZFileList->begin(), m_XYZFileList->end(),
      GoFigureFileInfoHelperTimeBasedCompare() );

  std::sort( m_XYTFileList->begin(), m_XYTFileList->end(),
      GoFigureFileInfoHelperZCoordBasedCompare() );

  m_Reader1->SetInput( m_XYZFileList );
  m_Reader1->SetTimePoint( m_TimePoint );
  m_Reader1->Update();


//   m_ThreadedReader1->SetInput( iFileList, itk::MultiFileReader::JPEG, true );
//   m_ThreadedReader1->SetTimePoint( m_TimePoint );

//   m_ThreadedReader1->start();

//   m_ThreadedReader2->SetInput( iFileList, itk::MultiFileReader::JPEG, false );
//   m_ThreadedReader2->SetZDepth( m_ZDepth );

//   m_ThreadedReader2->start();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// void QGoTabImageView4D::ReceiveXYZImage()
// {
//   m_XYZImage = m_ThreadedReader1->GetOutput();
//   m_XYZImageView->SetImage( m_XYZImage );
//   m_XYZImageView->Update();
// }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// void QGoTabImageView4D::ReceiveXYTImage()
// {
//   m_XYTImage = m_ThreadedReader2->GetOutput();
//   m_XYTImageView->SetImage( m_XYTImage );
//   m_XYTImageView->Update();
//
//   this->show();
// }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetTimePoint( const int& iTimePoint )
{
  m_TimePoint = iTimePoint;
  m_Reader1->SetTimePoint( m_TimePoint );
//   m_ThreadedReader1->SetTimePoint( m_TimePoint );
//   m_ThreadedReader1->start();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::Update()
{
  m_XYZImage = m_Reader1->GetOutput();
  m_XYZImageView->SetImage( m_XYZImage );
  m_XYZImageView->Update();

  m_ZDepth = m_XYZImageView->GetSliceViewXY();

  m_Reader2->SetInput( m_XYTFileList );
  m_Reader2->SetZDepth( m_ZDepth );
  m_Reader2->Update();

  m_XYTImage = m_Reader2->GetOutput();
  m_XYTImageView->SetImage( m_XYTImage );
  m_XYTImageView->Update();

//  int tslice = m_XYTImageView->GetSliceViewXY();

//   std::cout <<"Z Slice : " <<zslice <<std::endl;
//   std::cout <<"T Slice : " <<tslice <<std::endl;
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
QString QGoTabImageView4D::SnapshotViewXY(
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName )
{
  return m_XYZImageView->SnapshotViewXY( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString QGoTabImageView4D::SnapshotView2(
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName )
{
  return m_XYZImageView->SnapshotView2( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString QGoTabImageView4D::SnapshotView3(
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName )
{
  return m_XYZImageView->SnapshotView3( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString QGoTabImageView4D::SnapshotViewXYZ(
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName )
{
  return m_XYZImageView->SnapshotViewXYZ( iType, iBaseName );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::SetSliceViewXY( const int& iS )
{
  m_XYZImageView->SetSliceViewXY( iS );
  m_XYTImageView->SetSliceViewXY( iS );
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
void QGoTabImageView4D::SetFullScreenView( const int& iS )
{
  m_XYZImageView->SetFullScreenView( iS );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::Octview()
{
  m_XYZImageView->Quadview();
  m_XYTImageView->Quadview();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::QuadviewXYZ()
{
  m_XYZImageView->Quadview();
  m_XYTImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::QuadviewXYT()
{
  m_XYTImageView->Quadview();
  m_XYZImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXY()
{
  m_XYZImageView->FullScreenViewXY();
  m_XYTImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXZ()
{
  m_XYZImageView->FullScreenViewXZ();
  m_XYTImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewYZ()
{
  m_XYZImageView->FullScreenViewYZ();
  m_XYTImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXYZ()
{
  m_XYZImageView->FullScreenViewXYZ();
  m_XYTImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXT()
{
  m_XYTImageView->FullScreenViewXZ();
  m_XYZImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewYT()
{
  m_XYTImageView->FullScreenViewYZ();
  m_XYZImageView->hide();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoTabImageView4D::FullScreenViewXYT()
{
  m_XYTImageView->FullScreenViewXYZ();
  m_XYZImageView->hide();
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
  oList.push_back( m_DockWidget );
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

