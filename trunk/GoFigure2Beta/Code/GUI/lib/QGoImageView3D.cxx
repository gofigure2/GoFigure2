#include "QGoImageView3D.h"

#include "vtkImageData.h"
#include "vtkViewImage2D.h"
#include "vtkViewImage3D.h"
#include "vtkViewImage2DCollection.h"

#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"
#include "vtkTextProperty.h"
#include "vtkProperty.h"
#include "vtkImageClip.h"
#include "vtkImagePermute.h"
#include "vtkImageResample.h"
#include "vtkWindowToImageFilter.h"
#include "vtkBMPWriter.h"
#include "vtkPostScriptWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkPNGWriter.h"
#include "vtkTIFFWriter.h"

#include "vtkEventQtSlotConnect.h"
#include "QSplitterChild.h"
#include "QVTKWidget.h"

#include <QResizeEvent>
#include <QSettings>

#include "vtkViewImage2DCommand.h"
#include "vtkViewImage2DCollectionCommand.h"


//-------------------------------------------------------------------------
/**
 *
 * @param iParent
 */
QGoImageView3D::
QGoImageView3D( QWidget* iParent ) :
  QGoImageView( iParent ),
  IsFullScreen( 0 ),
  m_FirstRender( true )
{
  VtkEventQtConnector = vtkEventQtSlotConnect::New();

  m_HighlightedContourProperty = vtkProperty::New();
  m_HighlightedContourProperty->SetColor( 1., 0., 0. );
  m_HighlightedContourProperty->SetLineWidth( 3. );

  setupUi( this );

  QObject::connect( this->SliderXY, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSliceViewXY( int ) ) );
  QObject::connect( this->SliderXZ, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSliceViewXZ( int ) ) );
  QObject::connect( this->SliderYZ, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSliceViewYZ( int ) ) );

  QObject::connect( this->HtSplitter, SIGNAL( splitterMoved( int, int ) ),
    this->HbSplitter, SLOT( moveSplitter( int, int ) ) );
  QObject::connect(this->HbSplitter, SIGNAL( splitterMoved( int, int ) ),
    this->HtSplitter, SLOT( moveSplitter( int, int ) ) );

  View3D = vtkViewImage3D::New();

  vtkViewImage2D* View1 = vtkViewImage2D::New();
  SetupViewGivenQVTKWidget( View1, this->QvtkWidget_XY );

  this->m_Pool->AddItem( View1 );
  View1->Delete();

  vtkViewImage2D* View2 = vtkViewImage2D::New();
  SetupViewGivenQVTKWidget( View2, this->QvtkWidget_XZ );

  this->m_Pool->AddItem( View2 );
  View2->Delete();

  vtkViewImage2D* View3 = vtkViewImage2D::New();
  SetupViewGivenQVTKWidget( View3, this->QvtkWidget_YZ );

  this->m_Pool->AddItem( View3 );
  View3->Delete();

  vtkRenderWindow* renwin4 = this->QvtkWidget_XYZ->GetRenderWindow( );
  this->View3D->SetRenderWindow( renwin4 );
  this->View3D->SetupInteractor( this->QvtkWidget_XYZ->GetInteractor() );
  this->m_Pool->SetExtraRenderWindow( renwin4 );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
