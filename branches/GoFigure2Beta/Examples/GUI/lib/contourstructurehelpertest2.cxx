#include <vector>

#include "vtkActor.h"
#include "vtkPolyData.h"

#include "ContourStructure.h"
#include "ContourStructureHelper.h"

int main( int , char** )
{
  ContourStructureMultiIndexContainer container;

  std::vector< vtkActor* > ActorVector( 12 );
  std::vector< vtkPolyData* > NodesVector( 4 );

  unsigned int meshid = 0;
  unsigned int timepoint = 0;
  bool highlighted = false;
  double r = 1.;
  double g = 0;
  double b = 0;
  
  unsigned int i;

  for( i = 0; i < ActorVector.size(); i++ )
    {
    if( i < NodesVector.size() )
      {
      NodesVector[i] = vtkPolyData::New();
      }
    ActorVector[i] = vtkActor::New();

    container.insert( ContourStructure( i, ActorVector[i], 
      NodesVector[i%NodesVector.size()],
      meshid, timepoint, highlighted, r, g, b, i % 4 ) );
    }

  std::list< ContourStructure > list = 
    FindContourGivenContourId( container, 0 );

  if( list.empty() )
    {
    std::cerr <<"empty list after FindContourGivenContourId" <<std::endl;
    return EXIT_FAILURE;
    }
  
  std::list< ContourStructure >::iterator list_it = list.begin();
  ContourStructure c;

  while( list_it != list.end() )
    {
    if( (*list_it).ContourId != 0 )
      {
      std::cerr <<"(*list_it).ContourId != 0" <<std::endl;
      return EXIT_FAILURE;
      }
    ++list_it;
    }
  
  c = FindContourGivenActor( container, ActorVector[2] );

  if( c.ContourId != 2 )
    {
    std::cerr <<"c.ContourId != 2" <<std::endl;
    for( i = 0; i < ActorVector.size(); i++ )
      {
      if( i < NodesVector.size() )
        {
        NodesVector[i]->Delete();
        }
      ActorVector[i]->Delete();
      }
    }

  list = FindContourGivenNodes( container, NodesVector.front());

  if( list.empty() )
    {
    std::cerr <<"empty list after FindContourGivenNodes" <<std::endl;
    return EXIT_FAILURE;
    }

  list_it = list.begin();
  while( list_it != list.end() )
    {
    c = *list_it;
    if( c.Nodes != NodesVector.front() )
      {
      std::cerr <<c.Nodes <<" != " <<NodesVector.front() <<std::endl;
      std::cerr <<c.ContourId <<std::endl;

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

  list = FindContourGivenTimePoint( container, 0 );

  if( list.empty() )
    {
    std::cerr <<"empty list after FindContourGivenTimePoint" <<std::endl;
    return EXIT_FAILURE;
    }

  list_it = list.begin();

  while( list_it != list.end() )
    {
    if( (*list_it).TCoord != 0 )
      {
      std::cerr <<"(*list_it).ContourId != k" <<std::endl;
      std::cerr <<(*list_it).ContourId <<" != " <<k <<std::endl;
      
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

  std::cout <<c <<std::endl;

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
