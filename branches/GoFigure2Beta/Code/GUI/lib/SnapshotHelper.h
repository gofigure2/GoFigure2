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
  int tsize = 0 );
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool BuildScreenshotFromRenderWindow(
  vtkRenderWindow *win,
  vtkImageData* screenshot,
  int tsize = 0 );
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QString SnapshotView( QVTKWidget* iWidget,
  const GoFigure::SnapshotImageType& iType,
  const QString& iBaseName,
  const unsigned int& iSnapshotId );
//-------------------------------------------------------------------------


#endif