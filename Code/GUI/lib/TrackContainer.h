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

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/ordered_index.hpp"

#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "QGoImageView3D.h"

/**
  \class TrackContainer
  \brief Wraps a boost multi index container of TrackStructure.
  This class intends to synchronize Track representation in
  the Visualization and in the TableWidget
  \sa TrackStructure QGoTableWidget QGoImageView3D
  */
class TrackContainer:public QObject
{
  Q_OBJECT
public:

  typedef std::map< int, double*> PointsMapType;

  typedef boost::multi_index::multi_index_container<
    TrackStructure,
    boost::multi_index::indexed_by<
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< ActorXY >,
        BOOST_MULTI_INDEX_MEMBER(TrackStructure, vtkActor *, ActorXY)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< ActorXZ >,
        BOOST_MULTI_INDEX_MEMBER(TrackStructure, vtkActor *, ActorXZ)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< ActorYZ >,
        BOOST_MULTI_INDEX_MEMBER(TrackStructure, vtkActor *, ActorYZ)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< ActorXYZ >,
        BOOST_MULTI_INDEX_MEMBER(TrackStructure, vtkActor *, ActorXYZ)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< Nodes >,
        BOOST_MULTI_INDEX_MEMBER(TrackStructure, vtkPolyData *, Nodes)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< PointsMap >,
        BOOST_MULTI_INDEX_MEMBER(TrackStructure, PointsMapType, PointsMap)
        >,
      boost::multi_index::ordered_unique<
        boost::multi_index::tag< TraceID >,
        BOOST_MULTI_INDEX_MEMBER(TrackStructure, unsigned int, TraceID)
        >,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag< Highlighted >,
        BOOST_MULTI_INDEX_MEMBER(TrackStructure, bool, Highlighted)
        >,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag< Visible >,
        BOOST_MULTI_INDEX_MEMBER(TrackStructure, bool, Visible)
        >
      >
    > MultiIndexContainer;

  typedef MultiIndexContainer::index< ActorXY >::type::iterator
  MultiIndexContainerActorXYIterator;

  typedef MultiIndexContainer::index< ActorXZ >::type::iterator
  MultiIndexContainerActorXZIterator;

  typedef MultiIndexContainer::index< ActorYZ >::type::iterator
  MultiIndexContainerActorYZIterator;

  typedef MultiIndexContainer::index< ActorXYZ >::type::iterator
  MultiIndexContainerActorXYZIterator;

  typedef MultiIndexContainer::index< Nodes >::type::iterator
  MultiIndexContainerNodesIterator;

  typedef MultiIndexContainer::index< TraceID >::type::iterator
  MultiIndexContainerTraceIDIterator;

  typedef MultiIndexContainer::index< Highlighted >::type::iterator
  MultiIndexContainerHighlightedIterator;

  typedef MultiIndexContainer::index< Visible >::type::iterator
  MultiIndexContainerVisibleIterator;

  //------------------------------------------------------------------------

  /** \brief Constructor. */
  explicit TrackContainer(QObject *iParent,
                                QGoImageView3D *iView);

  /** \brief Destructor. */
  ~TrackContainer();

  /** \brief Underlying container. */
  MultiIndexContainer m_Container;

  /** \brief Link to the visualization. */
  QGoImageView3D *m_ImageView;

  /** \brief Current Element to be inserted in the container */
  TrackStructure m_CurrentElement;

  // ----------------------------------------------------------------------

  /** \brief Print the container content in the application output */
  template< class TIterator >
  void Print(TIterator iBegin, TIterator iEnd)
  {
    TIterator it = iBegin;

    while ( it != iEnd )
      {
      std::cout << *it;
      std::cout << "***" << std::endl;
      std::cout << std::endl;
      ++it;
      }
  }

  /**
    \brief Print the container content in the application output according
    to the template parameter.
    \tparam TIndex
    */
  template< class TIndex >
  void Print()
  {
    this->Print( m_Container.get< TIndex >().begin(),
                 m_Container.get< TIndex >().end() );
  }

  /** \brief Print the container content in the application output. */
  void Print();
  // ----------------------------------------------------------------------

