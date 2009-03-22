#include "vtkViewImage2DCollection.h"

#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkImageMapToWindowLevelColors.h"
#include "vtkInteractorStyleImage.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkMatrix4x4.h"
#include "vtkScalarBarActor.h"
#include "vtkOrientationAnnotation.h"
#include "vtkCornerAnnotation.h"
#include "vtkTextProperty.h"
#include "vtkLookupTable.h"
#include "vtkMath.h"
#include "vtkPlane.h"
#include "vtkCutter.h"
#include "vtkActor.h"
#include "vtkPolyDataMapper.h"
#include "vtkProp3DCollection.h"
#include "vtkDataSetCollection.h"
#include "vtkPoints.h"
#include "vtkIdList.h"
#include "vtkOutlineSource.h"
#include "vtkMatrixToLinearTransform.h"
#include "vtkPointData.h"
#include "vtkUnsignedCharArray.h"
#include "vtkIntArray.h"
#include "vtkImageAccumulate.h"
#include "vtkProperty.h"

#include "vtkViewImage2DCommand.h"
#include "vtkInteractorStyleImage2D.h"

#include <vector>
#include <string>
#include <sstream>

vtkCxxRevisionMacro(vtkViewImage2DCollection, "$Revision: 2 $");
vtkStandardNewMacro(vtkViewImage2DCollection);


//----------------------------------------------------------------------------
vtkViewImage2DCollection::vtkViewImage2DCollection()
{
  this->Command = vtkViewImage2DCollectionCommand::New();
  this->Command->SetCollection (this);
  this->ExtraRenderWindow = NULL;
}

//----------------------------------------------------------------------------
vtkViewImage2DCollection::~vtkViewImage2DCollection()
{
  this->Command->Delete();
}


// Add an object to the list. Does not prevent duplicate entries.
void vtkViewImage2DCollection::AddItem(vtkViewImage2D *a)
{
  this->Superclass::AddItem (a);

  a->GetInteractorStyle()->AddObserver (vtkCommand::ResetWindowLevelEvent,
    this->Command);
  a->GetInteractorStyle()->AddObserver (vtkCommand::WindowLevelEvent,
    this->Command);
  a->GetInteractorStyle()->AddObserver (vtkViewImage2DCommand::SliceMoveEvent,
    this->Command);
  a->GetInteractorStyle()->AddObserver (
    vtkViewImage2DCommand::RequestedPositionEvent, this->Command);
  a->GetInteractorStyle()->AddObserver (
    vtkViewImage2DCommand::ResetViewerEvent, this->Command);

}

// Remove an object from the list. Removes the first object found, not
// all occurrences. If no object found, list is unaffected.  See warning
// in description of RemoveItem(int).
void vtkViewImage2DCollection::RemoveItem(vtkViewImage2D *a)
{
  this->Superclass::RemoveItem (a);
}

// Remove all objects from the list.
void vtkViewImage2DCollection::RemoveAllItems()
{
  this->Superclass::RemoveAllItems ();
}


// Replace the i'th item in the collection with a
void vtkViewImage2DCollection::ReplaceItem(int i, vtkViewImage2D *a)
{
  this->Superclass::ReplaceItem (i, a);
}


// Remove the i'th item in the list.
// Be careful if using this function during traversal of the list using
// GetNextItemAsObject (or GetNextItem in derived class).  The list WILL
// be shortened if a valid index is given!  If this->Current is equal to the
// element being removed, have it point to then next element in the list.
void vtkViewImage2DCollection::RemoveItem(int i)
{
  this->Superclass::RemoveItem (i);
}


void vtkViewImage2DCollection::Initialize(void)
{
  vtkProperty* plane_property = vtkProperty::New();
  plane_property->SetRepresentationToWireframe();

  for (int i=0; i<this->GetNumberOfItems(); i++)
  {
    for (int j=0; j<this->GetNumberOfItems(); j++)
    {
      this->GetItem (j)->AddDataSet(
        this->GetItem (i)->GetSlicePlane(), plane_property, ( i != j ) );
    }
  }
  plane_property->Delete();
}

void vtkViewImage2DCollection::SyncRender(void)
{
  this->InitTraversal();
  vtkViewImage2D* item = this->GetNextItem();
  while(item)
  {
    item->Render ();
    item = this->GetNextItem();
  }
  if (this->ExtraRenderWindow)
    this->ExtraRenderWindow->Render();
}


void vtkViewImage2DCollection::SyncReset(void)
{
  this->InitTraversal();
  vtkViewImage2D* item = this->GetNextItem();
  while(item)
  {
    item->Reset ();
    item = this->GetNextItem();
  }
}

void vtkViewImage2DCollection::SyncResetWindowLevel(void)
{
  this->InitTraversal();
  vtkViewImage2D* item = this->GetNextItem();
  while(item)
  {
    item->ResetWindowLevel ();
    item = this->GetNextItem();
  }
}




//----------------------------------------------------------------------------
vtkViewImage2DCollectionCommand::vtkViewImage2DCollectionCommand()
{
}

//----------------------------------------------------------------------------
void vtkViewImage2DCollectionCommand::SetCollection(vtkViewImage2DCollection* p)
{
  this->Collection = p;
}

//----------------------------------------------------------------------------
void vtkViewImage2DCollectionCommand::Execute(vtkObject *caller,
  unsigned long event,
  void *vtkNotUsed(callData))
{
  if (!this->Collection)
    return;

  vtkInteractorStyleImage2D *isi =
    vtkInteractorStyleImage2D::SafeDownCast(caller);
  this->GetCollection()->InitTraversal();
  vtkViewImage2D* v = this->GetCollection()->GetNextItem();
  vtkViewImage2D* viewer = NULL;
  while(v)
  {
    if (isi == v->GetInteractorStyle())
      viewer = v;
    v = this->GetCollection()->GetNextItem();
  }

  if (!isi || !viewer || !viewer->GetInput())
    return;

  // Reset
  if (event == vtkCommand::ResetWindowLevelEvent)
  {
    this->Collection->SyncResetWindowLevel();
    this->Collection->SyncRender();
    return;
  }
  // Reset
  if (event == vtkViewImage2DCommand::ResetViewerEvent)
  {
    this->Collection->SyncReset();
    this->Collection->SyncRender();
    return;
  }

  // Adjust the window level here
  if (event == vtkCommand::WindowLevelEvent)
  {
    this->Collection->SyncSetColorWindow(viewer->GetColorWindow());
    this->Collection->SyncSetColorLevel(viewer->GetColorLevel());
    this->Collection->SyncRender();
  }

  // Move
  if (event == vtkViewImage2DCommand::SliceMoveEvent)
  {
    // do not synchronize this, but render all
    this->Collection->SyncRender();

  }

  // Position requested
  if (event == vtkViewImage2DCommand::RequestedPositionEvent)
  {
    double* position = viewer->GetWorldCoordinatesFromDisplayPosition (
      isi->GetRequestedPosition ());
    this->Collection->SyncSetWorldCoordinates(position);
    this->Collection->SyncRender();
  }


}


