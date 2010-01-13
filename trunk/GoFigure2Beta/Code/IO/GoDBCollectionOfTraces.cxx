/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Revision: 590 $  // Revision of last commit
  Date: $Date: 2009-08-21 17:48:12 -0400 (Fri, 21 Aug 2009) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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
#include "GoDBCollectionOfTraces.h"
#include "SelectQueryDatabaseHelper.h"
#include "QueryDataBaseHelper.h"
#include "ConvertToStringHelper.h"
#include "vtkMySQLDatabase.h"
#include "GoDBCoordinateRow.h"
#include "GoDBTableWidgetContainer.h"
#include "GoDBMeshRow.h"
#include "GoDBTrackRow.h"
#include "GoDBLineageRow.h"
#include <QStringList>
#include <QString>
#include <string>
#include <map>


GoDBCollectionOfTraces::GoDBCollectionOfTraces()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCollectionOfTraces::GoDBCollectionOfTraces( 
  std::string iCollectionName,std::string iTracesName)
{
  m_CollectionName   = iCollectionName;
  m_CollectionIDName = m_CollectionName;
  m_CollectionIDName += "ID";
  m_TracesName       = iTracesName;
  m_TracesIDName     = m_TracesName;
  m_TracesIDName     += "ID";
  
  m_LinkToRowContainer = new GoDBTableWidgetContainer(iCollectionName,iTracesName);

  //m_ColumnsInfos     = GetColumnsInfoForTraceTable();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCollectionOfTraces::~GoDBCollectionOfTraces()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::SetImgSessionID (unsigned int iImgSessionID)
{
  m_ImgSessionID = iImgSessionID;  
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::DeleteTraces(std::list<int> TracesToDelete,
  vtkMySQLDatabase* DatabaseConnector)
{
  std::list<int>::iterator iter = TracesToDelete.begin();
  while (iter != TracesToDelete.end())
    {
    int ID = *iter;

    DeleteRow(DatabaseConnector,m_TracesName,m_TracesIDName,ConvertToString<int>(ID));
    iter++;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::AddSelectedTracesToCollection(
  std::list<int> iListSelectedTraces,int inewCollectionID,
  vtkMySQLDatabase* DatabaseConnector)
{
  std::string newCollectionIDstring = ConvertToString<int>(inewCollectionID);
  std::list<int>::iterator iter = iListSelectedTraces.begin();
  while (iter != iListSelectedTraces.end())
    {
    int TraceID = *iter;
    /*if (TraceID == 0)
      {
      std::string TraceID = "null";
      }*/
    UpdateValueInDB(DatabaseConnector,m_TracesName,m_CollectionIDName, 
      newCollectionIDstring,m_TracesIDName, ConvertToString<int>(TraceID));
    iter++;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QStringList GoDBCollectionOfTraces::ListCollectionID(
  vtkMySQLDatabase* DatabaseConnector)
 {
   QStringList ListIDs;
   std::vector<std::string> vectListIDs = ListAllValuesForOneColumn(
     DatabaseConnector,m_CollectionIDName,m_CollectionName);

   for( unsigned int i = 0; i < vectListIDs.size(); ++i )
     {
     ListIDs.append( vectListIDs[i].c_str( ) );
     }

   return ListIDs;
 }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int GoDBCollectionOfTraces::GetCoordMinID(vtkMySQLDatabase* DatabaseConnector,
  int CollectionID,std::list<int> ListSelectedTraces)
{
  GoDBCoordinateRow NewCollectionCoordMin;
  //transform the std::list<int> to a vector<string>:
  std::vector<std::string> VectorSelectedTraces;
  std::list<int>::iterator iter = ListSelectedTraces.begin();
  while (iter != ListSelectedTraces.end())
    {
    int ID = *iter;
    VectorSelectedTraces.push_back(ConvertToString<int>(ID));
    iter++;
    }
  
  //Get the min of the selecting traces to add:
  GoDBCoordinateRow SelectingCoordMin = GetSelectingTracesCoordMin(
    DatabaseConnector, VectorSelectedTraces );

  //Get the CoordMin of the existing Collection if it is not a new one:
  //if it is a new one, the CollectionID will be 0
  if (CollectionID != 0)
    { 
    //find the CoordIDMin of the existing collection:
    int CollectionCoordIDMin = FindOneID(DatabaseConnector,
      this->m_CollectionName,"CoordIDMin",this->m_CollectionIDName,
      ConvertToString<int>(CollectionID)); 
    //fill the ExistingCoordMin with the values from the DB of the coordinate values
    // of the coordMin for the existing collection
    GoDBCoordinateRow ExistingCoordMin = GetExistingCoordMin(DatabaseConnector,
      CollectionCoordIDMin, CollectionID );
    
    //get the column names for the coordinate table:
    std::vector<std::string> ColumnNames = NewCollectionCoordMin.GetVectorColumnNames();
    bool SameCoord = true;
    //put the min between the existing coord value and the selecting ones in the 
    //NewCollectionCoorMin
    for( unsigned int i = 0; i <ColumnNames.size(); i++ )
      {
      int ValueExistingCoordMin = atoi(ExistingCoordMin.GetMapValue(ColumnNames[i]).c_str());
      int ValueSelectingCoordMin = atoi(SelectingCoordMin.GetMapValue(ColumnNames[i]).c_str());
      if (ValueExistingCoordMin > ValueSelectingCoordMin)
        {
        SameCoord = false;
        NewCollectionCoordMin.SetField(ColumnNames[i],ValueSelectingCoordMin);
        }
      }
    if (SameCoord == true)
      {
      return CollectionCoordIDMin;
      }
    }
  else
    {
    NewCollectionCoordMin = SelectingCoordMin;
    }
  //check if the coordinate already exists in the DB, if not, will be = -1:
   int ID = NewCollectionCoordMin.DoesThisCoordinateExist(DatabaseConnector);
   if (ID != -1)
     {
     return ID;
     }
   //if ID == -1, there is no coordinate already in the DB, so have to create it:
   return AddOnlyOneNewObjectInTable<GoDBCoordinateRow>( DatabaseConnector,
    "coordinate",NewCollectionCoordMin, "CoordID");
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int GoDBCollectionOfTraces::GetCoordMaxID(vtkMySQLDatabase* DatabaseConnector,
  int CollectionID,std::list<int> ListSelectedTraces)
{
  GoDBCoordinateRow NewCollectionCoordMax;
  //transform the std::list<int> to a vector<string>:
  std::vector<std::string> VectorSelectedTraces;
  std::list<int>::iterator iter = ListSelectedTraces.begin();
  while (iter != ListSelectedTraces.end())
    {
    int ID = *iter;
    VectorSelectedTraces.push_back(ConvertToString<int>(ID));
    iter++;
    }
  
  //Get the max of the selecting traces to add:
  GoDBCoordinateRow SelectingCoordMax = GetSelectingTracesCoordMax(
    DatabaseConnector, VectorSelectedTraces );

  //Get the CoordMax of the existing Collection if it is not a new one:
  //if it is a new one, the CollectionID will be 0
  if (CollectionID != 0)
    { 
    //find the CoordIDMax of the existing collection:
    int CollectionCoordIDMax = FindOneID(DatabaseConnector,
      this->m_CollectionName,"CoordIDMax",this->m_CollectionIDName,
      ConvertToString<int>(CollectionID)); 
    //fill the ExistingCoordMax with the values from the DB of the coordinate values
    // of the coordMax for the existing collection
    GoDBCoordinateRow ExistingCoordMax = GetExistingCoordMax(DatabaseConnector,
      CollectionCoordIDMax, CollectionID );
    
    //get the column names for the coordinate table:
    std::vector<std::string> ColumnNames = NewCollectionCoordMax.GetVectorColumnNames();
    bool SameCoord = true;
    //put the min between the existing coord value and the selecting ones in the 
    //NewCollectionCoorMin
    for( unsigned int i = 0; i <ColumnNames.size(); i++ )
      {
      int ValueExistingCoordMax = atoi(ExistingCoordMax.GetMapValue(ColumnNames[i]).c_str());
      int ValueSelectingCoordMax = atoi(SelectingCoordMax.GetMapValue(ColumnNames[i]).c_str());
      if (ValueExistingCoordMax < ValueSelectingCoordMax)
        {
        SameCoord = false;
        NewCollectionCoordMax.SetField(ColumnNames[i],ValueSelectingCoordMax);
        }
      }
    if (SameCoord == true)
      {
      return CollectionCoordIDMax;
      }
    }
  else
    {
    NewCollectionCoordMax = SelectingCoordMax;
    }
  //check if the coordinate already exists in the DB, if not, will be = -1:
   int ID = NewCollectionCoordMax.DoesThisCoordinateExist(DatabaseConnector);
   if (ID != -1)
     {
     return ID;
     }
   //if ID == -1, there is no coordinate already in the DB, so have to create it:
   return AddOnlyOneNewObjectInTable<GoDBCoordinateRow>( DatabaseConnector,
    "coordinate",NewCollectionCoordMax, "CoordID");
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCoordinateRow GoDBCollectionOfTraces::GetSelectingTracesCoordMin(
  vtkMySQLDatabase* DatabaseConnector, 
  std::vector<std::string> ListSelectedTracesID )
{ 
  GoDBCoordinateRow CoordMin;
   //First, get the coordID in the contour table that corresponds to the
  //coordIDMin of the Contours selected:
  std::vector<std::string> ListSelectedTracesCoordIDMin = 
    ListSpecificValuesForOneColumn(DatabaseConnector,
    this->m_TracesName,"CoordIDMin",this->m_TracesIDName,ListSelectedTracesID);
  //then, go to the coordinate table and compare the values for the coordID
  //corresponding to the coordIDMax of the selected contours:
  std::vector<std::string> ColumnNames = CoordMin.GetVectorColumnNames();
  for( unsigned int i = 0; i <ColumnNames.size(); i++ )
    {
    //don't compare the coordID !!!:
    if (ColumnNames[i] != "CoordID")
      {
      CoordMin.SetField(ColumnNames[i],MinValueForOneColumnInTable(
        DatabaseConnector,ColumnNames[i],"coordinate","CoordID",
        ListSelectedTracesCoordIDMin));
      }
    }
  return CoordMin;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCoordinateRow GoDBCollectionOfTraces::GetSelectingTracesCoordMax(
  vtkMySQLDatabase* DatabaseConnector, 
  std::vector<std::string> ListSelectedTracesID )
{ 
  GoDBCoordinateRow CoordMax;
  //First, get the coordID in the contour table that corresponds to the
  //coordIDMax of the Contours selected:
  std::vector<std::string> ListSelectedTracesCoordIDMax = 
    ListSpecificValuesForOneColumn(DatabaseConnector,
    this->m_TracesName,"CoordIDMax",this->m_TracesIDName,ListSelectedTracesID);
  //then, go to the coordinate table and compare the values for the coordID
  //corresponding to the coordIDMax of the selected contours:
  std::vector<std::string> ColumnNames = CoordMax.GetVectorColumnNames();
  for( unsigned int i = 0; i <ColumnNames.size(); i++ )
    {
    //don't compare the coordID !!!:
    if (ColumnNames[i] != "CoordID")
      {
      CoordMax.SetField(ColumnNames[i],MaxValueForOneColumnInTable(
        DatabaseConnector,ColumnNames[i],"coordinate","CoordID",
        ListSelectedTracesCoordIDMax));
      }
    }
  return CoordMax;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCoordinateRow GoDBCollectionOfTraces::GetExistingCoordMin(
  vtkMySQLDatabase* DatabaseConnector, int CollectionCoordIDMin,
  int CollectionID )
{
  GoDBCoordinateRow ExistingCoordMin;
  std::vector<std::string> VectorValuesExistingCoordMin = ListSpecificValuesForRow(
    DatabaseConnector,"coordinate", "CoordID",ConvertToString<int>(CollectionCoordIDMin));

  std::vector<std::string> ColumnNames = ExistingCoordMin.GetVectorColumnNames();

  for( unsigned int i = 0; i <ColumnNames.size(); i++ )
    {
    ExistingCoordMin.SetField(ColumnNames[i],VectorValuesExistingCoordMin[i]);
    }

  return ExistingCoordMin;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCoordinateRow GoDBCollectionOfTraces::GetExistingCoordMax(
  vtkMySQLDatabase* DatabaseConnector, int CollectionCoordIDMax,
  int CollectionID )
{
  GoDBCoordinateRow ExistingCoordMax;
  std::vector<std::string> VectorValuesExistingCoordMax = ListSpecificValuesForRow(
    DatabaseConnector,"coordinate", "CoordID",ConvertToString<int>(CollectionCoordIDMax));

  std::vector<std::string> ColumnNames = ExistingCoordMax.GetVectorColumnNames();

  for( unsigned int i = 0; i <ColumnNames.size(); i++ )
    {
    ExistingCoordMax.SetField(ColumnNames[i],VectorValuesExistingCoordMax[i]);
    }

  return ExistingCoordMax;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list<std::string> GoDBCollectionOfTraces::
GetListColumnsNamesForTableWidget()
{
  return m_LinkToRowContainer->GetListColumnsNamesForTableWidget();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCollectionOfTraces::DBTableWidgetContainerType
  GoDBCollectionOfTraces::GetRowContainer(vtkMySQLDatabase* DatabaseConnector)
{
 /*first, get the right parts of the first query:
 all the fields except the ones where table.field are already in the query:*/
  std::vector<std::string> JoinFirstTablesOnTraceTable = 
    m_LinkToRowContainer->GetQueryStringForTraceJoinedTables(false);
  std::vector<std::string> SelectFirstFields = 
    m_LinkToRowContainer->GetQueryStringForSelectFieldsTables(false);

  //then, get the results of the first query:
  std::vector<std::vector<std::string> >ResultsFirstQuery = GetValuesFromSeveralTables(
    DatabaseConnector,this->m_TracesName,SelectFirstFields, "ImagingSessionID",
    ConvertToString<unsigned int>(this->m_ImgSessionID),JoinFirstTablesOnTraceTable,false);

  //fill the row container with the results of the first query:
  m_LinkToRowContainer->FillRowContainer(ResultsFirstQuery,SelectFirstFields);

  //Get the right parts of the second query (with only the remaining fields):
   std::vector<std::string> JoinSecondTablesOnTraceTable = 
    m_LinkToRowContainer->GetQueryStringForTraceJoinedTables(true);
  std::vector<std::string> SelectSecondFields = 
    m_LinkToRowContainer->GetQueryStringForSelectFieldsTables(true);

  //then, get the results of the second query:
  std::vector<std::vector<std::string> >ResultsSecondQuery = GetValuesFromSeveralTables(
    DatabaseConnector,this->m_TracesName,SelectSecondFields, "ImagingSessionID",
    ConvertToString<unsigned int>(this->m_ImgSessionID),JoinSecondTablesOnTraceTable,false);
  
  //fill the row container with the results of the second query:
  m_LinkToRowContainer->FillRowContainer(ResultsSecondQuery,SelectSecondFields);

 return m_LinkToRowContainer->GetRowContainer();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBTableWidgetContainer* GoDBCollectionOfTraces::GetLinkToNewCreatedTraceContainer(
  vtkMySQLDatabase* iDatabaseConnector)
{
  GoDBTableWidgetContainer* LinkToNewCreatedTraceContainer = new
    GoDBTableWidgetContainer(this->m_CollectionName,
    this->m_TracesName); 
 
  /*first, get the right parts of the first query:
 all the fields except the ones where table.field are already in the query:*/
  std::vector<std::string> JoinFirstTablesOnTraceTable = 
    m_LinkToRowContainer->GetQueryStringForTraceJoinedTables(false);
  std::vector<std::string> SelectFirstFields = 
    m_LinkToRowContainer->GetQueryStringForSelectFieldsTables(false);
  
  //then, get the last ID in the database, corresponding to the new created trace:
  int NewTraceID = MaxValueForOneColumnInTable(iDatabaseConnector,
    this->m_TracesIDName,this->m_TracesName,"ImagingSessionID",
    ConvertToString<unsigned int>(this->m_ImgSessionID));

  //then, get the results of the first query:
  std::vector<std::vector<std::string> >ResultsFirstQuery = GetValuesFromSeveralTables(
    iDatabaseConnector,this->m_TracesName,SelectFirstFields, this->m_TracesIDName,
    ConvertToString<int>(NewTraceID),JoinFirstTablesOnTraceTable,false);

  //insert into the row container, the results of the first query:
 // m_LinkToRowContainer->FillRowContainer(ResultsFirstQuery,SelectFirstFields);
  LinkToNewCreatedTraceContainer->FillRowContainer(ResultsFirstQuery,SelectFirstFields);

  //Get the right parts of the second query (with only the remaining fields):
   std::vector<std::string> JoinSecondTablesOnTraceTable = 
    m_LinkToRowContainer->GetQueryStringForTraceJoinedTables(true);
  std::vector<std::string> SelectSecondFields = 
    m_LinkToRowContainer->GetQueryStringForSelectFieldsTables(true);

  //then, get the results of the second query:
  std::vector<std::vector<std::string> >ResultsSecondQuery = GetValuesFromSeveralTables(
    iDatabaseConnector,this->m_TracesName,SelectSecondFields, this->m_TracesIDName,
    ConvertToString<int>(NewTraceID),JoinSecondTablesOnTraceTable,false);
  
  //insert into the row container, the results of the second query:
  //m_LinkToRowContainer->FillRowContainer(ResultsSecondQuery,SelectSecondFields);
   LinkToNewCreatedTraceContainer->FillRowContainer(ResultsSecondQuery,SelectSecondFields);
   GoDBCollectionOfTraces::DBTableWidgetContainerType test = LinkToNewCreatedTraceContainer->GetRowContainer();//for test
   return LinkToNewCreatedTraceContainer;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int GoDBCollectionOfTraces::CreateCollectionWithNoTraces(
  vtkMySQLDatabase* DatabaseConnector, GoDBTraceRow iNewCollection)
{
  iNewCollection.SetField<unsigned int>("ImagingSessionID",this->m_ImgSessionID);
  // As there is no traces in the collection, the bounding box is the minimum one:
  // CoordIDMax correspond to the imagingsession Min and CoordIDMin to the Max:
  int CoordIDMax = FindOneID(DatabaseConnector, "imagingsession", "CoordIDMin",
    "ImagingSessionID", ConvertToString<int>(this->m_ImgSessionID));

  int CoordIDMin = FindOneID(DatabaseConnector, "imagingsession", "CoordIDMax",
    "ImagingSessionID", ConvertToString<int>(this->m_ImgSessionID));

  iNewCollection.SetField< int >( "CoordIDMax", CoordIDMax );
  iNewCollection.SetField< int >( "CoordIDMin", CoordIDMin );

  return this->CreateNewCollection(DatabaseConnector,iNewCollection);
    
 /* if (this->m_CollectionName == "mesh")
    {
    GoDBMeshRow* NewMesh = static_cast<GoDBMeshRow*>(&iNewCollection);*/

    /*if (NewMesh == 0)
      {
      std::cout<<"ca foire"<<std::endl;
      }
    else
      {
      std::cout<<"ca marche"<<std::endl;
      }
    return this->CreateNewCollection<GoDBMeshRow>(DatabaseConnector,*NewMesh);
    }
  return 0;*/
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int GoDBCollectionOfTraces::CreateNewCollectionFromSelection(
  std::list<int> iListSelectedTraces, vtkMySQLDatabase* DatabaseConnector,
    GoDBTraceRow iNewCollection)
{
/** \todo merge createNewCollectionFromSelection and CreateCollectionWithNotraces*/
 //the following fields are common to all the collections, the one
 //that are different are specified in QGoPrintDatabase:
 iNewCollection.SetField("ImagingSessionID",this->m_ImgSessionID);
 //the CollectionID is set to 0 as it is a new Collection that will be created, not
 //contours to be added to an existing collection:
 iNewCollection.SetField("CoordIDMax",this->GetCoordMaxID(
   DatabaseConnector,0,iListSelectedTraces));
 iNewCollection.SetField("CoordIDMin",this->GetCoordMinID(
   DatabaseConnector,0,iListSelectedTraces));

 int NewCollectionID = this->CreateNewCollection(DatabaseConnector,iNewCollection);

  AddSelectedTracesToCollection(iListSelectedTraces,
    NewCollectionID, DatabaseConnector);

  return NewCollectionID;
 }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int GoDBCollectionOfTraces::CreateNewCollection(
  vtkMySQLDatabase* DatabaseConnector, GoDBTraceRow iNewCollection)
{
  if (this->m_CollectionName == "mesh")
    {
    GoDBMeshRow* NewMesh = static_cast<GoDBMeshRow*>(&iNewCollection);
    return AddOnlyOneNewObjectInTable<GoDBMeshRow>(
      DatabaseConnector,this->m_CollectionName, NewMesh , m_CollectionIDName);
    }
 if (this->m_CollectionName == "track")
    {
    GoDBTrackRow* NewTrack = static_cast<GoDBTrackRow*>(&iNewCollection);
    return AddOnlyOneNewObjectInTable<GoDBTrackRow>(
      DatabaseConnector,this->m_CollectionName, NewTrack , m_CollectionIDName);
    }
 if (this->m_CollectionName == "lineage")
    {
    GoDBLineageRow* NewLineage = static_cast<GoDBLineageRow*>(&iNewCollection);
    return AddOnlyOneNewObjectInTable<GoDBLineageRow>(
      DatabaseConnector,this->m_CollectionName, NewLineage , m_CollectionIDName);
    }
  return 0;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string GoDBCollectionOfTraces::GetCollectionOf()
{
  if (this->m_TracesName == "contour")
    {
    return "";
    }
  if (this->m_TracesName == "mesh")
    {
    return "contour";
    }
  if (this->m_TracesName == "track")
    {
    return "mesh";
    }
  if (this->m_TracesName == "lineage")
    {
    return "track";
    }
}