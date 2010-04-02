/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Rev: 1150 $  // Revision of last commit
  Date: $Date: 2010-03-30 15:00:47 -0400 (Tue, 30 Mar 2010) $  // Date of last commit
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

#ifndef __GoDBExport_h
#define __GoDBExport_h

#include "vtkMySQLDatabase.h"
#include <vector>


class GoDBExport
{
public:

  GoDBExport(std::string iServerName,std::string iLogin,
    std::string iPassword, int iImagingSessionID,
    std::string iFilename);
  virtual ~GoDBExport();
  void ExportContours();

//private slots:
  //void on_actionImportContour_triggered( );

private:
  //QGoExport( const QGoExport& );
  //QGoExport operator = ( const QGoExport& );
  vtkMySQLDatabase*   m_DatabaseConnector;
  std::string         m_ServerName;
  std::string         m_Password;
  std::string         m_Login;
  int                 m_ImagingSessionID;
  std::fstream        m_outfile;

  /** \brief return a vector of pair containing the name of the info as .first
  and the info as .second. for Imagingsession such as Name, creation date and
  microscope name*/
  std::vector<std::pair<std::string,std::string> > GetImagingSessionInfoFromDB();
  /** \brief return a pair containing as .first the iNameInfo and as.second the
  corresponding info found in the Database for the table imagingsession*/
  std::pair<std::string,std::string> GetOneInfoFromDBForImgSession(
    std::string iNameInfo);
  /** \brief get the info from the database for all the entities from a table or
  with a limitation defined with field and value and write them in the output file*/
  template< typename T >
  void WriteTableInfoFromDB(std::string field, std::string value)
   {
     T TableRow;
     std::vector<std::string> ListTableIDs = ListSpecificValuesForOneColumn(
     this->m_DatabaseConnector,TableRow.GetTableName(),
     TableRow.GetTableIDName(),field,value);
    std::vector<std::string>::iterator iter = ListTableIDs.begin();
    while(iter != ListTableIDs.end())
      {
      std::vector<std::pair<std::string,std::string> > EntityInfo = 
        this->GetOneEntityInfoFromDB(*iter,TableRow);
      this->WriteOnTheOutputFile(TableRow.GetTableName(),EntityInfo);
      iter++;
      }
   }
  
  /** \brief get the info with their names for an entity from the database 
  and put them in a vector of pair of string (name of the info + value of the info)*/
  template< typename T >
  std::vector<std::pair<std::string,std::string> > 
    GetOneEntityInfoFromDB(std::string iEntityID,T iTableRow)
  {
    std::vector<std::pair<std::string,std::string> > oEntityInfo;
    iTableRow.SetValuesForSpecificID(atoi(iEntityID.c_str()),this->m_DatabaseConnector);
    std::vector<std::string> FieldNames = iTableRow.GetVectorColumnNames();
    std::vector<std::string>::iterator iter = FieldNames.begin();
    while (iter != FieldNames.end())
      {
      std::pair<std::string,std::string> FieldInfo;
      FieldInfo.first = *iter;
      FieldInfo.second = iTableRow.GetMapValue(*iter);
      oEntityInfo.push_back(FieldInfo);
      iter++;
      }
    return oEntityInfo;
  }

  std::vector<std::pair<std::string,std::string> > GetOneColorInfoFromDB(std::string iColorID);
  /** \brief return <iName> */
  std::string GetNameWithBrackets(std::string iName);
  /** \brief return </iName> */
  std::string GoDBExport::GetNameWithSlashBrackets(std::string iName);
  /**\ brief write on the output file the info contained in the vector with
  the name of the entity they describe*/
  void WriteOnTheOutputFile(std::string iNameOfEntity,
    std::vector<std::pair<std::string,std::string> > iInfoToWrite);
  /** \ brief add 2 spaces to the output file for xml tabulation*/
  void AddTabulation();
  void OpenDBConnection();
  void CloseDBConnection();

};
#endif
