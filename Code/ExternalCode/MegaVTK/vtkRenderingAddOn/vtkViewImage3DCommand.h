#ifndef __vtkViewImage3DCommand_h
#define __vtkViewImage3DCommand_h

#include "vtkCommand.h"
#include "vtkObject.h"
#include "MegaVTK2Configure.h"

class vtkProp;
class vtkViewImage3D;
class vtkOrientedBoxWidget;

class VTK_RENDERINGADDON2_EXPORT vtkViewImage3DCommand :
  public vtkCommand
{
 public:

  static vtkViewImage3DCommand* New();

  //BTX
  enum EventIds
  {
    SliceMoveEvent=(vtkCommand::UserEvent+1),
    StartSliceMoveEvent,
    EndSliceMoveEvent,
    ZoomEvent,
    PanEvent,
    RequestedPositionEvent,
    ResetViewerEvent,
    ContourPickingEvent,
    MeshPickingEvent,
    BoxPickingEvent,
    CameraMoveEvent,
    DefaultMoveEvent,
    SeedEvent
  };
  //ETX

  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute( vtkObject *caller, unsigned long event,
    void *vtkNotUsed(callData) );

  vtkProp*              GetPickedActor();
  vtkOrientedBoxWidget* GetBoxWidget();

  void SetVtkImageView3D( vtkViewImage3D* vtkViewImage3D );


 protected:

  vtkViewImage3DCommand();
  ~vtkViewImage3DCommand();

 private:
  vtkViewImage3D*       m_vtkViewImage3D;
  vtkProp*              m_PickedActor;
  vtkOrientedBoxWidget* m_BoxWidget;
  //bool                  m_BoxPickingEnabled;
};

#endif
