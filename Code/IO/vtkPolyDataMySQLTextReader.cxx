/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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

#include "vtkPolyDataMySQLTextReader.h"

#include "vtkObjectFactory.h"
#include "vtkCellArray.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"

#include <sstream>

vtkCxxRevisionMacro(vtkPolyDataMySQLTextReader, "$Revision$");
vtkStandardNewMacro(vtkPolyDataMySQLTextReader);

vtkPolyDataMySQLTextReader::vtkPolyDataMySQLTextReader():
  m_Text(""), IsContour(true)
{}

vtkPolyDataMySQLTextReader::~vtkPolyDataMySQLTextReader()
{}

void vtkPolyDataMySQLTextReader::SetIsContour(const bool & iContour)
{
  IsContour = iContour;
}

vtkPolyData * vtkPolyDataMySQLTextReader::GetPolyData(const std::string & iString)
{
  m_Text = iString;
  if ( IsContour )
    {
    return GetContour();
    }
  else
    {
    return GetMesh();
    }
}

vtkPolyData * vtkPolyDataMySQLTextReader::GetContour()
{
  vtkPolyData *oContour = vtkPolyData::New();

  vtkSmartPointer< vtkPoints > points =
    vtkSmartPointer< vtkPoints >::New();

  std::stringstream str(m_Text);

  vtkIdType N;
  // NOTE ALEX:
  // As expected the problem is here in the Reader ....
  // the following line takes the first char of the string off
  // str >> quote >> N;
  // in the case I m dealing with, there is no characters to skip
  // Thus resulting dropping the frist 1 of 111 to make it eleven.
  // great ...
  str >> N;
  points->SetNumberOfPoints(N);

  double pt[3];

  for ( vtkIdType i = 0; i < N; i++ )
    {
    str >> pt[0] >> pt[1] >> pt[2];
    points->SetPoint(i, pt);
    }
  oContour->SetPoints(points);

  vtkSmartPointer< vtkCellArray > cells =
    vtkSmartPointer< vtkCellArray >::New();
  vtkIdType *ids = new vtkIdType[N + 1];

  for ( vtkIdType i = 0; i < N; i++ )
    {
    ids[i] = i;
    }

  ids[N] = 0;
  cells->InsertNextCell(N + 1, ids);
  oContour->SetLines(cells);

  delete[] ids;

  return oContour;
}

vtkPolyData * vtkPolyDataMySQLTextReader::GetMesh()
{
  std::stringstream str(m_Text);

  vtkIdType N;

  str >> N;

  // if N == 0, the mesh is a collection of contours
  vtkPolyData *oMesh = NULL;
  if ( N != 0 )
    {
    oMesh = vtkPolyData::New();
    vtkSmartPointer< vtkPoints > points =
      vtkSmartPointer< vtkPoints >::New();

    points->SetNumberOfPoints(N);

    double pt[3];

    for ( vtkIdType i = 0; i < N; i++ )
      {
      str >> pt[0] >> pt[1] >> pt[2];
      points->SetPoint(i, pt);
      }
    oMesh->SetPoints(points);

    vtkSmartPointer< vtkCellArray > cells =
      vtkSmartPointer< vtkCellArray >::New();
    str >> N;

    vtkSmartPointer< vtkIdList > cell_points =
      vtkSmartPointer< vtkIdList >::New();
    vtkIdType NbOfPointsInCell;
    vtkIdType id;

    for ( vtkIdType i = 0; i < N; i++ )
      {
      str >> NbOfPointsInCell;
      cell_points->Reset();
      for ( vtkIdType k = 0; k < NbOfPointsInCell; k++ )
        {
        str >> id;
        cell_points->InsertNextId(id);
        }
      cells->InsertNextCell(cell_points);
      }
    oMesh->SetPolys(cells);
    }

  return oMesh;
}
