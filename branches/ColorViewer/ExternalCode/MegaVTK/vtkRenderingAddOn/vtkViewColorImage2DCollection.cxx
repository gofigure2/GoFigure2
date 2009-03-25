/*========================================================================
 Copyright (c) INRIA - ASCLEPIOS Project (http://www-sop.inria.fr/asclepios).
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of INRIA or ASCLEPIOS, nor the names of any contributors
 may be used to endorse or promote products derived from this software 
 without specific prior written permission.
 
 * Modified source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 =========================================================================*/

/*=========================================================================
 Modifications were made by the GoFigure Dev. Team.
 while at Megason Lab, Systems biology, Harvard Medical school, 2009 
 
 Copyright (c) 2009, President and Fellows of Harvard College.
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 Redistributions of source code must retain the above copyright notice, 
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation 
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College 
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written 
 permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 =========================================================================*/

#include "vtkViewColorImage2DCollection.h"

#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkImageActor.h"
#include "vtkImageData.h"
#include "vtkInteractorStyleImage.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkMatrix4x4.h"
#include "vtkOrientationAnnotation.h"
#include "vtkCornerAnnotation.h"
#include "vtkTextProperty.h"
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

#include "vtkViewColorImage2DCommand.h"
#include "vtkInteractorStyleImage2D.h"

#include <vector>
#include <string>
#include <sstream>

vtkCxxRevisionMacro(vtkViewColorImage2DCollection, "$Revision: 2 $");
vtkStandardNewMacro(vtkViewColorImage2DCollection);


//----------------------------------------------------------------------------
vtkViewColorImage2DCollection::vtkViewColorImage2DCollection()
{
  this->Command = vtkViewColorImage2DCollectionCommand::New();
  this->Command->SetCollection (this);
  this->ExtraRenderWindow = NULL;
}

//----------------------------------------------------------------------------
vtkViewColorImage2DCollection::~vtkViewColorImage2DCollection()
{
  this->Command->Delete();
}


// Add an object to the list. Does not prevent duplicate entries.
void vtkViewColorImage2DCollection::AddItem(vtkViewColorImage2D *a)
{
  this->Superclass::AddItem (a);

  a->GetInteractorStyle()->AddObserver (vtkViewColorImage2DCommand::SliceMoveEvent,
    this->Command);
  a->GetInteractorStyle()->AddObserver (
    vtkViewColorImage2DCommand::RequestedPositionEvent, this->Command);
  a->GetInteractorStyle()->AddObserver (
    vtkViewColorImage2DCommand::ResetViewerEvent, this->Command);

}

// Remove an object from the list. Removes the first object found, not
// all occurrences. If no object found, list is unaffected.  See warning
// in description of RemoveItem(int).
void vtkViewColorImage2DCollection::RemoveItem(vtkViewColorImage2D *a)
{
  this->Superclass::RemoveItem (a);
}

// Remove all objects from the list.
void vtkViewColorImage2DCollection::RemoveAllItems()
{
  this->Superclass::RemoveAllItems ();
}


// Replace the i'th item in the collection with a
void vtkViewColorImage2DCollection::ReplaceItem(int i, vtkViewColorImage2D *a)
{
  this->Superclass::ReplaceItem (i, a);
}


// Remove the i'th item in the list.
// Be careful if using this function during traversal of the list using
// GetNextItemAsObject (or GetNextItem in derived class).  The list WILL
// be shortened if a valid index is given!  If this->Current is equal to the
// element being removed, have it point to then next element in the list.
void vtkViewColorImage2DCollection::RemoveItem(int i)
{
  this->Superclass::RemoveItem (i);
}


void vtkViewColorImage2DCollection::Initialize(void)
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

void vtkViewColorImage2DCollection::SyncRender(void)
{
  this->InitTraversal();
  vtkViewColorImage2D* item = this->GetNextItem();
  while(item)
  {
    item->Render ();
    item = this->GetNextItem();
  }
  if (this->ExtraRenderWindow)
    this->ExtraRenderWindow->Render();
}


void vtkViewColorImage2DCollection::SyncReset(void)
{
  this->InitTraversal();
  vtkViewColorImage2D* item = this->GetNextItem();
  while(item)
  {
    item->Reset ();
    item = this->GetNextItem();
  }
}

//----------------------------------------------------------------------------
vtkViewColorImage2DCollectionCommand::vtkViewColorImage2DCollectionCommand()
{
}

//----------------------------------------------------------------------------
void vtkViewColorImage2DCollectionCommand::SetCollection(vtkViewColorImage2DCollection* p)
{
  this->Collection = p;
}

//----------------------------------------------------------------------------
void vtkViewColorImage2DCollectionCommand::Execute(vtkObject *caller,
  unsigned long event,
  void *vtkNotUsed(callData))
{
  if (!this->Collection)
    return;

  vtkInteractorStyleImage2D *isi =
    vtkInteractorStyleImage2D::SafeDownCast(caller);
  this->GetCollection()->InitTraversal();
  vtkViewColorImage2D* v = this->GetCollection()->GetNextItem();
  vtkViewColorImage2D* viewer = NULL;
  while(v)
  {
    if (isi == v->GetInteractorStyle())
      viewer = v;
    v = this->GetCollection()->GetNextItem();
  }

  if (!isi || !viewer || !viewer->GetInput())
    return;

  // Reset
  if (event == vtkViewColorImage2DCommand::ResetViewerEvent)
  {
    this->Collection->SyncReset();
    this->Collection->SyncRender();
    return;
  }

  // Move
  if (event == vtkViewColorImage2DCommand::SliceMoveEvent)
  {
    // do not synchronize this, but render all
    this->Collection->SyncRender();

  }

  // Position requested
  if (event == vtkViewColorImage2DCommand::RequestedPositionEvent)
  {
    double* position = viewer->GetWorldCoordinatesFromDisplayPosition (
      isi->GetRequestedPosition ());
    this->Collection->SyncSetWorldCoordinates(position);
    this->Collection->SyncRender();
  }
}


