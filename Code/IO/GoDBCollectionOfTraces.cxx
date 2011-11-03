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
#include <algorithm>

GoDBCollectionOfTraces::GoDBCollectionOfTraces()
{
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCollectionOfTraces::GoDBCollectionOfTraces(
  std::string iCollectionName, std::string iTracesName,
  std::string iCollectionOfName, unsigned int iImgSessionID)
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
void GoDBCollectionOfTraces::DeleteTracesInDB(std::list< unsigned int > TracesToDelete,
                                              vtkMySQLDatabase *DatabaseConnector)
{
  std::list< unsigned int >::iterator iter = TracesToDelete.begin();
  while ( iter != TracesToDelete.end() )
    {
    unsigned int ID = *iter;

    // if the trace is a mesh, we should delete its intensity as well
    if(m_TracesName.compare("mesh") == 0)
      {
      // delete the related intensity table
      DeleteRow(DatabaseConnector, "intensity" , m_TracesIDName,
                ConvertToString< unsigned int >(ID));
      }

    DeleteRow( DatabaseConnector, m_TracesName, m_TracesIDName,
      ConvertToString< unsigned int >(ID) );
    ++iter;
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//Modif into Database
void GoDBCollectionOfTraces::DeleteTraceInDB(int TraceToDelete,
                                             vtkMySQLDatabase *DatabaseConnector)
{
  DeleteRow( DatabaseConnector, m_TracesName, m_TracesIDName,
             ConvertToString< int >(TraceToDelete) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//Modif into Database
void GoDBCollectionOfTraces::UpdateCollectionIDOfSelectedTraces(
  std::list< unsigned int > iListSelectedTraces, unsigned int inewCollectionID,
  vtkMySQLDatabase *DatabaseConnector)
{
  std::string newCollectionIDstring = ConvertToString< unsigned int >(inewCollectionID);

  std::list< unsigned int >::iterator iter = iListSelectedTraces.begin();
  while ( iter != iListSelectedTraces.end() )
    {
    unsigned int TraceID = *iter;
    UpdateValueInDB( DatabaseConnector, m_TracesName, m_CollectionIDName,
                     newCollectionIDstring, m_TracesIDName,
                     ConvertToString< unsigned int >(TraceID) );
    ++iter;
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//Modif into Database
void GoDBCollectionOfTraces::UpdateCollectionIDOfSelectedTrace(
  int iSelectedTraceID, int inewCollectionID,
  vtkMySQLDatabase *DatabaseConnector)
{
  UpdateValueInDB( DatabaseConnector, m_TracesName, m_CollectionIDName,
                   ConvertToString< int >(inewCollectionID), m_TracesIDName,
                   ConvertToString< int >(iSelectedTraceID) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::UpdateValueForListTraces(
  vtkMySQLDatabase *iDatabaseConnector, std::string iNameValue,
  std::string iValue, std::list< unsigned int > iListTraceIDs)
{
  std::vector< unsigned int > VectIDs;
  std::copy( iListTraceIDs.begin(), iListTraceIDs.end(), std::back_inserter(VectIDs) );
  UpdateValueInDB(iDatabaseConnector, this->m_TracesName, iNameValue,
                  iValue, this->m_TracesIDName, VectIDs);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//Modif into Database
void GoDBCollectionOfTraces::RecalculateDBBoundingBox(
  vtkMySQLDatabase *iDatabaseConnector, int iCollectionID)
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
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTracesIDs)
{
  std::list< unsigned int >::iterator iter = iListTracesIDs.begin();
  while ( iter != iListTracesIDs.end() )
    {
    this->RecalculateDBBoundingBox(iDatabaseConnector, *iter);
    ++iter;
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< GoDBCollectionOfTraces::NameWithColorData >
GoDBCollectionOfTraces::GetAllTracesIDsWithColor(
  vtkMySQLDatabase *iDatabaseConnector)
{
  //First, build the query with selected fields and table to join with on
  // conditions:
  std::vector< std::string > SelectFields;
  std::vector< std::string > JoinTablesOnTraceTable;
  this->GetFieldsNeededForQueryForColorData(SelectFields, JoinTablesOnTraceTable);
  std::vector< std::vector< std::string > > ResultsQuery;

  ResultsQuery  = GetValuesFromSeveralTables(iDatabaseConnector,
                                             this->m_TracesName, SelectFields,
                                             "ImagingSessionID",
                                             ConvertToString< unsigned int >(
                                               this->m_ImgSessionID),
                                             JoinTablesOnTraceTable, true);

  return this->GetListNameWithColorDataFromResultsQuery(ResultsQuery);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< GoDBCollectionOfTraces::NameWithColorData >
GoDBCollectionOfTraces::GetTracesIDsWithColorForATimePoint(
  vtkMySQLDatabase *iDatabaseConnector, unsigned int iTimePoint)
{
  std::list< NameWithColorData > oListTraceIDs;
  //First, build the query with selected fields and table to join with on
  // conditions:
  std::vector< std::string > SelectFields;
  std::vector< std::string > JoinTablesOnTraceTable;
  this->GetFieldsNeededForQueryForColorData(SelectFields, JoinTablesOnTraceTable);

  std::string JoinTable = "coordinate";
  JoinTablesOnTraceTable.push_back(JoinTable);
  std::string OnCondition = this->m_TracesName;
  OnCondition += ".";
  OnCondition += "CoordIDMin = coordinate.coordid";
  JoinTablesOnTraceTable.push_back(OnCondition);
  std::vector< std::string > WhereAndConditions;
  OnCondition = this->m_TracesName;
  OnCondition += ".ImagingsessionID";
  WhereAndConditions.push_back(OnCondition);
  WhereAndConditions.push_back( ConvertToString< unsigned int >(this->m_ImgSessionID) );
  WhereAndConditions.push_back("coordinate.TCoord");
  WhereAndConditions.push_back( ConvertToString< int >(iTimePoint) );

  std::vector< std::vector< std::string > > ResultsQuery = GetValuesFromSeveralTables(
      iDatabaseConnector,
      this->m_TracesName, SelectFields,
      WhereAndConditions,
      JoinTablesOnTraceTable, true);

  return this->GetListNameWithColorDataFromResultsQuery(ResultsQuery);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::GetFieldsNeededForQueryForColorData(
  std::vector< std::string > & ioSelectedFields,
  std::vector< std::string > & ioJoinTablesOnTraceTable)
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

  std::string JoinTable = "color";
  ioJoinTablesOnTraceTable.push_back(JoinTable);
  std::string OnCondition = this->m_TracesName;
  OnCondition += ".ColorID = color.ColorID";
  ioJoinTablesOnTraceTable.push_back(OnCondition);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< GoDBCollectionOfTraces::NameWithColorData >
GoDBCollectionOfTraces::GetListNameWithColorDataFromResultsQuery(
  std::vector< std::vector< std::string > > iResultsQuery)
{
  std::list< NameWithColorData >                      oListNameWithColorData;
  unsigned int                                        i = 0;
  std::vector< std::vector< std::string > >::iterator iter = iResultsQuery.begin();
  while ( iter != iResultsQuery.end() )
    {
    std::vector< std::string > ResultsOneRow = *iter;

    int intRed   = atoi( ResultsOneRow[i + 1].c_str() );
    int intGreen = atoi( ResultsOneRow[i + 2].c_str() );
    int intBlue  = atoi( ResultsOneRow[i + 3].c_str() );
    int intAlpha = atoi( ResultsOneRow[i + 4].c_str() );

    QColor                     Color(intRed, intGreen, intBlue, intAlpha);
    NameWithColorData          temp;
    temp.first = ResultsOneRow[i];
    temp.second = Color;
    oListNameWithColorData.push_back(temp);
    ++iter;
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
int GoDBCollectionOfTraces::GetCoordMinID(vtkMySQLDatabase *iDatabaseConnector,
                                          int iTraceID)
{
  //Get the list of the collectionof tracesID belonging to the iTraceID:
  std::list< unsigned int > ListTracesIDs;
  ListTracesIDs.push_back(iTraceID);
  std::list< unsigned int > ListCollectionOfTraces =
    this->GetListTracesIDsFromThisCollectionOf(iDatabaseConnector, ListTracesIDs);

  if ( ListCollectionOfTraces.empty() )
    {
    return this->GetCoordIDMinForBoundingBoxWithNoTraces(iDatabaseConnector);
    }
  else
    {
    //Get the min of the traces:
    //std::vector< std::string > VectorCollectionOfTraces =
    //ListUnsgIntToVectorString(ListCollectionOfTraces);
    GoDBCoordinateRow TracesCoordMin = this->GetCollectionOfTracesCoordMin(
        iDatabaseConnector, ListCollectionOfTraces);
    return TracesCoordMin.SaveInDB(iDatabaseConnector);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//get from Database and/or Modif into Database
int GoDBCollectionOfTraces::GetCoordMaxID(vtkMySQLDatabase *iDatabaseConnector,
                                          int iTraceID)
{
  //Get the list of the tracesID belonging to the collection:
  std::list< unsigned int > ListTracesIDs;
  ListTracesIDs.push_back(iTraceID);
  std::list< unsigned int > ListCollectionOfTraces =
    this->GetListTracesIDsFromThisCollectionOf(iDatabaseConnector, ListTracesIDs);

  if ( ListCollectionOfTraces.empty() )
    {
    return this->GetCoordIDMaxForBoundingBoxWithNoTraces(iDatabaseConnector);
    }
  else
    {
    //Get the max of the traces:
    //std::vector< std::string > VectorCollectionOfTraces =
    // ListUnsgIntToVectorString(ListCollectionOfTraces);
    GoDBCoordinateRow TracesCoordMax = this->GetCollectionOfTracesCoordMax(
        iDatabaseConnector, ListCollectionOfTraces);
    return TracesCoordMax.SaveInDB(iDatabaseConnector);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//get from Database and/or Modif into Database
GoDBCoordinateRow GoDBCollectionOfTraces::GetCollectionOfTracesCoordMin(
  vtkMySQLDatabase *DatabaseConnector,
  std::list< unsigned int > iListCollectionOfTracesID)
{
  GoDBCoordinateRow CoordMin;

  //First, get the coordID in the contour table that corresponds to the
  //coordIDMin of the Contours selected:
  std::list< unsigned int > ListCollectionOfTracesCoordIDMin =
    ListSpecificValuesForOneColumn(DatabaseConnector,
                                   this->m_CollectionOfName, "CoordIDMin", this->m_CollectionOfIDName,
                                   iListCollectionOfTracesID);
  //then, go to the coordinate table and compare the values for the coordID
  //corresponding to the coordIDMax of the selected contours:
  std::vector< std::string > ColumnNames = CoordMin.GetVectorColumnNames();
  for ( unsigned int i = 0; i < ColumnNames.size(); i++ )
    {
    //don't compare the coordID !!!:
    if ( ColumnNames[i] != "CoordID" )
      {
      std::vector< std::string > VectorCollectionOfTracesCoordIDMin = ListUnsgIntToVectorString(
          ListCollectionOfTracesCoordIDMin);
      CoordMin.SetField( ColumnNames[i], MinValueForOneColumnInTable(
                           DatabaseConnector, ColumnNames[i], "coordinate", "CoordID",
                           VectorCollectionOfTracesCoordIDMin) );
      }
    }
  return CoordMin;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCoordinateRow GoDBCollectionOfTraces::GetCollectionOfTracesCoordMax(
  vtkMySQLDatabase *DatabaseConnector,
  std::list< unsigned int > iListCollectionOfTracesID)
{
  GoDBCoordinateRow CoordMax;

  //First, get the coordID in the contour table that corresponds to the
  //coordIDMax of the Contours selected:
  std::list< unsigned int > ListCollectionOfTracesCoordIDMax =
    ListSpecificValuesForOneColumn(DatabaseConnector,
                                   this->m_CollectionOfName, "CoordIDMax",
                                   this->m_CollectionOfIDName,
                                   iListCollectionOfTracesID);
  //then, go to the coordinate table and compare the values for the coordID
  //corresponding to the coordIDMax of the selected contours:
  std::vector< std::string > ColumnNames = CoordMax.GetVectorColumnNames();
  for ( unsigned int i = 0; i < ColumnNames.size(); i++ )
    {
    //don't compare the coordID !!!:
    if ( ColumnNames[i] != "CoordID" )
      {
      std::vector< std::string > VectorCollectionOfTracesCoordIDMax = ListUnsgIntToVectorString(
          ListCollectionOfTracesCoordIDMax);
      CoordMax.SetField( ColumnNames[i], MaxValueForOneColumnInTable(
                           DatabaseConnector, ColumnNames[i], "coordinate", "CoordID",
                           VectorCollectionOfTracesCoordIDMax) );
      }
    }
  return CoordMax;
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
  vtkMySQLDatabase *iDatabaseConnector)
{
  // As there is no traces in the collection, the bounding box is the minimum
  // one:
  // CoordIDMin correspond to the imagingsession Max:
  return FindOneID( iDatabaseConnector, "imagingsession", "CoordIDMax",
                    "ImagingSessionID", ConvertToString< int >(this->m_ImgSessionID) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int GoDBCollectionOfTraces::GetCoordIDMaxForBoundingBoxWithNoTraces(
  vtkMySQLDatabase *iDatabaseConnector)
{
  // As there is no traces in the collection, the bounding box is the minimum
  // one:
  // CoordIDMax correspond to the imagingsession Min:
  return FindOneID( iDatabaseConnector, "imagingsession", "CoordIDMin",
                    "ImagingSessionID", ConvertToString< int >(this->m_ImgSessionID) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::SetTheTimePointCoordinatesForMesh(
  unsigned int iTimePoint,
  int & ioCoordIDMax,
  int & ioCoordIDMin,
  vtkMySQLDatabase *iDatabaseConnector)
{
  GoDBCoordinateRow Coordinate;

  Coordinate.SetValuesForSpecificID(ioCoordIDMax, iDatabaseConnector);
  Coordinate.SetField< int >("TCoord", iTimePoint);
  Coordinate.SetField("CoordID", "0");
  ioCoordIDMax = Coordinate.SaveInDB(iDatabaseConnector);

  Coordinate.SetValuesForSpecificID(ioCoordIDMin, iDatabaseConnector);
  Coordinate.SetField< int >("TCoord", iTimePoint);
  Coordinate.SetField("CoordID", "0");
  ioCoordIDMin = Coordinate.SaveInDB(iDatabaseConnector);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int GoDBCollectionOfTraces::CreateNewCollection(
  vtkMySQLDatabase *DatabaseConnector, GoDBTraceRow & iNewCollection)
{
  if ( this->m_CollectionName == "mesh" )
    {
    GoDBMeshRow NewMesh;
    NewMesh.SafeDownCast(iNewCollection);

    return AddOnlyOneNewObjectInTable< GoDBMeshRow >(
             DatabaseConnector, this->m_CollectionName, &NewMesh, m_CollectionIDName);
    }
  if ( this->m_CollectionName == "track" )
    {
    GoDBTrackRow *NewTrack = static_cast< GoDBTrackRow * >( &iNewCollection );
    return AddOnlyOneNewObjectInTable< GoDBTrackRow >(
             DatabaseConnector, this->m_CollectionName, NewTrack, m_CollectionIDName);
    }
  if ( this->m_CollectionName == "lineage" )
    {
    GoDBLineageRow *NewLineage = static_cast< GoDBLineageRow * >( &iNewCollection );
    return AddOnlyOneNewObjectInTable< GoDBLineageRow >(
             DatabaseConnector, this->m_CollectionName, NewLineage, m_CollectionIDName);
    }
  return 0;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*std::list< unsigned int > GoDBCollectionOfTraces::UpdateDBDataForAddedTracesToExistingCollection(
  std::list< unsigned int > iListSelectedTraces, int iNewCollectionID,
  vtkMySQLDatabase *iDatabaseConnector)
{
  std::list< unsigned int >           ListTraceIDWithBoundingBoxUpdated;
  std::vector< std::string >          VectorSelectedTraces( iListSelectedTraces.size() );
  std::list< unsigned int >::iterator iter = iListSelectedTraces.begin();
  unsigned int                        i = 0;
  while ( iter != iListSelectedTraces.end() )
    {
    unsigned int temp = *iter;
    VectorSelectedTraces.at(i) = ConvertToString< unsigned int >(temp);
    i++;
    iter++;
    }
  //update the bounding boxes for the previous collection if the traces had one:
  //std::list<int>::iterator iter = iListSelectedTraces.begin();

  std::vector< std::string > ListCollectionIDWithBoundingBoxToUpdate =
    ListSpecificValuesForOneColumn(iDatabaseConnector, this->m_TracesName,
                                   this->m_CollectionIDName, this->m_TracesIDName, VectorSelectedTraces, true, true);
  //change the collectionID of the selected trace to the new one:
  iter = iListSelectedTraces.begin();
  while ( iter != iListSelectedTraces.end() )
    {
    this->UpdateCollectionIDOfSelectedTrace(*iter, iNewCollectionID,
                                            iDatabaseConnector);
    iter++;
    }

  if ( !ListCollectionIDWithBoundingBoxToUpdate.empty() )
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

    std::vector< std::string >::iterator iterVector =
      ListCollectionIDWithBoundingBoxToUpdate.begin();
    while ( iterVector != ListCollectionIDWithBoundingBoxToUpdate.end() )
      {
      std::string temp = *iterVector;
      this->RecalculateDBBoundingBox( iDatabaseConnector, atoi( temp.c_str() ) );
      ListTraceIDWithBoundingBoxUpdated.push_back( atoi( temp.c_str() ) );
      iterVector++;
      }
    }

  //Get the max and min coordid for the bounding box:
  int CoordMaxID;
  int CoordMinID;
  if ( !iListSelectedTraces.empty() )
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
}*/

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string GoDBCollectionOfTraces::GetCollectionOf()
{
  if ( this->m_TracesName == "contour" )
    {
    return "";
    }
  if ( this->m_TracesName == "mesh" )
    {
    return "contour";
    }
  if ( this->m_TracesName == "track" )
    {
    return "mesh";
    }
  if ( this->m_TracesName == "lineage" )
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
                                                   vtkMySQLDatabase *iDatabaseConnector)
{
  //update the bounding box for the max coord:
  UpdateValueInDB( iDatabaseConnector, this->m_TracesName, "CoordIDMax",
                   ConvertToString< int >(iCoordIDMax), this->m_TracesIDName,
                   ConvertToString< int >(iTraceID) );

  //update the bounding box for the min coord:
  UpdateValueInDB( iDatabaseConnector, this->m_TracesName, "CoordIDMin",
                   ConvertToString< int >(iCoordIDMin), this->m_TracesIDName,
                   ConvertToString< int >(iTraceID) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list< unsigned int > GoDBCollectionOfTraces::GetListTracesIDsFromThisCollectionOf(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTraces)
{
  std::list< unsigned int > ListTracesFromCollectionOf = std::list< unsigned int >();
  if ( this->m_CollectionOfName != "None" )
    {
    ListTracesFromCollectionOf = ListSpecificValuesForOneColumn(iDatabaseConnector,
                                                                this->m_CollectionOfName, this->m_CollectionOfIDName,
                                                                this->m_TracesIDName, iListTraces);
    }
  return ListTracesFromCollectionOf;
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------
std::list< unsigned int > GoDBCollectionOfTraces::GetListCollectionIDs(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTracesIDs,
  bool ExcludeZero, bool Distinct)
{
  std::list< unsigned int > ListCollectionIDs = std::list< unsigned int >();
  if ( this->m_CollectionName != "None" )
    {
    ListCollectionIDs = ListSpecificValuesForOneColumn(
        iDatabaseConnector, this->m_TracesName, this->m_CollectionIDName,
        this->m_TracesIDName, iListTracesIDs, Distinct, ExcludeZero);
    }
  return ListCollectionIDs;
}

//-------------------------------------------------------------------------

//------------------------------------------------------------------------

std::list< unsigned int > GoDBCollectionOfTraces::GetListTracesIDWithNoPoints(
  std::list< unsigned int > iListTracesIDs, vtkMySQLDatabase *iDatabaseConnector)
{
  std::vector< std::string > VectorTracesIDs =
    ListUnsgIntToVectorString(iListTracesIDs);

  if ( !VectorTracesIDs.empty() )
    {
    return GetSpecificValuesEqualToZero(
             iDatabaseConnector, this->m_TracesIDName, this->m_TracesName,
             VectorTracesIDs, "points");
    }
  else
    {
    return std::list< unsigned int >();
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > GoDBCollectionOfTraces::GetLastCreatedTracesIDs(
  vtkMySQLDatabase *iDatabaseConnector, int iNumberOfTraces)
{
  std::vector< std::string > VectorTracesIDs = GetOrderByWithLimit(
      iDatabaseConnector, this->m_TracesIDName, this->m_TracesName,
      "imagingsessionid", ConvertToString< unsigned int >(this->m_ImgSessionID),
      false, ConvertToString< int >(iNumberOfTraces) );

  return VectorStringToUnsgInt(VectorTracesIDs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< double * > GoDBCollectionOfTraces::GetCoordinateCenterBoundingBox(
  vtkMySQLDatabase *iDatabaseConnector, unsigned int iTraceID)
{
  return GetCenterBoundingBoxes( iDatabaseConnector,
                                 this->m_CollectionOfName, this->m_TracesIDName,
                                 ConvertToString< unsigned int >(iTraceID) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int >  GoDBCollectionOfTraces::GetTraceIDsWithTimePointAndCollectionID(
  vtkMySQLDatabase *iDatabaseConnector,
  unsigned int iCollectionID,
  unsigned int iTimePoint)
{
  FieldWithValue JoinCondition = { "CoordIDMin", "CoordID", "=" };

  std::vector< FieldWithValue > Conditions(2);
  FieldWithValue                CollectionID = { this->m_CollectionIDName, ConvertToString< unsigned int >(
                                                   iCollectionID), "=" };
  Conditions[0] = CollectionID;
  FieldWithValue TimePoint = { "TCoord", ConvertToString< unsigned int >(iTimePoint), "=" };
  Conditions[1] = TimePoint;

  return GetAllSelectedValuesFromTwoTables(
           iDatabaseConnector, this->m_TracesName, "coordinate", this->m_TracesIDName, JoinCondition, Conditions);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > GoDBCollectionOfTraces::GetTimePointWithSeveralTracesFromTheList(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTraceIDs)
{
  std::list< unsigned int > TimePoints = std::list< unsigned int >();
  if ( !iListTraceIDs.empty() )
    {
    FieldWithValue             JoinCondition = { "CoordIDMin", "CoordID", "=" };
    std::vector< std::string > VectTraceIDs = ListUnsgIntToVectorString(iListTraceIDs);

    TimePoints = GetDoublonValuesFromTwoTables(
        iDatabaseConnector, this->m_TracesName, "coordinate",
        "TCoord", JoinCondition, this->m_TracesIDName, VectTraceIDs);
    }
  return TimePoints;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBCollectionOfTraces::GetMaxTraceIDsForSpecificTimePoint(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTraceIDs,
  unsigned int iTimePoint)
{
  int oMaxTraceID = -1;

  if ( !iListTraceIDs.empty() )
    {
    FieldWithValue             JoinCondition = { "CoordIDMin", "CoordID", "=" };
    std::vector< std::string > VectTraceIDs = ListUnsgIntToVectorString(iListTraceIDs);
    FieldWithValue             TCoord = { "TCoord", ConvertToString< unsigned int >(iTimePoint), "=" };
    oMaxTraceID = GetMaxValueFromTwoTables(iDatabaseConnector,
                                           this->m_TracesName,
                                           "coordinate",
                                           this->m_TracesIDName,
                                           JoinCondition,
                                           this->m_TracesIDName,
                                           VectTraceIDs,
                                           TCoord);
    }
  return oMaxTraceID;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > GoDBCollectionOfTraces::GetNonMaxTraceIDsForSpecificTimePoint(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTraceIDs,
  unsigned int iTimePoint, unsigned int iMaxTraceID)
{
  std::list< unsigned int >::iterator iter = std::find(iListTraceIDs.begin(),
                                                       iListTraceIDs.end(), iMaxTraceID);
  iListTraceIDs.erase(iter);
  std::list< unsigned int > oListTraceIDs = std::list< unsigned int >();
  if ( !iListTraceIDs.empty() )
    {
    FieldWithValue             JoinCondition = { "CoordIDMin", "CoordID", "=" };
    std::vector< std::string > VectTraceIDs = ListUnsgIntToVectorString(iListTraceIDs);
    FieldWithValue             TCoord = { "TCoord", ConvertToString< unsigned int >(iTimePoint), "=" };
    oListTraceIDs = GetAllSelectedValuesFromTwoTables(iDatabaseConnector,
                                                      this->m_TracesName,
                                                      "coordinate",
                                                      this->m_TracesIDName,
                                                      JoinCondition,
                                                      this->m_TracesIDName,
                                                      VectTraceIDs,
                                                      TCoord);
    }
  return oListTraceIDs;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > GoDBCollectionOfTraces::GetListTimePointsFromTraceIDs(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTraceIDs)
{
  FieldWithValue JoinCondition = { "CoordIDMin", "CoordID", "=" };

  std::vector< std::string > VectTraceIDs = ListUnsgIntToVectorString(iListTraceIDs);
  return GetAllSelectedValuesFromTwoTables(iDatabaseConnector,
                                           this->m_TracesName, "coordinate", "TCoord", JoinCondition,
                                           this->m_TracesIDName, VectTraceIDs, true);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > GoDBCollectionOfTraces::GetTraceIDsBelongingToCollectionID(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTraceIDs,
  unsigned int iCollectionID)
{
  return ListSpecificValuesForOneColumn( iDatabaseConnector,
                                         this->m_TracesName, this->m_TracesIDName,
                                         this->m_TracesIDName, iListTraceIDs,
                                         this->m_CollectionIDName, ConvertToString< unsigned int >(iCollectionID) );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > GoDBCollectionOfTraces::GetTraceIDsBelongingToCollectionID(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListCollectionIDs)
{
  return ListSpecificValuesForOneColumn(iDatabaseConnector, this->m_TracesName,
                                        this->m_TracesIDName, this->m_CollectionIDName, iListCollectionIDs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list<unsigned int> GoDBCollectionOfTraces::GetTraceIDsBelongingToListTimePoints(
    vtkMySQLDatabase *iDatabaseConnector,std::list<unsigned int> iListTPs)
{
  FieldWithValue JoinCondition = { "CoordIDMin", "CoordID", "=" };
  std::vector< std::string > VectTimePoints = ListUnsgIntToVectorString(iListTPs);
  FieldWithValue AndCondition =
    {"imagingsessionID", ConvertToString<unsigned int>(this->m_ImgSessionID), "="};

  return GetAllSelectedValuesFromTwoTables(iDatabaseConnector, this->m_TracesName,
    "coordinate", this->m_TracesIDName, JoinCondition, "TCoord", VectTimePoints,
     AndCondition);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > GoDBCollectionOfTraces::GetTimePointsForTraceIDs(
  vtkMySQLDatabase *iDatabaseConnector, std::list< unsigned int > iListTraceIDs)
{
  FieldWithValue JoinCondition = { "CoordIDMin", "CoordID", "=" };

  std::vector< std::string > VectTraceIDs = ListUnsgIntToVectorString(iListTraceIDs);
  return GetAllSelectedValuesFromTwoTables(iDatabaseConnector,
                                           this->m_TracesName, "coordinate", "TCoord", JoinCondition,
                                           this->m_TracesIDName, VectTraceIDs);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > GoDBCollectionOfTraces::GetTraceIDsWithTimePointInf(vtkMySQLDatabase *iDatabaseConnector,
                                                                              std::list< unsigned int > iListTraceIDs,
                                                                              unsigned int iTimePoint)
{
  FieldWithValue JoinCondition = { "CoordIDMin", "CoordID", "=" };
  FieldWithValue AndCondition =
                              { "TCoord", ConvertToString< unsigned int >(iTimePoint), "<" };

  std::vector< std::string > VectTraceIDs = ListUnsgIntToVectorString(iListTraceIDs);
  return GetListValuesFromTwoTablesAndCondition(
           iDatabaseConnector,
           this->m_TracesName, "coordinate", this->m_TracesIDName,
           JoinCondition, this->m_TracesIDName, VectTraceIDs, AndCondition);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*unsigned int GoDBCollectionOfTraces::GetTimePointMin(
  vtkMySQLDatabase *iDatabaseConnector, unsigned int iTraceID)
{
  FieldWithValue JoinCondition = {"CoordIDMin", "CoordID", "="};
  std::vector<std::string> TraceID(1);
  TraceID.at(0) = ConvertToString<unsigned int>(iTraceID);
  std::list<unsigned int> results = GetAllSelectedValuesFromTwoTables(
    iDatabaseConnector, this->m_TracesName, "coordinate", "TCoord", JoinCondition,
  this->m_TracesIDName, TraceID);
  return results.front();
  }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------

unsigned int GoDBCollectionOfTraces::GetTimePointMax(
  vtkMySQLDatabase *iDatabaseConnector, unsigned int iTraceID)
{
  FieldWithValue JoinCondition = {"CoordIDMax", "CoordID", "="};
  std::vector<std::string> TraceID(1);
  TraceID.at(0) = ConvertToString<unsigned int>(iTraceID);
  std::list<unsigned int> results = GetAllSelectedValuesFromTwoTables(
    iDatabaseConnector, this->m_TracesName, "coordinate", "TCoord", JoinCondition,
  this->m_TracesIDName, TraceID);
  return results.front();
  }*/
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
unsigned int GoDBCollectionOfTraces::GetBoundedBoxTimePoint(
  vtkMySQLDatabase *iDatabaseConnector, unsigned int iTraceID, bool MinTimePoint)
{
  std::string WhichTimePoint = "CoordIDMin";

  if ( !MinTimePoint )
    {
    WhichTimePoint = "CoordIDMax";
    }
  FieldWithValue             JoinCondition = { WhichTimePoint, "CoordID", "=" };
  std::vector< std::string > TraceID(1);
  TraceID.at(0) = ConvertToString< unsigned int >(iTraceID);
  std::list< unsigned int > results = GetAllSelectedValuesFromTwoTables(
      iDatabaseConnector, this->m_TracesName, "coordinate", "TCoord", JoinCondition,
      this->m_TracesIDName, TraceID);
  return results.front();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< std::string > GoDBCollectionOfTraces::GetAttributesForTraces()
{
  std::vector< std::string > oTraceAttributes;
  oTraceAttributes.push_back(this->m_TracesIDName);
  if (this->m_CollectionIDName != "NoneID")
    {
    oTraceAttributes.push_back(this->m_CollectionIDName);
    }
  else
    {
    oTraceAttributes.push_back("TrackIDRoot");
    }
  oTraceAttributes.push_back("Red");
  oTraceAttributes.push_back("Green");
  oTraceAttributes.push_back("Blue");
  oTraceAttributes.push_back("Alpha");
  oTraceAttributes.push_back("Points");
  oTraceAttributes.push_back("TCoord");

  return oTraceAttributes;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
int GoDBCollectionOfTraces::GetTraceIDWithLowestTimePoint(
  vtkMySQLDatabase *iDatabaseConnector,
  std::list<unsigned int> iListTraceIDs )
{
  int oTraceID = -1;
  std::vector< std::string > SelectedFields(2);
  SelectedFields.at(0) = "track.trackid";
  SelectedFields.at(1) = "coordinate.TCoord";
  FieldWithValue JoinCondition = { "CoordIDMin", "CoordID", "=" };
  std::vector<FieldWithValue> Conditions = std::vector<FieldWithValue>();
  std::list<unsigned int>::iterator iter = iListTraceIDs.begin();

  while(iter != iListTraceIDs.end() )
    {
    unsigned int Value = *iter;
    FieldWithValue Condition = {this->m_TracesIDName, ConvertToString<unsigned int>(Value), "="};
    Conditions.push_back(Condition);
    ++iter;
    }

  std::vector< std::string > ResultQuery
    = GetAllSelectedValuesFromTwoTables(
        iDatabaseConnector, this->m_TracesName, "coordinate", SelectedFields,
        JoinCondition, Conditions, "OR", "TCoord");
  if (ResultQuery.size()>2)
    {
      if (ResultQuery.at(1) != ResultQuery.at(3)) //if the 2 lowest timepoints are different
        {
        oTraceID = ss_atoi<int>(ResultQuery.at(0)); //return the 1rst traceID
        }
    }
  return oTraceID;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
 std::list<unsigned int> GoDBCollectionOfTraces::GetTrackFamilyDataFromDB(
   vtkMySQLDatabase *iDatabaseConnector)
 {

   std::vector<std::string> VectColumnsTrackFamily(3);
   VectColumnsTrackFamily.at(0) = "TrackIDMother";
   VectColumnsTrackFamily.at(1) = "TrackIDDaughter1";
   VectColumnsTrackFamily.at(2) = "TrackIDDaughter2";
   FieldWithValue JoinCondition = {"TrackID", "TrackIDMother", "="};

   return GetAllSelectedValuesFromTwoTables(  iDatabaseConnector,
                                              "track",
                                              "trackfamily",
                                              VectColumnsTrackFamily,
                                              JoinCondition,
                                              "ImagingsessionID",
                                              ConvertToString<unsigned int>(this->m_ImgSessionID),
                                              true);
 }

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
 std::list<unsigned int> GoDBCollectionOfTraces::GetTrackFamiliesForLineages(
   vtkMySQLDatabase *iDatabaseConnector, std::list<unsigned int> iLineagesID)
 {
   FieldWithValue JoinCondition = { "trackID", "TrackIDMother", "=" };

   std::vector<std::string> VectorLineages = ListUnsgIntToVectorString(iLineagesID);

   return GetAllSelectedValuesFromTwoTables( iDatabaseConnector, "track", "trackfamily",
     "trackfamily.trackfamilyID", JoinCondition, "track.lineageid", VectorLineages );
 }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
 std::list<unsigned int> GoDBCollectionOfTraces::GetTrackFamilyID(
   vtkMySQLDatabase *iDatabaseConnector, std::list<unsigned int> iListTrackIDs)
{
   FieldWithValue JoinCondition = { "trackID", "trackIDMother", "=" };
   std::vector<std::string> VectTrackIDs = ListUnsgIntToVectorString(iListTrackIDs);
   std::list<unsigned int> oList = GetTwoFieldsFromTwoTables(iDatabaseConnector,
     this->m_TracesName, "trackfamily", JoinCondition, "track.trackfamilyID",
     "trackfamily.trackfamilyID", this->m_TracesIDName, VectTrackIDs, true);
   return oList;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string
GoDBCollectionOfTraces::
GetPoints(vtkMySQLDatabase *iDatabaseConnector, std::string iTraceName,
          unsigned int iTraceID)
{
  // WHAT
  std::string QueryString = "SELECT Points ";

  // FROM
  QueryString += "FROM " + iTraceName;

  //WHERE
  // trace to Trace (mysql not case sensitive on linux but just in case)
  std::string traceID = iTraceName;
  std::transform(iTraceName.begin(), ++iTraceName.begin(),traceID.begin(), ::toupper);
  traceID += "ID";
  QueryString += " WHERE " + traceID + " = ";
  // iTraceID int to string
  std::stringstream s;
  s << iTraceID;
  QueryString += s.str();

  return ExecuteSelectQueryOneValue< std::string >( iDatabaseConnector,
                                                    QueryString);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<unsigned int>
GoDBCollectionOfTraces::
GetTrackFamily(vtkMySQLDatabase *iDatabaseConnector, unsigned int iTrackID)
{
  // WHAT
  std::string QueryString = "SELECT * ";

  // FROM
  QueryString += "FROM trackfamily ";

  //WHERE
  // iTraceID int to string
  std::stringstream s;
  s << iTrackID;
  std::string trackID = s.str();

  QueryString += " WHERE (TrackIDDaughter1=" + trackID;
  QueryString += " OR TrackIDDaughter2=" + trackID + ")";

  std::cout << "query: " << QueryString << std::endl;

  return ExecuteSelectQuery< std::vector<unsigned int> >( iDatabaseConnector,
                                                          QueryString);
}
//-------------------------------------------------------------------------
