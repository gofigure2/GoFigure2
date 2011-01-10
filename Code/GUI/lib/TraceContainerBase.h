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

#ifndef TRACECONTAINERBASE_H
#define TRACECONTAINERBASE_H

#include <Qt>
#include <QObject>
#include "QGoImageView3D.h"

#include "vtkProperty.h"
#include "StructureHelper.h"

#include "boost/multi_index_container.hpp"
#include "boost/multi_index/member.hpp"
#include "boost/multi_index/hashed_index.hpp"
#include "boost/multi_index/ordered_index.hpp"
#include "boost/numeric/conversion/cast.hpp"
#include "boost/lexical_cast.hpp"

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
        tproperty->SetLineWidth( this->m_IntersectionLineWidth );

        vtkPolyData *nodes = id_it->Nodes;
        if ( nodes )
          {
          temp.Visible = id_it->Visible;

          std::vector< vtkActor * > actor =
              this->AddTrace( nodes, tproperty );

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

          if( m_ImageView )
            {
            for ( int i = 0; i < 4; i++ )
              {
              ( m_ImageView->*f )(i, actor[i]);
              }
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

    if( m_ImageView )
      {
      for ( int i = 0; i < 4; i++ )
        {
        ( m_ImageView->*f )(i, iActors[i]);
        }
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
  \param[in] IsVisible
  */
  void UpdateCurrentElementFromDB(unsigned int iTraceID, double irgba[4],
    bool IsVisible = false);

  /**
    \brief Get the polydata representing the current element track
    \return Pointer to the current element track
  */
  vtkPolyData* GetCurrentElementNodes();

  /**
  \brief put the information of the existing element into m_CurrentElement
  and remove the existing element from the container,the visu and the memory
  \param[in] iTraceID ID of the existing element
  \return true if the element was found in the container, false if not
  */
  bool UpdateCurrentElementFromExistingOne(unsigned int iTraceID);

  /**
    \brief Update element visibility given it TraceId
    \param[in] iId TraceID of the element to be modified
    \return true if the element was present in the container.
  */
  bool UpdateElementVisibilityWithGivenTraceID(const unsigned int & iId);

  //-------------------------------------------------------------------------

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
    \brief Get the list of highlighted elements TraceID.
    */
  std::list< unsigned int > GetHighlightedElementsTraceID();

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
  virtual bool DeleteElement(const unsigned int & iId) = 0;

  /**
  \overload DeleteElement(const unsigned int & iId)
  */
  virtual bool DeleteElement(MultiIndexContainerTraceIDIterator iIter) = 0;

  /** \brief Delete all highlighted elements
  \return the list of TraceIDs of such elements
  */
  virtual std::list< unsigned int > DeleteAllHighlightedElements() = 0;

  /**
    \brief Update all highlighted elements in the container with a given color.
    \note Elements remain highlighted as long as it is checked in the Table Widget.
    \param[in] iColor
    \return list of highlighted elements
  */
  std::list< unsigned int > UpdateAllHighlightedElementsWithGivenColor(
    QColor iColor);

  /**
    \brief Color code contour / mesh according to values provided
    \param[in] iColumnName Name of data provided
    \param[in] ivalues is a map where the key is the TraceID and the Value is
    a string that can be either converted to a double, or not
    \note if iColumnName and/or iValues are empty traces will be then rendered
    with their original colors.
  */
  void SetColorCode( const std::string& iColumnName,
                     const std::map< unsigned int, std::string >& iValues );

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

    using boost::multi_index::get;

    if( iColumnName.empty() || iValues.empty() )
      {
      RenderAllElementsWithOriginalColors();
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

          trace_it->SetScalarData( iColumnName, temp );
          }
        } //end make sure the trace has points !!!
      ++it;
      }

    SetScalarRangeForAllElements( min_value, max_value );

    if( m_ImageView )
      {
      this->m_ImageView->UpdateRenderWindows();
      }
    }

  void SetRandomColor( const std::string& iColumnName,
                       const std::map< unsigned int, unsigned int >& iIds );

  void SetRandomColor( const std::string& iColumnName,
                       const std::map< unsigned int, std::string >& iValues );

  /** \brief Apply the given lookup table to all traces in the container
      \param[in] iLut lookup table */
  void SetLookupTableForColorCoding( vtkLookupTable* iLut );

  void SetIntersectionLineWidth( const float& iWidth );


