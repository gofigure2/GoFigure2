#ifndef _vtkViewImage2DCommand_h_
#define _vtkViewImage2DCommand_h_

#include<vtkCommand.h>

class vtkInteractorStyleImage2D;
class vtkInteractorStyleImage;
class vtkViewImage2D;

class vtkViewImage2DCommand : public vtkCommand
{
 public:

  static vtkViewImage2DCommand* New()
    { return new vtkViewImage2DCommand; }

  //BTX
  enum EventIds
  {
    SliceMoveEvent=(vtkCommand::UserEvent+1),
    StartSliceMoveEvent,
    EndSliceMoveEvent,
    ZoomEvent,
    PanEvent,
    RequestedPositionEvent,
    ResetViewerEvent
  };
  //ETX



  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute(vtkObject *caller,unsigned long event,
    void *vtkNotUsed(callData));

  void SetViewer(vtkViewImage2D *viewer)
  { this->Viewer = viewer; }

 protected:
  vtkViewImage2DCommand();
  ~vtkViewImage2DCommand(){};

  void Windowing( vtkInteractorStyleImage2D* isi );
  void Zooming();
  void Panning();
  void PrintInformation();

 private:
  vtkViewImage2D* Viewer;
  double InitialWindow;
  double InitialLevel;
  int InitialSlice;

};

#endif
