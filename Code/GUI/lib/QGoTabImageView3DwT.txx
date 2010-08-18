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

#ifndef __QGoTabImageView3DwT_txx
#define __QGoTabImageView3DwT_txx

#include "QGoImageView3D.h"
#include "vtkProperty.h"

//-------------------------------------------------------------------------
template< typename TActor >
void
QGoTabImageView3DwT::
HighLightContours()
{
  /// Modifiy "picked" to "modified"
  std::list<vtkProp3D*>
      listofpicked = m_ImageView->GetListOfPickedContours();

  std::list<vtkProp3D*>::iterator
      it = listofpicked.begin();

  while (it != listofpicked.end())
    {
    // Mode 0: One click selection
    HighLightActorsInContainer< TActor >( m_ContourContainer,
                                          static_cast<vtkActor*>(*it) );
    ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< typename TActor >
void
QGoTabImageView3DwT::
HighLightMeshes()
{
  std::list<vtkProp3D*>
      listofpicked = m_ImageView->GetListOfModifiedActors3D();

  std::list<vtkProp3D*>::iterator
      it = listofpicked.begin();

  while (it != listofpicked.end())
    {
    HighLightActorsInContainer< TActor >( m_MeshContainer,
                                          static_cast<vtkActor*>(*it) );
  ++it;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
template< typename TActor >
void
QGoTabImageView3DwT::
HighLightActorsInContainer(
    ContourMeshStructureMultiIndexContainer& iContainer,
    vtkActor* iActor )
{
  vtkProperty* select_property = vtkProperty::New();
  select_property->SetColor(1., 0., 0.);
  select_property->SetLineWidth(3.);

  // Change the corresponding highlighted value in the container
  typename ContourMeshStructureMultiIndexContainer::index<TActor>::type::iterator
    actor_it = iContainer.get<TActor>().find(iActor);

  // Is the actor in the container?
  if (actor_it != iContainer.get<TActor>().end())
    {
    // if the element was not highlighted
    if ( !actor_it->Highlighted )
      {
      // highlight the element
      this->m_ImageView->ChangeActorProperty( 0,
                                              actor_it->ActorXY,
                                              select_property );
      this->m_ImageView->ChangeActorProperty( 1,
                                              actor_it->ActorXZ,
                                              select_property );
      this->m_ImageView->ChangeActorProperty( 2,
                                              actor_it->ActorYZ,
                                              select_property );
      this->m_ImageView->ChangeActorProperty( 3,
                                              actor_it->ActorXYZ,
                                              select_property );
      }
    else
      {
      // change the color of the element to its original color
      vtkProperty* temp_property = vtkProperty::New();
      temp_property->SetColor( actor_it->rgba[0],
                               actor_it->rgba[1],
                               actor_it->rgba[2] );
      temp_property->SetOpacity( actor_it->rgba[3] );
      temp_property->SetLineWidth(1.);

      this->m_ImageView->ChangeActorProperty( 0,
                                              actor_it->ActorXY,
                                              temp_property );
      this->m_ImageView->ChangeActorProperty( 1,
                                              actor_it->ActorXZ,
                                              temp_property );
      this->m_ImageView->ChangeActorProperty( 2,
                                              actor_it->ActorYZ,
                                              temp_property );
      this->m_ImageView->ChangeActorProperty( 3,
                                              actor_it->ActorXYZ,
                                              temp_property );
      temp_property->Delete();
      }

    ContourMeshStructure temp(*actor_it);
    temp.Highlighted = !actor_it->Highlighted;
    iContainer.get<TActor>().replace(actor_it, temp);
    }

  select_property->Delete();
}
//-------------------------------------------------------------------------

#endif // __QGoTabImageView3DwT_txx
