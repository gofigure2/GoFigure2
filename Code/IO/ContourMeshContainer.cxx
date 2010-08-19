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

#include "ContourMeshContainer.h"

#include "QGoImageView3D.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkPolyData.h"

ContourMeshContainer::
    ContourMeshContainer( QGoImageView3D* iView ) : m_ImageView( iView )
{}

ContourMeshContainer::~ContourMeshContainer( )
{}

void
ContourMeshContainer::
RemoveActorsWithGivenTimePoint( const unsigned int& iT )
{
  MultiIndexContainerTCoordIterator it0, it1;

  boost::tuples::tie(it0, it1) = m_Container.get<TCoord>().equal_range(iT);

  while (it0 != it1)
    {
    this->m_ImageView->RemoveActor( 0, it0->ActorXY );
    this->m_ImageView->RemoveActor( 1, it0->ActorXZ );
    this->m_ImageView->RemoveActor( 2, it0->ActorYZ );
    this->m_ImageView->RemoveActor( 3, it0->ActorXYZ );
    ++it0;
    }
  m_ImageView->UpdateRenderWindows();
}

void
ContourMeshContainer::
AddActorsWithGivenTimePoint( const unsigned int& iT )
{
  MultiIndexContainerTCoordIterator it0, it1;

  boost::tuples::tie(it0, it1) = m_Container.get<TCoord>().equal_range(iT);

  while (it0 != it1)
    {
    this->m_ImageView->AddActor( 0, it0->ActorXY );
    this->m_ImageView->AddActor( 1, it0->ActorXZ );
    this->m_ImageView->AddActor( 2, it0->ActorYZ );
    this->m_ImageView->AddActor( 3, it0->ActorXYZ );

    ++it0;
    }

  m_ImageView->UpdateRenderWindows();
}


bool
ContourMeshContainer::
RemoveElementFromVisualizationWithGivenTraceID(
    const unsigned int& iId )
{
  MultiIndexContainerTraceIDIterator
      it = m_Container.get<TraceID>().find(iId);
  if (it != m_Container.get<TraceID>().end())
    {
    this->m_ImageView->RemoveActor( 0, it->ActorXY );
    this->m_ImageView->RemoveActor( 1, it->ActorXZ );
    this->m_ImageView->RemoveActor( 2, it->ActorYZ );
    this->m_ImageView->RemoveActor( 3, it->ActorXYZ );

    m_Container.get<TraceID>().erase(it);

    m_ImageView->UpdateRenderWindows();
    return true;
    }
  return false;
}


bool
ContourMeshContainer::
UpdateElementHighlightingWithGivenTraceID( const unsigned int& iId,
                                           vtkProperty* iProperty )
{
  MultiIndexContainerTraceIDIterator
      it = m_Container.get<TraceID>().find(iId);

  vtkProperty* temp_property = NULL;

  if( it != m_Container.get<TraceID>().end() )
    {
    if( !it->Highlighted )
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
      temp_property = iProperty;
      }

    it->ActorXY->SetProperty( temp_property );
    it->ActorXZ->SetProperty( temp_property );
    it->ActorYZ->SetProperty( temp_property );
    it->ActorXYZ->SetProperty( temp_property );

    if( !it->Highlighted )
      {
      temp_property->Delete();
      }

    ContourMeshStructure tempStructure( *it );
    tempStructure.Highlighted = it->Highlighted;

    m_Container.get<TraceID>().replace( it, tempStructure );
    m_ImageView->UpdateRenderWindows();

    return true;
    }

  return false;
}

bool
ContourMeshContainer::
UpdateElementVisibilityWithGivenTraceID( const unsigned int& iId )
{
  MultiIndexContainerTraceIDIterator
      it = m_Container.get<TraceID>().find(iId);

  if( it != m_Container.get<TraceID>().end() )
    {
    it->ActorXY->SetVisibility( !it->Visible );
    it->ActorXZ->SetVisibility( !it->Visible );
    it->ActorYZ->SetVisibility( !it->Visible );
    it->ActorXYZ->SetVisibility( !it->Visible );

    ContourMeshStructure tempStructure( *it );
    tempStructure.Highlighted = !it->Highlighted;

    m_Container.get<TraceID>().replace(it, tempStructure);

    m_ImageView->UpdateRenderWindows();
    return true;
    }

  return false;
}

bool
ContourMeshContainer::
DeleteElement( const unsigned int& iId )
{
  MultiIndexContainerTraceIDIterator
      it = m_Container.get<TraceID>().find(iId);

  if (it != m_Container.get<TraceID>().end())
    {
    this->m_ImageView->RemoveActor( 0, it->ActorXY );
    this->m_ImageView->RemoveActor( 1, it->ActorXZ );
    this->m_ImageView->RemoveActor( 2, it->ActorYZ );
    this->m_ImageView->RemoveActor( 3, it->ActorXYZ );

    it->Nodes->Delete();
    it->ActorXY->Delete();
    it->ActorXZ->Delete();
    it->ActorYZ->Delete();
    it->ActorXYZ->Delete();

    m_Container.get<TraceID>().erase(it);

    m_ImageView->UpdateRenderWindows();
    return true;
    }
  else
    {
    return false;
    }
}
