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
  double        rgba[4];
  int           Direction;

  ContourMeshStructure( ) : TraceID( 0 ), Actor( 0 ), Nodes( 0 ), CollectionID( 0 ),
    TCoord( 0 ), Highlighted( false ), Direction( 0 )
    {
    }

  ContourMeshStructure( const unsigned int& iTraceID, vtkActorPointer iActor,
    vtkPolyDataPointer iNodes, const unsigned int& iCollectionID, const unsigned int& iT,
    const bool& iHighlighted, const double& r, const double& g, const double& b,
    const double& alpha, const int& iDir )
    : TraceID( iTraceID ), Actor( iActor ), Nodes( iNodes ), CollectionID( iCollectionID ),
      TCoord( iT ), Highlighted( iHighlighted ), Direction( iDir )
    {
    rgba[0] = r;
    rgba[1] = g;
    rgba[2] = b;
    rgba[3] = alpha;
    }

  ~ContourMeshStructure()
    {}

  friend std::ostream& operator << ( std::ostream& os, const ContourMeshStructure& c )
    {
    os <<"TraceID " <<c.TraceID <<std::endl;
    os <<"Actor " <<c.Actor <<std::endl;
    os <<"Nodes "<<c.Nodes <<std::endl;
    os <<"CollectionID " <<c.CollectionID <<std::endl;
    os <<"TCoord " <<c.TCoord <<std::endl;
    os <<"Highlighted " <<c.Highlighted <<std::endl;
    os <<"RGBA [" <<c.rgba[0] <<", " <<c.rgba[1] <<", " <<c.rgba[2]
       <<", " <<c.rgba[3] <<"]" <<std::endl;
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
