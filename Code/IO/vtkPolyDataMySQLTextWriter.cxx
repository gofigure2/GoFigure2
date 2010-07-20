/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

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

#include "vtkPolyDataMySQLTextWriter.h"

#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkMath.h"
#include "vtkIdList.h"
#include "vtkSmartPointer.h"

vtkCxxRevisionMacro(vtkPolyDataMySQLTextWriter, "$Revision$");
vtkStandardNewMacro(vtkPolyDataMySQLTextWriter);

std::string vtkPolyDataMySQLTextWriter::GetMySQLText(vtkPolyData* iPolyData)
{
  m_PolyData = iPolyData;
  IsContour = IsPlanarContour();

  if (IsContour)
    {
    return ContourProcessing();
    }
  else
    {
    return MeshProcessing();
    }
}

vtkPolyDataMySQLTextWriter::vtkPolyDataMySQLTextWriter() : m_PolyData(0),
  IsContour(true)
  {
  }
vtkPolyDataMySQLTextWriter::~vtkPolyDataMySQLTextWriter()
  {
  }

bool vtkPolyDataMySQLTextWriter::IsPlanarContour()
{
  double bounds[6];
  m_PolyData->GetBounds(bounds);

  for (int dim = 0; dim < 3; dim++)
    {
    if (bounds[2 * dim] == bounds[2 * dim + 1])
      {
      return true;
      }
    }
  return false;
}

std::string vtkPolyDataMySQLTextWriter::ContourProcessing()
{
  vtkIdType N = m_PolyData->GetNumberOfPoints();

  double            pt[3];
  std::stringstream oMyString;
  oMyString << N << " ";

  for (vtkIdType i = 0; i < N; i++)
    {
    m_PolyData->GetPoint(i, pt);
    oMyString << pt[0] << " " << pt[1] << " " << pt[2] << " ";
    }
  return oMyString.str();
}

std::string vtkPolyDataMySQLTextWriter::MeshProcessing()
{
  vtkIdType N = m_PolyData->GetNumberOfPoints();
  double    pt[3];

  std::stringstream oMyString;
  oMyString << N << " ";

  for (vtkIdType i = 0; i < N; i++)
    {
    m_PolyData->GetPoint(i, pt);
    oMyString << pt[0] << " " << pt[1] << " " << pt[2] << " ";
    }

  vtkSmartPointer<vtkIdList>
  cell_points = vtkSmartPointer<vtkIdList>::New();

  vtkIdType NbOfPointsInCell;
  N = m_PolyData->GetNumberOfCells();
  oMyString << N << " ";

  for (vtkIdType i = 0; i < N; i++)
    {
    m_PolyData->GetCellPoints(i, cell_points);
    NbOfPointsInCell = cell_points->GetNumberOfIds();
    oMyString << NbOfPointsInCell << " ";

    for (vtkIdType k = 0; k < NbOfPointsInCell; k++)
      {
      oMyString << cell_points->GetId(k) << " ";
      }
    }

  return oMyString.str();
}