QGoImageView3D::~QGoImageView3D()
{
  delete HtSplitter;
  delete HbSplitter;

  // note m_Pool is supposed to be deleted in QGoImageView, but due to a bug
  // it has to be deleted in this order...
  if( m_Pool )
    {
    m_Pool->Delete();
    m_Pool = 0;
    }
  View3D->Delete();
  VtkEventQtConnector->Delete();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param iParent
 */
void QGoImageView3D::setupUi( QWidget* iParent )
{
  if(iParent->objectName().isEmpty())
    {
    iParent->resize(800, 800);
    }

  QList< int > list_size;
  list_size.push_back( 10 );
  list_size.push_back( 10 );

  this->VSplitter  = new QSplitter( Qt::Vertical, iParent );

  this->HtSplitter = new QSplitterChild( this->VSplitter );
  this->HbSplitter = new QSplitterChild( this->VSplitter );

  this->VSplitter->addWidget(this->HtSplitter);
  this->VSplitter->addWidget(this->HbSplitter);
  this->VSplitter->setSizes( list_size );
  this->VSplitter->resize( 800, 800 );

  this->QvtkWidget_XY = new QVTKWidget;
  this->SliderXY = new QSlider( Qt::Vertical );
  this->LayOut1 = new QHBoxLayout;
  this->LayOut1->addWidget( this->QvtkWidget_XY );
  this->LayOut1->addWidget( this->SliderXY );
  this->LayOutWidget1 = new QWidget;
  this->LayOutWidget1->setLayout( this->LayOut1 );
  this->HtSplitter->addWidget( this->LayOutWidget1 );

  this->QvtkWidget_XZ = new QVTKWidget;
  this->SliderXZ = new QSlider( Qt::Vertical );
  this->LayOut2 = new QHBoxLayout;
  this->LayOut2->addWidget( this->QvtkWidget_XZ );
  this->LayOut2->addWidget( this->SliderXZ );
  this->LayOutWidget2 = new QWidget;
  this->LayOutWidget2->setLayout( this->LayOut2 );
  this->HbSplitter->addWidget( this->LayOutWidget2 );
//   this->HtSplitter->setSizes( list_size );

  this->QvtkWidget_YZ = new QVTKWidget;
  this->SliderYZ = new QSlider( Qt::Vertical );
  this->LayOut3 = new QHBoxLayout;
  this->LayOut3->addWidget( this->QvtkWidget_YZ );
  this->LayOut3->addWidget( this->SliderYZ );
  this->LayOutWidget3 = new QWidget;
  this->LayOutWidget3->setLayout( this->LayOut3 );
  this->HtSplitter->addWidget( this->LayOutWidget3 );

  this->QvtkWidget_XYZ = new QVTKWidget;
  this->Spacer = new QSpacerItem( 27, 10,
    QSizePolicy::Minimum, QSizePolicy::Minimum);
  this->LayOut4 = new QHBoxLayout;
  this->LayOut4->addWidget( this->QvtkWidget_XYZ );
  this->LayOut4->addItem( this->Spacer );
  this->LayOutWidget4 = new QWidget;
  this->LayOutWidget4->setLayout( this->LayOut4 );
  this->HbSplitter->addWidget( this->LayOutWidget4 );

  retranslateUi(iParent);

  QMetaObject::connectSlotsByName(iParent);
} // setupUi
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param iParent
 */
void QGoImageView3D::retranslateUi(QWidget *iParent)
{
  iParent->setWindowTitle( tr("QGoImageView3D") );
  Q_UNUSED(iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void QGoImageView3D::Update()
{
  vtkViewImage2D* View1 = this->m_Pool->GetItem( 0 );
  View1->SetInput( this->m_Image );
  View1->SetViewOrientation( vtkViewImage2D::VIEW_ORIENTATION_AXIAL );
  View1->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );

  this->View3D->Add2DPhantom( 0,
      View1->GetImageActor(), View1->GetSlicePlane() );

  int *range = View1->GetSliceRange();

  this->SliderXY->setMinimum( range[0] );
  this->SliderXY->setMaximum( range[1] );

  vtkViewImage2D* View2 = this->m_Pool->GetItem( 1 );
  View2->SetInput( this->m_Image );
  View2->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
  View2->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_CORONAL);

  this->View3D->Add2DPhantom( 1,
    View2->GetImageActor(), View2->GetSlicePlane() );

  range = View2->GetSliceRange();

  this->SliderXZ->setMinimum( range[0] );
  this->SliderXZ->setMaximum( range[1] );

  vtkViewImage2D* View3 = this->m_Pool->GetItem( 2 );
  View3->SetInput( this->m_Image );
  View3->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
  View3->SetViewOrientation( vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL );

  this->View3D->Add2DPhantom(
    2, View3->GetImageActor(), View3->GetSlicePlane() );

  range = View3->GetSliceRange();
  this->SliderYZ->setMinimum( range[0] );
  this->SliderYZ->setMaximum( range[1] );

  this->View3D->SetInput( this->m_Image );
  this->View3D->SetVolumeRenderingOff();
  this->View3D->SetTriPlanarRenderingOn();
  this->View3D->SetShowScalarBar( false );
  this->View3D->ResetCamera();

  this->m_Pool->SyncSetBackground( this->m_Pool->GetItem(0)->GetBackground() );
  this->m_Pool->SyncSetShowAnnotations( true );

  for( int i = 0; i < 3; i++ )
    {
    this->m_Pool->GetItem(i)->GetTextProperty()->SetFontFamilyToArial();
    this->m_Pool->GetItem(i)->GetTextProperty()->SetFontSize( 14 );
    }

  this->m_Pool->SyncSetShowScalarBar( false );
  this->m_Pool->SyncRender();
  this->m_Pool->SyncReset();
  this->m_Pool->InitializeAllObservers();


  if( m_FirstRender )
    {
    this->m_Pool->Initialize();

    this->SliderXY->setValue( (this->SliderXY->minimum()+this->SliderXY->maximum())/2 );
    this->SliderXZ->setValue( (this->SliderXZ->minimum()+this->SliderXZ->maximum())/2 );
    this->SliderYZ->setValue( (this->SliderYZ->minimum()+this->SliderYZ->maximum())/2 );
    m_FirstRender = false;
    }

  SetupVTKtoQtConnections();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void QGoImageView3D::SetupVTKtoQtConnections()
{
  vtkViewImage2D* View1 = this->m_Pool->GetItem( 0 );
  vtkViewImage2D* View2 = this->m_Pool->GetItem( 1 );
  vtkViewImage2D* View3 = this->m_Pool->GetItem( 2 );

  // Event connection between vtk and qt
  // when RequestedPositionEvent occurs in the XY View (double-click),
  // SliderXZ and SliderYZ move.
  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View1->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSliderXZ() ) );

  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View1->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSliderYZ() ) );

  // Event connection between vtk and qt
  // when RequestedPositionEvent occurs in the XY View (double-click),
  // SliderXZ and SliderYZ move.
  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View2->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSliderXY() ) );

  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View2->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSliderYZ() ) );

  // Event connection between vtk and qt
  // when SliceMoveEvent occurs in the XY View, SliderXY moves.
  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View3->GetInteractorStyle() ),
    vtkViewImage2DCommand::EndSliceMoveEvent,
    this, SLOT( MoveSliderYZ() ) );

  // Event connection between vtk and qt
  // when RequestedPositionEvent occurs in the XY View (double-click),
  // SliderXZ and SliderYZ move.
  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View3->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSliderXY() ) );

  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View3->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSliderXZ() ) );

   // Event connection between vtk and qt
  // when SliceMoveEvent occurs in the XY View, SliderXY moves.
  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View2->GetInteractorStyle() ),
    vtkViewImage2DCommand::EndSliceMoveEvent,
    this, SLOT( MoveSliderXZ() ) );

  // Event connection between vtk and qt
  // when SliceMoveEvent occurs in the XY View, SliderXY moves.
  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View1->GetInteractorStyle() ),
    vtkViewImage2DCommand::EndSliceMoveEvent,
    this, SLOT( MoveSliderXY() ) );

  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View1->GetInteractorStyle() ),
    vtkViewImage2DCommand::ContourPickingEvent,
    this, SIGNAL( ActorsSelectionChanged() ) );

  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View2->GetInteractorStyle() ),
    vtkViewImage2DCommand::ContourPickingEvent,
    this, SIGNAL( ActorsSelectionChanged() ) );

  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View3->GetInteractorStyle() ),
    vtkViewImage2DCommand::ContourPickingEvent,
    this, SIGNAL( ActorsSelectionChanged() ) );

