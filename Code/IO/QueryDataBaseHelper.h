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
#ifndef __QueryDatabaseHelper_h
#define __QueryDatabaseHelper_h

#include <vector>
#include <string>
#include <map>
#include "itkMacro.h"
#include "vtkMySQLDatabase.h"
#include "GoDBContourRow.h"

#include "QGoIOConfigure.h"

QGOIO_EXPORT
std::pair<bool,vtkMySQLDatabase*> ConnectToServer(
  std::string ServerName, std::string login,
  std::string Password);

QGOIO_EXPORT
std::pair<bool,vtkMySQLDatabase*> ConnectToDatabase(
  std::string ServerName,std::string login,
  std::string Password,std::string DBName);

QGOIO_EXPORT
vtkMySQLDatabase* OpenDatabaseConnection(
  std::string ServerName,std::string login,
  std::string Password,std::string DBName);

/**\brief return true if the connection has been closed, false if
the connection was already closed*/
QGOIO_EXPORT
bool CloseDatabaseConnection(
  vtkMySQLDatabase* DatabaseConnector);

QGOIO_EXPORT
std::vector<std::string> ListDatabases(
  vtkMySQLDatabase* ServerConnector);

QGOIO_EXPORT
std::vector<std::string> ListTables(
  vtkMySQLDatabase* DatabaseConnector );

//query: "UPDATE TableName SET field = newValue WHERE ColumnName = value"
QGOIO_EXPORT
void UpdateValueInDB(vtkMySQLDatabase* DatabaseConnector,
  std::string TableName, std::string field, std::string newValue,
  std::string ColumnName, std::string value);

/** \todo to review*/
QGOIO_EXPORT
void UpdateContourInDB(vtkMySQLDatabase* DatabaseConnector,
                       GoDBContourRow UpdatedContour);

QGOIO_EXPORT
void DropDatabase( 
  vtkMySQLDatabase* ServerConnector,
  std::string DBName );

QGOIO_EXPORT
void DropTable(
  vtkMySQLDatabase* DatabaseConnector,
  std::string TableName );

// query: "DELETE FROM TableName WHERE field = value"
QGOIO_EXPORT
void DeleteRow(
  vtkMySQLDatabase* DatabaseConnector,
  std::string TableName, std::string field, std::string value);

// query: "DELETE FROM TableName WHERE (field = 
//value1 or field = value2...."
QGOIO_EXPORT
void DeleteRows(
  vtkMySQLDatabase* DatabaseConnector,
  std::string TableName, std::string field, 
  std::vector<std::string> VectorValues);

QGOIO_EXPORT
bool DoesDatabaseExist(
  vtkMySQLDatabase* ServerConnector,
  std::string DBName );

QGOIO_EXPORT
bool DoesTableExist(
  vtkMySQLDatabase* DatabaseConnector,
  std::string TableName );

//query: "DESCRIBE TableName"
QGOIO_EXPORT
std::vector<std::string> GetFieldNames(std::string TableName,
  vtkMySQLDatabase* ServerConnector);

#endif
