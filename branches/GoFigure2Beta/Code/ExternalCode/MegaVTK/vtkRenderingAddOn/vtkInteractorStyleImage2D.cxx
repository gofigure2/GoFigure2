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

#include "vtkInteractorStyleImage2D.h"

#include "vtkAbstractPropPicker.h"
#include "vtkAssemblyPath.h"
#include "vtkMath.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
#include <vtkImageData.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include "vtkViewImage2DCommand.h"


vtkCxxRevisionMacro (vtkInteractorStyleImage2D, "$Revision: 490 $");
vtkStandardNewMacro (vtkInteractorStyleImage2D);


//----------------------------------------------------------------------------
vtkInteractorStyleImage2D::vtkInteractorStyleImage2D()
{
  this->SliceStep = 0;
  this->RequestedPosition = new int[2];
  this->RequestedPosition[0] = this->RequestedPosition[1] = 0;
}

//----------------------------------------------------------------------------
vtkInteractorStyleImage2D::~vtkInteractorStyleImage2D()
{
  delete this->RequestedPosition;
}

//----------------------------------------------------------------------------
void vtkInteractorStyleImage2D::OnMouseMove()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  switch (this->State)
  {
  case VTKIS_SLICE_MOVE:
    this->FindPokedRenderer(x, y);
    this->SliceMove();
    this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
    this->Superclass::OnMouseMove();
    break;
  case VTKIS_DOLLY:
    this->InvokeEvent(vtkViewImage2DCommand::ZoomEvent);
    break;
  case VTKIS_PAN:
    this->InvokeEvent(vtkViewImage2DCommand::PanEvent);
    break;
  default:
    this->Superclass::OnMouseMove();
    break;
  }

//   this->RequestedPosition = this->Interactor->GetEventPosition();
  this->InvokeEvent (vtkCommand::InteractionEvent);

//
/// NOTE:12 MAR 2009
/// This is a really dirty solution: it breaks the "natural" behavior of the vtkViewImage2D
/// alone (not in a Collection: synchronized view);
// this->Superclass::OnMouseMove();
}


//----------------------------------------------------------------------------
void vtkInteractorStyleImage2D::OnLeftButtonDown()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);
  if ( !this->CurrentRenderer )
    {
    return;
    }

  if (this->Interactor->GetRepeatCount())
  {
    this->RequestedPosition[0] = x;
    this->RequestedPosition[1] = y;
    this->InvokeEvent (vtkViewImage2DCommand::RequestedPositionEvent);
  }
  else
  {
    this->InvokeEvent( vtkViewImage2DCommand::ContourPickingEvent );
  }

  this->Superclass::OnLeftButtonDown();
}


//----------------------------------------------------------------------------
void vtkInteractorStyleImage2D::OnLeftButtonUp()
{
  this->Superclass::OnLeftButtonUp();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleImage2D::OnMiddleButtonDown()
{
  int x = this->Interactor->GetEventPosition()[0];
  int y = this->Interactor->GetEventPosition()[1];

  this->FindPokedRenderer(x, y);

  if ( !this->CurrentRenderer )
    {
    return;
    }

  this->Superclass::OnMiddleButtonDown();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleImage2D::OnMiddleButtonUp()
{
  this->Superclass::OnMiddleButtonUp();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleImage2D::OnRightButtonDown()
{
  this->Superclass::OnRightButtonDown();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleImage2D::OnRightButtonUp()
{
  this->Superclass::OnRightButtonUp();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleImage2D::OnChar()
{
  vtkRenderWindowInteractor *rwi = this->Interactor;

  switch( rwi->GetKeyCode() )
  {
    case 'o':
    {
      this->InvokeEvent (vtkViewImage2DCommand::ResetViewerEvent, this);
      break;
    }
    case 'r':
    {
      this->InvokeEvent (vtkViewImage2DCommand::ResetWindowLevelEvent, this);
      break;
    }
  }
  this->Superclass::OnChar();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleImage2D::OnKeyUp()
{
 vtkRenderWindowInteractor *rwi = this->Interactor;

  if (!strcmp (rwi->GetKeySym(),"Up" ) )
  {
    this->SliceStep = 1;
    this->StartSliceMove();
    this->SliceMove();
    this->EndSliceMove();
  }
  else if(!strcmp (rwi->GetKeySym(),"Down" ) )
  {
    this->SliceStep = -1;
    this->StartSliceMove();
    this->SliceMove();
    this->EndSliceMove();
  }
  else if (!strcmp (rwi->GetKeySym(),"o" ) )
  {
    this->InvokeEvent (vtkViewImage2DCommand::ResetViewerEvent, this );
  }
  else if (!strcmp (rwi->GetKeySym(),"r" ) )
  {
    this->InvokeEvent (vtkViewImage2DCommand::ResetWindowLevelEvent, this );
  }


  this->Superclass::OnKeyUp();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleImage2D::OnKeyPress()
{
  this->Superclass::OnKeyPress();
}

//----------------------------------------------------------------------------
void vtkInteractorStyleImage2D::OnKeyRelease()
{
  this->Superclass::OnKeyRelease();
}

//----------------------------------------------------------------------------
void
vtkInteractorStyleImage2D::StartSliceMove()
{
  if ((this->State != VTKIS_NONE) && (this->State != VTKIS_PICK))
  {
    return;
  }
  this->StartState(VTKIS_SLICE_MOVE);
  this->InvokeEvent(vtkViewImage2DCommand::StartSliceMoveEvent, this);
}

//----------------------------------------------------------------------------
void
vtkInteractorStyleImage2D::EndSliceMove()
{
  if (this->State != VTKIS_SLICE_MOVE)
  {
    return;
  }
  this->InvokeEvent(vtkViewImage2DCommand::EndSliceMoveEvent, this);
  this->StopState();
}

//----------------------------------------------------------------------------
void
vtkInteractorStyleImage2D::SliceMove()
{
  if (this->State != VTKIS_SLICE_MOVE)
  {
    return;
  }
  this->InvokeEvent(vtkViewImage2DCommand::SliceMoveEvent, this);
}

void
vtkInteractorStyleImage2D::OnMouseWheelForward()
{
  this->StartSliceMove();
  this->SliceStep = static_cast< int >( this->MouseWheelMotionFactor );
  this->SliceMove();
  this->EndSliceMove();
}
void
vtkInteractorStyleImage2D::OnMouseWheelBackward()
{
  this->StartSliceMove();
  this->SliceStep = static_cast< int >( -this->MouseWheelMotionFactor );
  this->SliceMove();
  this->EndSliceMove();
}

// void vtkInteractorStyleImage2D::PropagateCameraFocalAndPosition()
// {
//   if( !this->GetView() )
//   {
//     return;
//   }

//   //double pos[3], focal[3];

//   if( !this->GetView()->GetRenderer() )
//   {
//     return;
//   }

//   vtkCamera* camera = this->GetView()->GetRenderer()->GetActiveCamera();
//   double* focal = camera->GetFocalPoint ();
//   double* pos   = camera->GetPosition ();

//   bool LinkCamera = this->GetView()->GetLinkCameraFocalAndPosition();
//   this->GetView()->SetLinkCameraFocalAndPosition (false);
//   this->GetView()->SyncSetCameraFocalAndPosition (focal, pos);
//   this->GetView()->SetLinkCameraFocalAndPosition ( LinkCamera );

//   //this->GetView()->Render();


// }
