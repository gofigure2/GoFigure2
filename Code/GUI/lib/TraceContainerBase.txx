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

#endif
