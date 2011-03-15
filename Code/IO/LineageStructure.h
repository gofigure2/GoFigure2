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

#ifndef __LineageStructure_h
#define __LineageStructure_h

#include "TraceStructure.h"
#include "QGoIOConfigure.h"

#include "GoFigureTrackAttributes.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "StructureHelper.h"
#endif

/**
\defgroup Lineage Lineage
*/

/**
 * \struct LineageStructure
 * \brief  Structure which represent a lineage, and used for
 * interaction between Visualization and TableWidget
 * \ingroup Lineage Trace
 */
class QGOIO_EXPORT LineageStructure : public TraceStructure
{
public:

  typedef LineageStructure Self;

  /**
   * \brief Default Constructor
   * */
  LineageStructure();

  /** Destructor */
  ~LineageStructure();

  // Add daughters to the current node. 1 or 2
  // Arnaud: Note implemented!
  void ConnectTracks( 
    unsigned int iD1TrackID, vtkPolyData* iD1PolyData,
    unsigned int iD2TrackID = 0, vtkPolyData* iD2PolyData = NULL );

  // Arnaud: const?
  void SetRootID( unsigned int iRoot)
  {
    m_RootID = iRoot;
  }
  // Arnaud: const?
  void SetMotherID(unsigned int iMother)
  {
    TraceID = iMother;
  }

  // Arnaud: const?
  void SetDaughter1ID(unsigned int iDaughter1)
  {
    m_Daughter1 = iDaughter1;
  }

  // Arnaud: const?
  void SetDaughter2ID(unsigned int iDaughter2)
  {
    m_Daughter2 = iDaughter2;
  }

  /*
   * \brief Does the current structure has daughters
   * \return true or false
   * Arnaud: Rename bool IsLeaf() const
   */
  bool HasDaughters();

  /*
   * \brief Cut the current lineage at the current node. 2 new lineages will be
   * created with the daughters. New lineage means:
   * -new root lineage ID
   * -Root = true
   * Update the lineages polydatas.
   * \param[in] iNewLineageID1 ID of the first new lineage
   * \param[in] iNewLineageID2 ID of the second new lineage
   * \param[in] iRootPolyData Polydata representing the root track.
   * Required if we cut a track at the root level
   */
  // Need root polydata if we cut just after the first: nodes doesnt contain real polydata for root
  void CutLineage( 
    unsigned int iNewLineageID1, unsigned int iNewLineageID2,
    vtkPolyData* iRootPolyData = NULL);

  /*
   * \brief Update the root element polydata using vtkAppend. Required since the
   * root is the only relevant element (polydata+lineageID) to represent a lineage.
   */
  void UpdateRootPolyData();

  /*
   * \brief Useful variable to sort in the container. true if is Root.
   */
  bool              Root;

private:
  // only lineage ID of root is relevant as well??
  // Arnaud why using unsigned int? what about pointers?
  // Arnaud: what is m_RootID? Her mother? the root?
  unsigned int             m_RootID;

  // Arnaud: Rename m_Children? Why not using an array?
  unsigned int             m_Daughter1;
  unsigned int             m_Daughter2;

  /** Printing one element. std::cout << element << std::endl; */
  friend std::ostream & operator<<
    (std::ostream & os, const LineageStructure & c)
  {
    os << "TraceID " << c.TraceID << std::endl;
    os << "Root " << c.Root << std::endl;
    os << "m_RootID " << c.m_RootID << std::endl;
    os << "m_Daughter1 " << c.m_Daughter1 << std::endl;
    os << "m_Daughter2 " << c.m_Daughter2 << std::endl;
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
