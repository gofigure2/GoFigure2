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

#ifndef __itkNearestNeighborTracking_h
#define __itkNearestNeighborTracking_h

#include "itkLightObject.h"

#include "vtkSmartPointer.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"
#include "vtkKdTree.h"

#include <map>

namespace itk
{
class NearestNeighborTracking : public LightObject
{
public:
  typedef NearestNeighborTracking     Self;
  typedef LightObject                 Superclass;
  typedef SmartPointer< Self >        Pointer;
  typedef SmartPointer< const Self >  ConstPointer;

  /** Method for creation through object factory */
  itkNewMacro(Self);

  /** Run-time type information */
  itkTypeMacro(NearestNeighborTracking, LightObject);

  typedef std::map< unsigned int, vtkSmartPointer< vtkPolyData > >  MeshMapType;

  void SetMeshContainer1( const MeshMapType& iContainer )
    {
    this->m_MeshContainer1 = iContainer;
    }

  void SetMeshContainer2( const MeshMapType& iContainer )
    {
    this->m_MeshContainer2 = iContainer;
    }

  void Update()
    {
    this->GenerateData();
    }

  std::map< unsigned int, unsigned int > GetMeshMapping()
    {
    return this->m_Mapping;
    }

protected:
  NearestNeighborTracking();
  ~NearestNeighborTracking();

  void GenerateData()
  {
    std::map< vtkIdType, unsigned int > TempMap;

    for( MeshMapIterator it = this->m_MeshContainer2.begin(),
         vtkIdType i = 0;
         it! = this->m_MeshContainer2.end();
         ++it, ++i )
      {
      double center[3];

      ( it->second )->GetCenter( center );
      m_Centroids->SetPoint( i, center );
      TempMap[ i ] = it->first;
      }

    m_KdTree->BuildLocatorFromPoints( m_Centroids );

    for( MeshMapIterator it = this->m_MeshContainer1.begin(),
         vtkIdType i = 0;
         it! = this->m_MeshContainer1.end();
         ++it, ++i )
      {
      double center[3];
      double closestPointDist;

      (*it)->GetCenter( center );
      vtkIdType id = kDTree->FindClosestPoint( center, closestPointDist );

      m_Mapping[ it->first ] = TempMap[ id ];
      }
  }

  vtkSmartPointer< vtkKdTree >  m_KdTree;
  vtkSmartPointer< vtkPoints >  m_Centroids;

  typedef typename MeshMapType::iterator                            MeshMapIterator;
  typedef typename MeshMapType::const_iterator                      MeshMapConstIterator;

  MeshMapType m_MeshContainer1;
  MeshMapType m_MeshContainer2;

  std::map< unsigned int, unsigned int >  m_Mapping;

private:
  NearestNeigborTracking( const Self& );
  void operator = ( const Self& );

};
}
#endif

