#include <vector>

#include "vtkActor.h"
#include "vtkPolyData.h"

#include "ContourStructure.h"
#include "ContourStructureHelper.h"

int main( int argc, char** argv )
{
  ContourStructureMultiIndexContainer container;

  std::vector< vtkActor* > ActorVector( 20 );
  std::vector< vtkPolyData* > NodesVector( 20 );
  unsigned int id = 0;
  unsigned int t = 0;

  for( unsigned int i = 0; i < 20; i++ )
    {
    ActorVector[i] = vtkActor::New();
    NodesVector[i] = vtkPolyData::New();

    id = i / 4;
    container.insert( ContourStructure( i, ActorVector[i], NodesVector[i],
      id, t, ( i == 10 ), 0.5, 0.5, 0.5 ) );
    }

  ContourStructure c = FindContourGivenContourId( container, 10 );
  if( c.Actor != ActorVector[10]  )
    {
    std::cerr <<"c.Actor != ActorVector[10]" <<std::endl;
    return EXIT_FAILURE;
    }
  if( c.Nodes != NodesVector[10] )
    {
    std::cerr <<"c.Nodes != NodesVector[10]" <<std::endl;
    return EXIT_FAILURE;
    }
  if( c.Highlighted != true )
    {
    std::cerr <<"c.Highlighted != true" <<std::endl;
    return EXIT_FAILURE;
    }

  c.Highlighted = false;

  c = FindContourGivenActor( container, ActorVector[5] );

  if( c.ContourId != 5 )
    {
    std::cerr <<"c.ContourId != 5" <<std::endl;
    return EXIT_FAILURE;
    }

  c = FindContourGivenNodes( container, NodesVector[15] );

  if( c.ContourId != 15 )
    {
    std::cerr <<"c.ContourId != 15" <<std::endl;
    return EXIT_FAILURE;
    }

  for( int i = 0; i < 20; i++ )
    {
    ActorVector[i]->Delete();
    NodesVector[i]->Delete();
    }
  return EXIT_SUCCESS;
}
