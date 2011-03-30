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

#ifndef __TraceContainerBase_txx
#define __TraceContainerBase_txx

#include "TraceContainerBase.h"
#include "vtkPointData.h"
#include "vtkDoubleArray.h"
#include "vtkLookupTableManager.h"

template< class TContainer >
TraceContainerBase< TContainer >::TraceContainerBase(QObject *iParent, QGoImageView3D *iView) :
  QObject(iParent),
  m_ImageView(iView),
  m_CurrentElement(),
  m_HighlightedProperty(NULL),
  m_IntersectionLineWidth(1.)
{
}

template< class TContainer >
TraceContainerBase< TContainer >::
~TraceContainerBase()
{
}

template< class TContainer >
void
TraceContainerBase< TContainer >::Print()
{
  this->Print( m_Container.begin(), m_Container.end() );
}

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::SetHighlightedProperty(vtkProperty *iProperty)
{
  using boost::multi_index:: get;

  this->m_HighlightedProperty = iProperty;

  MultiIndexContainerHighlightedIterator it0, it1;
  boost::tuples::tie(it0, it1) =
    m_Container.get< Highlighted >().equal_range(true);

  while ( it0 != it1 )
    {
    if ( it0->Highlighted )
      {
      it0->SetActorProperties(this->m_HighlightedProperty);
      }
    ++it0;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
vtkProperty *
TraceContainerBase< TContainer >::GetHighlightedProperty()
{
  return m_HighlightedProperty;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
std::vector< vtkActor * >
TraceContainerBase< TContainer >::GetActorGivenTraceID(unsigned int iTraceID)
{
  using boost::multi_index:: get;

  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iTraceID);

  std::vector< vtkActor * > listActors;

  if ( it != m_Container.get< TraceID >().end() )
    {
    listActors.push_back( ( *it ).ActorXY );
    listActors.push_back( ( *it ).ActorYZ );
    listActors.push_back( ( *it ).ActorXZ );
    listActors.push_back( ( *it ).ActorXYZ );

    //std::cout << "Actor found" << std::endl;
    }
  else
    {
    std::cout << "Actor not found" << std::endl;
    }

  return listActors;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
bool
TraceContainerBase< TContainer >::UpdateCurrentElementFromExistingOne(unsigned int iTraceID, bool iErase)
{
  using boost::multi_index:: get;

  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iTraceID);

  if ( it != m_Container.get< TraceID >().end() )
    {
    // update current element
    this->m_CurrentElement = *it;

    // clean the container but don't erase the pointers since we still have the
    // adresses in the m_CurrentElement
    if(iErase)
      {
      m_Container.get< TraceID >().erase(it);
      }

    return true;
    }
  else
    {
    return false;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::Insert(const typename TraceContainerBase::MultiIndexContainerElementType & iE)
{
  this->m_Container.insert(iE);
}

template< class TContainer >
void
TraceContainerBase< TContainer >::InsertCurrentElement()
{
  this->m_Container.insert(this->m_CurrentElement);
}

template< class TContainer >
void
TraceContainerBase< TContainer >::ResetCurrentElement()
{
  this->m_CurrentElement = MultiIndexContainerElementType();
}

//-------------------------------------------------------------------------
template< class TContainer >
void TraceContainerBase< TContainer >::UpdateCurrentElementFromDB(unsigned int iTraceID,
                                                                  double irgba[4], bool IsVisible)
{
  this->m_CurrentElement.TraceID = iTraceID;
  this->m_CurrentElement.rgba[0] = irgba[0];
  this->m_CurrentElement.rgba[1] = irgba[1];
  this->m_CurrentElement.rgba[2] = irgba[2];
  this->m_CurrentElement.rgba[3] = irgba[3];
  this->m_CurrentElement.Visible = IsVisible;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
vtkPolyData *
TraceContainerBase< TContainer >::GetCurrentElementNodes()
{
  return this->m_CurrentElement.Nodes;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
double *
TraceContainerBase< TContainer >::GetCurrentElementColor()
{
  return this->m_CurrentElement.rgba;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
bool
TraceContainerBase< TContainer >::RemoveElementFromVisualizationWithGivenTraceID(const unsigned int & iId)
{
  using boost::multi_index:: get;

  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iId);

  if ( it != m_Container.get< TraceID >().end() )
    {
    if ( m_ImageView )
      {
      if ( it->ActorXY )
        {
        this->m_ImageView->RemoveActor(0, it->ActorXY);
        }
      if ( it->ActorXZ )
        {
        this->m_ImageView->RemoveActor(1, it->ActorXZ);
        }
      if ( it->ActorYZ )
        {
        this->m_ImageView->RemoveActor(2, it->ActorYZ);
        }
      if ( it->ActorXYZ )
        {
        this->m_ImageView->RemoveActor(3, it->ActorXYZ);
        }
      }

    this->m_CurrentElement = *it;

    m_Container.get< TraceID >().erase(it);

    if ( m_ImageView )
      {
      m_ImageView->UpdateRenderWindows();
      }

    return true;
    }
  return false;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
bool
TraceContainerBase< TContainer >::UpdateElementHighlightingWithGivenTraceID(const unsigned int & iId)
{
  using boost::multi_index:: get;

  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iId);

  vtkProperty *temp_property = NULL;

  if ( it != m_Container.get< TraceID >().end() )
    {
    if ( it->Highlighted )
      {
      temp_property = vtkProperty::New();
      temp_property->SetColor(it->rgba[0],
                              it->rgba[1],
                              it->rgba[2]);
      temp_property->SetOpacity(it->rgba[3]);
      temp_property->SetLineWidth(this->m_IntersectionLineWidth);
      }
    else
      {
      temp_property = this->m_HighlightedProperty;
      }

    it->SetActorProperties(temp_property);

    if ( it->Highlighted )
      {
      temp_property->Delete();
      }

    bool highlighted = !it->Highlighted;
    
    m_Container.get< TraceID >().
        modify( it , change_highlighted<MultiIndexContainerElementType>(highlighted) );

    assert ( m_ImageView );

    m_ImageView->UpdateRenderWindows();

    return true;
    }

  return false;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::UpdateElementHighlightingWithGivenTraceIDsBase(const QStringList & iList,
                                                                                 const Qt::CheckState & iCheck)
{
  using boost::multi_index:: get;

  if ( !iList.empty() )
    {
    MultiIndexContainerTraceIDIterator it;

    vtkProperty *temp_property = NULL;

    QStringList::const_iterator constIterator = iList.begin();

    while ( constIterator != iList.end() )
      {
      it = m_Container.get< TraceID >().find( ( *constIterator ).toUInt() );

      if ( it != m_Container.get< TraceID >().end() )
        {
        if ( !iCheck )
          {
          temp_property = vtkProperty::New();
          temp_property->SetColor(it->rgba[0],
                                  it->rgba[1],
                                  it->rgba[2]);
          temp_property->SetOpacity(it->rgba[3]);
          temp_property->SetLineWidth(this->m_IntersectionLineWidth);
          }
        else
          {
          temp_property = this->m_HighlightedProperty;
          }

        it->SetActorProperties(temp_property);

        if ( !iCheck )
          {
          temp_property->Delete();
          }

        bool highlight = iCheck;

        m_Container.get< TraceID >().
            modify( it , change_highlighted<MultiIndexContainerElementType>(highlight) );

        }

      ++constIterator;
      }

    assert ( m_ImageView );

    m_ImageView->UpdateRenderWindows();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::UpdateElementVisibilityWithGivenTraceIDsBase(const QStringList & iList,
                                                                               const Qt::CheckState & iCheck)
{
  using boost::multi_index:: get;

  if ( !iList.empty() )
    {
    MultiIndexContainerTraceIDIterator it;

    typedef void ( QGoImageView3D::*ImageViewMember )(const int &, vtkActor *);
    ImageViewMember f;

    QStringList::const_iterator constIterator = iList.begin();
    bool                        visible;

    while ( constIterator != iList.end() )
      {
      it = m_Container.get< TraceID >().find( ( *constIterator ).toUInt() );

      if ( it != m_Container.get< TraceID >().end() )
        {
        if ( iCheck == Qt::Unchecked )
          {
          f = &QGoImageView3D::RemoveActor;
          visible = false;
          }
        else
          {
          f = &QGoImageView3D::AddActor;
          visible = true;
          }

        if ( m_ImageView )
          {
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
          }

        it->SetActorVisibility(visible);
        
        m_Container.get< TraceID >().
            modify( it , change_visible<MultiIndexContainerElementType>(visible) );
        }

      ++constIterator;
      }
    if ( m_ImageView )
      {
      m_ImageView->UpdateRenderWindows();
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
std::list< unsigned int >
TraceContainerBase< TContainer >::UpdateAllHighlightedElementsWithGivenColor(QColor iColor)
{
  using boost::multi_index:: get;

  MultiIndexContainerHighlightedIterator it0, it1;

  boost::tuples::tie(it0, it1) =
    m_Container.get< Highlighted >().equal_range(true);

  std::list< unsigned int > oList;
  while ( it0 != it1 )
    {
    m_Container.get< Highlighted >().modify(it0,
        change_color<MultiIndexContainerElementType>(iColor));
    oList.push_back(it0->TraceID);
    ++it0;
    }

  return oList;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
std::list< unsigned int >
TraceContainerBase< TContainer >::GetHighlightedElementsTraceID()
{
  MultiIndexContainerHighlightedIterator it0, it1;

  using boost::multi_index:: get;

  boost::tuples::tie(it0, it1) =
    m_Container.get< Highlighted >().equal_range(true);

  std::list< unsigned int > oList;
  while ( it0 != it1 )
    {
    oList.push_back(it0->TraceID);
    ++it0;
    }
  return oList;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
bool
TraceContainerBase< TContainer >::UpdateElementVisibilityWithGivenTraceID(const unsigned int & iId)
{
  using boost::multi_index:: get;

  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iId);

  typedef void ( QGoImageView3D::*ImageViewMember )(const int &, vtkActor *);
  ImageViewMember f;

/*
* \todo Nicolas-should be an assert
*/
  if ( it != m_Container.get< TraceID >().end() )
    {
    if ( it->Visible )
      {
      f = &QGoImageView3D::RemoveActor;
      }
    else
      {
      f = &QGoImageView3D::AddActor;
      }

    if ( m_ImageView )
      {
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
      }

    it->SetActorVisibility(!it->Visible);
    
    bool visible = !it->Visible;
    
    m_Container.get< TraceID >().
        modify( it , change_visible<MultiIndexContainerElementType>(visible) );

    assert ( m_ImageView );

    m_ImageView->UpdateRenderWindows();

    return true;
    }

  return false;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::SetColorCode(const std::string & iColumnName,
                                               const std::map< unsigned int, std::string > & iValues)
{
  typedef typename std::map< unsigned int, std::string > MapType;
  typedef typename MapType::const_iterator               MapConstIterator;

  std::map< std::string, double > stringmap;

  using boost::multi_index:: get;

  if ( iColumnName.empty() || iValues.empty() )
    {
    this->RenderAllElementsWithOriginalColors();
    return;
    }

  MapConstIterator it = iValues.begin();

  double temp = 0.;
  try
    {
    temp = boost::lexical_cast< double >(it->second);
    }
  catch(boost::bad_lexical_cast &)
    {
    if ( stringmap.empty() )
      {
      stringmap[it->second] = 0.;
      }
    else
      {
      std::map< std::string, double >::iterator m_it = stringmap.find(it->second);

      if ( m_it != stringmap.end() )
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

  while ( it != iValues.end() )
    {
    MultiIndexContainerTraceIDIterator
      trace_it = this->m_Container.get< TraceID >().find(it->first);

    if ( trace_it != this->m_Container.get< TraceID >().end() )
      {
      if ( trace_it->Nodes ) //make sure the trace has points !!!
        {
        // Here let's make sure you are not passing crazy values!
        try
          {
          temp = boost::lexical_cast< double >(it->second);
          }
        catch(boost::bad_lexical_cast &)
          {
          // stringmap is not empty and has at least one element
          std::map< std::string, double >::iterator m_it = stringmap.find(it->second);

          if ( m_it != stringmap.end() )
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

        if ( temp > max_value )
          {
          max_value = temp;
          }
        if ( temp < min_value )
          {
          min_value = temp;
          }
        trace_it->SetScalarData(iColumnName, temp);
        }
      } //end make sure the trace has points !!!
    ++it;
    }

  SetScalarRangeForAllElements(min_value, max_value);

  if ( m_ImageView )
    {
    this->m_ImageView->UpdateRenderWindows();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::SetRandomColor(const std::string & iColumnName,
                                                 const std::map< unsigned int, unsigned int > & iIds)
{
  typedef typename std::map< unsigned int, unsigned int > MapType;
  typedef typename MapType::const_iterator                MapConstIterator;

  using boost::multi_index:: get;

  if ( iColumnName.empty() || iIds.empty() )
    {
    RenderAllElementsWithOriginalColors();
    return;
    }

  MapConstIterator it = iIds.begin();

  double temp = static_cast< double >(it->second % 30);

  double min_value = temp;
  double max_value = temp;

  while ( it != iIds.end() )
    {
    MultiIndexContainerTraceIDIterator
      trace_it = this->m_Container.get< TraceID >().find(it->first);

    if ( trace_it != this->m_Container.get< TraceID >().end() )
      {
      if ( trace_it->Nodes ) //make sure the trace has points !!!
        {
        // Here let's make sure you are not passing crazy values!
        temp = boost::numeric_cast< double >(it->second % 30);

        if ( temp > max_value )
          {
          max_value = temp;
          }
        if ( temp < min_value )
          {
          min_value = temp;
          }

        trace_it->SetScalarData(iColumnName, temp);
        }
      } //end make sure the trace has points !!!
    ++it;
    }

  this->SetScalarRangeForAllElements(min_value, max_value);
  this->SetLookupTableForColorCoding(
    vtkLookupTableManager::GetRandomLookupTable() );

  if ( m_ImageView )
    {
    this->m_ImageView->UpdateRenderWindows();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::RenderAllElementsWithOriginalColors()
{
  typename MultiIndexContainerType::iterator t_it = m_Container.begin();

  while ( t_it != m_Container.end() )
    {
    t_it->RenderWithOriginalColors();
    ++t_it;
    }
  if ( m_ImageView )
    {
    this->m_ImageView->UpdateRenderWindows();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::SetScalarRangeForAllElements(const double & iMin, const double & iMax)
{
  // Let's set the scalar range (in order to get nice colors)
  typename MultiIndexContainerType::iterator t_it = m_Container.begin();

  while ( t_it != m_Container.end() )
    {
    t_it->SetScalarRange(iMin, iMax);
    ++t_it;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::SetLookupTableForColorCoding(vtkLookupTable *iLut)
{
  if ( iLut )
    {
    typename MultiIndexContainerType::iterator it = m_Container.begin();

    while ( it != m_Container.end() )
      {
      it->SetLookupTable(iLut);
      ++it;
      }
    if ( m_ImageView )
      {
      this->m_ImageView->UpdateRenderWindows();
      }
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::SetRandomColor(const std::string & iColumnName,
                                                 const std::map< unsigned int, std::string > & iValues)
{
  typedef std::map< unsigned int, std::string > MapType;
  typedef MapType::const_iterator               MapConstIterator;

  std::map< std::string, double > stringmap;

  if ( iColumnName.empty() || iValues.empty() )
    {
    this->RenderAllElementsWithOriginalColors();
    return;
    }

  MapConstIterator it = iValues.begin();

  double temp = 0.;
  try
    {
    temp = boost::lexical_cast< double >(it->second);
    }
  catch(boost::bad_lexical_cast &)
    {
    if ( stringmap.empty() )
      {
      stringmap[it->second] = 0.;
      }
    else
      {
      std::map< std::string, double >::iterator m_it = stringmap.find(it->second);

      if ( m_it != stringmap.end() )
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

  unsigned int val = static_cast< unsigned int >(temp);
  unsigned int modulo = val % 30;

  temp = static_cast< double >(modulo);

  double min_value = temp;
  double max_value = temp;

  using boost::multi_index:: get;

  while ( it != iValues.end() )
    {
    MultiIndexContainerTraceIDIterator
      trace_it = this->m_Container.get< TraceID >().find(it->first);

    if ( trace_it != this->m_Container.get< TraceID >().end() )
      {
      if ( trace_it->Nodes ) //make sure the trace has points !!!
        {
        // Here let's make sure you are not passing crazy values!
        try
          {
          temp = boost::lexical_cast< double >(it->second);
          }
        catch(boost::bad_lexical_cast &)
          {
          // stringmap is not empty and has at least one element
          std::map< std::string, double >::iterator m_it = stringmap.find(it->second);

          if ( m_it != stringmap.end() )
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

        val = static_cast< unsigned int >(temp);
        modulo = val % 30;

        temp = static_cast< double >(modulo);

        if ( temp > max_value )
          {
          max_value = temp;
          }
        if ( temp < min_value )
          {
          min_value = temp;
          }
        trace_it->SetScalarData(iColumnName, temp);
        }
      } //end make sure the trace has points !!!
    ++it;
    }

  SetScalarRangeForAllElements(min_value, max_value);

  if ( m_ImageView )
    {
    this->m_ImageView->UpdateRenderWindows();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::UpdateCurrentElementFromVisuBase(std::vector< vtkActor * > iActors,
                                                                   vtkPolyData *iNodes,
                                                                   const bool & iHighlighted,
                                                                   const bool & iVisible)
{
  if ( iActors.size() != 4 )
    {
    std::cerr << "iActors.size() != 4" << std::endl;
    return;
    }
  else
    {
    this->m_CurrentElement.ActorXY = iActors[0];
    this->m_CurrentElement.ActorXZ = iActors[1];
    this->m_CurrentElement.ActorYZ = iActors[2];
    this->m_CurrentElement.ActorXYZ = iActors[3];
    }
  this->m_CurrentElement.Nodes = iNodes;
  this->m_CurrentElement.Highlighted = iHighlighted;
  this->m_CurrentElement.Visible = iVisible;

  if ( iHighlighted )
    {
    this->m_CurrentElement.SetActorProperties(this->m_HighlightedProperty);
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::SetIntersectionLineWidth(const float & iWidth)
{
  m_IntersectionLineWidth = iWidth;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
unsigned int
TraceContainerBase< TContainer >::GetCollectionIDOfGivenTraceID(unsigned int iTraceID)
{
  using boost::multi_index:: get;
  MultiIndexContainerTraceIDIterator
    it0 = m_Container.get< TraceID >().find(iTraceID);

  if ( it0 != m_Container.get< TraceID >().end() )
    {
    return it0->CollectionID;
    }
  else
    {
    std::cout << "booo" << std::endl;
    return std::numeric_limits< unsigned int >::max();
    }
}

//-------------------------------------------------------------------------

#endif
