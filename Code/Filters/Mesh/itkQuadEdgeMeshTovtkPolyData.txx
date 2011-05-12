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
#ifndef __itkQuadEdgeMeshTovtkPolyData_txx
#define __itkQuadEdgeMeshTovtkPolyData_txx

#include "itkQuadEdgeMeshTovtkPolyData.h"

namespace itk
{
template< class TMesh >
QuadEdgeMeshTovtkPolyData< TMesh >
::QuadEdgeMeshTovtkPolyData()
{
  m_PolyData = vtkSmartPointer< vtkPolyData >::New();
}


template< class TMesh >
QuadEdgeMeshTovtkPolyData< TMesh >
::~QuadEdgeMeshTovtkPolyData()
{
}

template< class TMesh >
void
QuadEdgeMeshTovtkPolyData< TMesh >
::SetInput(MeshType* mesh)
{
  if( mesh )
    {
    m_itkMesh = mesh;
    this->Modified();
    }
}

template< class TMesh >
vtkPolyData *
QuadEdgeMeshTovtkPolyData< TMesh >
::GetOutput()
{
  return m_PolyData;
}

template< class TMesh >
void
QuadEdgeMeshTovtkPolyData< TMesh >
::Update()
{
  if( m_itkMesh.IsNull() )
    {
    itkGenericExceptionMacro( << "m_itkMesh is NULL" );
    }
  this->GenerateData();
}

template< class TMesh >
void
QuadEdgeMeshTovtkPolyData< TMesh >
::GenerateData()
{

  PointIdentifier NumberOfPoints = m_itkMesh->GetNumberOfPoints();

  if( NumberOfPoints == 0 )
    {
    itkGenericExceptionMacro( << "NumberOfPoints == 0" );
    }

  vtkSmartPointer< vtkPoints > vtk_Points =
      vtkSmartPointer< vtkPoints >::New();
  vtk_Points->SetNumberOfPoints( NumberOfPoints );

  PointsContainerPointer      itk_Points = m_itkMesh->GetPoints();
  PointsContainerConstIterator     p_it = itk_Points->Begin();
  PointsContainerConstIterator     p_end = itk_Points->End();
  PointType itk_point;

  vtkIdType id=0;
  double vtk_point[3];

  while( p_it != p_end )
    {
    itk_point = p_it.Value();

    vtk_point[0] = itk_point[0];
    vtk_point[1] = itk_point[1];
    vtk_point[2] = itk_point[2];

    vtk_Points->SetPoint( id, vtk_point );
    ++p_it;
    ++id;
    }

  if( m_itkMesh->GetNumberOfCells() == 0 )
    {
    itkGenericExceptionMacro( << "NumberOfCells == 0" );
    }

  CellsContainerPointer itk_Cells = m_itkMesh->GetCells();
  CellsContainerConstIterator c_it = itk_Cells->Begin();
  CellsContainerConstIterator c_end = itk_Cells->End();

  vtkSmartPointer< vtkCellArray > vtk_Polys =
      vtkSmartPointer< vtkCellArray >::New();

  while ( c_it != c_end )
    {
    CellType* temp_cell = c_it->Value();
    typename CellType::PointIdIterator temp_c_it = temp_cell->PointIdsBegin();
    typename CellType::PointIdIterator temp_c_end = temp_cell->PointIdsEnd();

    size_t nb_pts = temp_cell->GetNumberOfPoints();

    std::vector< vtkIdType > pts( nb_pts );
    size_t i = 0;

    switch (temp_cell->GetType())
      {
      default:
      case CellType::VERTEX_CELL:
      case CellType::LINE_CELL:
        break;

      case CellType::TRIANGLE_CELL:
      case CellType::POLYGON_CELL:
        while( temp_c_it != temp_c_end )
          {
          pts[i] = *temp_c_it;
          ++i;
          ++temp_c_it;
          }
        vtk_Polys->InsertNextCell( nb_pts, &( pts.front() ) );
        break;
      }
    ++c_it;
    }

  m_PolyData->SetPoints( vtk_Points );
  m_PolyData->SetPolys( vtk_Polys );
}

}

#endif
