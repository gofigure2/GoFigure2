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

  std::list< ContourMeshStructure > list = FindContourGivenTraceID( container, 10 );
  ContourMeshStructure c = list.front();
  std::cout <<c <<std::endl;

  if( c.Actor != ActorVector[10]  )
    {
    std::cerr <<"c.Actor != ActorVector[10]" <<std::endl;
    for( i = 0; i < 20; i++ )
      {
      ActorVector[i]->Delete();
      NodesVector[i]->Delete();
      }
    return EXIT_FAILURE;
    }
  if( c.Nodes != NodesVector[10] )
    {
    std::cerr <<"c.Nodes != NodesVector[10]" <<std::endl;
    for( i = 0; i < 20; i++ )
      {
      ActorVector[i]->Delete();
      NodesVector[i]->Delete();
      }
    return EXIT_FAILURE;
    }
  if( c.Highlighted != true )
    {
    std::cerr <<"c.Highlighted != true" <<std::endl;
    for( i = 0; i < 20; i++ )
      {
      ActorVector[i]->Delete();
      NodesVector[i]->Delete();
      }
    return EXIT_FAILURE;
    }

  c.Highlighted = false;

  c = FindContourGivenActor( container, ActorVector[5] );

  if( c.TraceID != 5 )
    {
    std::cerr <<"c.TraceID != 5" <<std::endl;
    for( i = 0; i < 20; i++ )
      {
      ActorVector[i]->Delete();
      NodesVector[i]->Delete();
      }
    return EXIT_FAILURE;
    }

  list = FindContourGivenNodes( container, NodesVector[15] );
  c = list.front();

  if( c.TraceID != 15 )
    {
    std::cerr <<"c.TraceID != 15" <<std::endl;

    for( i = 0; i < 20; i++ )
      {
      ActorVector[i]->Delete();
      NodesVector[i]->Delete();
      }

    return EXIT_FAILURE;
    }

  std::cout <<c <<std::endl;

  unsigned int k = 0;

  for( i = 0; i < 5; i++ )
    {
    list = FindContourGivenTimePoint( container, i );

    std::list< ContourMeshStructure >::iterator c_it = list.begin();

    while( c_it != list.end() )
      {
      if( (*c_it).TraceID != k )
        {
        std::cerr <<"(*c_it).TraceID != k" <<std::endl;
        std::cerr <<(*c_it).TraceID <<" != " <<k <<std::endl;

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
