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
#include "GoDBTableWidgetContainer.h"
#include <iostream>

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
  GetSpecificInfoForTraceTable();

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

  GetSpecificInfoForTraceTable();

  return m_ColumnsInfos;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTableWidgetContainer::GetSpecificInfoForTraceTable()
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
    std::pair<GoDBTraceInfoForTableWidget,std::vector<std::string> > PairTemp;
    PairTemp.first = temp;
    m_RowContainer.push_back(PairTemp);
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
    std::pair<GoDBTraceInfoForTableWidget,std::vector<std::string> > PairTemp;
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
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::list<std::string> GoDBTableWidgetContainer::
GetListColumnsNamesForTableWidget()
{
  std::list<std::string> ListColumnNames;
  for (int i=0;i<m_ColumnsInfos.size();i++)
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
std::vector<std::string> GoDBTableWidgetContainer::
GetQueryStringForSelectFieldsTables(bool SameFieldsInQuery)
{
  std::vector<std::string> SelectFields;
  int i=0;
  while (i < m_ColumnsInfos.size())
    {
    //if there is info to get from the database and to put directly in the
    //table widget, and that the column Name in the table widget is not "NoneID"
    // (that can be found in the lineage table):
    if( m_ColumnsInfos[i].ColumnNameDatabase != "None" &&
      //m_ColumnsInfos[i].ColumnNameTableWidget != "None" &&
      m_ColumnsInfos[i].SameFieldForDifferentValues == SameFieldsInQuery &&
      m_ColumnsInfos[i].ColumnNameTableWidget != "NoneID" && 
      m_ColumnsInfos[i].TableNameDatabase != "None")
      {
      //record TableNameDatabase.ColumnNameDatabase if it is not already in the vector:
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
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void GoDBTableWidgetContainer::FillRowContainer(
  std::vector<std::vector<std::string> >iResultsFromQuery,
  std::vector<std::string> iSelectFields)
{       
  for ( int i=0; i < iSelectFields.size(); i++)
     {
     bool HasBeenFound = false;
     for (int j=0; j<m_RowContainer.size()&& HasBeenFound == false ; j++)
       {
       std::string test = iSelectFields[i]; //for test purpose
       std::string test2= m_RowContainer[j].first.ColumnNameDatabase; //for test purpose
       int PosColumnNameFound = iSelectFields[i].find(m_RowContainer[j].first.ColumnNameDatabase);
         
       if (PosColumnNameFound > 0 && m_RowContainer[j].second.empty())
         {
         HasBeenFound = true;
         for (int RowNumberForQueryResults = 0; RowNumberForQueryResults < iResultsFromQuery.size();
           RowNumberForQueryResults++ )
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
  int i=0;
  //for all the TableNameDatabase in the m_ColumnsInfos:
  while (i < m_ColumnsInfos.size())
    {
    std::string name = m_ColumnsInfos[i].TableNameDatabase; //for test purpose
    std::string name2 = m_ColumnsInfos[i].ColumnNameDatabase; //for test purpose
    //check first that the ColumnsInfos has direct infosfrom the database:
    if( m_ColumnsInfos[i].TableNameDatabase != "None")
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
int GoDBTableWidgetContainer::GetIndexInsideRowContainer(std::string iInfoName)
{
  for (int i = 0; i < m_RowContainer.size();i++)
    {
    if (m_RowContainer[i].first.InfoName == iInfoName)
      {
      return i;
      }
    }
  return -1;
}