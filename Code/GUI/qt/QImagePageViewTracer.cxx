/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

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
#include <QMessageBox>

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
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkMath.h>
#include <vtkPlane.h>
#include <qsettings.h>

#include "vtkPolyDataMySQLTextWriter.h"
#include "vtkPolyDataMySQLTextReader.h"

#include "GoDBFigureRow.h"
#include "GoDBRecordSet.h"

void QImagePageViewTracer::SetupViewGivenQVTKWidget(
  vtkViewImage2DWithContourWidget* iView,
  QVTKWidget* iWidget )
{
  vtkRenderWindow* renwin = iWidget->GetRenderWindow( );
  iView->SetRenderWindow( renwin );
  iView->SetRenderer( renwin->GetRenderers()->GetFirstRenderer() );
  iView->SetupInteractor( iWidget->GetInteractor() );
}

//-------------------------------------------------------------------------
QImagePageViewTracer::QImagePageViewTracer( QWidget* iiParent ) :
  QWidget( iiParent )
{
  IsFullScreen = 0;
  SnapshotId = 1;
  IsVolumeRendering = false;
  CellId = 0;
  ContourId = 0;

  Tag = QString( "QImagePageViewTracer" );
  Image = 0;

  m_DBExperimentID = -1;
  m_TimePoint      = -1;
  Is2DImage = false;

  VtkEventQtConnector = vtkEventQtSlotConnect::New();

  setupUi( this );
  QObject::connect( this->Slider1, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSlideView1( int ) ) );
  QObject::connect( this->Slider2, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSlideView2( int ) ) );
  QObject::connect( this->Slider3, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetSlideView3( int ) ) );

  QObject::connect( this->HtSplitter, SIGNAL( splitterMoved( int, int ) ),
    this->HbSplitter, SLOT( moveSplitter( int, int ) ) );
  QObject::connect(this->HbSplitter, SIGNAL( splitterMoved( int, int ) ),
    this->HtSplitter, SLOT( moveSplitter( int, int ) ) );

  Pool = vtkViewImage2DWithContourWidgetCollection::New();
  View3D = vtkViewImage3D::New();

  vtkViewImage2DWithContourWidget* View1 =
    vtkViewImage2DWithContourWidget::New();
  View1->SetViewOrientation( vtkViewImage2D::VIEW_ORIENTATION_AXIAL );
  View1->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );

  this->SetupViewGivenQVTKWidget( View1, this->QvtkWidget_XY );

  this->Pool->AddItem( View1 );
  View1->Delete();

  vtkViewImage2DWithContourWidget* View2 =
    vtkViewImage2DWithContourWidget::New();
  View2->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
  View2->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_CORONAL);

  this->SetupViewGivenQVTKWidget( View2, this->QvtkWidget_2 );

  this->Pool->AddItem( View2 );
  View2->Delete();

  vtkViewImage2DWithContourWidget* View3 =
    vtkViewImage2DWithContourWidget::New();
  View3->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
  View3->SetViewOrientation( vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL );

  this->SetupViewGivenQVTKWidget( View3, this->QvtkWidget_3 );

  this->Pool->AddItem( View3 );
  View3->Delete();

  vtkRenderWindow* renwin4 = this->QvtkWidget_XYZ->GetRenderWindow( );
  this->View3D->SetRenderWindow( renwin4 );
  this->View3D->SetupInteractor( this->QvtkWidget_XYZ->GetInteractor() );
  this->Pool->SetExtraRenderWindow( renwin4 );

  this->Handle.resize( this->Pool->GetNumberOfItems() );
  this->SeedRep.resize( this->Pool->GetNumberOfItems() );
  this->SeedWidget.resize( this->Pool->GetNumberOfItems() );

  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++)
    {
    this->Handle[i] = vtkConstrainedPointHandleRepresentation::New();
    this->Handle[i]->GetProperty()->SetColor(1,0,0);

    this->SeedRep[i] = vtkSeedRepresentation::New();
    this->SeedRep[i]->SetHandleRepresentation(this->Handle[i]);

    this->SeedWidget[i] = vtkSeedWidget::New();
    this->SeedWidget[i]->SetPriority( 10.0 );
    this->SeedWidget[i]->SetRepresentation( this->SeedRep[i] );
    }

  this->Handle[0]->SetProjectionNormal( vtkViewImage2D::VIEW_ORIENTATION_AXIAL );
  this->Handle[1]->SetProjectionNormal( vtkViewImage2D::VIEW_ORIENTATION_CORONAL );
  this->Handle[2]->SetProjectionNormal( vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL );

  this->SeedWidget[0]->SetInteractor( this->QvtkWidget_XY->GetInteractor() );
  this->SeedWidget[1]->SetInteractor( this->QvtkWidget_2->GetInteractor() );
  this->SeedWidget[2]->SetInteractor( this->QvtkWidget_3->GetInteractor() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QImagePageViewTracer::~QImagePageViewTracer()
{
  delete HtSplitter;
  delete HbSplitter;

  for( unsigned int i = 0; i < this->SeedWidget.size(); i++ )
    {
    Handle[i]->Delete();
    SeedRep[i]->Delete();
    SeedWidget[i]->Delete();
    }

  ContourIdActorMapIterator it = m_ContourIdActorMap.begin();
  ContourIdActorMapIterator end = m_ContourIdActorMap.end();

  while( it != end )
    {
    (*it).second.Actor->Delete();
//     (*it).second.Nodes->Delete();
    ++it;
    }

  m_ContourIdActorMap.clear();
  m_ActorContourIdMap.clear();

  Pool->Delete();
  View3D->Delete();
  VtkEventQtConnector->Delete();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::setupUi( QWidget* iParent )
{
  if(iParent->objectName().isEmpty())
    {
    iParent->resize(800, 800);
    }

  QList< int > list_size;
  list_size.push_back( 10 );
  list_size.push_back( 10 );

  this->VSplitter  = new QSplitter( Qt::Vertical, iParent );

  this->HtSplitter = new QSplitterchild( this->VSplitter );
  this->HbSplitter = new QSplitterchild( this->VSplitter );

  this->VSplitter->addWidget(this->HtSplitter);
  this->VSplitter->addWidget(this->HbSplitter);
  this->VSplitter->setSizes( list_size );
  this->VSplitter->resize( 800, 800 );

  this->QvtkWidget_XY = new QVTKWidget;
  this->Slider1 = new QSlider( Qt::Vertical );
  this->LayOut1 = new QHBoxLayout;
  this->LayOut1->addWidget( this->QvtkWidget_XY );
  this->LayOut1->addWidget( this->Slider1 );
  this->LayOutWidget1 = new QWidget;
  this->LayOutWidget1->setLayout( this->LayOut1 );
  this->HtSplitter->addWidget( this->LayOutWidget1 );

  this->QvtkWidget_2 = new QVTKWidget;
  this->Slider2 = new QSlider( Qt::Vertical );
  this->LayOut2 = new QHBoxLayout;
  this->LayOut2->addWidget( this->QvtkWidget_2 );
  this->LayOut2->addWidget( this->Slider2 );
  this->LayOutWidget2 = new QWidget;
  this->LayOutWidget2->setLayout( this->LayOut2 );
  this->HbSplitter->addWidget( this->LayOutWidget2 );
//   this->HtSplitter->setSizes( list_size );

  this->QvtkWidget_3 = new QVTKWidget;
  this->Slider3 = new QSlider( Qt::Vertical );
  this->LayOut3 = new QHBoxLayout;
  this->LayOut3->addWidget( this->QvtkWidget_3 );
  this->LayOut3->addWidget( this->Slider3 );
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
void QImagePageViewTracer::retranslateUi(QWidget *iParent)
{
  iParent->setWindowTitle( this->Tag );
  Q_UNUSED(iParent);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QImagePageViewTracer::BuildScreenshotFromImage( vtkImageData* image,
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
bool QImagePageViewTracer::BuildScreenshotFromRenderWindow(
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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QImagePageViewTracer::SnapshotViewXY( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  return SnapshotView( QvtkWidget_XY, iType, iBaseName );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QImagePageViewTracer::SnapshotView2( const SnapshotImageType& iType,
    const QString& iBaseName )
{
  return SnapshotView( QvtkWidget_2, iType, iBaseName );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QImagePageViewTracer::SnapshotView3(
    const SnapshotImageType& iType,
    const QString& iBaseName )
{
  return SnapshotView( QvtkWidget_3, iType, iBaseName );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QImagePageViewTracer::SnapshotViewXYZ(
  const SnapshotImageType& iType,
  const QString& iBaseName )
{
  return SnapshotView( QvtkWidget_XYZ, iType, iBaseName );
}
//------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetFullScreenView( const int& iS )
{
  if( this->Is2DImage )
    {
    return;
    }

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
int QImagePageViewTracer::GetFullScreenView( ) const
{
  return IsFullScreen;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QImagePageViewTracer::GetVolumeRendering( ) const
{
  return IsVolumeRendering;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
double* QImagePageViewTracer::GetBackgroundColor()
{
  return this->Pool->GetItem( 0 )->GetBackground();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::GetBackgroundColor( double& r,
  double& g, double& b )
{
  double* rgb = this->Pool->GetItem( 0 )->GetBackground();
  r = rgb[0];
  g = rgb[1];
  b = rgb[2];
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetBackgroundColor( const double& r,
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
void QImagePageViewTracer::SetBackgroundColor( double rgb[3] )
{
  double textcolor[3];
  textcolor[0] = 1. - rgb[0];
  textcolor[1] = 1. - rgb[1];
  textcolor[2] = 1. - rgb[2];

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
QVTKWidget* QImagePageViewTracer::GetActiveQVTKWidget( )
{
  if( !this->Is2DImage )
    {
    switch(this->IsFullScreen)
      {
      case 1 :
        return QvtkWidget_XY;
      case 2 :
        return QvtkWidget_2;
      case 3 :
        return QvtkWidget_3;
      default :
        return QvtkWidget_XYZ;
      }
    }
  else
    {
    return QvtkWidget_XY;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::quadview()
{
  if( !this->Is2DImage )
    {
    SetFullScreenView( 0 );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::FullScreenViewXY ()
{
  if( !this->Is2DImage )
    {
    SetFullScreenView( 1 );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::FullScreenView2( )
{
  if( !this->Is2DImage )
    {
    SetFullScreenView( 2 );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::FullScreenView3( )
{
  if( !this->Is2DImage )
    {
    SetFullScreenView( 3 );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::FullScreenViewXYZ ( )
{
  if( !this->Is2DImage )
    {
    SetFullScreenView( 4 );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetView3DToTriPlanarMode()
{
  if( !this->Is2DImage )
    {
    View3D->SetTriPlanarRenderingOn();
    View3D->SetVolumeRenderingOff();
    View3D->Render();
    IsVolumeRendering = false;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetView3DToVolumeRenderingMode()
{
  if( !this->Is2DImage )
    {
    View3D->SetTriPlanarRenderingOff();
    View3D->SetVolumeRenderingOn();
    View3D->Render();
    IsVolumeRendering = true;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
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
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::Set3DImage( vtkImageData* iInput )
{
  vtkViewImage2DWithContourWidget* View1 = this->Pool->GetItem( 0 );
  View1->SetInput( iInput );

  this->View3D->Add2DPhantom(
    0, View1->GetImageActor(), View1->GetSlicePlane() );

  int *range = View1->GetSliceRange();

  this->Slider1->setMinimum( range[0] );
  this->Slider1->setMaximum( range[1] );

  // Event connection between vtk and qt
  // when SliceMoveEvent occurs in the XY View, Slider1 moves.
  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View1->GetInteractorStyle() ),
    vtkViewImage2DCommand::SliceMoveEvent,
    this, SLOT( MoveSlider1() ) );

  // Event connection between vtk and qt
  // when RequestedPositionEvent occurs in the XY View (double-click),
  // Slider2 and Slider3 move.
  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View1->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSlider2() ) );

  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View1->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSlider3() ) );

  vtkViewImage2DWithContourWidget* View2 = this->Pool->GetItem( 1 );
  View2->SetInput( iInput );

  this->View3D->Add2DPhantom(
    1, View2->GetImageActor(), View2->GetSlicePlane() );

  range = View2->GetSliceRange();
  this->Slider2->setMinimum( range[0] );
  this->Slider2->setMaximum( range[1] );
  // Event connection between vtk and qt
  // when SliceMoveEvent occurs in the XY View, Slider1 moves.
  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View2->GetInteractorStyle() ),
    vtkViewImage2DCommand::SliceMoveEvent,
    this, SLOT( MoveSlider2() ) );

  // Event connection between vtk and qt
  // when RequestedPositionEvent occurs in the XY View (double-click),
  // Slider2 and Slider3 move.
  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View2->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSlider1() ) );

  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View2->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSlider3() ) );

  vtkViewImage2DWithContourWidget* View3 = this->Pool->GetItem( 2 );
  View3->SetInput( iInput );

  this->View3D->Add2DPhantom(
    2, View3->GetImageActor(), View3->GetSlicePlane() );

  range = View3->GetSliceRange();
  this->Slider3->setMinimum( range[0] );
  this->Slider3->setMaximum( range[1] );

  // Event connection between vtk and qt
  // when SliceMoveEvent occurs in the XY View, Slider1 moves.
  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View3->GetInteractorStyle() ),
    vtkCommand::ModifiedEvent,
    this, SLOT( MoveSlider3() ) );

  // Event connection between vtk and qt
  // when RequestedPositionEvent occurs in the XY View (double-click),
  // Slider2 and Slider3 move.
  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View3->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSlider1() ) );

  VtkEventQtConnector->Connect(
    reinterpret_cast< vtkObject* >( View3->GetInteractorStyle() ),
    vtkViewImage2DCommand::RequestedPositionEvent,
    this, SLOT( MoveSlider2() ) );


  this->View3D->SetInput( iInput );
  this->View3D->SetVolumeRenderingOff();
  this->View3D->SetTriPlanarRenderingOn();
  this->View3D->SetShowScalarBar( false );
  this->View3D->ResetCamera();

  this->Pool->Initialize();
  this->Pool->InitializeAllObservers();
  this->Pool->SyncSetBackground( this->Pool->GetItem(0)->GetBackground() );
  this->Pool->SyncSetShowAnnotations( true );

  for( int i = 0; i < 3; i++ )
    {
    this->Pool->GetItem(i)->GetTextProperty()->SetFontFamilyToArial();
    this->Pool->GetItem(i)->GetTextProperty()->SetFontSize( 14 );
    }

  this->Handle[0]->SetPosition( View1->GetImageActor()->GetCenter() );
  this->Handle[0]->SetProjectionPosition(
    View1->GetImageActor()->GetCenter()[vtkViewImage2D::VIEW_ORIENTATION_AXIAL] );
  this->Handle[1]->SetPosition( View2->GetImageActor()->GetCenter() );
  this->Handle[1]->SetProjectionPosition(
    View2->GetImageActor()->GetCenter()[vtkViewImage2D::VIEW_ORIENTATION_CORONAL] );
  this->Handle[2]->SetPosition( View3->GetImageActor()->GetCenter() );
  this->Handle[2]->SetProjectionPosition(
    View3->GetImageActor()->GetCenter()[vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL] );

  double bounds[6];
  iInput->GetBounds( bounds );

  vtkPlane* p1 = vtkPlane::New();
  p1->SetOrigin( bounds[0], bounds[2], bounds[4] );
  p1->SetNormal( 1.0, 0.0, 0.0 );

  vtkPlane* p2 = vtkPlane::New();
  p2->SetOrigin( bounds[0], bounds[2], bounds[4] );
  p2->SetNormal( 0.0, 0.0, 1.0 );

  vtkPlane *p3 = vtkPlane::New();
  p3->SetOrigin( bounds[1], bounds[3], bounds[5] );
  p3->SetNormal( -1.0, 0.0, 0.0 );

  vtkPlane *p4 = vtkPlane::New();
  p4->SetOrigin( bounds[1], bounds[3], bounds[5] );
  p4->SetNormal( 0.0, 0.0, -1.0 );

  vtkPlane *p5 = vtkPlane::New();
  p5->SetOrigin( bounds[0], bounds[2], bounds[4] );
  p5->SetNormal( 0.0, 1.0, 0.0 );

  vtkPlane *p6 = vtkPlane::New();
  p6->SetOrigin( bounds[1], bounds[3], bounds[5] );
  p6->SetNormal( 0.0, -1.0, 0.0 );

  for( int i = 0; i < 3; i++ )
    {
    this->Handle[i]->AddBoundingPlane( p1 );
    this->Handle[i]->AddBoundingPlane( p2 );
    this->Handle[i]->AddBoundingPlane( p3 );
    this->Handle[i]->AddBoundingPlane( p4 );
    this->Handle[i]->AddBoundingPlane( p5 );
    this->Handle[i]->AddBoundingPlane( p6 );
    }

  p1->Delete();
  p2->Delete();
  p3->Delete();
  p4->Delete();
  p5->Delete();
  p6->Delete();

  this->Pool->SyncSetShowScalarBar( false );
  this->Pool->SyncRender();
  this->Pool->SyncReset();

  this->Slider1->setValue( (this->Slider1->minimum()+this->Slider1->maximum())/2 );
  this->Slider2->setValue( (this->Slider2->minimum()+this->Slider2->maximum())/2 );
  this->Slider3->setValue( (this->Slider3->minimum()+this->Slider3->maximum())/2 );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::Set2DImage( vtkImageData* iInput )
{
  vtkViewImage2DWithContourWidget* View1 =
    this->Pool->GetItem( 0 );
  View1->SetInput( iInput );

  View1->GetTextProperty()->SetFontFamilyToArial();
  View1->GetTextProperty()->SetFontSize( 20 );

  this->Pool->Initialize();
  this->Pool->InitializeAllObservers();
  this->Pool->SyncSetShowAnnotations( true );
  this->Pool->SyncSetShowScalarBar( false );
  this->Pool->SyncSetBackground( this->Pool->GetItem(0)->GetBackground() );
  this->Pool->SyncSetTextProperty( this->Pool->GetItem(0)->GetTextProperty());

  this->Pool->SyncRender();
  this->Pool->SyncReset();

  this->Handle[0]->SetPosition( View1->GetImageActor()->GetCenter() );
  this->Handle[0]->SetProjectionPosition(
    View1->GetImageActor()->GetCenter()[vtkViewImage2D::VIEW_ORIENTATION_AXIAL] );

  double bounds[6];
  iInput->GetBounds( bounds );

  vtkPlane* p1 = vtkPlane::New();
  p1->SetOrigin( bounds[0], bounds[2], bounds[4] );
  p1->SetNormal( 1.0, 0.0, 0.0 );

  vtkPlane* p2 = vtkPlane::New();
  p2->SetOrigin( bounds[0], bounds[2], bounds[4] );
  p2->SetNormal( 0.0, 0.0, 1.0 );

  vtkPlane *p3 = vtkPlane::New();
  p3->SetOrigin( bounds[1], bounds[3], bounds[5] );
  p3->SetNormal( -1.0, 0.0, 0.0 );

  vtkPlane *p4 = vtkPlane::New();
  p4->SetOrigin( bounds[1], bounds[3], bounds[5] );
  p4->SetNormal( 0.0, 0.0, -1.0 );

  vtkPlane *p5 = vtkPlane::New();
  p5->SetOrigin( bounds[0], bounds[2], bounds[4] );
  p5->SetNormal( 0.0, 1.0, 0.0 );

  vtkPlane *p6 = vtkPlane::New();
  p6->SetOrigin( bounds[1], bounds[3], bounds[5] );
  p6->SetNormal( 0.0, -1.0, 0.0 );

  this->Handle[0]->AddBoundingPlane( p1 );
  this->Handle[0]->AddBoundingPlane( p2 );
  this->Handle[0]->AddBoundingPlane( p3 );
  this->Handle[0]->AddBoundingPlane( p4 );
  this->Handle[0]->AddBoundingPlane( p5 );
  this->Handle[0]->AddBoundingPlane( p6 );

  p1->Delete();
  p2->Delete();
  p3->Delete();
  p4->Delete();
  p5->Delete();
  p6->Delete();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetImage( vtkImageData* iInput )
{
  if( !iInput )
    {
    return;
    }
  else
    {
    bool Is2DImagePrevious = this->Is2DImage;

    this->Image = iInput;

    int extent[6];
    this->Image->GetExtent( extent );

    this->Is2DImage = ( extent[0] == extent[1] )
      || ( extent[2] == extent[3] )
      || ( extent[4] == extent[5] );

    if( !this->Is2DImage )
      {
      if( this->Is2DImage != Is2DImagePrevious )
        {
        vtkViewImage2DWithContourWidget* View2 =
          vtkViewImage2DWithContourWidget::New();
        View2->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
        View2->SetViewOrientation (vtkViewImage2D::VIEW_ORIENTATION_CORONAL);

        this->SetupViewGivenQVTKWidget( View2, this->QvtkWidget_2 );

        this->Pool->AddItem( View2 );
        View2->Delete();

        vtkViewImage2DWithContourWidget* View3 =
          vtkViewImage2DWithContourWidget::New();
        View3->SetViewConvention( vtkViewImage2D::VIEW_CONVENTION_NEUROLOGICAL );
        View3->SetViewOrientation( vtkViewImage2D::VIEW_ORIENTATION_SAGITTAL );

        this->SetupViewGivenQVTKWidget( View3, this->QvtkWidget_3 );

        this->Pool->AddItem( View3 );
        View3->Delete();

        this->Slider1->show();
        this->LayOutWidget2->show();
        this->LayOutWidget3->show();
        this->LayOutWidget4->show();
        }

      Set3DImage( iInput );
      }
    else
      {
      if( this->Is2DImage != Is2DImagePrevious )
        {
        this->Pool->InitTraversal();
        vtkViewImage2DWithContourWidget* item = this->Pool->GetNextItem();
        item = this->Pool->GetNextItem();

        int index = this->Pool->GetNumberOfItems() - 1;
        while( this->Pool->GetNumberOfItems() != 1 )
          {
          this->Pool->RemoveItem( index );
          index--;
          }

        this->Slider1->hide();
        this->LayOutWidget2->hide();
        this->LayOutWidget3->hide();
        this->LayOutWidget4->hide();
        }

      Set2DImage( iInput );
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetShowScalarBar( const bool& state )
{
  this->Pool->SyncSetShowScalarBar(state);
  //   this->View3D->SetShowScalarBar( state );
  this->Pool->SyncRender();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetShowAnnotations( const bool& iState )
{
  this->Pool->SyncSetShowAnnotations( iState );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetColorWindow( const double& iValue )
{
  this->Pool->SyncSetColorWindow( iValue );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetColorLevel( const double& iValue )
{
  this->Pool->SyncSetColorLevel( iValue );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetTag( const QString& iTag )
{
  this->Tag = iTag;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString QImagePageViewTracer::GetTag( ) const
{
  return this->Tag;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int QImagePageViewTracer::GetCellId() const
{
  return this->CellId;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QImagePageViewTracer::GetTracerStatus() const
{
  if( this->Pool->GetNumberOfItems() )
    {
    return this->Pool->GetItem( 0 )->GetContourWidgetEnabled( );
    }
  else
    {
    return false;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetTracerON()
{
  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++ )
    {
    this->Pool->GetItem( i )->SetContourWidgetInteractionOn();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetTracerOFF()
{
  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++ )
    {
    this->Pool->GetItem( i )->SetContourWidgetInteractionOff();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetTracer( const bool& iState )
{
  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++ )
    {
    this->Pool->GetItem( i )->SetContourWidgetInteraction( iState );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetSeedingON()
{
  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++ )
    {
    this->Pool->GetItem( i )->SetContourWidgetInteractionOff();
    this->SeedWidget[i]->On();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetSeedingOFF()
{
  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++ )
    {
    this->Pool->GetItem( i )->SetContourWidgetInteractionOff();
    this->SeedWidget[i]->Off();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetSeeding( const bool& iState )
{
  for( unsigned int i = 0; i < this->SeedWidget.size(); i++ )
    {
    this->SeedWidget[i]->SetEnabled( iState );
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool QImagePageViewTracer::GetSeedingStatus( ) const
{
  return this->SeedWidget.front()->GetEnabled();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
vtkPoints* QImagePageViewTracer::GetAllSeeds()
{
  double tpos[3];
  vtkPoints* oPoints = vtkPoints::New();

  for( unsigned int i = 0; i < this->SeedWidget.size(); i++ )
    {
    int N = this->SeedRep[i]->GetNumberOfSeeds();

    for( int j = 0; j < N; j++ )
      {
      this->SeedRep[i]->GetSeedWorldPosition( j, tpos );
      oPoints->InsertNextPoint( tpos );
      }
    }

  return oPoints;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::ClearAllSeeds()
{
  for( unsigned int i = 0; i < this->SeedWidget.size(); i++ )
    {
    int N = this->SeedRep[i]->GetNumberOfSeeds();
    int k = N - 1;
    for( int j = 0; j < N; j++ )
      {
      this->SeedWidget[i]->DeleteSeed( k-- );
      this->SeedRep[i]->RemoveLastHandle();
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::resizeEvent( QResizeEvent* iEvent )
{
  QWidget::resizeEvent( iEvent );
  VSplitter->resize( iEvent->size() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::ValidateContour(
  const int& iId,
  const QColor& iColor,
  const bool& iSave )
{
  // The spline representation of the figure
  vtkOrientedGlyphContourRepresentation* contour_rep;

  // The polydata (discrete mesh) representation of the figure
  vtkPolyData* contour;

  vtkViewImage2DWithContourWidget* view;

  bool degenerated = true;

  // For each vizualization window
  // NOTE ALEX: why are we making a loop over 2D views here? Don't we know in
  //            which view we are editing?
  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++ )
    {
    view = this->Pool->GetItem( i );

    // Get the figure as a spline and transform it into a polydata
    contour_rep = view->GetContourRepresentation();
    contour = contour_rep->GetContourRepresentationAsPolyData( );

    // if we were succesfull and the contour is no degenrated, proceed
    if( contour )
      {
      if( contour_rep->GetNumberOfNodes() > 2 )
        {
        degenerated = false;

        SaveAndDisplayContour( iId, iColor, iSave, view, contour, contour_rep );
        }
      } // ENDOF if contour

    } // ENDOF for each pool item

  //NOTE: we should only add the contour of the widget in which we were drawing
  // and avoid adding too many contours at the same time
  ContourId++;

  if( degenerated )
    {
    QMessageBox::warning( this, tr("Degenerated Contour"),
      tr("You can not validate degenerated contour...") );
    return;
    }

  this->Pool->SyncRender();

}

void QImagePageViewTracer::SaveAndDisplayContour(
  const int& iId,
  const QColor& iColor,
  const bool& iSave,
  vtkViewImage2DWithContourWidget* iView,
  vtkPolyData* iContour,
  vtkOrientedGlyphContourRepresentation* iContour_rep
)
{
  // color object
  double rgb[3];
  rgb[0] = static_cast< double >( iColor.red() )   / 255.;
  rgb[1] = static_cast< double >( iColor.green() ) / 255.;
  rgb[2] = static_cast< double >( iColor.blue() )  / 255.;

  // the correponding cell ID (several figures can have the same id
  // as several 2D contours belong to the same 3D cell
  CellId = iId;

  // properties (representation style and color) of the figure
  vtkProperty* contour_property = vtkProperty::New();
  contour_property->SetRepresentationToWireframe();
  contour_property->SetColor( rgb );

  // -------
  // BOUNDS COMPUTATION
  // -------
  double bounds[6];
  iContour->GetBounds( bounds );

  double Min[3], Max[3];
  Min[0] = bounds[0];
  Max[0] = bounds[1];
  Min[1] = bounds[2];
  Max[1] = bounds[3];
  Min[2] = bounds[4];
  Max[2] = bounds[5];

  int* min_idx = iView->GetImageCoordinatesFromWorldCoordinates( Min );
  int* max_idx = iView->GetImageCoordinatesFromWorldCoordinates( Max );

  // NOTE ALEX: use itk debug / vtk debug or a debug flag here

//         std::cout << "Min = [" << min_idx[0];
//         std::cout << " " << min_idx[1] << " " << min_idx[2] << "]" << std::endl;
//         std::cout << "Max = [" <<max_idx[0];
//         std::cout << " " << max_idx[1] << " " << max_idx[2] << "]" << std::endl;

  // make a copy for each view
  vtkPolyData* contour_copy = vtkPolyData::New();
  contour_copy->ShallowCopy( iContour );

  vtkActor* temp;
  vtkPolyData* ControlPointsPolyData = vtkPolyData::New();

  if( iContour_rep )
    {
    iContour_rep->GetNodePolyData( ControlPointsPolyData );
    }

// #ifndef TEMPARNAUD
  for( int j = 0; j < this->Pool->GetNumberOfItems(); j++ )
// #else
//   for( int j = 0; j < 1; j++ )
// #endif
    {
    temp = this->Pool->GetItem( j )->AddDataSet(
      contour_copy, contour_property, true, false );

    m_ContourIdActorMap.insert(
      std::pair< unsigned int, ContourStructure >( ContourId,
        ContourStructure( temp,
        ControlPointsPolyData, //control points of the spline representation
        contour_copy, // vtkPolyData corresponding to the displaid contour
        CellId, //MeshId
        j, //Direction
        this->m_TimePoint, //TimePoint
        rgb[0],
        rgb[1],
        rgb[2],
        false ) ) );

    m_ActorContourIdMap[ temp ] = ContourId;
    }


    temp = this->View3D->AddDataSet( contour_copy, contour_property,
      false, false );

    temp->Delete();

//         contour_copy->Delete();

    // ------
    // SAVE AS A FILE AND IN DB
    // ------

    // NOTE ALEX: separate method
    // NOTE ALEX: shoudn't saving in a file and in the DB
    //            be mutually exclusive?
    if( iSave )
      {
      // build up the base file name
      QString identifier = QString( "_id%1" ).arg( CellId );
      QString MinString = QString( "_m%1_%2_%3" )
        .arg( min_idx[0] ).arg( min_idx[1] ).arg( min_idx[2] );
      QString MaxString = QString( "_M%1_%2_%3" )
        .arg( max_idx[0] ).arg( max_idx[1] ).arg( max_idx[2] );
      QString filename_prefix = QString( "contour%1%2%3" )
        .arg( identifier ).arg( MinString ).arg( MaxString );

      SaveValidatedContourAndNodesInFile( iContour, ControlPointsPolyData,
        filename_prefix );

      // Save in database*
      bool database_info = ( m_DBExperimentID == -1 ) && m_DBLogin.isNull()
        && m_DBExperimentName.isNull() && m_DBName.isNull()
        && m_DBServer.isNull() && m_DBPassword.isNull();
      if( !database_info )
        {
// #ifndef TEMPARNAUD
        SaveValidatedContourInDatabase( iContour );
// #else
//         SaveValidatedContourInDatabase( ControlPointsPolyData );
// #endif
        }

      }  // ENDOF Save in a File

//         ControlPointsPolyData->Delete();

//         contour_property->Delete();

}
//-------------------------------------------------------------------------
void QImagePageViewTracer::SaveValidatedContourAndNodesInFile( vtkPolyData* contour,
  vtkPolyData* nodes,
  QString iBaseName )
{
  QString vtk_filename = iBaseName;

  // write the polydata in a file
  vtk_filename.append( ".vtk");
  vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
  writer->SetInput( contour );
  writer->SetFileName( vtk_filename.toAscii().constData() );
  writer->Write();

  // write the spline contour in a file
  QString ctrlpt_filename = iBaseName;
  ctrlpt_filename.append( ".scpts" );

  writer->SetInput( nodes );
  writer->SetFileName( ctrlpt_filename.toAscii().constData() );
  writer->Write();
  writer->Delete();
}

//-------------------------------------------------------------------------
void QImagePageViewTracer::SaveValidatedContourInDatabase( vtkPolyData* contour )
{
  // set up a Figure Table's row object
  GoDBFigureRow row;
  row.meshID = CellId;
  row.experimentID = m_DBExperimentID;
  row.TCoord = this->m_TimePoint;

  // transform the PolyData into a suitable String
  vtkPolyDataMySQLTextWriter* db_convert = vtkPolyDataMySQLTextWriter::New();
  row.points = db_convert->GetMySQLText( contour );
  db_convert->Delete();

  // compute perimeter and center
  double tpos[3]; // curent point
  double prev[3];// previous point
  contour->GetPoint( 0, prev );
  double perimeter = 0;
  double center[3] = {0, 0, 0};

  for( int ii = 1; ii < contour->GetNumberOfPoints(); ii++ )
    {
    contour->GetPoint( ii, tpos );
    perimeter += sqrt( vtkMath::Distance2BetweenPoints( prev, tpos ) );
    for( int dim = 0; dim < 3; dim++ )
      {
      center[dim] += tpos[dim];
      prev[dim] = tpos[dim];
      }
    }

  row.perimeter = static_cast< int >( perimeter );
  row.xCenter = static_cast< int >( center[0] / contour->GetNumberOfPoints() );
  row.yCenter = static_cast< int >( center[1] / contour->GetNumberOfPoints() );


  // Set up A RecordSet to connect to the DataBase
  typedef GoDBRecordSet< GoDBFigureRow >   SetType;
  SetType* mySet = new SetType;
  mySet->SetServerName( m_DBServer.toStdString() );
  mySet->SetDataBaseName( m_DBName.toStdString() );
  mySet->SetTableName( "figure" );
  mySet->SetUser( m_DBLogin.toStdString() );
  mySet->SetPassword( m_DBPassword.toStdString() );
  // NOTE ALEX: this should be unecessary if this is a new object.
  //            in that case reading all the table just to add one
  //            contour is a performance overkill
  // NOTE ALEX: this should load only the figures related to
  //            ONE EXPERIMENT. Have to find the good generic design
  //            for that. The easiest way is to pass the good query
  //            Here instead of hardcoding it in RecordSet.
  // NOTE ALEX: should restrict it to the time point and experimentID
  //            instead of loading the entire DB
  mySet->PopulateFromDB();
  mySet->AddObject( row );
  mySet->SaveInDB();
  delete mySet;

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::LoadFiguresFromDB( )
{

  // NOTE ALEX: this will load all figures from the the DB
  // even if they belong to different ExpID

  bool database_info = ( m_DBExperimentID == -1 ) && m_DBLogin.isNull()
    && m_DBExperimentName.isNull() && m_DBName.isNull()
    && m_DBServer.isNull() && m_DBPassword.isNull();
  if( !database_info )
    {
    typedef GoDBRecordSet< GoDBFigureRow >   SetType;
    typedef SetType::RowContainerType        SetContainerType;

    SetType* mySet = new SetType;
    mySet->SetServerName( m_DBServer.toStdString() );
    mySet->SetDataBaseName( m_DBName.toStdString() );
    mySet->SetTableName( "figure" );
    mySet->SetUser( m_DBLogin.toStdString() );
    mySet->SetPassword( m_DBPassword.toStdString() );
    std::stringstream whereString;
    whereString << "TCoord = " << this->m_TimePoint;
    mySet->SetWhereString( whereString.str() );
    mySet->PopulateFromDB();

    // FOR EACH CONTOUR IN THE RECORDSET
    SetContainerType * myRowContainer = mySet->GetRowContainer();
    for( unsigned int i = 0; i < myRowContainer->size(); i++ )
      {
      // USE POLYDATADBREADER TO TRANSFORM STRING IN PD
      vtkPolyData* nodes = vtkPolyData::New();
      vtkPolyDataMySQLTextReader* reader = vtkPolyDataMySQLTextReader::New();
      reader->SetIsContour( true );
      std::string PolyDataAsString = (*myRowContainer)[i].second.points;
      nodes = reader->GetPolyData( PolyDataAsString );
      reader->Delete();

      // SET PROPOERTY
      vtkProperty* contour_property = vtkProperty::New();
      contour_property->SetRepresentationToWireframe();

      // COPY FOR EACH VIEW
      vtkPolyData* nodes_copy = vtkPolyData::New();
      nodes_copy->ShallowCopy( nodes );

      double bounds[6];
      nodes->GetBounds( bounds );

      int dir = -1;

      if( bounds[4] == bounds[5] )
        {
        dir = 0; //XY
        }
      else
        {
        if( bounds[2] == bounds[3] )
          {
          dir = 1;//XZ
          }
        else
          {
          if( bounds[0] == bounds[1] )
            {
            dir = 2;
            }
          }
        }

      vtkActor* temp;
// #ifndef TEMPARNAUD
      for( int j = 0; j < this->Pool->GetNumberOfItems(); j++ )
        {
        temp = this->Pool->GetItem( j )->AddDataSet(
                  nodes_copy, contour_property, true, false );

        //TODO the second argument for the constructor of ContourStructure MUST
        // be the control points of the spline representation. For the time being
        // I use all the points of the contour (meaning much more nodes than
        // really required).
        //TODO It requires to create a new method where we can add control points
        // and the added contour is generated from these points.
        m_ContourIdActorMap.insert(
            std::pair< unsigned int, ContourStructure >( (*myRowContainer)[i].second.figureID,
              ContourStructure( temp,
                nodes_copy,
                nodes_copy,
                (*myRowContainer)[i].second.meshID, //MeshId
                j, //Direction
                this->m_TimePoint, //TimePoint
                1., 1., 1., // rgb => white by default
                false ) ) );

        m_ActorContourIdMap[ temp ] = (*myRowContainer)[i].second.figureID;
        }

      temp = this->View3D->AddDataSet( nodes_copy, contour_property, false );
      temp->Delete();

      nodes_copy->Delete();
      nodes->Delete();
// #else
//       for( int j = 0; j < 1; j++ )
//         {
//         this->Pool->GetItem( j )->SetContourWidgetInteractionOn();
//         this->Pool->GetItem( j )->GetContourWidget()->Initialize( nodes_copy );
//
//         ValidateContour( (*myRowContainer)[i].second.meshID, QColor( Qt::white ),
//           false );
//
//         this->Pool->GetItem( j )->SetContourWidgetInteractionOff();
//         } // ENDFOR
// #endif
      } // ENDFOR

    delete mySet;

    } //ENDIF
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::ReinitializeContour( )
{
  for( int i = 0; i < this->Pool->GetNumberOfItems(); i++ )
    {
    this->Pool->GetItem( i )->GetContourWidget()->Initialize( 0 );
    }
  this->Pool->SyncRender();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetSlideView1( const int& iSlice )
{
  this->Pool->GetItem( 0 )->SetSlice( iSlice );
  this->Pool->SyncRender();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetSlideView2( const int& iSlice )
{
  this->Pool->GetItem( 1 )->SetSlice( iSlice );
  this->Pool->SyncRender();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetSlideView3( const int& iSlice )
{
  this->Pool->GetItem( 2 )->SetSlice( iSlice );
  this->Pool->SyncRender();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::MoveSlider1( )
{
  this->Slider1->setValue( this->Pool->GetItem( 0 )->GetSlice() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::MoveSlider2( )
{
  this->Slider2->setValue( this->Pool->GetItem( 1 )->GetSlice() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::MoveSlider3( )
{
  this->Slider3->setValue( this->Pool->GetItem( 2 )->GetSlice() );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SaveStateSplitters()
{
  QSettings settings;
  settings.setValue("VSplitterSizes", VSplitter->saveState());
  settings.setValue("HtSplitterSizes", HtSplitter->saveState());
  settings.setValue("HbSplitterSizes", HbSplitter->saveState());
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::SetDatabaseRelatedVariables( const QString& iServer,
  const QString& iLogin, const QString& iPassword, const QString& iDatabaseName,
  const int& iExperimentID, const QString& iExperimentName )
{
  m_DBServer = iServer;
  m_DBLogin = iLogin;
  m_DBPassword = iPassword;
  m_DBName = iDatabaseName;
  m_DBExperimentID = iExperimentID;
  m_DBExperimentName = iExperimentName;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::HighlightContour( const unsigned int& iId,
  const bool& iToBeHighlighted )
{
  std::pair< ContourIdActorMapIterator, ContourIdActorMapIterator >
    result = m_ContourIdActorMap.equal_range( iId );

  ContourIdActorMapIterator it = result.first;
  if( it != m_ContourIdActorMap.end() )
    {
    unsigned int tempid = it->first;

    if( tempid != iId )
      {
      return;
      }
    else
      {
      do
        {
        int dir = it->second.Direction;
        vtkActor* actor = it->second.Actor;
        bool highlighted = it->second.Highlighted;

        if( highlighted != iToBeHighlighted )
          {
          this->Pool->GetItem( dir )->HighlightContour( actor, !highlighted );
          it->second.Highlighted = !highlighted;
          }
        ++it;
        } while( it != result.second );
      }
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::HighlightContours( std::map< unsigned int, bool > iIds )
{
  std::map< unsigned int, bool >::const_iterator it = iIds.begin();
  std::map< unsigned int, bool >::const_iterator end = iIds.end();
  // NOTE ALEX: unused variable
  //size_t tsize = iIds.size();

  while( it != end )
    {
    HighlightContour( it->first, it->second );
    ++it;
    }
  this->Pool->SyncRender();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QImagePageViewTracer::ReeditContour( const unsigned int& iId )
{
  std::pair< ContourIdActorMapIterator, ContourIdActorMapIterator >
    result = m_ContourIdActorMap.equal_range( iId );

  ContourIdActorMapIterator it = result.first;
  unsigned int temp = it->first;

  if( temp != iId )
    {
    return;
    }
  else
    {
    int dir = 0;
    vtkActor* ContourActor = 0;
    // NOTE ALEX: unused variable
    // vtkPolyData* Contour = 0;
    vtkPolyData* Nodes = 0;
    unsigned int MeshId = 0;
    unsigned int tempId = 0;

    do
      {
      // First let's get in which view is the contour to be reedited
      dir = (it->second).Direction;

      // Then get the corresponding actor
      ContourActor = (it->second).Actor;

      Nodes = (it->second).Nodes;

      MeshId = (it->second).MeshId;

      tempId = it->first;

      // Remove the actor from the visualization
      this->Pool->GetItem( dir )->GetRenderer()->RemoveActor( ContourActor );

      //TODO still have to work around to remove properly all corresponding
      // actors on the different views.

//       // RemoveDataSet( Contour );
//       // this->View3D->RemoveDataSet( Contour );

      // Reintialize the contour widget with corresponding control points (nodes).
      this->Pool->GetItem( dir )->SetContourWidgetInteractionOn();
      this->Pool->GetItem( dir )->GetContourWidget()->Initialize( Nodes );
      ++it;
      } while( it != result.second );
    }
}
//-------------------------------------------------------------------------

vtkPolyData* QImagePageViewTracer::GenerateCircleFromGivenSphereAndGivenX( double iC[3],
  const double& iRadius, double iX, const int& iN )
{
  double res = ( iC[0] - iX );
  res *= res;
  res = iRadius * iRadius - res;

  if( res < 0 )
    {
    return 0;
    }
  else
    {
    vtkPolyData* oCircle = vtkPolyData::New();
    vtkPoints    *points      = vtkPoints::New();
    vtkCellArray *lines       = vtkCellArray::New();
    vtkIdType    *lineIndices = new vtkIdType[iN+1];

    double theta = 0.;
    double r = sqrt( res );

    for( int i = 0; i < iN; i++ )
      {
      theta = 2. * i * vtkMath::Pi() / static_cast< double >( iN );
      points->InsertPoint( static_cast< vtkIdType>( i ), iX,
        iC[1] + r * cos( theta ), iC[2] + r * sin( theta ) );
      lineIndices[i] = static_cast<vtkIdType>(i);
      }

    lineIndices[iN] = 0;
    lines->InsertNextCell(iN+1,lineIndices);
    delete [] lineIndices;
    oCircle->SetPoints( points );
    oCircle->SetLines(lines);

    return oCircle;
    }
}

vtkPolyData* QImagePageViewTracer::GenerateCircleFromGivenSphereAndGivenY( double iC[3],
  const double& iRadius, double iY, const int& iN )
{
  double res = ( iC[1] - iY );
  res *= res;
  res = iRadius * iRadius - res;

  if( res < 0 )
    {
    return 0;
    }
  else
    {
    vtkPolyData* oCircle = vtkPolyData::New();
    vtkPoints    *points      = vtkPoints::New();
    vtkCellArray *lines       = vtkCellArray::New();
    vtkIdType    *lineIndices = new vtkIdType[iN+1];

    double theta = 0.;
    double r = sqrt( res );

    for( int i = 0; i < iN; i++ )
      {
      theta = 2. * i * vtkMath::Pi() / static_cast< double >( iN );
      points->InsertPoint( static_cast< vtkIdType>( i ),
        iC[0] + r * cos( theta ), iY, iC[2] + r * sin( theta ) );
      lineIndices[i] = static_cast<vtkIdType>(i);
      }

    lineIndices[iN] = 0;
    lines->InsertNextCell(iN+1,lineIndices);
    delete [] lineIndices;
    oCircle->SetPoints( points );
    oCircle->SetLines(lines);

    return oCircle;
    }
}

vtkPolyData* QImagePageViewTracer::GenerateCircleFromGivenSphereAndGivenZ( double iC[3],
  const double& iRadius, double iZ, const int& iN )
{
  double res = ( iC[2] - iZ );
  res *= res;
  res = iRadius * iRadius - res;

  if( res < 0 )
    {
    return 0;
    }
  else
    {
    vtkPolyData* oCircle = vtkPolyData::New();
    vtkPoints    *points      = vtkPoints::New();
    vtkCellArray *lines       = vtkCellArray::New();
    vtkIdType    *lineIndices = new vtkIdType[iN+1];

    double theta = 0.;
    double r = sqrt( res );

    for( int i = 0; i < iN; i++ )
      {
      theta = 2. * i * vtkMath::Pi() / static_cast< double >( iN );
      points->InsertPoint( static_cast< vtkIdType>( i ),
        iC[0] + r * cos( theta ), iC[1] + r * sin( theta ), iZ );
      lineIndices[i] = static_cast<vtkIdType>(i);
      }

    lineIndices[iN] = 0;
    lines->InsertNextCell(iN+1,lineIndices);
    delete [] lineIndices;
    oCircle->SetPoints( points );
    oCircle->SetLines(lines);

    return oCircle;
    }
}

void QImagePageViewTracer::JustForNick( double iCenter[3], double iRadius )
{
  vtkViewImage2DWithContourWidget* view = this->Pool->GetItem( 0 );
  int* center_id = view->GetImageCoordinatesFromWorldCoordinates( iCenter );

  double corner[3];
  corner[0] = iCenter[0] - iRadius;
  corner[1] = iCenter[1];
  corner[2] = iCenter[2];

  int* corner_id = view->GetImageCoordinatesFromWorldCoordinates( corner );
  int xlength = 2 * std::abs( center_id[0] - corner_id[0] );

  int idx[3];
  idx[0] = corner_id[0];
  idx[1] = corner_id[1];
  idx[2] = corner_id[2];

  double* pos;

  for( int i = 0; i < xlength; i++, idx[0]++ )
    {
    pos = view->GetWorldCoordinatesFromImageCoordinates( idx );
    vtkPolyData* circle = GenerateCircleFromGivenSphereAndGivenX( iCenter, iRadius,
      pos[0], 20 );

    if( circle )
      {
//       for( int j = 0; j < 3; j++ )
        {
        SaveAndDisplayContour(
          CellId,
          QColor( 255, 0, 0 ),
          true,
          this->Pool->GetItem( 0 ),
          circle,  0 );
        }
      }
    }

  corner[0] = iCenter[0];
  corner[1] = iCenter[1] - iRadius;
  corner[2] = iCenter[2];

  corner_id = view->GetImageCoordinatesFromWorldCoordinates( corner );
  int ylength = 2 * std::abs( center_id[1] - corner_id[1] );

  idx[0] = corner_id[0];
  idx[1] = corner_id[1];
  idx[2] = corner_id[2];

  for( int i = 0; i < ylength; i++, idx[1]++ )
    {
    pos = view->GetWorldCoordinatesFromImageCoordinates( idx );
    vtkPolyData* circle = GenerateCircleFromGivenSphereAndGivenY( iCenter, iRadius,
      pos[1], 20 );

    if( circle )
      {
      for( int j = 0; j < 3; j++ )
        {
        SaveAndDisplayContour(
          CellId,
          QColor( 255, 0, 0 ),
          true,
          this->Pool->GetItem( j ),
          circle,  0 );
        }
      }
    }

  corner[0] = iCenter[0];
  corner[1] = iCenter[1];
  corner[2] = iCenter[2] - iRadius;

  corner_id = view->GetImageCoordinatesFromWorldCoordinates( corner );
  int zlength = 2 * std::abs( center_id[2] - corner_id[2] );

  idx[0] = corner_id[0];
  idx[1] = corner_id[1];
  idx[2] = corner_id[2];

  for( int i = 0; i < zlength; i++, idx[2]++ )
    {
    pos = view->GetWorldCoordinatesFromImageCoordinates( idx );
    vtkPolyData* circle = GenerateCircleFromGivenSphereAndGivenZ( iCenter, iRadius,
      pos[2], 20 );

    if( circle )
      {
//       for( int j = 0; j < 3; j++ )
        {
        SaveAndDisplayContour(
          CellId,
          QColor( 255, 0, 0 ),
          true,
          this->Pool->GetItem( 0 ),
          circle,  0 );
        }
      }
    }

  CellId++;
}