  /**
    \brief Update Visualization of the given TraceIDs
    \tparam TContainer Container of TraceIDs
    \param[in] iList input container of TraceIDs
  */
  template< class TContainer >
  void UpdateVisualizationForGivenIDs(TContainer iList)
  {
    typename TContainer::iterator it = iList.begin();

    while ( it != iList.end() )
      {
      MultiIndexContainerTraceIDIterator id_it =
        m_Container.get< TraceID >().find( static_cast< unsigned int >( *it ) );

      if ( id_it != m_Container.get< TraceID >().end() )
        {
        TrackStructure temp(*id_it);
        temp.Highlighted = false;
        temp.Visible = false;

        vtkProperty *tproperty = vtkProperty::New();
        tproperty->SetColor(id_it->rgba[0], id_it->rgba[1], id_it->rgba[2]);
        tproperty->SetOpacity(id_it->rgba[3]);

        vtkPolyData *nodes = id_it->Nodes;
        if ( nodes )
          {
          temp.Visible = id_it->Visible;

          std::vector< vtkActor * > actor;

          actor = this->m_ImageView->AddContour(nodes, tproperty);

          temp.ActorXY = actor[0];
          temp.ActorXZ = actor[1];
          temp.ActorYZ = actor[2];
          temp.ActorXYZ = actor[3];

          typedef void ( QGoImageView3D::*ImageViewMember )(const int &, vtkActor *);
          ImageViewMember f;

          if ( temp.Visible )
            {
            f = &QGoImageView3D::AddActor;
            }
          else
            {
            f = &QGoImageView3D::RemoveActor;
            }

          for ( int i = 0; i < 4; i++ )
            {
            ( m_ImageView->*f )(i, actor[i]);
            }
          }
        else
          {
          temp.Visible = false;
          }

        m_Container.get< TraceID >().replace(id_it, temp);
        }
      ++it;
      }
  }

  /** \brief Display all elements for a given time point
  *   \param[in] iT time point
  */
  void ShowActorsWithGivenTimePoint(const unsigned int & iT);

  /**
 * \brief Update Actors, Highlighted, Visibility (properties) of given
 * a element
 * \tparam TIndex Index Type (referring to multi index container's indices)
 * \param[in] iIt element to update
 * \param[in] iActors its actors
 * \param[in] iHighlighted
 * \param[in] iVisible if false remove the element from the scene, else
 * add it
 */
  template< class TIndex >
  void UpdateVisualizationForGivenElement(
    typename MultiIndexContainer::index< TIndex >::type::iterator iIt,
    std::vector< vtkActor * > iActors,
    const bool & iHighlighted,
    const bool & iVisible)
  {
    TrackStructure temp = *iIt;

    if ( iActors.size() == 4 )
      {
      temp.ActorXY = iActors[0];
      temp.ActorXZ = iActors[1];
      temp.ActorYZ = iActors[2];
      temp.ActorXYZ = iActors[3];
      }
    temp.Highlighted = iHighlighted;
    temp.Visible = iVisible;

    typedef void ( QGoImageView3D::*ImageViewMember )(const int &, vtkActor *);
    ImageViewMember f;

    if ( iVisible )
      {
      f = &QGoImageView3D::AddActor;
      }
    else
      {
      f = &QGoImageView3D::RemoveActor;
      }

    for ( int i = 0; i < 4; i++ )
      {
      ( m_ImageView->*f )(i, iActors[i]);
      }

    m_Container.get< TIndex >().replace(iIt, temp);
  }

  /**
  \brief Insert one element in the container
  \param[in] iE element to be insert in the container
  */
  void Insert(const TrackStructure & iE);

  /** \brief Insert Current Element in the container */
  void InsertCurrentElement();

  /** \brief Reset Current Element to a default state */
  void ResetCurrentElement();

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

  /** 
  \brief Update Current Element from the database.
  \param[in] iTraceID
  \param[in] irgba
  */
  void UpdateCurrentElementFromDB(unsigned int iTraceID, double irgba[4]);

  /**
  \brief put the information of the existing element into m_CurrentElement
  and remove the existing element from the container,the visu and the memory
  \param[in] iTraceID ID of the existing element
  \return true if the element was found in the container, false if not
  */
  bool UpdateCurrentElementFromExistingOne(unsigned int iTraceID);

  /**
  \brief Remove all actors (elements) from the scene for a given time point
  \param[in] iT
  */
  void RemoveActorsWithGivenTimePoint(const unsigned int & iT);

  /**
  \brief Remove all actors of the current element. Useful to update a track.
  */
  void RemoveCurrentActorsFromVisu();

  /**
    \brief Add all actors (elements) from the scene for a given time point
  */
  void AddActorsWithGivenTimePoint(const unsigned int & iT);

  /**
    \brief Remove element from visualization
    \param[in] iId TraceID of the element to be removed
    \return true if the element was present in the container.
  */
  bool
  RemoveElementFromVisualizationWithGivenTraceID(
    const unsigned int & iId);

  /**
    \brief Update element highlighting given it TraceId
    \param[in] iId TraceID of the element to be modified
    \return true if the element was present in the container.
  */
  bool UpdateElementHighlightingWithGivenTraceID(const unsigned int & iId);

