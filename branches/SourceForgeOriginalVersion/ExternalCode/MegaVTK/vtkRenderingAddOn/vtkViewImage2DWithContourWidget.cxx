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

#include "vtkViewImage2DWithContourWidget.h"

#include <vtkObjectFactory.h>
#include <vtkImageActorPointPlacer.h>
#include <vtkCornerAnnotation.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>

#include "vtkFillImageWithPolyData.h"

vtkCxxRevisionMacro(vtkViewImage2DWithContourWidget, "$Revision$");
vtkStandardNewMacro(vtkViewImage2DWithContourWidget);


vtkViewImage2DWithContourWidget::vtkViewImage2DWithContourWidget()
{
  this->ContourWidgetEnabled = false;

  this->ContourRepresentation = vtkOrientedGlyphContourRepresentation::New();

  this->ContourWidget = vtkContourWidget::New();
  this->ContourWidget->SetPriority( 10.0 );

  this->CornerAnnotation->SetText( 1, "Tracer OFF");

  this->CurrentLabel = 1;

  this->LUT = vtkLookupTable::New();
  this->LUT->SetNumberOfTableValues( 256 );

  this->LUT->SetTableValue (1, 1.0, 0.0, 0.0, 0.5);
  this->LUT->SetTableValue (2, 0.0, 1.0, 0.0, 0.5);
  this->LUT->SetTableValue (3, 0.0, 0.0, 1.0, 0.5);
  this->LUT->SetTableValue (4, 1.0, 1.0, 0.0, 0.5);
  this->LUT->SetTableValue (5, 0.0, 1.0, 1.0, 0.5);
  this->LUT->SetTableValue (6, 1.0, 0.0, 1.0, 0.5);

  double d_id;
  for( int i = 7; i < 256; i++ )
  {
    d_id = static_cast< double >( i );
    if( i % 3 == 0 )
    {
    this->LUT->SetTableValue( i,
      ( 200. * ( 84.0 - d_id ) / 85. + 50. ) / 255.,
      ( 200. * d_id / 85. + 50. ) / 255.,
      0.,
      0.5);
    }
    else
    {
      if( i % 3 == 1 )
      {
        this->LUT->SetTableValue( i,
          0,
          ( ( 169. - d_id ) / 85. * 200. + 50. ) / 255.,
          ( ( d_id - 85. ) / 85. * 200. + 50. ) / 255.,
          0.5 );
      }
      else
      {
        this->LUT->SetTableValue( i,
          ( ( d_id -170. ) / 85.0 * 200.0 + 50. ) / 255.0,
          0.0,
          ( ( 255.0 - d_id ) / 85.0 * 200.0 + 50. ) /255.0,
          0.5);
      }
    }
  }
}

vtkViewImage2DWithContourWidget::~vtkViewImage2DWithContourWidget()
{
  this->ContourRepresentation->Delete();
  this->ContourWidget->Delete();
  this->LUT->Delete();
}

void vtkViewImage2DWithContourWidget::SetupInteractor(
  vtkRenderWindowInteractor* arg )
{
  vtkViewImage2D::SetupInteractor( arg );
  if( arg )
  {
    this->ContourWidget->SetInteractor( arg );
  }
}

void vtkViewImage2DWithContourWidget::SetInput( vtkImageData* image )
{
  vtkViewImage2D::SetInput( image );
  if( image )
  {
    vtkImageActorPointPlacer* point_placer = vtkImageActorPointPlacer::New();
    point_placer->SetImageActor( this->GetImageActor() );

    this->ContourRepresentation->SetPointPlacer( point_placer );
    this->ContourWidget->SetRepresentation(this->ContourRepresentation);

    point_placer->Delete();
    this->SetContourWidgetInteractionOff();
  }
}

