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
#ifndef __GoDBTrackRow_h
#define __GoDBTrackRow_h

#include "ConvertToStringHelper.h"
#include "GoDBTraceRow.h"
#include "vtkMySQLDatabase.h"
#include <string>
#include <map>

/**
\class GoDBTrackRow
\brief this class manages the map with the keys matching the fields of the
Track gofiguredatabase table and values of the map matching a row of the Track table
\ingroup DB
*/
class QGOIO_EXPORT GoDBTrackRow:public GoDBTraceRow
{
public:
  GoDBTrackRow();

  ~GoDBTrackRow()
  {}
  /**
  \brief fill the track map with the values gotten from the visualization
  \param[in] DatabaseConnector connection to the database
  \param[in] TraceVisu vtkPolyData the points will be extracted from to create 
  a string for "Points"
  \param[in] Min coordinate row for the minimum of the bounding box
  \param[in] Max coordinate row for the maximum of the bounding box
  \param[in] ImgSessionID ID of the current imagingsession
  */
  GoDBTrackRow(vtkMySQLDatabase *DatabaseConnector, vtkPolyData *TraceVisu,
    GoDBCoordinateRow Min,GoDBCoordinateRow Max, unsigned int ImgSessionID);

  GoDBTrackRow(vtkMySQLDatabase *DatabaseConnector,GoDBCoordinateRow Min,
    GoDBCoordinateRow Max, unsigned int ImgSessionID,std::string iPoints);

  /**
  \param[in] ImagingSessionID current imagingsession the track belongs to
  */
  GoDBTrackRow(unsigned int ImagingSessionID);

  //mother class method
  void SetTheDataFromTheVisu(vtkMySQLDatabase *DatabaseConnector,
                                         vtkPolyData *TrackVisu,
                                         GoDBCoordinateRow iCoordMin,
                                         GoDBCoordinateRow iCoordMax);

  /**
  \brief convert the iTrackVisu into a string and
  set the field 'points' of the map 
  \param[in] iTrackVisu vtkPolyData the points will be extracted from to create 
  a string for "Points"
  */
  void SetThePointsFromPolydata(vtkPolyData * iTrackVisu);

  /**
  \brief 
  \return the TrackID of the Track with the same bounding box
  already registered in the DB or -1 if not yet created
  */
  int DoesThisBoundingBoxTrackExist(vtkMySQLDatabase *DatabaseConnector);

  //mother class method
  virtual int SaveInDB(vtkMySQLDatabase *DatabaseConnector);

protected:
  //mother class method
  virtual void InitializeMap();

};
#endif
