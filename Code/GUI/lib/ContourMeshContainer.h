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

#ifndef __ContourMeshContainer_h
#define __ContourMeshContainer_h

#include <QObject>

#include "ContourMeshStructure.h"

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

/**
  \class ContourMeshContainer
  \brief Wraps a boost multi index container of ContourMeshStructure.
  This class intends to synchronize Contour and Mesh representation in
  the Visualization and in the TableWidget
  \sa ContourMeshStructure QGoTableWidget QGoImageView3D
  */
class ContourMeshContainer:public QObject
{
  Q_OBJECT
public:

  typedef boost::multi_index::multi_index_container<
    ContourMeshStructure,
    boost::multi_index::indexed_by<
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag< TCoord >,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, unsigned int, TCoord)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< ActorXY >,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, vtkActor *, ActorXY)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< ActorXZ >,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, vtkActor *, ActorXZ)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< ActorYZ >,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, vtkActor *, ActorYZ)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< ActorXYZ >,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, vtkActor *, ActorXYZ)
        >,
      boost::multi_index::hashed_non_unique<
        boost::multi_index::tag< Nodes >,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, vtkPolyData *, Nodes)
        >,
      boost::multi_index::ordered_unique<
        boost::multi_index::tag< TraceID >,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, unsigned int, TraceID)
        >,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag< Highlighted >,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, bool, Highlighted)
        >,
      boost::multi_index::ordered_non_unique<
        boost::multi_index::tag< Visible >,
        BOOST_MULTI_INDEX_MEMBER(ContourMeshStructure, bool, Visible)
        >
      >
    > MultiIndexContainer;

  typedef MultiIndexContainer::index< TCoord >::type::iterator
  MultiIndexContainerTCoordIterator;

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
  explicit ContourMeshContainer(QObject *iParent,
                                QGoImageView3D *iView);

  /** \brief Destructor. */
  ~ContourMeshContainer();

  /** \brief Underlying container. */
  MultiIndexContainer m_Container;

  /** \brief Link to the visualization. */
  QGoImageView3D *m_ImageView;

  /** \brief Current Element to be inserted in the container */
  ContourMeshStructure m_CurrentElement;

  /** */
  void SetTimePoint(const unsigned int & iT);

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
    \param[in] iContour
  */
  template< class TContainer >
  void UpdateVisualizationForGivenIDs(TContainer iList,
                                      const bool & iContour)
  {
    typename TContainer::iterator it = iList.begin();

    while ( it != iList.end() )
      {
      MultiIndexContainerTraceIDIterator id_it =
        m_Container.get< TraceID >().find( static_cast< unsigned int >( *it ) );

      if ( id_it != m_Container.get< TraceID >().end() )
        {
        ContourMeshStructure temp(*id_it);
        temp.Highlighted = false;
        temp.Visible = false;

        vtkProperty *tproperty = vtkProperty::New();
        tproperty->SetColor(id_it->rgba[0], id_it->rgba[1], id_it->rgba[2]);
        tproperty->SetOpacity(id_it->rgba[3]);

        vtkPolyData *nodes = id_it->Nodes;
        if ( nodes )
          {
          temp.Visible =
            ( static_cast< unsigned int >( m_TCoord ) == id_it->TCoord );

          std::vector< vtkActor * > actor;

          if ( iContour )
            {
            int dir = ComputeDirectionFromContour(nodes);

            if ( dir != -1 )
              {
              m_ImageView->EnableContourWidget(true);
              m_ImageView->InitializeContourWidgetNodes(dir, nodes);

              vtkPolyData *trace = vtkPolyData::New();
              trace->ShallowCopy(
                m_ImageView->GetContourRepresentationAsPolydata(dir) );

              actor = this->m_ImageView->AddContour(trace, tproperty);

              m_ImageView->ReinitializeContourWidget();
              m_ImageView->EnableContourWidget(false);
              }
            }
          else
            {
            actor = this->m_ImageView->AddContour(nodes, tproperty);
            }

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
 *  \brief Change elements (in between iBegin and iEnd ) visibility
 *  \param[in] iBegin first element
 *  \param[in] iEnd last element
 *  \param[in] iVisibility
 */
  void
  ChangeActorsVisibility(
    MultiIndexContainerTCoordIterator iBegin,
    MultiIndexContainerTCoordIterator iEnd,
    const bool & iVisibility);

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
    ContourMeshStructure temp = *iIt;

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
  void Insert(const ContourMeshStructure & iE);

  /** \brief Insert Current Element in the container */
  void InsertCurrentElement();

  /** \brief Reset Current Element to a default state */
  void ResetCurrentElement();

  /** \brief Update Current Element by providing all required informations
  from the visualization.
  \param[in] iActors
  \param[in] iNodes
  \param[in] iT
  \param[in] iHighlighted
  \param[in] iVisible
  \see ContourMeshStructure
  */
  void UpdateCurrentElementFromVisu(std::vector< vtkActor * > iActors,
                                    vtkPolyData *iNodes,
                                    const unsigned int & iT,
                                    const bool & iHighlighted,
                                    const bool & iVisible);

  /** \brief Update Current Element from the database.
  \param[in] iTraceID
  \param[in] irgba
  */
  void UpdateCurrentElementFromDB(unsigned int iTraceID, double irgba[4]);

  /**
  \brief Remove all actors (elements) from the scene for a given time point
  \param[in] iT
  */
  void RemoveActorsWithGivenTimePoint(const unsigned int & iT);

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
  ComputeDirectionFromContour(vtkPolyData *iContour)
  {
    double bounds[6];

    iContour->GetBounds(bounds);

    return ComputeDirectionFromBounds< double >(bounds);
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
  ComputeDirectionFromBounds(T *iBounds)
  {
    int oDir = -1;

    for ( int i = 0; i < 3; i++ )
      {
      if ( iBounds[2 * i] == iBounds[2 * i + 1] )
        {
        oDir = 2 - i;
        }
      }

    return oDir;
  }

  template< typename T >
  static
  int
  ComputeDirectionFromBounds(const std::vector< T > & iBounds)
  {
    int oDir = -1;

    if ( iBounds.size() == 6 )
      {
      for ( int i = 0; i < 3; ++i )
        {
        if ( iBounds[2 * i] == iBounds[2 * i + 1] )
          {
          oDir = 2 - i;
          }
        }
      }

    return oDir;
  }

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

        it->SetActorProperties( temp_property );

        if ( it->Highlighted )
          {
          temp_property->Delete();
          }

        ContourMeshStructure tempStructure(*it);
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
      bool t_visible = false;

      if ( it != m_Container.get< TActor >().end() )
        {
        t_visible = !it->Visible;
        it->SetActorVisibility( t_visible );

        /*if ( it->ActorXY )
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
          }*/

        ContourMeshStructure tempStructure(*it);
        tempStructure.Visible = t_visible;

        Qt::CheckState State;

        // Note: it->Highlighted is the status before picking the actor
        if ( t_visible )
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
          it->SetActorVisibility( iState );
          /*if ( it->ActorXY )
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
            }*/

          ContourMeshStructure tempStructure(*it);
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

        ContourMeshStructure tempStructure(*it);
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
    \return the traceIDs with TCoord = iTimePoint
    \param[in] iTimePoint timepoint for which the traceIDs are needed
    */
  std::list< unsigned int > GetElementsTraceIDForGivenTimePoint(unsigned int iTimePoint);

  /**
    \brief Set property whenever the trace is highlighted
    \param[in] iProperty
  */
  void SetHighlightedProperty(vtkProperty *iProperty);

  /**
    \brief Get property for highlighted traces
  */
  vtkProperty * GetHighlightedProperty();


  void SetColorCode( const std::string& iColumnName,
                     const std::map< unsigned int, std::string >& iValues )
  {
    typedef typename std::map< unsigned int, std::string > MapType;
    typedef typename MapType::const_iterator MapConstIterator;

    std::map< std::string, double > stringmap;

    if( iColumnName.empty() || iValues.empty() )
      {
      RenderWithOriginalColors();
      return;
      }

    MapConstIterator it = iValues.begin();

    double temp = 0.;
    try
      {
      temp = boost::lexical_cast< double >( it->second );
      }
    catch( boost::bad_lexical_cast& )
      {
        if( stringmap.empty() )
          {
          stringmap[ it->second ] = 0.;
          }
        else
          {
          std::map< std::string, double >::iterator m_it = stringmap.find( it->second );

          if( m_it != stringmap.end() )
            {
            temp = m_it->second;
            }
          else
            {
            std::map< std::string, double >::reverse_iterator r_it = stringmap.rbegin();
            temp = r_it->second;
            temp += 1.0;
            }
          }
      }

    double min_value = temp;
    double max_value = temp;

    while( it != iValues.end() )
      {
      MultiIndexContainerTraceIDIterator
          trace_it = this->m_Container.get<TraceID>().find( it->first );

      if( trace_it != this->m_Container.get<TraceID>().end() )
        {
          if (trace_it->Nodes) //make sure the trace has points !!!
          {
          vtkPolyData* pd = trace_it->Nodes;

          // Here let's make sure you are not passing crazy values!
          try
            {
            temp = boost::lexical_cast< double >( it->second );
            }
          catch( boost::bad_lexical_cast& )
            {
              if( stringmap.empty() )
                {
                stringmap[ it->second ] = 0.;
                }
              else
                {
                std::map< std::string, double >::iterator m_it = stringmap.find( it->second );

                if( m_it != stringmap.end() )
                  {
                  temp = m_it->second;
                  }
                else
                  {
                  std::map< std::string, double >::reverse_iterator r_it = stringmap.rbegin();
                  temp = r_it->second;
                  temp += 1.0;
                  }
                }
            }

          if( temp > max_value )
            {
            max_value = temp;
            }
          if( temp < min_value )
            {
            min_value = temp;
            }

          vtkIdType NbOfPoints = pd->GetNumberOfPoints();
          vtkDoubleArray* data = vtkDoubleArray::New();
          data->SetNumberOfComponents( 1 );
          data->SetName( iColumnName.c_str() );

          for( vtkIdType i = 0; i < NbOfPoints; ++i )
            {
            data->InsertNextValue( temp );
            }

          pd->GetPointData()->SetScalars( data );
          pd->GetPointData()->SetActiveScalars( iColumnName.c_str() );
          }
        } //end make sure the trace has points !!!
      ++it;
      }

    // Let's set the scalar range (in order to get nice colors)
    typename MultiIndexContainer::iterator t_it = m_Container.begin();
    while( t_it != m_Container.end() )
      {
      if( t_it->ActorXY )
        {
        t_it->ActorXY->GetMapper()->SetScalarRange( min_value, max_value );
        t_it->ActorXY->GetMapper()->SetScalarVisibility( true );
        }
      if( t_it->ActorXZ )
        {
        t_it->ActorXZ->GetMapper()->SetScalarRange( min_value, max_value );
        t_it->ActorXY->GetMapper()->SetScalarVisibility( true );
        }
      if( t_it->ActorYZ )
        {
        t_it->ActorYZ->GetMapper()->SetScalarRange( min_value, max_value );
        t_it->ActorXY->GetMapper()->SetScalarVisibility( true );
        }
      if( t_it->ActorXYZ )
        {
        t_it->ActorXYZ->GetMapper()->SetScalarRange( min_value, max_value );
        t_it->ActorXY->GetMapper()->SetScalarVisibility( true );
        }
      ++t_it;
      }

    this->m_ImageView->UpdateRenderWindows();
  }

  /**
    \brief Color code contour / mesh according to values provided
    \tparam TValue numerical type that can be converted into double
    \param[in] iColumnName Name of data provided
    \param[in] ivalues is a map where the key is the TraceID and the Value is
    the actual data used to color.
    \note if iColumnName and/or iValues are empty traces will be then rendered
    with their original colors.
  */
  template< typename TValue >
  void SetColorCode( const std::string& iColumnName,
                     const std::map< unsigned int, TValue >& iValues )
    {
    typedef TValue ValueType;
    typedef typename std::map< unsigned int, ValueType > MapType;
    typedef typename MapType::const_iterator MapConstIterator;

    if( iColumnName.empty() || iValues.empty() )
      {
      RenderWithOriginalColors();
      return;
      }

    MapConstIterator it = iValues.begin();

    double temp = 0.;
    try
      {
      temp = boost::numeric_cast< double >( it->second );
      }
    catch( boost::numeric::bad_numeric_cast& e )
      {
      std::cout <<  e.what() <<std::endl;
      return;
      }

    double min_value = temp;
    double max_value = temp;

    while( it != iValues.end() )
      {
      MultiIndexContainerTraceIDIterator
          trace_it = this->m_Container.get<TraceID>().find( it->first );

      if( trace_it != this->m_Container.get<TraceID>().end() )
        {
          if (trace_it->Nodes) //make sure the trace has points !!!
          {
          vtkPolyData* pd = trace_it->Nodes;

          // Here let's make sure you are not passing crazy values!
          try
            {
            temp = boost::numeric_cast< double >( it->second );
            }
          catch( boost::numeric::bad_numeric_cast& e )
            {
            std::cout << e.what() <<std::endl;
            return;
            }

          if( temp > max_value )
            {
            max_value = temp;
            }
          if( temp < min_value )
            {
            min_value = temp;
            }

          vtkIdType NbOfPoints = pd->GetNumberOfPoints();
          vtkDoubleArray* data = vtkDoubleArray::New();
          data->SetNumberOfComponents( 1 );
          data->SetName( iColumnName.c_str() );

          for( vtkIdType i = 0; i < NbOfPoints; ++i )
            {
            data->InsertNextValue( temp );
            }

          pd->GetPointData()->SetScalars( data );
          pd->GetPointData()->SetActiveScalars( iColumnName.c_str() );
          }
        } //end make sure the trace has points !!!
      ++it;
      }

    // Let's set the scalar range (in order to get nice colors)
    typename MultiIndexContainer::iterator t_it = m_Container.begin();
    while( t_it != m_Container.end() )
      {
      if( t_it->ActorXY )
        {
        t_it->ActorXY->GetMapper()->SetScalarRange( min_value, max_value );
        t_it->ActorXY->GetMapper()->SetScalarVisibility( true );
        }
      if( t_it->ActorXZ )
        {
        t_it->ActorXZ->GetMapper()->SetScalarRange( min_value, max_value );
        t_it->ActorXY->GetMapper()->SetScalarVisibility( true );
        }
      if( t_it->ActorYZ )
        {
        t_it->ActorYZ->GetMapper()->SetScalarRange( min_value, max_value );
        t_it->ActorXY->GetMapper()->SetScalarVisibility( true );
        }
      if( t_it->ActorXYZ )
        {
        t_it->ActorXYZ->GetMapper()->SetScalarRange( min_value, max_value );
        t_it->ActorXY->GetMapper()->SetScalarVisibility( true );
        }
      ++t_it;
      }

    this->m_ImageView->UpdateRenderWindows();
    }

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

signals:
  /** \brief When one contour / mesh has been picked (highlighted) from the visualization */
  void TracePicked(unsigned int, Qt::CheckState);

  /** \brief When one contour / mesh's visibility has been changed from the visualization */
  void TraceVisibilityChanged(unsigned int, Qt::CheckState);

protected:
  unsigned int m_TCoord;
  vtkProperty *m_HighlightedProperty;

  void RenderWithOriginalColors()
    {
    typename MultiIndexContainer::iterator t_it = m_Container.begin();
    while( t_it != m_Container.end() )
      {
        if (t_it->Nodes) //make sure the trace has points !!!
        {
        t_it->Nodes->GetPointData()->SetActiveScalars( NULL );
        if( t_it->ActorXY )
          {
          t_it->ActorXY->GetMapper()->SetScalarVisibility( false );
          }
        if( t_it->ActorXZ )
          {
          t_it->ActorXY->GetMapper()->SetScalarVisibility( false );
          }
        if( t_it->ActorYZ )
          {
          t_it->ActorXY->GetMapper()->SetScalarVisibility( false );
          }
        if( t_it->ActorXYZ )
          {
          t_it->ActorXY->GetMapper()->SetScalarVisibility( false );
          }
        } //end make sure the trace has points !!!
      ++t_it;
      }
    }
private:
  Q_DISABLE_COPY(ContourMeshContainer);
};

#endif // CONTOURMESHCONTAINER_H
