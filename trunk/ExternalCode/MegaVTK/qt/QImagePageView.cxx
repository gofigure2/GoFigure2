#include "QImagePageView.h"
#include <QResizeEvent>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkTextProperty.h>

#include <vtkImageClip.h>
#include <vtkImagePermute.h>
#include <vtkImageResample.h>
#include <vtkWindowToImageFilter.h>

#include <vtkBMPWriter.h>
#include <vtkPostScriptWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkTIFFWriter.h>

QImagePageView::QImagePageView( QWidget* parent ) : QWidget( parent )
{
  IsFullScreen = 0;
  SnapshotId = 1;

  Tag = QString( "QImagePageView" );
  Image = 0;

  Pool = vtkViewImage2DCollection::New();
  View3D = vtkViewImage3D::New();

  setupUi( this );
}

QImagePageView::~QImagePageView()
{
  Pool->Delete();
  View3D->Delete();

//   delete qvtkWidget_4;
//   delete qvtkWidget_3;
//   delete qvtkWidget_2;
//   delete qvtkWidget_1;
//
//   delete htSplitter;
//   delete hbSplitter;
//   delete vSplitter;
}

void QImagePageView::setupUi( QWidget* parent )
{
  if(parent->objectName().isEmpty())
    {
    parent->resize(800, 800);
    }

  QList< int > list_size;
  list_size.push_back( 10 );
  list_size.push_back( 10 );

  vSplitter  = new QSplitter( Qt::Vertical, parent );
  htSplitter = new QSplitter( vSplitter );
  hbSplitter = new QSplitter( vSplitter );
  vSplitter->addWidget(htSplitter);
  vSplitter->addWidget(hbSplitter);
  vSplitter->setSizes( list_size );
  vSplitter->resize( 800, 800 );

  qvtkWidget_1 = new QVTKWidget;
  slider1 = new QSlider( Qt::Vertical );
  LayOut1 = new QHBoxLayout;
  LayOut1->addWidget( qvtkWidget_1 );
  LayOut1->addWidget( slider1 );
  LayOutWidget1 = new QWidget;
  LayOutWidget1->setLayout( LayOut1 );
  htSplitter->addWidget( LayOutWidget1 );

  qvtkWidget_2 = new QVTKWidget;
  slider2 = new QSlider( Qt::Vertical );
  LayOut2 = new QHBoxLayout;
  LayOut2->addWidget( qvtkWidget_2 );
  LayOut2->addWidget( slider2 );
  LayOutWidget2 = new QWidget;
  LayOutWidget2->setLayout( LayOut2 );
  htSplitter->addWidget( LayOutWidget2 );
  htSplitter->setSizes( list_size );

  qvtkWidget_3 = new QVTKWidget;
  slider3 = new QSlider( Qt::Vertical );
  LayOut3 = new QHBoxLayout;
  LayOut3->addWidget( qvtkWidget_3 );
  LayOut3->addWidget( slider3 );
  LayOutWidget3 = new QWidget;
  LayOutWidget3->setLayout( LayOut3 );
  hbSplitter->addWidget( LayOutWidget3 );

  qvtkWidget_4 = new QVTKWidget;
  Spacer = new QSpacerItem( 27, 10,
    QSizePolicy::Minimum, QSizePolicy::Minimum);
  LayOut4 = new QHBoxLayout;
  LayOut4->addWidget( qvtkWidget_4 );
  LayOut4->addItem( Spacer );
  LayOutWidget4 = new QWidget;
  LayOutWidget4->setLayout( LayOut4 );
  hbSplitter->addWidget( LayOutWidget4 );
  hbSplitter->setSizes( list_size );

  retranslateUi(parent);

  QMetaObject::connectSlotsByName(parent);
} // setupUi

void QImagePageView::retranslateUi(QWidget *parent)
{
  parent->setWindowTitle( this->Tag );
  Q_UNUSED(parent);
}

