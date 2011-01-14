/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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

#include "GoFigureFileInfoMultiIndexContainerHelper.h"

// method to speed up the display
std::map< unsigned int, std::list< std::string > >
GetAllFileNamesForGivenTCoord(
  const GoFigureFileInfoHelperMultiIndexContainer & iContainer,
  const unsigned int & iT,
  const unsigned int & iMinCh,
  const unsigned int & iMaxCh
  )
{
  std::map< unsigned int, std::list< std::string > > oList;

  using boost::multi_index::index;
  using boost::multi_index::get;
  using boost::tuples::tie;

  index< GoFigureFileInfoHelperMultiIndexContainer, m_TCoord >::type::iterator it0, it1;
  tie(it0, it1) = get< m_TCoord >(iContainer).equal_range(iT);

  GoFigureFileInfoHelperChannelViewContainer subset;

  while ( it0 != it1 )
    {
    subset.insert(&*it0);
    ++it0;
    }

  GoFigureFileInfoHelperChannelViewContainer::nth_index< 0 >::type::iterator ic0, ic1;
  ic0 = subset.get< 0 >().lower_bound(iMinCh);
  ic1 = subset.get< 0 >().upper_bound(iMaxCh);

  GoFigureFileInfoHelperZCoordViewContainer final_container;

  while ( ic0 != ic1 )
    {
    final_container.insert(*ic0);
    ++ic0;
    }

  GoFigureFileInfoHelperZCoordViewContainer::iterator z_it0 = final_container.begin();
  GoFigureFileInfoHelperZCoordViewContainer::iterator z_it1 = final_container.end();

  while ( z_it0 != z_it1 )
    {
    oList[( *z_it0 )->m_Channel].push_back( ( *z_it0 )->m_Filename );
    ++z_it0;
    }

  return oList;
}

std::map< unsigned int, std::list< std::string > >
GetAllFileNamesForGivenZCoord(
  const GoFigureFileInfoHelperMultiIndexContainer & iContainer,
  const unsigned int & iZ,
  const unsigned int & iMinCh,
  const unsigned int & iMaxCh
  )
{
  std::map< unsigned int, std::list< std::string > > oList;

  using boost::multi_index::index;
  using boost::multi_index::get;
  using boost::tuples::tie;

  index< GoFigureFileInfoHelperMultiIndexContainer, m_ZCoord >::type::iterator it0, it1;
  tie(it0, it1) = get< m_ZCoord >(iContainer).equal_range(iZ);

  GoFigureFileInfoHelperChannelViewContainer subset;

  while ( it0 != it1 )
    {
    subset.insert(&*it0);
    ++it0;
    }

  GoFigureFileInfoHelperChannelViewContainer::nth_index< 0 >::type::iterator ic0, ic1;
  ic0 = subset.get< 0 >().lower_bound(iMinCh);
  ic1 = subset.get< 0 >().upper_bound(iMaxCh);

  GoFigureFileInfoHelperTCoordViewContainer final_container;

  while ( ic0 != ic1 )
    {
    final_container.insert(*ic0);
    ++ic0;
    }

  GoFigureFileInfoHelperTCoordViewContainer::iterator z_it0 = final_container.begin();
  GoFigureFileInfoHelperTCoordViewContainer::iterator z_it1 = final_container.end();

  while ( z_it0 != z_it1 )
    {
    oList[( *z_it0 )->m_Channel].push_back( ( *z_it0 )->m_Filename );
    ++z_it0;
    }

  return oList;
}

std::list< std::string > GetAllFileNamesForGivenTCoordAndChannel(
  const GoFigureFileInfoHelperMultiIndexContainer & iContainer,
  const unsigned int & iT,
  const unsigned int & iCh)
{
  std::list< std::string > oList;

  using boost::multi_index::index;
  using boost::multi_index::get;
  using boost::tuples::tie;

  index< GoFigureFileInfoHelperMultiIndexContainer, m_TCoord >::type::iterator it0, it1;
  tie(it0, it1) = get< m_TCoord >(iContainer).equal_range(iT);

  GoFigureFileInfoHelperChannelViewContainer subset;

  while ( it0 != it1 )
    {
    subset.insert(&*it0);
    ++it0;
    }

  GoFigureFileInfoHelperChannelViewContainer::nth_index< 0 >::type::iterator ic0, ic1;
  ic0 = subset.get< 0 >().lower_bound(iCh);
  ic1 = subset.get< 0 >().upper_bound(iCh);

  GoFigureFileInfoHelperZCoordViewContainer final_container;

  while ( ic0 != ic1 )
    {
    final_container.insert(*ic0);
    ++ic0;
    }

  GoFigureFileInfoHelperZCoordViewContainer::iterator z_it0 = final_container.begin();
  GoFigureFileInfoHelperZCoordViewContainer::iterator z_it1 = final_container.end();

  while ( z_it0 != z_it1 )
    {
    oList.push_back( ( *z_it0 )->m_Filename );
    ++z_it0;
    }

  return oList;
}

std::list< std::string > GetAllFileNamesForGivenZCoordPointAndChannel(
  const GoFigureFileInfoHelperMultiIndexContainer & iContainer,
  const unsigned int & iZ,
  const unsigned int & iCh)
{
  std::list< std::string > oList;

  using boost::multi_index::index;
  using boost::multi_index::get;
  using boost::tuples::tie;

  index< GoFigureFileInfoHelperMultiIndexContainer, m_ZCoord >::type::iterator it0, it1;
  tie(it0, it1) = get< m_ZCoord >(iContainer).equal_range(iZ);

  GoFigureFileInfoHelperChannelViewContainer subset;

  while ( it0 != it1 )
    {
    subset.insert(&*it0);
    ++it0;
    }

  GoFigureFileInfoHelperChannelViewContainer::nth_index< 0 >::type::iterator ic0, ic1;
  ic0 = subset.get< 0 >().lower_bound(iCh);
  ic1 = subset.get< 0 >().upper_bound(iCh);

  GoFigureFileInfoHelperTCoordViewContainer final_container;

  while ( ic0 != ic1 )
    {
    final_container.insert(*ic0);
    ++ic0;
    }

  GoFigureFileInfoHelperTCoordViewContainer::iterator z_it0 = final_container.begin();
  GoFigureFileInfoHelperTCoordViewContainer::iterator z_it1 = final_container.end();

  while ( z_it0 != z_it1 )
    {
    oList.push_back( ( *z_it0 )->m_Filename );
    ++z_it0;
    }

  return oList;
}

std::map< unsigned int, std::list< std::string > >
GetAllFileNamesForGivenChannelAndTCoords(
    const GoFigureFileInfoHelperMultiIndexContainer & iContainer,
    const unsigned int & iCh,
    const std::set< unsigned int > & iTCoordList
    )
{
  std::map< unsigned int, std::list< std::string > > oList;

  std::set< unsigned int >::const_iterator t_it = iTCoordList.begin();

  while( t_it != iTCoordList.end() )
    {
    oList[ *t_it ] = GetAllFileNamesForGivenTCoordAndChannel( iContainer, *t_it, iCh );
    ++t_it;
    }

  return oList;
}
