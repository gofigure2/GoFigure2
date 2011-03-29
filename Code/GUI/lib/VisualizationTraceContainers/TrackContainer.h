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
#include "QGoGUILibConfigure.h"
#include <QString>

//-----------------------------------------------------------------------------
struct change_visible_division
{
  change_visible_division(bool& iVisible):visible(iVisible){}

  void operator()(TrackStructure iStructure)
  {
    iStructure.ModifyDivisionVisibility(visible);
  }

private:
  bool visible;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
struct change_highlighted_division
{
  change_highlighted_division(bool& iHilighted):highlighted(iHilighted){}

  void operator()(TrackStructure iStructure)
  {
    iStructure.ModifyDivisionHighlight(highlighted);
  }

private:
  bool highlighted;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

namespace boost
{
typedef multi_index::multi_index_container<
  TrackStructure,
  boost::multi_index::indexed_by<
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
class QGOGUILIB_EXPORT TrackContainer:
    public TraceContainerBase< boost::MultiIndexTrackContainer >
{
  Q_OBJECT
public:

  typedef TraceContainerBase< boost::MultiIndexTrackContainer > Superclass;

  typedef Superclass::MultiIndexContainerType         MultiIndexContainerType;
  typedef Superclass::MultiIndexContainerElementType  TrackType;

  typedef TrackType::PointsMapType          PointsMapType;
  typedef TrackType::PointsMapIterator      PointsMapIterator;
  typedef TrackType::PointsMapConstIterator PointsMapConstIterator;

  //------------------------------------------------------------------------

  /** \brief Constructor. */
  explicit TrackContainer( QObject *iParent,
                           QGoImageView3D *iView);

  /** \brief Destructor. */
  virtual ~TrackContainer();

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
    \brief Update the TrackStructure polydata according to the current map.
    \param[in] iTrackStructure Structure to be updated
    \return true if the polydata has been updated
    \return false if it hasn't (i.e. mesh without point)
  */
  bool UpdateTrackStructurePolyData( const TrackStructure& iTrackStructure);

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
  void ImportTrackInCurrentElement( std::map< unsigned int, double* > iMeshes);

  /**
   * \brief Create new actors for the current polydata and update and visualize
   *  the current actors
   */
  void CreateTrackActors( TrackStructure& iStructure);

  /**
  \brief get the element with iTrackID into the current element, remove it from
  the container, recalculate the points from the iListCenterBoundingBox and emit
  a signal for the current element to be saved into the database
  \param[in] iTrackID  ID for the track to be updated
  \param[in] iListCenterBoundingBoxes list of the center of the bounding boxes
  for the meshes belonging to this track */
  TrackStructure* UpdatePointsForATrack(unsigned int iTrackID,
                             std::list< double*> iListCenterBoundingBoxes);

  /**
  \brief Update highlighting property of one element given one actor.
  \param[in] iActor Actor of the element to be modified
  \return true if the element exists
  \return false else
  \note move to superclass
  */
  void UpdateElementHighlighting(unsigned int TraceId)
    {
    Qt::CheckState state;
    Superclass::UpdateElementHighlightingWithTraceID(TraceId,
                                                     state );
    emit TracePicked(TraceId, state);
    }

  /**
  \brief Update highlighting property of one element given one actor.
  \param[in] iActor Actor of the element to be modified
  \return true if the element exists
  \return false else
  \note move to superclass*/
  void UpdateElementVisibility(unsigned int iTraceID, bool iState)
    {
    Superclass::UpdateElementVisibilityWithTraceID(iTraceID, iState);
    if(iState)
      {
      emit TraceVisibilityChanged(iTraceID, Qt::Checked );
      return;
      }
    emit TraceVisibilityChanged(iTraceID, Qt::Unchecked );
    }

  /*
   * \brief Merge given tracks
   * \param[in] iId1 ID of the first track
   * \param[in] iId2 ID of the second track
   */
  void MergeTrack( const unsigned int& iId1, const unsigned int& iId2 );

  /*
   * \brief Set time interval between each image.
   * Useful to estimate the speed of a cell.
   * \param[in] iTimeInterval time interval between 2 time points
   */
  void setTimeInterval( int iTimeInterval);

  /*
   * \brief Get time interval between each image.
   * Necessary to estimate the speed of a cell.
   * \return time interval between 2 time points
   */
  int getTimeInterval();

  /*
   * \brief Convenience to get the position of the first point of a track.
   * Used to create the divisions actors.
   * \param[in] iTrackID track of interest
   * \return position of the first point (double* pointing to a double[3])
   */
  double* GetFirstPointOfTheTrack(unsigned int iTrackID);

  /*
   * \brief Convenience to get the position of the last point of a track.
   * Used to create the divisions actors.
   * \param[in] iTrackID track of interest
   * \return position of the last point (double* pointing to a double[3])
   */
  double* GetLastPointOfTheTrack(unsigned int iTrackID);

  /*
   * \brief Create divisions from a list of track ids.
   * the list has the following format:
   * motherID daughter1ID daughter2ID motherID daughter1ID ...
   * \param[in] iListOfDivisions list of the track ids to create the divisions
   */
  void SetListOfDivisions( std::list<unsigned int> iListOfDivisions);

  /*
   * \brief Create a division between 3 tracks.
   * Assigns mother and child pointers. Create 4 actors (one for each view)
   * for this division.
   * \param[in] iMotherID ID of the mother of the division
   * \param[in] iDaughter1ID ID of the daughter1 of the division
   * \param[in] iDaughter2ID ID of the daughter2 of the division
   */
  void AddDivision( unsigned int iMotherID, unsigned int iDaughter1ID,
      unsigned int iDaughter2ID);

  /*
   * \brief Create 4 actors (one for each view)
   * for this division.
   * \param[in] iMotherID ID of the mother of the division
   * \param[in] iDaughter1ID ID of the daughter1 of the division
   * \param[in] iDaughter2ID ID of the daughter2 of the division
   * \return vector of 4 actors (1 for each view) representing the division
   */
  std::vector<vtkActor* > CreateDivisionActor( unsigned int iMother, unsigned int iDaughter1,
      unsigned int iDaughter2);

  /*
   * \brief Cut the lineage after the given track ID. Modifies mother child pointers to NULL.
   * Modifies child mother pointer to NULL;
   * \param[in] iMotherID last track of the "mother lineage"
   */
  void CutLineage(unsigned int iMotherID);


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

  /**
  \brief Change elements highlighting property given a list of TraceIDs and the
  new status.
  \param[in] iList list of TraceIDs
  \param[in] iCheck */
  virtual void UpdateElementHighlightingWithGivenTraceIDs( const QStringList& iList,
                                                   const Qt::CheckState& iCheck );

  /**
  \brief Change elements visibility property given a list of TraceIDs and the
  new status.
  \param[in] iList list of TraceIDs
  \param[in] iCheck */
  virtual void UpdateElementVisibilityWithGivenTraceIDs( const QStringList& iList,
                                                 const Qt::CheckState& iCheck );

  /**
  \brief Color code the track by time.
  \param[in] iColorCode Display Time Color Code (true) or Real Color (false) */
  void ChangeColorCode( const char* iColorCode);

  void UpdateTracksRepresentation( double iRadius,double iRadius2);

  void UpdateCollectionVisibility( MultiIndexContainerTraceIDIterator it, bool iVisibility,
      int (TrackContainer::*iFunction)(MultiIndexContainerTraceIDIterator, bool));

  void HighlightCollection(unsigned int, bool);
  int ModifyDivisionHighlight( MultiIndexContainerTraceIDIterator it, bool iHighlight );

  void ShowCollection(unsigned int, bool);
  int ModifyDivisionVisibility( MultiIndexContainerTraceIDIterator it, bool iVisibility );

  void UpdateTrackStructureLineage(TrackStructure* iStructure);

  void UpdateDivisionActor(TrackStructure* iStructure);

  void DeleteADivision( unsigned int iMotherID);

protected:
  /**
  \brief Recompute a polydata from a list of point (coordinates) for the
  current element. If the current element is a new track, then the polydata,
  actors are allocated and added in consequence.
  \param[in] iPoints list of points to generate the new polydata */
  void RecomputeMap( TrackStructure* iStructure, std::list< double* > iPoints);

  /** \brief Changes the scalars to be displayed and return the new range
   * \param[in] iArrayName Array to be displayed
   * \return Pointer to double[2] where [0] is the min scalar value and [1] is
   * the max scalar value. Pointer has to be deleted (delete[] pointer) */
  double* setNodeScalars(const char *iArrayName);

  void ComputeSpeed();

private:
  int m_TimeInterval;
  QString m_ActiveScalars;

  Q_DISABLE_COPY(TrackContainer);
};

#endif // TrackCONTAINER_H
