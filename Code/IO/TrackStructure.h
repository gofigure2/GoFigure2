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

#ifndef __TrackStructure_h
#define __TrackStructure_h

class vtkActor;
class vtkPolyData;
class vtkLookupTable;

#include <ostream>
#include <vector>

#include "QGoIOConfigure.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "StructureHelper.h"
#endif

/**
\defgroup Track Track
\defgroup Trace Trace
\defgroup Structure Structure
*/

/**
 * \struct TrackStructure
 * \brief  Structure which represent a track, and used for
 * interaction between Visualization and TableWidget
 * \ingroup Track Trace Structure
 */
struct QGOIO_EXPORT TrackStructure {
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
  * Polydata representing the track (a line). It also contains the temporal information.
  */
  vtkPolyData *Nodes;

  /** Is the track Highlighted in the Visualization ? */
  bool Highlighted;

  /** Is the track Visible (appears on the screen)
  * in the Visualization ?
  */
  bool Visible;

  /** color of the track. \note each component is in [0,1] */
  double rgba[4];

  /** Default Constructor */
  TrackStructure();

  /** Constructor */
  TrackStructure(const unsigned int & iTraceID,
                       std::vector< vtkActor * > iActors,
                       vtkPolyData *iNodes,
                       const bool & iHighlighted,
                       const bool & iVisible,
                       const double & r,
                       const double & g,
                       const double & b,
                       const double & alpha);

  /** Constructor */
  TrackStructure(const unsigned int & iTraceID,
                       std::vector< vtkActor * > iActors,
                       vtkPolyData *iNodes,
                       const bool & iHighlighted,
                       const bool & iVisible,
                       double iRgba[4]);

  /** Constructor */
  TrackStructure(const unsigned int & iTraceID,
                       vtkActor *iActorXY,
                       vtkActor *iActorYZ,
                       vtkActor *iActorXZ,
                       vtkActor *iActorXYZ,
                       vtkPolyData *iNodes,
                       const bool & iHighlighted,
                       const bool & iVisible,
                       const double & r,
                       const double & g,
                       const double & b,
                       const double & alpha);

  /** Constructor by copy */
  TrackStructure(const TrackStructure & iE);

  /** Destructor */
  ~TrackStructure();

  /** \brief Render with original colors (Remove the active scalars data).*/
  void RenderWithOriginalColors() const;

  /** \brief Set Scalar Data associated to the elements (use for color coding)
      \param[in] iName data name
      \param[in] iValue value
  */
  void SetScalarData( const std::string& iName, const double& iValue ) const;

  /** \brief Set the scalar range (use for color coding)
      \param[in] iMin
      \param[in] iMax */
  void SetScalarRange( const double& iMin, const double& iMax ) const;

  /** \brief Set the lookup table
      \param[in] iLut */
  void SetLookupTable( vtkLookupTable* iLut ) const;

  /** Printing one element. std::cout << element << std::endl; */
  friend std::ostream & operator<<
    (std::ostream & os, const TrackStructure & c)
  {
    os << "TraceID " << c.TraceID << std::endl;
    os << "ActorXY " << c.ActorXY << std::endl;
    os << "ActorXZ " << c.ActorXZ << std::endl;
    os << "ActorYZ " << c.ActorYZ << std::endl;
    os << "ActorXYZ " << c.ActorXYZ << std::endl;
    os << "Nodes " << c.Nodes << std::endl;
    os << "Highlighted " << c.Highlighted << std::endl;
    os << "Visible " << c.Visible << std::endl;
    os << "RGBA [" << c.rgba[0] << ", " << c.rgba[1] << ", " << c.rgba[2]
    << ", " << c.rgba[3] << "]" << std::endl;

    return os;
  }
};

#endif
