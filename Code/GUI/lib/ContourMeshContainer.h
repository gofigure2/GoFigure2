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

#include <QObject>

#include "ContourMeshStructure.h"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/ordered_index.hpp"

#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "QGoImageView3D.h"

/**
  \class ContourMeshContainer
  \brief
  */
class ContourMeshContainer : public QObject
{
  Q_OBJECT

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
          >,
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<Highlighted>,
            BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, bool, Highlighted)
        >,
        boost::multi_index::ordered_non_unique<
            boost::multi_index::tag<Visible>,
            BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, bool, Visible)
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

  typedef MultiIndexContainer::index<Highlighted>::type::iterator
        MultiIndexContainerHighlightedIterator;

  typedef MultiIndexContainer::index<Visible>::type::iterator
        MultiIndexContainerVisibleIterator;

  //-------------------------------------------------------------------------

  /** \brief Constructor. */
  explicit ContourMeshContainer( QObject* iParent,
                                 QGoImageView3D* iView );

  /** \brief Destructor. */
  ~ContourMeshContainer( );

  /** \brief Underlying container. */
  MultiIndexContainer m_Container;

  /** \brief Link to the visualization. */
  QGoImageView3D* m_ImageView;

  ContourMeshStructure m_CurrentElement;

  /**
    \brief Print the container content in the application output
    */
  template< class TIterator >
  void Print( TIterator iBegin, TIterator iEnd )
  {
    TIterator it = iBegin;
    while( it != iEnd )
      {
      std::cout << *it;
      std::cout << "***" <<std::endl;
      std::cout << std::endl;
      ++it;
      }
  }

  /**
    \brief Print the container content in the application output according to
    the template parameter.
    \tparam TIndex
    */
  template< class TIndex >
  void Print()
  {
    this->Print( m_Container.get< TIndex >().begin(),
                 m_Container.get< TIndex >().end() );
  }

  /**
    \brief Print the container content in the application output.
    */
  void Print();

  template< class TContainer >
  void UpdateVisualizationForGivenIDs( TContainer iList,
                                       const bool& iContour )
  {
    typename TContainer::iterator it = iList.begin();

    while( it != iList.end() )
      {
      MultiIndexContainerTraceIDIterator id_it =
          m_Container.get< TraceID >().find( static_cast< unsigned int >( *it ) );

      if( id_it != m_Container.get< TraceID >().end() )
        {
        ContourMeshStructure temp( *id_it );
        temp.Highlighted = false;
        temp.Visible =
            ( static_cast< unsigned int >( m_TCoord ) == id_it->TCoord );

        vtkProperty* property = vtkProperty::New();
        property->SetColor( id_it->rgba[0], id_it->rgba[1], id_it->rgba[2] );
        property->SetOpacity( id_it->rgba[3] );

        vtkPolyData* nodes = id_it->Nodes;
        std::vector<vtkActor*> actor;

        if( iContour )
          {
          int dir = ComputeDirectionFromContour(nodes);

          if( dir != -1 )
            {
            m_ImageView->EnableContourWidget( true );
            m_ImageView->InitializeContourWidgetNodes( dir, nodes );

            vtkPolyData* trace =
                m_ImageView->GetContourRepresentationAsPolydata(dir);

            actor = this->m_ImageView->AddContour(trace, property);

            m_ImageView->ReinitializeContourWidget();
            m_ImageView->EnableContourWidget( false );
            }
          }
        else
          {
          actor = this->m_ImageView->AddContour( nodes, property );
          }

        temp.ActorXY = actor[0];
        temp.ActorXZ = actor[1];
        temp.ActorYZ = actor[2];
        temp.ActorXYZ = actor[3];

        m_Container.get< TraceID >().replace( id_it, temp );
        }
      ++it;
      }
  }

  void ShowActorsWithGivenTimePoint( const unsigned int& iT );

  void
    ChangeActorsVisibility(
      MultiIndexContainerTCoordIterator iBegin,
      MultiIndexContainerTCoordIterator iEnd,
      const bool& iVisibility );

  template< class TIndex >
  void UpdateVisualizationForGivenElement(
      typename MultiIndexContainer::index<TIndex>::type::iterator iIt,
      std::vector<vtkActor*> iActors,
      const bool & iHighlighted,
      const bool & iVisible )
  {
    ContourMeshStructure temp = *iIt;

    if( iActors.size() == 4 )
      {
      temp.ActorXY = iActors[0];
      temp.ActorXZ = iActors[1];
      temp.ActorYZ = iActors[2];
      temp.ActorXYZ = iActors[3];
      }
    temp.Highlighted = iHighlighted;
    temp.Visible = iVisible;

    if( iVisible )
      {
      this->m_ImageView->AddActor( 0, iActors[0] );
      this->m_ImageView->AddActor( 1, iActors[1] );
      this->m_ImageView->AddActor( 2, iActors[2] );
      this->m_ImageView->AddActor( 3, iActors[3] );
      }
    else
      {
      this->m_ImageView->RemoveActor( 0, iActors[0] );
      this->m_ImageView->RemoveActor( 1, iActors[1] );
      this->m_ImageView->RemoveActor( 2, iActors[2] );
      this->m_ImageView->RemoveActor( 3, iActors[3] );
      }

    m_Container.get< TIndex >().replace( iIt, temp );
  }

  /**
  \brief Insert one element in the container
  \param[in] iE element to be insert in the container
  */
  void Insert( const ContourMeshStructure& iE );
  void InsertCurrentElement();

  void ResetCurrentElement();

  void UpdateCurrentElementFromVisu(std::vector<vtkActor*> iActors,
                                    vtkPolyData* iNodes,
                                    const unsigned int & iT,
                                    const bool & iHighlighted,
                                    const bool & iVisible);

  void UpdateCurrentElementFromDB(unsigned int iTraceID, double irgba[4]);

  /**
  \brief Remove all actors for a given time point
  \param[in] iT
  */
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
  bool
      RemoveElementFromVisualizationWithGivenTraceID(
        const unsigned int& iId );

  /**
    \brief Update element highlighting given it TraceId
    \param[in] iId TraceID of the element to be modified
    \param[in] iProperty highlighting property
    \return true if the element was present in the container.
  */
  bool UpdateElementHighlightingWithGivenTraceID( const unsigned int& iId );

  /**
  \brief Returns the direction of a given contour vtkPolyData.
  This static method is supposed to be used when dealing with contours.
  \param[in] iContour
  \return 0 if z coordinates are constant
  \return 1 if y coordinates are constant
  \return 2 if x coordinates are constant
  \return -1 else
  */
  static
  int
  ComputeDirectionFromContour(vtkPolyData* iContour)
  {
    double bounds[6];
    iContour->GetBounds( bounds );

    return ComputeDirectionFromBounds<double>( bounds );
  }

  /**
  \brief Returns the direction of a given element given its bounding box.
  This static method is supposed to be used when dealing with contours.
  \return 0 if z coordinates are constant
  \return 1 if y coordinates are constant
  \return 2 if x coordinates are constant
  \return -1 else
  */
  template< typename T >
  static
  int
  ComputeDirectionFromBounds( T* iBounds )
  {
    int oDir = -1;

    for (int i = 0; i < 3; i++)
      {
      if (iBounds[2 * i] == iBounds[2 * i + 1])
        {
        oDir = 2 - i;
        }
      }

    return oDir;
  }

  /**
  \brief Update highlighting property of one element given one actor.
  \param[in] iActor Actor of the element to be modified
  \param[in] iProperty Property to be applied on all actors of the element
  \return true if the element exists
  \return false else
  */
  template< class TActor >
  bool UpdateElementHighlightingWithGivenActor( vtkActor* iActor )
  {
    if( iActor )
      {
      typedef typename MultiIndexContainer::index<TActor>::type::iterator
        IteratorType;
      IteratorType it = m_Container.get<TActor>().find(iActor);

      vtkProperty* temp_property = NULL;

      if( it != m_Container.get<TActor>().end() )
        {
        if( it->Highlighted )
          {
          temp_property = vtkProperty::New();
          temp_property->SetColor( it->rgba[0],
                                  it->rgba[1],
                                  it->rgba[2]);
          temp_property->SetOpacity( it->rgba[3] );
          temp_property->SetLineWidth(1.);
          }
        else
          {
          temp_property = this->m_HighlightedProperty;
          }

        if( it->ActorXY )
          {
          it->ActorXY->SetProperty( temp_property );
          }
        if( it->ActorXZ )
          {
          it->ActorXZ->SetProperty( temp_property );
          }
        if( it->ActorYZ )
          {
          it->ActorYZ->SetProperty( temp_property );
          }
        if( it->ActorXYZ )
          {
          it->ActorXYZ->SetProperty( temp_property );
          }

        if( it->Highlighted )
          {
          temp_property->Delete();
          }

        ContourMeshStructure tempStructure( *it );
        tempStructure.Highlighted = !it->Highlighted;

        Qt::CheckState State;

        // Note: it->Highlighted is the status before picking the actor
        if (!it->Highlighted)
          {
          State = Qt::Checked;
          }
        else
          {
          State = Qt::Unchecked;
          }

        m_Container.get<TActor>().replace( it, tempStructure );
        m_ImageView->UpdateRenderWindows();

        emit TracePicked( it->TraceID, State );

        return true;
        }
      }

    return false;
  }

  template< class TActor >
  bool UpdateElementVisibilityWithGivenActor( vtkActor* iActor )
  {
    if( iActor )
      {
      typedef typename MultiIndexContainer::index<TActor>::type::iterator
        IteratorType;
      IteratorType it = m_Container.get<TActor>().find(iActor);

      vtkProperty* temp_property = NULL;

      if( it != m_Container.get<TActor>().end() )
        {
        if( it->ActorXY )
          {
          it->ActorXY->SetVisibility( !it->Visible );
          }
        if( it->ActorXZ )
          {
          it->ActorXZ->SetVisibility( !it->Visible );
          }
        if( it->ActorYZ )
          {
          it->ActorYZ->SetVisibility( !it->Visible );
          }
        if( it->ActorXYZ )
          {
          it->ActorXYZ->SetVisibility( !it->Visible );
          }

        ContourMeshStructure tempStructure( *it );
        tempStructure.Visible = !it->Visible;

        Qt::CheckState State;

        // Note: it->Highlighted is the status before picking the actor
        if (!it->Visible)
          {
          State = Qt::Checked;
          }
        else
          {
          State = Qt::Unchecked;
          }

        m_Container.get<TActor>().replace( it, tempStructure );
        m_ImageView->UpdateRenderWindows();

        emit TraceVisibilityChanged( it->TraceID, State );

        return true;
        }
      }

    return false;
  }

  /**
  \brief Update highlighting property of one element given one actor.
  \param[in] iActor Actor of the element to be modified
  \param[in] iProperty Property to be applied on all actors of the element
  \return true if the element exists
  \return false else
  */
  template< class TActor >
  bool UpdateElementVisibilityWithGivenActor( vtkActor* iActor, bool iState )
  {
    if( iActor )
      {
      typedef typename MultiIndexContainer::index<TActor>::type::iterator
        IteratorType;
      IteratorType it = m_Container.get<TActor>().find(iActor);

      if( it != m_Container.get<TActor>().end() )
        {
        if( it->Visible != iState )
          {
          if( it->ActorXY )
            {
            it->ActorXY->SetVisibility( iState );
            }
          if( it->ActorXZ )
            {
            it->ActorXZ->SetVisibility( iState );
            }
          if( it->ActorYZ )
            {
            it->ActorYZ->SetVisibility( iState );
            }
          if( it->ActorXYZ )
            {
            it->ActorXYZ->SetVisibility( iState );
            }

          ContourMeshStructure tempStructure( *it );
          tempStructure.Visible = iState;

          Qt::CheckState State;

          // Note: it->Highlighted is the status before picking the actor
          if (iState)
            {
            State = Qt::Checked;
            }
          else
            {
            State = Qt::Unchecked;
            }

          m_Container.get<TActor>().replace( it, tempStructure );
          //m_ImageView->UpdateRenderWindows();

          emit TraceVisibilityChanged( it->TraceID, State );
          }

        return true;
        }
      }

    return false;
  }

  //-------------------------------------------------------------------------
  template< class TIndex >
  void ChangeActorsVisibility(
      typename MultiIndexContainer::index<TIndex>::type::iterator iBegin,
      typename MultiIndexContainer::index<TIndex>::type::iterator iEnd,
      const bool& iVisibility )
  {
    typename MultiIndexContainer::index<TIndex>::type::iterator it = iBegin;

    typedef void (QGoImageView3D::*ImageViewMember)(const int&, vtkActor*);
    ImageViewMember f;

    if( iVisibility )
      {
      f = & QGoImageView3D::AddActor;
      }
    else
      {
      f = & QGoImageView3D::RemoveActor;
      }

    while( it != iEnd )
      {
      if( it->Visible != iVisibility )
        {
        if( it->ActorXY )
          {
          it->ActorXY->SetVisibility( iVisibility );
          (m_ImageView->*f)( 0, it->ActorXY );
          }
        if( it->ActorXZ )
          {
          it->ActorXZ->SetVisibility( iVisibility );
          (m_ImageView->*f)( 1, it->ActorXZ );
          }
        if( it->ActorYZ )
          {
          it->ActorYZ->SetVisibility( iVisibility );
          (m_ImageView->*f)( 2, it->ActorYZ );
          }
        if( it->ActorXYZ )
          {
          it->ActorXYZ->SetVisibility( iVisibility );
          (m_ImageView->*f)( 3, it->ActorXYZ );
          }

        ContourMeshStructure tempStructure( *it );
        tempStructure.Visible = iVisibility;

        m_Container.get<TIndex>().replace(it, tempStructure);
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

  std::list< unsigned int > DeleteAllHighlightedElements();

  /**
    \brief Update all highlighted elements in the container with a given color.
    \note Elements remain highlighted as long as it is checked in the Table Widget.
    \param[in] iColor
    \return list of highlighted elements
  */
  std::list< unsigned int > UpdateAllHighlightedElementsWithGivenColor(
      QColor iColor );

  /**
    \brief Get the list of highlighted elements TraceID.
    */
  std::list< unsigned int > GetHighlightedElementsTraceID();

  /**
    \brief Set property whenever the trace is highlighted
    \param[in] iProperty
  */
  void SetHighlightedProperty( vtkProperty* iProperty );

  /**
    \brief Get property for highlighted traces
  */
  vtkProperty* GetHighlightedProperty();

signals:
  void TracePicked( unsigned int, Qt::CheckState );
  void TraceVisibilityChanged( unsigned int, Qt::CheckState );

protected:
  unsigned int m_TCoord;
  vtkProperty* m_HighlightedProperty;

private:
  Q_DISABLE_COPY( ContourMeshContainer );
};


#endif // CONTOURMESHCONTAINER_H
