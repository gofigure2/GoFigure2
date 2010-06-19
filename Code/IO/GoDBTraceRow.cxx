/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

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
#include "GoDBTraceRow.h"
#include "GoDBColorRow.h"
#include "SelectQueryDatabaseHelper.h"
#include "GoDBRecordSetHelper.h"
#include "vtkSmartPointer.h"

GoDBTraceRow::GoDBTraceRow()
{
  this->InitializeMap();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBTraceRow::GoDBTraceRow(vtkMySQLDatabase* DatabaseConnector,
  vtkPolyData* TraceVisu,GoDBCoordinateRow Min, GoDBCoordinateRow Max,
  unsigned int ImgSessionID)
{
  this->InitializeMap();
  this->CreateBoundingBox(DatabaseConnector,Min,Max);
  this->m_MapRow["ImagingSessionID"] =
    ConvertToString<unsigned int>(ImgSessionID);
  vtkSmartPointer<vtkPolyDataMySQLTextWriter> convert =
    vtkSmartPointer<vtkPolyDataMySQLTextWriter>::New();
  std::string PointsString = convert->GetMySQLText(TraceVisu);

  this->SetField("Points",PointsString);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBTraceRow::GoDBTraceRow(vtkMySQLDatabase* DatabaseConnector,
  std::string TraceVisu,GoDBCoordinateRow Min, GoDBCoordinateRow Max,
  unsigned int ImgSessionID)
{
  this->InitializeMap();
  this->CreateBoundingBox(DatabaseConnector,Min,Max);
  this->m_MapRow["ImagingSessionID"] =
    ConvertToString<unsigned int>(ImgSessionID);

  this->SetField("Points",TraceVisu);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBTraceRow::InitializeMap()
{ 
  this->m_MapRow["ImagingSessionID"] = ConvertToString<int>(0);
  this->m_MapRow["ColorID"] = ConvertToString<int>(1);
  this->m_MapRow["CoordIDMax"] = ConvertToString<int>(0);
  this->m_MapRow["CoordIDMin"] = ConvertToString<int>(0);
  //this->m_MapRow["Points"] = "NULL";
  this->m_MapRow["Points"] = ConvertToString<int>(0);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBTraceRow::CreateBoundingBox(vtkMySQLDatabase* DatabaseConnector,
  GoDBCoordinateRow Min,GoDBCoordinateRow Max)
{
  int CoordMin = Min.DoesThisCoordinateExist(DatabaseConnector);
  if (CoordMin == -1)
    {
    CoordMin = Min.SaveInDB(DatabaseConnector);
    }
  this->m_MapRow["CoordIDMin"] = ConvertToString<int>(CoordMin);

  int CoordMax = Max.DoesThisCoordinateExist(DatabaseConnector);
  if (CoordMax == -1)
    {
    CoordMax = Max.SaveInDB(DatabaseConnector);
    }
  this->m_MapRow["CoordIDMax"] = ConvertToString<int>(CoordMax);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBTraceRow::DoesThisBoundingBoxExist(
  vtkMySQLDatabase* DatabaseConnector)
{
  return FindOneID(DatabaseConnector,this->m_TableName,
    this->m_TableIDName,"CoordIDMax",this->GetMapValue("CoordIDMax"),
    "CoordIDMin",this->GetMapValue("CoordIDMin"));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBTraceRow::SetColor(unsigned int Red, unsigned int Green,
  unsigned int Blue,unsigned int Alpha, std::string ColorName,
  vtkMySQLDatabase* DatabaseConnector)
{
  GoDBColorRow ColorRow;
  ColorRow.SetField<int>("Red",Red);
  ColorRow.SetField<int>("Green",Green);
  ColorRow.SetField<int>("Blue",Blue);
  ColorRow.SetField<int>("Alpha",Alpha);
  ColorRow.SetField("Name", ColorName);
  int ColorID = ColorRow.DoesThisColorAlreadyExists(DatabaseConnector);
  if (ColorID == -1)
    {
    this->m_MapRow["ColorID"] = ConvertToString<int>(ColorRow.SaveInDB(DatabaseConnector));
    }
  else
    {
    this->SetField<int>("ColorID",ColorID);
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GoDBTraceRow::GetCollectionIDName()
{
  return this->m_CollectionIDName;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GoDBTraceRow::GetCollectionName()
{
  return this->m_CollectionName;
}
