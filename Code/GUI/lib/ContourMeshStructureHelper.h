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

#ifndef __ContourMeshStructureHelper_h
#define __ContourMeshStructureHelper_h

#include "ContourMeshStructure.h"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/ordered_index.hpp"

#include <list>

typedef boost::multi_index::multi_index_container<
    ContourMeshStructure,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<TCoord>,
        BOOST_MULTI_INDEX_MEMBER( ContourMeshStructure, unsigned int, TCoord )
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag<Actor>,
        BOOST_MULTI_INDEX_MEMBER( ContourMeshStructure, vtkActor*, Actor )
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag<Nodes>,
        BOOST_MULTI_INDEX_MEMBER( ContourMeshStructure, vtkPolyData*, Nodes)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag<TraceID>,
        BOOST_MULTI_INDEX_MEMBER( ContourMeshStructure, unsigned int, TraceID )
        > //,
  //       hashed_non_unique<
  //         tag<CollectionID>,
  //         BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,unsigned int,CollectionID)>
      >
  > ContourMeshStructureMultiIndexContainer;

typedef boost::multi_index::multi_index_container<
  ContourMeshStructure*,
  boost::multi_index::indexed_by<
    boost::multi_index::hashed_non_unique<
      BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,unsigned int,TraceID)>
>
> ContourMeshStructureTraceIDViewContainer;

typedef boost::multi_index::multi_index_container<
    ContourMeshStructure*,
    boost::multi_index::indexed_by<
      boost::multi_index::hashed_non_unique<
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,unsigned int,CollectionID)>
    >
  > ContourMeshStructureCollectionIDViewContainer;

typedef boost::multi_index::multi_index_container<
    ContourMeshStructure*,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_non_unique<
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,unsigned int,TCoord)>
    >
  > ContourMeshStructureTCoordViewContainer;

typedef boost::multi_index::multi_index_container<
    ContourMeshStructure*,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_unique<
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,vtkActor*,Actor)>
    >
  > ContourMeshStructureActorViewContainer;

typedef boost::multi_index::multi_index_container<
    ContourMeshStructure*,
    boost::multi_index::indexed_by<
      boost::multi_index::hashed_non_unique<
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,vtkPolyData*,Nodes)>
    >
  > ContourMeshStructureNodesViewContainer;

/**
\brief Return the direction of a given contour iContour
\param[in] iContour
\return 0 \if z coordinates are constant
\return 1 \if y coordinates are constant
\return 2 \if x coordinates are constant
\return -1 \else
*/
int
ComputeDirectionFromContour( vtkPolyData* iContour );

std::list< ContourMeshStructure* >
FindContourGivenTraceID(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  const unsigned int& iId );

ContourMeshStructure*
FindContourGivenActor(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  vtkActor* iActor );

std::list< ContourMeshStructure* >
FindContourGivenNodes(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  vtkPolyData* iNodes );

std::list< ContourMeshStructure* >
FindContourGivenTimePoint(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  const unsigned int& iTimePoint
);

#endif