// bool QImagePageView::BuildScreenshotFromImage( vtkImageData* image,
//   vtkImageData* screenshot,
//   int size )
// {
//   if (!image || !screenshot)
//     {
//     return false;
//     }
//
//   // Empty image, remove thumbnail/screenshot
//
//   int image_dims[3];
//   image->GetDimensions(image_dims);
//   if (image_dims[0] == 0 ||
//       image_dims[1] == 0 ||
//       image_dims[2] == 0)
//     {
//     return false;
//     }
//
//   double factor;
//   vtkImageData *resample_input, *resample_output;
//
//   // First, let's make sure we are processing the image as it
//   // is by clipping its UpdateExtent. By doing so, we prevent our resample
//   // and permute filter the process the image's *whole* extent.
//
//   vtkImageClip *clip = vtkImageClip::New();
//   clip->SetInput(image);
//   clip->SetOutputWholeExtent(image->GetUpdateExtent());
//   clip->Update();
//
//   // Permute, as a convenience
//
//   int clip_dims[3];
//   clip->GetOutput()->GetDimensions(clip_dims);
//
//   vtkImagePermute *permute = NULL;
//   if (clip_dims[2] != 1)
//     {
//     permute = vtkImagePermute::New();
//     permute->SetInput(clip->GetOutput());
//     if (clip_dims[0] == 1)
//       {
//       permute->SetFilteredAxes(1, 2, 0);
//       }
//     else
//       {
//       permute->SetFilteredAxes(0, 2, 1);
//       }
//     resample_input = permute->GetOutput();
//     }
//   else
//     {
//     resample_input = clip->GetOutput();
//     }
//
//   resample_input->Update();
//   int resample_input_dims[3];//, resample_output_dims[3];
//
//   resample_input->GetDimensions(resample_input_dims);
//   double *resample_input_spacing = resample_input->GetSpacing();
//
//   int large_dim = 0, small_dim = 1;
//   if (resample_input_dims[0] < resample_input_dims[1])
//     {
//     large_dim = 1;
//     small_dim = 0;
//     }
//
//   if( size != 0 )
//     {
//     vtkImageResample *resample = vtkImageResample::New();
//     resample->SetInput(resample_input);
//     resample->SetInterpolationModeToCubic();
//     resample->SetDimensionality(2);
//
//     // Create the screenshot
//
//     factor = static_cast< double >( size ) /
//       static_cast< double >( resample_input_dims[large_dim] );
//
//     resample->SetAxisMagnificationFactor(large_dim, factor);
//     resample->SetAxisMagnificationFactor(
//       small_dim, factor * (resample_input_spacing[small_dim] /
//                           resample_input_spacing[large_dim]));
//     resample->Update();
//     resample_output = resample->GetOutput();
//   //   resample_output->GetDimensions(resample_output_dims);
//
//     screenshot->ShallowCopy( resample_output );
// // SetImage(
// //     (const unsigned char*) resample_output->GetScalarPointer(),
// //     resample_output_dims[0],
// //     resample_output_dims[1],
// //     3,
// //     0,
// //     vtkKWIcon::ImageOptionFlipVertical);
//     resample->Delete();
//     }
//   else
//     {
//     screenshot->ShallowCopy( resample_input );
//     }
//
//   // Deallocate
//
//   clip->Delete();
//
//   if( permute )
//     {
//     permute->Delete();
//     }
//
//   return true;
// }
//
// bool QImagePageView::BuildScreenshotFromRenderWindow(
//   vtkRenderWindow *win,
//   vtkImageData* screenshot,
//   int size )
// {
//   if( win && screenshot )
//     {
//     vtkWindowToImageFilter* filter = vtkWindowToImageFilter::New();
//     filter->ShouldRerenderOff();
//     filter->SetInput( win );
//     filter->Update();
//     bool res = this->BuildScreenshotFromImage( filter->GetOutput(),
//       screenshot, size );
//     filter->Delete();
//     return res;
//     }
//   return false;
// }
/*
void QImagePageView::SnapshotView( QVTKWidget* iWidget,
  const SnapshotImageType& iType,
  const QString& iBaseName )
{
  vtkImageData* image = vtkImageData::New();
  BuildScreenshotFromRenderWindow( iWidget->GetRenderWindow(),
        image );
  QString filename = iBaseName;
  filename.append( QString( "%1" ).arg( SnapshotId ) );

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
  SnapshotId++;
  image->Delete();
}

void QImagePageView::SnapshotView1( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  SnapshotView( qvtkWidget_1, iType, iBaseName );
}

void QImagePageView::SnapshotView2( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  SnapshotView( qvtkWidget_2, iType, iBaseName );
}
void QImagePageView::SnapshotView3( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  SnapshotView( qvtkWidget_3, iType, iBaseName );
}
void QImagePageView::SnapshotView4( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  SnapshotView( qvtkWidget_4, iType, iBaseName );
}*/

