#ifndef __SnapshotHelper_h
#define __SnapshotHelper_h

class QString;

class vtkRenderWindow;
class vtkImageData;
class QVTKWidget;
class vtkViewImage2D;

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
