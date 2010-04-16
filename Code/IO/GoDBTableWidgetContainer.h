/*=========================================================================
  Author: $Author: lydiesouhait$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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
#ifndef __GoDBTableWidgetContainer_h
#define __GoDBTableWidgetContainer_h

#include "GoDBTraceInfoForTableWidget.h"
#include "GoDBTraceRow.h"
#include <string>
#include <vector>
#include <list>

#include "QGoIOConfigure.h"

class QGOIO_EXPORT GoDBTableWidgetContainer
{

 public:
   GoDBTableWidgetContainer();
   GoDBTableWidgetContainer(std::string iCollectionName,std::string iTracesName);
   ~GoDBTableWidgetContainer(){};

   typedef  std::vector<std::pair<GoDBTraceInfoForTableWidget,std::vector<std::string> > >
    DBTableWidgetContainerType;
   /** \brief Return a list with all the ColumnNames to be displayed in the
   tableWidget:*/
   std::list<std::string> GetListColumnsNamesForTableWidget();
   
   /** \brief return a vector of the table.fields to be selected from the database
   for all the fields except the ones with the same name if SameFieldsQuery is set 
   to false and only for them if SameFieldsQuery is set to true*/
   std::vector<std::string> GetQueryStringForSelectFieldsTables(bool SameFieldsInQuery);

    /** \brief return a vector of string with the tables to be joined with the 
   trace table in the database query for all the fields except the ones with the
   same name if SameFieldsQuery is set to false and only for them if 
   SameFieldsQuery is set to true*/
   std::vector<std::string> GetQueryStringForTraceJoinedTables(bool SameFieldsInQuery);
     
   /** \brief fill the columns of the row container following the vector of string 
   containing the columns to be filled with the results contained in the vector
   results from query*/
   void FillRowContainer(std::vector<std::vector<std::string> >iResultsFromQuery,
     std::vector<std::string> iSelectFields);
     
   DBTableWidgetContainerType GetRowContainer();

   /** *\brief return the index in the row container for the column with the given 
   InfoName*/
   int GetIndexInsideRowContainer(std::string iInfoName);
   
   /** *\brief return a list of all the traces with a bounding box 
   containing the given ZCoord*/
   std::list<std::string> GetTracesIDForAGivenZCoord(int iZCoord);

   /** \brief Insert a new created trace with the datas contained in the 
   NewTraceContainer into the m_RowContainer*/
   //void InsertNewCreatedTrace(GoDBTableWidgetContainer iLinkToNewTraceContainer);

   //void DeleteSelectedTraces(std::list<int> iSelectedTraces);

   //void UpdateIDs(std::list<int> iListTracesToUpdate,unsigned int NewCollectionID);

protected:
  std::string m_CollectionName;
  std::string m_CollectionIDName;
  std::string m_TracesName;     
  std::string m_TracesIDName;    

  std::vector<GoDBTraceInfoForTableWidget> m_ColumnsInfos;
  DBTableWidgetContainerType               m_RowContainer;

   /** \brief Fill a vector of GoDBTraceInfoForTableWidget with the info
  needed to fill the table widget for all the traces*/ 
  std::vector<GoDBTraceInfoForTableWidget> GetColumnsInfoForTraceTable();

  /** \brief Fill the vector of GoDBTraceInfoForTableWidget with the info
  common to 2 traces only*/  
  void GetSpecificInfoForTraceTable();

};
#endif