void QImagePageView::SetFullScreenView( const int& iS )
{
  if( IsFullScreen == iS )
    IsFullScreen = 0;
  else
    IsFullScreen = iS;

  switch( IsFullScreen )
  {
  default:
  case 0:
    {
      qvtkWidget_1->show();
      qvtkWidget_2->show();
      qvtkWidget_3->show();
      qvtkWidget_4->show();
      break;
    }
  case 1:
    {
      qvtkWidget_1->show();
      qvtkWidget_2->hide();
      qvtkWidget_3->hide();
      qvtkWidget_4->hide();
      break;
    }
  case 2:
    {
      qvtkWidget_1->hide();
      qvtkWidget_2->show();
      qvtkWidget_3->hide();
      qvtkWidget_4->hide();
      break;
    }
  case 3:
    {
      qvtkWidget_1->hide();
      qvtkWidget_2->hide();
      qvtkWidget_3->show();
      qvtkWidget_4->hide();
      break;
    }
  case 4:
    {
      qvtkWidget_1->hide();
      qvtkWidget_2->hide();
      qvtkWidget_3->hide();
      qvtkWidget_4->show();
      break;
    }
  }
}

int QImagePageView::GetFullScreenView( ) const
{
  return IsFullScreen;
}
//
// void QImagePageView::Render( )
// {
//    this->View1->Render();
//    this->View2->Render();
//    this->View3->Render();
//    this->View4->Render();
// }

void QImagePageView::SetBackgroundColor( const double& r,
const double& g, const double& b )
{
  double textcolor[3];
  textcolor[0] = 1. - r;
  textcolor[1] = 1. - g;
  textcolor[2] = 1. - b;

//   View1->SetBackground( r, g, b );
//   View1->GetTextProperty()->SetColor( textcolor );
//
//   View2->SetBackground( r, g, b );
//   View1->GetTextProperty()->SetColor( textcolor );
//
//   View3->SetBackground( r, g, b );
//   View3->GetTextProperty()->SetColor( textcolor );
//
//   View4->SetBackground( r, g, b );
//   View4->GetTextProperty()->SetColor( textcolor );
}

void QImagePageView::SetBackgroundColor( double rgb[3] )
{
  double textcolor[3];
  textcolor[0] = 1. - rgb[0];
  textcolor[1] = 1. - rgb[1];
  textcolor[2] = 1. - rgb[2];

//   View1->SetBackground( rgb[0], rgb[1], rgb[2] );
//   View1->GetTextProperty()->SetColor( textcolor );
//
//   View2->SetBackground( rgb[0], rgb[1], rgb[2] );
//   View2->GetTextProperty()->SetColor( textcolor );
//
//   View3->SetBackground( rgb[0], rgb[1], rgb[2] );
//   View3->GetTextProperty()->SetColor( textcolor );
//
//   View4->SetBackground( rgb[0], rgb[1], rgb[2] );
//   View4->GetTextProperty()->SetColor( textcolor );
}

