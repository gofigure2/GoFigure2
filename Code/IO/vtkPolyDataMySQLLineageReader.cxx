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

#include "vtkPolyDataMySQLLineageReader.h"

#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkTriangle.h"

#include <sstream>

vtkCxxRevisionMacro(vtkPolyDataMySQLLineageReader, "$Revision$");
vtkStandardNewMacro(vtkPolyDataMySQLLineageReader);

//--------------------------------------------------------------------------
vtkPolyDataMySQLLineageReader::vtkPolyDataMySQLLineageReader()
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkPolyDataMySQLLineageReader::
~vtkPolyDataMySQLLineageReader()
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
vtkSmartPointer< vtkPolyData >
vtkPolyDataMySQLLineageReader::GetPolyData(const std::string & iString)
{
  std::stringstream str(iString);

  // Might not be useful
  vtkIdType N;
  str >> N;

  if ( N != 0 )
    {
      double* pt = NULL;
      vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
      for ( vtkIdType i = 0; i < N; i++ )
        {
        pt = new double[3];
        str >> pt[0] >> pt[1] >> pt[2];
        points->InsertNextPoint(pt[0], pt[1], pt[2]);
        }

      vtkSmartPointer<vtkCellArray> triangles = vtkSmartPointer<vtkCellArray>::New();

      //create a triangle on the three points in the polydata
      vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
      triangle->GetPointIds()->SetId ( 0, 0 );
      triangle->GetPointIds()->SetId ( 1, 1 );
      triangle->GetPointIds()->SetId ( 2, 2 );

      //add the triangle to the list of triangles (in this case there is only 1)
      triangles->InsertNextCell ( triangle );

      vtkSmartPointer<vtkPolyData> polyData =
          vtkSmartPointer<vtkPolyData>::New();
      polyData->SetPoints ( points );
      polyData->SetPolys ( triangles );

    return polyData;
    }

  return NULL;
}
//--------------------------------------------------------------------------