  /**
  \brief Update highlighting property of one element given one actor.
  \param[in] iActor Actor of the element to be modified
  \return true if the element exists
  \return false else
  */
  template< class TActor >
  bool UpdateElementHighlightingWithGivenActor(vtkActor *iActor)
  {
    if ( iActor )
      {
      typedef typename MultiIndexContainer::index< TActor >::type::iterator
      IteratorType;
      IteratorType it = m_Container.get< TActor >().find(iActor);

      vtkProperty *temp_property = NULL;

      if ( it != m_Container.get< TActor >().end() )
        {
        if ( it->Highlighted )
          {
          temp_property = vtkProperty::New();
          temp_property->SetColor(it->rgba[0],
                                  it->rgba[1],
                                  it->rgba[2]);
          temp_property->SetOpacity(it->rgba[3]);
          temp_property->SetLineWidth(1.);
          }
        else
          {
          temp_property = this->m_HighlightedProperty;
          }

        if ( it->ActorXY )
          {
          it->ActorXY->SetProperty(temp_property);
          }
        if ( it->ActorXZ )
          {
          it->ActorXZ->SetProperty(temp_property);
          }
        if ( it->ActorYZ )
          {
          it->ActorYZ->SetProperty(temp_property);
          }
        if ( it->ActorXYZ )
          {
          it->ActorXYZ->SetProperty(temp_property);
          }

        if ( it->Highlighted )
          {
          temp_property->Delete();
          }

        TrackStructure tempStructure(*it);
        tempStructure.Highlighted = !it->Highlighted;

        Qt::CheckState State;

        // Note: it->Highlighted is the status before picking the actor
        if ( !it->Highlighted )
          {
          State = Qt::Checked;
          }
        else
          {
          State = Qt::Unchecked;
          }

        m_Container.get< TActor >().replace(it, tempStructure);
        m_ImageView->UpdateRenderWindows();

        emit TracePicked(it->TraceID, State);

        return true;
        }
      }

    return false;
  }

  /** \brief Update element Visibility property given one actor.
  \tparam TActor either ActorXY, ActorXZ, ActorYZ, ActorXYZ depending on the view
  \param[in] iActor provided actor
  \return true if iActor is in the container
  \return false else
  */
  template< class TActor >
  bool UpdateElementVisibilityWithGivenActor(vtkActor *iActor)
  {
    if ( iActor )
      {
      typedef typename MultiIndexContainer::index< TActor >::type::iterator
      IteratorType;
      IteratorType it = m_Container.get< TActor >().find(iActor);

      vtkProperty *temp_property = NULL;

      if ( it != m_Container.get< TActor >().end() )
        {
        if ( it->ActorXY )
          {
          it->ActorXY->SetVisibility(!it->Visible);
          }
        if ( it->ActorXZ )
          {
          it->ActorXZ->SetVisibility(!it->Visible);
          }
        if ( it->ActorYZ )
          {
          it->ActorYZ->SetVisibility(!it->Visible);
          }
        if ( it->ActorXYZ )
          {
          it->ActorXYZ->SetVisibility(!it->Visible);
          }

        TrackStructure tempStructure(*it);
        tempStructure.Visible = !it->Visible;

        Qt::CheckState State;

        // Note: it->Highlighted is the status before picking the actor
        if ( !it->Visible )
          {
          State = Qt::Checked;
          }
        else
          {
          State = Qt::Unchecked;
          }

        m_Container.get< TActor >().replace(it, tempStructure);
        m_ImageView->UpdateRenderWindows();

        emit TraceVisibilityChanged(it->TraceID, State);

        return true;
        }
      }

    return false;
  }

  /**
  \brief Update highlighting property of one element given one actor.
  \param[in] iActor Actor of the element to be modified
  \param[in] iState Visibility to applied to the element
  \return true if the element exists
  \return false else
  */
  template< class TActor >
  bool UpdateElementVisibilityWithGivenActor(vtkActor *iActor, bool iState)
  {
    if ( iActor )
      {
      typedef typename MultiIndexContainer::index< TActor >::type::iterator
      IteratorType;
      IteratorType it = m_Container.get< TActor >().find(iActor);

      if ( it != m_Container.get< TActor >().end() )
        {
        if ( it->Visible != iState )
          {
          if ( it->ActorXY )
            {
            it->ActorXY->SetVisibility(iState);
            }
          if ( it->ActorXZ )
            {
            it->ActorXZ->SetVisibility(iState);
            }
          if ( it->ActorYZ )
            {
            it->ActorYZ->SetVisibility(iState);
            }
          if ( it->ActorXYZ )
            {
            it->ActorXYZ->SetVisibility(iState);
            }

          TrackStructure tempStructure(*it);
          tempStructure.Visible = iState;

          Qt::CheckState State;

          // Note: it->Highlighted is the status before picking the actor
          if ( iState )
            {
            State = Qt::Checked;
            }
          else
            {
            State = Qt::Unchecked;
            }

          m_Container.get< TActor >().replace(it, tempStructure);
          //m_ImageView->UpdateRenderWindows();

          emit TraceVisibilityChanged(it->TraceID, State);
          }

        return true;
        }
      }

    return false;
  }

