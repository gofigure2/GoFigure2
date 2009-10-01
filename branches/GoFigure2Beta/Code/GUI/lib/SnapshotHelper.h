#ifndef __SnapshotHelper_h
#define __SnapshotHelper_h

#include "vtkRenderWindow.h"
#include "vtkImageData.h"

#include <vtkImageClip.h>
#include <vtkImagePermute.h>
#include <vtkImageResample.h>
#include <vtkWindowToImageFilter.h>

#include <vtkBMPWriter.h>
#include <vtkPostScriptWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkTIFFWriter.h>

#include "QVTKWidget.h"

namespace GoFigure
{
  enum SnapshotImageType
    {
    BMP = 0,
    EPS,
    JPEG,
    PNG,
    TIFF
    };
}

// QString SnapshotView( QVTKWidget* iWidget,
//   const GoFigure::SnapshotImageType& iType,
//   const QString& iBaseName,
//   const unsigned int& iSnapshotId );
//
// bool BuildScreenshotFromImage( vtkImageData* image,
//     vtkImageData* screenshot, int size = 0 );
//
// bool BuildScreenshotFromRenderWindow(
//   vtkRenderWindow *win,
//   vtkImageData* screenshot,
//   int tsize = 0 );

//-------------------------------------------------------------------------
template< class TWriter >
void WriteImage( vtkImageData* iImage, const QString& iFileName )
{
  typedef TWriter WriterType;
  WriterType* writer = WriterType::New();
  writer->SetInput( iImage );
  writer->SetFileName( iFileName.toAscii( ).constData( ) );
  writer->Write();
  writer->Delete();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool BuildScreenshotFromImage( vtkImageData* image,
  vtkImageData* screenshot,
  int tsize = 0 )
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
bool BuildScreenshotFromRenderWindow(
  vtkRenderWindow *win,
  vtkImageData* screenshot,
  int tsize = 0 )
{
  if( win && screenshot )
    {
    vtkWindowToImageFilter* filter = vtkWindowToImageFilter::New();
    filter->ShouldRerenderOff();
    filter->SetInput( win );
    filter->Update();
    bool res = BuildScreenshotFromImage( filter->GetOutput(),
      screenshot, tsize );
    filter->Delete();
    return res;
    }
  return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString SnapshotView( QVTKWidget* iWidget,
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName,
  const unsigned int& iSnapshotId )
{
  vtkImageData* image = vtkImageData::New();
  BuildScreenshotFromRenderWindow( iWidget->GetRenderWindow(),
        image );
  QString filename = iBaseName;
  filename.append( QString( "%1" ).arg( iSnapshotId ) );

  switch( iType )
    {
    default:
    case GoFigure::BMP:
      {
      filename.append( ".bmp" );
      WriteImage< vtkBMPWriter >( image, filename );
      }
      break;
    case GoFigure::EPS:
      {
      filename.append( ".eps" );
      WriteImage< vtkPostScriptWriter >( image, filename );
      }
      break;
    case GoFigure::JPEG:
      {
      filename.append( ".jpeg" );
      WriteImage< vtkJPEGWriter >( image, filename );
      }
      break;
    case GoFigure::PNG:
      {
      filename.append( ".png" );
      WriteImage< vtkPNGWriter >( image, filename );
      }
      break;
    case GoFigure::TIFF:
      {
      filename.append( ".tiff" );
      WriteImage< vtkTIFFWriter >( image, filename );
      }
      break;
    }
  image->Delete();
  return filename;
}
//-------------------------------------------------------------------------


#endif