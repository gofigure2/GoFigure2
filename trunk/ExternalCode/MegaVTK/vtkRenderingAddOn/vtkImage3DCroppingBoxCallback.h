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

#ifndef _vtk_Image3DCroppingBoxCallback_h_
#define _vtk_Image3DCroppingBoxCallback_h_

#include <vtkCommand.h>
#include <vtkVolumeMapper.h>
#include <vtkSetGet.h>
#include <vtkObjectFactory.h>

class vtkImage3DCroppingBoxCallback: 
public vtkCommand
{

 public:
  static vtkImage3DCroppingBoxCallback* New()
  { return new vtkImage3DCroppingBoxCallback; }

  virtual void Execute ( vtkObject *caller, unsigned long, void* );

  void SetVolumeMapper (vtkVolumeMapper* mapper)
  {
    this->VolumeMapper = mapper;
  }
  vtkVolumeMapper* GetVolumeMapper (void) const
  {
    return this->VolumeMapper;
  }


 protected:
  vtkImage3DCroppingBoxCallback() : VolumeMapper( 0 ) {}
  ~vtkImage3DCroppingBoxCallback(){}

 private:

  vtkVolumeMapper* VolumeMapper;

};


#endif
