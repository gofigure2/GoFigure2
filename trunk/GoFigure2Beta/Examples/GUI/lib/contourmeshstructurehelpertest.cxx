/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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

#include <vector>

#include "vtkActor.h"
#include "vtkPolyData.h"

#include "ContourMeshStructure.h"
#include "ContourMeshStructureHelper.h"

int main( int , char** )
{

    // Test Allocation/Deallocation on the stack  
    {
    ContourMeshStructureMultiIndexContainer temp_container;
    std::cout <<"Allocation / Deallocation on the stack OK!!!" <<std::endl;
    }

    {
    ContourMeshStructureMultiIndexContainer* temp_container2 =
      new ContourMeshStructureMultiIndexContainer;
    delete temp_container2;
    std::cout <<"Allocation / Deallocation on the heap OK!!!" <<std::endl;
    }

  ContourMeshStructureMultiIndexContainer container;

  std::vector< vtkActor* > ActorVector( 20 );
  std::vector< vtkPolyData* > NodesVector( 20 );
  unsigned int id = 0;
  unsigned int t = 0;
  unsigned int i;

  for( i = 0; i < 20; i++ )
    {
    ActorVector[i] = vtkActor::New();
    NodesVector[i] = vtkPolyData::New();

    id = i / 4;
    t = id;
    container.insert( ContourMeshStructure( i, ActorVector[i], NodesVector[i],
      id, t, ( i == 10 ), 0.5, 0.5, 0.5, 1., 0 ) );
    }

  std::list< ContourMeshStructure* > list = FindContourGivenTraceID( container, 10 );
  ContourMeshStructure* c0 = list.front();
  std::cout <<*c0 <<std::endl;

  if( c0->Actor != ActorVector[10]  )
    {
    std::cerr <<"c0.Actor != ActorVector[10]" <<std::endl;
    for( i = 0; i < 20; i++ )
      {
      ActorVector[i]->Delete();
      NodesVector[i]->Delete();
      }
    return EXIT_FAILURE;
    }
  if( c0->Nodes != NodesVector[10] )
    {
    std::cerr <<"c0->Nodes != NodesVector[10]" <<std::endl;
    for( i = 0; i < 20; i++ )
      {
      ActorVector[i]->Delete();
      NodesVector[i]->Delete();
      }
    return EXIT_FAILURE;
    }
  if( c0->Highlighted != true )
    {
    std::cerr <<"c0.Highlighted != true" <<std::endl;
    for( i = 0; i < 20; i++ )
      {
      ActorVector[i]->Delete();
      NodesVector[i]->Delete();
      }
    return EXIT_FAILURE;
    }

  c0->Highlighted = false;

  ContourMeshStructure* c1 = FindContourGivenActor( container, ActorVector[5] );

  if( c1->TraceID != 5 )
    {
    std::cerr <<"c1->TraceID != 5" <<std::endl;
    for( i = 0; i < 20; i++ )
      {
      ActorVector[i]->Delete();
      NodesVector[i]->Delete();
      }
    return EXIT_FAILURE;
    }

  list = FindContourGivenNodes( container, NodesVector[15] );
  ContourMeshStructure* c2 = list.front();

  if( c2->TraceID != 15 )
    {
    std::cerr <<"c2->TraceID != 15" <<std::endl;

    for( i = 0; i < 20; i++ )
      {
      ActorVector[i]->Delete();
      NodesVector[i]->Delete();
      }

    return EXIT_FAILURE;
    }

  std::cout <<*c2 <<std::endl;

  unsigned int k = 0;

  for( i = 0; i < 5; i++ )
    {
    list = FindContourGivenTimePoint( container, i );

    std::list< ContourMeshStructure* >::iterator c_it = list.begin();

    while( c_it != list.end() )
      {
      if( (*c_it)->TraceID != k )
        {
        std::cerr <<"(*c_it)->TraceID != k" <<std::endl;
        std::cerr <<(*c_it)->TraceID <<" != " <<k <<std::endl;

        for( i = 0; i < 20; i++ )
          {
          ActorVector[i]->Delete();
          NodesVector[i]->Delete();
          }

        return EXIT_FAILURE;
        }
      ++c_it;
      ++k;
      }
    }

  for( i = 0; i < 20; i++ )
    {
    ActorVector[i]->Delete();
    NodesVector[i]->Delete();
    }
  return EXIT_SUCCESS;
}
