#ifndef __ContourMeshStructureHelper_h
#define __ContourMeshStructureHelper_h

#include "ContourMeshStructure.h"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/ordered_index.hpp"

#include <list>

class ContourMeshStructureMultiIndexContainer :
  public boost::multi_index::multi_index_container<
    ContourMeshStructure,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag<TCoord>,
        BOOST_MULTI_INDEX_MEMBER( ContourMeshStructure, unsigned int, TCoord )
        >,
      boost::multi_index::hashed_unique<
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
  >
{
};

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
