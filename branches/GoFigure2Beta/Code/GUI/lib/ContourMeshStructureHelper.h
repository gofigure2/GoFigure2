#ifndef __ContourMeshStructureHelper_h
#define __ContourMeshStructureHelper_h

#include "ContourMeshStructure.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <list>

using boost::multi_index_container;
using namespace boost::multi_index;

typedef multi_index_container<
  ContourMeshStructure,
    indexed_by<
      hashed_non_unique<
        tag<TraceID>,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,unsigned int,TraceID)>,
      hashed_unique<
        tag<Actor>,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,ContourMeshStructure::vtkActorPointer,Actor)>,
      hashed_non_unique<
        tag<Nodes>,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,ContourMeshStructure::vtkPolyDataPointer,Nodes)>,
      hashed_non_unique<
        tag<CollectionID>,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,unsigned int,CollectionID)>,
      ordered_non_unique<
        tag<TCoord>,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,unsigned int,TCoord)>
    >
> ContourMeshStructureMultiIndexContainer;

typedef multi_index_container<
  const ContourMeshStructure*,
  indexed_by<
    ordered_non_unique<
      BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,const unsigned int,TraceID)>
>
> ContourMeshStructureTraceIDViewContainer;

typedef multi_index_container<
    const ContourMeshStructure*,
    indexed_by<
      ordered_non_unique<
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,const unsigned int,CollectionID)>
    >
  > ContourMeshStructureCollectionIDViewContainer;

typedef multi_index_container<
    const ContourMeshStructure*,
    indexed_by<
      ordered_non_unique<
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,const unsigned int,TCoord)>
    >
  > ContourMeshStructureTCoordViewContainer;

typedef multi_index_container<
    const ContourMeshStructure*,
    indexed_by<
      ordered_unique<
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,const ContourMeshStructure::vtkActorPointer,Actor)>
    >
  > ContourMeshStructureActorViewContainer;

typedef multi_index_container<
    const ContourMeshStructure*,
    indexed_by<
      ordered_non_unique<
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure,const ContourMeshStructure::vtkPolyDataPointer,Nodes)>
    >
  > ContourMeshStructureNodesViewContainer;



std::list< ContourMeshStructure >
FindContourGivenTraceID(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  const unsigned int& iId );

ContourMeshStructure
FindContourGivenActor(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  vtkActor* iActor );

std::list< ContourMeshStructure >
FindContourGivenNodes(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  vtkPolyData* iNodes );

std::list< ContourMeshStructure >
FindContourGivenTimePoint(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  const unsigned int& iTimePoint
);

#endif
