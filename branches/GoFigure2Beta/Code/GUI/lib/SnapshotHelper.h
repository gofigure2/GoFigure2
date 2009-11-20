#ifndef __SnapshotHelper_h
#define __SnapshotHelper_h

#include "vtkRenderWindow.h"
#include "vtkImageData.h"

#include "vtkImageWriterHelper.h"

#include "QVTKWidget.h"
#include "vtkViewImage2D.h"

#include "GoFigureGlobalDefinition.h"

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