/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
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
  //m_LinkToRowContainer = 0;
  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCollectionOfTraces::GoDBCollectionOfTraces(
  std::string iCollectionName, std::string iTracesName,
  std::string iCollectionOfName,unsigned int iImgSessionID)
  {
  this->SetCollectionInfo(iCollectionName, iTracesName, iCollectionOfName);
  this->SetImgSessionID(iImgSessionID);
  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCollectionOfTraces::~GoDBCollectionOfTraces()
  {
  }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::SetCollectionInfo(std::string iCollectionName,
                                               std::string iTracesName,
                                               std::string iCollectionOfName)
{
  m_CollectionName   = iCollectionName;
  m_CollectionIDName = m_CollectionName;
  m_CollectionIDName += "ID";
  m_TracesName       = iTracesName;
  m_TracesIDName       = m_TracesName;
  m_TracesIDName      += "ID";
  m_CollectionOfName   = iCollectionOfName;
  m_CollectionOfIDName = m_CollectionOfName;
  m_CollectionOfIDName += "ID";

  //m_LinkToRowContainer = new GoDBTableWidgetContainer(iCollectionName, iTracesName);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::SetImgSessionID(unsigned int iImgSessionID)
{
  m_ImgSessionID = iImgSessionID;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//Modif into Database
void GoDBCollectionOfTraces::DeleteTracesInDB(std::list<unsigned int> TracesToDelete,
                                              vtkMySQLDatabase* DatabaseConnector)
{
  std::list<unsigned int>::iterator iter = TracesToDelete.begin();
  while (iter != TracesToDelete.end())
    {
    int ID = *iter;

    DeleteRow(DatabaseConnector, m_TracesName, m_TracesIDName, ConvertToString<unsigned int>(ID));
    iter++;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//Modif into Database
void GoDBCollectionOfTraces::DeleteTraceInDB(int TraceToDelete,
                                             vtkMySQLDatabase* DatabaseConnector)
{
  DeleteRow(DatabaseConnector, m_TracesName, m_TracesIDName,
            ConvertToString<int>(TraceToDelete));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//Modif into Database
void GoDBCollectionOfTraces::UpdateCollectionIDOfSelectedTraces(
  std::list<unsigned int> iListSelectedTraces, unsigned int inewCollectionID,
  vtkMySQLDatabase* DatabaseConnector)
{
  std::string newCollectionIDstring = ConvertToString<unsigned int>(inewCollectionID);
  std::list<unsigned int>::iterator iter = iListSelectedTraces.begin();
  while (iter != iListSelectedTraces.end())
    {
    unsigned int TraceID = *iter;
    UpdateValueInDB(DatabaseConnector, m_TracesName, m_CollectionIDName,
                    newCollectionIDstring, m_TracesIDName,
                    ConvertToString<unsigned int>(TraceID));
    iter++;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//Modif into Database
void GoDBCollectionOfTraces::UpdateCollectionIDOfSelectedTrace(
  int iSelectedTraceID, int inewCollectionID,
  vtkMySQLDatabase* DatabaseConnector)
{
  UpdateValueInDB(DatabaseConnector, m_TracesName, m_CollectionIDName,
                  ConvertToString<int>(inewCollectionID), m_TracesIDName,
                  ConvertToString<int>(iSelectedTraceID));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//Modif into Database
void GoDBCollectionOfTraces::RecalculateDBBoundingBox(
  vtkMySQLDatabase* iDatabaseConnector, int iCollectionID)
{
  //update the corresponding bounding box:
  int CoordIDMax = this->GetCoordMaxID(iDatabaseConnector, iCollectionID);
  int CoordIDMin = this->GetCoordMinID(iDatabaseConnector, iCollectionID);

  this->UpdateBoundingBoxInDB(CoordIDMin, CoordIDMax,
                                        iCollectionID, iDatabaseConnector);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::RecalculateDBBoundingBox(
  vtkMySQLDatabase* iDatabaseConnector, std::list<unsigned int> iListTracesIDs)
{
  //std::vector< std::string > VectorTracesIDs(
  //  iListTracesIDs.begin(),iListTracesIDs.end() );
 // std::vector<std::string> VectorCollectionIDs = ListSpecificValuesForOneColumn(
  //  iDatabaseConnector,this->m_TracesName, this->m_CollectionIDName,
  //  this->m_TracesIDName, VectorTracesIDs,true,true);
  std::list<unsigned int>::iterator iter = iListTracesIDs.begin();
  while (iter != iListTracesIDs.end())
    {
    this->RecalculateDBBoundingBox(iDatabaseConnector,*iter);
    iter++;
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list<GoDBCollectionOfTraces::NameWithColorData> 
GoDBCollectionOfTraces::GetAllTracesIDsWithColor(
    vtkMySQLDatabase* iDatabaseConnector)
 {
  //First, build the query with selected fields and table to join with on conditions:
  std::vector<std::string> SelectFields;
  std::vector<std::string> JoinTablesOnTraceTable;
  this->GetFieldsNeededForQueryForColorData(SelectFields, JoinTablesOnTraceTable);
  std::vector<std::vector<std::string> > ResultsQuery;
  
  ResultsQuery  = GetValuesFromSeveralTables(iDatabaseConnector,
                                             this->m_TracesName, SelectFields, 
                                             "ImagingSessionID",
                                             ConvertToString<unsigned int>(
                                                 this->m_ImgSessionID), 
                                             JoinTablesOnTraceTable, true);
   
  return this->GetListNameWithColorDataFromResultsQuery(ResultsQuery);
 }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list<GoDBCollectionOfTraces::NameWithColorData> 
  GoDBCollectionOfTraces::GetTracesIDsWithColorForATimePoint(
  vtkMySQLDatabase* iDatabaseConnector,unsigned int iTimePoint)
{
  std::list<NameWithColorData > oListTraceIDs;
  //First, build the query with selected fields and table to join with on conditions:
  std::vector<std::string> SelectFields;
  std::vector<std::string> JoinTablesOnTraceTable;
  this->GetFieldsNeededForQueryForColorData(SelectFields, JoinTablesOnTraceTable);
  
  std::string JoinTable = "coordinate";
  JoinTablesOnTraceTable.push_back(JoinTable);
  std::string OnCondition = this->m_TracesName;
  OnCondition += ".";
  OnCondition += "CoordIDMin = coordinate.coordid";
  JoinTablesOnTraceTable.push_back(OnCondition);
  std::vector<std::string> WhereAndConditions;
  OnCondition = this->m_TracesName;
  OnCondition += ".ImagingsessionID";
  WhereAndConditions.push_back(OnCondition);
  WhereAndConditions.push_back(ConvertToString<unsigned int>(this->m_ImgSessionID));
  WhereAndConditions.push_back("coordinate.TCoord");
  WhereAndConditions.push_back(ConvertToString<int>(iTimePoint));

  std::vector<std::vector<std::string> > ResultsQuery = GetValuesFromSeveralTables(
                                            iDatabaseConnector,
                                            this->m_TracesName, SelectFields, 
                                            WhereAndConditions,
                                            JoinTablesOnTraceTable, true);

  return this->GetListNameWithColorDataFromResultsQuery(ResultsQuery);

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
 void GoDBCollectionOfTraces::GetFieldsNeededForQueryForColorData(
   std::vector<std::string> &ioSelectedFields,
   std::vector<std::string> &ioJoinTablesOnTraceTable)
 {
  ioSelectedFields.push_back(this->m_TracesIDName);
  std::string Red = "color.Red";
  ioSelectedFields.push_back(Red);
  std::string Green = "color.Green";
  ioSelectedFields.push_back(Green);
  std::string Blue = "color.Blue";
  ioSelectedFields.push_back(Blue);
  std::string Alpha = "color.Alpha";
  ioSelectedFields.push_back(Alpha);

  std::string              JoinTable = "color";
  ioJoinTablesOnTraceTable.push_back(JoinTable);
  std::string OnCondition = this->m_TracesName;
  OnCondition += ".ColorID = color.ColorID";
  ioJoinTablesOnTraceTable.push_back(OnCondition);
 }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
 std::list<GoDBCollectionOfTraces::NameWithColorData> 
   GoDBCollectionOfTraces::GetListNameWithColorDataFromResultsQuery(
   std::vector<std::vector<std::string> >iResultsQuery)
{
  std::list<NameWithColorData> oListNameWithColorData;
  unsigned int i = 0;
  std::vector<std::vector<std::string> >::iterator iter = iResultsQuery.begin();
  while (iter != iResultsQuery.end())
    {
    std::vector<std::string>       ResultsOneRow = *iter;
    int                            intRed   = atoi(ResultsOneRow[i + 1].c_str());
    int                            intGreen = atoi(ResultsOneRow[i + 2].c_str());
    int                            intBlue  = atoi(ResultsOneRow[i + 3].c_str());
    int                            intAlpha = atoi(ResultsOneRow[i + 4].c_str());
    QColor                         Color(intRed, intGreen, intBlue, intAlpha);
    NameWithColorData temp;
    temp.first = ResultsOneRow[i];
    temp.second = Color;
    oListNameWithColorData.push_back(temp);
    iter++;
    }  
  return oListNameWithColorData;
}
 //--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//Get From Database
/*QStringList GoDBCollectionOfTraces::ListCollectionID(
  vtkMySQLDatabase* DatabaseConnector)
{
  QStringList              ListIDs;
  std::vector<std::string> vectListIDs = ListAllValuesForOneColumn(
    DatabaseConnector, m_CollectionIDName, m_CollectionName);

  for (unsigned int i = 0; i < vectListIDs.size(); ++i)
    {
    ListIDs.append(vectListIDs[i].c_str());
    }

  return ListIDs;
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//get from Database and/or Modif into Database
int GoDBCollectionOfTraces::GetCoordMinID(vtkMySQLDatabase* DatabaseConnector,
                                          int CollectionID,
                                          std::list<unsigned int> ListSelectedTraces)
{
  GoDBCoordinateRow NewCollectionCoordMin;
  //transform the std::list<int> to a vector<string>:
  std::vector<std::string> VectorSelectedTraces;
  std::list<unsigned int>::iterator iter = ListSelectedTraces.begin();
  while (iter != ListSelectedTraces.end())
    {
    unsigned int ID = *iter;
    VectorSelectedTraces.push_back(ConvertToString<unsigned int>(ID));
    ++iter;
    }
  //Get the min of the selecting traces to add:
  GoDBCoordinateRow SelectingCoordMin = this->GetCollectionOfTracesCoordMin(
    DatabaseConnector, VectorSelectedTraces);

  //Get the CoordMin of the existing Collection if it is not a new one:
  //if it is a new one, the CollectionID will be 0
  if (CollectionID != 0)
    {
    //find the CoordIDMin of the existing collection:
    int CollectionCoordIDMin = FindOneID(DatabaseConnector,
                                         this->m_CollectionName, "CoordIDMin", this->m_CollectionIDName,
                                         ConvertToString<unsigned int>(CollectionID));
    //fill the ExistingCoordMin with the values from the DB of the coordinate values
    // of the coordMin for the existing collection
    GoDBCoordinateRow ExistingCoordMin = GetExistingCoordMin(DatabaseConnector,
                                                             CollectionCoordIDMin, CollectionID);

    //get the column names for the coordinate table:
    std::vector<std::string> ColumnNames = NewCollectionCoordMin.GetVectorColumnNames();
    bool                     SameCoord = true;
    //put the min between the existing coord value and the selecting ones in the
    //NewCollectionCoorMin
    for (unsigned int i = 0; i < ColumnNames.size(); i++)
      {
      int ValueExistingCoordMin = atoi(ExistingCoordMin.GetMapValue(ColumnNames[i]).c_str());
      int ValueSelectingCoordMin = atoi(SelectingCoordMin.GetMapValue(ColumnNames[i]).c_str());
      if (ValueExistingCoordMin > ValueSelectingCoordMin)
        {
        SameCoord = false;
        NewCollectionCoordMin.SetField(ColumnNames[i], ValueSelectingCoordMin);
        }
      else
        {
        NewCollectionCoordMin.SetField(ColumnNames[i], ValueExistingCoordMin);
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
  /*int ID = NewCollectionCoordMin.DoesThisCoordinateExist(DatabaseConnector);
  if (ID != -1)
    {
    return ID;
    }
  //if ID == -1, there is no coordinate already in the DB, so have to create it:
  return AddOnlyOneNewObjectInTable<GoDBCoordinateRow>( DatabaseConnector,
   "coordinate",NewCollectionCoordMin, "CoordID");*/
  return NewCollectionCoordMin.SaveInDB(DatabaseConnector);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//get from Database and/or Modif into Database
int GoDBCollectionOfTraces::GetCoordMinID(vtkMySQLDatabase* iDatabaseConnector,
                                          int iTraceID)
{
  //Get the list of the collectionof tracesID belonging to the iTraceID:
  //std::vector<std::string> VectorTracesIDs = ListSpecificValuesForOneColumn(
  //  iDatabaseConnector, this->m_CollectionOfName, this->m_CollectionOfIDName,
  //  this->m_TracesIDName, ConvertToString<int>(iTraceID));
  std::list<unsigned int> ListTracesIDs;
  ListTracesIDs.push_back(iTraceID);
  std::list<unsigned int> ListCollectionOfTraces = 
    this->GetListTracesIDsFromThisCollectionOf(iDatabaseConnector,ListTracesIDs);

  if (ListCollectionOfTraces.empty())
    {
    return this->GetCoordIDMinForBoundingBoxWithNoTraces(iDatabaseConnector);
    }
  else
    {
    //Get the min of the traces:
      std::vector<std::string> VectorCollectionOfTraces = 
        this->ListUnsgIntToVectorString(ListCollectionOfTraces);
      GoDBCoordinateRow TracesCoordMin = this->GetCollectionOfTracesCoordMin(
      iDatabaseConnector, VectorCollectionOfTraces);

    //check if the coordinate already exists in the DB, if not, will be = -1:
    /* int ID = TracesCoordMin.DoesThisCoordinateExist(iDatabaseConnector);
     if (ID != -1)
       {
       return ID;
       }
     //if ID == -1, there is no coordinate already in the DB, so have to create it:
      return AddOnlyOneNewObjectInTable<GoDBCoordinateRow>( iDatabaseConnector,
        "coordinate",TracesCoordMin, "CoordID");*/
    return TracesCoordMin.SaveInDB(iDatabaseConnector);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//get from Database and/or Modif into Database
int GoDBCollectionOfTraces::GetCoordMaxID(vtkMySQLDatabase* DatabaseConnector,
                                          int CollectionID,
                                          std::list<unsigned int> ListSelectedTraces)
{
  GoDBCoordinateRow NewCollectionCoordMax;
  //transform the std::list<int> to a vector<string>:
  std::vector<std::string> VectorSelectedTraces = 
    this->ListUnsgIntToVectorString(ListSelectedTraces);
  /*std::list<unsigned int>::iterator iter = ListSelectedTraces.begin();
  while (iter != ListSelectedTraces.end())
    {
    unsigned int ID = *iter;
    VectorSelectedTraces.push_back(ConvertToString<unsigned int>(ID));
    ++iter;
    }*/

  //Get the max of the selecting traces to add:
  GoDBCoordinateRow SelectingCoordMax = this->GetCollectionOfTracesCoordMax(
    DatabaseConnector, VectorSelectedTraces);

  //Get the CoordMax of the existing Collection if it is not a new one:
  //if it is a new one, the CollectionID will be 0
  if (CollectionID != 0)
    {
    //find the CoordIDMax of the existing collection:
    int CollectionCoordIDMax = FindOneID(DatabaseConnector,
                                         this->m_CollectionName, "CoordIDMax", this->m_CollectionIDName,
                                         ConvertToString<int>(CollectionID));
    //fill the ExistingCoordMax with the values from the DB of the coordinate values
    // of the coordMax for the existing collection
    GoDBCoordinateRow ExistingCoordMax = GetExistingCoordMax(DatabaseConnector,
                                                             CollectionCoordIDMax, CollectionID);

    //get the column names for the coordinate table:
    std::vector<std::string> ColumnNames = GetFieldNames("coordinate", DatabaseConnector);
    bool                     SameCoord = true;
    //put the max between the existing coord value and the selecting ones in the
    //NewCollectionCoorMax
    for (unsigned int i = 0; i < ColumnNames.size(); i++)
      {
      if (ColumnNames[i] != "CoordID")
        {
        int ValueExistingCoordMax = atoi(ExistingCoordMax.GetMapValue(ColumnNames[i]).c_str());
        int ValueSelectingCoordMax = atoi(SelectingCoordMax.GetMapValue(ColumnNames[i]).c_str());
        if (ValueExistingCoordMax < ValueSelectingCoordMax)
          {
          SameCoord = false;
          NewCollectionCoordMax.SetField(ColumnNames[i], ValueSelectingCoordMax);
          }
        else
          {
          NewCollectionCoordMax.SetField(ColumnNames[i], ValueExistingCoordMax);
          }
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
  /* int ID = NewCollectionCoordMax.DoesThisCoordinateExist(DatabaseConnector);
   if (ID != -1)
     {
     return ID;
     }
   //if ID == -1, there is no coordinate already in the DB, so have to create it:
   return AddOnlyOneNewObjectInTable<GoDBCoordinateRow>( DatabaseConnector,
    "coordinate",NewCollectionCoordMax, "CoordID");*/
  return NewCollectionCoordMax.SaveInDB(DatabaseConnector);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//get from Database and/or Modif into Database
int GoDBCollectionOfTraces::GetCoordMaxID(vtkMySQLDatabase* iDatabaseConnector,
                                          int iTraceID)
{
  //Get the list of the tracesID belonging to the collection:
  std::vector<std::string> VectorCollectionOfTracesIDs = 
    ListSpecificValuesForOneColumn(iDatabaseConnector, this->m_CollectionOfName, 
    this->m_CollectionOfIDName,this->m_TracesIDName, ConvertToString<int>(iTraceID));

  if (VectorCollectionOfTracesIDs.empty())
    {
    return this->GetCoordIDMaxForBoundingBoxWithNoTraces(iDatabaseConnector);
    }
  else
    {
    //Get the max of the traces:
      GoDBCoordinateRow TracesCoordMax = this->GetCollectionOfTracesCoordMax(
      iDatabaseConnector, VectorCollectionOfTracesIDs);

    //check if the coordinate already exists in the DB, if not, will be = -1:
    /* int ID = TracesCoordMax.DoesThisCoordinateExist(iDatabaseConnector);
     if (ID != -1)
       {
       return ID;
       }
     //if ID == -1, there is no coordinate already in the DB, so have to create it:
     return AddOnlyOneNewObjectInTable<GoDBCoordinateRow>( iDatabaseConnector,
      "coordinate",TracesCoordMax, "CoordID");*/
    return TracesCoordMax.SaveInDB(iDatabaseConnector);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//get from Database and/or Modif into Database
GoDBCoordinateRow GoDBCollectionOfTraces::GetCollectionOfTracesCoordMin(
  vtkMySQLDatabase* DatabaseConnector,
  std::vector<std::string> iListCollectionOfTracesID)
{
  GoDBCoordinateRow CoordMin;
  //First, get the coordID in the contour table that corresponds to the
  //coordIDMin of the Contours selected:
  std::vector<std::string> ListCollectionOfTracesCoordIDMin =
    ListSpecificValuesForOneColumn(DatabaseConnector,
    this->m_CollectionOfName, "CoordIDMin", this->m_CollectionOfIDName,
    iListCollectionOfTracesID);
  //then, go to the coordinate table and compare the values for the coordID
  //corresponding to the coordIDMax of the selected contours:
  std::vector<std::string> ColumnNames = CoordMin.GetVectorColumnNames();
  for (unsigned int i = 0; i < ColumnNames.size(); i++)
    {
    //don't compare the coordID !!!:
    if (ColumnNames[i] != "CoordID")
      {
      CoordMin.SetField(ColumnNames[i], MinValueForOneColumnInTable(
                          DatabaseConnector, ColumnNames[i], "coordinate", "CoordID",
                          ListCollectionOfTracesCoordIDMin));
      }
    }
  return CoordMin;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCoordinateRow GoDBCollectionOfTraces::GetCollectionOfTracesCoordMax(
  vtkMySQLDatabase* DatabaseConnector,
  std::vector<std::string> iListCollectionOfTracesID)
{
  GoDBCoordinateRow CoordMax;
  //First, get the coordID in the contour table that corresponds to the
  //coordIDMax of the Contours selected:
  std::vector<std::string> ListCollectionOfTracesCoordIDMax =
    ListSpecificValuesForOneColumn(DatabaseConnector,
                                   this->m_CollectionOfName, "CoordIDMax", 
                                   this->m_CollectionOfIDName, 
                                   iListCollectionOfTracesID);
  //then, go to the coordinate table and compare the values for the coordID
  //corresponding to the coordIDMax of the selected contours:
  std::vector<std::string> ColumnNames = CoordMax.GetVectorColumnNames();
  for (unsigned int i = 0; i < ColumnNames.size(); i++)
    {
    //don't compare the coordID !!!:
    if (ColumnNames[i] != "CoordID")
      {
      CoordMax.SetField(ColumnNames[i], MaxValueForOneColumnInTable(
                          DatabaseConnector, ColumnNames[i], "coordinate", "CoordID",
                          ListCollectionOfTracesCoordIDMax));
      }
    }
  return CoordMax;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCoordinateRow
GoDBCollectionOfTraces::
GetExistingCoordMin(vtkMySQLDatabase* DatabaseConnector,
                    int CollectionCoordIDMin,
                    int CollectionID)
{
  (void) CollectionID;
  GoDBCoordinateRow ExistingCoordMin;

  std::vector<std::string> VectorValuesExistingCoordMin =
    ListSpecificValuesForRow(DatabaseConnector,
                             "coordinate", "CoordID",
                             ConvertToString<int>(CollectionCoordIDMin));

  std::vector<std::string> ColumnNames =
    GetFieldNames("coordinate", DatabaseConnector);

  for (unsigned int i = 0; i < ColumnNames.size(); i++)
    {
    ExistingCoordMin.SetField(ColumnNames[i],
                              VectorValuesExistingCoordMin[i]);
    }

  return ExistingCoordMin;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCoordinateRow
GoDBCollectionOfTraces::
GetExistingCoordMax(vtkMySQLDatabase* DatabaseConnector,
                    int CollectionCoordIDMax,
                    int CollectionID)
{
  (void) CollectionID;

  GoDBCoordinateRow        ExistingCoordMax;
  std::vector<std::string> VectorValuesExistingCoordMax =
    ListSpecificValuesForRow(DatabaseConnector, "coordinate", "CoordID",
                             ConvertToString<int>(CollectionCoordIDMax));

  std::vector<std::string> ColumnNames = GetFieldNames("coordinate",
                                                       DatabaseConnector);

  for (unsigned int i = 0; i < ColumnNames.size(); i++)
    {
    ExistingCoordMax.SetField(ColumnNames[i],
                              VectorValuesExistingCoordMax[i]);
    }

  return ExistingCoordMax;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*int GoDBCollectionOfTraces::CreateCollectionWithNoTraces(
  vtkMySQLDatabase* DatabaseConnector, GoDBTraceRow& iNewCollection,
  int iTimePoint)
{
  iNewCollection.SetField<unsigned int>("ImagingSessionID", this->m_ImgSessionID);

  int CoordIDMax = GetCoordIDMaxForBoundingBoxWithNoTraces(DatabaseConnector);
  int CoordIDMin = GetCoordIDMinForBoundingBoxWithNoTraces(DatabaseConnector);

  iNewCollection.SetField<int>("CoordIDMax", CoordIDMax);
  iNewCollection.SetField<int>("CoordIDMin", CoordIDMin);

  if (this->m_CollectionName == "mesh")
    {
    SetTheTimePointForMesh(iTimePoint, iNewCollection, DatabaseConnector);
    }
  return this->CreateNewCollection(DatabaseConnector, iNewCollection);
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int GoDBCollectionOfTraces::GetCoordIDMinForBoundingBoxWithNoTraces(
  vtkMySQLDatabase* iDatabaseConnector)
{
  // As there is no traces in the collection, the bounding box is the minimum one:
  // CoordIDMin correspond to the imagingsession Max:
  return FindOneID(iDatabaseConnector, "imagingsession", "CoordIDMax",
                   "ImagingSessionID", ConvertToString<int>(this->m_ImgSessionID));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int GoDBCollectionOfTraces::GetCoordIDMaxForBoundingBoxWithNoTraces(
  vtkMySQLDatabase* iDatabaseConnector)
{
  // As there is no traces in the collection, the bounding box is the minimum one:
  // CoordIDMax correspond to the imagingsession Min:
  return FindOneID(iDatabaseConnector, "imagingsession", "CoordIDMin",
                   "ImagingSessionID", ConvertToString<int>(this->m_ImgSessionID));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::SetTheTimePointCoordinatesForMesh(
                              unsigned int iTimePoint, 
                              int &ioCoordIDMax,
                              int &ioCoordIDMin,
                              vtkMySQLDatabase* iDatabaseConnector)
{
  GoDBCoordinateRow Coordinate;
  Coordinate.SetValuesForSpecificID(ioCoordIDMax, iDatabaseConnector);
  Coordinate.SetField<int>("TCoord", iTimePoint);
  Coordinate.SetField("CoordID","0");
  ioCoordIDMax = Coordinate.SaveInDB(iDatabaseConnector);

 
  Coordinate.SetValuesForSpecificID(ioCoordIDMin, iDatabaseConnector);
  Coordinate.SetField<int>("TCoord", iTimePoint);
  Coordinate.SetField("CoordID", "0");
  ioCoordIDMin = Coordinate.SaveInDB(iDatabaseConnector);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int GoDBCollectionOfTraces::CreateNewCollection(
  vtkMySQLDatabase* DatabaseConnector, GoDBTraceRow& iNewCollection)
{
  if (this->m_CollectionName == "mesh")
    {
    GoDBMeshRow NewMesh;
    NewMesh.SafeDownCast(iNewCollection);

    return AddOnlyOneNewObjectInTable<GoDBMeshRow>(
             DatabaseConnector, this->m_CollectionName, &NewMesh, m_CollectionIDName);
    }
  if (this->m_CollectionName == "track")
    {
    GoDBTrackRow* NewTrack = static_cast<GoDBTrackRow*>(&iNewCollection);
    return AddOnlyOneNewObjectInTable<GoDBTrackRow>(
             DatabaseConnector, this->m_CollectionName, NewTrack, m_CollectionIDName);
    }
  if (this->m_CollectionName == "lineage")
    {
    GoDBLineageRow* NewLineage = static_cast<GoDBLineageRow*>(&iNewCollection);
    return AddOnlyOneNewObjectInTable<GoDBLineageRow>(
             DatabaseConnector, this->m_CollectionName, NewLineage, m_CollectionIDName);
    }
  return 0;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list<unsigned int> GoDBCollectionOfTraces::UpdateDBDataForAddedTracesToExistingCollection(
  std::list<unsigned int> iListSelectedTraces, int iNewCollectionID,
  vtkMySQLDatabase* iDatabaseConnector)
{
  std::list<unsigned int>           ListTraceIDWithBoundingBoxUpdated;
  std::vector<std::string> VectorSelectedTraces(iListSelectedTraces.size());
  std::list<unsigned int>::iterator iter = iListSelectedTraces.begin();
  unsigned int             i = 0;
  while (iter != iListSelectedTraces.end())
    {
    unsigned int temp = *iter;
    VectorSelectedTraces.at(i) = ConvertToString<unsigned int>(temp);
    i++;
    iter++;
    }
  //update the bounding boxes for the previous collection if the traces had one:
  //std::list<int>::iterator iter = iListSelectedTraces.begin();

  std::vector<std::string> ListCollectionIDWithBoundingBoxToUpdate =
    ListSpecificValuesForOneColumn(iDatabaseConnector, this->m_TracesName,
                                   this->m_CollectionIDName, this->m_TracesIDName, VectorSelectedTraces, true, true);
  //change the collectionID of the selected trace to the new one:
  iter = iListSelectedTraces.begin();
  while (iter != iListSelectedTraces.end())
    {
    this->UpdateCollectionIDOfSelectedTrace(*iter, iNewCollectionID,
                                            iDatabaseConnector);
    iter++;
    }

  if (!ListCollectionIDWithBoundingBoxToUpdate.empty())
    {

    //while (iter != iListSelectedTraces.end())
    //  {
    //  int tempTraceID = *iter;
    //  int tempCollectionID = FindOneID(iDatabaseConnector,this->m_TracesName,
    //    this->m_CollectionIDName,this->m_TracesIDName,
    //    ConvertToString<int>(tempTraceID));

    //change the collectionID of the selected trace to the new one:
    // this->UpdateCollectionIDOfSelectedTrace(tempTraceID,iNewCollectionID,
    //  iDatabaseConnector);
    //  if (tempCollectionID != 0)
    //    {
    //    this->RecalculateDBBoundingBox(iDatabaseConnector,tempCollectionID);
    //    ListTraceIDWithBoundingBoxUpdated.push_back(tempCollectionID);
    //    }
    //  iter++;
    //  }

    std::vector<std::string>::iterator iterVector =
      ListCollectionIDWithBoundingBoxToUpdate.begin();
    while (iterVector != ListCollectionIDWithBoundingBoxToUpdate.end())
      {
      std::string temp = *iterVector;
      this->RecalculateDBBoundingBox(iDatabaseConnector, atoi(temp.c_str()));
      ListTraceIDWithBoundingBoxUpdated.push_back(atoi(temp.c_str()));
      iterVector++;
      }
    }

  //Get the max and min coordid for the bounding box:
  int CoordMaxID;
  int CoordMinID;
  if (!iListSelectedTraces.empty())
    {
    CoordMaxID = this->GetCoordMaxID(iDatabaseConnector, iNewCollectionID, iListSelectedTraces);
    CoordMinID = this->GetCoordMinID(iDatabaseConnector, iNewCollectionID, iListSelectedTraces);
    }
  else
    {
    CoordMaxID = this->GetCoordIDMaxForBoundingBoxWithNoTraces(iDatabaseConnector);
    CoordMinID = this->GetCoordIDMinForBoundingBoxWithNoTraces(iDatabaseConnector);
    }

  //Update the bounding box for the collection where traces are added:
  this->UpdateBoundingBoxInDB(CoordMinID, CoordMaxID, iNewCollectionID,
                                        iDatabaseConnector);

  return ListTraceIDWithBoundingBoxUpdated;
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

  return "";
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::UpdateBoundingBoxInDB(int iCoordIDMin,
                                                        int iCoordIDMax,
                                                        int iTraceID,
                                                        vtkMySQLDatabase* iDatabaseConnector)
{
  //update the bounding box for the max coord:
  UpdateValueInDB(iDatabaseConnector, this->m_TracesName, "CoordIDMax",
                  ConvertToString<int>(iCoordIDMax), this->m_TracesIDName,
                  ConvertToString<int>(iTraceID));

  //update the bounding box for the min coord:
  UpdateValueInDB(iDatabaseConnector, this->m_TracesName, "CoordIDMin",
                  ConvertToString<int>(iCoordIDMin), this->m_TracesIDName,
                  ConvertToString<int>(iTraceID));
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list<int> GoDBCollectionOfTraces::GetTracesIDPartOfTheCollection(
  vtkMySQLDatabase* DatabaseConnector, int iCollectionID)
{
  std::vector<std::string> ResultsQuery = ListSpecificValuesForOneColumn(
    DatabaseConnector, this->m_TracesName, this->m_TracesIDName,
    this->m_CollectionIDName, ConvertToString<int>(iCollectionID));

  std::list<int> ListIDs;
  for (unsigned int i = 0; i < ResultsQuery.size(); i++)
    {
    ListIDs.push_back(atoi(ResultsQuery.at(i).c_str()));
    }
  return ListIDs;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list<unsigned int> GoDBCollectionOfTraces::GetListTracesIDsFromThisCollectionOf(
  vtkMySQLDatabase* iDatabaseConnector,std::list<unsigned int> iListTraces)
{
  std::list<unsigned int> ListTracesFromCollectionOf;
  if (this->m_CollectionOfName != "None")
    {
    std::vector<unsigned int> iVectorTraces( iListTraces.begin(), iListTraces.end() );
    ListTracesFromCollectionOf = ListSpecificValuesForOneColumn(iDatabaseConnector,
      this->m_CollectionOfName, this->m_CollectionOfIDName,
      this->m_TracesIDName, iVectorTraces);
    }
  return ListTracesFromCollectionOf;
}
//-------------------------------------------------------------------------

//------------------------------------------------------------------------
std::list<unsigned int> GoDBCollectionOfTraces::GetListCollectionIDs(
  vtkMySQLDatabase* iDatabaseConnector,std::list<unsigned int> iListTracesIDs)
{
  std::vector<std::string> VectorValues;
  std::list<unsigned int>::iterator iter = iListTracesIDs.begin();
  while (iter != iListTracesIDs.end())
    {
    int TraceID = *iter;
    VectorValues.push_back(ConvertToString<unsigned int>(TraceID));
    iter++;
    }
  std::vector<std::string> VectorCollectionIDs = ListSpecificValuesForOneColumn(
    iDatabaseConnector,this->m_TracesName,this->m_CollectionIDName,
    this->m_TracesIDName, VectorValues,true,true);

  /*std::vector<std::string>::iterator iterbis = VectorCollectionIDs.begin();
  std::list<unsigned int> oListCollectionIDs;
  while(iterbis != VectorCollectionIDs.end())
    {
    std::string StringID = *iterbis;
    oListCollectionIDs.push_back(ss_atoi<unsigned int>(StringID));
    iterbis++;
    }*/  
  //return oListCollectionIDs;
  return this->VectorStringToUnsgInt(VectorCollectionIDs);
}
//-------------------------------------------------------------------------

//------------------------------------------------------------------------
std::vector<std::string> GoDBCollectionOfTraces::
  ListUnsgIntToVectorString(std::list<unsigned int> iList)
{
  std::list<unsigned int>::iterator iter = iList.begin();
  std::vector<std::string> oVector;
  while(iter != iList.end())
    {
    unsigned int temp = *iter;
    oVector.push_back(ConvertToString<unsigned int>(temp));
    iter++;
    }
  return oVector;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<unsigned int> GoDBCollectionOfTraces::
  VectorStringToUnsgInt(std::vector<std::string> iVector)
{
  std::vector<std::string>::iterator iter = iVector.begin();
  std::list<unsigned int> oList;
  while(iter != iVector.end())
    {
    std::string temp = *iter;
    unsigned int tempint = ss_atoi<unsigned int>(temp);
    oList.push_back(tempint);
    iter++;
    }
  return oList;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<unsigned int> GoDBCollectionOfTraces::GetListTracesIDWithNoPoints(
  std::list<unsigned int> iListTracesIDs, vtkMySQLDatabase* iDatabaseConnector)
{
  std::vector<std::string> VectorTracesIDs =
    this->ListUnsgIntToVectorString(iListTracesIDs);
  return GetSpecificValuesEqualToZero(
    iDatabaseConnector,this->m_TracesIDName, this->m_TracesName,
    this->m_TracesIDName, VectorTracesIDs,"points");
}
//********************************FOR TABLE WIDGET***************************
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*void GoDBCollectionOfTraces::FillRowContainerForComputedValues(
  GoDBTableWidgetContainer* iLinkToRowContainer,
  std::vector<std::vector<std::string> >* iComputedValues)
{
  std::vector<std::string> ListComputedNames =
    iLinkToRowContainer->GetNameComputedColumns();
  if (!ListComputedNames.empty())
    {
    std::vector<std::vector<std::string> > ComputedValues;
    if (iComputedValues == 0 && !ListComputedNames.empty())
      {
      for (unsigned int j = 0; j < iLinkToRowContainer->GetNumberOfRows(); j++)
        {
        std::vector<std::string> EmptyVector;
//         int Size = iLinkToRowContainer->GetNumberOfRows();
        for (unsigned int i = 0; i < ListComputedNames.size(); i++)
          {
          EmptyVector.push_back("NV");
          }
        ComputedValues.push_back(EmptyVector);
        EmptyVector.clear();
        }
      }
    else
      {
      ComputedValues = *iComputedValues;
      }
    iLinkToRowContainer->FillRowContainer(
      ComputedValues, ListComputedNames, "ColumnNameTableWidget");
    }
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*void GoDBCollectionOfTraces::FillRowContainerForIntensityValues(
  vtkMySQLDatabase* DatabaseConnector, std::vector<std::string> iVectMeshIDs,
  GoDBTableWidgetContainer* iLinkToRowContainer)
{
  std::vector<std::vector<std::string> > ResultsFromQuery;
  std::vector<std::string>               SelectFields;
  std::vector<std::vector<std::string> > ChannelsInfo =
    iLinkToRowContainer->GetChannelsInfo();

  for (unsigned int i = 0; i < ChannelsInfo.size(); i++)
    {
    std::string NameTableWidgetColumn = "T.I.";
    NameTableWidgetColumn += ChannelsInfo.at(i).at(0);
    SelectFields.push_back(NameTableWidgetColumn);
    }
  std::vector<std::string>::iterator iter = iVectMeshIDs.begin();
  std::vector<std::string>           temp;
  while (iter != iVectMeshIDs.end())
    {
    for (unsigned int i = 0; i < ChannelsInfo.size(); i++)
      {
      std::string ChannelID = ChannelsInfo.at(i).at(1);
      std::string MeshID = *iter;
      int         ValueIntensity = FindOneID(DatabaseConnector,
                                             "intensity", "Value", "MeshID", MeshID,
                                             "ChannelID", ChannelID);
      temp.push_back(ConvertToString<int>(ValueIntensity));
      }
    ResultsFromQuery.push_back(temp);
    temp.clear();
    iter++;
    }
  iLinkToRowContainer->FillRowContainer(
    ResultsFromQuery, SelectFields, "ColumnNameTableWidget");
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*void GoDBCollectionOfTraces::SetChannelsInfo(vtkMySQLDatabase* DatabaseConnector,
                                             GoDBTableWidgetContainer* iLinkToRowContainer)
{
  std::vector<std::string> SelectFields;
  //GoDBTableWidgetContainer* LinkToRowContainer = iLinkToRowContainer;
  if (iLinkToRowContainer == 0)
    {
    iLinkToRowContainer = this->m_LinkToRowContainer;
    }
  SelectFields.push_back("Name");
  SelectFields.push_back("channel.ChannelID");
  std::vector<std::string> JoinTablesOnTraceTable;
  JoinTablesOnTraceTable.push_back("channel");
  JoinTablesOnTraceTable.push_back("image.ChannelID = channel.ChannelID");

  std::vector<std::vector<std::string> > Results = GetValuesFromSeveralTables(
    DatabaseConnector, "image", SelectFields, "ImagingSessionID",
    ConvertToString<unsigned int>(this->m_ImgSessionID), JoinTablesOnTraceTable, true);
  //  iLinkToRowContainer->SetChannelsInfo(Results);
  iLinkToRowContainer->SetSpecificColumnsInfoForMesh(Results);
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*GoDBTableWidgetContainer* GoDBCollectionOfTraces::GetLinkToUpdatedTraceContainer(
  vtkMySQLDatabase* iDatabaseConnector, int iUpdatedTraceID)
{
  GoDBTableWidgetContainer* LinkToUpdatedTraceContainer = new
                                                          GoDBTableWidgetContainer(this->m_CollectionName,
                                                                                   this->m_TracesName);

  /*first, get the right parts of the first query:
 all the fields except the ones where table.field are already in the query:
  std::vector<std::string> JoinFirstTablesOnTraceTable =
    m_LinkToRowContainer->GetQueryStringForTraceJoinedTables(false);
  std::vector<std::string> SelectFirstFields =
    m_LinkToRowContainer->GetQueryStringForSelectFieldsTables(false);

  //then, get the results of the first query:
  std::vector<std::vector<std::string> > ResultsFirstQuery = GetValuesFromSeveralTables(
    iDatabaseConnector, this->m_TracesName, SelectFirstFields, this->m_TracesIDName,
    ConvertToString<int>(iUpdatedTraceID), JoinFirstTablesOnTraceTable, false);

  //insert into the row container, the results of the first query:
  // m_LinkToRowContainer->FillRowContainer(ResultsFirstQuery,SelectFirstFields);
  LinkToUpdatedTraceContainer->FillRowContainer(ResultsFirstQuery, SelectFirstFields);

  //Get the right parts of the second query (with only the remaining fields):
  std::vector<std::string> JoinSecondTablesOnTraceTable =
    m_LinkToRowContainer->GetQueryStringForTraceJoinedTables(true);
  std::vector<std::string> SelectSecondFields =
    m_LinkToRowContainer->GetQueryStringForSelectFieldsTables(true);

  //then, get the results of the second query:
  std::vector<std::vector<std::string> > ResultsSecondQuery = GetValuesFromSeveralTables(
    iDatabaseConnector, this->m_TracesName, SelectSecondFields, this->m_TracesIDName,
    ConvertToString<int>(iUpdatedTraceID), JoinSecondTablesOnTraceTable, false);

  //insert into the row container, the results of the second query:
  LinkToUpdatedTraceContainer->FillRowContainer(ResultsSecondQuery, SelectSecondFields);

  if (this->m_TracesName == "mesh")
    {
    std::vector<std::string> TraceIDs;
    TraceIDs.push_back(ConvertToString<int>(iUpdatedTraceID));
    this->SetChannelsInfo(iDatabaseConnector, LinkToUpdatedTraceContainer);
    this->FillRowContainerForIntensityValues(iDatabaseConnector, TraceIDs,
                                             LinkToUpdatedTraceContainer);
    }

  return LinkToUpdatedTraceContainer;
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*GoDBTableWidgetContainer* GoDBCollectionOfTraces::GetLinkToNewCreatedTraceContainer(
  vtkMySQLDatabase* iDatabaseConnector, int iTraceID)
{
  GoDBTableWidgetContainer* LinkToNewCreatedTraceContainer = new
                                                             GoDBTableWidgetContainer(this->m_CollectionName,
                                                                                      this->m_TracesName);

  /*if(this->m_TracesName == "mesh")
     {
     //std::vector<std::string> TraceID;
     //TraceID.push_back(ConvertToString<int>(NewTraceID));
     this->SetChannelsInfo(iDatabaseConnector,LinkToNewCreatedTraceContainer);
     //this->FillRowContainerForIntensityValues(iDatabaseConnector,TraceID,
     //  LinkToNewCreatedTraceContainer);
     }*/

  /*first, get the right parts of the first query:
 all the fields except the ones where table.field are already in the query:
  std::vector<std::string> JoinFirstTablesOnTraceTable =
    m_LinkToRowContainer->GetQueryStringForTraceJoinedTables(false);
  std::vector<std::string> SelectFirstFields =
    m_LinkToRowContainer->GetQueryStringForSelectFieldsTables(false);

  int NewTraceID = iTraceID;
  if (NewTraceID == 0)
    {
    //then, get the last ID in the database, corresponding to the new created trace:
    NewTraceID = MaxValueForOneColumnInTable(iDatabaseConnector,
                                             this->m_TracesIDName, this->m_TracesName, "ImagingSessionID",
                                             ConvertToString<unsigned int>(this->m_ImgSessionID));
    }

  //then, get the results of the first query:
  std::vector<std::vector<std::string> > ResultsFirstQuery = GetValuesFromSeveralTables(
    iDatabaseConnector, this->m_TracesName, SelectFirstFields, this->m_TracesIDName,
    ConvertToString<int>(NewTraceID), JoinFirstTablesOnTraceTable, false);

  //insert into the row container, the results of the first query:
  // m_LinkToRowContainer->FillRowContainer(ResultsFirstQuery,SelectFirstFields);
  LinkToNewCreatedTraceContainer->FillRowContainer(ResultsFirstQuery, SelectFirstFields);

  //Get the right parts of the second query (with only the remaining fields):
  std::vector<std::string> JoinSecondTablesOnTraceTable =
    m_LinkToRowContainer->GetQueryStringForTraceJoinedTables(true);
  std::vector<std::string> SelectSecondFields =
    m_LinkToRowContainer->GetQueryStringForSelectFieldsTables(true);

  //then, get the results of the second query:
  std::vector<std::vector<std::string> > ResultsSecondQuery = GetValuesFromSeveralTables(
    iDatabaseConnector, this->m_TracesName, SelectSecondFields, this->m_TracesIDName,
    ConvertToString<int>(NewTraceID), JoinSecondTablesOnTraceTable, false);

  //insert into the row container, the results of the second query:
  //m_LinkToRowContainer->FillRowContainer(ResultsSecondQuery,SelectSecondFields);
  LinkToNewCreatedTraceContainer->FillRowContainer(ResultsSecondQuery, SelectSecondFields);

  if (this->m_TracesName == "mesh")
    {
    std::vector<std::string> TraceID;
    TraceID.push_back(ConvertToString<int>(NewTraceID));
    this->SetChannelsInfo(iDatabaseConnector, LinkToNewCreatedTraceContainer);
    this->FillRowContainerForIntensityValues(iDatabaseConnector, TraceID,
                                             LinkToNewCreatedTraceContainer);
    }
  return LinkToNewCreatedTraceContainer;
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*std::list<std::string> GoDBCollectionOfTraces::
GetListColumnsNamesForTableWidget()
{
  return m_LinkToRowContainer->GetListColumnsNamesForTableWidget();
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*GoDBCollectionOfTraces::DBTableWidgetContainerType
GoDBCollectionOfTraces::GetRowContainer(vtkMySQLDatabase* DatabaseConnector)
{
  /*first, get the right parts of the first query:
  all the fields except the ones where table.field are already in the query:
  std::vector<std::string> JoinFirstTablesOnTraceTable =
    m_LinkToRowContainer->GetQueryStringForTraceJoinedTables(false);
  std::vector<std::string> SelectFirstFields =
    m_LinkToRowContainer->GetQueryStringForSelectFieldsTables(false);

  //then, get the results of the first query:
  std::vector<std::vector<std::string> > ResultsFirstQuery = GetValuesFromSeveralTables(
    DatabaseConnector, this->m_TracesName, SelectFirstFields, "ImagingSessionID",
    ConvertToString<unsigned int>(this->m_ImgSessionID), JoinFirstTablesOnTraceTable, true);

  //fill the row container with the results of the first query:
  m_LinkToRowContainer->FillRowContainer(ResultsFirstQuery, SelectFirstFields);

  //Get the right parts of the second query (with only the remaining fields):
  std::vector<std::string> JoinSecondTablesOnTraceTable =
    m_LinkToRowContainer->GetQueryStringForTraceJoinedTables(true);
  std::vector<std::string> SelectSecondFields =
    m_LinkToRowContainer->GetQueryStringForSelectFieldsTables(true);

  //then, get the results of the second query:
  std::vector<std::vector<std::string> > ResultsSecondQuery = GetValuesFromSeveralTables(
    DatabaseConnector, this->m_TracesName, SelectSecondFields, "ImagingSessionID",
    ConvertToString<unsigned int>(this->m_ImgSessionID), JoinSecondTablesOnTraceTable, false);

  //fill the row container with the results of the second query:
  m_LinkToRowContainer->FillRowContainer(ResultsSecondQuery, SelectSecondFields);

  if (this->m_TracesName == "mesh")
    {
    this->FillRowContainerForIntensityValues(DatabaseConnector,
                                             ListSpecificValuesForOneColumn(DatabaseConnector, "mesh", "MeshID",
                                                                            "ImagingSessionID",
                                                                            ConvertToString<unsigned int>(this->
                                                                                                          m_ImgSessionID)),
                                             this->m_LinkToRowContainer);
    }
  /** \todo fill the container with the computed values
  this->FillRowContainerForComputedValues(m_LinkToRowContainer);

  return m_LinkToRowContainer->GetRowContainer();
}*/
