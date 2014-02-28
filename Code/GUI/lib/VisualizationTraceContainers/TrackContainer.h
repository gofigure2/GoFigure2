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

#include "vtkMutableDirectedGraph.h"
#include "GoFigureLineageAttributes.h"

/**
  \struct change_visible_division
  \brief Change the visibility of a division with an unary function.
  \sa TrackStructure
  */
//-----------------------------------------------------------------------------
struct change_visible_division
{
  change_visible_division(const bool& iVisible):visible(iVisible){}

  void operator()(TrackStructure& iStructure)
  {
    iStructure.ModifyDivisionVisibility(visible);
  }

private:
  bool visible;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
  \struct change_highlighted_division
  \brief Change the highlight of a division with an unary function.
  \sa TrackStructure
  */
struct change_highlighted_division
{
  change_highlighted_division(vtkProperty* iProperty, bool iHighlight):
    property(iProperty),highlight(iHighlight){}

  void operator()(TrackStructure& iStructure)
  {
    iStructure.ModifyDivisionHighlight(property,highlight);
  }

private:
  vtkProperty* property;
  bool highlight;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
  \struct change_color_division
  \brief Change the color of a division with an unary function.
  \sa TrackStructure
  */
struct change_data_color_division
{
  change_data_color_division(const double* iColor):color(iColor){}

  void operator()(TrackStructure& iStructure)
  {
    iStructure.ModifyDivisionColorData(color);
  }

private:
  const double* color;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
  \struct change_color_division
  \brief Change the color of a division with an unary function.
  \sa TrackStructure
  */
struct change_actor_color_division
{
  change_actor_color_division(const double* iColor):color(iColor){}

  void operator()(TrackStructure& iStructure)
  {
    iStructure.ModifyDivisionColorActor(color);
  }

private:
  const double* color;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
  \struct add_array_division
  \brief Add an array to a division with an unary function.
  Useful for the color coding.
  \sa TrackStructure
  */
struct add_array_division
{
  add_array_division(vtkIntArray* iArray):array(iArray){}

  void operator()(TrackStructure& iStructure)
  {
    iStructure.AddDivisionArray(array);
  }

private:
  vtkIntArray* array;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
  \struct create_node_division
  \brief Create a polydata for a division with an unary function.
  Useful for the color coding.
  \sa TrackStructure
  */
struct create_node_division
{
  create_node_division(vtkPolyData* iNode):node(iNode){}

  void operator()(TrackStructure& iStructure)
  {
    iStructure.CreateDivisionNode(node);
  }

private:
  vtkPolyData* node;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
  /**
  \struct add_avg_volume
  \brief Create a polydata for a division with an unary function.
  Useful for the color coding.
  \sa TrackStructure
  */
struct add_volume
{
  add_volume(const double& iVolume):volume(iVolume){}

  void operator()(TrackStructure& iStructure)
  {
    iStructure.AddVolume(volume);
  }

private:
  double volume;
};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

namespace boost
{
typedef multi_index::multi_index_container<
  TrackStructure,
  boost::multi_index::indexed_by<
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
  bool UpdateTrackStructurePolyData(const TrackStructure& iTrackStructure);

    /** \brief Display all elements for a given time point
  *   \param[in] iT time point
  */
  void ShowActorsWithGivenTimePoint(const unsigned int & iT);

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
  void UpdateTracksStrings(const TList& iTrackList)
    {
    typename TList::const_iterator it = iTrackList.begin();
    unsigned int temp = 0;

    while(it!=iTrackList.end())
      {
      temp = static_cast< unsigned int >(*it);
      // update the current element
      UpdateCurrentElementFromExistingOne(temp);

      // emit signal to get the meshes informations
      emit NeedMeshesInfoForImportedTrack(temp);

      ++it;
      }
    }

  /**
   * \brief Update the current element map then polydata
   * \param[in] iMeshes meshes to be added in the map
   */
  void ImportTrackInCurrentElement(std::map< unsigned int, double*>& iMeshes);

  /**
   * \brief Create new actors for the current polydata and update and visualize
   *  the current actors
   * \param[in] iStructure Structure to be updated
   * \note input paramter not cont sine it is modified in the method
   */
  void CreateTrackActors(TrackStructure& iStructure);

