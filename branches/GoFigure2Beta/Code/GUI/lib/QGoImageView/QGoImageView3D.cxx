#include "QGoImageView3D.h"

#include "vtkRenderWindow.h"
#include "vtkRendererCollection.h"
#include "vtkRenderer.h"
#include "vtkTextProperty.h"
#include "vtkImageClip.h"
#include "vtkImagePermute.h"
#include "vtkImageResample.h"
#include "vtkWindowToImageFilter.h"
#include "vtkBMPWriter.h"
#include "vtkPostScriptWriter.h"
#include "vtkJPEGWriter.h"
#include "vtkPNGWriter.h"
#include "vtkTIFFWriter.h"

#include <QResizeEvent>
#include <QSettings>

#include "vtkViewImage2DCommand.h"

void QGoImageView3D::SetupViewGivenQVTKWidget(
  vtkViewImage2D* iView,
  QVTKWidget* iWidget )
{
  vtkRenderWindow* renwin = iWidget->GetRenderWindow( );
  iView->SetRenderWindow( renwin );
  iView->SetRenderer( renwin->GetRenderers()->GetFirstRenderer() );
  iView->SetupInteractor( iWidget->GetInteractor() );
}

//-------------------------------------------------------------------------
QGoImageView3D::QGoImageView3D( QWidget* iParent ) : QWidget( iParent )
{
  IsFullScreen = 0;
  m_SnapshotId = 1;

  m_Tag = QString( "QGoImageView3D" );
  m_Image = 0;

  VtkEventQtConnector = vtkEventQtSlotConnect::New();

  setupUi( this );
  QObject::connect( this->SliderXY, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSlideView1( int ) ) );
  QObject::connect( this->SliderXZ, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSlideView2( int ) ) );
  QObject::connect( this->SliderYZ, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSlideView3( int ) ) );

  QObject::connect( this->HtSplitter, SIGNAL( splitterMoved( int, int ) ),
    this->HbSplitter, SLOT( moveSplitter( int, int ) ) );
  QObject::connect(this->HbSplitter, SIGNAL( splitterMoved( int, int ) ),
    this->HtSplitter, SLOT( moveSplitter( int, int ) ) );

  Pool = vtkViewImage2DCollection::New();
  View3D = vtkViewImage3D::New();

  vtkViewImage2D* View1 = vtkViewImage2D::New();
  View1->SetViewOrientation( vtkViewImage2D::VIEW_ORIENTATION_AXIAL );
  View1->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );

  this->SetupViewGivenQVTKWidget( View1, this->QvtkWidget_XY );

  this->Pool->AddItem( View1 );
  View1->Delete();

  vtkViewImage2D* View2 = vtkViewImage2D::New();
  View2->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
  View2->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_CORONAL);

  this->SetupViewGivenQVTKWidget( View2, this->QvtkWidget_XZ );

  this->Pool->AddItem( View2 );
  View2->Delete();

  vtkViewImage2D* View3 = vtkViewImage2D::New();
  View3->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
  View3->SetViewOrientation( vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL );

  this->SetupViewGivenQVTKWidget( View3, this->QvtkWidget_YZ );

  this->Pool->AddItem( View3 );
  View3->Delete();

  vtkRenderWindow* renwin4 = this->QvtkWidget_XYZ->GetRenderWindow( );
  this->View3D->SetRenderWindow( renwin4 );
  this->View3D->SetupInteractor( this->QvtkWidget_XYZ->GetInteractor() );
  this->Pool->SetExtraRenderWindow( renwin4 );
}

QGoImageView3D::~QGoImageView3D()
{
  delete HtSplitter;
  delete HbSplitter;

  Pool->Delete();
  View3D->Delete();
  VtkEventQtConnector->Delete();
}

