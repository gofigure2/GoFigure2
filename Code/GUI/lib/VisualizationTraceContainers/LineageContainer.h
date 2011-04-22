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
        boost::multi_index::tag< TrackRootID >,
        BOOST_MULTI_INDEX_MEMBER(LineageStructure, unsigned int, TrackRootID)
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

  //typedef typename MultiIndexContainerType::template index< TrackRootID >::type::iterator
  //MultiIndexContainerTrackRootIDIterator;

  //------------------------------------------------------------------------

  /** \brief Constructor. */
  explicit LineageContainer( QObject *iParent,
                           QGoImageView3D *iView);

  /** \brief Destructor. */
  virtual ~LineageContainer();

  /**
  \brief insert a new element in the container with all the info needed
  \param[in] iLineageID ID for the new lineage
  \param[in] irgba color of the lineage
  \param[in] iTrackIDRoot for the new lineage
  \param[in] IsVisible
  */
  void InsertNewLineage(
    const unsigned int& iLineageID,
    double irgba[4],
    const unsigned int& iTrackIDRoot,
    const bool& IsVisible = false);

  /*
   * \brief Get the list of all the lineages roots, as track IDs
   * \return list containg the track root ids
   */
  std::list<unsigned int> GetListOfTrackRootIDs();

  /*
   * \brief Get the list of all the lineages IDs
   * \return list containg the lineage ids
   */
  std::list<unsigned int> GetListOfLineageIDs();

  /*
   * \brief Get the track root ID of the given lineage
   * \param[in] iTraceID lineage ID of interest
   * \return related track root ID
   */
  unsigned int GetLineageTrackRootID( const unsigned int& iTraceID );

  /*
   * \brief Get the trace root ID from the track root ID
   * \param[in] iTraceID track root id
   * \return related lineage ID
   */
  unsigned int GetTraceIDFromTrackRootID( const unsigned int& iTraceID );

  /*
   * \brief Get the visibility of the given lineage
   * \param[in] iTraceID lineage ID of interest
   * \return visibility
   */
  bool GetLineageVisibile( const unsigned int& iTraceID );

  /*
   * \brief Get the hilighted of the given lineage
   * \param[in] iTraceID lineage ID of interest
   * \return highlighted
   */
  bool GetLineageHighlighted( const unsigned int& iTraceID );

  /*
   * \brief Get the color of the given lineage
   * \param[in] iTraceID lineage ID of interest
   * \return color
   */
  double* GetLineageColor( const unsigned int& iTraceID );

  /**\todo implement them:*/
  bool DeleteElement(const unsigned int & iId);
  bool DeleteElement(MultiIndexContainerTraceIDIterator iIter);
  std::list< unsigned int > DeleteAllHighlightedElements();

signals:
  void HighlightLineage(unsigned int, bool);
  //void TraceVisibilityChanged(unsigned int, Qt::CheckState);
  void ShowLineage(unsigned int, bool);
  /** \brief When one track has been picked (highlighted) from the visualization */
  void TracePicked(unsigned int, Qt::CheckState);

  void ExportLineages();

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
  \brief Update highlighting property of one element given one actor.
  \param[in] iActor Actor of the element to be modified
  \return true if the element exists
  \return false else
  */
  void UpdateElementHighlighting(unsigned int iTraceID)
    {
    std::cout << "lineage picked: " << iTraceID << std::endl;
    Qt::CheckState state;
    Superclass::UpdateElementHighlightingWithTraceID( iTraceID, state );
    emit TracePicked(iTraceID, state);
    }

protected:
  std::vector< vtkActor* > AddTrace( vtkPolyData* , vtkProperty* );

private:
  Q_DISABLE_COPY(LineageContainer);
};

#endif // LineageCONTAINER_H