  /*
   * \brief Remove the actors from the visualization if the track has less than 2 points.
   * \param[in] iStructure Structure to be updated
   * \note input paramter not cont sine it is modified in the method
   */
  void UpdateTrackActors(TrackStructure& iStructure);

  /**
  \brief get the element with iTrackID into the current element, remove it from
  the container, recalculate the points from the iListCenterBoundingBox and emit
  a signal for the current element to be saved into the database
  \param[in] iTrackID  ID for the track to be updated
  \param[in] iListCenterBoundingBoxes list of the center of the bounding boxes
  for the meshes belonging to this track */
  TrackStructure* UpdatePointsForATrack(const unsigned int& iTrackID,
                             std::list< double*>& iListCenterBoundingBoxes);

  /**
  \brief Update highlighting property of one element given one actor.
  \param[in] iActor Actor of the element to be modified
  \return true if the element exists
  \return false else
  \note move to superclass
  */
  void UpdateElementHighlighting(const unsigned int& TraceId)
    {
    Qt::CheckState state;
    Superclass::UpdateElementHighlightingWithTraceID(TraceId,
                                                     state);
    emit TracePicked(TraceId, state);
    }

  /*
   * \brief Update the full lineage after picking a division
   * 1-Pick actor
   * 2-Get track ID
   * 3-Get root track ID
   * 4-Modify highlight
   * \param[in] iTraceId ID of the track which owns the picked division
   */
  void UpdateCollectionHighlighting(const unsigned int& iTraceId);

  /*
   * \brief Convenience method to get an iterator to the root structure.
   * Useful for the division picking.
   */
  void GetRootIterator(
      MultiIndexContainerTraceIDIterator& iMotherIterator);

  /**
  \brief Update highlighting property of one element given one actor.
  \param[in] iActor Actor of the element to be modified
  \return true if the element exists
  \return false else
  \note move to superclass*/
  void UpdateElementVisibility(const unsigned int& iTraceID, const bool& iState)
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
  void MergeTrack(const unsigned int& iId1, const unsigned int& iId2);

  /*
   * \brief Set time interval between each image.
   * Useful to estimate the speed of a cell.
   * \param[in] iTimeInterval time interval between 2 time points
   */
  void setTimeInterval(const int& iTimeInterval);

  /*
   * \brief Get time interval between each image.
   * Necessary to estimate the speed of a cell.
   * \return time interval between 2 time points
   */
  int getTimeInterval();

  /*
   * \brief Border type enum to define FIRST and LAST point for a better
   * lisibility
   */
  enum BorderType { FIRST = 0, LAST = 1 };

  /*
   * \brief Convenience to get the position (x,y,z) of a border of a track.
   * (first or last point).
   * Used to create the divisions actors.
   * \param[in] iTrackID track of interest
   * \param[in] iBorder FIRST or LAST
   * \return position of the first point (double* pointing to a double[3])
   */
  double* GetBorderOfTheTrack(const unsigned int& iTrackID,
                              const BorderType& iBorder);

  /*
   * \brief Create divisions from a list of track ids.
   * the list has the following format:
   * motherID daughter1ID daughter2ID motherID daughter1ID ...
   * \param[in] iListOfDivisions list of the track ids to create the divisions
   */
  void SetListOfDivisions(std::list<unsigned int>& iListOfDivisions);

  /*
   * \brief Create a division between 3 tracks.
   * Assigns mother and child pointers. Create 4 actors (one for each view)
   * for this division.
   * \param[in] iMotherID ID of the mother of the division
   * \param[in] iDaughter1ID ID of the daughter1 of the division
   * \param[in] iDaughter2ID ID of the daughter2 of the division
   * \param[in] iVisible visible = true
   */
  void AddDivision( const unsigned int& iMotherID,
                    const unsigned int& iDaughter1ID,
                    const unsigned int& iDaughter2ID,
                    const bool& iVisible = true);

