#ifndef __ContourStructure_h
#define __ContourStructure_h

class vtkActor;
class vtkPolyData;

struct ContourStructure
{
  typedef vtkActor*     vtkActorPointer;
  typedef vtkPolyData*  vtkPolyDataPointer;
  unsigned int  ContourId;
  vtkActorPointer     Actor;
  vtkPolyDataPointer  Nodes;
  unsigned int  MeshId;
  unsigned int  TCoord;
  bool          Highlighted;
  double        rgb[3];
  int           Direction;

  ContourStructure( ) : ContourId( 0 ), Actor( 0 ), Nodes( 0 ), MeshId( 0 ),
    TCoord( 0 ), Highlighted( false ), Direction( 0 )
    {
    }

  ContourStructure( const unsigned int& iContourId, vtkActorPointer iActor,
    vtkPolyDataPointer iNodes, const unsigned int& iMeshId, const unsigned int& iT,
    const bool& iHighlighted, const double& r, const double& g, const double& b,
    const int& iDir )
    : ContourId( iContourId ), Actor( iActor ), Nodes( iNodes ), MeshId( iMeshId ),
      TCoord( iT ), Highlighted( iHighlighted ), Direction( iDir )
    {
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
    }

  ~ContourStructure()
    {}
};

struct ContourId {};
struct Actor {};
struct Nodes {};
struct MeshId {};
struct TCoord {};

#endif
