#include "ContourStructureHelper.h"

using boost::multi_index_container;
using namespace boost::multi_index;

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