  /*
   * \brief Create 4 actors (one for each view)
   * for this division.
   * \param[in] iPolyData polydata representing the division
   * \param[in] iVisible visibility of the polydata (defaut = true)
   * \return vector of 4 actors (1 for each view) representing the division
   */
  std::vector<vtkActor* > CreateDivisionActor(vtkPolyData* iPolyData,
                                              const bool& iVisible = true);

  /*
   * \brief Create Create a division from 3 track IDs.
   * Updates the node in the structure.
   * \param[in] iMother ID of the mother
   */
  void CreateDivisionPolydata(const unsigned int& iMother);

  /*
   * \brief get the tree below a given division
   * \param[in] iTrackID division to start the search
   * \return list containing all the track IDs
   */
  std::list<unsigned int> GetSubLineage(const unsigned int& iTrackID);
  /*
   * \brief Convenience method to go through the tree and get a SubLineage
   *
   * \param[in] it iterator to go through the lineage
   * \param[in] iList list of tracks to be returned
   */
  void UpdateSubLineage(MultiIndexContainerTraceIDIterator it,
                        std::list<unsigned int>& iList);

  /**
  \brief update the color and the divisions scalars of an all lineage which has
  iTrackIDRoot as track root
  */
  GoFigureLineageAttributes UpdateDivisionsForALineage( unsigned int iTrackIDRoot, double* color);
  /*
   * \brief Update the lineage's divisions scalars given the track root ID
   * \param[in] iTrackID track root ID
   */
  GoFigureLineageAttributes UpdateCollectionScalars(const unsigned int& iTrackID);
  /*
   * \brief Update the lineage's divisions scalars given the track root ID
   * \param[in] iMotherIterator iterator to go through the lineage
   * \param[in] iDepth depth or the structure referenced by the iterator
   */
  void UpdateDivisionScalar(
      MultiIndexContainerTraceIDIterator& iMotherIterator,
      const unsigned int& iDepth);

  GoFigureLineageAttributes GetLineageAttributes(unsigned int iTrackRootID);

  /*
   * \brief Update the lineage's divisions color given the track root ID and
   * a color
   * \param[in] iTrackID track root ID
   * \param[in] color color of the divisions
   */
  void UpdateCollectionColors(const unsigned int& iTrackID,
                              const double* color);
  /*
   * \brief Update the lineage's divisions color given the track root ID and a color
   * \param[in] iMotherIterator iterator to go through the lineage
   * \param[in] color color of the divisions
   */
  void UpdateDivisionColor(
      MultiIndexContainerTraceIDIterator& iMotherIterator,
      const double* iColor);

  /*
   * \brief Update the lineage's divisions data color given the track root ID and
   * a color
   * \param[in] iTrackID track root ID
   * \param[in] color color of the divisions
   */
  void UpdateCollectionColorsData(const unsigned int& iTrackID,
                                  const double* color);
  /*
   * \brief Update the lineage's divisions data color given the track root ID and a color
   * \param[in] iMotherIterator iterator to go through the lineage
   * \param[in] color color of the divisions
   */
  void UpdateDivisionColorData(
      MultiIndexContainerTraceIDIterator& iMotherIterator,
      const double* iColor);

