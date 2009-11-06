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

  GoFigureFileInfoHelperChannelViewContainer::nth_index<0>::type::iterator ic0, ic1;
  ic0 = subset.get<0>().lower_bound( iCh );
  ic1 = subset.get<0>().upper_bound( iCh );

  GoFigureFileInfoHelperZCoordViewContainer final_container;

  while( ic0 != ic1 )
    {
    final_container.insert( *ic0 );
    ++ic0;
    }

  GoFigureFileInfoHelperZCoordViewContainer::iterator z_it0 = final_container.begin();
  GoFigureFileInfoHelperZCoordViewContainer::iterator z_it1 = final_container.end();

  while( z_it0 != z_it1 )
    {
    oList.push_back( (*z_it0)->m_Filename );
    ++z_it0;
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