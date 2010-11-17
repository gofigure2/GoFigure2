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

#ifndef TRACECONTAINERBASE_H
#define TRACECONTAINERBASE_H

#include "QGoImageView3D.h"
#include "vtkProperty.h"
#include "StructureHelper.h"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/ordered_index.hpp"

template< class TContainer >
class TraceContainerBase : public QObject
{
public:
  typedef TContainer MultiIndexContainerType;
  typedef typename MultiIndexContainerType::value_type MultiIndexContainerElementType;

  typedef typename MultiIndexContainerType::template index< ActorXY >::type::iterator
  MultiIndexContainerActorXYIterator;

  typedef typename MultiIndexContainerType::template index< ActorXZ >::type::iterator
  MultiIndexContainerActorXZIterator;

  typedef typename MultiIndexContainerType::template index< ActorYZ >::type::iterator
  MultiIndexContainerActorYZIterator;

  typedef typename MultiIndexContainerType::template index< ActorXYZ >::type::iterator
  MultiIndexContainerActorXYZIterator;

  typedef typename MultiIndexContainerType::template index< Nodes >::type::iterator
  MultiIndexContainerNodesIterator;

  typedef typename MultiIndexContainerType::template index< TraceID >::type::iterator
  MultiIndexContainerTraceIDIterator;

  typedef typename MultiIndexContainerType::template index< Highlighted >::type::iterator
  MultiIndexContainerHighlightedIterator;

  typedef typename MultiIndexContainerType::template index< Visible >::type::iterator
  MultiIndexContainerVisibleIterator;

  explicit TraceContainerBase( QObject* iParent, QGoImageView3D* iView );
  virtual ~TraceContainerBase();


  MultiIndexContainerType m_Container;

  /** \brief Link to the visualization. */
  QGoImageView3D *m_ImageView;

  MultiIndexContainerElementType m_CurrentElement;

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
    using boost::multi_index::get;

    this->Print(  this->m_Container.get< TIndex >().begin(),
                  this->m_Container.get< TIndex >().end() );
  }

  /** \brief Print the container content in the application output. */
  void Print();

  // ----------------------------------------------------------------------

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
    \brief Update Visualization of the given TraceIDs
    \tparam TContainer Container of TraceIDs
    \param[in] iList input container of TraceIDs
  */
  template< class TList >
  void UpdateVisualizationForGivenIDs(TList iList)
  {
    using boost::multi_index::get;

    typename TList::iterator it = iList.begin();

    while ( it != iList.end() )
      {
      MultiIndexContainerTraceIDIterator id_it =
        m_Container.get< TraceID >().find( static_cast< unsigned int >( *it ) );

      if ( id_it != m_Container.get< TraceID >().end() )
        {
        MultiIndexContainerElementType temp(*id_it);
        temp.Highlighted = false;
        temp.Visible = false;

        vtkProperty *tproperty = vtkProperty::New();
        tproperty->SetColor(id_it->rgba[0], id_it->rgba[1], id_it->rgba[2]);
        tproperty->SetOpacity(id_it->rgba[3]);

        vtkPolyData *nodes = id_it->Nodes;
        if ( nodes )
          {
          temp.Visible = id_it->Visible;

          std::vector< vtkActor * > actor =
              this->m_ImageView->AddContour(nodes, tproperty);

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
    typename MultiIndexContainerType::template index< TIndex >::type::iterator iIt,
    std::vector< vtkActor * > iActors,
    const bool & iHighlighted,
    const bool & iVisible)
  {
    MultiIndexContainerElementType temp = *iIt;

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

    using boost::multi_index::get;
    m_Container.get< TIndex >().replace(iIt, temp);
  }
  /**
  \brief Insert one element in the container
  \param[in] iE element to be insert in the container
  */
  void Insert(const MultiIndexContainerElementType & iE);

  /** \brief Insert Current Element in the container */
  void InsertCurrentElement();

  /** \brief Reset Current Element to a default state */
  void ResetCurrentElement();

  /**
  \brief Update Current Element from te database.
  \param[in] iTraceID
  \param[in] irgba
  */
  void UpdateCurrentElementFromDB(unsigned int iTraceID, double irgba[4]);

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
    typename MultiIndexContainerType::template index< TIndex >::type::iterator iBegin,
    typename MultiIndexContainerType::template index< TIndex >::type::iterator iEnd,
    const bool & iVisibility)
  {
    typename MultiIndexContainerType::template index< TIndex >::type::iterator
        it = iBegin;

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
        it->SetActorVisibility( iVisibility );

        if ( it->ActorXY )
          {
          ( m_ImageView->*f )(0, it->ActorXY);
          }
        if ( it->ActorXZ )
          {
          ( m_ImageView->*f )(1, it->ActorXZ);
          }
        if ( it->ActorYZ )
          {
          ( m_ImageView->*f )(2, it->ActorYZ);
          }
        if ( it->ActorXYZ )
          {
          ( m_ImageView->*f )(3, it->ActorXYZ);
          }

        MultiIndexContainerElementType tempStructure(*it);
        tempStructure.Visible = iVisibility;

        using boost::multi_index::get;
        m_Container.get< TIndex >().replace(it, tempStructure);
        }
      ++it;
      }
  }

  /**
    \brief Update all highlighted elements in the container with a given color.
    \note Elements remain highlighted as long as it is checked in the Table Widget.
    \param[in] iColor
    \return list of highlighted elements
  */
  std::list< unsigned int > UpdateAllHighlightedElementsWithGivenColor(
    QColor iColor);

public slots:

  /** \brief Change elements highlighting property given a list of TraceIDs
  and the new status.
    \param[in] iList list of TraceIDs
    \param[in] iCheck */
  virtual void UpdateElementHighlightingWithGivenTraceIDs( const QStringList& iList,
                                                   const Qt::CheckState& iCheck );

  /** \brief Change elements visibility property given a list of TraceIDs
  and the new status.
    \param[in] iList list of TraceIDs
    \param[in] iCheck */
  virtual void UpdateElementVisibilityWithGivenTraceIDs( const QStringList& iList,
                                                 const Qt::CheckState& iCheck );

protected:
  vtkProperty *m_HighlightedProperty;
};

#include "TraceContainerBase.txx"

#endif // TRACECONTAINERBASE_H
