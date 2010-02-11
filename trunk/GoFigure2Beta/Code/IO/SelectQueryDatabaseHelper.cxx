/*=========================================================================
  Author: $Author: lydiesouhait $  // Author of last commit
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
#include "SelectQueryDatabaseHelper.h"
#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStdString.h"
#include "vtkSmartPointer.h"
#include "vtkVariant.h"
#include "vtkPolyDataMySQLTextReader.h"
#include <sstream>
#include <string>


std::vector<std::string> ListAllValuesForOneColumn(vtkMySQLDatabase* DatabaseConnector,
  std::string ColumnName, std::string TableName)
{
  std::vector< std::string > result;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
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
    query->Delete();
    return result;
    }
  if (ColumnName != "*")
    {
    while( query->NextRow() )
      {
      result.push_back( query->DataValue( 0 ).ToString() );
      }
    }
  else
    {
    while(query->NextRow())
      {
      for( int i = 0; i < query->GetNumberOfFields(); i++)
        {
        result.push_back( query->DataValue( i ).ToString() );
        }
      }
    }

  query->Delete();

  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::map<std::string,std::string> MapTwoColumnsFromTable(
  vtkMySQLDatabase* DatabaseConnector,std::string ColumnNameOne,
  std::string ColumnNameTwo,std::string TableName)
{
  std::map<std::string,std::string> Result;
  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << ColumnNameOne;
  querystream << " , ";
  querystream << ColumnNameTwo;
  querystream << " FROM ";
  querystream << TableName;
  querystream << ";";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "List of all fields query failed"
      << query->GetLastErrorText() );
    query->Delete();
    return Result;
    }
  while( query->NextRow() )
    {
    Result[query->DataValue(0).ToString()]= query->DataValue(1).ToString();
    }
  query->Delete();
  return Result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::map<std::string,std::string> MapTwoColumnsFromTable(
  vtkMySQLDatabase* DatabaseConnector,std::string ColumnNameOne,
  std::string ColumnNameTwo,std::string TableName,
  std::string field, std::string value)
{
  std::map<std::string,std::string> Result;
  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << ColumnNameOne;
  querystream << " , ";
  querystream << ColumnNameTwo;
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
      << "List of all fields query failed"
      << query->GetLastErrorText() );
    query->Delete();
    return Result;
    }
  while( query->NextRow() )
    {
    Result[query->DataValue(0).ToString()]= query->DataValue(1).ToString();
    }
  query->Delete();
  return Result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector<std::string> ListSpecificValuesForRow(
  vtkMySQLDatabase* DatabaseConnector,std::string TableName, std::string field,
  std::string value)
{
  std::vector< std::string > result;
  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
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
  query->Delete();

  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int FindOneID(vtkMySQLDatabase* DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::string value)
{
  int ID = -1;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
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
      << "The FindOneID query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return ID;
    }

  if (query->NextRow())
    {
    ID = query->DataValue(0).ToInt();
    }
  query->Delete();

  return ID;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int FindOneID(vtkMySQLDatabase* DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string ColumnNameOne,std::string valueOne,
  std::string ColumnNameTwo,std::string valueTwo,
  std::string ColumnNameThree,std::string valueThree)
{
  int ID = -1;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << ColumnName;
  querystream << " FROM ";
  querystream << TableName;
  querystream << " WHERE (";
  querystream << ColumnNameOne;
  querystream << " = '";
  querystream << valueOne;
  querystream << "' AND ";
  querystream << ColumnNameTwo;
  querystream << " = '";
  querystream << valueTwo;
  querystream << "' AND ";
  querystream << ColumnNameThree;
  querystream << " = '";
  querystream << valueThree;
  querystream << "');";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "The FindOneID query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return ID;
    }

  if (query->NextRow())
    {
    ID = query->DataValue(0).ToInt();
    }
  query->Delete();

  return ID;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int FindOneID(vtkMySQLDatabase* DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string ColumnNameOne,std::string valueOne,
  std::string ColumnNameTwo,std::string valueTwo)
{
  int ID = -1;
  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << ColumnName;
  querystream << " FROM ";
  querystream << TableName;
  querystream << " WHERE (";
  querystream << ColumnNameOne;
  querystream << " = '";
  querystream << valueOne;
  querystream << "' AND ";
  querystream << ColumnNameTwo;
  querystream << " = '";
  querystream << valueTwo;
  querystream << "');";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "The FindOneID query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return ID;
    }

  if (query->NextRow())
    {
    ID = query->DataValue(0).ToInt();
    }
  query->Delete();

  return ID;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int FindOneID(vtkMySQLDatabase* DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string ColumnNameOne,std::string valueOne,
  std::string ColumnNameTwo,std::string valueTwo,
  std::string ColumnNameThree,std::string valueThree,
  std::string ColumnNameFour,std::string valueFour,
  std::string ColumnNameFive,std::string valueFive)
{
  int ID = -1;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << ColumnName;
  querystream << " FROM ";
  querystream << TableName;
  querystream << " WHERE (";
  querystream << ColumnNameOne;
  querystream << " = '";
  querystream << valueOne;
  querystream << "' AND ";
  querystream << ColumnNameTwo;
  querystream << " = '";
  querystream << valueTwo;
  querystream << "' AND ";
  querystream << ColumnNameThree;
  querystream << " = '";
  querystream << valueThree;
  querystream << "' AND ";
  querystream << ColumnNameFour;
  querystream << " = '";
  querystream << valueFour;
  querystream << "' AND ";
  querystream << ColumnNameFive;
  querystream << " = '";
  querystream << valueFive;
  querystream << "');";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "The FindOneID query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return ID;
    }

  if (query->NextRow())
    {
    ID = query->DataValue(0).ToInt();
    }
  query->Delete();

  return ID;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int FindOneID(vtkMySQLDatabase* DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string ColumnNameOne,std::string valueOne,
  std::string ColumnNameTwo,std::string valueTwo,
  std::string ColumnNameThree,std::string valueThree,
  std::string ColumnNameFour,std::string valueFour,
  std::string ColumnNameFive,std::string valueFive,
  std::string ColumnNameSix,std::string valueSix,
  std::string ColumnNameSeven,std::string valueSeven,
  std::string ColumnNameEight,std::string valueEight,
  std::string ColumnNameNine,std::string valueNine,
  std::string ColumnNameTen,std::string valueTen)
{
  int ID = -1;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << ColumnName;
  querystream << " FROM ";
  querystream << TableName;
  querystream << " WHERE (";
  querystream << ColumnNameOne;
  querystream << " = '";
  querystream << valueOne;
  querystream << "' AND ";
  querystream << ColumnNameTwo;
  querystream << " = '";
  querystream << valueTwo;
  querystream << "' AND ";
  querystream << ColumnNameThree;
  querystream << " = '";
  querystream << valueThree;
  querystream << "' AND ";
  querystream << ColumnNameFour;
  querystream << " = '";
  querystream << valueFour;
  querystream << "' AND ";
  querystream << ColumnNameFive;
  querystream << " = '";
  querystream << valueFive;
  querystream << "' AND ";
  querystream << ColumnNameSix;
  querystream << " = '";
  querystream << valueSix;
  querystream << "' AND ";
  querystream << ColumnNameSeven;
  querystream << " = '";
  querystream << valueSeven;
  querystream << "' AND ";
  querystream << ColumnNameEight;
  querystream << " = '";
  querystream << valueEight;
  querystream << "' AND ";
  querystream << ColumnNameNine;
  querystream << " = '";
  querystream << valueNine;
  querystream << "' AND ";
  querystream << ColumnNameTen;
  querystream << " = '";
  querystream << valueTen;
  querystream << "');";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "The FindOneID query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return ID;
    }

  if (query->NextRow())
    {
    ID = query->DataValue(0).ToInt();
    }
  query->Delete();

  return ID;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector<std::string> ListSpecificValuesForOneColumn(
  vtkMySQLDatabase* DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field,std::string value)

{ std::vector< std::string > result;

vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
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
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
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

  query->Delete();

  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//query: "SELECT ColumnName FROM TableName WHERE field = value
//ORDER BY ColumnNameOrder ASC"
std::vector<std::string> ListSpecificValuesForOneColumn(
  vtkMySQLDatabase* DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field,std::string value,std::string ColumnNameOrder)
{
  std::vector< std::string > result;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
    std::stringstream querystream;
    querystream << "SELECT ";
    querystream << ColumnName;
    querystream << " FROM ";
    querystream << TableName;
    querystream << " WHERE ";
    querystream << field;
    querystream << " = '";
    querystream << value;
    querystream << "' ORDER BY ";
    querystream << ColumnNameOrder;
    querystream << " ASC;";

    query->SetQuery( querystream.str().c_str() );
    if ( !query->Execute() )
      {
      itkGenericExceptionMacro(
        << "List of all values of ExpID query failed"
        << query->GetLastErrorText() );
      DatabaseConnector->Close();
      DatabaseConnector->Delete();
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

    query->Delete();

    return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector<std::string> ListSpecificValuesForOneColumn(
  vtkMySQLDatabase* DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field,std::vector<std::string> VectorValuess)
{
  std::vector< std::string > result;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << ColumnName;
  querystream << " FROM ";
  querystream << TableName;
  querystream << " WHERE (";
  unsigned int i;
  for( i=0;i < VectorValuess.size()-1; i++ )
    {
    querystream << field;
    querystream << " = '";
    querystream << VectorValuess[i];
    querystream << "' OR ";
    }
  querystream << field;
  querystream << " = '";
  querystream << VectorValuess[i];
  querystream << "');";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "List of all values of ExpID query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  while (query->NextRow())
    {
    for( int k = 0; k < query->GetNumberOfFields(); k++)
      {
      result.push_back( query->DataValue( k ).ToString() );
      }
    }

  query->Delete();

  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ReturnOnlyOneValue(vtkMySQLDatabase* DatabaseConnector,
  std::string TableName, std::string ColumnName,std::string field,
  std::string value)
{
  std::string result;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << ColumnName;
  querystream << " FROM ";
  querystream << TableName;
  querystream << " WHERE ";
  querystream << field;
  querystream << " = '";
  querystream << value;
  querystream << "' LIMIT 1;";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "List of all values of ExpID query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  while (query->NextRow())
    {
    for( int i = 0; i < query->GetNumberOfFields(); i++)
      {
      result = query->DataValue( i ).ToString();
      }
    }

  query->Delete();

  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int MaxValueForOneColumnInTable(
  vtkMySQLDatabase* DatabaseConnector,
  std::string ColumnName,std::string TableName )
{
  int MaxValue = -1;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
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
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return MaxValue;
    }

  if (query->NextRow())
    {
    MaxValue = query->DataValue(0).ToInt();
    }
  query->Delete();

  return MaxValue;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int MaxValueForOneColumnInTable(vtkMySQLDatabase* DatabaseConnector,
  std::string ColumnName,std::string TableName,std::string field,
  std::string value)
{
  int MaxValue = -1;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT MAX(";
  querystream << ColumnName;
  querystream << ") FROM ";
  querystream << TableName;
  querystream << " WHERE ";
  querystream << field;
  querystream << " = ";
  querystream << value;
  querystream << ";";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "find max value query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return MaxValue;
    }

  if (query->NextRow())
    {
    MaxValue = query->DataValue(0).ToInt();
    }
  query->Delete();

  return MaxValue;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int MaxValueForOneColumnInTable(vtkMySQLDatabase* DatabaseConnector,
  std::string ColumnName,std::string TableName,std::string field,
  std::vector<std::string> VectorValues)
{
  int MaxValue = -1;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT MAX(";
  querystream << ColumnName;
  querystream << ") FROM ";
  querystream << TableName;
  querystream << " WHERE (";
  unsigned int i;
  for( i=0;i < VectorValues.size()-1; i++ )
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
      << "find max value query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return MaxValue;
    }

  if (query->NextRow())
    {
    MaxValue = query->DataValue(0).ToInt();
    }
  query->Delete();

  return MaxValue;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int MinValueForOneColumnInTable(vtkMySQLDatabase* DatabaseConnector,
  std::string ColumnName,std::string TableName,std::string field,
  std::vector<std::string> VectorValues)
{
  int MinValue = -1;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT MIN(";
  querystream << ColumnName;
  querystream << ") FROM ";
  querystream << TableName;
  querystream << " WHERE (";
  unsigned int i;
  for (i=0;i < VectorValues.size()-1; i++)
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
      << "find min value query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return MinValue;
    }

  if (query->NextRow())
    {
    MinValue = query->DataValue(0).ToInt();
    }
  query->Delete();

  return MinValue;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int LastInsertID(std::string ServerName, std::string login,
  std::string Password, std::string DBName)
{
  int ID = -1;
  vtkMySQLDatabase * DatabaseConnector = vtkMySQLDatabase::New();
  DatabaseConnector->SetHostName( ServerName.c_str() );
  DatabaseConnector->SetUser( login.c_str() );
  DatabaseConnector->SetPassword( Password.c_str() );
  DatabaseConnector->SetDatabaseName( DBName.c_str() );
  if( !DatabaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "Find last insert ID can not be done."<< std::endl;
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    return ID;
    }

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  query->SetQuery( "SELECT LAST_INSERT_ID();" );

  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "replace value in DB query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return ID;
    }

  ID = query->DataValue(0).ToInt();
  DatabaseConnector->Close();
  DatabaseConnector->Delete();
  query->Delete();

  return ID;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//query: "SELECT TableOne.ColumnOne, TableTwo.ColumnTwo FROM TableOne
//JOIN TableTwo ON (TableOne.Foreignkey = TableTwo.PrimaryKey)
//WHERE field = value;
std::vector<std::pair<int,std::string> > ListSpecificValuesForTwoColumnsAndTwoTables(
  vtkMySQLDatabase* DatabaseConnector,std::string TableOne, std::string ColumnOne,
  std::string TableTwo, std::string ColumnTwo,std::string ForeignKey, std::string PrimaryKey,
  std::string field, std::string value)
{
  std::vector<std::pair<int,std::string> > result;

  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << TableOne;
  querystream << ".";
  querystream << ColumnOne;
  querystream << ",";
  querystream << TableTwo;
  querystream << ".";
  querystream << ColumnTwo;
  querystream << " FROM ";
  querystream << TableOne;
  querystream << " JOIN ";
  querystream << TableTwo;
  querystream << " ON (";
  querystream << TableOne;
  querystream << ".";
  querystream << ForeignKey;
  querystream << " = ";
  querystream << TableTwo;
  querystream << ".";
  querystream << PrimaryKey;
  querystream << " WHERE ";
  querystream << field;
  querystream << " = '";
  querystream << value;
  querystream << "';";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "join on 2 tables query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  while (query->NextRow())
    {
      {
      std::pair<int, std::string> temp;
      temp.first = query->DataValue(0).ToInt();
      temp.second = query->DataValue(1).ToString();
      result.push_back(temp);
      }
    }
  query->Delete();
  return result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector<ContourMeshStructure>* GetTracesInfoFromDB(
  vtkMySQLDatabase* DatabaseConnector, std::string TraceName,
  std::string CollectionName,unsigned int ImgSessionID)
{
  std::vector<ContourMeshStructure>* Results = new std::vector<ContourMeshStructure>;
  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();

  std::stringstream Querystream;
  Querystream << "SELECT ";
  Querystream << TraceName;
  Querystream << ".";
  Querystream << TraceName;
  Querystream << "ID, ";
  Querystream << TraceName;
  Querystream << ".";
  Querystream << CollectionName;
  Querystream << "ID, ";
  Querystream << TraceName;
  Querystream << ".Points, coordinate.TCoord, color.Red,\
                 color.Green, color.Blue, color.Alpha from (";
  Querystream << TraceName;
  Querystream << " left join coordinate on coordinate.CoordID = ";
  Querystream << TraceName;
  Querystream << ".coordIDMax) left join color on ";
  Querystream << TraceName;
  Querystream << ".colorID = color.colorID  where ImagingSessionID = ";
  Querystream << ImgSessionID;
  Querystream << ";";

  query->SetQuery(Querystream.str().c_str());
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "return info Contours query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return Results;
    }

  while (query->NextRow())
    {
      {
      ContourMeshStructure temp;
      temp.TraceID = query->DataValue(0).ToInt();
      vtkSmartPointer< vtkPolyDataMySQLTextReader > convert_reader =
        vtkSmartPointer< vtkPolyDataMySQLTextReader >::New();
      temp.CollectionID = query->DataValue(1).ToUnsignedInt();
      std::string polydata_string = query->DataValue(2).ToString();
      if (!polydata_string.empty())
        {
        if( TraceName.compare( "contour" ) == 0 )
          {
          convert_reader->SetIsContour( true );
          }
        else
          {
          if( TraceName.compare( "mesh" ) == 0 )
            {
            convert_reader->SetIsContour( false );
            }
          }
        vtkPolyData* output = convert_reader->GetPolyData( polydata_string );
        temp.Nodes = output;
        }
      temp.TCoord       = query->DataValue(3).ToUnsignedInt();
      temp.rgba[0]      = query->DataValue(4).ToDouble();
      temp.rgba[1]      = query->DataValue(5).ToDouble();
      temp.rgba[2]      = query->DataValue(6).ToDouble();
      temp.rgba[3]      = query->DataValue(7).ToDouble();
      Results->push_back(temp);
      }
    }
  query->Delete();
  return Results;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
ContourMeshStructure GetTraceInfoFromDB(
  vtkMySQLDatabase* DatabaseConnector, std::string TraceName,
  std::string CollectionName,unsigned int TraceID)
{
  ContourMeshStructure Results;
  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();

  std::stringstream Querystream;
  Querystream << "SELECT ";
  Querystream << TraceName;
  Querystream << ".";
  Querystream << CollectionName;
  Querystream << "ID, ";
  Querystream << TraceName;
  Querystream << ".Points, coordinate.TCoord, color.Red,\
                 color.Green, color.Blue, color.Alpha from (";
  Querystream << TraceName;
  Querystream << " left join coordinate on coordinate.CoordID = ";
  Querystream << TraceName;
  Querystream << ".coordIDMax) left join color on ";
  Querystream << TraceName;
  Querystream << ".colorID = color.colorID  where ";
  Querystream << TraceName;
  Querystream << "ID = ";
  Querystream << TraceID;
  Querystream << ";";

  query->SetQuery(Querystream.str().c_str());
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "return info Contours query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return Results;
    }

  while (query->NextRow())
    {
      {
      //temp.TraceID = query->DataValue(0).ToInt();
      Results.TraceID = TraceID;
      vtkSmartPointer< vtkPolyDataMySQLTextReader > convert_reader =
        vtkSmartPointer< vtkPolyDataMySQLTextReader >::New();
      Results.CollectionID = query->DataValue(0).ToUnsignedInt();
      std::string polydata_string = query->DataValue(1).ToString();
      if (!polydata_string.empty())
        {
        if( TraceName.compare( "contour" ) == 0 )
          {
          convert_reader->SetIsContour( true );
          }
        else
          {
          if( TraceName.compare( "mesh" ) == 0 )
            {
            convert_reader->SetIsContour( false );
            }
          }
        vtkPolyData* output = convert_reader->GetPolyData( polydata_string );
        Results.Nodes = output;
        }
      Results.TCoord       = query->DataValue(2).ToUnsignedInt();
      Results.rgba[0]      = query->DataValue(3).ToDouble();
      Results.rgba[1]      = query->DataValue(4).ToDouble();
      Results.rgba[2]      = query->DataValue(5).ToDouble();
      Results.rgba[3]      = query->DataValue(6).ToDouble();
      }
    }
  query->Delete();
  return Results;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector<std::vector<std::string> >GetValuesFromSeveralTables(
  vtkMySQLDatabase* DatabaseConnector,std::string MainTable,
  std::vector<std::string> SelectFields, std::string field,
  std::string value,std::vector<std::string> JoinTablesOnTraceTable, bool Distinct)
{
  std::vector<std::vector<std::string> > Results;
  vtkSQLQuery* query = DatabaseConnector->GetQueryInstance();

  std::stringstream Querystream;
  Querystream << "SELECT ";
  if (Distinct)
    {
    Querystream << "DISTINCT ";
    }
  unsigned int i;
  for (i=0; i <SelectFields.size()-1;i++)
    {
    Querystream <<SelectFields[i];
    Querystream << ",";
    }
  Querystream << SelectFields[i];
  Querystream << " FROM ";
  Querystream << MainTable;
  unsigned int j =0;
  while (j<JoinTablesOnTraceTable.size())
    {
    Querystream << " LEFT JOIN ";
    Querystream << JoinTablesOnTraceTable[j];
    Querystream << " ON ";
    Querystream << JoinTablesOnTraceTable[j+1];
    Querystream << " ";
    j=j+2;
    } 
  Querystream << "WHERE ";
  Querystream << MainTable;
  Querystream << ".";
  Querystream << field;
  Querystream << " = ";
  Querystream << value;
  Querystream << ";";

  query->SetQuery(Querystream.str().c_str());
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "return info Contours query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return Results;
    }
  while (query->NextRow())
    {
    std::vector<std::string> ResultsForOneRow;
    for( int i = 0; i < query->GetNumberOfFields(); i++)
      {
      ResultsForOneRow.push_back( query->DataValue( i ).ToString() );
      }
    Results.push_back(ResultsForOneRow);
    }
  
  query->Delete();

  return Results;

}
