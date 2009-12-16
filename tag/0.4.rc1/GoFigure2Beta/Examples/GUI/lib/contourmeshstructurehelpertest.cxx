#include <vector>

#include "vtkActor.h"
#include "vtkPolyData.h"

#include "ContourMeshStructure.h"
#include "ContourMeshStructureHelper.h"

int main( int , char** )
{
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
