#ifndef __ContourMeshStructure_h
#define __ContourMeshStructure_h

class vtkActor;
class vtkPolyData;

#include <ostream>

struct ContourMeshStructure
{
  typedef vtkActor*     vtkActorPointer;
  typedef vtkPolyData*  vtkPolyDataPointer;
  unsigned int  TraceID;
  vtkActorPointer     Actor;
  vtkPolyDataPointer  Nodes;
  unsigned int  CollectionID;
  unsigned int  TCoord;
  bool          Highlighted;
  double        rgb[3];
  int           Direction;

  ContourMeshStructure( ) : TraceID( 0 ), Actor( 0 ), Nodes( 0 ), CollectionID( 0 ),
    TCoord( 0 ), Highlighted( false ), Direction( 0 )
    {
    }

  ContourMeshStructure( const unsigned int& iTraceID, vtkActorPointer iActor,
    vtkPolyDataPointer iNodes, const unsigned int& iCollectionID, const unsigned int& iT,
    const bool& iHighlighted, const double& r, const double& g, const double& b,
    const int& iDir )
    : TraceID( iTraceID ), Actor( iActor ), Nodes( iNodes ), CollectionID( iCollectionID ),
      TCoord( iT ), Highlighted( iHighlighted ), Direction( iDir )
    {
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
    }

  ~ContourStructure()
    {}

  friend std::ostream& operator << ( std::ostream& os, const ContourStructure& c )
    {
    os <<"TraceID " <<c.ContourId <<std::endl;
    os <<"Actor " <<c.Actor <<std::endl;
    os <<"Nodes "<<c.Nodes <<std::endl;
    os <<"CollectionID " <<c.MeshId <<std::endl;
    os <<"TCoord " <<c.TCoord <<std::endl;
    os <<"Highlighted " <<c.Highlighted <<std::endl;
    os <<"RGB [" <<c.rgb[0] <<", " <<c.rgb[1] <<", " <<c.rgb[2] <<"]" <<std::endl;
    os <<"Direction " <<c.Direction <<std::endl;

    return os;
    }
};

struct TraceID {};
struct Actor {};
struct Nodes {};
struct CollectionID {};
struct TCoord {};

#endif
