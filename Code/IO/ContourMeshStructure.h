/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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

#ifndef __ContourMeshStructure_h
#define __ContourMeshStructure_h

class vtkActor;
class vtkPolyData;

#include <ostream>
#include <vector>

#include "QGoIOConfigure.h"

struct QGOIO_EXPORT ContourMeshStructure {
  unsigned int TraceID;
  vtkActor *ActorXY;
  vtkActor *ActorXZ;
  vtkActor *ActorYZ;
  vtkActor *ActorXYZ;

  vtkPolyData *Nodes;

  //unsigned int CollectionID;
  unsigned int TCoord;
  bool Highlighted;
  bool Visible;
  double rgba[4];

  /**
  \brief Return the direction of a given contour iContour
  \return 0 if z coordinates are constant
  \return 1 if y coordinates are constant
  \return 2 if x coordinates are constant
  \return -1 else
  */
  int GetDirection();

  bool IsAContour();

  ContourMeshStructure();

  ContourMeshStructure(const unsigned int & iTraceID,
                       std::vector< vtkActor * > iActors,
                       vtkPolyData *iNodes,
                       const unsigned int & iT,
                       const bool & iHighlighted,
                       const bool & iVisible,
                       const double & r,
                       const double & g,
                       const double & b,
                       const double & alpha);

  ContourMeshStructure(const unsigned int & iTraceID,
                       std::vector< vtkActor * > iActors,
                       vtkPolyData *iNodes,
                       const unsigned int & iT,
                       const bool & iHighlighted,
                       const bool & iVisible,
                       double iRgba[4]);

  ContourMeshStructure(const unsigned int & iTraceID,
                       vtkActor *iActorXY,
                       vtkActor *iActorYZ,
                       vtkActor *iActorXZ,
                       vtkActor *iActorXYZ,
                       vtkPolyData *iNodes,
                       const unsigned int & iT,
                       const bool & iHighlighted,
                       const bool & iVisible,
                       const double & r,
                       const double & g,
                       const double & b,
                       const double & alpha);

  ContourMeshStructure(const ContourMeshStructure & iE);
  ~ContourMeshStructure();

  friend std::ostream & operator<<
    (std::ostream & os, const ContourMeshStructure & c)
  {
    os << "TraceID " << c.TraceID << std::endl;
    os << "ActorXY " << c.ActorXY << std::endl;
    os << "ActorXZ " << c.ActorXZ << std::endl;
    os << "ActorYZ " << c.ActorYZ << std::endl;
    os << "ActorXYZ " << c.ActorXYZ << std::endl;
    os << "Nodes " << c.Nodes << std::endl;
    //os << "CollectionID " << c.CollectionID << std::endl;
    os << "TCoord " << c.TCoord << std::endl;
    os << "Highlighted " << c.Highlighted << std::endl;
    os << "Visible " << c.Visible << std::endl;
    os << "RGBA [" << c.rgba[0] << ", " << c.rgba[1] << ", " << c.rgba[2]
    << ", " << c.rgba[3] << "]" << std::endl;

    return os;
  }
};

#ifndef DOXYGEN_SHOULD_SKIP_THIS
struct TraceID {};
struct ActorXY {};
struct ActorXZ {};
struct ActorYZ {};
struct ActorXYZ {};
struct Nodes {};
struct TCoord {};
struct Highlighted {};
struct Visible {};
#endif

#endif
