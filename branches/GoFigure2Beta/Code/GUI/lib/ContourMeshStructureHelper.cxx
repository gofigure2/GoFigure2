#include "ContourMeshStructureHelper.h"

using boost::multi_index_container;
using namespace boost::multi_index;

std::list< ContourMeshStructure >
FindContourGivenTraceID(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  const unsigned int& iId )
{
  std::list< ContourMeshStructure > oList;

  if( iContainer.size() != 0 )
    {
    ContourMeshStructureMultiIndexContainer::index< TraceID >::type::iterator
      it = iContainer.get< TraceID >().find( iId );

    if( it != iContainer.get< TraceID >().end() )
      {
      while( it->TraceID == iId )
        {
        oList.push_back( *it );
        ++it;
        }
      }
    }

  return oList;
}


ContourMeshStructure
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
      return (*it);
      }
    else
      {
      return ContourMeshStructure();
      }
    }
  return ContourMeshStructure();
}

std::list< ContourMeshStructure >
FindContourGivenNodes(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  vtkPolyData* iNodes )
{
  std::list< ContourMeshStructure > oList;

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
          oList.push_back( *it );
          }
        ++it;
        }
      }
    }
  return oList;
}

std::list< ContourMeshStructure >
FindContourGivenTimePoint(
  const ContourMeshStructureMultiIndexContainer& iContainer,
  const unsigned int& iTimePoint )
{
   ContourMeshStructureMultiIndexContainer::index< TCoord >::type::iterator it0, it1;
   boost::tuples::tie(it0,it1) = iContainer.get< TCoord >().equal_range( iTimePoint );

   std::list< ContourMeshStructure > oList;

    while( it0 != it1 )
      {
      oList.push_back( *it0 );
      ++it0;
      }
    return oList;
}