void QImagePageView::SetBackgroundColor( const QColor& iColor )
{
  int r, g, b;
  iColor.getRgb( &r, &g, &b );
  double rgb[3];
  rgb[0] = static_cast< double >( r ) / 255.;
  rgb[1] = static_cast< double >( g ) / 255.;
  rgb[2] = static_cast< double >( b ) / 255.;

  double textcolor[3];
  textcolor[0] = 1. - rgb[0];
  textcolor[1] = 1. - rgb[1];
  textcolor[2] = 1. - rgb[2];

//   View1->SetBackground( rgb[0], rgb[1], rgb[2] );
//   View1->GetTextProperty()->SetColor( textcolor );
//
//   View2->SetBackground( rgb[0], rgb[1], rgb[2] );
//   View2->GetTextProperty()->SetColor( textcolor );
//
//   View3->SetBackground( rgb[0], rgb[1], rgb[2] );
//   View3->GetTextProperty()->SetColor( textcolor );
//
//   View4->SetBackground( rgb[0], rgb[1], rgb[2] );
//   View4->GetTextProperty()->SetColor( textcolor );
}

QVTKWidget* QImagePageView::GetActiveQVTKWidget( )
{

  switch(this->IsFullScreen)
  {
  case 1 :
    return qvtkWidget_1;
  case 2 :
    return qvtkWidget_2;
  case 3 :
    return qvtkWidget_3;
  default :
    return qvtkWidget_4;
  }
}


vtkViewImage* QImagePageView::GetActiveView()
{
  switch(this->IsFullScreen)
  {
  case 1 :
    return this->Pool->GetItem(0);
  case 2 :
    return this->Pool->GetItem(1);
  case 3 :
    return this->Pool->GetItem(2);
  default :
    return this->Pool->GetItem(3);
  }
}

void QImagePageView::SetLookupTable( vtkLookupTable* lut )
{
  if( !lut )
    {
    return;
    }
  else
    {
    this->Pool->SyncSetLookupTable( lut );
    }
}

void QImagePageView::FullScreenView1 ()
{
  SetFullScreenView( 1 );
}


void QImagePageView::FullScreenView2( )
{
  SetFullScreenView( 2 );
}


void QImagePageView::FullScreenView3( )
{
  SetFullScreenView( 3 );
}


void QImagePageView::FullScreenView4 ( )
{
  SetFullScreenView( 4 );
}

void QImagePageView::CreateAllViews()
{
//   ConfigureView( this->Pool->GetItem(0), qvtkWidget_1 );
//   ConfigureView( this->Pool->GetItem(1), qvtkWidget_2 );
//   ConfigureView( this->Pool->GetItem(2), qvtkWidget_3 );
//   ConfigureView( this->Pool->GetItem(3), qvtkWidget_4 );

  this->Pool->GetItem(0)->SetBackground(0.,0.,0.);
  this->Pool->GetItem(1)->SetBackground(0.,0.,0.);
  this->Pool->GetItem(2)->SetBackground(0.,0.,0.);
//   this->Pool->GetItem(3)->SetBackground(0.,0.,0.);

//   this->SetView4ToTriPlanarMode();
}

// void QImagePageView::SetView4ToTriPlanarMode()
// {
//   View4->SetPlaneWidgetVisibility( true );
// }
//
// void QImagePageView::SetView4ToVolumeRenderingMode()
// {
//   View4->SetPlaneWidgetVisibility( false );
// }

#ifdef MegaVTK_USE_ITK
template< class TImage >
void QImagePageView::SetITKImage( TImage::Pointer itkImage )
{
  if( itkImage.IsNull() )
  {
    return;
  }

  typedef itk::ImageToVTKImageFilter< TImage > ConverterType;
  ConverterType::Pointer myConverter = ConverterType::New();
  myConverter->SetInput ( itkImage );
  myConverter->Update();

  this->SetImage ( myConverter->GetOutput() );

  this->ImageConverter = myConverter;
}
#endif