//   QObject::connect( this, SIGNAL( ActorsSelectionChanged() ),
//     this, SLOT( HighLightContours() ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param input
 */
void QGoImageView3D::SetImage( vtkImageData* input )
{
  if( !input )
    {
    return;
    }
  else
    {
    this->m_Image = input;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @return
 */
vtkImageData* QGoImageView3D::GetImage()
{
  return m_Image;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param iId
 * @return
 */
vtkImageActor* QGoImageView3D::GetImageActor( const int& iId )
{
  if( ( iId < 0 ) || ( iId > 2 ) )
    {
    return 0;
    }
  else
    {
    vtkViewImage2D* View = m_Pool->GetItem( iId );
    return View->GetImageActor();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param iId
 * @return
 */
QVTKInteractor* QGoImageView3D::GetInteractor( const int& iId )
{

  if( ( iId < 0 ) || ( iId > 3 ) )
    {
    return 0;
    }
  else
    {
    switch( iId )
      {
      default:
      case 0:
        {
        return this->QvtkWidget_XY->GetInteractor();
        }
      case 1:
        {
        return this->QvtkWidget_XZ->GetInteractor();
        }
      case 2:
        {
        return this->QvtkWidget_XZ->GetInteractor();
        }
      case 3:
        {
        return this->QvtkWidget_XYZ->GetInteractor();
        }
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param iType
 * @param iBaseName
 * @return
 */
QString QGoImageView3D::SnapshotViewXY( const GoFigure::FileType& iType,
    const QString& iBaseName )
{
  QString filename = SnapshotView( QvtkWidget_XY, iType,
    iBaseName, m_SnapshotId );

  m_SnapshotId++;
  return filename;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param iType
 * @param iBaseName
 * @return
 */
QString QGoImageView3D::SnapshotView2( const GoFigure::FileType& iType,
    const QString& iBaseName )
{
  QString filename = SnapshotView( QvtkWidget_XZ, iType,
    iBaseName, m_SnapshotId );

  m_SnapshotId++;
  return filename;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param iType
 * @param iBaseName
 * @return
 */
QString QGoImageView3D::SnapshotView3(
    const GoFigure::FileType& iType,
    const QString& iBaseName )
{
  QString filename = SnapshotView( QvtkWidget_YZ, iType,
    iBaseName, m_SnapshotId );

  m_SnapshotId++;
  return filename;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param iType
 * @param iBaseName
 * @return
 */
QString QGoImageView3D::SnapshotViewXYZ(
  const GoFigure::FileType& iType,
  const QString& iBaseName )
{
  QString filename = SnapshotView( QvtkWidget_XYZ, iType,
    iBaseName, m_SnapshotId );

  m_SnapshotId++;
  return filename;
}
//------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param iS
 */
void QGoImageView3D::SetFullScreenView( const int& iS )
{
  if( IsFullScreen == iS )
    {
    IsFullScreen = 0;
    }
  else
    {
    IsFullScreen = iS;
    }

  switch( IsFullScreen )
    {
    default:
    case 0:
      {
      Quadview();
      break;
      }
    case 1:
      {
      FullScreenViewXY();
      break;
      }
    case 2:
      {
      FullScreenViewXZ();
      break;
      }
    case 3:
      {
      FullScreenViewYZ();
      break;
      }
    case 4:
      {
      FullScreenViewXYZ();
      break;
      }
    }
  emit FullScreenViewChanged( IsFullScreen );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void QGoImageView3D::Quadview()
{
  IsFullScreen = 0;
  LayOutWidget1->show();
  LayOutWidget2->show();
  LayOutWidget3->show();
  LayOutWidget4->show();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void QGoImageView3D::FullScreenViewXY()
{
  IsFullScreen = 1;
  LayOutWidget1->show();
  LayOutWidget2->hide();
  LayOutWidget3->hide();
  LayOutWidget4->hide();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void QGoImageView3D::FullScreenViewXZ()
{
  IsFullScreen = 2;
  LayOutWidget1->hide();
  LayOutWidget2->show();
  LayOutWidget3->hide();
  LayOutWidget4->hide();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void QGoImageView3D::FullScreenViewYZ()
{
  IsFullScreen = 3;
  LayOutWidget1->hide();
  LayOutWidget2->hide();
  LayOutWidget3->show();
  LayOutWidget4->hide();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void QGoImageView3D::FullScreenViewXYZ()
{
  IsFullScreen = 4;
  LayOutWidget1->hide();
  LayOutWidget2->hide();
  LayOutWidget3->hide();
  LayOutWidget4->show();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @return
 */
int QGoImageView3D::GetFullScreenView( ) const
{
  return IsFullScreen;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param iEvent
 */
void QGoImageView3D::resizeEvent( QResizeEvent* iEvent )
{
  QWidget::resizeEvent( iEvent );
  VSplitter->resize( iEvent->size() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param iSlice
 */
void QGoImageView3D::SetSliceViewXY( const int& iSlice )
{
  int s = GetSliceViewXY();

  if( iSlice != s )
    {
    this->m_Pool->GetItem( 0 )->SetSlice( iSlice );
    this->m_Pool->SyncRender();
    emit SliceViewXYChanged( iSlice );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @return
 */
int QGoImageView3D::GetSliceViewXY() const
{
  return this->m_Pool->GetItem( 0 )->GetSlice();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @param iSlice
 */
void QGoImageView3D::SetSliceViewXZ( const int& iSlice )
{
  int s = GetSliceViewXZ();

  if( s != iSlice )
    {
    this->m_Pool->GetItem( 1 )->SetSlice( iSlice );
    this->m_Pool->SyncRender();
    emit SliceViewXZChanged( iSlice );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 * @return
 */
int QGoImageView3D::GetSliceViewXZ() const
{
  return this->m_Pool->GetItem( 1 )->GetSlice();
}
//-------------------------------------------------------------------------
/**
 *
 * @param iSlice
 */
void QGoImageView3D::SetSliceViewYZ( const int& iSlice )
{
  int s = GetSliceViewYZ();

  if( s != iSlice )
    {
    this->m_Pool->GetItem( 2 )->SetSlice( iSlice );
    this->m_Pool->SyncRender();
    emit SliceViewYZChanged( iSlice );
    }
}
//-------------------------------------------------------------------------
/**
 *
 * @return
 */
int QGoImageView3D::GetSliceViewYZ() const
{
  return this->m_Pool->GetItem( 2 )->GetSlice();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void QGoImageView3D::MoveSliderXY( )
{
  int s = GetSliceViewXY();

  if( s != this->SliderXY->value() )
    {
    this->SliderXY->setValue( s );
    emit SliceViewXYChanged( s );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void QGoImageView3D::MoveSliderXZ( )
{
  int s = GetSliceViewXZ();

  if( s != this->SliderXZ->value() )
    {
    this->SliderXZ->setValue( s );
    emit SliceViewXZChanged( s );
    }

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void QGoImageView3D::MoveSliderYZ( )
{
  int s = GetSliceViewYZ();

  if( s != this->SliderYZ->value() )
    {
    this->SliderYZ->setValue( s );
    emit SliceViewYZChanged( s );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/**
 *
 */
void QGoImageView3D::SaveStateSplitters()
{
  QSettings settings;
  settings.setValue( "VSplitterSizes", VSplitter->saveState() );
  settings.setValue( "HtSplitterSizes", HtSplitter->saveState() );
  settings.setValue( "HbSplitterSizes", HbSplitter->saveState() );
}
//-------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkViewImage3D*
QGoImageView3D::
GetImageViewer3D()
{
  return View3D;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iId
 * @param dataset
 * @param iProperty
 * @return
 */
std::vector< vtkActor* >
QGoImageView3D::
AddContour( const int& iId, vtkPolyData* dataset, vtkProperty* iProperty )
{
  std::vector< vtkActor* > oList =
    QGoImageView::AddContour( iId, dataset, iProperty );

  vtkActor* temp = View3D->AddDataSet( (vtkDataSet*) dataset,
    iProperty, false, false );

  View3D->Render();
  oList.push_back( temp );

  std::vector< vtkActor* >::iterator list_it = oList.begin();
  while( list_it != oList.end() )
    {
    m_ActorsPropertyMap[*list_it] = iProperty;
    ++list_it;
    }

  return oList;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 *
 * @param iProp
 * @param iToDo
 */
void
QGoImageView3D::
ChangeActorProperty( vtkProp3D* iActor, vtkProperty* iProperty )
{
  View3D->ChangeActorProperty( iActor, iProperty );
  QGoImageView::ChangeActorProperty( iActor, iProperty );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoImageView3D::
ChangeActorProperty( int iDir, vtkProp3D* iActor, vtkProperty* iProperty )
{
  if( ( iDir >= 0 ) && ( iDir < m_Pool->GetNumberOfItems() ) )
    {
    vtkViewImage2D* viewer = m_Pool->GetItem( iDir );
    viewer->ChangeActorProperty( iActor, iProperty );
    }
  else
    {
    if( iDir == 3 )
      {
      View3D->ChangeActorProperty( iActor, iProperty );
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoImageView3D::
RemoveActor( const int& iId, vtkActor* iActor )
{
  if( iId == 3 )
    {
    View3D->GetRenderer()->RemoveActor( iActor );
    }
  else
    {
    QGoImageView::RemoveActor( iId, iActor );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoImageView3D::
AddActor( const int& iId, vtkActor* iActor )
{
  if( iId == 3 )
    {
    View3D->GetRenderer()->AddActor( iActor );
    }
  else
    {
    QGoImageView::AddActor( iId, iActor );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoImageView3D::
SetLookupTable( vtkLookupTable* iLut )
{
  if( this->m_Image->GetNumberOfScalarComponents() == 1 )
    {
    m_Pool->SyncSetLookupTable( iLut );
    m_Pool->SyncRender();
    View3D->SetLookupTable( iLut );
    View3D->Render();
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoImageView3D::
ShowScalarBar( const bool& iShow )
{
  if( this->m_Image->GetNumberOfScalarComponents() == 1 )
    {
    m_Pool->SyncSetShowScalarBar( iShow );
    m_Pool->SyncRender();
    View3D->SetShowScalarBar( iShow );
    View3D->Render();
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------

/**
 * \todo use dynamic_cast or more appropriate cast operator
 */
void
QGoImageView3D::
AddMesh( vtkPolyData* iMesh )
{
  vtkProperty* prop = vtkProperty::New();
  prop->SetColor( 0., 1., 1. );

  for( int i = 0; i < m_Pool->GetNumberOfItems(); i++ )
    {
    vtkViewImage2D* viewer = m_Pool->GetItem( i );
    viewer->AddDataSet( iMesh, prop, true, false );
    }
  View3D->AddDataSet( (vtkDataSet*) iMesh, prop, false, false );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// void
// QGoImageView3D::
// HighLightContours()
// {
//   vtkViewImage2DCollectionCommand* command = m_Pool->GetCommand();
//   std::list< vtkProp3D* > listofpicked = command->GetListOfPickedActors();
//
//   std::list< vtkProp3D* >::iterator it = listofpicked.begin();
//
//   while( it != listofpicked.end() )
//     {
//     ChangeActorProperty( *it, m_HighlightedContourProperty );
//     ++it;
//     }
//
//   std::list< vtkProp3D* > listofunpicked = command->GetListOfUnPickedActors();
//   it = listofunpicked.begin();
//
//   while( it != listofunpicked.end() )
//     {
//     ChangeActorProperty( *it, m_ActorsPropertyMap[*it] );
//     ++it;
//     }
// }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< vtkProp3D* >
QGoImageView3D::GetListOfPickedActors()
{
  vtkViewImage2DCollectionCommand* command = m_Pool->GetCommand();
  return command->GetListOfPickedActors();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< vtkProp3D* >
QGoImageView3D::
GetListOfUnPickedActors()
{
  vtkViewImage2DCollectionCommand* command = m_Pool->GetCommand();
  return command->GetListOfUnPickedActors();
}
//--------------------------------------------------------------------------
