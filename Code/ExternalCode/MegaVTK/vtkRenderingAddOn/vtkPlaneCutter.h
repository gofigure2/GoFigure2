/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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

#ifndef __vtkPlaneCutter_h
#define __vtkPlaneCutter_h

#include "vtkCutter.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"

class vtkPlaneCutter : public vtkCutter
{
public:
  vtkTypeMacro(vtkPlaneCutter,vtkCutter);

  static vtkPlaneCutter *New();
  
protected:
  vtkPlaneCutter(vtkImplicitFunction *cf=NULL) : vtkCutter(cf) {}
  virtual ~vtkPlaneCutter() {}
  
  virtual int RequestData(
    vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
  {
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

    // get the input and output
    vtkDataSet *input = vtkDataSet::SafeDownCast(
      inInfo->Get(vtkDataObject::DATA_OBJECT()));
      
    if (!this->CutFunction)
      {
      vtkErrorMacro("No cut function specified");
      return 0;
      }

    if ( input->GetNumberOfPoints() < 1 )
      {
      return 1;
      }

    double bounds[6];
    input->GetBounds( bounds );
    
    double pt[8][3];
    int k = 0;
    pt[k][0] = bounds[0];
    pt[k][1] = bounds[2];
    pt[k++][2] = bounds[4];

    pt[k][0] = bounds[1];
    pt[k][1] = bounds[2];
    pt[k++][2] = bounds[4];
      
    pt[k][0] = bounds[0];
    pt[k][1] = bounds[3];
    pt[k++][2] = bounds[4];
    
    pt[k][0] = bounds[0];
    pt[k][1] = bounds[3];
    pt[k++][2] = bounds[4];
    
    pt[k][0] = bounds[0];
    pt[k][1] = bounds[2];
    pt[k++][2] = bounds[5];
    
    pt[k][0] = bounds[1];
    pt[k][1] = bounds[2];
    pt[k++][2] = bounds[5];
    
    pt[k][0] = bounds[0];
    pt[k][1] = bounds[3];
    pt[k++][2] = bounds[5];
    
    pt[k][0] = bounds[1];
    pt[k][1] = bounds[3];
    pt[k++][2] = bounds[5];
    
    bool sign0 = ( this->CutFunction->FunctionValue( pt[0] ) > 0. );
    bool intersect = false;
    
    for( unsigned int i = 1; ( i < 8 ) && ( !intersect ); i++ )
    {
      bool sign = ( this->CutFunction->FunctionValue( pt[i] ) >= 0. );
      if( sign != sign0 )
      {
        intersect = true;
      }
    }
    
    if( intersect )
      {
      return vtkCutter::RequestData( request, inputVector, outputVector );
      }
    else
      {
      return 1;
      }
  }
  
private:
  vtkPlaneCutter(const vtkPlaneCutter&);
  void operator = (const vtkPlaneCutter&);
};

vtkStandardNewMacro(vtkPlaneCutter);
#endif

