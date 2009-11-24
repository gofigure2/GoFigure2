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

  m_ColumnsInfos     = GetColumnsInfoForTraceTable();
  m_ColumnNamesMap   = GetColumnsNamesMapForTableWidget();
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
void GoDBCollectionOfTraces::DeleteTraces(QStringList TracesToDelete,
  vtkMySQLDatabase* DatabaseConnector)
{
  for (int i = 0; i<TracesToDelete.size();i++)
    {
    std::string ID = TracesToDelete.at(i).toStdString();
    DeleteRow(DatabaseConnector,m_TracesName,m_TracesIDName,ID);
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::AddSelectedTracesToCollection(QStringList ListSelectedTraces,
  int newCollectionID,vtkMySQLDatabase* DatabaseConnector)
{
  std::string newCollectionIDstring = ConvertToString<int>(newCollectionID);
  for (int i=0; i<ListSelectedTraces.size();i++)
    {
    UpdateValueInDB(DatabaseConnector,m_TracesName,m_CollectionIDName, 
      newCollectionIDstring,m_TracesIDName, ListSelectedTraces.at(i).toStdString());
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
  int CollectionID,QStringList ListSelectedTraces)
{
  GoDBCoordinateRow NewCollectionCoordMin;
  //transform the QStringList to a vector<string>:
  std::vector<std::string> VectorSelectedTraces;
  for (int i = 0; i <ListSelectedTraces.size();i++)
    {
    std::string test = ListSelectedTraces.at(i).toStdString();//test
    VectorSelectedTraces.push_back(ListSelectedTraces.at(i).toStdString());
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
  int CollectionID,QStringList ListSelectedTraces)
{
  GoDBCoordinateRow NewCollectionCoordMax;
  //transform the QStringList to a vector<string>:
  std::vector<std::string> VectorSelectedTraces;
  for (int i = 0; i <ListSelectedTraces.size();i++)
    {
    std::string test = ListSelectedTraces.at(i).toStdString();//test
    VectorSelectedTraces.push_back(ListSelectedTraces.at(i).toStdString());
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
std::vector<GoDBTraceInfoForTableWidget> GoDBCollectionOfTraces
  ::GetColumnsInfoForTraceTable()
{ 
  GoDBTraceInfoForTableWidget temp;
  //IsSelected column will correspond to the "IsHighLighted":
  temp.InfoName = "Selected";
  temp.ColumnNameTableWidget = "Selected";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the TraceID:
  temp.InfoName = this->m_TracesIDName;
  temp.ColumnNameDatabase = this->m_TracesIDName;
  temp.ColumnNameTableWidget = this->m_TracesIDName;
  temp.TableNameDatabase = this->m_TracesName;
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the CollectionID:
  temp.InfoName   = this->m_CollectionIDName;
  temp.ColumnNameDatabase = this->m_CollectionIDName;
  temp.ColumnNameTableWidget = this->m_CollectionIDName;
  temp.TableNameDatabase = this->m_TracesName;
  temp.TableForeignKeyDatabase = this->m_CollectionIDName;
  temp.TableKeyDatabase = this->m_CollectionIDName;
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the ColorID of the trace:
  //check if it is needed in the query ??
  std::string ColorTrace = this->m_TracesName;
  ColorTrace += "Color";
  temp.ColumnNameDatabase = "ColorID";
  temp.TableNameDatabase = this->m_TracesName;
  temp.InfoName = ColorTrace;
  temp.TableForeignKeyDatabase = "ColorID";
  temp.TableKeyDatabase = "ColorID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the Red value of the trace:
  temp.ColumnNameDatabase = "Red";
  temp.TableNameDatabase = "color";
  std::string OtherInfo = "RedFor";
  OtherInfo += this->m_TracesName;
  temp.InfoName = OtherInfo;
  temp.TableForeignKeyDatabase = "ColorID";
  temp.TableKeyDatabase = "ColorID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the Green value of the trace:
  temp.ColumnNameDatabase = "Green";
  temp.TableNameDatabase = "color";
  OtherInfo = "GreenFor";
  OtherInfo += this->m_TracesName;
  temp.InfoName = OtherInfo;
  temp.TableForeignKeyDatabase = "ColorID";
  temp.TableKeyDatabase = "ColorID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the Blue value of the trace:
  temp.ColumnNameDatabase = "Blue";
  temp.TableNameDatabase = "color";
  OtherInfo = "BlueFor";
  OtherInfo += this->m_TracesName;
  temp.InfoName = OtherInfo;
  temp.TableForeignKeyDatabase = "ColorID";
  temp.TableKeyDatabase = "ColorID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the ColorID of the collection:
  std::string ColorCollection = this->m_CollectionName;
  ColorCollection += "Color";
  temp.ColumnNameDatabase = "ColorID";
  temp.TableNameDatabase = this->m_CollectionName;
  temp.InfoName = ColorCollection;
  temp.TableForeignKeyDatabase = "ColorID";
  temp.TableKeyDatabase = "ColorID";
  temp.DisplayedDirectlyInTraceTable = false;
  m_ColumnsInfos.push_back(temp);

  temp.Clear();

  //Get the info for the Red value of the collection:
  temp.ColumnNameDatabase = "Red";
  temp.TableNameDatabase = "color";
  OtherInfo = "RedFor";
  OtherInfo += this->m_CollectionName;
  temp.InfoName = OtherInfo;
  temp.TableForeignKeyDatabase = "ColorID";
  temp.TableKeyDatabase = "ColorID";
  temp.DisplayedDirectlyInTraceTable = false;
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the Green value of the collection:
  temp.ColumnNameDatabase = "Green";
  temp.TableNameDatabase = "color";
  OtherInfo = "GreenFor";
  OtherInfo += this->m_CollectionName;
  temp.InfoName = OtherInfo;
  temp.TableForeignKeyDatabase = "ColorID";
  temp.TableKeyDatabase = "ColorID";
  temp.DisplayedDirectlyInTraceTable = false;
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the Blue value of the collection:
  temp.ColumnNameDatabase = "Blue";
  temp.TableNameDatabase = "color";
  OtherInfo = "BlueFor";
  OtherInfo += this->m_CollectionName;
  temp.InfoName = OtherInfo;
  temp.TableForeignKeyDatabase = "ColorID";
  temp.TableKeyDatabase = "ColorID";
  temp.DisplayedDirectlyInTraceTable = false;
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the PCoord:
  temp.InfoName = "PCoord";
  temp.ColumnNameDatabase = "PCoord";
  temp.ColumnNameTableWidget = "Plate";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the RCoord:
  temp.InfoName = "RCoord";
  temp.ColumnNameDatabase = "RCoord";
  temp.ColumnNameTableWidget = "Row";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the CCoord:
  temp.InfoName = "CCoord";
  temp.ColumnNameDatabase = "CCoord";
  temp.ColumnNameTableWidget = "Column";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the XTile:
  temp.InfoName = "XTile";
  temp.ColumnNameDatabase = "XTile";
  temp.ColumnNameTableWidget = "XTile";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the YTile:
  temp.InfoName = "YTile";
  temp.ColumnNameDatabase = "YTile";
  temp.ColumnNameTableWidget = "YTile";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the ZTile:
  temp.InfoName = "ZTile";
  temp.ColumnNameDatabase = "ZTile";
  temp.ColumnNameTableWidget = "ZTile";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the XMin:
  temp.InfoName = "BDXMin";
  temp.ColumnNameDatabase = "XCoord";
  temp.ColumnNameTableWidget = "XMin";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the YMin:
  temp.InfoName = "BDYMin";
  temp.ColumnNameDatabase = "YCoord";
  temp.ColumnNameTableWidget = "YMin";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the ZMin:
  temp.InfoName = "BDZMin";
  temp.ColumnNameDatabase = "ZCoord";
  temp.ColumnNameTableWidget = "ZMin";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the XMax:
  temp.InfoName = "BDXMax";
  temp.ColumnNameDatabase = "XCoord";
  temp.ColumnNameTableWidget = "XMax";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMax";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the YMax:
  temp.InfoName = "BDYMax";
  temp.ColumnNameDatabase = "YCoord";
  temp.ColumnNameTableWidget = "YMax";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMax";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  //Get the info for the ZMax:
  temp.InfoName = "BDZMax";
  temp.ColumnNameDatabase = "ZCoord";
  temp.ColumnNameTableWidget = "ZMax";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMax";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  temp.Clear();

  GetSpecificInfoForTraceTable();

  return m_ColumnsInfos;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBCollectionOfTraces::GetSpecificInfoForTraceTable()
{
  if (this->m_TracesName == "contour" || this->m_TracesName == "mesh")
    {
    GoDBTraceInfoForTableWidget temp;
    
    //Get the info for the Time Point:
    temp.InfoName = "TimePoint";
    temp.ColumnNameDatabase = "TCoord";
    temp.ColumnNameTableWidget = "TimePoint";
    temp.TableNameDatabase = "coordinate";
    temp.TableForeignKeyDatabase = "CoordIDMin";
    temp.TableKeyDatabase = "CoordID";
    m_ColumnsInfos.push_back(temp);
    temp.Clear();
    }
  else
    {
    GoDBTraceInfoForTableWidget temp;
    
    //Get the info for the Time Point Min:
    temp.InfoName = "TimePointMin";
    temp.ColumnNameDatabase = "TCoord";
    temp.ColumnNameTableWidget = "TimePointMin";
    temp.TableNameDatabase = "coordinate";
    temp.TableForeignKeyDatabase = "CoordIDMin";
    temp.TableKeyDatabase = "CoordID";
    m_ColumnsInfos.push_back(temp);
    temp.Clear();

    //Get the info for the Time Point Max:
    temp.InfoName = "TimePointMax";
    temp.ColumnNameDatabase = "TCoord";
    temp.ColumnNameTableWidget = "TimePointMax";
    temp.TableNameDatabase = "coordinate";
    temp.TableForeignKeyDatabase = "CoordIDMax";
    temp.TableKeyDatabase = "CoordID";
    m_ColumnsInfos.push_back(temp);
    temp.Clear();
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBCollectionOfTraces::MapString GoDBCollectionOfTraces::
GetColumnsNamesMapForTableWidget()
{
  for (int i=0;i<m_ColumnsInfos.size();i++)
    {
    if (m_ColumnsInfos[i].ColumnNameTableWidget != "None" &&
      m_ColumnsInfos[i].ColumnNameTableWidget != "NoneID")
      {
      m_ColumnNamesMap[m_ColumnsInfos[i].ColumnNameTableWidget] = "";
      }
    }
  return m_ColumnNamesMap;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<GoDBCollectionOfTraces::MapString> GoDBCollectionOfTraces::
  GetRowContainer()
{
  std::vector<GoDBCollectionOfTraces::MapString> RowContainer;
 //Select directly from the Trace Table:
  std::vector<std::string> JoinTablesOnTraceTable = 
    GetQueryStringForTraceJoinedTables();
  std::vector<std::string> SelectFields = GetQueryStringForSelectFieldsTables();
    
 return RowContainer;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<std::string> GoDBCollectionOfTraces::
GetQueryStringForTraceJoinedTables()
{
  std::list<std::string> SelectNamesColumns;
  std::vector <std::string> SelectJoinTables;
  int i=0;
  while (i < m_ColumnsInfos.size())
    {
    if( m_ColumnsInfos[i].TableNameDatabase != "None")
      {
      std::vector<std::string>::iterator iter = SelectJoinTables.begin();
      bool IsTableInTheList = false;
      while (iter !=  SelectJoinTables.end()&& IsTableInTheList == false)
        {
        if (*iter == m_ColumnsInfos[i].TableNameDatabase )
          {
          IsTableInTheList = true;
          }
        iter++;
        }
      if (IsTableInTheList == false && 
        m_ColumnsInfos[i].TableNameDatabase != this->m_TracesName &&
        m_ColumnsInfos[i].DisplayedDirectlyInTraceTable == true)
        {
        SelectJoinTables.push_back(m_ColumnsInfos[i].TableNameDatabase);
        std::string OnQuery = this->m_TracesName;
        OnQuery += ".";
        OnQuery += m_ColumnsInfos[i].TableForeignKeyDatabase;
        OnQuery += "=";
        OnQuery += m_ColumnsInfos[i].TableNameDatabase;
        OnQuery += ".";
        OnQuery += m_ColumnsInfos[i].TableKeyDatabase;
        SelectJoinTables.push_back(OnQuery);
        }
      }
    i++;
    }
  return SelectJoinTables;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<std::string> GoDBCollectionOfTraces::
GetQueryStringForSelectFieldsTables()
{
  std::vector<std::string> SelectFields;
  int i=0;
  while (i < m_ColumnsInfos.size())
    {
    if( m_ColumnsInfos[i].ColumnNameDatabase != "None" &&
      m_ColumnsInfos[i].ColumnNameTableWidget != "None" &&
      m_ColumnsInfos[i].ColumnNameTableWidget != "NoneID")
      {
      std::string temp = m_ColumnsInfos[i].TableNameDatabase;
      temp += ".";
      temp += m_ColumnsInfos[i].ColumnNameDatabase;
      bool IsSelectFieldsInTheVector = false;
      int j = 0;
      while (IsSelectFieldsInTheVector == false && j<SelectFields.size())
        {
        if (SelectFields[j] == temp)
          {
          IsSelectFieldsInTheVector = true;
          }
        j++;
        }
      if (IsSelectFieldsInTheVector == false)
        {
        SelectFields.push_back(temp);
        }
      }
    i++;
    }
  return SelectFields;
}