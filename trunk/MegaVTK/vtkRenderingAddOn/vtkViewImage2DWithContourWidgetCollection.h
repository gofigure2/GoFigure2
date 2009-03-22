#ifndef __vtkViewImage2DWithContourWidgetCollection_h
#define __vtkViewImage2DWithContourWidgetCollection_h

#include <vtkCollection.h>
#include <vtkCommand.h>

#include "vtkViewImage2DCollection.h"
#include "vtkViewImage2DWithContourWidget.h"

class vtkViewImage2DWithContourWidgetCollectionCommand;

class vtkViewImage2DWithContourWidgetCollection :
public vtkCollection
{
public:
  static vtkViewImage2DWithContourWidgetCollection* New();
  vtkTypeRevisionMacro( vtkViewImage2DWithContourWidgetCollection,
    vtkCollection );

  // Description:
  // Get the next vtkViewImage2D in the list. Return NULL when at the end of the
  // list.
  vtkViewImage2DWithContourWidget *GetNextItem() {
    return static_cast<vtkViewImage2DWithContourWidget*>(
      this->GetNextItemAsObject() );};
  // Description:
  // Get the next vtkViewImage2D2D in the list. Return NULL when at the end of
  // the list.
  vtkViewImage2DWithContourWidget *GetItem(int i) {
    return static_cast<vtkViewImage2DWithContourWidget*>(
      this->GetItemAsObject(i) );};
  // Description:
  // Add an object to the list. Does not prevent duplicate entries.
  void AddItem(vtkViewImage2DWithContourWidget*);

  // Description:
  // Replace the i'th item in the collection with a
  void ReplaceItem(int i, vtkViewImage2DWithContourWidget *);

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
  void RemoveItem(vtkViewImage2DWithContourWidget *);

  // Description:
  // Remove all objects from the list.
  void RemoveAllItems();

  // Description:
  // Initialize the viewers togethers.
  void Initialize(void);

//   vtkGetObjectMacro (Command, vtkViewImage2DCollectionCommand);
  vtkGetObjectMacro (ExtraRenderWindow, vtkRenderWindow);
  void SetExtraRenderWindow (vtkRenderWindow* win)
  { this->ExtraRenderWindow = win; }

  vtkSyncSetMacro (Slice, int, vtkViewImage2DWithContourWidget );
/*   vtkSyncSetMacro (ViewOrientation, int); */
  vtkSyncSetMacro (SliceOrientation, int, vtkViewImage2DWithContourWidget);
  vtkSyncSetMacro (ShowAnnotations, int, vtkViewImage2DWithContourWidget);
  vtkSyncSetMacro (ShowScalarBar, int, vtkViewImage2DWithContourWidget);
/*   vtkSyncSetMacro (Interpolate, int); */

  vtkSyncSetMacro (ColorWindow, double, vtkViewImage2DWithContourWidget);
  vtkSyncSetMacro (ColorLevel, double, vtkViewImage2DWithContourWidget);

// //   vtkSyncSetMacro (Zoom, double, vtkViewImage2DWithContourWidget);

  vtkSyncSetObjectMacro (OrientationMatrix, vtkMatrix4x4,
    vtkViewImage2DWithContourWidget);
  vtkSyncSetObjectMacro (LookupTable, vtkLookupTable,
    vtkViewImage2DWithContourWidget);
  vtkSyncSetObjectMacro (TextProperty, vtkTextProperty,
    vtkViewImage2DWithContourWidget);
  vtkSyncSetObjectMacro (Input, vtkImageData, vtkViewImage2DWithContourWidget);
  vtkSyncSetObjectMacro (InputConnection, vtkAlgorithmOutput,
    vtkViewImage2DWithContourWidget);

  vtkSyncSetObjectMacro (Size, int, vtkViewImage2DWithContourWidget);
  vtkSyncSetObjectMacro (Position, int, vtkViewImage2DWithContourWidget);
  vtkSyncSetObjectMacro (WorldCoordinates, double,
    vtkViewImage2DWithContourWidget);
//   vtkSyncSetObjectMacro (Background, double, vtkViewImage2DWithContourWidget);

  void SyncSetBackground( double* rgb );
  void SyncRender (void);
  void SyncResetWindowLevel (void);
  void SyncReset (void);
  void SyncPan (void);
  void SyncMaskImage( void );
  void SyncMaskImage( vtkImageData* mask, vtkLookupTable* lut );
  void SyncSetZoomAndParallelScale( double Zoom, double ParallelScale );
//   void SyncSetCameraFocalAndZoomRatio( double focal[3], double d );

protected:
  vtkViewImage2DWithContourWidgetCollection();
  ~vtkViewImage2DWithContourWidgetCollection();

  vtkViewImage2DWithContourWidgetCollectionCommand* Command;
  vtkRenderWindow* ExtraRenderWindow;

private:
  vtkViewImage2DWithContourWidgetCollection(
    const vtkViewImage2DWithContourWidgetCollection& );
  void operator = ( const vtkViewImage2DWithContourWidgetCollection& );
};


class vtkViewImage2DWithContourWidgetCollectionCommand
: public vtkCommand
{

 public:

  static vtkViewImage2DWithContourWidgetCollectionCommand *New()
    {return new vtkViewImage2DWithContourWidgetCollectionCommand;};

  vtkViewImage2DWithContourWidgetCollection* GetCollection (void)
  { return this->Collection;};
  void SetCollection( vtkViewImage2DWithContourWidgetCollection* p );

  // Description:
  // Satisfy the superclass API for callbacks. Recall that the caller is
  // the instance invoking the event; eid is the event id (see
  // vtkCommand.h); and calldata is information sent when the callback
  // was invoked (e.g., progress value in the vtkCommand::ProgressEvent).
  virtual void Execute(vtkObject *caller,unsigned long event,
    void *vtkNotUsed(callData));

 protected:

  vtkViewImage2DWithContourWidgetCollectionCommand();
  ~vtkViewImage2DWithContourWidgetCollectionCommand(){};

 private:
  vtkViewImage2DWithContourWidgetCollection* Collection;

  double InitialWindow;
  double InitialLevel;

};

#endif
