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

#include "TrackStructure.h"

#include <iostream>
#include "vtkPolyData.h"
#include "vtkActor.h"

//--------------------------------------------------------------------------
TrackStructure::
TrackStructure():TraceStructure()
{
}

//--------------------------------------------------------------------------
/*
//--------------------------------------------------------------------------
TrackStructure::
TrackStructure(const unsigned int & iTraceID,
                                           std::vector< vtkActor * > iActors,
                                           vtkPolyData *iNodes,
                                           const bool & iHighlighted,
                                           const bool & iVisible,
                                           const double & r,
                                           const double & g,
                                           const double & b,
                                           const double & alpha):
  TraceID(iTraceID), Nodes(iNodes), Highlighted(iHighlighted), Visible(iVisible)
{
  if ( iActors.size() == 4 )
    {
    ActorXY = iActors[0];
    ActorXZ = iActors[1];
    ActorYZ = iActors[2];
    ActorXYZ = iActors[3];
    }
  else
    {
    std::cerr << "iActors.size() != 4" << std::endl;
    return;
    }
  this->rgba[0] = r;
  this->rgba[1] = g;
  this->rgba[2] = b;
  this->rgba[3] = alpha;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
TrackStructure::
TrackStructure(const unsigned int & iTraceID,
                                           std::vector< vtkActor * > iActors,
                                           vtkPolyData *iNodes,
                                           const bool & iHighlighted,
                                           const bool & iVisible,
                                           double iRgba[4]):
  TraceID(iTraceID), Nodes(iNodes), Highlighted(iHighlighted), Visible(iVisible)
{
  if ( iActors.size() == 4 )
    {
    ActorXY = iActors[0];
    ActorXZ = iActors[1];
    ActorYZ = iActors[2];
    ActorXYZ = iActors[3];
    }
  else
    {
    std::cout << "iActors.size() != 4" << std::endl;
    return;
    }
  this->rgba[0] = iRgba[0];
  this->rgba[1] = iRgba[1];
  this->rgba[2] = iRgba[2];
  this->rgba[3] = iRgba[3];
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
TrackStructure::
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
                                           const double & alpha):
  TraceID(iTraceID), ActorXY(iActorXY), ActorXZ(iActorXZ),
  ActorYZ(iActorYZ), ActorXYZ(iActorXYZ), Nodes(iNodes),
  Highlighted(iHighlighted), Visible(iVisible)
{
  this->rgba[0] = r;
  this->rgba[1] = g;
  this->rgba[2] = b;
  this->rgba[3] = alpha;
}

//--------------------------------------------------------------------------
*/
//--------------------------------------------------------------------------
TrackStructure::
TrackStructure(const TrackStructure & iE):
  TraceStructure( iE ), PointsMap(iE.PointsMap)
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
TrackStructure::
~TrackStructure()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool
TrackStructure::
InsertElement(int iTime, double* iPoint)
{
  // check if there is something at the iTime time point
  PointsMapIterator pointsMapIterator = this->PointsMap.find(iTime);

  // if there is no point, insert it and return true
  if ( pointsMapIterator == this->PointsMap.end() )
    {
    this->PointsMap.insert( std::pair< unsigned int,double*>(iTime, iPoint) );
    return true;
    }

  // else do nothing and return false
  std::cout << "in: " << __FILE__ << " at line: " << __LINE__ << std::endl;
  std::cout << "can't insert a point at this time point" << std::endl;
  return false;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool
TrackStructure::
DeleteElement(int iTime)
{
  // check if there is something at the iTime time point
  PointsMapIterator pointsMapIterator = this->PointsMap.find(iTime);

  // if there is a point, delete it and return true
  if ( pointsMapIterator != this->PointsMap.end() )
    {
    // free memory
    delete[] pointsMapIterator->second;
    // clear map
    this->PointsMap.erase(pointsMapIterator);
    return true;
    }

  // else do nothing and return false
  std::cout << "in: " << __FILE__ << " at line: " << __LINE__ << std::endl;
  std::cout << "can't delete a point at this time point" << std::endl;
  return false;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
bool
TrackStructure::
ReplaceElement(int iTime, double* iPoint)
{
  // delete the existing element
  bool deleteElement = DeleteElement(iTime);

  // if sth has been deleted, insert the point and return true
  if(deleteElement)
    {
    return InsertElement(iTime,iPoint);
    }

  // else do nothing and return false
  std::cout << "in: " << __FILE__ << " at line: " << __LINE__ << std::endl;
  std::cout << "can't replace a point at this time point" << std::endl;
  return deleteElement;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
TrackStructure::ReleaseData()
{
  TraceStructure::ReleaseData();
}
