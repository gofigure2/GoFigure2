#ifndef __vtkViewImage2DCollectionCommand_h
#define __vtkViewImage2DCollectionCommand_h

#include "vtkViewImage2DCollection.h"

class vtkViewImage2DCollectionCommand :
  public vtkCommand
{

 public:

  static vtkViewImage2DCollectionCommand *New()
    {return new vtkViewImage2DCollectionCommand;};

  vtkViewImage2DCollection* GetCollection (void)
  { return this->Collection;};
  void SetCollection (vtkViewImage2DCollection* p);

  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute(vtkObject *caller,unsigned long event,
    void *vtkNotUsed(callData));

 protected:

  vtkViewImage2DCollectionCommand();
  ~vtkViewImage2DCollectionCommand(){};

 private:
  vtkViewImage2DCollection* Collection;

  double InitialWindow;
  double InitialLevel;

};

#endif