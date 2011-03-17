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
        >/*,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag< Root >,
        BOOST_MULTI_INDEX_MEMBER(LineageStructure, bool, Root)
        >*/
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

  // define lineage specific boost iterators
  //typedef boost::MultiIndexLineageContainer::index<Root>::type::iterator
  //    MultiIndexContainerRootIterator;

  typedef Superclass::MultiIndexContainerType MultiIndexContainerType;
  typedef Superclass::MultiIndexContainerElementType LineageType;

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
  void InsertNewLineage(unsigned int iLineageID, 
    double irgba[4], unsigned int iTrackIDRoot,
    bool IsVisible = false);

  /*
   * \brief Get the list of all the lineages roots, as track IDs
   * \return list containg the track root ids
   */
  std::list<unsigned int> GetListOfTrackRootIDs();

  /*
   * \brief Get the track root ID of the given lineage
   * \param[in] iTraceID lineage ID of interest
   * \return related track root ID
   */
  unsigned int GetLineageTrackRootID( unsigned int iTraceID );

  /**\todo implement them:*/
  virtual bool DeleteElement(const unsigned int & iId);
  virtual bool DeleteElement(MultiIndexContainerTraceIDIterator iIter);
  std::list< unsigned int > DeleteAllHighlightedElements();

signals:
  void HighlightLineage( unsigned int, bool);
  void ShowLineage( unsigned int, bool);

public slots:
  /**
  \brief Change elements highlighting property given a list of TraceIDs and the
  new status.
  \param[in] iList list of TraceIDs
  \param[in] iCheck */
  void UpdateElementHighlightingWithGivenTraceIDs( const QStringList& iList,
                                                   const Qt::CheckState& iCheck );

  /**
  \brief Change elements visibility property given a list of TraceIDs and the
  new status.
  \param[in] iList list of TraceIDs
  \param[in] iCheck */
  void UpdateElementVisibilityWithGivenTraceIDs( const QStringList& iList,
                                                 const Qt::CheckState& iCheck );

protected:
  std::vector< vtkActor* > AddTrace( vtkPolyData* , vtkProperty* );

private:
  Q_DISABLE_COPY(LineageContainer);
};

#endif // LineageCONTAINER_H
