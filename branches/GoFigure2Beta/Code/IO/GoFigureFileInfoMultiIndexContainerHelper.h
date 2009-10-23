#ifndef __FileInfoHelperMultiIndexContainer_h
#define __FileInfoHelperMultiIndexContainer_h

#if !defined(NDEBUG)
#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
#endif


#include <string>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <list>

using boost::multi_index_container;
using namespace boost::multi_index;

// namespace GoFigure
// {

struct FileInfoHelper
{
  unsigned int    m_PCoord;
  unsigned int    m_RCoord;
  unsigned int    m_CCoord;
  unsigned int    m_XTileCoord;
  unsigned int    m_YTileCoord;
  unsigned int    m_ZTileCoord;
  unsigned int    m_XCoord;
  unsigned int    m_YCoord;
  unsigned int    m_ZCoord;
  unsigned int    m_TCoord;
  unsigned int    m_Channel;
  std::string     m_Filename;


  FileInfoHelper( const unsigned int& p, const unsigned int& r,
                  const unsigned int& c, const unsigned int& xt,
                  const unsigned int& yt, const unsigned int& zt,
                  const unsigned int& xs, const unsigned int& ys,
                  const unsigned int& zs, const unsigned int& t,
                  const unsigned int& ch, const std::string& iFile )
      : m_PCoord( p ), m_RCoord( r ), m_CCoord( c ), m_XTileCoord( xt ),
        m_YTileCoord( yt ), m_ZTileCoord( zt ), m_XCoord( xs ),
        m_YCoord( ys ), m_ZCoord( zs ), m_TCoord( t ), m_Channel( ch ),
        m_Filename( iFile )
    {}

  FileInfoHelper() : m_PCoord( 0 ), m_RCoord( 0 ), m_CCoord( 0 ),
    m_XTileCoord( 0 ), m_YTileCoord( 0 ), m_ZTileCoord( 0 ), m_XCoord( 0 ),
    m_YCoord( 0 ), m_ZCoord( 0 ), m_TCoord( 0 ), m_Channel( 0 ), m_Filename( "" )
    {}

    ~FileInfoHelper() {}
  };

  struct m_PCoord {};
  struct m_RCoord {};
  struct m_CCoord {};
  struct m_XTileCoord {};
  struct m_YTileCoord {};
  struct m_ZTileCoord {};
  struct m_XCoord {};
  struct m_YCoord {};
  struct m_ZCoord {};
  struct m_TCoord {};
  struct m_Channel {};

  typedef multi_index_container<
    FileInfoHelper,
    indexed_by<
      ordered_non_unique<
        tag<m_PCoord>, BOOST_MULTI_INDEX_MEMBER(FileInfoHelper,unsigned int,m_PCoord)>,
      ordered_non_unique<
        tag<m_RCoord>, BOOST_MULTI_INDEX_MEMBER(FileInfoHelper,unsigned int,m_RCoord)>,
      ordered_non_unique<
        tag<m_CCoord>, BOOST_MULTI_INDEX_MEMBER(FileInfoHelper,unsigned int,m_CCoord)>,
      ordered_non_unique<
        tag<m_XTileCoord>, BOOST_MULTI_INDEX_MEMBER(FileInfoHelper,unsigned int,m_XTileCoord)>,
      ordered_non_unique<
        tag<m_YTileCoord>, BOOST_MULTI_INDEX_MEMBER(FileInfoHelper,unsigned int,m_YTileCoord)>,
      ordered_non_unique<
        tag<m_ZTileCoord>, BOOST_MULTI_INDEX_MEMBER(FileInfoHelper,unsigned int,m_ZTileCoord)>,
      ordered_non_unique<
        tag<m_ZCoord>, BOOST_MULTI_INDEX_MEMBER(FileInfoHelper,unsigned int,m_ZCoord)>,
      ordered_non_unique<
        tag<m_Channel>, BOOST_MULTI_INDEX_MEMBER(FileInfoHelper,unsigned int,m_Channel)>,
      ordered_non_unique<
        tag<m_TCoord>, BOOST_MULTI_INDEX_MEMBER(FileInfoHelper,unsigned int,m_TCoord)>
    >
  > FileInfoHelperMultiIndexContainer;

  typedef multi_index_container<
    const FileInfoHelper*,
    indexed_by<
      ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(FileInfoHelper,const unsigned int,m_ZCoord)>
    >
  > FileInfoHelperZCoordViewContainer;

  typedef multi_index_container<
    const FileInfoHelper*,
    indexed_by<
      ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(FileInfoHelper,const unsigned int,m_TCoord)>
    >
  > FileInfoHelperTCoordViewContainer;

  typedef multi_index_container<
    const FileInfoHelper*,
    indexed_by<
      ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(FileInfoHelper,const unsigned int,m_Channel)>
    >
  > FileInfoHelperChannelViewContainer;
// }

std::list< std::string > GetAllFileNamesForGivenTCoordAndChannel(
  FileInfoHelperMultiIndexContainer iContainer,
  const unsigned int& iT,
  const unsigned int& iCh )
{
  std::list< std::string > oList;

  boost::multi_index::index<FileInfoHelperMultiIndexContainer,m_TCoord>::type::iterator it0,it1;
  boost::tuples::tie(it0,it1)=get<m_TCoord>(iContainer).equal_range( iT );

  FileInfoHelperChannelViewContainer subset;

  while( it0 != it1 )
    {
    subset.insert( &*it0 );
    ++it0;
    }

  FileInfoHelperChannelViewContainer::iterator ic0, ic1;
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
  FileInfoHelperMultiIndexContainer iContainer,
  const unsigned int& iZ,
  const unsigned int& iCh )
{
  std::list< std::string > oList;

  boost::multi_index::index<FileInfoHelperMultiIndexContainer,m_ZCoord>::type::iterator it0,it1;
  boost::tuples::tie(it0,it1)=get<m_ZCoord>(iContainer).equal_range( iZ );

  FileInfoHelperChannelViewContainer subset;

  while( it0 != it1 )
    {
    subset.insert( &*it0 );
    ++it0;
    }

  FileInfoHelperChannelViewContainer::iterator ic0, ic1;
  ic0 = subset.lower_bound( iCh );
  ic1 = subset.upper_bound( iCh );

  while( ic0 != ic1 )
    {
    oList.push_back( (*ic0)->m_Filename );
    ++ic0;
    }

  return oList;
}

#endif
