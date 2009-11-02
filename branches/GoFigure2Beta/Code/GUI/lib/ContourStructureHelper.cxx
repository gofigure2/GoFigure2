#include "ContourStructureHelper.h"

using boost::multi_index_container;
using namespace boost::multi_index;

std::list< ContourStructure >
FindContourGivenContourId(
  ContourStructureMultiIndexContainer iContainer,
  const unsigned int& iId )
{
  std::list< ContourStructure > oList;

  ContourStructureMultiIndexContainer::index< ContourId >::type::iterator
    it = iContainer.get< ContourId >().find( iId );

  if( it != iContainer.get< ContourId >().end() )
    {
    while( it->ContourId == iId )
      {
      oList.push_back( *it );
      ++it;
      }
    }

  return oList;
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

std::list< ContourStructure >
FindContourGivenTimePoint(
  ContourStructureMultiIndexContainer iContainer,
  const unsigned int& iTimePoint )
{
   ContourStructureMultiIndexContainer::index< TCoord >::type::iterator it0, it1;
   boost::tuples::tie(it0,it1) = iContainer.get< TCoord >().equal_range( iTimePoint );

   std::list< ContourStructure > oList;

    while( it0 != it1 )
      {
      oList.push_back( *it0 );
      ++it0;
      }
    return oList;
}