  //-------------------------------------------------------------------------
  /**
  \brief Change element visibility in the scene
  \tparam TIndex refers to any index from the multi index container indices
  \param[in] iBegin first element
  \param[in] iEnd last element
  \param[in] iVisibility
  */
  template< class TIndex >
  void ChangeActorsVisibility(
    typename MultiIndexContainer::index< TIndex >::type::iterator iBegin,
    typename MultiIndexContainer::index< TIndex >::type::iterator iEnd,
    const bool & iVisibility)
  {
    typename MultiIndexContainer::index< TIndex >::type::iterator it = iBegin;

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

    while ( it != iEnd )
      {
      if ( it->Visible != iVisibility )
        {
        if ( it->ActorXY )
          {
          it->ActorXY->SetVisibility(iVisibility);
          ( m_ImageView->*f )(0, it->ActorXY);
          }
        if ( it->ActorXZ )
          {
          it->ActorXZ->SetVisibility(iVisibility);
          ( m_ImageView->*f )(1, it->ActorXZ);
          }
        if ( it->ActorYZ )
          {
          it->ActorYZ->SetVisibility(iVisibility);
          ( m_ImageView->*f )(2, it->ActorYZ);
          }
        if ( it->ActorXYZ )
          {
          it->ActorXYZ->SetVisibility(iVisibility);
          ( m_ImageView->*f )(3, it->ActorXYZ);
          }

        TrackStructure tempStructure(*it);
        tempStructure.Visible = iVisibility;

        m_Container.get< TIndex >().replace(it, tempStructure);
        }
      ++it;
      }
  }

  //-------------------------------------------------------------------------

  /**
    \brief Update element visibility given it TraceId
    \param[in] iId TraceID of the element to be modified
    \return true if the element was present in the container.
  */
  bool UpdateElementVisibilityWithGivenTraceID(const unsigned int & iId);

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
    \brief Update all highlighted elements in the container with a given color.
    \note Elements remain highlighted as long as it is checked in the Table Widget.
    \param[in] iColor
    \return list of highlighted elements
  */
  std::list< unsigned int > UpdateAllHighlightedElementsWithGivenColor(
    QColor iColor);

  /**
    \brief Get the list of highlighted elements TraceID.
    */
  std::list< unsigned int > GetHighlightedElementsTraceID();

  /**
    \brief Set property whenever the trace is highlighted
    \param[in] iProperty
  */
  void SetHighlightedProperty(vtkProperty *iProperty);

  /**
    \brief Get property for highlighted traces
  */
  vtkProperty * GetHighlightedProperty();

  /**
    \brief Add a point in the current track.
    If there is already a point existing at the same time point, we don't do anything.
    We could add a variable to either override or not the point who already
    exists at a current time point.
    \param[in] iPoint new point to be added in the track
  */
  void AddPointToCurrentElement(double* iPoint, int iTime);

  /**
    \brief Get the polydata representing the current element track
    \return Pointer to the current element track
  */
  vtkPolyData* GetCurrentElementNodes();

  /**
    \brief Update the current element actors from the visualization.
    \param[in] iActors Vector container an actor pointer for each view
  */
  void UpdateCurrentElementActorsFromVisu(std::vector< vtkActor * > iActors);

public slots:

  void UpdateElementHighlightingWithGivenTraceIDs( const QStringList& iList,
                                                   const Qt::CheckState& iCheck );
  void UpdateElementVisibilityWithGivenTraceIDs( const QStringList& iList,
                                                 const Qt::CheckState& iCheck );

signals:
  /** \brief When one track has been picked (highlighted) from the visualization */
  void TracePicked(unsigned int, Qt::CheckState);

  /** \brief When one track's visibility has been changed from the visualization */
  void TraceVisibilityChanged(unsigned int, Qt::CheckState);

  /** \brief When a point is added to the track, update the database */
  void CurrentTrackToSave();

protected:
  vtkProperty *m_HighlightedProperty;
private:
  Q_DISABLE_COPY(TrackContainer);
};

#endif // TrackCONTAINER_H
