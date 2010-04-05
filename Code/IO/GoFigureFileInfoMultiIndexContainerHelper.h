/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#ifndef __GoFigureFileInfoHelperMultiIndexContainer_h
#define __GoFigureFileInfoHelperMultiIndexContainer_h

//#if !defined(NDEBUG)
//#define BOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING
//#define BOOST_MULTI_INDEX_ENABLE_SAFE_MODE
//#endif


#include <string>
#include "boost/multi_index_container.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/ordered_index.hpp"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <list>

// namespace GoFigure
// {

struct GoFigureFileInfoHelper
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


  GoFigureFileInfoHelper( const unsigned int& p, const unsigned int& r,
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

  GoFigureFileInfoHelper() : m_PCoord( 0 ), m_RCoord( 0 ), m_CCoord( 0 ),
    m_XTileCoord( 0 ), m_YTileCoord( 0 ), m_ZTileCoord( 0 ), m_XCoord( 0 ),
    m_YCoord( 0 ), m_ZCoord( 0 ), m_TCoord( 0 ), m_Channel( 0 ), m_Filename( "" )
    {}

  ~GoFigureFileInfoHelper() {}
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
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
#endif

  /// \todo Since we are not yet using all information, the unused one are
  /// commented for performance issue.
  typedef boost::multi_index::multi_index_container<
    GoFigureFileInfoHelper,
    boost::multi_index::indexed_by<
//       ordered_non_unique<
//         tag<m_PCoord>, BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,unsigned int,m_PCoord)>,
//       ordered_non_unique<
//         tag<m_RCoord>, BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,unsigned int,m_RCoord)>,
//       ordered_non_unique<
//         tag<m_CCoord>, BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,unsigned int,m_CCoord)>,
//       ordered_non_unique<
//         tag<m_XTileCoord>, BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,unsigned int,m_XTileCoord)>,
//       ordered_non_unique<
//         tag<m_YTileCoord>, BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,unsigned int,m_YTileCoord)>,
//       ordered_non_unique<
//         tag<m_ZTileCoord>, BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,unsigned int,m_ZTileCoord)>,
    boost::multi_index::ordered_non_unique<
      boost::multi_index::tag<m_ZCoord>,
      BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,unsigned int,m_ZCoord)>,
    boost::multi_index::ordered_non_unique<
      boost::multi_index::tag<m_Channel>,
      BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,unsigned int,m_Channel)>,
    boost::multi_index::ordered_non_unique<
      boost::multi_index::tag<m_TCoord>,
      BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,unsigned int,m_TCoord)>
    >
  > GoFigureFileInfoHelperMultiIndexContainer;

  typedef boost::multi_index::multi_index_container<
    const GoFigureFileInfoHelper*,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_non_unique<
        BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,const unsigned int,m_ZCoord)>
    >
  > GoFigureFileInfoHelperZCoordViewContainer;

  typedef boost::multi_index::multi_index_container<
    const GoFigureFileInfoHelper*,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_non_unique<
        BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,const unsigned int,m_TCoord)>
    >
  > GoFigureFileInfoHelperTCoordViewContainer;

  typedef boost::multi_index::multi_index_container<
    const GoFigureFileInfoHelper*,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_non_unique<
        BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,const unsigned int,m_Channel)>//,
      //ordered_non_unique<BOOST_MULTI_INDEX_MEMBER(GoFigureFileInfoHelper,const unsigned int,m_ZCoord)>
    >
  > GoFigureFileInfoHelperChannelViewContainer;

// }

std::list< std::string > GetAllFileNamesForGivenTCoordAndChannel(
  const GoFigureFileInfoHelperMultiIndexContainer& iContainer,
  const unsigned int& iT,
  const unsigned int& iCh );

std::list< std::string > GetAllFileNamesForGivenZCoordPointAndChannel(
  const GoFigureFileInfoHelperMultiIndexContainer& iContainer,
  const unsigned int& iZ,
  const unsigned int& iCh );

#endif
