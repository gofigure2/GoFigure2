/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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

#ifndef CONTOURMESHCONTAINER_H
#define CONTOURMESHCONTAINER_H

#include "ContourMeshStructure.h"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/ordered_index.hpp"

class QGoImageView3D;
class vtkProperty;

class ContourMeshContainer
{
public:

  typedef boost::multi_index::multi_index_container<
      ContourMeshStructure,
      boost::multi_index::indexed_by<
        boost::multi_index::ordered_non_unique<
          boost::multi_index::tag<TCoord>,
          BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, unsigned int, TCoord)
          >,
        boost::multi_index::hashed_non_unique<
          boost::multi_index::tag<ActorXY>,
          BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, vtkActor*, ActorXY)
          >,
        boost::multi_index::hashed_non_unique<
          boost::multi_index::tag<ActorXZ>,
          BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, vtkActor*, ActorXZ)
          >,
        boost::multi_index::hashed_non_unique<
          boost::multi_index::tag<ActorYZ>,
          BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, vtkActor*, ActorYZ)
          >,
        boost::multi_index::hashed_non_unique<
          boost::multi_index::tag<ActorXYZ>,
          BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, vtkActor*, ActorXYZ)
          >,
        boost::multi_index::hashed_non_unique<
          boost::multi_index::tag<Nodes>,
          BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, vtkPolyData*, Nodes)
          >,
        boost::multi_index::ordered_unique<
          boost::multi_index::tag<TraceID>,
          BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, unsigned int, TraceID)
          >
        >
      > MultiIndexContainer;

  typedef MultiIndexContainer::index<TCoord>::type::iterator
        MultiIndexContainerTCoordIterator;

  typedef MultiIndexContainer::index<ActorXY>::type::iterator
        MultiIndexContainerActorXYIterator;

  typedef MultiIndexContainer::index<ActorXZ>::type::iterator
        MultiIndexContainerActorXZIterator;

  typedef MultiIndexContainer::index<ActorYZ>::type::iterator
        MultiIndexContainerActorYZIterator;

  typedef MultiIndexContainer::index<ActorXYZ>::type::iterator
        MultiIndexContainerActorXYZIterator;

  typedef MultiIndexContainer::index<Nodes>::type::iterator
        MultiIndexContainerNodesIterator;

  typedef MultiIndexContainer::index<TraceID>::type::iterator
        MultiIndexContainerTraceIDIterator;

  ContourMeshContainer( QGoImageView3D* iView );

  ~ContourMeshContainer( );

  MultiIndexContainer m_Container;

  QGoImageView3D* m_ImageView;

  void RemoveActorsWithGivenTimePoint( const unsigned int& iT );

  /**
    \brief Add elements to the visualization given it TraceID.
    \param[in] iId TraceID of the element to be displaid
    \note This method should be used only when changing time point.
  */
  void AddActorsWithGivenTimePoint( const unsigned int& iT );

  /**
    \brief Remove element from visualization
    \param[in] iId TraceID of the element to be removed
    \return true if the element was present in the container.
  */
  bool RemoveElementFromVisualizationWithGivenTraceID(
      const unsigned int& iId );

  /**
    \brief Update element highlighting given it TraceId
    \param[in] iId TraceID of the element to be modified
    \param[in] iProperty highlighting property
    \return true if the element was present in the container.
  */
  bool UpdateElementHighlightingWithGivenTraceID( const unsigned int& iId,
                                                  vtkProperty* iProperty );

  /**
    \brief Update element visibility given it TraceId
    \param[in] iId TraceID of the element to be modified
    \return true if the element was present in the container.
  */
  bool UpdateElementVisibilityWithGivenTraceID( const unsigned int& iId );

  /**
    \brief Remove the element which TraceId = iId
    <ul>
      <li>from the visualization
      <li>from the container
      <li>from memory (release memory)
    </ul>
    \param[in] iId TraceID of the element to be deleted
    \return true if the element was present in the container.
  */
  bool DeleteElement( const unsigned int& iId );


};


#endif // CONTOURMESHCONTAINER_H
