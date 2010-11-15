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
class vtkProperty;

#include <ostream>
#include <vector>

#include "QGoIOConfigure.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "StructureHelper.h"
#endif

/**
\defgroup Contour Contour
\defgroup Mesh Mesh
\defgroup Trace Trace
\defgroup Structure Structure
*/


/**
 * \struct ContourMeshStructure
 * \brief  Structure which represent a contour or a mesh, and used for
 * interaction between Visualization and TableWidget
 * \ingroup Contour Mesh Trace Structure
 *
 * \sa ContourMeshContainer
 */
struct QGOIO_EXPORT ContourMeshStructure {
  /** TraceID */
  unsigned int TraceID;

  /** Actor in the XY View */
  vtkActor *ActorXY;

  /** Actor in the XZ View */
  vtkActor *ActorXZ;

  /** Actor in the YZ View */
  vtkActor *ActorYZ;

  /** Actor in the XYZ View */
  vtkActor *ActorXYZ;

  /**
  * In the case of contours, Nodes represent the control points.
  *   In the case of meshes, Nodes represent the triangular mesh.
  */
  vtkPolyData *Nodes;

  //unsigned int CollectionID;

  /** Time point of the contour / mesh */
  unsigned int TCoord;

  /** Is the contour / mesh Highlighted in the Visualization ? */
  bool Highlighted;

  /** Is the contour / mesh Visible (appears on the screen)
  * in the Visualization ?
  */
  bool Visible;

  /** color of the contour / mesh. \note each component is in [0,1] */
  double rgba[4];

  /**
  \brief Return the direction of a given contour iContour
  \return 0 if z coordinates are constant
  \return 1 if y coordinates are constant
  \return 2 if x coordinates are constant
  \return -1 else
  */
  int GetDirection();

  /**
  \brief Is it a contour or a mesh ?
  \return true if it is a contour
  \return false else
  */
  bool IsAContour();

  /** Default Constructor */
  ContourMeshStructure();

  /** Constructor */
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

  /** Constructor */
  ContourMeshStructure(const unsigned int & iTraceID,
                       std::vector< vtkActor * > iActors,
                       vtkPolyData *iNodes,
                       const unsigned int & iT,
                       const bool & iHighlighted,
                       const bool & iVisible,
                       double iRgba[4]);

  /** Constructor */
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

  /** Constructor by copy */
  ContourMeshStructure(const ContourMeshStructure & iE);

  /** Destructor */
  ~ContourMeshStructure();

  /** \brief Set Property for all actors
      \param[in] iProperty */
  void SetActorProperties( vtkProperty* iProperty ) const;

  /** \brief Set Visibility for all actors
      \param[in] iVisible */
  void SetActorVisibility( const bool& iVisible ) const;

  void SetScalarData( const std::string& iName, const double& iValue ) const;
  void SetScalarRange( const double& iMin, const double& iMax ) const;
  void RenderWithOriginalColors() const;

  /** Printing one element. std::cout << element << std::endl; */
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
#endif