protected:
  vtkProperty *m_HighlightedProperty;
  float m_IntersectionLineWidth;

  virtual std::vector< vtkActor* > AddTrace( vtkPolyData* , vtkProperty* ) = 0;

  /** \brief Change elements highlighting property given a list of TraceIDs
  and the new status.
    \param[in] iList list of TraceIDs
    \param[in] iCheck */
  virtual void UpdateElementHighlightingWithGivenTraceIDsBase( const QStringList& iList,
                                                   const Qt::CheckState& iCheck );

  /** \brief Change elements visibility property given a list of TraceIDs
  and the new status.
    \param[in] iList list of TraceIDs
    \param[in] iCheck */
  virtual void UpdateElementVisibilityWithGivenTraceIDsBase( const QStringList& iList,
                                                 const Qt::CheckState& iCheck );


  /** \brief Render with original colors */
  void RenderAllElementsWithOriginalColors();

  /** \brief Set the scalar range */
  void SetScalarRangeForAllElements(const double& iMin, const double& iMax );

  void UpdateCurrentElementFromVisuBase( std::vector< vtkActor* > iActors,
                                         vtkPolyData* iNodes,
                                         const bool& iHighlighted,
                                         const bool& iVisible );

  /**
  \brief Update highlighting property of one element given one actor.
  \param[in] iActor Actor of the element to be modified
  \return true if the element exists
  \return false else
  */
  template< class TActor >
  bool UpdateElementHighlightingWithGivenActor(
      vtkActor *iActor,
      unsigned int& oTraceId,
      Qt::CheckState& oState )
  {
    using boost::multi_index::get;

    if ( iActor )
      {
      typedef typename MultiIndexContainerType::template index< TActor >::type::iterator
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
          temp_property->SetLineWidth( this->m_IntersectionLineWidth );
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

        MultiIndexContainerElementType tempStructure(*it);
        tempStructure.Highlighted = !it->Highlighted;

        // Note: it->Highlighted is the status before picking the actor
        if ( !it->Highlighted )
          {
          oState = Qt::Checked;
          }
        else
          {
          oState = Qt::Unchecked;
          }

        m_Container.get< TActor >().replace(it, tempStructure);

        if( m_ImageView )
          {
          m_ImageView->UpdateRenderWindows();
          }

        oTraceId = it->TraceID;

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
  bool UpdateElementVisibilityWithGivenActor(
      vtkActor *iActor,
      unsigned int& oTraceId,
      Qt::CheckState& oState )
  {
    using boost::multi_index::get;

    if ( iActor )
      {
      typedef typename MultiIndexContainerType::template index< TActor >::type::iterator
      IteratorType;
      IteratorType it = m_Container.get< TActor >().find(iActor);

      vtkProperty *temp_property = NULL;

      if ( it != m_Container.get< TActor >().end() )
        {
        it->SetActorVisibility( !it->Visible );

        MultiIndexContainerElementType tempStructure(*it);
        tempStructure.Visible = !it->Visible;

        // Note: it->Highlighted is the status before picking the actor
        if ( !it->Visible )
          {
          oState = Qt::Checked;
          }
        else
          {
          oState = Qt::Unchecked;
          }

        m_Container.get< TActor >().replace(it, tempStructure);

        if( m_ImageView )
          {
          m_ImageView->UpdateRenderWindows();
          }

        oTraceId = it->TraceID;

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
  bool UpdateElementVisibilityWithGivenActor(
      vtkActor *iActor,
      bool iState,
      unsigned int& oTraceID,
      Qt::CheckState& oState )
  {
    using boost::multi_index::get;

    if ( iActor )
      {
      typedef typename MultiIndexContainerType::template index< TActor >::type::iterator
      IteratorType;
      IteratorType it = m_Container.get< TActor >().find(iActor);

      if ( it != m_Container.get< TActor >().end() )
        {
        if ( it->Visible != iState )
          {
          it->SetActorVisibility( iState );

          MultiIndexContainerElementType tempStructure(*it);
          tempStructure.Visible = iState;

          // Note: it->Highlighted is the status before picking the actor
          if ( iState )
            {
            oState = Qt::Checked;
            }
          else
            {
            oState = Qt::Unchecked;
            }

          m_Container.get< TActor >().replace(it, tempStructure);
          //m_ImageView->UpdateRenderWindows();

          oTraceID = it->TraceID;
          }

        return true;
        }
      }

    return false;
  }
};

#include "TraceContainerBase.txx"

#endif // TRACECONTAINERBASE_H
