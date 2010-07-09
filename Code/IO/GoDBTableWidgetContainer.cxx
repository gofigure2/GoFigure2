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
#include "GoDBTableWidgetContainer.h"
#include <iostream>
#include <cstdlib>

GoDBTableWidgetContainer::GoDBTableWidgetContainer(std::string iCollectionName,
  std::string iTracesName)
{ 
  m_CollectionName   = iCollectionName;
  m_CollectionIDName = m_CollectionName;
  m_CollectionIDName += "ID";
  m_TracesName       = iTracesName;
  m_TracesIDName     = m_TracesName;
  m_TracesIDName     += "ID";

  m_ColumnsInfos     = GetColumnsInfoForTraceTable();

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<GoDBTraceInfoForTableWidget> GoDBTableWidgetContainer::
  GetColumnsInfoForTraceTable()
{
  GoDBTraceInfoForTableWidget temp;
  //IsSelected column will correspond to the "IsHighLighted":
  temp.InfoName = "Selected";
  temp.ColumnNameTableWidget = "";
  //temp.ColumnNameTableWidget = "Selected";
  m_ColumnsInfos.push_back(temp);
  std::pair<GoDBTraceInfoForTableWidget,std::vector<std::string> > PairTemp;
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the TraceID:
  temp.InfoName = this->m_TracesIDName;
  temp.ColumnNameDatabase = this->m_TracesIDName;
  temp.ColumnNameTableWidget = this->m_TracesIDName;
  temp.TableNameDatabase = this->m_TracesName;
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the CollectionID:
  temp.InfoName   = this->m_CollectionIDName;
  temp.ColumnNameDatabase = this->m_CollectionIDName;
  temp.ColumnNameTableWidget = this->m_CollectionIDName;
  temp.TableNameDatabase = this->m_TracesName;
  temp.TableForeignKeyDatabase = this->m_CollectionIDName;
  temp.TableKeyDatabase = this->m_CollectionIDName;
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
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
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
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
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
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
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
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
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the ColorID of the collection:
  std::string ColorCollection = this->m_CollectionName;
  ColorCollection += "Color";
  temp.ColumnNameDatabase = "ColorID";
  temp.TableNameDatabase = this->m_CollectionName;
  temp.InfoName = ColorCollection;
  temp.TableForeignKeyDatabase = this->m_CollectionIDName;
  temp.TableKeyDatabase = this->m_CollectionIDName;
  temp.SameFieldForDifferentValues = false;
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the Red value of the collection:
  temp.ColumnNameDatabase = "Red";
  temp.TableNameDatabase = "color";
  OtherInfo = "RedFor";
  OtherInfo += this->m_CollectionName;
  temp.AccessFromTraceTableThroughWhichTable = this->m_CollectionName;
  temp.InfoName = OtherInfo;
  temp.TableForeignKeyDatabase = "ColorID";
  temp.TableKeyDatabase = "ColorID";
  temp.SameFieldForDifferentValues = true;
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the Green value of the collection:
  temp.ColumnNameDatabase = "Green";
  temp.TableNameDatabase = "color";
  OtherInfo = "GreenFor";
  OtherInfo += this->m_CollectionName;
   temp.AccessFromTraceTableThroughWhichTable = this->m_CollectionName;
  temp.InfoName = OtherInfo;
  temp.TableForeignKeyDatabase = "ColorID";
  temp.TableKeyDatabase = "ColorID";
  temp.SameFieldForDifferentValues = true;
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the Blue value of the collection:
  temp.ColumnNameDatabase = "Blue";
  temp.TableNameDatabase = "color";
  OtherInfo = "BlueFor";
  OtherInfo += this->m_CollectionName;
  temp.AccessFromTraceTableThroughWhichTable = this->m_CollectionName;
  temp.InfoName = OtherInfo;
  temp.TableForeignKeyDatabase = "ColorID";
  temp.TableKeyDatabase = "ColorID";
  temp.SameFieldForDifferentValues = true;  
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the PCoord:
  temp.InfoName = "PCoord";
  temp.ColumnNameDatabase = "PCoord";
  temp.ColumnNameTableWidget = "Plate";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the RCoord:
  temp.InfoName = "RCoord";
  temp.ColumnNameDatabase = "RCoord";
  temp.ColumnNameTableWidget = "Row";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the CCoord:
  temp.InfoName = "CCoord";
  temp.ColumnNameDatabase = "CCoord";
  temp.ColumnNameTableWidget = "Column";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the XTile:
  temp.InfoName = "XTile";
  temp.ColumnNameDatabase = "XTileCoord";
  temp.ColumnNameTableWidget = "XTile";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the YTile:
  temp.InfoName = "YTile";
  temp.ColumnNameDatabase = "YTileCoord";
  temp.ColumnNameTableWidget = "YTile";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the ZTile:
  temp.InfoName = "ZTile";
  temp.ColumnNameDatabase = "ZTileCoord";
  temp.ColumnNameTableWidget = "ZTile";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the CoordIDMin of the bounding box:
  temp.InfoName = "BDCoordIDMin";
  temp.ColumnNameDatabase = "CoordIDMin";
  temp.TableNameDatabase = this->m_TracesName;
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the XMin:
  temp.InfoName = "BDXMin";
  temp.ColumnNameDatabase = "XCoord";
  temp.ColumnNameTableWidget = "XMin";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the YMin:
  temp.InfoName = "BDYMin";
  temp.ColumnNameDatabase = "YCoord";
  temp.ColumnNameTableWidget = "YMin";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the ZMin:
  temp.InfoName = "BDZMin";
  temp.ColumnNameDatabase = "ZCoord";
  temp.ColumnNameTableWidget = "ZMin";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMin";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the CoordIDMax of the bounding box:
  temp.InfoName = "BDCoordIDMax";
  temp.ColumnNameDatabase = "CoordIDMax";
  temp.TableNameDatabase = this->m_TracesName;
  temp.TableForeignKeyDatabase = "CoordIDMax";
  temp.TableKeyDatabase = "CoordID";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the XMax:
  temp.InfoName = "BDXMax";
  temp.ColumnNameDatabase = "XCoord";
  temp.ColumnNameTableWidget = "XMax";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMax";
  temp.TableKeyDatabase = "CoordID";
  temp.SameFieldForDifferentValues = true;
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the YMax:
  temp.InfoName = "BDYMax";
  temp.ColumnNameDatabase = "YCoord";
  temp.ColumnNameTableWidget = "YMax";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMax";
  temp.TableKeyDatabase = "CoordID";
  temp.SameFieldForDifferentValues = true;
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the ZMax:
  temp.InfoName = "BDZMax";
  temp.ColumnNameDatabase = "ZCoord";
  temp.ColumnNameTableWidget = "ZMax";
  temp.TableNameDatabase = "coordinate";
  temp.TableForeignKeyDatabase = "CoordIDMax";
  temp.TableKeyDatabase = "CoordID";
  temp.SameFieldForDifferentValues = true;
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();
 
  this->GetCommonInfoForTwoTracesTable();
  this->GetSpecificInfoForTraceTable();

  return m_ColumnsInfos;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTableWidgetContainer:: GetCommonInfoForTwoTracesTable()
{
  GoDBTraceInfoForTableWidget temp;
  std::pair<GoDBTraceInfoForTableWidget,std::vector<std::string> > PairTemp;
  if (this->m_TracesName == "contour" || this->m_TracesName == "mesh")
    {
    //GoDBTraceInfoForTableWidget temp;
    
    //Get the info for the Time Point:
    temp.InfoName = "TimePoint";
    temp.ColumnNameDatabase = "TCoord";
    temp.ColumnNameTableWidget = "TimePoint";
    temp.TableNameDatabase = "coordinate";
    temp.TableForeignKeyDatabase = "CoordIDMin";
    temp.TableKeyDatabase = "CoordID";
    m_ColumnsInfos.push_back(temp);
    //std::pair<GoDBTraceInfoForTableWidget,std::vector<std::string> > PairTemp;
    PairTemp.first = temp;
    m_RowContainer.push_back(PairTemp);
    temp.Clear();
    }
  else
    {
    //GoDBTraceInfoForTableWidget temp;
    
    //Get the info for the Time Point Min:
    temp.InfoName = "TimePointMin";
    temp.ColumnNameDatabase = "TCoord";
    temp.ColumnNameTableWidget = "TimePointMin";
    temp.TableNameDatabase = "coordinate";
    temp.TableForeignKeyDatabase = "CoordIDMin";
    temp.TableKeyDatabase = "CoordID";
    m_ColumnsInfos.push_back(temp);
    //std::pair<GoDBTraceInfoForTableWidget,std::vector<std::string> > PairTemp;
    PairTemp.first = temp;
    m_RowContainer.push_back(PairTemp);
    temp.Clear();

    //Get the info for the Time Point Max:
    temp.InfoName = "TimePointMax";
    temp.ColumnNameDatabase = "TCoord";
    temp.ColumnNameTableWidget = "TimePointMax";
    temp.TableNameDatabase = "coordinate";
    temp.TableForeignKeyDatabase = "CoordIDMax";
    temp.TableKeyDatabase = "CoordID";
    temp.SameFieldForDifferentValues = true;
    m_ColumnsInfos.push_back(temp);
    PairTemp.first = temp;
    m_RowContainer.push_back(PairTemp);
    temp.Clear();
    }
  //for the column "is visible or not":
  temp.InfoName = "Show";
  temp.ColumnNameTableWidget = "Show";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTableWidgetContainer::GetSpecificInfoForTraceTable()
{
  /*GoDBTraceInfoForTableWidget temp;
  std::pair<GoDBTraceInfoForTableWidget,std::vector<std::string> > PairTemp;
  if (this->m_TracesName == "mesh")
    {
    SetSpecificColumnsInfoForMesh(*/
    
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list<std::string> GoDBTableWidgetContainer::
GetListColumnsNamesForTableWidget()
{
  std::list<std::string> ListColumnNames;
  for (unsigned int i=0;i<m_ColumnsInfos.size();i++)
    {
    if (m_ColumnsInfos[i].ColumnNameTableWidget != "None" &&
      m_ColumnsInfos[i].ColumnNameTableWidget != "NoneID")
      {
      ListColumnNames.push_back(m_ColumnsInfos[i].ColumnNameTableWidget);
      }
    }
  return ListColumnNames; 
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<std::string> GoDBTableWidgetContainer::GetNameComputedColumns()
{
  std::vector<std::string> ListComputedColumnNames;
  for (unsigned int i=0;i<m_ColumnsInfos.size();i++)
    {
    if (m_ColumnsInfos[i].ColumnNameTableWidget != "None" &&
      m_ColumnsInfos[i].ColumnNameTableWidget != "NoneID" && 
      m_ColumnsInfos[i].ColumnNameDatabase == "None" &&
      m_ColumnsInfos[i].InfoName != "Selected") 
      {
      ListComputedColumnNames.push_back(m_ColumnsInfos[i].ColumnNameTableWidget);
      }
    }
  return ListComputedColumnNames; 
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<std::string> GoDBTableWidgetContainer::
GetQueryStringForSelectFieldsTables(bool SameFieldsInQuery)
{
  std::vector<std::string> SelectFields;
  unsigned int i=0;
  while (i < m_ColumnsInfos.size())
    {
    //if there is info to get from the database and to put directly in the
    //table widget, and that the column Name in the table widget is not "NoneID"
    // (that can be found in the lineage table):
    if( m_ColumnsInfos[i].ColumnNameDatabase != "None" &&
      //m_ColumnsInfos[i].ColumnNameTableWidget != "None" &&
      m_ColumnsInfos[i].SameFieldForDifferentValues == SameFieldsInQuery &&
      m_ColumnsInfos[i].ColumnNameTableWidget != "NoneID" && 
      m_ColumnsInfos[i].TableNameDatabase != "None" && 
      m_ColumnsInfos[i].TableNameDatabase != "intensity")
      {
      //record TableNameDatabase.ColumnNameDatabase if it is not already in the vector:
      std::string temp = m_ColumnsInfos[i].TableNameDatabase;
      temp += ".";
      temp += m_ColumnsInfos[i].ColumnNameDatabase;
      bool IsSelectFieldsInTheVector = false;
      unsigned int j = 0;
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
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTableWidgetContainer::FillRowContainer(
  std::vector<std::vector<std::string> >iResultsFromQuery,
  std::vector<std::string> iSelectFields,std::string BaseOn)
{       
  for ( unsigned int i=0; i < iSelectFields.size(); i++)
     {
     bool HasBeenFound = false;
     for (unsigned int j=0; j<m_RowContainer.size()&& HasBeenFound == false ; j++)
       {
       std::string test = iSelectFields[i]; //for test purpose
       std::string test2= m_RowContainer[j].first.ColumnNameDatabase; //for test purpose
       size_t PosColumnNameFound;
       if (BaseOn.empty())
         {
         PosColumnNameFound = 
           iSelectFields[i].find(m_RowContainer[j].first.ColumnNameDatabase); 
         }
       else
         {
         if (BaseOn == "ColumnNameTableWidget")
           {
           std::string NameColonne = m_RowContainer[j].first.ColumnNameTableWidget; 
           if (!NameColonne.empty())
             {
             PosColumnNameFound = 
             iSelectFields[i].find(NameColonne); 
             }
           else
             {
             PosColumnNameFound = std::string::npos;
             }
           }
         else
           {
           std::cout<< "The BaseOn you choose doesn't exist"<<std::endl;
           std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
           std::cout << std::endl;
           return;
           }
         }
       if (PosColumnNameFound != std::string::npos && m_RowContainer[j].second.empty())
         {
         HasBeenFound = true;
         for (unsigned int RowNumberForQueryResults = 0; 
           RowNumberForQueryResults < iResultsFromQuery.size();RowNumberForQueryResults++ )
           {  
           std::vector<std::string> ResultsFromQueryForOneTrace = 
           iResultsFromQuery[RowNumberForQueryResults];
           //j found corresponds to the vector in the row_container to be filled
           //with the values from the results of the query corresponding to i column:
           m_RowContainer[j].second.push_back(ResultsFromQueryForOneTrace[i]);
           }
         }
       }
    }  
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::vector<std::string> GoDBTableWidgetContainer::
GetQueryStringForTraceJoinedTables(bool SameFieldsInQuery)
{
  std::list<std::string> SelectNamesColumns;
  std::vector <std::string> SelectJoinTables;
  unsigned int i=0;
  //for all the TableNameDatabase in the m_ColumnsInfos:
  while (i < m_ColumnsInfos.size())
    {
    std::string name = m_ColumnsInfos[i].TableNameDatabase; //for test purpose
    std::string name2 = m_ColumnsInfos[i].ColumnNameDatabase; //for test purpose
    //check first that the ColumnsInfos has direct infosfrom the database:
    if( m_ColumnsInfos[i].TableNameDatabase != "None" &&  
        m_ColumnsInfos[i].TableNameDatabase != "intensity")
      {
      //check that the table is not already in the list:
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
      //if the table is not in the list,is not the table of the trace and 
      //is accessed during the first query, record the name of the table,
      //and the traceName.tableforeignkey = tableName.primarykey for the
      // "on" condition in the "Join" part of the query
      if (IsTableInTheList == false && 
        m_ColumnsInfos[i].TableNameDatabase != this->m_TracesName &&
        m_ColumnsInfos[i].SameFieldForDifferentValues == SameFieldsInQuery)
        {
        if (m_ColumnsInfos[i].AccessFromTraceTableThroughWhichTable == "None")
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
        else
          {
          if (m_ColumnsInfos[i].AccessFromTraceTableThroughWhichTable != this->m_CollectionName)
            {
            std::cout<< "Pb: access table is different than the Collection Table"<<std::endl;
            std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
            std::cout << std::endl;
            }
          else
            {
            //join the trace table and the collection table (=intermediary table)
            SelectJoinTables.push_back(m_ColumnsInfos[i].AccessFromTraceTableThroughWhichTable);
            std::string OnQuery =this->m_TracesName;
            OnQuery += ".";
            OnQuery += this->m_CollectionIDName;
            OnQuery += "=";
            OnQuery += this->m_CollectionName;
            OnQuery += ".";
            OnQuery += this->m_CollectionIDName;
            SelectJoinTables.push_back(OnQuery);
            //join the collection table and the TableNameDatabase of the ColumnsInfo:
            SelectJoinTables.push_back(m_ColumnsInfos[i].TableNameDatabase);
            std::string OnQuerybis =this->m_CollectionName;
            OnQuerybis += ".";
            OnQuerybis += m_ColumnsInfos[i].TableForeignKeyDatabase;
            OnQuerybis += "=";
            OnQuerybis += m_ColumnsInfos[i].TableNameDatabase;
            OnQuerybis += ".";
            OnQuerybis += m_ColumnsInfos[i].TableKeyDatabase;
            SelectJoinTables.push_back(OnQuerybis);
            }
          }//ENDELSE
        }//ENDIF
      }//ENDIF
    i++;
    }//ENDWHILE

  return SelectJoinTables;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoDBTableWidgetContainer::DBTableWidgetContainerType 
  GoDBTableWidgetContainer::GetRowContainer()
{
  return m_RowContainer;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTableWidgetContainer::ClearRowContainer()
{
  this->m_RowContainer.clear();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
int GoDBTableWidgetContainer::GetIndexInsideRowContainer(std::string iInfoName)
{
  for (unsigned int i = 0; i < m_RowContainer.size();i++)
    {
    if (m_RowContainer[i].first.InfoName == iInfoName)
      {
      return i;
      }
    }
  return -1;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
 std::list<std::string> GoDBTableWidgetContainer::
   GetTracesIDForAGivenZCoord(int iZCoord)
{
  std::list<std::string> SelectedTracesIDFromRowContainer;
  int IndexZMin = this->GetIndexInsideRowContainer("BDZMin");
  int IndexZMax = this->GetIndexInsideRowContainer("BDZMax");
  int IndexTraceID = this->GetIndexInsideRowContainer(this->m_TracesIDName);

  std::vector<std::string>::iterator iterZMin = 
    this->m_RowContainer[IndexZMin].second.begin();
  std::vector<std::string>::iterator iterZMax = 
    this->m_RowContainer[IndexZMax].second.begin();
  std::vector<std::string>::iterator iterTraceID = 
    this->m_RowContainer[IndexTraceID].second.begin();

  while (iterTraceID != this->m_RowContainer[IndexTraceID].second.end())
    {
    std::string ZMinStrg = *iterZMin;
    int ZMin = atoi(ZMinStrg.c_str());
    std::string ZMaxStrg = *iterZMax;
    int ZMax = atoi(ZMaxStrg.c_str());
    if ( iZCoord >= ZMin && iZCoord <= ZMax)
      {
      SelectedTracesIDFromRowContainer.push_back(*iterTraceID);
      }
    iterZMin    ++;
    iterZMax    ++;
    iterTraceID ++;
    }

  return SelectedTracesIDFromRowContainer;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTableWidgetContainer::SetSpecificColumnsInfoForMesh(
  std::vector<std::vector<std::string> > iChannelsInfo)
{
  this->m_ChannelsInfo = iChannelsInfo;
  /*m_ColumnsInfos.clear();
  this->m_RowContainer.clear();
  m_ColumnsInfos = GetColumnsInfoForTraceTable();*/
  //Get the info for the total intensities per channel:
  GoDBTraceInfoForTableWidget temp;
  std::pair<GoDBTraceInfoForTableWidget,std::vector<std::string> > PairTemp;
  int NumberOfChannels = this->m_ChannelsInfo.size();
  if (this->m_ChannelsInfo.empty())
    {
    std::cout<<"No info for the channels"<<std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return;
    }
  for (int i = 0; i < NumberOfChannels; i++)
    {
    std::string InfoName = "TotalIntensityForChannelID";
    InfoName += this->m_ChannelsInfo.at(i).at(1);
    temp.InfoName = InfoName;
    temp.ColumnNameDatabase = "Value";
    std::string ColumnNameTableWidget = "T.I.";
    ColumnNameTableWidget += this->m_ChannelsInfo.at(i).at(0);
    temp.ColumnNameTableWidget = ColumnNameTableWidget;
    temp.TableNameDatabase = "intensity";
    temp.TableForeignKeyDatabase = "MeshID";
    temp.TableKeyDatabase = "MeshID";
    m_ColumnsInfos.push_back(temp);
    PairTemp.first = temp;
    m_RowContainer.push_back(PairTemp);
    temp.Clear();
    }
  //Get the info for the Volume:
  temp.InfoName = "Volume";
  temp.ColumnNameTableWidget = "Volume";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();

  //Get the info for the Surface Area:
  temp.InfoName = "SurfaceArea";
  temp.ColumnNameTableWidget = "SurfaceArea";
  m_ColumnsInfos.push_back(temp);
  PairTemp.first = temp;
  m_RowContainer.push_back(PairTemp);
  temp.Clear();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
 std::vector<std::vector<std::string> > GoDBTableWidgetContainer::
   GetChannelsInfo()
 {
  return this->m_ChannelsInfo;
 }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
 unsigned int GoDBTableWidgetContainer::GetNumberOfRows()
{
  if(!this->m_RowContainer.empty())
    {
    //return the number of traceID in the container which correspond to the 2nd column
    return this->m_RowContainer.at(1).second.size();
    }
  return 0;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*void GoDBTableWidgetContainer::InsertNewCreatedTrace(GoDBTableWidgetContainer
  iLinkToNewTraceContainer)
{
  GoDBTableWidgetContainer::DBTableWidgetContainerType NewTraceContainer =
    iLinkToNewTraceContainer.GetRowContainer();
  //iterator for the m_RowContainer:
  GoDBTableWidgetContainer::DBTableWidgetContainerType::iterator iterCurrentColumn =
    this->m_RowContainer.begin();
  //iterator for the new container to be added to m_RowContainer:
  GoDBTableWidgetContainer::DBTableWidgetContainerType::iterator iterNewColumn =
    NewTraceContainer.begin();
      
  while(iterCurrentColumn != this->m_RowContainer.end())
    {
    if (iterCurrentColumn->second.size() != 0)
      {
      std::vector<std::string>::iterator iterNewRow = 
        iterNewColumn->second.begin();
      while (iterNewRow != iterNewColumn->second.end())
        {
        std::string NewValue = *iterNewRow;
        iterCurrentColumn->second.push_back(NewValue);
        iterNewRow++;
        }
      }
  iterCurrentColumn++;
  iterNewColumn++;
    }
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//void GoDBTableWidgetContainer::UpdateIDs(std::list<int> iListTracesToUpdate,
  //unsigned int NewCollectionID)
//{
  //GoDBTableWidgetContainer::DBTableWidgetContainerType::iterator iter =
  //  this->m_RowContainer.begin();
  
/*  int CollectionIDIndex = this->GetIndexInsideRowContainer(this->m_CollectionIDName);
  int TraceIDIndex = this->GetIndexInsideRowContainer(this->m_TracesIDName);
  bool AllUpdated = false;
  std::list<int>::iterator iterTracesToUpdate = iListTracesToUpdate.begin();
  std::list<int>::reverse_iterator reverseIter = iListTracesToUpdate.rbegin();
  int FinalID = *reverseIter;
  std::vector<std::string>::iterator iter = this->m_RowContainer[TraceIDIndex].second.begin();
  int i =0; //to count the index in the vector of string.
  while (iter != this->m_RowContainer[TraceIDIndex].second.end() && !AllUpdated)
    {
    while (iterTracesToUpdate != iListTracesToUpdate.end()&& !AllUpdated)
      {
      int TraceID = *iterTracesToUpdate;
      if (ConvertToString<int>(TraceID) == *iter )
        {
        this->m_RowContainer[CollectionIDIndex].second[i] = 
          ConvertToString<unsigned int>(NewCollectionID);
        if (*iterTracesToUpdate == FinalID)
          {
          AllUpdated = true;
          }
        iterTracesToUpdate++;
        }
      iter++;
      i++;
      }
    }*/
//}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*void GoDBTableWidgetContainer::DeleteSelectedTraces(std::list<int> iSelectedTraces)
{
  std::list<int>::iterator iter = iSelectedTraces.begin();
  unsigned int j = 0;
  while(iter != iSelectedTraces.end())
    {
    unsigned int indexrowtodelete = 0;
    bool found = false;
    while(j<this->m_RowContainer[1].second.size() && !found)
      {
      int TraceID = *iter;
      if (this->m_RowContainer[1].second[j] == ConvertToString<int>(TraceID) )
        {  
        indexrowtodelete = j;
        found = true;
        for(unsigned int k = 0; k<this->m_RowContainer.size();k++)
          {
          if (this->m_RowContainer[k].second.size()>indexrowtodelete)
            {
            std::vector<std::string>::iterator itertodel = this->m_RowContainer[k].second.begin();
            bool foundErase = false;
            bool EndofVector = false;
            int l=0;
            while(!foundErase && !EndofVector)
              {
              if (l == indexrowtodelete)
                {
                this->m_RowContainer[k].second.erase(itertodel);
                foundErase = true;
                }
              else
                {
                l++;
                itertodel++;
                if(itertodel == this->m_RowContainer[k].second.end())
                  {
                  EndofVector = true;
                  }
                }
              }//ENDWHILE         
            }//ENDIF
          }//ENDFOR        
        }//ENDIF
      j++;
      }//ENDWHILE
    iter++;
    }//ENDWHILE
}
*/
