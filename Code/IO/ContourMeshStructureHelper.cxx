/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

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

#include "ContourMeshStructureHelper.h"
#include "vtkPolyData.h"

// ---------------------------------------------------------------------------
int
ComputeDirectionFromContour( vtkPolyData* iContour )
{
  double bounds[6];
  iContour->GetBounds( bounds );

  int oDir = -1;

  for( int i = 0; i < 3; i++ )
    {
    if( bounds[2*i] == bounds[2*i+1] )
      {
      oDir = 2 - i;
      }
    }

  return oDir;
}
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
std::list< ContourMeshStructure* >
FindContourGivenTraceID(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  const unsigned int& iId )
{
  std::list< ContourMeshStructure* > oList;

  if( iContainer.size() != 0 )
    {
    ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
      it = iContainer.get< TraceID >().find( iId );

    if( it != iContainer.get< TraceID >().end() )
      {
      while( it->TraceID == iId )
        {
        oList.push_back( const_cast< ContourMeshStructure* >( &*it ) );
        ++it;
        }
      }
    }

  return oList;
}
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
ContourMeshStructure*
FindContourGivenActor(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  vtkActor* iActor )
{
  if( iContainer.size() != 0 )
    {
    ContourMeshStructureMultiIndexContainer::nth_index< 1 >::type::iterator
      it = iContainer.get< 1 >().find( iActor );

    if( it != iContainer.get< 1 >().end() )
      {
      return const_cast< ContourMeshStructure* >( &(*it) );
      }
    else
      {
      return 0;
      }
    }
  return 0;
}
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
std::list< ContourMeshStructure* >
FindContourGivenNodes(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  vtkPolyData* iNodes )
{
  std::list< ContourMeshStructure* > oList;

  if( !iContainer.empty() )
    {
    ContourMeshStructureMultiIndexContainer::nth_index< 2 >::type::iterator
      it = iContainer.get< 2 >().find( iNodes );

    if( it != iContainer.get< 2 >().end() )
      {
      while( it != iContainer.get< 2 >().end() )
        {
        if( (*it).Nodes == iNodes )
          {
          oList.push_back( const_cast< ContourMeshStructure* >( &*it ) );
          }
        ++it;
        }
      }
    }
  return oList;
}
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
std::list< ContourMeshStructure* >
FindContourGivenTimePoint(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  const unsigned int& iTimePoint )
{
  ContourMeshStructureMultiIndexContainer::index< TCoord >::type::iterator it0, it1;
  boost::tuples::tie(it0,it1) = iContainer.get< TCoord >().equal_range( iTimePoint );

  std::list< ContourMeshStructure* > oList;

  while( it0 != it1 )
    {
    oList.push_back( const_cast< ContourMeshStructure* >( &*it0 ) );
    ++it0;
    }
  return oList;
}
// ---------------------------------------------------------------------------
