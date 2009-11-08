#include <vector>

#include "vtkActor.h"
#include "vtkPolyData.h"

#include "ContourStructure.h"
#include "ContourStructureHelper.h"

int main( int , char** )
{
  ContourStructureMultiIndexContainer container;

  std::vector< vtkActor* > ActorVector( 9 );
  std::vector< vtkPolyData* > NodesVector( 3 );

  unsigned int meshid = 0;
  unsigned int timepoint = 0;
  bool highlighted = false;
  double r = 1.;
  double g = 0;
  double b = 0;
  
  unsigned int i;

  for( i = 0; i < 9; i++ )
    {
    if( i < 3 )
      {
      NodesVector[i] = vtkPolyData::New();
      }
    ActorVector[i] = vtkActor::New();

    container.insert( ContourStructure( i, ActorVector[i], NodesVector[i%3],
      meshid, timepoint, highlighted, r, g, b, i ) );
    }

  std::list< ContourStructure > list = FindContourGivenContourId( container, 0 );
  ContourStructure c = list.front();
  std::cout <<c <<std::endl;

  if( c.Actor != ActorVector[0]  )
    {
    std::cerr <<"c.Actor != ActorVector[0]" <<std::endl;
    for( i = 0; i < 9; i++ )
      {
      if( i < 3 )
        {
        NodesVector[i]->Delete();
        }
      ActorVector[i]->Delete();
      }
    return EXIT_FAILURE;
    }
  if( c.Nodes != NodesVector.front() )
    {
    std::cerr <<"c.Nodes != NodesVector.front()" <<std::endl;
    for( i = 0; i < 9; i++ )
      {
      if( i < 3 )
        {
        NodesVector[i]->Delete();
        }
      ActorVector[i]->Delete();
      }
    return EXIT_FAILURE;
    }
  if( c.Highlighted != false )
    {
    std::cerr <<"c.Highlighted != false" <<std::endl;
    for( i = 0; i < 9; i++ )
      {
      if( i < 3 )
        {
        NodesVector[i]->Delete();
        }
      ActorVector[i]->Delete();
      }
    return EXIT_FAILURE;
    }

  c.Highlighted = false;

  c = FindContourGivenActor( container, ActorVector[2] );

  if( c.ContourId != 2 )
    {
    std::cerr <<"c.ContourId != 2" <<std::endl;
    for( i = 0; i < 9; i++ )
      {
      if( i < 3 )
        {
        NodesVector[i]->Delete();
        }
      ActorVector[i]->Delete();
      }
    }

  list = FindContourGivenNodes( container, NodesVector.front());

  std::list< ContourStructure >::iterator list_it = list.begin();
  for( i = 0; i < 3; i++ )
    {
    c = *list_it;
    if( c.ContourId != 3*i )
      {
      std::cerr <<"c.ContourId !=" <<i <<std::endl;
      std::cerr <<c.ContourId <<std::endl;

      for( i = 0; i < 9; i++ )
        {
        if( i < 3 )
          {
          NodesVector[i]->Delete();
          }
        ActorVector[i]->Delete();
        }

      return EXIT_FAILURE;
      }
    ++list_it;
    }

  std::cout <<c <<std::endl;

  unsigned int k = 0;

  list = FindContourGivenTimePoint( container, 0 );

  list_it = list.begin();

  while( list_it != list.end() )
    {
    if( (*list_it).ContourId != k )
      {
      std::cerr <<"(*list_it).ContourId != k" <<std::endl;
      std::cerr <<(*list_it).ContourId <<" != " <<k <<std::endl;
      
      for( i = 0; i < 9; i++ )
        {
        if( i < 3 )
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

  for( i = 0; i < 9; i++ )
    {
    if( i < 3 )
      {
      NodesVector[i]->Delete();
      }
    ActorVector[i]->Delete();
    }


  return EXIT_SUCCESS;
}
