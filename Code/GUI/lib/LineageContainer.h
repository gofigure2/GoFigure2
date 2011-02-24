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

#ifndef __LineageContainer_h
#define __LineageContainer_h

#include <QObject>

#include "LineageStructure.h"
#include "StructureHelper.h"

#include "TraceContainerBase.h"

#include "QGoImageView3D.h"

namespace boost
{
  typedef multi_index::multi_index_container<
    LineageStructure,
    boost::multi_index::indexed_by<
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< ActorXY >,
        BOOST_MULTI_INDEX_MEMBER(TraceStructure, vtkActor *, ActorXY)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< ActorXZ >,
        BOOST_MULTI_INDEX_MEMBER(TraceStructure, vtkActor *, ActorXZ)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< ActorYZ >,
        BOOST_MULTI_INDEX_MEMBER(TraceStructure, vtkActor *, ActorYZ)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< ActorXYZ >,
        BOOST_MULTI_INDEX_MEMBER(TraceStructure, vtkActor *, ActorXYZ)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< Nodes >,
        BOOST_MULTI_INDEX_MEMBER(TraceStructure, vtkPolyData *, Nodes)
        >,
      boost::multi_index::ordered_unique<
        boost::multi_index::tag< TraceID >,
        BOOST_MULTI_INDEX_MEMBER(TraceStructure, unsigned int, TraceID)
        >,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag< CollectionID >,
        BOOST_MULTI_INDEX_MEMBER(TraceStructure, unsigned int, CollectionID)
      >,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag< Highlighted >,
        BOOST_MULTI_INDEX_MEMBER(TraceStructure, bool, Highlighted)
        >,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag< Visible >,
        BOOST_MULTI_INDEX_MEMBER(TraceStructure, bool, Visible)
        >,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag< TrackID >,
        BOOST_MULTI_INDEX_MEMBER(LineageStructure, unsigned int, TrackID)
        >,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag< Root >,
        BOOST_MULTI_INDEX_MEMBER(LineageStructure, bool, Root)
        >
      >
    > MultiIndexLineageContainer;
}

/**
  \class LineageContainer
  \brief Wraps a boost multi index container of LineageStructure.
  This class intends to synchronize Lineage representation in
  the Visualization and in the TableWidget
  \sa LineageStructure QGoTableWidget QGoImageView3D
  */
class LineageContainer:
    public TraceContainerBase< boost::MultiIndexLineageContainer >
{
  Q_OBJECT
public:

  typedef TraceContainerBase< boost::MultiIndexLineageContainer > Superclass;

  typedef Superclass::MultiIndexContainerType MultiIndexContainerType;
  typedef Superclass::MultiIndexContainerElementType LineageType;

  //------------------------------------------------------------------------

  /** \brief Constructor. */
  explicit LineageContainer( QObject *iParent,
                           QGoImageView3D *iView);

  /** \brief Destructor. */
  ~LineageContainer();

  /*
   * \brief Extract 1 polydata from a lineage. Useful for the database.
   * Use vtkAppendFilter??
   * \param[in] iLineageID lineage of interest
   * \return Pointer to the new polydata. Has to be deleted?? HOW IMPLEMENT IT WITH REAL TRACKS
   */
  vtkPolyData* extractPolyDataFromLineage( unsigned int iLineageID);

  /*
   * \brief Create a lineage from a polydata. Useful when reading the database.
   * Format: NbOfTracks TrackID MotherTrackID NbOfPoints Points[...]
   *                    TrackID MotherTrackID NbOfPoints Points[...]
   *                    TrackID MotherTrackID NbOfPoints Points[...]
   *                    ...
   * \param[in] iPolyData Pointer to the base polydata
   */
  void extractLineageFromPolyData( vtkPolyData* iPolyData);

  /*
   * \brief Create a new connection between the selected points
   * \param[in] iMoTrackID ID of the mother to find it in the container
   * \param[in] iD1TrackID ID of the first daughter
   * \param[in] iD1PolyData PolyData of the first daughter
   * \param[in] iD2TrackID ID of the second daughter (not required)
   * \param[in] iD2PolyData PolyData of the second daughter (not required)
   */
  void connectTracks( unsigned int iMoTrackID,
                      unsigned int iD1TrackID, vtkPolyData* iD1PolyData,
                      unsigned int iD2TrackID = 0, vtkPolyData* iD2PolyData = NULL);

  /*
   * \brief Cut a lineage after the chosen point.
   * Will Create 2 new lineage from the 2 daughters.
   * \param[in] iMoTrackID ID of the mother to find it in the container
   * \param[in] iNewLineageID1 ID of the first new lineage
   * \param[in] iNewLineageID2 ID of the second new lineage
   * \param[in] iRootPolyData Polydata representing the root track.
   * Required if we cut a track at the root level
   */
  void cutLineage( unsigned int iMoTrackID,
                   unsigned int iNewLineageID1,
                   unsigned int iNewLineageID2,
                   vtkPolyData* iRootPolyData = NULL);

  //void updateActor( unsigned int iLineageID );

  //void createTreeForLineageViewer();

private:
  Q_DISABLE_COPY(LineageContainer);
};

#endif // LineageCONTAINER_H
