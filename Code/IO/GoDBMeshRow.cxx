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
#include "GoDBMeshRow.h"
#include "GoDBColorRow.h"
#include "SelectQueryDatabaseHelper.h"
#include "GoDBRecordSetHelper.h"
#include "GoDBIntensityRow.h"
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
{
}

//-------------------------------------------------------------------------
GoDBMeshRow::
GoDBMeshRow(vtkMySQLDatabase* DatabaseConnector,
  vtkPolyData* TraceVisu, GoDBCoordinateRow Min, GoDBCoordinateRow Max,
  unsigned int ImgSessionID, GoFigureMeshAttributes* iMeshAttributes) :
  GoDBTraceRow( DatabaseConnector, TraceVisu, Min, Max, ImgSessionID )
{
  this->InitializeMap();
  if (this->DoesThisBoundingBoxExist(DatabaseConnector))
    {
    std::cout<<"The bounding box already exists for this mesh"<<std::endl;
    }
    
  m_NameChannelWithValues.clear();
  
  if( iMeshAttributes )
    {
    this->m_NameChannelWithValues = iMeshAttributes->m_TotalIntensityMap;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*GoDBMeshRow::GoDBMeshRow(vtkMySQLDatabase* DatabaseConnector,
  GoDBCoordinateRow Min, GoDBCoordinateRow Max,unsigned int ImgSessionID,
  vtkPolyData* TraceVisu,GoFigureMeshAttributes* iMeshAttributes)
  :GoDBTraceRow(DatabaseConnector,TraceVisu,Min,Max,ImgSessionID)
{
  this->InitializeMap();
  if (this->DoesThisBoundingBoxExist(DatabaseConnector))
    {
    std::cout<<"The bounding box already exists for this Mesh"<<std::endl;
    }   
  this->m_NameChannelWithValues = iMeshAttributes->m_TotalIntensityMap;
}*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBMeshRow::GoDBMeshRow(const GoDBMeshRow & iRow) : GoDBTraceRow()
{
  this->m_TableName = iRow.m_TableName;
  this->m_TableIDName = iRow.m_TableIDName;
  this->m_CollectionIDName = iRow.m_CollectionIDName;
  this->m_CollectionName = iRow.m_CollectionName;
  this->InitializeMap();
  this->m_MapRow = iRow.m_MapRow;
  if( !iRow.m_NameChannelWithValues.empty() )
    {
    m_NameChannelWithValues = iRow.m_NameChannelWithValues;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void
GoDBMeshRow::
SafeDownCast( GoDBTraceRow& iRow )
{
  GoDBTraceRow::StringMapConstIterator iRowIt = iRow.ConstMapBegin();
  while( iRowIt != iRow.ConstMapEnd() )
    {
    this->SetField( iRowIt->first, iRowIt->second );
    ++iRowIt;
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBMeshRow::InitializeMap()
{
  this->m_TableName = "mesh";
  this->m_TableIDName = "MeshID";
  this->m_CollectionName = "track";
  this->m_CollectionIDName = "TrackID";
  this->m_MapRow[this->m_TableIDName] = ConvertToString<int>(0);
  this->m_MapRow["CellTypeID"] = ConvertToString<int>(0);
  this->m_MapRow["SubCellularID"] = ConvertToString<int>(0);
  this->m_MapRow["TrackID"] = ConvertToString<int>(0);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBMeshRow::SaveInDB(vtkMySQLDatabase* DatabaseConnector)
{
  //std::cout <<*this <<std::endl;
  int NewMeshID = AddOnlyOneNewObjectInTable<GoDBMeshRow>( DatabaseConnector,
    "mesh", this, "MeshID" );
  this->SetField("MeshID",NewMeshID);
  if (!this->m_NameChannelWithValues.empty())
    {
    this->SaveInDBTotalIntensityPerChannel(DatabaseConnector,
      this->m_NameChannelWithValues);
    }
  return NewMeshID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBMeshRow::SetCellType(vtkMySQLDatabase* DatabaseConnector,
  std::string CellTypeName)
{
  this->SetField<int>("CellTypeID",FindOneID(DatabaseConnector,
  "celltype", "CellTypeID","Name",CellTypeName));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBMeshRow::SetSubCellType(vtkMySQLDatabase* DatabaseConnector,
  std::string SubCellTypeName)
{
  this->SetField<int>("SubCellularID",FindOneID(DatabaseConnector,
  "subcellulartype", "SubCellularID","Name",SubCellTypeName));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBMeshRow::SetCollectionID (int iCollectionID)
{
  this->SetField<int>("TrackID",iCollectionID);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBMeshRow::ReInitializeMapAfterCast()
{
  GoDBMeshRow::InitializeMap();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBMeshRow::SaveInDBTotalIntensityPerChannel(
  vtkMySQLDatabase* DatabaseConnector,
  std::map<std::string,int> iNameChannelWithValues)
{
  if (this->GetMapValue("MeshID") == "0")
    {
    std::cout<<"The mesh needs to be saved before"<<std::endl;
    return;
    }
  if (this->GetMapValue("ImagingSessionID")== "0")
    {
    std::cout<<"The imagingSession hasn't been entered for the mesh"<<std::endl;
    }
  std::map<std::string,int>::iterator iter = iNameChannelWithValues.begin();
  while (iter != iNameChannelWithValues.end())
    {
    int ChannelID = FindOneID(DatabaseConnector,"channel", "ChannelID","Name",
      iter->first,"ImagingSessionID",this->GetMapValue("ImagingSessionID"));
    GoDBIntensityRow NewIntensity;
    NewIntensity.SetField("ChannelID",ChannelID);
    NewIntensity.SetField("Value",iter->second);
    NewIntensity.SetField("MeshID",this->GetMapValue("MeshID"));
    NewIntensity.SaveInDB(DatabaseConnector);
    iter++;
    }
}
