#ifndef __ContourStructureHelper_h
#define __ContourStructureHelper_h

#include "ContourStructure.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>

using boost::multi_index_container;
using namespace boost::multi_index;

typedef multi_index_container<
  ContourStructure,
    indexed_by<
      ordered_unique<
        tag<ContourId>, BOOST_MULTI_INDEX_MEMBER(ContourStructure,unsigned int,ContourId)>,
      ordered_unique<
        tag<Actor>, BOOST_MULTI_INDEX_MEMBER(ContourStructure,ContourStructure::vtkActorPointer,Actor)>,
      ordered_unique<
        tag<Nodes>, BOOST_MULTI_INDEX_MEMBER(ContourStructure,ContourStructure::vtkPolyDataPointer,Nodes)>,
      ordered_non_unique<
        tag<MeshId>, BOOST_MULTI_INDEX_MEMBER(ContourStructure,unsigned int,MeshId)>,
      ordered_non_unique<
        tag<TCoord>, BOOST_MULTI_INDEX_MEMBER(ContourStructure,unsigned int,TCoord)>
    >
> ContourStructureMultiIndexContainer;


typedef multi_index_container<
    const ContourStructure*,
    indexed_by<
      ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(ContourStructure,const unsigned int,MeshId)>
    >
  > ContourStructureMeshIdViewContainer;

typedef multi_index_container<
    const ContourStructure*,
    indexed_by<
      ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(ContourStructure,const unsigned int,TCoord)>
    >
  > ContourStructureTCoordViewContainer;

typedef multi_index_container<
    const ContourStructure*,
    indexed_by<
      ordered_unique<BOOST_MULTI_INDEX_MEMBER(ContourStructure,const ContourStructure::vtkActorPointer,Actor)>
    >
  > ContourStructureActorViewContainer;

typedef multi_index_container<
    const ContourStructure*,
    indexed_by<
      ordered_unique<BOOST_MULTI_INDEX_MEMBER(ContourStructure,const ContourStructure::vtkPolyDataPointer,Nodes)>
    >
  > ContourStructureNodesViewContainer;



ContourStructure
FindContourGivenContourId(
  ContourStructureMultiIndexContainer iContainer,
  const unsigned int& iId )
{
  ContourStructureMultiIndexContainer::index< ContourId >::type::iterator
    it = iContainer.get< ContourId >().find( iId );

  if( it != iContainer.get< ContourId >().end() )
    {
    return (*it);
    }
  else
    {
    return ContourStructure();
    }
}


ContourStructure
FindContourGivenActor(
  ContourStructureMultiIndexContainer iContainer,
  vtkActor* iActor )
{
  ContourStructureMultiIndexContainer::nth_index< 1 >::type::iterator
    it = iContainer.get< 1 >().find( iActor );

  if( it != iContainer.get< 1 >().end() )
    {
    return (*it);
    }
  else
    {
    return ContourStructure();
    }
}

ContourStructure
FindContourGivenNodes(
  ContourStructureMultiIndexContainer iContainer,
  vtkPolyData* iNodes )
{
  ContourStructureMultiIndexContainer::nth_index< 2 >::type::iterator
    it = iContainer.get< 2 >().find( iNodes );

  if( it != iContainer.get< 2 >().end() )
    {
    return (*it);
    }
  else
    {
    return ContourStructure();
    }
}

#endif