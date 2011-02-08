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

#include "vtkPolyDataMySQLTrackWriter.h"

#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkMath.h"
#include "vtkIdList.h"

#include "vtkIntArray.h"
#include "vtkPointData.h"

#include "vtkSmartPointer.h"

vtkCxxRevisionMacro(vtkPolyDataMySQLTrackWriter, "$Revision$");
vtkStandardNewMacro(vtkPolyDataMySQLTrackWriter);

//--------------------------------------------------------------------------
vtkPolyDataMySQLTrackWriter::vtkPolyDataMySQLTrackWriter()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyDataMySQLTrackWriter::
~vtkPolyDataMySQLTrackWriter()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string
vtkPolyDataMySQLTrackWriter::GetMySQLText(vtkPolyData *iPolyData)
{
  vtkIdType N = iPolyData->GetNumberOfPoints();
  double *  pt = NULL;
  int       time = 0;

  std::stringstream oMyString;

  oMyString << N << " ";

  vtkSmartPointer< vtkPoints > points = iPolyData->GetPoints();
  // Might create problems because of the safedowncast
  vtkSmartPointer< vtkIntArray > temporalArray =
    vtkIntArray::SafeDownCast( iPolyData->GetPointData()->GetArray("TemporalInformation") );

  for ( vtkIdType i = 0; i < N; i++ )
    {
    pt = points->GetPoint(i);
    time = temporalArray->GetValue(i);
    oMyString << pt[0] << " " << pt[1] << " " << pt[2] << " " << time << " ";
    }

  return oMyString.str();
}

//--------------------------------------------------------------------------