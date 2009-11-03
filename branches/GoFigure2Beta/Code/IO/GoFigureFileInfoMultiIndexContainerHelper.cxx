#include "GoFigureFileInfoMultiIndexContainerHelper.h"

std::list< std::string > GetAllFileNamesForGivenTCoordAndChannel(
  GoFigureFileInfoHelperMultiIndexContainer iContainer,
  const unsigned int& iT,
  const unsigned int& iCh )
{
  std::list< std::string > oList;

  using namespace boost;

  multi_index::index<GoFigureFileInfoHelperMultiIndexContainer,m_TCoord>::type::iterator it0,it1;
  tuples::tie(it0,it1)=get<m_TCoord>(iContainer).equal_range( iT );

  GoFigureFileInfoHelperChannelViewContainer subset;

  while( it0 != it1 )
    {
    subset.insert( &*it0 );
    ++it0;
    }

  GoFigureFileInfoHelperChannelViewContainer::iterator ic0, ic1;
  ic0 = subset.lower_bound( iCh );
  ic1 = subset.upper_bound( iCh );

  while( ic0 != ic1 )
    {
    oList.push_back( (*ic0)->m_Filename );
    ++ic0;
    }

  return oList;
}

std::list< std::string > GetAllFileNamesForGivenZCoordPointAndChannel(
  GoFigureFileInfoHelperMultiIndexContainer iContainer,
  const unsigned int& iZ,
  const unsigned int& iCh )
{
  std::list< std::string > oList;

  using namespace boost;
  multi_index::index<GoFigureFileInfoHelperMultiIndexContainer,m_ZCoord>::type::iterator it0,it1;
  tuples::tie(it0,it1)=get<m_ZCoord>(iContainer).equal_range( iZ );

  GoFigureFileInfoHelperChannelViewContainer subset;

  while( it0 != it1 )
    {
    subset.insert( &*it0 );
    ++it0;
    }

  GoFigureFileInfoHelperChannelViewContainer::iterator ic0, ic1;
  ic0 = subset.lower_bound( iCh );
  ic1 = subset.upper_bound( iCh );

  while( ic0 != ic1 )
    {
    oList.push_back( (*ic0)->m_Filename );
    ++ic0;
    }

  return oList;
}