/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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

#include "TraceStructure.h"
#include "QGoIOConfigure.h"

#include "GoFigureTrackAttributes.h"

#include <map>

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "StructureHelper.h"
#endif

// TREE NODE STRUCTURE
template <class T>
class QGOIO_EXPORT TreeNodeStructure
{
  T* m_Mother;
  T* m_Child[2];
  vtkActor* DivisionActor;
};

/**
\defgroup Track Track
*/

/**
 * \struct TrackStructure
 * \brief  Structure which represent a track, and used for
 * interaction between Visualization and TableWidget
 * \ingroup Track Trace
 */
class QGOIO_EXPORT TrackStructure : public TraceStructure
{
public:

  typedef TrackStructure Self;

  // contains pointers + actor
  TreeNodeStructure<Self> TriNode;

  /**
   * Map containing all the polydata points ordered by time
   */
  typedef std::map< unsigned int, double* > PointsMapType;
  typedef PointsMapType::iterator PointsMapIterator;
  typedef PointsMapType::const_iterator PointsMapConstIterator;

  PointsMapType PointsMap;

  /** Default Constructor */
  TrackStructure();

  /** Constructor by copy */
  TrackStructure(const TrackStructure & iE);

  /** Destructor */
  ~TrackStructure();

  /**
   * \brief Insert a point at the current time point.
   * \param[in] iTime time point where we want to insert the point
   * \param[in] iPoint new point to be inserted
   * \return true is element has been inserted, false if not (i.e. there
   * is already a point associated to this time point). If you want to override
   * this point, call ReplaceElement(int iTime, double* iPoint) instead.
   */
  bool InsertElement(const unsigned int& iTime, double* iPoint);

  /**
   * \brief Delete the point at the current time point.
   * \param[in] iTime time point where we want to delete the mesh
   * \return true is element has been deleted, false if there where no point at
   * the specified time point.
   */
  bool DeleteElement(const unsigned int& iTime);

  void ReleaseData() const;

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
    os << "Map " << std::endl;

    std::map< unsigned int, double*>::const_iterator end = c.PointsMap.end();
    std::map< unsigned int, double*>::const_iterator it = c.PointsMap.begin();

    while( it != end )
      {
      os << "Time: " << it->first << std::endl;
      os << " Coordinate X: " << (it->second)[0] << std::endl;
      os << " Coordinate Y: " << (it->second)[1] << std::endl;
      os << " Coordinate Z: " << (it->second)[2] << std::endl;
      ++it;
      }

    os << "Highlighted " << c.Highlighted << std::endl;
    os << "Visible " << c.Visible << std::endl;
    os << "RGBA [" << c.rgba[0] << ", " << c.rgba[1] << ", " << c.rgba[2]
    << ", " << c.rgba[3] << "]" << std::endl;

    return os;
  }

  void UpdateTracksRepresentation( double iRadius, double iRadius2 ) const;

  GoFigureTrackAttributes ComputeAttributes() const;

};

#endif