//-------------------------------------------------------------------------
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
void QGoImageView3D::retranslateUi(QWidget *iParent)
{
  iParent->setWindowTitle( this->m_Tag );
  Q_UNUSED(iParent);
}
//-------------------------------------------------------------------------
void QGoImageView3D::SetImage( vtkImageData* input )
{
  if( !input )
    {
    return;
    }
  else
    {
    this->m_Image = input;

    vtkViewImage2D* View1 = this->Pool->GetItem( 0 );
    View1->SetInput( this->m_Image );

    this->View3D->Add2DPhantom( 0,
      View1->GetImageActor(), View1->GetSlicePlane() );

    int *range = View1->GetSliceRange();

    this->SliderXY->setMinimum( range[0] );
    this->SliderXY->setMaximum( range[1] );

    // Event connection between vtk and qt
    // when SliceMoveEvent occurs in the XY View, SliderXY moves.
    VtkEventQtConnector->Connect(
      reinterpret_cast< vtkObject* >( View1->GetInteractorStyle() ),
      vtkViewImage2DCommand::SliceMoveEvent,
      this, SLOT( MoveSliderXY() ) );

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

    vtkViewImage2D* View2 = this->Pool->GetItem( 1 );
    View2->SetInput( this->m_Image );

    this->View3D->Add2DPhantom( 1,
      View2->GetImageActor(), View2->GetSlicePlane() );

    range = View2->GetSliceRange();

    this->SliderXZ->setMinimum( range[0] );
    this->SliderXZ->setMaximum( range[1] );
    // Event connection between vtk and qt
    // when SliceMoveEvent occurs in the XY View, SliderXY moves.
    VtkEventQtConnector->Connect(
      reinterpret_cast< vtkObject* >( View2->GetInteractorStyle() ),
      vtkViewImage2DCommand::SliceMoveEvent,
      this, SLOT( MoveSliderXZ() ) );

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

    vtkViewImage2D* View3 = this->Pool->GetItem( 2 );
    View3->SetInput( this->m_Image );

    this->View3D->Add2DPhantom(
      2, View3->GetImageActor(), View3->GetSlicePlane() );

    range = View3->GetSliceRange();
    this->SliderYZ->setMinimum( range[0] );
    this->SliderYZ->setMaximum( range[1] );

    // Event connection between vtk and qt
    // when SliceMoveEvent occurs in the XY View, SliderXY moves.
    VtkEventQtConnector->Connect(
      reinterpret_cast< vtkObject* >( View3->GetInteractorStyle() ),
      vtkViewImage2DCommand::SliceMoveEvent,
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


    this->View3D->SetInput( this->m_Image );
    this->View3D->SetVolumeRenderingOff();
    this->View3D->SetTriPlanarRenderingOn();
    this->View3D->SetShowScalarBar( false );
    this->View3D->ResetCamera();

    this->Pool->Initialize();
//     this->Pool->InitializeAllObservers();
    this->Pool->SyncSetBackground( this->Pool->GetItem(0)->GetBackground() );
    this->Pool->SyncSetShowAnnotations( true );

    for( int i = 0; i < 3; i++ )
      {
      this->Pool->GetItem(i)->GetTextProperty()->SetFontFamilyToArial();
      this->Pool->GetItem(i)->GetTextProperty()->SetFontSize( 14 );
      }

    this->Pool->SyncSetShowScalarBar( false );
    this->Pool->SyncRender();
    this->Pool->SyncReset();

    this->SliderXY->setValue( (this->SliderXY->minimum()+this->SliderXY->maximum())/2 );
    this->SliderXZ->setValue( (this->SliderXZ->minimum()+this->SliderXZ->maximum())/2 );
    this->SliderYZ->setValue( (this->SliderYZ->minimum()+this->SliderYZ->maximum())/2 );
    }
}

//-------------------------------------------------------------------------
bool QGoImageView3D::BuildScreenshotFromImage( vtkImageData* image,
  vtkImageData* screenshot,
  int tsize )
{
  if (!image || !screenshot)
    {
    return false;
    }

  // Empty image, remove thumbnail/screenshot

  int image_dims[3];
  image->GetDimensions(image_dims);
  if (image_dims[0] == 0 ||
      image_dims[1] == 0 ||
      image_dims[2] == 0)
    {
    return false;
    }

  double factor;
  vtkImageData *resample_input, *resample_output;

  // First, let's make sure we are processing the image as it
  // is by clipping its UpdateExtent. By doing so, we prevent our resample
  // and permute filter the process the image's *whole* extent.

  vtkImageClip *clip = vtkImageClip::New();
  clip->SetInput(image);
  clip->SetOutputWholeExtent(image->GetUpdateExtent());
  clip->Update();

  // Permute, as a convenience

  int clip_dims[3];
  clip->GetOutput()->GetDimensions(clip_dims);

  vtkImagePermute *permute = NULL;
  if (clip_dims[2] != 1)
    {
    permute = vtkImagePermute::New();
    permute->SetInput(clip->GetOutput());
    if (clip_dims[0] == 1)
      {
      permute->SetFilteredAxes(1, 2, 0);
      }
    else
      {
      permute->SetFilteredAxes(0, 2, 1);
      }
    resample_input = permute->GetOutput();
    }
  else
    {
    resample_input = clip->GetOutput();
    }

  resample_input->Update();
  int resample_input_dims[3];//, resample_output_dims[3];

  resample_input->GetDimensions(resample_input_dims);
  double *resample_input_spacing = resample_input->GetSpacing();

  int large_dim = 0, small_dim = 1;
  if (resample_input_dims[0] < resample_input_dims[1])
    {
    large_dim = 1;
    small_dim = 0;
    }

  if( tsize != 0 )
    {
    vtkImageResample *resample = vtkImageResample::New();
    resample->SetInput(resample_input);
    resample->SetInterpolationModeToCubic();
    resample->SetDimensionality(2);

    // Create the screenshot

    factor = static_cast< double >( tsize ) /
      static_cast< double >( resample_input_dims[large_dim] );

    resample->SetAxisMagnificationFactor(large_dim, factor);
    resample->SetAxisMagnificationFactor(
      small_dim, factor * (resample_input_spacing[small_dim] /
                          resample_input_spacing[large_dim]));
    resample->Update();
    resample_output = resample->GetOutput();
  //   resample_output->GetDimensions(resample_output_dims);

    screenshot->ShallowCopy( resample_output );
// SetImage(
//     (const unsigned char*) resample_output->GetScalarPointer(),
//     resample_output_dims[0],
//     resample_output_dims[1],
//     3,
//     0,
//     vtkKWIcon::ImageOptionFlipVertical);
    resample->Delete();
    }
  else
    {
    screenshot->ShallowCopy( resample_input );
    }

  // Deallocate

  clip->Delete();

  if( permute )
    {
    permute->Delete();
    }

  return true;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QGoImageView3D::BuildScreenshotFromRenderWindow(
  vtkRenderWindow *win,
  vtkImageData* screenshot,
  int tsize )
{
  if( win && screenshot )
    {
    vtkWindowToImageFilter* filter = vtkWindowToImageFilter::New();
    filter->ShouldRerenderOff();
    filter->SetInput( win );
    filter->Update();
    bool res = this->BuildScreenshotFromImage( filter->GetOutput(),
      screenshot, tsize );
    filter->Delete();
    return res;
    }
  return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoImageView3D::SnapshotView( QVTKWidget* iWidget,
  const SnapshotImageType& iType,
  const QString& iBaseName )
{
  vtkImageData* image = vtkImageData::New();
  BuildScreenshotFromRenderWindow( iWidget->GetRenderWindow(),
        image );
  QString filename = iBaseName;
  filename.append( QString( "%1" ).arg( m_SnapshotId ) );

  switch( iType )
    {
    default:
    case BMP:
      {
      filename.append( ".bmp" );
      vtkBMPWriter* bmp_writer = vtkBMPWriter::New();
      bmp_writer->SetInput( image );
      bmp_writer->SetFileName( filename.toAscii( ).constData( ) );
      bmp_writer->Write();
      bmp_writer->Delete();
      }
      break;
    case EPS:
      {
      filename.append( ".eps" );
      vtkPostScriptWriter* eps_writer = vtkPostScriptWriter::New();
      eps_writer->SetInput( image );
      eps_writer->SetFileName( filename.toAscii( ).constData( ) );
      eps_writer->Write();
      eps_writer->Delete();
      }
      break;
    case JPEG:
      {
      filename.append( ".jpeg" );
      vtkJPEGWriter* jpeg_writer = vtkJPEGWriter::New();
      jpeg_writer->SetInput( image );
      jpeg_writer->SetFileName( filename.toAscii( ).constData( ) );
      jpeg_writer->Write();
      jpeg_writer->Delete();
      }
      break;
    case PNG:
      {
      filename.append( ".png" );
      vtkPNGWriter* png_writer = vtkPNGWriter::New();
      png_writer->SetInput( image );
      png_writer->SetFileName( filename.toAscii( ).constData( ) );
      png_writer->Write();
      png_writer->Delete();
      }
      break;
    case TIFF:
      {
      filename.append( ".tiff" );
      vtkTIFFWriter* tiff_writer = vtkTIFFWriter::New();
      tiff_writer->SetInput( image );
      tiff_writer->SetFileName( filename.toAscii( ).constData( ) );
      tiff_writer->Write();
      tiff_writer->Delete();
      }
      break;
    }
  m_SnapshotId++;
  image->Delete();
  return filename;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoImageView3D::SnapshotViewXY( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  return SnapshotView( QvtkWidget_XY, iType, iBaseName );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoImageView3D::SnapshotView2( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  return SnapshotView( QvtkWidget_XZ, iType, iBaseName );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoImageView3D::SnapshotView3(
    const SnapshotImageType& iType,
    const QString& iBaseName )
{
  return SnapshotView( QvtkWidget_YZ, iType, iBaseName );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QGoImageView3D::SnapshotViewXYZ(
  const SnapshotImageType& iType,
  const QString& iBaseName )
{
  return SnapshotView( QvtkWidget_XYZ, iType, iBaseName );
}
//------------------------------------------------------------------------

//-------------------------------------------------------------------------
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
      LayOutWidget1->show();
      LayOutWidget2->show();
      LayOutWidget3->show();
      LayOutWidget4->show();
      break;
      }
    case 1:
      {
      LayOutWidget1->show();
      LayOutWidget2->hide();
      LayOutWidget3->hide();
      LayOutWidget4->hide();
      break;
      }
    case 2:
      {
      LayOutWidget1->hide();
      LayOutWidget2->show();
      LayOutWidget3->hide();
      LayOutWidget4->hide();
      break;
      }
    case 3:
      {
      LayOutWidget1->hide();
      LayOutWidget2->hide();
      LayOutWidget3->show();
      LayOutWidget4->hide();
      break;
      }
    case 4:
      {
      LayOutWidget1->hide();
      LayOutWidget2->hide();
      LayOutWidget3->hide();
      LayOutWidget4->show();
      break;
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QGoImageView3D::GetFullScreenView( ) const
{
  return IsFullScreen;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
double* QGoImageView3D::GetBackgroundColor()
{
  return this->Pool->GetItem( 0 )->GetBackground();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView3D::GetBackgroundColor( double& r,
  double& g, double& b )
{
  double* rgb = this->Pool->GetItem( 0 )->GetBackground();
  r = rgb[0];
  g = rgb[1];
  b = rgb[2];
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView3D::SetBackgroundColor( const double& r,
const double& g, const double& b )
{
  double textcolor[3];
  textcolor[0] = 1. - r;
  textcolor[1] = 1. - g;
  textcolor[2] = 1. - b;

  double rgb[3] = {r, g, b };

  this->Pool->SyncSetBackground( rgb );

  for( int i = 0; i < 3; i++ )
    {
    vtkTextProperty* tproperty =
      this->Pool->GetItem( i )->GetTextProperty();
    tproperty->SetFontFamilyToArial();
    tproperty->SetFontSize( 14 );
    tproperty->SetColor( textcolor );
    }

  //   this->Pool->SyncSetTextProperty( property );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView3D::SetBackgroundColor( double rgb[3] )
{
  SetBackgroundColor( rgb[0], rgb[1], rgb[2] );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView3D::SetBackgroundColor( const QColor& iColor )
{
  int r, g, b;
  iColor.getRgb( &r, &g, &b );

  SetBackgroundColor( static_cast< double >( r ) / 255.,
    static_cast< double >( g ) / 255.,
    static_cast< double >( b ) / 255. );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView3D::resizeEvent( QResizeEvent* iEvent )
{
  QWidget::resizeEvent( iEvent );
  VSplitter->resize( iEvent->size() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView3D::SetSlideViewXY( const int& iSlice )
{
  this->Pool->GetItem( 0 )->SetSlice( iSlice );
  this->Pool->SyncRender();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView3D::SetSlideViewXZ( const int& iSlice )
{
  this->Pool->GetItem( 1 )->SetSlice( iSlice );
  this->Pool->SyncRender();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView3D::SetSlideViewYZ( const int& iSlice )
{
  this->Pool->GetItem( 2 )->SetSlice( iSlice );
  this->Pool->SyncRender();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView3D::MoveSliderXY( )
{
  this->SliderXY->setValue( this->Pool->GetItem( 0 )->GetSlice() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView3D::MoveSliderXZ( )
{
  this->SliderXZ->setValue( this->Pool->GetItem( 1 )->GetSlice() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView3D::MoveSliderYZ( )
{
  this->SliderYZ->setValue( this->Pool->GetItem( 2 )->GetSlice() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoImageView3D::SaveStateSplitters()
{
  QSettings settings;
  settings.setValue("VSplitterSizes", VSplitter->saveState());
  settings.setValue("HtSplitterSizes", HtSplitter->saveState());
  settings.setValue("HbSplitterSizes", HbSplitter->saveState());
}
//-------------------------------------------------------------------------