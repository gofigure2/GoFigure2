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

#ifndef __vtkPolyDataToitkQuadEdgeMesh_h
#define __vtkPolyDataToitkQuadEdgeMesh_h

#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"

#include "itkQuadEdgeMesh.h"
#include "itkTriangleCell.h"

#include "itkObject.h"

namespace itk
{
template< class TMesh >
class vtkPolyDataToitkQuadEdgeMesh : public Object
{
public:
  typedef vtkPolyDataToitkQuadEdgeMesh Self;
  typedef Object Superclass;
  typedef SmartPointer< Self > Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(vtkPolyDataToitkQuadEdgeMesh, Object);

  typedef TMesh MeshType;
  typedef typename MeshType::Pointer MeshPointer;
  typedef typename MeshType::PointsContainerPointer PointsContainerPointer;
  typedef typename MeshType::PointIdentifier        PointIdentifier;
  typedef typename MeshType::CellAutoPointer        CellAutoPointer;
  typedef typename MeshType::CellType               CellType;
  typedef typename MeshType::PointType              PointType;
  typedef typename PointType::CoordRepType          CoordType;

  typedef TriangleCell< CellType > TriangleCellType;

  void SetInput( vtkPolyData* iMesh )
    {
    m_PolyData = iMesh;
    }

  void Update()
    {
    GenerateData();
    }

  MeshType* GetOutput() const
    {
    return m_OutputMesh.GetPointer();
    }

protected:
  vtkPolyDataToitkQuadEdgeMesh()
    {
    m_OutputMesh = MeshType::New();
    }

  ~vtkPolyDataToitkQuadEdgeMesh() {}

  vtkSmartPointer< vtkPolyData > m_PolyData;
  MeshPointer m_OutputMesh;

  void GenerateData()
    {
    if( m_PolyData )
      {
      itkGenericExceptionMacro( <<"m_PolyData is NULL" );
      }

    vtkIdType NbOfPoints = m_PolyData->GetNumberOfPoints();

    if( NbOfPoints == 0 )
      {
      itkGenericExceptionMacro( <<"Number Of Points is 0" );
      }

    vtkIdType NbOfCells = m_PolyData->GetNumberOfCells();

    if( NbOfCells == 0 )
      {
      itkGenericExceptionMacro( <<"Number Of Cells is 0" );
      }

    PointsContainerPointer points = m_OutputMesh->GetPoints();
    points->Reserve( NbOfPoints );

    double vtk_p[3];
    PointType itk_p;
    unsigned int dim;

    for( vtkIdType i = 0; i < NbOfPoints; ++i )
      {
      m_PolyData->GetPoint( i, vtk_p );
      for( dim = 0; dim < 3; ++dim )
        {
        itk_p[dim] = static_cast< CoordType >( vtk_p[dim] );
        }
      m_OutputMesh->SetPoint( i, itk_p );
      }

    for( vtkIdType i = 0; i < NbOfCells; ++i )
      {
      vtkIdList* cell_list = vtkIdList::New();

      m_PolyData->GetCellPoints( i, cell_list );

      CellAutoPointer   cell;
      TriangleCellType *triangleCell = new TriangleCellType;
      for ( vtkIdType k = 0; k < cell_list->GetNumberOfIds(); ++k )
        {
        triangleCell->SetPointId( k, cell_list->GetId( k ) );
        }

      cell.TakeOwnership(triangleCell);
      m_OutputMesh->SetCell(i, cell);

      cell_list->Delete();
      }
    }

private:
  vtkPolyDataToitkQuadEdgeMesh( const Self& );
  void operator = ( const Self & );
};
}

#endif // __vtkPolyDataToitkQuadEdgeMesh_h
