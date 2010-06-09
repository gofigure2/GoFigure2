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

#include "vtkInteractorStyleImage3D.h"

#include <vtkObjectFactory.h>
#include <vtkRenderWindowInteractor.h>
#include "vtkViewImage3DCommand.h"

#include <vtkProp.h>

vtkCxxRevisionMacro (vtkInteractorStyleImage3D, "$Revision: 1 $");
vtkStandardNewMacro (vtkInteractorStyleImage3D);

//----------------------------------------------------------------------------
vtkInteractorStyleImage3D::
vtkInteractorStyleImage3D()
{
  m_EnablePickingMode = false;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkInteractorStyleImage3D::
~vtkInteractorStyleImage3D()
{
}
//--------------------------------------------------------------------------
//----------------------------------------------------------------------------
void
vtkInteractorStyleImage3D::
OnLeftButtonDown()
{
  // if object is picked, send the event
  if( m_EnablePickingMode )
    {
    this->SetCurrentProp();
    this->InvokeEvent(vtkViewImage3DCommand::MeshPickingEvent);
    }

  if( m_EnableBoxSelectionMode )
    {
    this->InvokeEvent(vtkViewImage3DCommand::BoxPickingEvent);
    }

  // Call parent to handle all other states and perform additional work
  this->Superclass::OnLeftButtonDown();
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void
vtkInteractorStyleImage3D::
OnChar()
{
  vtkRenderWindowInteractor *rwi = this->Interactor;
  if ((rwi->GetKeyCode() == 'p') || (rwi->GetKeyCode() == 'P'))
    {
      this->m_EnablePickingMode = true;
    }
  if ((rwi->GetKeyCode() == 'b') || (rwi->GetKeyCode() == 'B'))
    {
      this->m_EnableBoxSelectionMode = true;
    }

  this->Superclass::OnChar();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void
vtkInteractorStyleImage3D::
OnKeyUp()
{
  vtkRenderWindowInteractor *rwi = this->Interactor;

  if ((rwi->GetKeyCode() == 'p') || (rwi->GetKeyCode() == 'P'))
    {
    this->m_EnablePickingMode = false;
    }
  if ((rwi->GetKeyCode() == 'b') || (rwi->GetKeyCode() == 'B'))
    {
    this->m_EnableBoxSelectionMode = false;
    }

  this->Superclass::OnKeyUp();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void
vtkInteractorStyleImage3D::
SetLeftButtonInteraction( InteractionTypeIds interactionType)
{
  LeftButtonInteraction = interactionType;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void
vtkInteractorStyleImage3D::
SetRightButtonInteraction( InteractionTypeIds interactionType)
{
  RightButtonInteraction = interactionType;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void
vtkInteractorStyleImage3D::
SetMiddleButtonInteraction( InteractionTypeIds interactionType)
{
  MiddleButtonInteraction = interactionType;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void
vtkInteractorStyleImage3D::
SetWheelButtonInteraction( InteractionTypeIds interactionType)
{
  WheelButtonInteraction = interactionType;
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void
vtkInteractorStyleImage3D::
SetCurrentProp()
{
  this->m_CurrentProp = this->CurrentProp;
}

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
vtkProp*
vtkInteractorStyleImage3D::
GetCurrentProp()
{
  return this->m_CurrentProp;
}

