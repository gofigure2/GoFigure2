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

//--------------------------------------------------------------------------
TrackStructure::TrackStructure():TraceID(0),
  ActorXY(NULL), ActorXZ(NULL), ActorYZ(NULL), ActorXYZ(NULL),
  TCoord(0), Highlighted(false), Visible(true)
{
  this->rgba[0] = 1.;
  this->rgba[1] = 1.;
  this->rgba[2] = 1.;
  this->rgba[3] = 1.;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
TrackStructure::TrackStructure(const unsigned int & iTraceID,
                                           std::vector< vtkActor * > iActors,
                                           const unsigned int & iT,
                                           const bool & iHighlighted,
                                           const bool & iVisible,
                                           const double & r,
                                           const double & g,
                                           const double & b,
                                           const double & alpha):
  TraceID(iTraceID),
  TCoord(iT), Highlighted(iHighlighted), Visible(iVisible)
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
TrackStructure::TrackStructure(const unsigned int & iTraceID,
                                           std::vector< vtkActor * > iActors,
                                           const unsigned int & iT,
                                           const bool & iHighlighted,
                                           const bool & iVisible,
                                           double iRgba[4]):
  TraceID(iTraceID),   //CollectionID(iCollectionID),
  TCoord(iT), Highlighted(iHighlighted), Visible(iVisible)
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
TrackStructure::TrackStructure(const unsigned int & iTraceID,
                                           vtkActor *iActorXY,
                                           vtkActor *iActorYZ,
                                           vtkActor *iActorXZ,
                                           vtkActor *iActorXYZ,
                                           const unsigned int & iT,
                                           const bool & iHighlighted,
                                           const bool & iVisible,
                                           const double & r,
                                           const double & g,
                                           const double & b,
                                           const double & alpha):
  TraceID(iTraceID), ActorXY(iActorXY), ActorXZ(iActorXZ),
  ActorYZ(iActorYZ), ActorXYZ(iActorXYZ),
  TCoord(iT), Highlighted(iHighlighted), Visible(iVisible)
{
  this->rgba[0] = r;
  this->rgba[1] = g;
  this->rgba[2] = b;
  this->rgba[3] = alpha;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
TrackStructure::TrackStructure(const TrackStructure & iE):
  TraceID(iE.TraceID), ActorXY(iE.ActorXY), ActorXZ(iE.ActorXZ),
  ActorYZ(iE.ActorYZ), ActorXYZ(iE.ActorXYZ),
  TCoord(iE.TCoord), Highlighted(iE.Highlighted), Visible(iE.Visible)
{
  for ( int i = 0; i < 4; i++ )
    {
    this->rgba[i] = iE.rgba[i];
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
TrackStructure::~TrackStructure()
{}

//--------------------------------------------------------------------------
