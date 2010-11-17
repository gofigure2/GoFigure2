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

#ifndef __TraceContainerBase_txx
#define __TraceContainerBase_txx

#include "TraceContainerBase.h"

template< class TContainer >
TraceContainerBase< TContainer >::
TraceContainerBase( QObject* iParent, QGoImageView3D* iView ) :
  QObject( iParent ),
  m_ImageView( iView ),
  m_CurrentElement(),
  m_HighlightedProperty(NULL)
{}

template< class TContainer >
TraceContainerBase< TContainer >::
~TraceContainerBase()
{}

template< class TContainer >
void
TraceContainerBase< TContainer >::
Print()
{
  this->Print( m_Container.begin(), m_Container.end() );
}

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase< TContainer >::
SetHighlightedProperty(vtkProperty *iProperty)
{
  using boost::multi_index::get;

  this->m_HighlightedProperty = iProperty;

  MultiIndexContainerHighlightedIterator it0, it1;
  boost::tuples::tie(it0, it1) =
    m_Container.get< Highlighted >().equal_range(true);

  while ( it0 != it1 )
    {
    if ( it0->Highlighted )
      {
      it0->SetActorProperties( this->m_HighlightedProperty );
      }
    ++it0;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
vtkProperty *
TraceContainerBase< TContainer >::
GetHighlightedProperty()
{
  return m_HighlightedProperty;
}
//-------------------------------------------------------------------------

template< class TContainer >
void
TraceContainerBase< TContainer >::
Insert(const typename TraceContainerBase::MultiIndexContainerElementType & iE)
{
  this->m_Container.insert( iE );
}

template< class TContainer >
void
TraceContainerBase< TContainer >::
InsertCurrentElement()
{
  this->m_Container.insert( this->m_CurrentElement );
}

template< class TContainer >
void
TraceContainerBase< TContainer >::ResetCurrentElement()
{
  this->m_CurrentElement = MultiIndexContainerElementType();
}

//-------------------------------------------------------------------------
template< class TContainer >
void TraceContainerBase< TContainer >::
UpdateCurrentElementFromDB( unsigned int iTraceID,
                            double irgba[4] )
{
  this->m_CurrentElement.TraceID = iTraceID;
  this->m_CurrentElement.rgba[0] = irgba[0];
  this->m_CurrentElement.rgba[1] = irgba[1];
  this->m_CurrentElement.rgba[2] = irgba[2];
  this->m_CurrentElement.rgba[3] = irgba[3];
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
bool
TraceContainerBase< TContainer >::
RemoveElementFromVisualizationWithGivenTraceID( const unsigned int & iId)
{
  using boost::multi_index::get;

  MultiIndexContainerTraceIDIterator
    it = m_Container.get< TraceID >().find(iId);

  if ( it != m_Container.get< TraceID >().end() )
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

    this->m_CurrentElement = *it;

    m_Container.get< TraceID >().erase(it);

    m_ImageView->UpdateRenderWindows();

    return true;
    }
  return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
bool
TraceContainerBase<TContainer>::
UpdateElementHighlightingWithGivenTraceID(const unsigned int & iId)
{
  using boost::multi_index::get;

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

    m_Container.get< TraceID >().replace(it, tempStructure);
    m_ImageView->UpdateRenderWindows();

    return true;
    }

  return false;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase<TContainer>::
UpdateElementHighlightingWithGivenTraceIDs( const QStringList& iList,
                                            const Qt::CheckState& iCheck )
{
  using boost::multi_index::get;

  if( !iList.empty() )
    {
    MultiIndexContainerTraceIDIterator it;

    vtkProperty *temp_property = NULL;

    QStringList::const_iterator constIterator = iList.begin();

    while( constIterator != iList.end() )
      {
      it = m_Container.get< TraceID >().find((*constIterator).toUInt());

      if ( it != m_Container.get< TraceID >().end() )
        {
        if ( !iCheck )
          {
          temp_property = vtkProperty::New();
          temp_property->SetColor(it->rgba[0],
                                  it->rgba[1],
                                  it->rgba[2]);
          temp_property->SetOpacity(it->rgba[3]);
          }
        else
          {
          temp_property = this->m_HighlightedProperty;
          }

        it->SetActorProperties( temp_property );

        if ( !iCheck )
          {
          temp_property->Delete();
          }

        MultiIndexContainerElementType tempStructure(*it);
        tempStructure.Highlighted = iCheck;

        m_Container.get< TraceID >().replace(it, tempStructure);
        }

      ++constIterator;
      }

    m_ImageView->UpdateRenderWindows();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
void
TraceContainerBase<TContainer>::
UpdateElementVisibilityWithGivenTraceIDs( const QStringList& iList,
                                          const Qt::CheckState& iCheck )
{
  using boost::multi_index::get;

  if( !iList.empty() )
    {
    MultiIndexContainerTraceIDIterator it;

    typedef void ( QGoImageView3D::*ImageViewMember )(const int &, vtkActor *);
    ImageViewMember f;

    QStringList::const_iterator constIterator = iList.begin();

    while( constIterator != iList.end() )
      {
      it = m_Container.get< TraceID >().find((*constIterator).toUInt());

      if ( it != m_Container.get< TraceID >().end() )
        {
        // if ( it->TCoord != m_TCoord )
          {
          if ( iCheck )
            {
            f = &QGoImageView3D::RemoveActor;
            }
          else
            {
            f = &QGoImageView3D::AddActor;
            }

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

        it->SetActorVisibility( iCheck );

        MultiIndexContainerElementType tempStructure(*it);
        tempStructure.Visible = iCheck;

        m_Container.get< TraceID >().replace(it, tempStructure);
        }

      ++constIterator;
      }
    m_ImageView->UpdateRenderWindows();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
std::list< unsigned int >
TraceContainerBase<TContainer>::
UpdateAllHighlightedElementsWithGivenColor(QColor iColor)
{
  using boost::multi_index::get;

  MultiIndexContainerHighlightedIterator it0, it1;

  boost::tuples::tie(it0, it1) =
    m_Container.get< Highlighted >().equal_range(true);

  double r(1.), g(1.), b(1.), a(1.);
  if ( iColor.isValid() )
    {
    iColor.getRgbF(&r, &g, &b, &a);
    }

  std::list< unsigned int > oList;
  while ( it0 != it1 )
    {
    MultiIndexContainerElementType temp(*it0);
    temp.rgba[0] = r;
    temp.rgba[1] = g;
    temp.rgba[2] = b;
    temp.rgba[3] = a;

    m_Container.get< Highlighted >().replace(it0, temp);

    oList.push_back(it0->TraceID);

    ++it0;
    }

  return oList;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< class TContainer >
std::list< unsigned int >
TraceContainerBase<TContainer>::
GetHighlightedElementsTraceID()
{
  MultiIndexContainerHighlightedIterator it0, it1;

  using boost::multi_index::get;

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

#endif
