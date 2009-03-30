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

#ifndef _vtkOrientedBoxWidget_h_
#define _vtkOrientedBoxWidget_h_

#include "vtkBoxWidget.h"
#include "MegaVTK2Configure.h"

class vtkMatrix4x4;

/**
   \author Nicolas Toussaint
   \class vtkOrientedBoxWidget
   \brief orthogonal hexahedron 3D widget with pre-defined orientation

   Description

   This 3D widget defines a region of interest. By default it behaves exactly as
   the vtkBoxWidget class does. But if the user set the Orientation matrix
   with SetOrientationMatrix() then all the actors of the widgets (handles,
   hexahedron, etc) will be oriented according to the argument.

   \see vtk3DWidget vtkBoxWidget
 */
class VTK_RENDERINGADDON2_EXPORT vtkOrientedBoxWidget : public vtkBoxWidget
{

 public:

  static vtkOrientedBoxWidget* New();
  vtkTypeRevisionMacro(vtkOrientedBoxWidget, vtkBoxWidget);

  vtkGetObjectMacro (OrientationMatrix, vtkMatrix4x4);
  virtual void SetOrientationMatrix (vtkMatrix4x4* matrix);

 protected:

  vtkOrientedBoxWidget();
  ~vtkOrientedBoxWidget();

  virtual void OnMouseMove();

  vtkMatrix4x4* OrientationMatrix;
  vtkMatrix4x4* InvertedOrientationMatrix;


};

#endif /* _vtkOrientedBoxWidget_h_ */