void QImagePageView::SetImage( vtkImageData* input )
{
  if( !input )
    return;
  else
    {
    this->Image = input;

    vtkViewImage2D* View1 = vtkViewImage2D::New();
    View1->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
    vtkRenderWindow* renwin1 = this->qvtkWidget_1->GetRenderWindow( );
//     renwin1->GetRenderers()->RemoveAllItems();
    View1->SetupInteractor( this->qvtkWidget_1->GetInteractor() );
    View1->SetRenderWindow( renwin1 );
    View1->SetRenderer( renwin1->GetRenderers()->GetFirstRenderer() );
    View1->SetViewOrientation( vtkViewImage2D::VIEW_ORIENTATION_AXIAL );
    View1->SetInput( this->Image );

    this->Pool->AddItem( View1 );
    this->View3D->Add2DPhantom( 0, View1->GetImageActor(), View1->GetSlicePlane() );
    View1->Delete();

    vtkViewImage2D* View2 = vtkViewImage2D::New();
    View2->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
    vtkRenderWindow* renwin2 = this->qvtkWidget_2->GetRenderWindow( );
    View2->SetRenderWindow( renwin2 );
    View2->SetRenderer( renwin2->GetRenderers()->GetFirstRenderer() );
    View2->SetupInteractor( this->qvtkWidget_2->GetInteractor() );
    View2->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_CORONAL);
    View2->SetInput( this->Image );

    this->Pool->AddItem( View2 );
    this->View3D->Add2DPhantom( 1, View2->GetImageActor(), View2->GetSlicePlane() );
    View2->Delete();

    vtkViewImage2D* View3 = vtkViewImage2D::New();
    View2->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
    vtkRenderWindow* renwin3 = this->qvtkWidget_3->GetRenderWindow( );
    View3->SetRenderWindow( renwin3 );
    View3->SetRenderer( renwin3->GetRenderers()->GetFirstRenderer() );
    View3->SetupInteractor( this->qvtkWidget_3->GetInteractor() );
    View3->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL);
    View3->SetInput( this->Image );

    this->Pool->AddItem( View3 );
    this->View3D->Add2DPhantom( 2, View3->GetImageActor(), View3->GetSlicePlane() );
    View3->Delete();

    int size[2] = {400, 400};
    this->Pool->SyncSetSize (size);

    vtkRenderWindow* renwin4 = this->qvtkWidget_4->GetRenderWindow( );
    this->View3D->SetRenderWindow( renwin4 );
    this->View3D->SetupInteractor( this->qvtkWidget_4->GetInteractor() );
    this->View3D->SetInput( this->Image );

    this->View3D->ResetCamera();
    this->Pool->SetExtraRenderWindow( renwin4 );

    this->Pool->Initialize();
    this->Pool->SyncSetShowAnnotations( true );
    this->Pool->SyncSetShowScalarBar( true );
    this->Pool->SyncReset();
    this->Pool->SyncRender();
    }
}

void QImagePageView::SetShowScalarBar( const bool& state )
{
  this->Pool->SyncSetShowScalarBar(state);
}

void QImagePageView::SetShowAnnotations( const bool& iState )
{
  this->Pool->SyncSetShowAnnotations( iState );
}

void QImagePageView::SetColorWindow( const double& iValue )
{
  this->Pool->SyncSetColorWindow( iValue );
}

void QImagePageView::SetColorLevel( const double& iValue )
{
  this->Pool->SyncSetColorLevel( iValue );
}

void QImagePageView::SetTag( const QString& iTag )
{
  this->Tag = iTag;
}

QString QImagePageView::GetTag( ) const
{
  return this->Tag;
}

void QImagePageView::resizeEvent( QResizeEvent* event )
{
  QWidget::resizeEvent( event );
  QSize size = event->size();
  vSplitter->resize( size );
//   gridLayoutWidget->setGeometry(
//     QRect(0, 0, size.width(), size.height() ) );
}
//
