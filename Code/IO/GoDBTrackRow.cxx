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
#include "GoDBTrackRow.h"
#include "SelectQueryDatabaseHelper.h"
#include "GoDBRecordSetHelper.h"
#include "vtkPolyDataMySQLTrackWriter.h"
#include <iostream>

GoDBTrackRow::GoDBTrackRow():GoDBTraceRow()
{
  this->InitializeMap();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBTrackRow::GoDBTrackRow(vtkMySQLDatabase *DatabaseConnector,
                           GoDBCoordinateRow Min, GoDBCoordinateRow Max, 
                           unsigned int ImgSessionID,
                           std::string TraceVisu):GoDBTraceRow()
  //GoDBTraceRow(DatabaseConnector, TraceVisu, Min, Max, ImgSessionID)
{
  this->InitializeMap();
  this->SetCollectionID(ImgSessionID);
  this->SetTheBoundingBox(DatabaseConnector,Min,Max);
  this->SetField("Points", TraceVisu);

  cout << "Track ID found: " << this->DoesThisBoundingBoxTrackExist(DatabaseConnector) << endl;

  if ( this->DoesThisBoundingBoxTrackExist(DatabaseConnector) )
    {
    std::cout << "The bounding box already exists for this track" << std::endl;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
  GoDBTrackRow::GoDBTrackRow(unsigned int ImagingSessionID):
  GoDBTraceRow()
{
  this->InitializeMap();
  this->SetCollectionID(ImagingSessionID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
  GoDBTrackRow::GoDBTrackRow(vtkMySQLDatabase *DatabaseConnector, 
    vtkPolyData *TraceVisu,GoDBCoordinateRow Min,GoDBCoordinateRow Max, 
    unsigned int ImgSessionID):GoDBTraceRow()
{
  this->InitializeMap();
  this->SetCollectionID(ImgSessionID);
  this->SetTheDataFromTheVisu(DatabaseConnector,TraceVisu,Min,Max);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBTrackRow::InitializeMap()
{
  //GoDBTraceRow::InitializeMap();
  //this->m_MapRow["TrackID"] = ConvertToString<int>(0);
  this->m_TableName = "track";
  this->m_TableIDName = "trackID";
  this->m_CollectionName = "lineage";
  this->m_CollectionIDName = "lineageID";
  this->m_MapRow[this->m_TableIDName] = ConvertToString< int >(0);
  this->m_MapRow["lineageID"] = ConvertToString< int >(0);
  this->m_MapRow["TrackFamilyID"] = ConvertToString< int >(0);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBTrackRow::DoesThisBoundingBoxTrackExist(
  vtkMySQLDatabase *DatabaseConnector)
{
  return FindOneID( DatabaseConnector, "track", "trackID",
                    "CoordIDMax", this->GetMapValue("CoordIDMax"),
                    "CoordIDMin", this->GetMapValue("CoordIDMin") );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBTrackRow::SaveInDB(vtkMySQLDatabase *DatabaseConnector)
{
  return this->SaveInDBTemplate< GoDBTrackRow >(DatabaseConnector, *this);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBTrackRow::SetTheDataFromTheVisu(vtkMySQLDatabase *DatabaseConnector,
                                         vtkPolyData *TrackVisu,
                                         GoDBCoordinateRow iCoordMin,
                                         GoDBCoordinateRow iCoordMax)
{
  this->SetTheBoundingBox(DatabaseConnector, iCoordMin, iCoordMax);

  this->SetThePointsFromPolydata(TrackVisu);


  if ( this->DoesThisBoundingBoxExist(DatabaseConnector) )
    {
    std::cout << "The bounding box already exists for this mesh" << std::endl;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBTrackRow::SetThePointsFromPolydata(vtkPolyData * iTrackVisu)
{
  //vtkSmartPointer< vtkPolyDataMySQLTrackWriter > convert =
    //vtkSmartPointer< vtkPolyDataMySQLTrackWriter >::New();
  vtkPolyDataMySQLTextWriter *convert = vtkPolyDataMySQLTextWriter::New();
  std::string PointsString = convert->GetMySQLText(iTrackVisu);
  convert->Delete();
  this->SetField("Points", PointsString);
}