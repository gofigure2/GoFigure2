#ifndef _vtkViewImage2DCollection_h_
#define _vtkViewImage2DCollection_h_

#include "vtkCollection.h"
#include "vtkCommand.h"

#include "vtkViewImage2D.h"
/**
   This macro can be useful as we frequently set
   an instance this way, meaning unregistering previously set instance,
   registering given instance, and call a modified event.
   However this might be not the best place to define it...
*/
//BTX
#define vtkSyncSetMacro(name, type, base )			\
  virtual void SyncSet##name (type _arg)		\
  {							\
    this->InitTraversal();				\
    base* item = this->GetNextItem();	\
    while(item)						\
    {							\
      item->Set##name (_arg);				\
      item = this->GetNextItem();			\
    }							\
  }
///ETX

///BTX
#define vtkSyncSetObjectMacro(name, type, base)		\
  virtual void SyncSet##name (type* _arg)		\
  {							\
    this->InitTraversal();				\
    base* item = this->GetNextItem();	\
    while(item)						\
    {							\
      item->Set##name (_arg);				\
      item = this->GetNextItem();			\
    }							\
  }

///ETX


class vtkScalarBarActor;
class vtkLookupTable;
class vtkTextProperty;
class vtkCornerAnnotation;
class vtkOrientationAnnotation;
class vtkPlane;
class vtkActor;
class vtkDataSet;
class vtkPolyData;
class vtkProperty;
class vtkProp3DCollection;
class vtkDataSetCollection;
class vtkMatrixToLinearTransform;
class vtkViewImage2DCollectionCommand;


/**

*/


class vtkViewImage2DCollection : public vtkCollection
{
 public:

  static vtkViewImage2DCollection* New();
  vtkTypeRevisionMacro(vtkViewImage2DCollection, vtkCollection);

  // Description:
  // Get the next vtkViewImage2D in the list. Return NULL when at the end of the
  // list.
  vtkViewImage2D *GetNextItem() {
    return static_cast<vtkViewImage2D *>(this->GetNextItemAsObject());};
  // Description:
  // Get the next vtkViewImage2D2D in the list. Return NULL when at the end of
  // the list.
  vtkViewImage2D *GetItem(int i) {
    return static_cast<vtkViewImage2D *>(this->GetItemAsObject(i));};
  // Description:
  // Add an object to the list. Does not prevent duplicate entries.
  void AddItem(vtkViewImage2D*);

  // Description:
  // Replace the i'th item in the collection with a
  void ReplaceItem(int i, vtkViewImage2D *);

  // Description:
  // Remove the i'th item in the list.
  // Be careful if using this function during traversal of the list using
  // GetNextItemAsObject (or GetNextItem in derived class).  The list WILL
  // be shortened if a valid index is given!  If this->Current is equal to the
  // element being removed, have it point to then next element in the list.
  void RemoveItem(int i);

  // Description:
  // Remove an object from the list. Removes the first object found, not
  // all occurrences. If no object found, list is unaffected.  See warning
  // in description of RemoveItem(int).
  void RemoveItem(vtkViewImage2D *);

  // Description:
  // Remove all objects from the list.
  void RemoveAllItems();

  // Description:
  // Initialize the viewers togethers.
  void Initialize(void);


  vtkGetObjectMacro (Command, vtkViewImage2DCollectionCommand);
  vtkGetObjectMacro (ExtraRenderWindow, vtkRenderWindow);
  void SetExtraRenderWindow (vtkRenderWindow* win)
  { this->ExtraRenderWindow = win; }

  vtkSyncSetMacro (Slice, int, vtkViewImage2D);
/*   vtkSyncSetMacro (ViewOrientation, int); */
  vtkSyncSetMacro (SliceOrientation, int, vtkViewImage2D);
  vtkSyncSetMacro (ShowAnnotations, int, vtkViewImage2D);
  vtkSyncSetMacro (ShowScalarBar, int, vtkViewImage2D);
/*   vtkSyncSetMacro (Interpolate, int); */

  vtkSyncSetMacro (ColorWindow, double, vtkViewImage2D);
  vtkSyncSetMacro (ColorLevel, double, vtkViewImage2D);

  vtkSyncSetObjectMacro (OrientationMatrix, vtkMatrix4x4, vtkViewImage2D);
  vtkSyncSetObjectMacro (LookupTable, vtkLookupTable, vtkViewImage2D);
  vtkSyncSetObjectMacro (TextProperty, vtkTextProperty, vtkViewImage2D);
  vtkSyncSetObjectMacro (Input, vtkImageData, vtkViewImage2D);
  vtkSyncSetObjectMacro (InputConnection, vtkAlgorithmOutput, vtkViewImage2D);

  vtkSyncSetObjectMacro (Size, int, vtkViewImage2D);
  vtkSyncSetObjectMacro (Position, int, vtkViewImage2D);
  vtkSyncSetObjectMacro (WorldCoordinates, double, vtkViewImage2D);
  vtkSyncSetObjectMacro (Background, double, vtkViewImage2D);

  void SyncRender (void);
  void SyncResetWindowLevel (void);
  void SyncReset (void);

 protected:

  vtkViewImage2DCollection();
  ~vtkViewImage2DCollection();

  vtkViewImage2DCollectionCommand* Command;
  vtkRenderWindow* ExtraRenderWindow;
};




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




#endif /* _vtkViewImage2DCollection_h_ */