  // compute statistics on the collection
  /*
   * \brief Get the max depth of the lineage
   * \param[in] iTrackRootID id of the track root
   * \return depth of the lineage
   */
  unsigned int GetCollectionMaxDepth(const unsigned int& iTrackRootID);
  /*
   * \brief Update the collection max depth
   * \param[in] it iterator to go through the lineage
   * \param[in] iDivisionDepth depth of the division
   * \param[in] iLineageDepth depth of the lineage
   * \note iLineageDepth should not be const
   */
  void UpdateCollectionMaxDepth(MultiIndexContainerTraceIDIterator& it,
                                const unsigned int& iDivisionDepth,
                                unsigned int& iLineageDepth);
  /*
   * \brief Get the min depth of the lineage
   * \param[in] iTrackRootID id of the track root
   * \return depth of the lineage
   */
  unsigned int GetCollectionMinDepth(const unsigned int& iTrackRootID );
  /*
   * \brief Update the collection min depth
   * \param[in] it iterator to go through the lineage
   * \param[in] iDivisionDepth depth of the division
   * \param[in] iLineageDepth depth of the lineage
   * \note iLineageDepth shouldnt be const
   */
  void UpdateCollectionMinDepth( MultiIndexContainerTraceIDIterator& it,
                                 const unsigned int& iDivisionDepth,
                                 unsigned int& iLineageDepth);
  /*
   * \brief Get the number of divisions of the lineage
   * \param[in] iTrackRootID id of the track root
   * \return depth of the lineage
   */
  unsigned int GetCollectionNumberOfDivisions(
          const unsigned int& iTrackRootID);
  /*
   * \brief Update the collection number of divisions
   * \param[in] it iterator to go through the lineage
   * \param[in] iNumberOfDivisions number of divisions of the lineage
   * \note iNumberOfDivisions shouldnt be const
   */
  void UpdateCollectionNumberOfDivisions(
          MultiIndexContainerTraceIDIterator& it,
          unsigned int& iNumberOfDivisions);
  /*
   * \brief Get the number of leaves of the lineage
   * \param[in] iTrackRootID id of the track root
   * \return depth of the lineage
   */
  unsigned int GetCollectionNumberOfLeaves(const unsigned int& iTrackRootID);
  /*
   * \brief Update the collection number of leaves
   * \param[in] it iterator to go through the lineage
   * \param[in] iNumberOfLeaves number of leaves of the lineage
   * \note iNumberOfLeaves shouldnt be const
   */
  void UpdateCollectionNumberOfLeaves(MultiIndexContainerTraceIDIterator& it,
      unsigned int& iNumberOfLeaves);

  /*
   * \brief Export a vtkMutableDirectedGraph from a given trackID root.
   * \param[in] iTrackID track root ID for the lineage export.
   * \return pointer to vtkMutableDirectedGraph. IMPORTANT: has to be deleted.
   */
  vtkMutableDirectedGraph* ExportLineage(const unsigned int& iTrackID);

  /*
   * \brief Color code lineages from the table widget
   * \param[in] iColumnName selected column name
   * \param[in] iValues pair track root id/value
   */
  void SetCollectionColorCode(const std::string& iColumnName,
                              const std::map< unsigned int,
                              std::string >& iValues);

  /*
   * \brief Randomly color code lineages from the table widget
   * \param[in] iColumnName selected column name
   * \param[in] iValues pair track root id/value
   */
  void SetDivisionRandomColor(const std::string & iColumnName,
          const std::map< unsigned int, std::string >& iValues);

  /*
   * \brief Update whole tree for table widget color coding
   * \param[in] it iterator to go through the lineage
   * \param[in] iColumnName selected column name
   * \param[in] iValue value of the current lineage
   * \param[in|out] iMin to adjust the lookup table after modifying polydata scalars
   * \param[in|out] iMax to adjust the lookup table after modifying polydata scalars
   * \note iMin and iMax should not be constant
   */
  void UpdateDivisionScalarData(MultiIndexContainerTraceIDIterator& it,
                                const std::string& iColumnName,
                                const double& iValue,
                                double& iMin,
                                double& iMax);

  /*
   * \brief Set the scalar range for all the divisions
   * \param[in] iMin min scalar
   * \param[in] iMax max value
   */
  void SetScalarRangeForAllDivisions(const double& iMin, const double& iMax);

  /*
   * \brief Set the LUT for all the divisions
   * \param[in] iLut the lookup table
   */
  void SetLookupTableForAllDivisionsColorCoding(const vtkLookupTable *iLut);

  /*
   * \brief Render all the divisions of all the lineages with the original
   * lineage color
   */
  void RenderAllDivisionsWithOriginalColors();

  /*
   * \brief Go through the lineage and update the graph structure and
   * informations.
   * \param[in] it iterator to go through the lineage
   * \param[in] iGraph graph to be modified
   * \param[in] iPedrigree vertex ID - is unique
   * \param[in] mother vertex ID of the mother - to create edges
   * \param[in] iDepth depth of the node in the lineage - to compute stats
   * \param[in] iDepthArray array to be modified to add information to the graph
   * \param[in] iIDArray array to be modified to add information to the graph
   */
  void UpdateLineage(MultiIndexContainerTraceIDIterator& it,
                     vtkMutableDirectedGraph* iGraph,
                     unsigned int iPedrigree,
                     vtkIdType mother,
                     unsigned int iDepth,
                     vtkDoubleArray* iDepthArray,
                     vtkDoubleArray* iIDArray,
                     vtkDoubleArray* iFirstTPArray, vtkDoubleArray* iLastTPArray);

signals:
  /** \brief When one track has been picked (highlighted) from the visualization */
  void TracePicked(unsigned int, Qt::CheckState);

