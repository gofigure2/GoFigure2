/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Rev: 542 $  // Revision of last commit
  Date: $Date: 2009-08-06 16:08:10 -0400 (Thu, 06 Aug 2009) $  // Date of last commit
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
#ifndef __QueryDataBaseHelper_h
#define __QueryDataBaseHelper_h

#include <vector>
#include <string>
#include "itkMacro.h"

std::vector<std::string> ListDataBases(
  std::string ServerName, std::string login,
  std::string Password);

std::vector<std::string> ListTables(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName );

//query: "SELECT ColumnName FROM TableName"
std::vector<std::string> ListAllValuesForOneColumn(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string ColumnName, std::string TableName);

//query: "SELECT * FROM TableName WHERE field = value"
//if field is a primary key, will return only the values for
//one row
std::vector<std::string> ListSpecificValuesForRow(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string TableName, std::string field,std::string value);

//query: "SELECT ColumnName FROM TableName WHERE field = value"
std::vector<std::string> ListSpecificValuesForOneColumn(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string TableName, std::string ColumnName,
  std::string field,std::string value);

//query: "UPDATE TableName SET field = newValue WHERE ColumnName = value"
void UpdateValueInDB(std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string TableName, std::string field, std::string newValue,
  std::string ColumnName, std::string value);

bool CanConnectToServer(
  std::string ServerName, std::string login,
  std::string Password);

void DropDatabase(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName );

void DropTable(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string TableName );

// query: "DELETE FROM TableName WHERE field = value"
void DeleteRow(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string TableName, std::string field, std::string value);

//query: "SELECT LAST_INSERT_ID(): ????"
/*int LastInsertID(std::string ServerName, std::string login,
  std::string Password, std::string DBName);*/

//query: "SELECT MAX(ColumnName) FROM TableName"
int MaxValueForOneColumnInTable(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string ColumnName,std::string TableName );

bool DoesDataBaseExist(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName );

bool DoesTableExist(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string TableName );

#endif
