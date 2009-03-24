/*========================================================================
 Copyright (c) INRIA - ASCLEPIOS Project (http://www-sop.inria.fr/asclepios).
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of INRIA or ASCLEPIOS, nor the names of any contributors
 may be used to endorse or promote products derived from this software 
 without specific prior written permission.
 
 * Modified source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 =========================================================================*/

/*=========================================================================
 Modifications were made by the GoFigure Dev. Team.
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
 
#include "QColorImagePageView.h"
#include "QSplitterchild.h"

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
#include <vtkOrientedGlyphContourRepresentation.h>

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

QColorImagePageView::QColorImagePageView( QWidget* parent ) : QWidget( parent )
{
  IsFullScreen = 0;
  SnapshotId = 1;

  Tag = QString( "QColorImagePageView" );
  Image = 0;

  Pool = vtkViewColorImage2DCollection::New();
  View3D = vtkViewColorImage3D::New();

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

QColorImagePageView::~QColorImagePageView()
{
  Pool->Delete();
  View3D->Delete();
}

void QColorImagePageView::setupUi( QWidget* parent )
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

void QColorImagePageView::retranslateUi(QWidget *parent)
{
  parent->setWindowTitle( this->Tag );
  Q_UNUSED(parent);
}

bool QColorImagePageView::BuildScreenshotFromImage( vtkImageData* image,
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

    screenshot->ShallowCopy( resample_output );
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

bool QColorImagePageView::BuildScreenshotFromRenderWindow(
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

void QColorImagePageView::SnapshotView( QVTKWidget* iWidget,
  const SnapshotImageType& iType,
  const QString& iBaseName )
{
  vtkImageData* image = vtkImageData::New();
  BuildScreenshotFromRenderWindow( iWidget->GetRenderWindow(),
        image, 0 );
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

void QColorImagePageView::SnapshotViewXY( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  SnapshotView( qvtkWidget_XY, iType, iBaseName );
}

void QColorImagePageView::SnapshotView2( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  SnapshotView( qvtkWidget_2, iType, iBaseName );
}
void QColorImagePageView::SnapshotView3( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  SnapshotView( qvtkWidget_3, iType, iBaseName );
}
void QColorImagePageView::SnapshotViewXYZ( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  SnapshotView( qvtkWidget_XYZ, iType, iBaseName );
}

void QColorImagePageView::SetFullScreenView( const int& iS )
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

int QColorImagePageView::GetFullScreenView( ) const
{
  return IsFullScreen;
}

double* QColorImagePageView::GetBackgroundColor()
{
  return this->Pool->GetItem( 0 )->GetBackground();
}

void QColorImagePageView::GetBackgroundColor( double& r,
  double& g, double& b )
{
  double* rgb = this->Pool->GetItem( 0 )->GetBackground();
  r = rgb[0];
  g = rgb[1];
  b = rgb[2];
}

void QColorImagePageView::SetBackgroundColor( const double& r,
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

void QColorImagePageView::SetBackgroundColor( double rgb[3] )
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

void QColorImagePageView::SetBackgroundColor( const QColor& iColor )
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

QVTKWidget* QColorImagePageView::GetActiveQVTKWidget( )
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


vtkViewColorImage* QColorImagePageView::GetActiveView()
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

void QColorImagePageView::SetLookupTable( vtkLookupTable* lut )
{
}

void QColorImagePageView::quadview()
{
   SetFullScreenView( 0 );
}

void QColorImagePageView::FullScreenViewXY ()
{
  SetFullScreenView( 1 );
}


void QColorImagePageView::FullScreenView2( )
{
  SetFullScreenView( 2 );
}


void QColorImagePageView::FullScreenView3( )
{
  SetFullScreenView( 3 );
}


void QColorImagePageView::FullScreenViewXYZ ( )
{
  SetFullScreenView( 4 );
}

void QColorImagePageView::SetView3DToTriPlanarMode()
{
  View3D->SetTriPlanarRenderingOn();
  View3D->SetVolumeRenderingOff();
  View3D->Render();
}
//
void QColorImagePageView::SetView3DToVolumeRenderingMode()
{
  View3D->SetTriPlanarRenderingOff();
  View3D->SetVolumeRenderingOn();
  View3D->Render();
}

void QColorImagePageView::SetImage( vtkImageData* input )
{
  if( !input )
    return;
  else
    {
    this->Image = input;

    vtkViewColorImage2D* View1 =
      vtkViewColorImage2D::New();
    View1->SetInput( this->Image );
    View1->SetViewConvention( vtkViewColorImage2D::VIEW_CONVENTION_NEUROLOGICAL );
    vtkRenderWindow* renwin1 = this->qvtkWidget_XY->GetRenderWindow( );
    View1->SetupInteractor( this->qvtkWidget_XY->GetInteractor() );
    View1->SetRenderWindow( renwin1 );
    View1->SetRenderer( renwin1->GetRenderers()->GetFirstRenderer() );
    View1->SetViewOrientation( vtkViewColorImage2D::VIEW_ORIENTATION_AXIAL );
    this->Pool->AddItem( View1 );
    this->View3D->Add2DPhantom( 0, View1->GetImageActor(), View1->GetSlicePlane() );

    int *range = View1->GetSliceRange();

    this->slider1->setMinimum( range[0] );
    this->slider1->setMaximum( range[1] );
    View1->Delete();

    vtkViewColorImage2D* View2 =
      vtkViewColorImage2D::New();
    View2->SetInput( this->Image );
    View2->SetViewConvention( vtkViewColorImage2D::VIEW_CONVENTION_NEUROLOGICAL );
    vtkRenderWindow* renwin2 = this->qvtkWidget_2->GetRenderWindow( );
    View2->SetRenderWindow( renwin2 );
    View2->SetRenderer( renwin2->GetRenderers()->GetFirstRenderer() );
    View2->SetupInteractor( this->qvtkWidget_2->GetInteractor() );
    View2->SetViewOrientation (vtkViewColorImage2D::VIEW_ORIENTATION_CORONAL);
    this->Pool->AddItem( View2 );
    this->View3D->Add2DPhantom( 1, View2->GetImageActor(), View2->GetSlicePlane() );

    range = View2->GetSliceRange();
    this->slider2->setMinimum( range[0] );
    this->slider2->setMaximum( range[1] );
    View2->Delete();

    vtkViewColorImage2D* View3 =
      vtkViewColorImage2D::New();
    View3->SetInput( this->Image );
    View3->SetViewConvention( vtkViewColorImage2D::VIEW_CONVENTION_NEUROLOGICAL );
    vtkRenderWindow* renwin3 = this->qvtkWidget_3->GetRenderWindow( );
    View3->SetRenderWindow( renwin3 );
    View3->SetRenderer( renwin3->GetRenderers()->GetFirstRenderer() );
    View3->SetupInteractor( this->qvtkWidget_3->GetInteractor() );
    View3->SetViewOrientation (vtkViewColorImage2D::VIEW_ORIENTATION_SAGITTAL);

    this->Pool->AddItem( View3 );
    this->View3D->Add2DPhantom( 2, View3->GetImageActor(), View3->GetSlicePlane() );

    range = View3->GetSliceRange();
    this->slider3->setMinimum( range[0] );
    this->slider3->setMaximum( range[1] );
    View3->Delete();

    int size[2] = {400, 400};
    this->Pool->SyncSetSize (size);

    vtkRenderWindow* renwin4 = this->qvtkWidget_XYZ->GetRenderWindow( );
    this->View3D->SetupInteractor( this->qvtkWidget_XYZ->GetInteractor() );
    this->View3D->SetInput( this->Image );
    this->View3D->SetVolumeRenderingOff();
    this->View3D->ResetCamera();
    this->Pool->SetExtraRenderWindow( renwin4 );

    this->Pool->Initialize();
    this->Pool->SyncSetShowAnnotations( true );
    this->Pool->SyncSetBackground( this->Pool->GetItem(0)->GetBackground() );
    this->Pool->SyncSetTextProperty( this->Pool->GetItem(0)->GetTextProperty());
    this->Pool->SyncRender();
    this->Pool->SyncReset();

    this->slider1->setValue( (this->slider1->minimum()+this->slider1->maximum())/2 );
    this->slider2->setValue( (this->slider2->minimum()+this->slider2->maximum())/2 );
    this->slider3->setValue( (this->slider3->minimum()+this->slider3->maximum())/2 );
    }
}

void QColorImagePageView::SetShowAnnotations( const bool& iState )
{
  this->Pool->SyncSetShowAnnotations( iState );
}

void QColorImagePageView::SetTag( const QString& iTag )
{
  this->Tag = iTag;
}

QString QColorImagePageView::GetTag( ) const
{
  return this->Tag;
}

void QColorImagePageView::SetCellId( const unsigned int& iId )
{
  this->CellId = iId;
}

unsigned int QColorImagePageView::GetCellId() const
{
  return this->CellId;
}

void QColorImagePageView::resizeEvent( QResizeEvent* event )
{
  QWidget::resizeEvent( event );
  vSplitter->resize( event->size() );
}


void QColorImagePageView::ValidateContour(
  const int& iId,
  const QColor& iColor,
  const bool& iSave )
{
}

void QColorImagePageView::ReinitializeContour( )
{
}

void QColorImagePageView::SetSlideView1( const int& iSlice )
{
  this->Pool->GetItem( 0 )->SetSlice( iSlice );
  this->Pool->SyncRender();
}

void QColorImagePageView::SetSlideView2( const int& iSlice )
{
  this->Pool->GetItem( 1 )->SetSlice( iSlice );
  this->Pool->SyncRender();
}

void QColorImagePageView::SetSlideView3( const int& iSlice )
{
  this->Pool->GetItem( 2 )->SetSlice( iSlice );
  this->Pool->SyncRender();
}
