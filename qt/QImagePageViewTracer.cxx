/*=========================================================================
 Authors: The GoFigure Dev. Team.
 while at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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

#include "QImagePageViewTracer.h"
#include "QSplitterchild.h"
#include "vtkViewImage2DCommand.h"

#include <QResizeEvent>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h>

#include <vtkImageClip.h>
#include <vtkImagePermute.h>
#include <vtkImageResample.h>
#include <vtkWindowToImageFilter.h>

#include <vtkBMPWriter.h>
#include <vtkPostScriptWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkTIFFWriter.h>

#include <vtkPolyDataWriter.h>

#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkMath.h>

QImagePageViewTracer::QImagePageViewTracer( QWidget* parent ) : QWidget( parent )
{
  IsFullScreen = 0;
  SnapshotId = 1;

  Tag = QString( "QImagePageViewTracer" );
  Image = 0;

  Pool = vtkViewImage2DWithContourWidgetCollection::New();
  View3D = vtkViewImage3D::New();

  vtkEventQtConnector = vtkEventQtSlotConnect::New();

  setupUi( this );
  QObject::connect( this->slider1, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSlideView1( int ) ) );
  QObject::connect( this->slider2, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSlideView2( int ) ) );
  QObject::connect( this->slider3, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSlideView3( int ) ) );

  QObject::connect( this->htSplitter, SIGNAL( splitterMoved( int, int ) ),
    this->hbSplitter, SLOT( moveSplitter( int, int ) ) );
  QObject::connect(this->hbSplitter, SIGNAL( splitterMoved( int, int ) ),
    this->htSplitter, SLOT( moveSplitter( int, int ) ) );

}

QImagePageViewTracer::~QImagePageViewTracer()
{
  delete htSplitter;
  delete hbSplitter;
  Pool->Delete();
  View3D->Delete();
  vtkEventQtConnector->Delete();
}

void QImagePageViewTracer::setupUi( QWidget* parent )
{
  if(parent->objectName().isEmpty())
    {
    parent->resize(800, 800);
    }

  QList< int > list_size;
  list_size.push_back( 10 );
  list_size.push_back( 10 );

  this->vSplitter  = new QSplitter( Qt::Vertical, parent );

  this->htSplitter = new QSplitterchild( this->vSplitter );
  this->hbSplitter = new QSplitterchild( this->vSplitter );

  this->vSplitter->addWidget(this->htSplitter);
  this->vSplitter->addWidget(this->hbSplitter);
  this->vSplitter->setSizes( list_size );
  this->vSplitter->resize( 800, 800 );

  this->qvtkWidget_XY = new QVTKWidget;
  this->slider1 = new QSlider( Qt::Vertical );
  this->LayOut1 = new QHBoxLayout;
  this->LayOut1->addWidget( this->qvtkWidget_XY );
  this->LayOut1->addWidget( this->slider1 );
  this->LayOutWidget1 = new QWidget;
  this->LayOutWidget1->setLayout( this->LayOut1 );
  this->htSplitter->addWidget( this->LayOutWidget1 );

  this->qvtkWidget_2 = new QVTKWidget;
  this->slider2 = new QSlider( Qt::Vertical );
  this->LayOut2 = new QHBoxLayout;
  this->LayOut2->addWidget( this->qvtkWidget_2 );
  this->LayOut2->addWidget( this->slider2 );
  this->LayOutWidget2 = new QWidget;
  this->LayOutWidget2->setLayout( this->LayOut2 );
  this->hbSplitter->addWidget( this->LayOutWidget2 );
//   this->htSplitter->setSizes( list_size );

  this->qvtkWidget_3 = new QVTKWidget;
  this->slider3 = new QSlider( Qt::Vertical );
  this->LayOut3 = new QHBoxLayout;
  this->LayOut3->addWidget( this->qvtkWidget_3 );
  this->LayOut3->addWidget( this->slider3 );
  this->LayOutWidget3 = new QWidget;
  this->LayOutWidget3->setLayout( this->LayOut3 );
  this->htSplitter->addWidget( this->LayOutWidget3 );

  this->qvtkWidget_XYZ = new QVTKWidget;
  this->Spacer = new QSpacerItem( 27, 10,
    QSizePolicy::Minimum, QSizePolicy::Minimum);
  this->LayOut4 = new QHBoxLayout;
  this->LayOut4->addWidget( this->qvtkWidget_XYZ );
  this->LayOut4->addItem( this->Spacer );
  this->LayOutWidget4 = new QWidget;
  this->LayOutWidget4->setLayout( this->LayOut4 );
  this->hbSplitter->addWidget( this->LayOutWidget4 );

  vtkRenderWindow* renwin4 = this->qvtkWidget_XYZ->GetRenderWindow( );
  View3D->SetRenderWindow( renwin4 );

  retranslateUi(parent);

  QMetaObject::connectSlotsByName(parent);
} // setupUi

void QImagePageViewTracer::retranslateUi(QWidget *parent)
{
  parent->setWindowTitle( this->Tag );
  Q_UNUSED(parent);
}

bool QImagePageViewTracer::BuildScreenshotFromImage( vtkImageData* image,
  vtkImageData* screenshot,
  int size )
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

  if( size != 0 )
    {
    vtkImageResample *resample = vtkImageResample::New();
    resample->SetInput(resample_input);
    resample->SetInterpolationModeToCubic();
    resample->SetDimensionality(2);

    // Create the screenshot

    factor = static_cast< double >( size ) /
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

bool QImagePageViewTracer::BuildScreenshotFromRenderWindow(
  vtkRenderWindow *win,
  vtkImageData* screenshot,
  int size )
{
  if( win && screenshot )
    {
    vtkWindowToImageFilter* filter = vtkWindowToImageFilter::New();
    filter->ShouldRerenderOff();
    filter->SetInput( win );
    filter->Update();
    bool res = this->BuildScreenshotFromImage( filter->GetOutput(),
      screenshot, size );
    filter->Delete();
    return res;
    }
  return false;
}

QString QImagePageViewTracer::SnapshotView( QVTKWidget* iWidget,
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
  return filename;
}

QString QImagePageViewTracer::SnapshotViewXY( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  return SnapshotView( qvtkWidget_XY, iType, iBaseName );
}

QString QImagePageViewTracer::SnapshotView2( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  return SnapshotView( qvtkWidget_2, iType, iBaseName );
}
QString QImagePageViewTracer::SnapshotView3( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  return SnapshotView( qvtkWidget_3, iType, iBaseName );
}
QString QImagePageViewTracer::SnapshotViewXYZ( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  return SnapshotView( qvtkWidget_XYZ, iType, iBaseName );
}

void QImagePageViewTracer::SetFullScreenView( const int& iS )
{

  if( this->Is2DImage ) return;

  if( IsFullScreen == iS )
    IsFullScreen = 0;
  else
    IsFullScreen = iS;

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

int QImagePageViewTracer::GetFullScreenView( ) const
{
  return IsFullScreen;
}

double* QImagePageViewTracer::GetBackgroundColor()
{
  return this->Pool->GetItem( 0 )->GetBackground();
}

void QImagePageViewTracer::GetBackgroundColor( double& r,
  double& g, double& b )
{
  double* rgb = this->Pool->GetItem( 0 )->GetBackground();
  r = rgb[0];
  g = rgb[1];
  b = rgb[2];
}

void QImagePageViewTracer::SetBackgroundColor( const double& r,
const double& g, const double& b )
{
  double textcolor[3];
  textcolor[0] = 1. - r;
  textcolor[1] = 1. - g;
  textcolor[2] = 1. - b;

  double rgb[3] = {r, g, b };

  this->Pool->SyncSetBackground( rgb );

  vtkTextProperty* property = this->Pool->GetItem( 0 )->GetTextProperty();
//   property->SetFontFamilyToArial();
//   property->SetFontSize( 14 );
  property->SetColor( textcolor );
  this->Pool->SyncSetTextProperty( property );
}

void QImagePageViewTracer::SetBackgroundColor( double rgb[3] )
{
  double textcolor[3];
  textcolor[0] = 1. - rgb[0];
  textcolor[1] = 1. - rgb[1];
  textcolor[2] = 1. - rgb[2];

  this->Pool->SyncSetBackground( rgb );

  vtkTextProperty* property = this->Pool->GetItem( 0 )->GetTextProperty();
//   property->SetFontFamilyToArial();
//   property->SetFontSize( 14 );
  property->SetColor( textcolor );
  this->Pool->SyncSetTextProperty( property );
}

void QImagePageViewTracer::SetBackgroundColor( const QColor& iColor )
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

  this->Pool->SyncSetBackground( rgb );

  vtkTextProperty* property = this->Pool->GetItem( 0 )->GetTextProperty();
//   property->SetFontFamilyToArial();
//   property->SetFontSize( 14 );
  property->SetColor( textcolor );
  this->Pool->SyncSetTextProperty( property );
}

QVTKWidget* QImagePageViewTracer::GetActiveQVTKWidget( )
{
  if( !this->Is2DImage )
  {
    switch(this->IsFullScreen)
    {
    case 1 :
      return qvtkWidget_XY;
    case 2 :
      return qvtkWidget_2;
    case 3 :
      return qvtkWidget_3;
    default :
      return qvtkWidget_XYZ;
    }
  }
  else
  {
    return qvtkWidget_XY;
  }
}


vtkViewImage* QImagePageViewTracer::GetActiveView()
{
  if( !this->Is2DImage )
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
  else
  {
    return this->Pool->GetItem(0);
  }
}

void QImagePageViewTracer::SetLookupTable( vtkLookupTable* lut )
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

void QImagePageViewTracer::quadview()
{
  if( !this->Is2DImage )
   SetFullScreenView( 0 );
}

void QImagePageViewTracer::FullScreenViewXY ()
{
  if( !this->Is2DImage )
    SetFullScreenView( 1 );
}


void QImagePageViewTracer::FullScreenView2( )
{
  if( !this->Is2DImage )
    SetFullScreenView( 2 );
}


void QImagePageViewTracer::FullScreenView3( )
{
  if( !this->Is2DImage )
    SetFullScreenView( 3 );
}


void QImagePageViewTracer::FullScreenViewXYZ ( )
{
  if( !this->Is2DImage )
    SetFullScreenView( 4 );
}

void QImagePageViewTracer::SetView3DToTriPlanarMode()
{
  if( !this->Is2DImage )
  {
    View3D->SetTriPlanarRenderingOn();
    View3D->SetVolumeRenderingOff();
    View3D->Render();
  }
}
//
void QImagePageViewTracer::SetView3DToVolumeRenderingMode()
{
  if( !this->Is2DImage )
  {
    View3D->SetTriPlanarRenderingOff();
    View3D->SetVolumeRenderingOn();
    View3D->Render();
  }
}

#ifdef MegaVTK_USE_ITK
template< class TImage >
void QImagePageViewTracer::SetITKImage( TImage::Pointer itkImage )
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

void QImagePageViewTracer::Set3DImage( vtkImageData* input )
{
  vtkViewImage2DWithContourWidget* View1 =
    vtkViewImage2DWithContourWidget::New();
  View1->SetInput( this->Image );

  vtkRenderWindow* renwin1 = this->qvtkWidget_XY->GetRenderWindow( );
  //     renwin1->GetRenderers()->RemoveAllItems();
  View1->SetupInteractor( this->qvtkWidget_XY->GetInteractor() );
  View1->SetRenderWindow( renwin1 );
  View1->SetRenderer( renwin1->GetRenderers()->GetFirstRenderer() );

  View1->SetViewOrientation( vtkViewImage2D::VIEW_ORIENTATION_AXIAL );
  View1->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );

  //     View1->GetTextProperty()->SetFontFamilyToArial();
  //     View1->GetTextProperty()->SetFontSize( 14 );

  //     View1->SetContourWidgetInteractionOn();
  this->Pool->AddItem( View1 );
  this->View3D->Add2DPhantom( 0, View1->GetImageActor(), View1->GetSlicePlane() );

  int *range = View1->GetSliceRange();

  this->slider1->setMinimum( range[0] );
  this->slider1->setMaximum( range[1] );

  // Event connection between vtk and qt
  // when SliceMoveEvent occurs in the XY View, Slider1 moves.
  vtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View1->GetInteractorStyle() ),
    vtkViewImage2DCommand::SliceMoveEvent,
    this, SLOT( MoveSlider1() ) );

  // Event connection between vtk and qt
  // when RequestedPositionEvent occurs in the XY View (double-click),
  // Slider2 and Slider3 move.
  vtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View1->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSlider2() ) );

  vtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View1->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSlider3() ) );

  View1->Delete();

  vtkViewImage2DWithContourWidget* View2 =
    vtkViewImage2DWithContourWidget::New();
  View2->SetInput( this->Image );

  vtkRenderWindow* renwin2 = this->qvtkWidget_2->GetRenderWindow( );
  View2->SetRenderWindow( renwin2 );
  View2->SetRenderer( renwin2->GetRenderers()->GetFirstRenderer() );
  View2->SetupInteractor( this->qvtkWidget_2->GetInteractor() );

  View2->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
  View2->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_CORONAL);

  //     View2->GetTextProperty()->SetFontFamilyToArial();
  //     View2->GetTextProperty()->SetFontSize( 14 );
  //     View2->SetContourWidgetInteractionOn();

  this->Pool->AddItem( View2 );
  this->View3D->Add2DPhantom( 1, View2->GetImageActor(), View2->GetSlicePlane() );

  range = View2->GetSliceRange();
  this->slider2->setMinimum( range[0] );
  this->slider2->setMaximum( range[1] );
  // Event connection between vtk and qt
  // when SliceMoveEvent occurs in the XY View, Slider1 moves.
  vtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View2->GetInteractorStyle() ),
    vtkViewImage2DCommand::SliceMoveEvent,
    this, SLOT( MoveSlider2() ) );

  // Event connection between vtk and qt
  // when RequestedPositionEvent occurs in the XY View (double-click),
  // Slider2 and Slider3 move.
  vtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View2->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSlider1() ) );

  vtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View2->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSlider3() ) );

  View2->Delete();

  vtkViewImage2DWithContourWidget* View3 =
    vtkViewImage2DWithContourWidget::New();
  View3->SetInput( this->Image );

  vtkRenderWindow* renwin3 = this->qvtkWidget_3->GetRenderWindow( );
  View3->SetRenderWindow( renwin3 );
  View3->SetRenderer( renwin3->GetRenderers()->GetFirstRenderer() );
  View3->SetupInteractor( this->qvtkWidget_3->GetInteractor() );

  View3->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
  View3->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL);

  //     View3->GetTextProperty()->SetFontFamilyToArial();
  //     View3->GetTextProperty()->SetFontSize( 14 );
  //     View3->SetContourWidgetInteractionOn();

  this->Pool->AddItem( View3 );
  this->View3D->Add2DPhantom( 2, View3->GetImageActor(), View3->GetSlicePlane() );

  range = View3->GetSliceRange();
  this->slider3->setMinimum( range[0] );
  this->slider3->setMaximum( range[1] );

  // Event connection between vtk and qt
  // when SliceMoveEvent occurs in the XY View, Slider1 moves.
  vtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View3->GetInteractorStyle() ),
    vtkViewImage2DCommand::SliceMoveEvent,
    this, SLOT( MoveSlider3() ) );

  // Event connection between vtk and qt
  // when RequestedPositionEvent occurs in the XY View (double-click),
  // Slider2 and Slider3 move.
  vtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View3->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSlider1() ) );

  vtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View3->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSlider2() ) );

  View3->Delete();

  int size[2] = {400, 400};
  this->Pool->SyncSetSize (size);

  vtkRenderWindow* renwin4 = this->qvtkWidget_XYZ->GetRenderWindow( );
  //     this->View3D->SetRenderWindow( renwin4 );
  this->View3D->SetupInteractor( this->qvtkWidget_XYZ->GetInteractor() );
  this->View3D->SetInput( this->Image );
  this->View3D->SetVolumeRenderingOff();
  this->View3D->SetShowScalarBar( false );
  this->View3D->ResetCamera();
  this->Pool->SetExtraRenderWindow( renwin4 );

  this->Pool->Initialize();
  this->Pool->SyncSetShowAnnotations( true );
  this->Pool->SyncSetShowScalarBar( false );
  this->Pool->SyncSetBackground( this->Pool->GetItem(0)->GetBackground() );
  this->Pool->SyncSetTextProperty( this->Pool->GetItem(0)->GetTextProperty());
  //     this->Pool->SyncMaskImage();
  this->Pool->SyncRender();
  this->Pool->SyncReset();

  this->slider1->setValue( (this->slider1->minimum()+this->slider1->maximum())/2 );
  this->slider2->setValue( (this->slider2->minimum()+this->slider2->maximum())/2 );
  this->slider3->setValue( (this->slider3->minimum()+this->slider3->maximum())/2 );
}

void QImagePageViewTracer::Set2DImage( vtkImageData* input )
{
  vtkViewImage2DWithContourWidget* View1 =
    vtkViewImage2DWithContourWidget::New();
  View1->SetInput( this->Image );

  vtkRenderWindow* renwin1 = this->qvtkWidget_XY->GetRenderWindow( );
  //     renwin1->GetRenderers()->RemoveAllItems();
  View1->SetupInteractor( this->qvtkWidget_XY->GetInteractor() );
  View1->SetRenderWindow( renwin1 );
  View1->SetRenderer( renwin1->GetRenderers()->GetFirstRenderer() );

  View1->SetViewOrientation( vtkViewImage2D::VIEW_ORIENTATION_AXIAL );
  View1->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );

  //     View1->GetTextProperty()->SetFontFamilyToArial();
  //     View1->GetTextProperty()->SetFontSize( 14 );

  //     View1->SetContourWidgetInteractionOn();
  this->Pool->AddItem( View1 );
  View1->Delete();

  this->Pool->Initialize();
  this->Pool->SyncSetShowAnnotations( true );
  this->Pool->SyncSetShowScalarBar( false );
  this->Pool->SyncSetBackground( this->Pool->GetItem(0)->GetBackground() );
  this->Pool->SyncSetTextProperty( this->Pool->GetItem(0)->GetTextProperty());
  //     this->Pool->SyncMaskImage();
  this->Pool->SyncRender();
  this->Pool->SyncReset();

  LayOutWidget2->hide();
  LayOutWidget3->hide();
  LayOutWidget4->hide();
}

void QImagePageViewTracer::SetImage( vtkImageData* input )
{
  if( !input )
    return;
  else
  {
    this->Image = input;

    int extent[6];
    this->Image->GetExtent( extent );

    this->Is2DImage = ( extent[0] == extent[1] ) || ( extent[2] == extent[3] )
      || ( extent[4] == extent[5] );

    if( !this->Is2DImage )
    {
      Set3DImage( input );
    }
    else
    {
      Set2DImage( input );
    }
  }
}

void QImagePageViewTracer::SetShowScalarBar( const bool& state )
{
  this->Pool->SyncSetShowScalarBar(state);
//   this->View3D->SetShowScalarBar( state );
  this->Pool->SyncRender();
}

void QImagePageViewTracer::SetShowAnnotations( const bool& iState )
{
  this->Pool->SyncSetShowAnnotations( iState );
}

void QImagePageViewTracer::SetColorWindow( const double& iValue )
{
  this->Pool->SyncSetColorWindow( iValue );
}

void QImagePageViewTracer::SetColorLevel( const double& iValue )
{
  this->Pool->SyncSetColorLevel( iValue );
}

void QImagePageViewTracer::SetTag( const QString& iTag )
{
  this->Tag = iTag;
}

QString QImagePageViewTracer::GetTag( ) const
{
  return this->Tag;
}

void QImagePageViewTracer::SetCellId( const unsigned int& iId )
{
  this->CellId = iId;
}

unsigned int QImagePageViewTracer::GetCellId() const
{
  return this->CellId;
}

bool QImagePageViewTracer::GetTracerStatus() const
{
  if( this->Pool->GetNumberOfItems() )
    return this->Pool->GetItem( 0 )->GetContourWidgetEnabled( );
  else
    return false;
}

void QImagePageViewTracer::SetTracerON()
{
  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++ )
  {
    this->Pool->GetItem( i )->SetContourWidgetInteractionOn();
  }
}

void QImagePageViewTracer::SetTracerOFF()
{
  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++ )
  {
    this->Pool->GetItem( i )->SetContourWidgetInteractionOff();
  }
}

void QImagePageViewTracer::SetTracer( const bool& iState )
{
  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++ )
  {
    this->Pool->GetItem( i )->SetContourWidgetInteraction( iState );
  }
}

void QImagePageViewTracer::resizeEvent( QResizeEvent* event )
{
  QWidget::resizeEvent( event );
  vSplitter->resize( event->size() );
}


// void QImagePageViewTracer::ValidateContour( const int& iId )
void QImagePageViewTracer::ValidateContour(
  const int& iId,
  const QColor& iColor,
  const bool& iSave )
{
  double rgb[3];
  rgb[0] = static_cast< double >( iColor.red() ) / 255.;
  rgb[1] = static_cast< double >( iColor.green() ) / 255.;
  rgb[2] = static_cast< double >( iColor.blue() ) / 255.;

  vtkOrientedGlyphContourRepresentation* contour_rep;
  vtkPolyData* contour;

  vtkProperty* contour_property = vtkProperty::New();
  contour_property->SetRepresentationToWireframe();
  contour_property->SetColor( rgb );
  contour_property->SetLineWidth( 3. );

  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++ )
   {
    contour_rep = this->Pool->GetItem( i )->GetContourRepresentation();
    contour = contour_rep->GetContourRepresentationAsPolyData( );

    if( contour )
    {
      if( contour->GetNumberOfPoints() > 2 )
      {
        double bounds[6];
        contour->GetBounds( bounds );

        double Min[3], Max[3];
        Min[0] = bounds[0];
        Max[0] = bounds[1];
        Min[1] = bounds[2];
        Max[1] = bounds[3];
        Min[2] = bounds[4];
        Max[2] = bounds[5];

        int* min_idx = this->Pool->GetItem( i )->GetImageCoordinatesFromWorldCoordinates( Min );
        int* max_idx = this->Pool->GetItem( i )->GetImageCoordinatesFromWorldCoordinates( Max );

        std::cout <<"Min = [" <<min_idx[0] <<" " <<min_idx[1] <<" " <<min_idx[2] <<"]" <<std::endl;
        std::cout <<"Max = [" <<max_idx[0] <<" " <<max_idx[1] <<" " <<max_idx[2] <<"]" <<std::endl;

        if( iSave )
        {
          QString identifier = QString( "_id%1" ).arg( iId );
          QString MinString = QString( "_m%1_%2_%3" ).arg( min_idx[0] ).arg( min_idx[1] ).arg( min_idx[2] );
          QString MaxString = QString( "_M%1_%2_%3" ).arg( max_idx[0] ).arg( max_idx[1] ).arg( max_idx[2] );

          QString filename = QString( "contour%1%2%3%4" )
            .arg( identifier ).arg( MinString ).arg( MaxString ).arg( ".vtk" );

          vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
          writer->SetInput( contour );
          writer->SetFileName( filename.toAscii().constData() );
          writer->Write();

          writer->Delete();
        }

        vtkPolyData* contour_copy = vtkPolyData::New();
        contour_copy->ShallowCopy( contour );

        for( int j = 0; j < this->Pool->GetNumberOfItems(); j++ )
        {
          this->Pool->GetItem( j )->AddDataSet( contour_copy, contour_property, true );
        }
        contour_copy->Delete();
//         this->Pool->GetItem( i )->GetContourWidget()->Initialize( 0 );
      }
    }

  }
  contour_property->Delete();
  this->Pool->SyncRender();
//   this->Pool->SyncMaskImage();
}
//


template< class TContourContainer,
          class TPropertyContainer >
void QImagePageViewTracer::AddContours( TContourContainer& iContours,
    TPropertyContainer& iProperty,
    const bool& iIntersection )
{
  this->Pool->SyncAddContours( iContours, iProperty, iIntersection );
}

template< class TPolyDataContainer >
void QImagePageViewTracer::RemoveContours( TPolyDataContainer& iContours )
{
  this->Pool->SyncRemoveContours( iContours );
}

void QImagePageViewTracer::ReinitializeContour( )
{
  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++ )
  {
    this->Pool->GetItem( i )->GetContourWidget()->Initialize( 0 );
  }
  this->Pool->SyncRender();
}

void QImagePageViewTracer::SetSlideView1( const int& iSlice )
{
  this->Pool->GetItem( 0 )->SetSlice( iSlice );
  this->Pool->SyncRender();
}

void QImagePageViewTracer::SetSlideView2( const int& iSlice )
{
  this->Pool->GetItem( 1 )->SetSlice( iSlice );
  this->Pool->SyncRender();
}

void QImagePageViewTracer::SetSlideView3( const int& iSlice )
{
  this->Pool->GetItem( 2 )->SetSlice( iSlice );
  this->Pool->SyncRender();
}

void QImagePageViewTracer::MoveSlider1( )
{
  this->slider1->setValue( this->Pool->GetItem( 0 )->GetSlice() );
}

void QImagePageViewTracer::MoveSlider2( )
{
  this->slider2->setValue( this->Pool->GetItem( 1 )->GetSlice() );
}

void QImagePageViewTracer::MoveSlider3( )
{
  this->slider3->setValue( this->Pool->GetItem( 2 )->GetSlice() );
}
