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
#include "vtkViewImage2D.h"

#include "GoFigureGlobalDefinition.h"

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
  const GoFigure::FileType& iType,
  const QString& iBaseName,
  const unsigned int& iSnapshotId );
//-------------------------------------------------------------------------

void SetupViewGivenQVTKWidget( vtkViewImage2D* iView, QVTKWidget* iWidget );

#endif