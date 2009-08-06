/*=========================================================================
  Author: $Author$  // Author of last commit
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
#include "QueryDataBaseHelper.h"

#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStdString.h"
#include "vtkVariant.h"
#include <sstream>
#include <string>

//------------------------------------------------------------------------------
bool CanConnectToServer(
  std::string ServerName,
  std::string login,
  std::string Password)
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->Open();
  bool IsOpen = DataBaseConnector->IsOpen();
  DataBaseConnector->Delete();
  return IsOpen;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector<std::string> ListDataBases(
  std::string ServerName, std::string login,
  std::string Password)
{
  std::vector< std::string > result;

  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  if( !DataBaseConnector->Open() )
    {
    itkGenericExceptionMacro( << "Could not open the Database." );
    DataBaseConnector->Delete();
    return result;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  query->SetQuery( "Show Databases;" );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Show Databases query failed."
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return result;
    }

  // all set, proceed

  // iterate over lines, we know there is only one column
  // and as many rows as there is databases
  while( query->NextRow() )
    {
    result.push_back( query->DataValue( 0 ).ToString() );
    }

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();

  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector<std::string> ListTables(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName )
{
  std::vector< std::string > result;

  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    itkGenericExceptionMacro(
      << "Could not open database."
      << "DB will not be created." );
    DataBaseConnector->Delete();
    return result;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  query->SetQuery( "Show tables;" );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Show tables query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return result;
    }

  // all set, proceed

  // iterate over lines, we know there is only one column
  // and as many rows as there is databases
  while( query->NextRow() )
    {
    result.push_back( query->DataValue( 0 ).ToString() );
    }

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();

  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector<std::string> ListAllValuesForOneColumn(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string ColumnName, std::string TableName)
{
  std::vector< std::string > result;

  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    return result;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << ColumnName;
  querystream << " FROM ";
  querystream << TableName;
  querystream << ";";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "List of all fields query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return result;
    }
  while( query->NextRow() )
    {
    result.push_back( query->DataValue( 0 ).ToString() );
    }

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();

  return result;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void DropDatabase(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName )
{
  // if( CanOpenDatabase && DoesDataBaseExist )
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  std::ostringstream insertQuery;
  insertQuery <<"DROP DATABASE "<< DBName;
  query->SetQuery( insertQuery.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Drop query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void DropTable(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string TableName )
{
  // if( CanOpenDatabase && DoesDataBaseExist && DoesTableExist )
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  std::ostringstream insertQuery;
  insertQuery<< "DROP TABLE "<< TableName;
  query->SetQuery( insertQuery.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Drop query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void DeleteRow(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string TableName, std::string field, std::string value )
{
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "DB will not be created."  << std::endl;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
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

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DoesDataBaseExist(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName )
{
  std::vector< std::string > list;
  list = ListDataBases( ServerName, login, Password );
  std::string myString( DBName );
  std::vector< std::string >::iterator start = list.begin();
  std::vector< std::string >::iterator end   = list.end();
  while( start != end )
    {
    if( (*start) == myString )
      {
      return true;
      }
    start++;
    }
  return false;

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
bool DoesTableExist(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string TableName )
{
  std::vector< std::string > list;
  list = ListTables( ServerName, login, Password, DBName );
  std::string myString( TableName );
  std::vector< std::string >::iterator start = list.begin();
  std::vector< std::string >::iterator end   = list.end();
  while( start != end )
    {
    if( (*start) == myString )
      {
      return true;
      }
    start++;
    }
  return false;

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector<std::string> ListSpecificValuesForRow(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string TableName, std::string field,
  std::string value)
{
  std::vector< std::string > result;

  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "The list of the values for the row will not be read.";
    std::cerr << std::endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    return result;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT * FROM ";
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
      << "List of all values of ExpID query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return result;
    }

  while (query->NextRow())
    {
    for(  int i = 0; i < query->GetNumberOfFields(); i++)
      {
      result.push_back( query->DataValue( i ).ToString() );
      }
    }

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();

  return result;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
std::vector<std::string> ListSpecificValuesForOneColumn(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string TableName, std::string ColumnName,
  std::string field,std::string value)

{ std::vector< std::string > result;

  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "List of the Values for the column can not be read."<< std::endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    return result;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << ColumnName;
  querystream << " FROM ";
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
      << "List of all values of ExpID query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return result;
    }

  while (query->NextRow())
    {
    for( int i = 0; i < query->GetNumberOfFields(); i++)
      {
      result.push_back( query->DataValue( i ).ToString() );
      }
    }

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();

  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void UpdateValueInDB(std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string TableName, std::string field, std::string newValue,
  std::string ColumnName, std::string value)

{ vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "Update Values can not be done."<< std::endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
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
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return;
    }

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/*
int LastInsertID(std::string ServerName, std::string login,
  std::string Password, std::string DBName)
{
  int ID = -1;
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "Find last insert ID can not be done."<< std::endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    return ID;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  query->SetQuery( "SELECT LAST_INSERT_ID();" );

  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "replace value in DB query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return ID;
    }

  ID = query->DataValue(0).ToInt();
  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();

  return ID;
}
*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int MaxValueForOneColumnInTable(
  std::string ServerName, std::string login,
  std::string Password, std::string DBName,
  std::string ColumnName,std::string TableName )
{
  int MaxValue = -1;
  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName( ServerName.c_str() );
  DataBaseConnector->SetUser( login.c_str() );
  DataBaseConnector->SetPassword( Password.c_str() );
  DataBaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DataBaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "Update Values can not be done."<< std::endl;
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    return MaxValue;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT MAX(";
  querystream << ColumnName;
  querystream << ") FROM ";
  querystream << TableName;
  querystream << ";";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "find max value query failed"
      << query->GetLastErrorText() );
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return MaxValue;
    }

  if (query->NextRow())
    {
    MaxValue = query->DataValue(0).ToInt();
    }

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();

  return MaxValue;
}
