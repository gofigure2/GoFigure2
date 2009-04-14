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

#ifndef __vtkViewImage2DWithContourWidget_h
#define __vtkViewImage2DWithContourWidget_h

#include "vtkViewImage2D.h"
#include "MegaVTK2Configure.h"

#include <vtkOrientedGlyphContourRepresentation.h>
#include <vtkContourWidget.h>
#include <vtkLookupTable.h>
#include <vtkCornerAnnotation.h>

class VTK_RENDERINGADDON2_EXPORT vtkViewImage2DWithContourWidget :
public vtkViewImage2D
{
public:
  static vtkViewImage2DWithContourWidget* New();
  vtkTypeRevisionMacro( vtkViewImage2DWithContourWidget, vtkViewImage2D );

  virtual void SetupInteractor( vtkRenderWindowInteractor* );

  void SetContourWidgetInteractionOn( )
  {
    this->ContourWidgetEnabled = true;
    this->ContourWidget->On();
    this->CornerAnnotation->SetText( 1, "Tracer ON");
  }
  void SetContourWidgetInteractionOff( )
  {
    this->ContourWidgetEnabled = false;
    this->ContourWidget->Off();
    this->CornerAnnotation->SetText( 1, "Tracer OFF");
  }
  void SetContourWidgetInteraction( const bool& iState )
  {
    if( iState )
      this->SetContourWidgetInteractionOn();
    else
      this->SetContourWidgetInteractionOff();
  }

  vtkSetMacro( CurrentLabel, unsigned int );
  vtkGetMacro( CurrentLabel, unsigned int );

  vtkGetMacro( ContourWidgetEnabled, bool );
  vtkGetObjectMacro( ContourWidget, vtkContourWidget );
  vtkGetObjectMacro( ContourRepresentation,
    vtkOrientedGlyphContourRepresentation );
  virtual void SetInput( vtkImageData* image );

protected:
  vtkViewImage2DWithContourWidget();
  ~vtkViewImage2DWithContourWidget();

  unsigned int CurrentLabel;
  bool ContourWidgetEnabled;
  vtkOrientedGlyphContourRepresentation* ContourRepresentation;
  vtkContourWidget* ContourWidget;
  vtkLookupTable* LUT;
  vtkImageData* Tracing;

private:
  vtkViewImage2DWithContourWidget( const vtkViewImage2DWithContourWidget& );
  void operator = ( const vtkViewImage2DWithContourWidget& );
};

#endif
