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
#include "QueryDataBaseHelper.h"
#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStdString.h"
#include "vtkVariant.h"
#include <sstream>
#include <string>

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::pair< bool, vtkMySQLDatabase * > ConnectToServer(std::string ServerName, std::string login,
                                                      std::string Password)
{
  std::pair< bool, vtkMySQLDatabase * > ConnectionServer;
  vtkMySQLDatabase *                    ServerConnector = vtkMySQLDatabase::New();
  ServerConnector->SetHostName( ServerName.c_str() );
  ServerConnector->SetUser( login.c_str() );
  ServerConnector->SetPassword( Password.c_str() );

  if ( !ServerConnector->Open() )
    {
    std::cout << "Could not connect to the server." << std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    ConnectionServer.first = false;
    return ConnectionServer;
    }
  ConnectionServer.first = true;
  ConnectionServer.second = ServerConnector;
  return ConnectionServer;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::pair< bool, vtkMySQLDatabase * > ConnectToDatabase(std::string ServerName, std::string login,
                                                        std::string Password, std::string DBName)
{
  std::pair< bool, vtkMySQLDatabase * > ConnectionDatabase(false, NULL);
  vtkMySQLDatabase *                    DatabaseConnector = vtkMySQLDatabase::New();
  DatabaseConnector->SetHostName( ServerName.c_str() );
  DatabaseConnector->SetUser( login.c_str() );
  DatabaseConnector->SetPassword( Password.c_str() );
  DatabaseConnector->SetDatabaseName( DBName.c_str() );
  if ( !DatabaseConnector->Open() )
    {
    itkGenericExceptionMacro(
      << "Could not open database."
      << "DB will not be created.");
    std::cout << "Could not connect to the database." << std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return ConnectionDatabase;
    }

  ConnectionDatabase.first = true;
  ConnectionDatabase.second = DatabaseConnector;

  return ConnectionDatabase;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
vtkMySQLDatabase * OpenDatabaseConnection(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName)
{
  std::pair< bool, vtkMySQLDatabase * > ConnectionDatabase = ConnectToDatabase(
    ServerName, login, Password, DBName);

  if ( !ConnectionDatabase.first )
    {
    std::cout << "No connection open for QGoOpenOrCreateImgSession" << std::endl;
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }

  return ConnectionDatabase.second;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool CloseDatabaseConnection(
  vtkMySQLDatabase *DatabaseConnector)
{
  if ( DatabaseConnector != 0 )
    {
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    return true;
    }
  return false;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::string > ListDatabases(vtkMySQLDatabase *ServerConnector)
{
  std::vector< std::string > result;
  vtkSQLQuery *              query = ServerConnector->GetQueryInstance();
  query->SetQuery("Show Databases;");
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Show Databases query failed."
      << query->GetLastErrorText() );
    query->Delete();
    return result;
    }

  // all set, proceed

  // iterate over lines, we know there is only one column
  // and as many rows as there is databases
  while ( query->NextRow() )
    {
    result.push_back( query->DataValue(0).ToString() );
    }
  query->Delete();

  return result;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::string > ListTables(vtkMySQLDatabase *DatabaseConnector)
{
  std::vector< std::string > result;
  vtkSQLQuery *              query = DatabaseConnector->GetQueryInstance();
  query->SetQuery("Show tables;");
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Show tables query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  // all set, proceed

  // iterate over lines, we know there is only one column
  // and as many rows as there is databases
  while ( query->NextRow() )
    {
    result.push_back( query->DataValue(0).ToString() );
    }
  query->Delete();

  return result;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void DropDatabase(vtkMySQLDatabase *ServerConnector, std::string DBName)
{
  vtkSQLQuery *      query = ServerConnector->GetQueryInstance();
  std::ostringstream insertQuery;

  insertQuery << "DROP DATABASE " << DBName;
  query->SetQuery( insertQuery.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Drop query failed"
      << query->GetLastErrorText() );
    query->Delete();
    return;
    }
  query->Delete();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void DropTable(vtkMySQLDatabase *DatabaseConnector, std::string TableName)
{
  vtkSQLQuery *      query = DatabaseConnector->GetQueryInstance();
  std::ostringstream insertQuery;

  insertQuery << "DROP TABLE " << TableName;
  query->SetQuery( insertQuery.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Drop query failed"
      << query->GetLastErrorText() );
    query->Delete();
    return;
    }
  query->Delete();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void DeleteRow(vtkMySQLDatabase *DatabaseConnector,
               std::string TableName, std::string field, std::string value)
{
  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;

  querystream << "DELETE FROM ";
  querystream << TableName;
  querystream << " WHERE ";
  querystream << field;
  querystream << " = '";
  querystream << value;
  querystream << "';";

  query->SetQuery( querystream.str().c_str() );

  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Delete row query failed"
      << query->GetLastErrorText() );
    }
  query->Delete();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void DeleteRows(vtkMySQLDatabase *DatabaseConnector, std::string TableName,
                std::string field, std::vector< std::string > VectorValues)
{
  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;

  querystream << "DELETE FROM ";
  querystream << TableName;
  querystream << " WHERE (";
  unsigned int i;
  for ( i = 0; i < VectorValues.size() - 1; i++ )
    {
    querystream << field;
    querystream << " = '";
    querystream << VectorValues[i];
    querystream << "' OR ";
    }
  querystream << field;
  querystream << " = '";
  querystream << VectorValues[i];
  querystream << "');";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Delete rows query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    }
  query->Delete();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DoesDatabaseExist(vtkMySQLDatabase *ServerConnector, std::string DBName)
{
  std::vector< std::string > list;
  list = ListDatabases(ServerConnector);
  std::string                          myString(DBName);
  std::vector< std::string >::iterator start = list.begin();
  std::vector< std::string >::iterator end   = list.end();
  while ( start != end )
    {
    if ( ( *start ) == myString )
      {
      return true;
      }
    start++;
    }
  return false;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DoesTableExist(vtkMySQLDatabase *DatabaseConnector,
                    std::string TableName)
{
  std::vector< std::string > list;
  list = ListTables(DatabaseConnector);
  std::string                          myString(TableName);
  std::vector< std::string >::iterator start = list.begin();
  std::vector< std::string >::iterator end   = list.end();
  while ( start != end )
    {
    if ( ( *start ) == myString )
      {
      return true;
      }
    start++;
    }
  return false;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void UpdateValueInDB(vtkMySQLDatabase *DatabaseConnector,
                     std::string TableName, std::string field, std::string newValue,
                     std::string ColumnName, std::string value)

{
  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;

  querystream << "UPDATE ";
  querystream << TableName;
  querystream << " SET ";
  querystream << field;
  querystream << " = '";
  querystream << newValue;
  querystream << "' WHERE ";
  querystream << ColumnName;
  querystream << " = '";
  querystream << value;
  querystream << "';";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "replace value in DB query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return;
    }
  query->Delete();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void UpdateValueInDB(vtkMySQLDatabase *DatabaseConnector,
                     std::string iTableName, std::string ifield,
                     std::string inewValue, std::vector< unsigned int > iVectIDs)
{
  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;

  querystream << "UPDATE ";
  querystream << iTableName;
  querystream << " SET ";
  querystream << ifield;
  querystream << " = '";
  querystream << inewValue;
  querystream << " WHERE (";
  unsigned int i;
  for ( i = 0; i < iVectIDs.size() - 1; i++ )
    {
    querystream << ifield;
    querystream << " = '";
    querystream << iVectIDs[i];
    querystream << "' OR ";
    }
  querystream << ifield;
  querystream << " = '";
  querystream << iVectIDs[i];
  querystream << "');";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "replace value in DB query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return;
    }
  query->Delete();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/*void UpdateContourInDB(vtkMySQLDatabase* DatabaseConnector,
                       GoDBContourRow UpdatedContour)
{
  std::string contourID = UpdatedContour.GetMapValue("ContourID");
  UpdateValueInDB(DatabaseConnector, "contour", "CoordIDMin", UpdatedContour.GetMapValue("CoordIDMin"),
                  "contourID", contourID);
  UpdateValueInDB(DatabaseConnector, "contour", "CoordIDMax", UpdatedContour.GetMapValue("CoordIDMax"),
                  "contourID", contourID);
  UpdateValueInDB(DatabaseConnector, "contour", "Points", UpdatedContour.GetMapValue("Points"),
                  "contourID", contourID);
}*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::string > GetFieldNames(std::string TableName,
                                         vtkMySQLDatabase *DatabaseConnector)
{
  std::vector< std::string > result;

  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "DESCRIBE ";
  querystream << TableName;
  querystream << ";";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "describe table query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }
  while ( query->NextRow() )
    {
    result.push_back( query->DataValue(0).ToString() );
    }
  query->Delete();

  return result;
}
