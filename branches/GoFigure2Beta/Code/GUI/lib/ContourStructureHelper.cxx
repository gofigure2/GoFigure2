#include "ContourStructureHelper.h"

using boost::multi_index_container;
using namespace boost::multi_index;

std::list< ContourStructure >
FindContourGivenContourId(
  const ContourStructureMultiIndexContainer& iContainer,
  const unsigned int& iId )
{
  std::list< ContourStructure > oList;

  if( iContainer.size() != 0 )
    {
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
    }

  return oList;
}


ContourStructure
FindContourGivenActor(
  const ContourStructureMultiIndexContainer& iContainer,
  vtkActor* iActor )
{
  if( iContainer.size() != 0 )
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
  return ContourStructure();
}

std::list< ContourStructure >
FindContourGivenNodes(
  const ContourStructureMultiIndexContainer& iContainer,
  vtkPolyData* iNodes )
{
  std::list< ContourStructure > oList;

  if( !iContainer.empty() )
    {
    ContourStructureMultiIndexContainer::nth_index< 2 >::type::iterator
      it = iContainer.get< 2 >().find( iNodes );

    if( it != iContainer.get< 2 >().end() )
      {
      while( it != iContainer.get< 2 >().end() )
        {
        if( (*it).Nodes == iNodes )
          {
          oList.push_back( *it );
          }
        ++it;
        }
      }
    }
  return oList;
}

std::list< ContourStructure >
FindContourGivenTimePoint(
  const ContourStructureMultiIndexContainer& iContainer,
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