  /** \brief When one track's visibility has been changed from the visualization */
  void TraceVisibilityChanged(unsigned int, Qt::CheckState);

  /** \brief When we want to import meshes into a track */
  void NeedMeshesInfoForImportedTrack(unsigned int);

  /*
   * \brief Send signal to tell to the lineage container which lineage to highlight
   */
  void UpdateLineageHighlightingFromTrackRootID(unsigned int);

  void UpdateTWCollectionStatus(std::list<unsigned int>, std::list<unsigned int>);

public slots:

  /**
  \brief Change elements highlighting property given a list of TraceIDs and the
  new status.
  \param[in] iList list of TraceIDs
  \param[in] iCheck */
  virtual void UpdateElementHighlightingWithGivenTraceIDs(
          const QStringList& iList, const Qt::CheckState& iCheck);

  /**
  \brief Change elements visibility property given a list of TraceIDs and the
  new status.
  \param[in] iList list of TraceIDs
  \param[in] iCheck */
  virtual void UpdateElementVisibilityWithGivenTraceIDs(
          const QStringList& iList, const Qt::CheckState& iCheck);

  /**
  \brief Color code the track by an array
  \param[in] iColorCode  name of the active array*/
  void ChangeColorCode(const QString& iColorCode);

  /**
  \brief Color code the lineage by an array
  \param[in] iColorCode name of the active array */
  void ChangeDivisionsColorCode(const QString& iColorCode);

  /*
   * \brief Update the representation of a track (adding glyphs, tubes or updating
   *  the line width). If iRadius or iRadius2 are null then the line width gets updated.
   * \param[in] iRadius radius of the glyph
   * \param[in] iRadius2 radius of the tube
   * \param[in] iWidth line width
   */
  void UpdateTracksRepresentation(const double& iRadius,
                                  const double& iRadius2,
                                  const double& iWidth);

  /*
   * \brief Highlight a collection given the track root ID
   * param[in] iRootTrackID trackID root
   * param[in] iHighlighted true (highlight) or false (real color)
   */
  void HighlightCollection(const unsigned int& iRootTrackID,
                           const bool& iHighlighted);
  /*
   * \brief Update the collection highlight
   * \param[in] it iterator to go through the lineage
   * \param[in] iHighlighted highlight (true) or real color (false)
   */
  void UpdateCollectionHighlighted(MultiIndexContainerTraceIDIterator& it,
                                   const bool& iHighlighted);
/*
 * \brief Change the highlight of the division belonging to the given structure.
 * Structure is modified through unary function.
 * \param[in] it iterator to the structure to be modified
 * \param[in] iHighlight highlight (true) or real color (false)
 */
  int ModifyDivisionHighlight(MultiIndexContainerTraceIDIterator& it,
                              const bool& iHighlight);

  /*
   * \brief Show/hide a collection given the track root ID
   * param[in] iRootTrackID trackID root
   * param[in] iVisibility true (show) or false (hide)
   */
  void ShowCollection(const unsigned int&, const bool&);

  /*
   * \brief Update the collection visibility
   * \param[in] it iterator to go through the lineage
   * \param[in] iVisibility show (true) or hide (false)
   */
  void UpdateCollectionVisibility(MultiIndexContainerTraceIDIterator& it,
                                  const bool& iVisibility);

