#ifndef __vtkViewImage3DCommand_h
#define __vtkViewImage3DCommand_h

#include "vtkCommand.h"
#include "vtkObject.h"
#include "MegaVTK2Configure.h"

#include <list>
#include "vtkProp3D.h"

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
    SeedEvent,
    BoxWidgetModifiedEvent,
    ReadyEvent
  };
  //ETX

  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute( vtkObject *caller, unsigned long event,
    void *vtkNotUsed(callData) );

  vtkOrientedBoxWidget*    GetBoxWidget();
  std::list< vtkProp3D* >  GetListOfModifiedActors();
  void                     Enable3DBoxWidget( bool iValue );

  void SetVtkImageView3D( vtkViewImage3D* vtkViewImage3D );


 protected:

  vtkViewImage3DCommand();
  ~vtkViewImage3DCommand();

 private:
  vtkViewImage3D*         m_vtkViewImage3D;
  vtkOrientedBoxWidget*   m_BoxWidget;
  std::list< vtkProp3D* > m_ListOfPickedActors;
  std::list< vtkProp3D* > m_ListOfModifiedActors;
  // which behavior do we want for this widget...?
  bool                    m_InitializedBoxWidget;
};

#endif
