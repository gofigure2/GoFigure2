#ifndef __ContourStructureHelper_h
#define __ContourStructureHelper_h

#include "ContourStructure.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

#include <list>

using boost::multi_index_container;
using namespace boost::multi_index;

typedef multi_index_container<
  ContourStructure,
    indexed_by<
      ordered_non_unique<
        tag<ContourId>,
        BOOST_MULTI_INDEX_MEMBER(ContourStructure,unsigned int,ContourId)>,
      ordered_unique<
        tag<Actor>,
        BOOST_MULTI_INDEX_MEMBER(ContourStructure,ContourStructure::vtkActorPointer,Actor)>,
      ordered_non_unique<
        tag<Nodes>,
        BOOST_MULTI_INDEX_MEMBER(ContourStructure,ContourStructure::vtkPolyDataPointer,Nodes)>,
      ordered_non_unique<
        tag<MeshId>,
        BOOST_MULTI_INDEX_MEMBER(ContourStructure,unsigned int,MeshId)>,
      ordered_non_unique<
        tag<TCoord>,
        BOOST_MULTI_INDEX_MEMBER(ContourStructure,unsigned int,TCoord)>
    >
> ContourStructureMultiIndexContainer;

typedef multi_index_container<
  const ContourStructure*,
  indexed_by<
    ordered_non_unique<
      BOOST_MULTI_INDEX_MEMBER(ContourStructure,const unsigned int,ContourId)>
>
> ContourStructureContourIdViewContainer;

typedef multi_index_container<
    const ContourStructure*,
    indexed_by<
      ordered_non_unique<
        BOOST_MULTI_INDEX_MEMBER(ContourStructure,const unsigned int,MeshId)>
    >
  > ContourStructureMeshIdViewContainer;

typedef multi_index_container<
    const ContourStructure*,
    indexed_by<
      ordered_non_unique<
        BOOST_MULTI_INDEX_MEMBER(ContourStructure,const unsigned int,TCoord)>
    >
  > ContourStructureTCoordViewContainer;

typedef multi_index_container<
    const ContourStructure*,
    indexed_by<
      ordered_unique<
        BOOST_MULTI_INDEX_MEMBER(ContourStructure,const ContourStructure::vtkActorPointer,Actor)>
    >
  > ContourStructureActorViewContainer;

typedef multi_index_container<
    const ContourStructure*,
    indexed_by<
      ordered_non_unique<
        BOOST_MULTI_INDEX_MEMBER(ContourStructure,const ContourStructure::vtkPolyDataPointer,Nodes)>
    >
  > ContourStructureNodesViewContainer;



std::list< ContourStructure >
FindContourGivenContourId(
  const ContourStructureMultiIndexContainer& iContainer,
  const unsigned int& iId );

ContourStructure
FindContourGivenActor(
  const ContourStructureMultiIndexContainer& iContainer,
  vtkActor* iActor );

std::list< ContourStructure >
FindContourGivenNodes(
  const ContourStructureMultiIndexContainer& iContainer,
  vtkPolyData* iNodes );

std::list< ContourStructure >
FindContourGivenTimePoint(
  const ContourStructureMultiIndexContainer& iContainer,
  const unsigned int& iTimePoint
);

#endif
