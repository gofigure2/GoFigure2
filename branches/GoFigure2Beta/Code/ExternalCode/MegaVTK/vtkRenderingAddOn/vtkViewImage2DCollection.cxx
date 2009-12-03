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

#include "vtkViewImage2DCollection.h"

#include "vtkSmartPointer.h"
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
#include "vtkViewImage2DCollectionCommand.h"
#include "vtkInteractorStyleImage2D.h"

#include "vtkCellPicker.h"

#include <vector>
#include <string>
#include <sstream>

vtkCxxRevisionMacro(vtkViewImage2DCollection, "$Revision: 490 $");
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

//----------------------------------------------------------------------------
void
vtkViewImage2DCollection::
InitializeAllObservers()
{
  this->InitTraversal();
  vtkViewImage2D* a = this->GetNextItem();
  while(a)
    {
    if( a->GetIsColor() )
      {
      a->GetInteractorStyle()->RemoveObservers(
        vtkCommand::ResetWindowLevelEvent );
      a->GetInteractorStyle()->RemoveObservers(
        vtkCommand::WindowLevelEvent );
      }
    a->GetInteractorStyle()->RemoveObservers(
      vtkViewImage2DCommand::SliceMoveEvent );
    a->GetInteractorStyle()->RemoveObservers(
      vtkViewImage2DCommand::ResetViewerEvent );
    a->GetInteractorStyle()->RemoveObservers( vtkViewImage2DCommand::
      RequestedPositionEvent );

    if( !a->GetIsColor() )
      {
      a->GetInteractorStyle()->AddObserver( vtkCommand::ResetWindowLevelEvent,
        this->Command );
      a->GetInteractorStyle()->AddObserver( vtkCommand::WindowLevelEvent,
        this->Command );
      }
    a->GetInteractorStyle()->AddObserver(
      vtkViewImage2DCommand::SliceMoveEvent,
      this->Command );
    a->GetInteractorStyle()->AddObserver(
      vtkViewImage2DCommand::ZoomEvent,
      this->Command );
    a->GetInteractorStyle()->AddObserver(
      vtkViewImage2DCommand::PanEvent,
      this->Command );
    a->GetInteractorStyle()->AddObserver(
      vtkViewImage2DCommand::RequestedPositionEvent,
      this->Command );
    a->GetInteractorStyle()->AddObserver(
      vtkViewImage2DCommand::ResetViewerEvent,
      this->Command );
    a->GetInteractorStyle()->AddObserver(
      vtkViewImage2DCommand::ContourPickingEvent,
      this->Command );
    a = this->GetNextItem();
    }
}

//----------------------------------------------------------------------------
// Add an object to the list. Does not prevent duplicate entries.
void vtkViewImage2DCollection::AddItem(vtkViewImage2D *a)
{
  this->Superclass::AddItem (a);
}

//----------------------------------------------------------------------------
// Remove an object from the list. Removes the first object found, not
// all occurrences. If no object found, list is unaffected.  See warning
// in description of RemoveItem(int).
void vtkViewImage2DCollection::RemoveItem(vtkViewImage2D *a)
{
  this->Superclass::RemoveItem (a);
}

//----------------------------------------------------------------------------
// Remove all objects from the list.
void vtkViewImage2DCollection::RemoveAllItems()
{
  this->Superclass::RemoveAllItems ();
}

//----------------------------------------------------------------------------
// Replace the i'th item in the collection with a
void vtkViewImage2DCollection::ReplaceItem(int i, vtkViewImage2D *a)
{
  this->Superclass::ReplaceItem (i, a);
}

//----------------------------------------------------------------------------
// Remove the i'th item in the list.
// Be careful if using this function during traversal of the list using
// GetNextItemAsObject (or GetNextItem in derived class).  The list WILL
// be shortened if a valid index is given!  If this->Current is equal to the
// element being removed, have it point to then next element in the list.
void vtkViewImage2DCollection::RemoveItem(int i)
{
  this->Superclass::RemoveItem (i);
}

//----------------------------------------------------------------------------
void vtkViewImage2DCollection::Initialize()
{
  vtkSmartPointer< vtkProperty > plane_property =
    vtkSmartPointer< vtkProperty >::New();
  plane_property->SetRepresentationToWireframe();

  for (int i=0; i<this->GetNumberOfItems(); i++)
  {
    for (int j=0; j<this->GetNumberOfItems(); j++)
    {
      vtkActor* temp = this->GetItem( j )->AddDataSet(
        static_cast<vtkDataSet*>( this->GetItem( i )->GetSlicePlane() ),
        plane_property, ( i != j ) );
      temp->Delete();
    }
  }
}

//----------------------------------------------------------------------------
void vtkViewImage2DCollection::
SyncSetBackground( double* rgb )
{
  this->InitTraversal();
  vtkViewImage2D* item = this->GetNextItem();
  while(item)
  {
    item->SetBackground( rgb );
    item = this->GetNextItem();
  }
  if (this->ExtraRenderWindow)
  {
    vtkRenderer* ren =
      this->ExtraRenderWindow->GetRenderers()->GetFirstRenderer();
    ren->SetBackground( rgb );
  }
}

//----------------------------------------------------------------------------
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

//----------------------------------------------------------------------------
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
//----------------------------------------------------------------------------
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
void vtkViewImage2DCollection::
SyncPan()
{
  this->InitTraversal();
  vtkViewImage2D* item = this->GetNextItem();
  while(item)
  {
    item = this->GetNextItem();
  }
}
//----------------------------------------------------------------------------
void vtkViewImage2DCollection::
SyncSetZoomAndParallelScale( double Zoom, double ParallelScale )
{
  this->InitTraversal();
  vtkViewImage2D* item = this->GetNextItem();
  double t = ParallelScale / Zoom;
  while(item)
  {
    item->SetZoom( Zoom );
    item->GetRenderer()->GetActiveCamera()->SetParallelScale( t );

    if( item->GetInteractorStyle()->GetInteractor()->GetLightFollowCamera() )
    {
      item->GetRenderer()->UpdateLightsGeometryToFollowCamera();
    }
    item->Render();
    item = this->GetNextItem();
  }
}

