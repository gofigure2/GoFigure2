#ifndef __vtkViewImage3DCommand_h
#define __vtkViewImage3DCommand_h

#include "vtkCommand.h"
#include "vtkObject.h"
#include "MegaVTK2Configure.h"

class vtkViewImage3D;

class VTK_RENDERINGADDON2_EXPORT vtkViewImage3DCommand :
  public vtkCommand
  {
public:

  static vtkViewImage3DCommand* New();

  //BTX
  enum EventIds
    {
    SliceMoveEvent = (vtkCommand::UserEvent + 1),
    StartSliceMoveEvent,
    EndSliceMoveEvent,
    ZoomEvent,
    PanEvent,
    RequestedPositionEvent,
    ResetViewerEvent,
    MeshPickingEvent,
    BoxPickingEvent,
    CameraMoveEvent,
    DefaultMoveEvent,
    BoxWidgetModifiedEvent,
    ReadyEvent,
    BoxWidgetReadyEvent
    };
  //ETX

  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute(vtkObject *caller, unsigned long, void *);

  void SetVtkImageView3D(vtkViewImage3D* vtkViewImage3D);

protected:

  vtkViewImage3DCommand();
  ~vtkViewImage3DCommand();

private:
  vtkViewImage3D*       m_vtkViewImage3D;
  };

#endif
