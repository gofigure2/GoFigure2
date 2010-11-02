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
#include "GoDBMeshRow.h"
#include "GoDBColorRow.h"
#include "SelectQueryDatabaseHelper.h"
#include "GoDBRecordSetHelper.h"
#include "GoDBIntensityRow.h"
#include "vtkPolyDataMySQLMeshWriter.h"

#include "vtkSmartPointer.h"

#include <iostream>

//-------------------------------------------------------------------------
GoDBMeshRow::GoDBMeshRow():GoDBTraceRow()
{
  this->InitializeMap();
  m_NameChannelWithValues.clear();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBMeshRow::~GoDBMeshRow()
{}

//-------------------------------------------------------------------------
GoDBMeshRow::GoDBMeshRow(vtkMySQLDatabase *DatabaseConnector,
                         vtkPolyData *TraceVisu, GoDBCoordinateRow Min, GoDBCoordinateRow Max,
                         unsigned int ImgSessionID, GoFigureMeshAttributes *iMeshAttributes):
  GoDBTraceRow()
{
  this->InitializeMap();
  this->SetImgSessionID(ImgSessionID);
  this->SetTheDataFromTheVisu(DatabaseConnector, TraceVisu, Min, Max, iMeshAttributes);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBMeshRow::GoDBMeshRow(unsigned int ImagingSessionID):
  GoDBTraceRow()
{
  this->InitializeMap();
  this->SetImgSessionID(ImagingSessionID);
  this->m_MapRow["ImagingSessionID"] = ConvertToString<int>(ImagingSessionID);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBMeshRow::GoDBMeshRow(const GoDBMeshRow & iRow):GoDBTraceRow()
{
  this->m_TableName = iRow.m_TableName;
  this->m_TableIDName = iRow.m_TableIDName;
  this->m_CollectionIDName = iRow.m_CollectionIDName;
  this->m_CollectionName = iRow.m_CollectionName;
  this->InitializeMap();
  this->m_MapRow = iRow.m_MapRow;
  if ( !iRow.m_NameChannelWithValues.empty() )
    {
    m_NameChannelWithValues = iRow.m_NameChannelWithValues;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBMeshRow::SetTheDataFromTheVisu(vtkMySQLDatabase *DatabaseConnector,
                                        vtkPolyData *TraceVisu,
                                        GoDBCoordinateRow iCoordMin,
                                        GoDBCoordinateRow iCoordMax,
                                        GoFigureMeshAttributes *iMeshAttributes)
{
  this->SetTheBoundingBox(DatabaseConnector, iCoordMin, iCoordMax);

  vtkSmartPointer< vtkPolyDataMySQLMeshWriter > convert =
    vtkSmartPointer< vtkPolyDataMySQLMeshWriter >::New();
  std::string PointsString = convert->GetMySQLText(TraceVisu);

  std::cout << "output string: " << PointsString << std::endl;

  this->SetField("Points", PointsString);

  if ( this->DoesThisBoundingBoxExist(DatabaseConnector) )
    {
    std::cout << "The bounding box already exists for this mesh" << std::endl;
    }

  m_NameChannelWithValues.clear();

  if ( iMeshAttributes )
    {
    this->m_NameChannelWithValues = iMeshAttributes->m_TotalIntensityMap;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoDBMeshRow::SafeDownCast(GoDBTraceRow & iRow)
{
  GoDBTraceRow::StringMapConstIterator iRowIt = iRow.ConstMapBegin();

  while ( iRowIt != iRow.ConstMapEnd() )
    {
    this->SetField(iRowIt->first, iRowIt->second);
    ++iRowIt;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBMeshRow::InitializeMap()
{
  this->m_TableName = "mesh";
  this->m_TableIDName = "meshID";
  this->m_CollectionName = "track";
  this->m_CollectionIDName = "trackID";
  this->m_MapRow[this->m_TableIDName] = ConvertToString< int >(0);
  this->m_MapRow["CellTypeID"] = ConvertToString< int >(0);
  this->m_MapRow["SubCellularID"] = ConvertToString< int >(0);
  this->m_MapRow["trackID"] = ConvertToString< int >(0);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBMeshRow::SaveInDB(vtkMySQLDatabase *DatabaseConnector)
{
  int SavedMeshID = GoDBTraceRow::SaveInDBTemplate< GoDBMeshRow >(DatabaseConnector, this);

  //int SavedMeshID = GoDBTraceRow::SaveInDBTemplate< GoDBMeshRow >(DatabaseConnector, *this);

  if ( !this->m_NameChannelWithValues.empty() )
    {
    this->SaveInDBTotalIntensityPerChannel(DatabaseConnector,
                                           this->m_NameChannelWithValues);
    }

  return SavedMeshID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBMeshRow::SetCellType(vtkMySQLDatabase *DatabaseConnector,
                              std::string CellTypeName)
{
  this->SetField< int >( "CellTypeID", FindOneID(DatabaseConnector,
                                                 "celltype", "CellTypeID", "Name", CellTypeName) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBMeshRow::SetSubCellType(vtkMySQLDatabase *DatabaseConnector,
                                 std::string SubCellTypeName)
{
  this->SetField< int >( "SubCellularID", FindOneID(DatabaseConnector,
                                                    "subcellulartype", "SubCellularID", "Name", SubCellTypeName) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBMeshRow::SaveInDBTotalIntensityPerChannel(
  vtkMySQLDatabase *DatabaseConnector,
  std::map< std::string, int > iNameChannelWithValues)
{
  if ( this->GetMapValue("meshID") == "0" )
    {
    std::cout << "The mesh needs to be saved before" << std::endl;
    return;
    }
  if ( this->GetMapValue("ImagingSessionID") == "0" )
    {
    std::cout << "The imagingSession hasn't been entered for the mesh" << std::endl;
    }
  std::map< std::string, int >::iterator iter = iNameChannelWithValues.begin();
  while ( iter != iNameChannelWithValues.end() )
    {
    int ChannelID = FindOneID( DatabaseConnector, "channel", "ChannelID", "Name",
                               iter->first, "ImagingSessionID", this->GetMapValue("ImagingSessionID") );
    GoDBIntensityRow NewIntensity;
    NewIntensity.SetField("ChannelID", ChannelID);
    NewIntensity.SetField("Value", iter->second);
    std::string strMeshID = this->GetMapValue("meshID");
    NewIntensity.SetField( "meshID", this->GetMapValue("meshID") );
    NewIntensity.SaveInDB(DatabaseConnector);
    iter++;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