  /*
   * \brief For the given lineages, show the divisions that interect the given time point
   * \param[in] iTrackIDRoot ids of the root tracks of the lineages
   * \param[in] iTimePoint show divisions at this time point
   */
  void ShowCurrentCollection(std::list<unsigned int>& iTrackIDRoot,
                             const unsigned int& iTimePoint);
  /*
   * \brief Recursive method to update the divisions visibility, based on given time point.
   * \return bool true if at least a division of the lineage is visible
   */
  bool UpdateCurrentCollectionVisibility(MultiIndexContainerTraceIDIterator& it,
                                  const int& iTime);
  /*
   * \brief Delete a collection given the track root ID
   * param[in] iRootTrackID trackID root
   */
  void DeleteCollection(unsigned int);  /*
   * \brief Update the collection delete
   * \param[in] it iterator to go through the lineage
   */
  void UpdateCollectionDelete( MultiIndexContainerTraceIDIterator& it);

  /*
   * \brief Change the visibility of the division belonging to the given structure.
   * Structure is modified through unary function.
   * \param[in] it iterator to the structure to be modified
   * \param[in] iVisibility show (true) or hide (false)
   */
  int ModifyDivisionVisibility(MultiIndexContainerTraceIDIterator& it,
                               const bool& iVisibility );

  /*
   * \brief Delete the division belonging to the given track
   * \param[in] iMotherID ID of the track which owns the division
   */
  void DeleteADivision(const unsigned int& iMotherID);

  void AddVolume(const unsigned int& iTrackID, const double& iVolume);

protected:
  /*
   \todo Nicolas why protected??
   */
  /**
  \brief Recompute a polydata from a list of point (coordinates) for the
  current element. If the current element is a new track, then the polydata,
  actors are allocated and added in consequence.
  \param[in] iPoints list of points to generate the new polydata */
  void RecomputeMap( TrackStructure* iStructure, std::list< double* >& iPoints);

  /** \brief Changes the scalars to be displayed and return the new range
   * \param[in] iArrayName Array to be displayed
   * \return Pointer to double[2] where [0] is the min scalar value and [1] is
   * the max scalar value. Pointer has to be deleted (delete[] pointer) */
  double* setTrackNodeScalars(const QString& iArrayName);

  /** \brief Changes the divisions scalars to be displayed and return the new range
   * \param[in] iArrayName Array to be displayed
   * \return Pointer to double[2] where [0] is the min scalar value and [1] is
   * the max scalar value. Pointer has to be deleted (delete[] pointer) */
  double* setDivisionNodeScalars(const QString& iArrayName);

  void ComputeSpeed();

private:
  int m_TimeInterval;
  QString m_ActiveTrackScalars;
  QString m_ActiveDivisionScalars;

    /**
  \brief Change element visibility in the scene
  \tparam TIndex refers to any index from the multi index container indices
  \param[in] iBegin first element
  \param[in] iEnd last element
  \param[in] iVisibility
  */
  template< class TIndex >
  void ChangeActorsVisibility(
    typename MultiIndexContainerType::template index< TIndex >::type::iterator iBegin,
    bool iVisibility)
    {
    typedef void ( QGoImageView3D::*ImageViewMember )(const int &, vtkActor *);
    ImageViewMember f;

    if ( iVisibility )
      {
      f = &QGoImageView3D::AddActor;
      }
    else
      {
      f = &QGoImageView3D::RemoveActor;
      }

    if ( iBegin->Visible != iVisibility )
      {
      iBegin->SetActorVisibility( iVisibility );

      if( m_ImageView )
        {
        if ( iBegin->ActorXY )
          {
          ( m_ImageView->*f )(0, iBegin->ActorXY);
          }
        if ( iBegin->ActorXZ )
          {
          ( m_ImageView->*f )(1, iBegin->ActorXZ);
          }
        if ( iBegin->ActorYZ )
          {
          ( m_ImageView->*f )(2, iBegin->ActorYZ);
          }
        if ( iBegin->ActorXYZ )
          {
          ( m_ImageView->*f )(3, iBegin->ActorXYZ);
          }
        }

      bool visible = iVisibility;
      m_Container.get< TIndex >().
          modify( iBegin , change_visible<TrackStructure>(visible) );

      Qt::CheckState State;

      if ( iVisibility )
        {
        State = Qt::Checked;
        }
      else
        {
        State = Qt::Unchecked;
        }
      emit TraceVisibilityChanged( iBegin->TraceID, State );
      }
    }

  Q_DISABLE_COPY(TrackContainer);
};

#endif // TrackCONTAINER_H
