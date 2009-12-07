#include <vector>

#include "vtkActor.h"
#include "vtkPolyData.h"

#include "ContourMeshStructure.h"
#include "ContourMeshStructureHelper.h"

int main( int , char** )
{
  ContourMeshStructureMultiIndexContainer container;

  std::vector< vtkActor* > ActorVector( 80 );
  std::vector< vtkPolyData* > NodesVector( 20 );

  unsigned int meshid = 0;
  bool highlighted = false;
  double r = 1.;
  double g = 0;
  double b = 0;
  double a = 0.5;

  unsigned int i;

  for( i = 0; i < ActorVector.size(); i++ )
    {
    if( i < NodesVector.size() )
      {
      NodesVector[i] = vtkPolyData::New();
      }
    ActorVector[i] = vtkActor::New();

    unsigned int timepoint = i % 12;
    container.insert( ContourMeshStructure( i, ActorVector[i],
      NodesVector[i%NodesVector.size()],
      meshid, timepoint, highlighted, r, g, b, a, i % 4 ) );
    }

  std::list< ContourMeshStructure* > list =
    FindContourGivenTraceID( container, 0 );

  if( list.empty() )
    {
    std::cerr <<"empty list after FindContourGivenTraceID" <<std::endl;
    return EXIT_FAILURE;
    }

  std::list< ContourMeshStructure* >::iterator list_it = list.begin();

  while( list_it != list.end() )
    {
    if( (*list_it)->TraceID != 0 )
      {
      std::cerr <<"(*list_it)->TraceID != 0" <<std::endl;
      return EXIT_FAILURE;
      }
    ++list_it;
    }

  ContourMeshStructure* c = FindContourGivenActor( container, ActorVector[2] );

  if( c->TraceID != 2 )
    {
    std::cerr <<"c->TraceID != 2" <<std::endl;
    for( i = 0; i < ActorVector.size(); i++ )
      {
      if( i < NodesVector.size() )
        {
        NodesVector[i]->Delete();
        }
      ActorVector[i]->Delete();
      }
    }

  list = FindContourGivenNodes( container, NodesVector.front() );

  if( list.empty() )
    {
    std::cerr <<"empty list after FindContourGivenNodes" <<std::endl;
    return EXIT_FAILURE;
    }

  list_it = list.begin();
  while( list_it != list.end() )
    {
    c = *list_it;
    if( c->Nodes != NodesVector.front() )
      {
      std::cerr <<c->Nodes <<" != " <<NodesVector.front() <<std::endl;
      std::cerr <<c->TraceID <<std::endl;

      for( i = 0; i < ActorVector.size(); i++ )
        {
        if( i < NodesVector.size() )
          {
          NodesVector[i]->Delete();
          }
        ActorVector[i]->Delete();
        }

      return EXIT_FAILURE;
      }
    ++list_it;
    }

  unsigned int k = 0;

  for( unsigned int t = 0; t < 3; t++ )
    {
    list = FindContourGivenTimePoint( container, t );

    if( list.empty() )
      {
      std::cerr <<"empty list after FindContourGivenTimePoint" <<std::endl;
      return EXIT_FAILURE;
      }

    list_it = list.begin();

    std::cout <<"*** FindContourGivenTimePoint( container, " <<t <<" ) *** "
      <<std::endl;

    while( list_it != list.end() )
      {
      std::cout << *(*list_it) << std::endl;

      if( (*list_it)->TCoord != t )
        {
        std::cerr <<"(*list_it)->TCoord != t" <<std::endl;
        std::cerr <<(*list_it)->TCoord <<" != " <<t <<std::endl;

        for( i = 0; i < ActorVector.size(); i++ )
          {
          if( i < NodesVector.size() )
            {
            NodesVector[i]->Delete();
            }
          ActorVector[i]->Delete();
          }

        return EXIT_FAILURE;
        }
      ++list_it;
      ++k;
      }
    }

  for( i = 0; i < ActorVector.size(); i++ )
    {
    if( i < NodesVector.size() )
      {
      NodesVector[i]->Delete();
      }
    ActorVector[i]->Delete();
    }


  return EXIT_SUCCESS;
}
