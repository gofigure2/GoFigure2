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

#ifndef __TrackContainer_h
#define __TrackContainer_h

#include <QObject>

#include "TrackStructure.h"
#include "StructureHelper.h"

#include "TraceContainerBase.h"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/ordered_index.hpp"
#include "boost/numeric/conversion/cast.hpp"
#include "boost/lexical_cast.hpp"

#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkMapper.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "QGoImageView3D.h"
#include "vtkLookupTableManager.h"

namespace boost
{
  typedef multi_index::multi_index_container<
    TrackStructure,
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
        >
      >
    > MultiIndexTrackContainer;
}

/**
  \class TrackContainer
  \brief Wraps a boost multi index container of TrackStructure.
  This class intends to synchronize Track representation in
  the Visualization and in the TableWidget
  \sa TrackStructure QGoTableWidget QGoImageView3D
  */
class TrackContainer:
    public TraceContainerBase< boost::MultiIndexTrackContainer >
{
  Q_OBJECT
public:

  typedef TraceContainerBase< boost::MultiIndexTrackContainer > Superclass;

  typedef Superclass::MultiIndexContainerType MultiIndexContainerType;
  typedef Superclass::MultiIndexContainerElementType TrackType;

  typedef TrackType::PointsMapType PointsMapType;
  typedef TrackType::PointsMapIterator PointsMapIterator;
  typedef TrackType::PointsMapConstIterator PointsMapConstIterator;

  //------------------------------------------------------------------------

  /** \brief Constructor. */
  explicit TrackContainer( QObject *iParent,
                           QGoImageView3D *iView);

  /** \brief Destructor. */
  ~TrackContainer();

  /** \brief Display all elements for a given time point
  *   \param[in] iT time point
  */
  // void ShowActorsWithGivenTimePoint(const unsigned int & iT);

  /** \brief Update Current Element by providing all required information
  from the visualization.
  \param[in] iActors
  \param[in] iNodes
  \param[in] iT
  \param[in] iHighlighted
  \param[in] iVisible
  \see TrackStructure
  */
  void UpdateCurrentElementFromVisu(std::vector< vtkActor * > iActors,
                                    vtkPolyData *iNodes,
                                    const bool & iHighlighted,
                                    const bool & iVisible);


  /*
  \brief Remove all actors (elements) from the scene for a given time point
  \param[in] iT
  */
  // void RemoveActorsWithGivenTimePoint(const unsigned int & iT);

  /*
    \brief Add all actors (elements) from the scene for a given time point
  */
  // void AddActorsWithGivenTimePoint(const unsigned int & iT);

  //-------------------------------------------------------------------------

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
  bool DeleteElement(const unsigned int & iId);

  /**
  \overload DeleteElement(const unsigned int & iId)
  */
  bool DeleteElement(MultiIndexContainerTraceIDIterator iIter);

  /** \brief Delete all highlighted elements
  \return the list of TraceIDs of such elements
  */
  std::list< unsigned int > DeleteAllHighlightedElements();

  /**
    \brief Add a point in the current track.
    If there is already a point existing at the same time point, we don't do anything.
    We could add a variable to either override or not the point who already
    exists at a current time point.
    \param[in] iPoint new point to be added in the track
    \param[in] iTime time coordinate of the new point
    \param[in] iReconstructPolyData should we reconstruct the polydata
  */
  //bool AddPointToCurrentElement(int iTime, double* iPoint, bool iReconstructPolyData = true);

  /**
    \brief Delete a point from the current track.
    \param[in] iTime time point to clear
    \param[in] iReconstructPolyData should we reconstruct the polydata
    \return true: a point has been deleted, false: no point has been deleted
  */
  bool DeletePointFromCurrentElement( unsigned int iTime,
                                      bool iReconstructPolyData );

  /**
    \brief Replace a point from the current track.
    \param[in] iPoint new point to be added in the track
    \param[in] iTime time point to update
    \return true: a point has been replace, false: no point has been replaced
  */
  //bool ReplacePointFromCurrentElement(int iTime, double* iPoint);

  /**
    \brief Update the TrackStructure polydata according to the current map.
    \param[in] iTrackStructure Structure to be updated
    \return true if the polydata has been updated, false if it hasn't (i.e. mesh without point)
  */
  bool UpdateTrackStructurePolyData( const TrackStructure& iTrackStructure);

  /**
    \brief Update the current element actors from the visualization.
    \param[in] iActors Vector container an actor pointer for each view
  */
  void UpdateCurrentElementActorsFromVisu(std::vector< vtkActor * > iActors);

  /**
    \brief Update the points strings of the tracks
    for each element of the list
    {
    1 -Add trackID to current element
    2- Remove old element
    3- Get corresponding meshes centers
    4- convert coordinates
    5- fill map
    6- generate new polydata
    7- emit signal (to save in DB + insert element)
    8- update the visu
    }
    \param[in] iTrackList List containing IDs of the track of interest
  */
  template< class TList >
  void UpdateTracksStrings( const TList& iTrackList)
    {
    typename TList::const_iterator it = iTrackList.begin();
    unsigned int temp = 0;

    while( it != iTrackList.end() )
      {
      temp = static_cast< unsigned int >( * it );
      // update the current element
      UpdateCurrentElementFromExistingOne( temp );

      // emit signal to get the meshes informations
      emit NeedMeshesInfoForImportedTrack( temp );

      ++it;
      }
    }

  /**
   * \brief Update the current element map then polydata
   * \param[in] iMeshes meshes to be added in the map
   */
  void UpdateCurrentElementMap( std::map< unsigned int, double* > iMeshes);

  /**
   * \brief Create new actors for the current polydata and update and visualize
   *  the current actors
   */
  void CreateCurrentTrackActors();

  /**
   * \brief Delete the points which are at the selected time point from the
   * current element.
   * \param[in] iTimeList List of the time points to be deleted.
   */
  void DeleteListFromCurrentElement( const std::list<unsigned int>& iTimeList );

  /**
   * \brief Define the appareance of a track (line/tubes, glyph/no glyph)
   * \param[in] iGlyph Do we want to see glyph(true)/no glyph(false)
   * \param[in] iTube Do we want to see a tube(true)/polyline(false)
   */
  void UpdateTracksReprensentation( bool iGlyph, bool iTube );


  /**
   * \brief Delete a list of tracks.
   * \param[in] iPointsToBeDeleted a list containing pairs.
   * Each pair is composed by a track ID and a list of the time points to be
   * deleted in this track ID.
   *
   * \todo Why does this method exist? \sa DeleteElement
   * \todo Let's assume this method is useful, why not using a map or
   * multi_map instead?
   */
  /*void DeleteListOfTracks(
      std::list< std::pair< unsigned int , std::list< unsigned int > > >
                 iPointsToBeDeleted);*/

  /**
    \brief Delete a point from a track.
    \param[in] iIterator trackstructure which will be modified
    \param[in] iTime time point to clear
    \param[in] iReconstructPolyData should we reconstruct the polydata
    \return true: a point has been deleted, false: no point has been deleted
  */
  bool DeletePointFromElement(
    MultiIndexContainerTraceIDIterator iIterator,
    unsigned int iTime,
    bool iReconstructPolyData );

  /**
    \brief Add traces (collections of contours / meshes) to a track, given their
    bounding box.
    \param[in] iTrackID track id
    \param[in] iBoundingBox list of bounding boxes
  */
  void UpdatePointsFromBBForGivenTrack(
    unsigned int iTrackID,
    std::list<std::vector<unsigned int> > iBoundingBox);

  /**
  \brief get the element with iTrackID into the current element, remove it from the container,
  recalculate the points from the iListCenterBoundingBox and emit a signal for the current element
  to be saved into the database
  \param[in] iTrackID  ID for the track to be updated
  \param[in] iListCenterBoundingBox list of the center of the bounding boxes for the meshes belonging
  to this track
  */
  void UpdatePointsForATrack(unsigned int iTrackID,
                             std::list< double*> iListCenterBoundingBoxes);

  /**
  \brief Update highlighting property of one element given one actor.
  \param[in] iActor Actor of the element to be modified
  \return true if the element exists
  \return false else
  */
  template< class TActor >
  bool UpdateElementHighlightingWithGivenActor(vtkActor *iActor)
    {
    unsigned TraceId;
    Qt::CheckState state;
    bool oValue =
        Superclass::UpdateElementHighlightingWithGivenActor< TActor >( iActor,
                                                                 TraceId,
                                                                 state );
    if( oValue )
      {
      emit TracePicked(TraceId, state);
      }
    return oValue;
    }

  /**
  \brief Update highlighting property of one element given one actor.
  \param[in] iActor Actor of the element to be modified
  \param[in] iState Visibility to applied to the element
  \return true if the element exists
  \return false else
  */
  template< class TActor >
  bool UpdateElementVisibilityWithGivenActor(
      vtkActor *iActor )
    {
    unsigned TraceId;
    Qt::CheckState state;
    bool oValue =
        Superclass::UpdateElementVisibilityWithGivenActor< TActor >( iActor,
                                                                 TraceId,
                                                                 state );
    if( oValue )
      {
      emit TraceVisibilityChanged(TraceId, state);
      }
    return oValue;
    }

  void MergeTrack( const unsigned int& iId1, const unsigned int& iId2 );

  std::map< unsigned int, std::pair< const double* , vtkPolyData*> >
  GetHighlightedElementsTrackPolyData();

signals:
  /** \brief When one track has been picked (highlighted) from the visualization */
  void TracePicked(unsigned int, Qt::CheckState);

  /** \brief When one track's visibility has been changed from the visualization */
  void TraceVisibilityChanged(unsigned int, Qt::CheckState);


  /** \brief When a point is added to the track, update the database */
  void CurrentTrackToSave();

  /** \brief When we want to import meshes into a track */
  void NeedMeshesInfoForImportedTrack(unsigned int);

public slots:

  /** \brief Change elements highlighting property given a list of TraceIDs
  and the new status.
    \param[in] iList list of TraceIDs
    \param[in] iCheck */
  void UpdateElementHighlightingWithGivenTraceIDs( const QStringList& iList,
                                                   const Qt::CheckState& iCheck );

  /** \brief Change elements visibility property given a list of TraceIDs
  and the new status.
    \param[in] iList list of TraceIDs
    \param[in] iCheck */
  void UpdateElementVisibilityWithGivenTraceIDs( const QStringList& iList,
                                                 const Qt::CheckState& iCheck );

  void ColorCodeTracksByTime( bool );

  double* getRange();

protected:

  /**
    \brief Recompute a polydata from a list of point (coordinates) for the
    current element. If the current element is a new track, then the polydata,
    actors are allocated and added in consequence.
    \param[in] iPoints list of points to generate the new polydata
  */
  void RecomputeCurrentElementMap( std::list< double* > iPoints);

  std::vector< vtkActor* > AddTrace( vtkPolyData* , vtkProperty* );

private:
  Q_DISABLE_COPY(TrackContainer);
};

#endif // TrackCONTAINER_H
