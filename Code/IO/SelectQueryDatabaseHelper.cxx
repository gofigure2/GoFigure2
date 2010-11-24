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
#include "SelectQueryDatabaseHelper.h"

#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "vtkStdString.h"
#include "vtkSmartPointer.h"
#include "vtkVariant.h"
#include "vtkPolyDataMySQLContourReader.h"
#include "vtkPolyDataMySQLMeshReader.h"
#include "vtkPolyDataMySQLTrackReader.h"

#include <sstream>
#include <string>

std::vector< std::string > ListAllValuesForOneColumn(vtkMySQLDatabase *DatabaseConnector,
                                                     std::string ColumnName, std::string TableName,
                                                     std::string OrderByColumnName)
{
  std::string QueryString = SelectQueryStream(TableName, ColumnName,OrderByColumnName);
  return ExecuteSelectQuery(DatabaseConnector,QueryString);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::map< std::string, std::string > MapTwoColumnsFromTable(
  vtkMySQLDatabase *DatabaseConnector,std::vector<std::string> iColumnNames,
  std::string iTableName, std::string iField, std::string iValue)
{
  std::map< std::string, std::string > Result = std::map<std::string,std::string>();
  if (iColumnNames.size() != 2)
    {
    std::cout<<"can not map if the size of the vector for the selected fields is different than 2 ";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return Result;
    }
  std::string  QueryString;
  if (iField.empty())
    {
    QueryString = SelectQueryStream(iTableName,iColumnNames);
    }
  else
    {
    QueryString = SelectQueryStreamCondition(iTableName,iColumnNames,iField,iValue);
    }

  vtkSQLQuery * query = DatabaseConnector->GetQueryInstance();
  query->SetQuery( QueryString.c_str() );

  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "List of all fields query failed"
      << query->GetLastErrorText() );
    query->Delete();
    return Result;
    }
  while ( query->NextRow() )
    {
    Result[query->DataValue(0).ToString()] = query->DataValue(1).ToString();
    }
  query->Delete();

  return Result;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::pair< std::string, std::string > >
VectorTwoColumnsFromTable(vtkMySQLDatabase *DatabaseConnector,
                          std::string ColumnNameOne, std::string ColumnNameTwo,
                          std::string TableName, std::string OrderByColumnName)
{
  std::vector< std::pair< std::string, std::string > > result;

  std::vector<std::string> ColumnNames(2);
  ColumnNames[0] = ColumnNameOne;
  ColumnNames[1] = ColumnNameTwo;
  std::string QueryString = SelectQueryStream(TableName, ColumnNames,
                              OrderByColumnName);
  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
  query->SetQuery(QueryString.c_str());
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "select 2 columns query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  while ( query->NextRow() )
    {
    std::pair< std::string, std::string > Pair;
    Pair.first = query->DataValue(0).ToString();
    Pair.second = query->DataValue(1).ToString();
    result.push_back(Pair);
    }

  query->Delete();

  return result;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::string > ListSpecificValuesForRow(
  vtkMySQLDatabase *DatabaseConnector, std::string TableName, std::string field,
  std::string value)
{
  std::string QueryString = SelectQueryStreamCondition(TableName,"*",field,value);
  return ExecuteSelectQuery(DatabaseConnector,QueryString);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int FindOneID(vtkMySQLDatabase *DatabaseConnector,
              std::string TableName, std::string ColumnName,
              std::string field, std::string value)
{
  int ID = -1;

  std::string QueryString = 
    SelectQueryStreamCondition(TableName,ColumnName,field,value);
  std::vector<std::string> Results = ExecuteSelectQuery(
    DatabaseConnector,QueryString);
  if (Results.size() > 1)
    {
    std::cout<<"there is not an unique ID";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return ID;
    }
  ID = atoi( Results[0].c_str() );
  return ID;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int FindOneID(vtkMySQLDatabase *DatabaseConnector,
              std::string TableName, std::string ColumnName,
              std::vector<FieldWithValue> iConditions)
{
  std::string QueryString = SelectQueryStreamListConditions(TableName,
                            ColumnName, iConditions,"AND");
  int ID = -1;

  std::vector<std::string> Results = ExecuteSelectQuery(
    DatabaseConnector,QueryString);
  
  if (!Results.empty())
    {
    ID = atoi( Results[0].c_str() );
    } 
  return ID;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::string > FindSeveralIDs(vtkMySQLDatabase * iDatabaseConnector,
  std::string TableName, std::string ColumnName, std::vector<FieldWithValue> iConditions)
{
  std::string QueryString = SelectQueryStreamListConditions(TableName,
                            ColumnName, iConditions,"AND");
 
  return ExecuteSelectQuery(iDatabaseConnector,QueryString);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::string > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::string value, bool Distinct,
  bool ExcludeZero)
{
  std::vector< std::string > result;
  vtkSQLQuery *              query = DatabaseConnector->GetQueryInstance();

  std::stringstream querystream;
  querystream << "SELECT ";
  if ( Distinct )
    {
    querystream << "DISTINCT ";
    }
  querystream << ColumnName;
  querystream << " FROM ";
  querystream << TableName;
  if ( field.empty() )
    {
    querystream << ";";
    }
  else
    {
    querystream << " WHERE ";
    querystream << field;
    querystream << " = '";
    querystream << value;
    //querystream << "';";
    querystream << "'";
    }
  if ( ExcludeZero )
    {
    querystream << " AND WHERE ";
    querystream << ColumnName;
    querystream << " <> 0";
    }

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

  while ( query->NextRow() )
    {
    for ( int i = 0; i < query->GetNumberOfFields(); i++ )
      {
      result.push_back( query->DataValue(i).ToString() );
      }
    }

  query->Delete();

  return result;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//query: "SELECT ColumnName FROM TableName WHERE field = value
//ORDER BY ColumnNameOrder ASC"
std::vector< std::string > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::string value, std::string ColumnNameOrder)
{
  std::vector< std::string > result;

  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
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

  while ( query->NextRow() )
    {
    for ( int i = 0; i < query->GetNumberOfFields(); i++ )
      {
      result.push_back( query->DataValue(i).ToString() );
      }
    }

  query->Delete();

  return result;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::string > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::vector< std::string > VectorValues,
  bool Distinct, bool ExcludeZero)
{
  std::vector< std::string > result;

  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  if ( Distinct )
    {
    querystream << "DISTINCT ";
    }
  querystream << ColumnName;
  querystream << " FROM ";
  querystream << TableName;
  querystream << " WHERE (";
  if ( ExcludeZero )
    {
    querystream << ColumnName;
    querystream << " <> 0 AND (";
    }

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
  //querystream << "');";
  querystream << "'";
  querystream << ")";
  if ( ExcludeZero )
    {
    querystream << ")";
    }

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

  while ( query->NextRow() )
    {
    for ( int k = 0; k < query->GetNumberOfFields(); k++ )
      {
      result.push_back( query->DataValue(k).ToString() );
      }
    }

  query->Delete();

  return result;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::list< unsigned int > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::vector< unsigned int > ListValues,
  bool Distinct, bool ExcludeZero)
{
  std::list< unsigned int > result;

  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  if ( Distinct )
    {
    querystream << "DISTINCT ";
    }
  querystream << ColumnName;
  querystream << " FROM ";
  querystream << TableName;
  querystream << " WHERE (";
  if ( ExcludeZero )
    {
    querystream << ColumnName;
    querystream << " <> 0 AND (";
    }

  unsigned int i;
  for ( i = 0; i < ListValues.size() - 1; i++ )
    {
    querystream << field;
    querystream << " = '";
    querystream << ListValues[i];
    querystream << "' OR ";
    }
  querystream << field;
  querystream << " = '";
  querystream << ListValues[i];
  //querystream << "');";
  querystream << "'";
  querystream << ")";
  if ( ExcludeZero )
    {
    querystream << ")";
    }

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

  while ( query->NextRow() )
    {
    for ( int k = 0; k < query->GetNumberOfFields(); k++ )
      {
      result.push_back( query->DataValue(k).ToUnsignedInt() );
      }
    }

  query->Delete();

  return result;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//query: "SELECT ColumnNameOne,ColumnName2 FROM TableName
//WHERE field = value ORDER BY ColumnNameOrder ASC"
std::vector< std::pair< std::string, std::string > >
ListSpecificValuesForTwoColumns(vtkMySQLDatabase *DatabaseConnector,
                                std::string TableName, std::string ColumnNameOne, std::string ColumnNameTwo,
                                std::string field, std::string value, std::string ColumnNameOrder)
{
  std::vector< std::pair< std::string, std::string > > result;

  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << ColumnNameOne;
  querystream << ",";
  querystream << ColumnNameTwo;
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
      << "select 2 columns query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  while ( query->NextRow() )
    {
    std::pair< std::string, std::string > Pair;
    Pair.first = query->DataValue(0).ToString();
    Pair.second = query->DataValue(1).ToString();
    result.push_back(Pair);
    }

  query->Delete();

  return result;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string ReturnOnlyOneValue(vtkMySQLDatabase *DatabaseConnector,
                               std::string TableName, std::string ColumnName, std::string field,
                               std::string value)
{
  std::string result;

  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
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

  while ( query->NextRow() )
    {
    for ( int i = 0; i < query->GetNumberOfFields(); i++ )
      {
      result = query->DataValue(i).ToString();
      }
    }

  query->Delete();

  return result;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int MaxValueForOneColumnInTable(
  vtkMySQLDatabase *DatabaseConnector,
  std::string ColumnName, std::string TableName)
{
  int MaxValue = -1;

  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
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

  if ( query->NextRow() )
    {
    MaxValue = query->DataValue(0).ToInt();
    }
  query->Delete();

  return MaxValue;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int MaxValueForOneColumnInTable(vtkMySQLDatabase *DatabaseConnector,
                                std::string ColumnName, std::string TableName, std::string field,
                                std::string value)
{
  int MaxValue = -1;

  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
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

  if ( query->NextRow() )
    {
    MaxValue = query->DataValue(0).ToInt();
    }
  query->Delete();

  return MaxValue;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int MaxValueForOneColumnInTable(vtkMySQLDatabase *DatabaseConnector,
                                std::string ColumnName, std::string TableName, std::string field,
                                std::vector< std::string > VectorValues)
{
  int MaxValue = -1;

  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;

  querystream << "SELECT MAX(";
  querystream << ColumnName;
  querystream << ") FROM ";
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
      << "find max value query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return MaxValue;
    }

  if ( query->NextRow() )
    {
    MaxValue = query->DataValue(0).ToInt();
    }
  query->Delete();

  return MaxValue;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int MinValueForOneColumnInTable(vtkMySQLDatabase *DatabaseConnector,
                                std::string ColumnName, std::string TableName, std::string field,
                                std::vector< std::string > VectorValues)
{
  int MinValue = -1;

  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;

  querystream << "SELECT MIN(";
  querystream << ColumnName;
  querystream << ") FROM ";
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
      << "find min value query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return MinValue;
    }

  if ( query->NextRow() )
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
  int               ID = -1;
  vtkMySQLDatabase *DatabaseConnector = vtkMySQLDatabase::New();

  DatabaseConnector->SetHostName( ServerName.c_str() );
  DatabaseConnector->SetUser( login.c_str() );
  DatabaseConnector->SetPassword( Password.c_str() );
  DatabaseConnector->SetDatabaseName( DBName.c_str() );
  if ( !DatabaseConnector->Open() )
    {
    std::cerr << "Could not open database." << std::endl;
    std::cerr << "Find last insert ID can not be done." << std::endl;
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    return ID;
    }

  vtkSQLQuery *query = DatabaseConnector->GetQueryInstance();
  query->SetQuery("SELECT LAST_INSERT_ID();");

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
std::vector< std::pair< int, std::string > > ListSpecificValuesForTwoColumnsAndTwoTables(
  vtkMySQLDatabase *DatabaseConnector, std::string TableOne, std::string ColumnOne,
  std::string TableTwo, std::string ColumnTwo, std::string ForeignKey, std::string PrimaryKey,
  std::string field, std::string value)
{
  std::vector< std::pair< int, std::string > > result;

  vtkSQLQuery *     query = DatabaseConnector->GetQueryInstance();
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

  while ( query->NextRow() )
    {
      {
      std::pair< int, std::string > temp;
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
ContourMeshStructure GetTraceInfoFromDB(
  vtkMySQLDatabase *DatabaseConnector, std::string TraceName,
  std::string CollectionName, unsigned int TraceID)
{
  ContourMeshStructure Results;
  vtkSQLQuery *        query = DatabaseConnector->GetQueryInstance();

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

  query->SetQuery( Querystream.str().c_str() );
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

  while ( query->NextRow() )
    {
      {
      //temp.TraceID = query->DataValue(0).ToInt();
      Results.TraceID = TraceID;
      vtkPolyData *output = vtkPolyData::New();
            std::string polydata_string = query->DataValue(1).ToString();
      if ( !polydata_string.empty() )
        {
        if ( TraceName.compare("contour") == 0 )
          {
          vtkSmartPointer< vtkPolyDataMySQLContourReader > convert_reader =
            vtkSmartPointer< vtkPolyDataMySQLContourReader >::New();
          output->DeepCopy(convert_reader->GetPolyData(polydata_string));
          }
        else
          {
          if ( TraceName.compare("mesh") == 0 )
            {
            vtkIdType N;
            std::stringstream str(polydata_string);
            str >> N;

            if( N > 0 )
              {
              vtkSmartPointer< vtkPolyDataMySQLMeshReader > convert_reader =
                vtkSmartPointer< vtkPolyDataMySQLMeshReader >::New();
              output->DeepCopy(convert_reader->GetPolyData(polydata_string));
              }
            else
              {
              output->Delete();
              output = NULL;
              }
            }
          }
        Results.Nodes = output;
        }
      Results.TCoord       = query->DataValue(2).ToUnsignedInt();
      /// \note For the visualization rgba values are supposed to be double in
      /// between 0 and 1; whereas in the database these values are in between
      /// 0 and 255.
      Results.rgba[0]      = query->DataValue(3).ToDouble() / 255.;
      Results.rgba[1]      = query->DataValue(4).ToDouble() / 255.;
      Results.rgba[2]      = query->DataValue(5).ToDouble() / 255.;
      Results.rgba[3]      = query->DataValue(6).ToDouble() / 255.;
      }
    }
  query->Delete();
  return Results;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/*ContourMeshStructureMultiIndexContainer* GetTracesInfoFromDBMultiIndex(
  vtkMySQLDatabase* DatabaseConnector, std::string TraceName,
  std::string CollectionName, std::string WhereField,
  unsigned int ImgSessionID, int iTimePoint, std::vector<int> iListIDs)
{
  ContourMeshStructureMultiIndexContainer* Results =
    new ContourMeshStructureMultiIndexContainer;
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
                 color.Green, color.Blue, color.Alpha from ("                                                          ;
  Querystream << TraceName;
  Querystream << " left join coordinate on coordinate.CoordID = ";
  Querystream << TraceName;
  Querystream << ".coordIDMax) left join color on ";
  Querystream << TraceName;
  if (iTimePoint != -1)
    {
    Querystream << ".colorID = color.colorID  where ( ";
    Querystream <<  WhereField;
    Querystream << " = ";
    Querystream << ImgSessionID;
    Querystream << " and ";
    Querystream << "coordinate.TCoord = ";
    Querystream << iTimePoint;
    if (!iListIDs.empty())
      {
      Querystream << " and (";
      unsigned int i;
      for (i = 0; i < iListIDs.size() - 1; i++)
        {
        Querystream << TraceName;
        Querystream << "ID = '";
        Querystream << iListIDs[i];
        Querystream << "' OR ";
        }
      Querystream << TraceName;
      Querystream << "ID = '";
      Querystream << iListIDs[i];
      Querystream << "')";
      }
    Querystream << ");";
    }
  else
    {
    Querystream << ".colorID = color.colorID  where ImagingSessionID = ";
    Querystream << ImgSessionID;
    Querystream << ";";
    }

  query->SetQuery(Querystream.str().c_str());
  if (!query->Execute())
    {
    itkGenericExceptionMacro(
      << "return info Contours query failed"
      << query->GetLastErrorText());
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
      vtkSmartPointer<vtkPolyDataMySQLTextReader> convert_reader =
        vtkSmartPointer<vtkPolyDataMySQLTextReader>::New();
      //temp.CollectionID = query->DataValue(1).ToUnsignedInt();
      std::string polydata_string = query->DataValue(2).ToString();
      if (!polydata_string.empty())
        {
        if (TraceName.compare("contour") == 0)
          {
          convert_reader->SetIsContour(true);
          }
        else
          {
          if (TraceName.compare("mesh") == 0)
            {
            convert_reader->SetIsContour(false);
            }
          }
        vtkPolyData* output = convert_reader->GetPolyData(polydata_string);
        temp.Nodes = output;
        }
      temp.TCoord       = query->DataValue(3).ToUnsignedInt();
      /// \note For the visualization rgba values are supposed to be double in
      /// between 0 and 1; whereas in the database these values are in between
      /// 0 and 255.
      temp.rgba[0]      = (query->DataValue(4).ToDouble()) / 255.;
      temp.rgba[1]      = (query->DataValue(5).ToDouble()) / 255.;
      temp.rgba[2]      = (query->DataValue(6).ToDouble()) / 255.;
      temp.rgba[3]      = (query->DataValue(7).ToDouble()) / 255.;
      Results->insert(temp);
      }
    }
  query->Delete();
  return Results;
}*/
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::vector< std::string > > GetValuesFromSeveralTables(
  vtkMySQLDatabase *DatabaseConnector, std::string MainTable,
  std::vector< std::string > SelectFields, std::string field,
  std::string value, std::vector< std::string > JoinTablesOnTraceTable, bool Distinct)
{
  std::vector< std::vector< std::string > > Results;
  vtkSQLQuery *                             query = DatabaseConnector->GetQueryInstance();

  std::stringstream Querystream;
  Querystream << "SELECT ";
  if ( Distinct )
    {
    Querystream << "DISTINCT ";
    }
  unsigned int i;
  for ( i = 0; i < SelectFields.size() - 1; i++ )
    {
    Querystream << SelectFields[i];
    Querystream << ",";
    }
  Querystream << SelectFields[i];
  Querystream << " FROM ";
  Querystream << MainTable;
  unsigned int j = 0;
  while ( j < JoinTablesOnTraceTable.size() )
    {
    Querystream << " LEFT JOIN ";
    Querystream << JoinTablesOnTraceTable[j];
    Querystream << " ON ";
    Querystream << JoinTablesOnTraceTable[j + 1];
    Querystream << " ";
    j = j + 2;
    }
  Querystream << "WHERE ";
  Querystream << MainTable;
  Querystream << ".";
  Querystream << field;
  Querystream << " = ";
  Querystream << value;
  Querystream << ";";

  query->SetQuery( Querystream.str().c_str() );
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
  while ( query->NextRow() )
    {
    std::vector< std::string > ResultsForOneRow;
    for ( int k = 0; k < query->GetNumberOfFields(); k++ )
      {
      ResultsForOneRow.push_back( query->DataValue(k).ToString() );
      }
    Results.push_back(ResultsForOneRow);
    }

  query->Delete();

  return Results;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::vector< std::string > > GetValuesFromSeveralTables(
  vtkMySQLDatabase *DatabaseConnector, std::string MainTable,
  std::vector< std::string > SelectFields, std::vector< std::string > WhereAndConditions,
  std::vector< std::string > JoinTablesOnTraceTable, bool Distinct)
{
  std::vector< std::vector< std::string > > Results;
  vtkSQLQuery *                             query = DatabaseConnector->GetQueryInstance();

  std::stringstream Querystream;
  Querystream << "SELECT ";
  if ( Distinct )
    {
    Querystream << "DISTINCT ";
    }
  unsigned int i;
  for ( i = 0; i < SelectFields.size() - 1; i++ )
    {
    Querystream << SelectFields[i];
    Querystream << ",";
    }
  Querystream << SelectFields[i];
  Querystream << " FROM (";
  Querystream << MainTable;
  unsigned int j = 0;
  while ( j < JoinTablesOnTraceTable.size() - 1 )
    {
    Querystream << " LEFT JOIN ";
    Querystream << JoinTablesOnTraceTable[j];
    Querystream << " ON ";
    Querystream << JoinTablesOnTraceTable[j + 1];
    Querystream << " ";
    j = j + 2;
    }

  unsigned int k = 0;
  Querystream << ") WHERE (";
  while ( k < WhereAndConditions.size() - 2 )
    {
    Querystream << WhereAndConditions[k];
    Querystream << " = ";
    Querystream << WhereAndConditions[k + 1];
    Querystream << " AND ";
    k = k + 2;
    }

  Querystream << WhereAndConditions[k];
  Querystream << " = ";
  Querystream << WhereAndConditions[k + 1];
  Querystream << ");";

  query->SetQuery( Querystream.str().c_str() );
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
  while ( query->NextRow() )
    {
    std::vector< std::string > ResultsForOneRow;
    for ( int kk = 0; kk < query->GetNumberOfFields(); kk++ )
      {
      ResultsForOneRow.push_back( query->DataValue(kk).ToString() );
      }
    Results.push_back(ResultsForOneRow);
    }

  query->Delete();

  return Results;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::string > GetSamefieldFromTwoTables(
  vtkMySQLDatabase *DatabaseConnector, std::string iTableOne,
  std::string iTableTwo, std::string iColumn, std::string iField,
  std::string iValue, std::string iFieldTwo,
  std::vector< std::string > iListConditionsTwo)
{
  std::vector< std::string > result;
  vtkSQLQuery *              query = DatabaseConnector->GetQueryInstance();
  std::stringstream          querystream;
  querystream << SelectQueryStreamCondition(iTableOne, iColumn, iField, iValue);
  querystream << " UNION ";
  querystream << SelectQueryStreamListConditions(iTableTwo, iColumn,
                                                 iFieldTwo, iListConditionsTwo);

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "List of all values for 2 tables query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  while ( query->NextRow() )
    {
    for ( int i = 0; i < query->GetNumberOfFields(); i++ )
      {
      result.push_back( query->DataValue(i).ToString() );
      }
    }

  query->Delete();

  return result;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::string > GetSamefieldsFromTwoTables(vtkMySQLDatabase *DatabaseConnector,
                                                      std::string iTableOne,
                                                      std::string iTableTwo,
                                                      std::string iColumnOne,
                                                      std::string iColumnTwo,
                                                      std::string iField,
                                                      std::string iValue)
{
  std::vector< std::string > result;
  vtkSQLQuery *              query = DatabaseConnector->GetQueryInstance();
  std::stringstream          querystream;
  querystream << SelectQueryStreamCondition(iTableOne, iColumnOne, iField, iValue);
  querystream << " UNION ";
  querystream << SelectQueryStreamCondition(iTableOne, iColumnTwo, iField, iValue);
  querystream << " UNION ";
  querystream << SelectQueryStreamCondition(iTableTwo, iColumnOne, iField, iValue);
  querystream << " UNION ";
  querystream << SelectQueryStreamCondition(iTableTwo, iColumnTwo, iField, iValue);

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "List of all values for 2 tables query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  while ( query->NextRow() )
    {
    for ( int i = 0; i < query->GetNumberOfFields(); i++ )
      {
      result.push_back( query->DataValue(i).ToString() );
      }
    }

  query->Delete();

  return result;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::string > GetSamefieldsFromTwoTables(vtkMySQLDatabase *DatabaseConnector,
                                                      std::string iTableOne,
                                                      std::string iTableTwo,
                                                      std::string iColumnOne,
                                                      std::string iColumnTwo,
                                                      std::string iField,
                                                      std::string iValue,
                                                      std::string iFieldTwo,
                                                      std::vector< std::string > iListConditionsTwo)
{
  std::vector< std::string > result;
  vtkSQLQuery *              query = DatabaseConnector->GetQueryInstance();
  std::stringstream          querystream;
  querystream << SelectQueryStreamCondition(iTableOne, iColumnOne, iField, iValue);
  querystream << " UNION ";
  querystream << SelectQueryStreamCondition(iTableOne, iColumnTwo, iField, iValue);
  querystream << " UNION ";
  querystream << SelectQueryStreamListConditions(iTableTwo, iColumnOne,
                                                 iFieldTwo, iListConditionsTwo);
  querystream << " UNION ";
  querystream << SelectQueryStreamListConditions(iTableTwo, iColumnTwo,
                                                 iFieldTwo, iListConditionsTwo);

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "List of all values for 2 tables query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  while ( query->NextRow() )
    {
    for ( int i = 0; i < query->GetNumberOfFields(); i++ )
      {
      result.push_back( query->DataValue(i).ToString() );
      }
    }

  query->Delete();

  return result;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::string > GetSameFieldsFromSeveralTables(
  vtkMySQLDatabase *DatabaseConnector, std::vector< std::string > iColumnNames,
  std::vector< std::string > iVectorTablesNames,
  std::vector< std::string > iVectorConditionFieldNames,
  std::vector< std::vector< std::string > > iVectorConditionsValues)
{
  std::vector< std::string > result;
  vtkSQLQuery *              query = DatabaseConnector->GetQueryInstance();
  std::stringstream          querystream;
  unsigned int               i = 0;
  while ( i < iVectorTablesNames.size() - 1 )
    {
    for ( unsigned int j = 0; j < iColumnNames.size(); j++ )
      {
      querystream << SelectQueryStreamListConditions(
        iVectorTablesNames[i], iColumnNames[j], iVectorConditionFieldNames[i],
        iVectorConditionsValues[i], true);
      querystream << " UNION DISTINCT ";
      }
    i++;
    }
  unsigned j = 0;
  while ( j < iColumnNames.size() - 1 )
    {
    querystream << SelectQueryStreamListConditions(
      iVectorTablesNames[i], iColumnNames[j], iVectorConditionFieldNames[i],
      iVectorConditionsValues[i], true);
    querystream << " UNION DISTINCT ";
    j++;
    }

  querystream << SelectQueryStreamListConditions(
    iVectorTablesNames[i], iColumnNames[j], iVectorConditionFieldNames[i],
    iVectorConditionsValues[i], true);

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "List of same field for different tables query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  while ( query->NextRow() )
    {
    for ( int k = 0; k < query->GetNumberOfFields(); k++ )
      {
      result.push_back( query->DataValue(k).ToString() );
      }
    }

  query->Delete();

  return result;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//SELECT iColumnName FROM TableName WHERE ( (iFieldOne =
// iVectorConditionFieldOne(i)
// OR iFieldOne = iVectorConditionFieldOne(i+1...) AND (iFieldTwo =
// iVectorConditionFieldTwo(j) OR
//iVectorConditionFieldTwo(j+1)... ) );
std::vector< std::string > GetSpecificValueFromOneTableWithConditionsOnTwoColumns(
  vtkMySQLDatabase *DatabaseConnector, std::string iColumnName, std::string iTableName,
  std::string iFieldOne, std::vector< std::string > iVectorConditionFieldOne,
  std::string iFieldTwo, std::vector< std::string > iVectorConditionFieldTwo)
{
  std::vector< std::string > result;
  vtkSQLQuery *              query = DatabaseConnector->GetQueryInstance();

  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << iColumnName;
  querystream << " FROM ";
  querystream << iTableName;
  querystream << " WHERE ((";
  unsigned int i = 0;
  while ( i < iVectorConditionFieldOne.size() - 1 )
    {
    querystream << iFieldOne;
    querystream << " = '";
    querystream << iVectorConditionFieldOne.at(i);
    querystream << "' OR ";
    i++;
    }
  querystream << iFieldOne;
  querystream << " = '";
  querystream << iVectorConditionFieldOne.at(i);
  querystream << "') AND (";

  unsigned j = 0;
  while ( j < iVectorConditionFieldTwo.size() - 1 )
    {
    querystream << iFieldTwo;
    querystream << " = '";
    querystream << iVectorConditionFieldTwo.at(j);
    querystream << "' OR ";
    j++;
    }
  querystream << iFieldTwo;
  querystream << " = '";
  querystream << iVectorConditionFieldTwo.at(j);
  querystream << "'))";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "GetSpecificValueFromOneTableWithConditionsOnTwoColumns query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  while ( query->NextRow() )
    {
    for ( int k = 0; k < query->GetNumberOfFields(); k++ )
      {
      result.push_back( query->DataValue(k).ToString() );
      }
    }

  query->Delete();

  return result;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > GetColumnForBoundedValue(std::string iColumnName,
                                                   std::string iTableName,
                                                   std::string iImgSessionID,
                                                   std::string iCoordType,
                                                   std::string iValue,
                                                   vtkMySQLDatabase *DatabaseConnector)
{
  std::list< unsigned int > result;
  vtkSQLQuery *             query = DatabaseConnector->GetQueryInstance();

  std::stringstream querystream;
  querystream << "SELECT T1.";
  querystream << iColumnName;
  querystream << " FROM (SELECT ";
  querystream << iTableName;
  querystream << ".";
  querystream << iColumnName;
  querystream << " FROM ";
  querystream << iTableName;
  querystream << " LEFT JOIN coordinate ON ";
  querystream << iTableName;
  querystream << ".CoordIDMax = coordinate.coordid WHERE (ImagingsessionID = ";
  querystream << iImgSessionID;
  querystream << " AND Coordinate.";
  querystream << iCoordType;
  querystream << " > ";
  querystream << iValue;
  querystream << ")) AS T1 INNER JOIN (SELECT ";
  querystream << iTableName;
  querystream << ".";
  querystream << iColumnName;
  querystream << " FROM ";
  querystream << iTableName;
  querystream << " LEFT JOIN coordinate ON ";
  querystream << iTableName;
  querystream << ".CoordIDMin = coordinate.coordid WHERE (ImagingsessionID = ";
  querystream << iImgSessionID;
  querystream << " AND Coordinate.";
  querystream << iCoordType;
  querystream << " < ";
  querystream << iValue;
  querystream << ")) AS T2 on T1.";
  querystream << iColumnName;
  querystream << " = T2.";
  querystream << iColumnName;

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "GetColumnForBoundedValue query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  while ( query->NextRow() )
    {
    for ( int k = 0; k < query->GetNumberOfFields(); k++ )
      {
      result.push_back( query->DataValue(k).ToInt() );
      }
    }

  query->Delete();

  return result;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > GetSpecificValuesEqualToZero(
  vtkMySQLDatabase *iDatabaseConnector, std::string iColumnName, std::string iTableName,
  std::vector< std::string > iVectorConditionFieldOne,
  std::string iFieldTwo)
{
  std::list< unsigned int > result;
  vtkSQLQuery *             query = iDatabaseConnector->GetQueryInstance();
  std::stringstream         querystream;
  std::string               temp = SelectQueryStreamListConditions(iTableName,
                                                                   iColumnName, iColumnName,
                                                                   iVectorConditionFieldOne);
  temp = temp.substr(0, temp.size() - 1);
  querystream << temp;
  querystream << " AND ";
  querystream << iFieldTwo;
  querystream << " = 0);";

  query->SetQuery( querystream.str().c_str() );
  /** \todo Lydie: check when several meshesID are in the query*/
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "GetColumnForBoundedValue query failed"
      << query->GetLastErrorText() );
    iDatabaseConnector->Close();
    iDatabaseConnector->Delete();
    query->Delete();
    return result;
    }

  while ( query->NextRow() )
    {
    for ( int k = 0; k < query->GetNumberOfFields(); k++ )
      {
      result.push_back( query->DataValue(k).ToInt() );
      }
    }

  query->Delete();

  return result;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< std::string > ExecuteSelectQuery(vtkMySQLDatabase *iDatabaseConnector,
                                              std::string iQuery)
{
  vtkSQLQuery *query = iDatabaseConnector->GetQueryInstance();

  std::vector< std::string > Results;
  query->SetQuery( iQuery.c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Execute select query failed"
      << query->GetLastErrorText() );
    iDatabaseConnector->Close();
    iDatabaseConnector->Delete();
    query->Delete();
    return Results;
    }
  while ( query->NextRow() )
    {
    for ( int k = 0; k < query->GetNumberOfFields(); k++ )
      {
      Results.push_back( query->DataValue(k).ToString() );
      }
    }

  query->Delete();

  return Results;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GetAllSelectedFields(std::stringstream & ioQuerystream,
                          std::vector< std::string > iSelectedFields)
{
  size_t i = 0;

  for (; i < iSelectedFields.size() - 1; i++ )
    {
    ioQuerystream << iSelectedFields[i];
    ioQuerystream << ", ";
    }
  ioQuerystream << iSelectedFields[i];
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< std::string > GetAllSelectedValuesFromTwoTables(
  vtkMySQLDatabase *iDatabaseConnector, std::string iTableOne, std::string iTableTwo,
  std::vector< std::string > iSelectedFields, std::string iJoinCondition,
  std::string ifield, std::string ifieldValue)
{
  std::stringstream QueryStream;

  QueryStream << "SELECT ";
  std::vector< std::string >::iterator iter = iSelectedFields.begin();
  GetAllSelectedFields(QueryStream, iSelectedFields);
  QueryStream << " FROM ";
  QueryStream << iTableOne;
  QueryStream << " LEFT JOIN ";
  QueryStream << iTableTwo;
  QueryStream << " ON ";
  QueryStream << iJoinCondition;
  QueryStream << " WHERE ";
  QueryStream << ifield;
  QueryStream << " = ";
  QueryStream << ifieldValue;

  //std::stringstream SelectQuery;
  //SelectQuery << SelectWithJoinNullIncluded(QueryStream.str(), iJoinCondition);
  //return ExecuteSelectQuery( iDatabaseConnector, SelectQuery.str() );
  return ExecuteSelectQuery( iDatabaseConnector, QueryStream.str() );
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< std::string > GetOrderByWithLimit(vtkMySQLDatabase *iDatabaseConnector,
                                               std::string iColumnName, std::string iTableName, std::string iField,
                                               std::string iValue, bool ASC, std::string iNumberLimit)
{
  std::stringstream QueryStream;

  QueryStream << SelectQueryStreamCondition(iTableName, iColumnName, iField, iValue);
  QueryStream << " ORDER BY ";
  QueryStream << iColumnName;
  if ( ASC )
    {
    QueryStream << " ASC LIMIT ";
    }
  else
    {
    QueryStream << " DESC LIMIT ";
    }
  QueryStream << iNumberLimit;
  return ExecuteSelectQuery( iDatabaseConnector, QueryStream.str() );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void GetTracesInfoFromDBAndModifyContainer(
  std::list< ContourMeshStructure > & ioContainer,
  vtkMySQLDatabase *DatabaseConnector, std::string TraceName,
  std::string CollectionName, unsigned int ImgSessionID, int iTimePoint,
  std::vector< int > iVectIDs)
{
  vtkSQLQuery *query = DatabaseConnector->GetQueryInstance();

  std::stringstream Querystream;
  Querystream << GetFirstPartQueryForTracesInfo(TraceName,CollectionName);
  if ( !iVectIDs.empty() )
    {
    Querystream << "(ImagingSessionID = ";
    Querystream << ImgSessionID;
    Querystream << " and ";
    Querystream << GetSecondPartQueryForTracesInfo(TraceName,iVectIDs);
    Querystream << ");";
    }
  else
    {
    Querystream << "ImagingSessionID = ";
    Querystream << ImgSessionID;
    Querystream << ";";
    }
  
  query->SetQuery( Querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "return info ContoursMesh query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return;
    }

  while ( query->NextRow() )
    {
      {
      ContourMeshStructure temp;
      temp.TraceID = query->DataValue(0).ToInt();
      std::string polydata_string = query->DataValue(2).ToString();
      vtkPolyData *output = vtkPolyData::New();
      if ( !polydata_string.empty() )
        {
        if ( TraceName.compare("contour") == 0 )
          {
          vtkSmartPointer< vtkPolyDataMySQLContourReader > convert_reader =
            vtkSmartPointer< vtkPolyDataMySQLContourReader >::New();
          output->DeepCopy(convert_reader->GetPolyData(polydata_string));
          }
        else
          {
          if ( TraceName.compare("mesh") == 0 )
            {
            vtkIdType N;
            std::stringstream str(polydata_string);
            str >> N;

            if( N > 0 )
              {
              vtkSmartPointer< vtkPolyDataMySQLMeshReader > convert_reader =
                vtkSmartPointer< vtkPolyDataMySQLMeshReader >::New();
              output->DeepCopy(convert_reader->GetPolyData(polydata_string));
              }
            else
              {
              output->Delete();
              output = NULL;
              }
            }
          }
        temp.Nodes = output;
        }
      temp.TCoord       = query->DataValue(3).ToUnsignedInt();
      /// \note For the visualization rgba values are supposed to be double in
      /// between 0 and 1; whereas in the database these values are in between
      /// 0 and 255.
      temp.rgba[0]      = ( query->DataValue(4).ToDouble() ) / 255.;
      temp.rgba[1]      = ( query->DataValue(5).ToDouble() ) / 255.;
      temp.rgba[2]      = ( query->DataValue(6).ToDouble() ) / 255.;
      temp.rgba[3]      = ( query->DataValue(7).ToDouble() ) / 255.;
      ioContainer.push_back(temp);
      }
    }
  query->Delete();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void GetTracesInfoFromDBAndModifyContainer(
  std::list< TrackStructure > & ioContainer,
  vtkMySQLDatabase *DatabaseConnector, std::string TraceName,
  std::string CollectionName, unsigned int ImgSessionID,
  std::vector< int > iVectIDs)
{
  vtkSQLQuery *query = DatabaseConnector->GetQueryInstance();
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
  Querystream << ".Points,color.Red,\
                 color.Green, color.Blue, color.Alpha from ";
  Querystream << TraceName;
  Querystream << " left join color on ";
  Querystream << TraceName;

  Querystream << ".colorID = color.colorID  where ";

  if ( !iVectIDs.empty() )
      {
      Querystream << "(ImagingSessionID = ";
      Querystream << ImgSessionID;
      Querystream << " and ";
      Querystream << GetSecondPartQueryForTracesInfo(TraceName,iVectIDs);
      Querystream << ")";
     }
  else
    {
    Querystream << "ImagingSessionID = ";
    Querystream << ImgSessionID;
    Querystream << ";";
    }

  query->SetQuery( Querystream.str().c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "return info tracks query failed"
      << query->GetLastErrorText() );
    DatabaseConnector->Close();
    DatabaseConnector->Delete();
    query->Delete();
    return;
    }

  while ( query->NextRow() )
    {
      {
      TrackStructure temp;
      temp.TraceID = query->DataValue(0).ToInt();
      vtkSmartPointer< vtkPolyDataMySQLTrackReader > convert_reader =
        vtkSmartPointer< vtkPolyDataMySQLTrackReader >::New();
      std::string polydata_string = query->DataValue(2).ToString();
      vtkIdType N;
      std::stringstream str(polydata_string);
      str >> N;

      if ( (N > 0) && (!polydata_string.empty()) )
        {
        vtkPolyData *output = vtkPolyData::New();
        output->DeepCopy(convert_reader->GetPolyData(polydata_string));
        temp.Nodes = output;
        }
      else
        {
        temp.Nodes = NULL;
        }
      /// \note For the visualization rgba values are supposed to be double in
      /// between 0 and 1; whereas in the database these values are in between
      /// 0 and 255.
      temp.rgba[0]      = ( query->DataValue(3).ToDouble() ) / 255.;
      temp.rgba[1]      = ( query->DataValue(4).ToDouble() ) / 255.;
      temp.rgba[2]      = ( query->DataValue(5).ToDouble() ) / 255.;
      temp.rgba[3]      = ( query->DataValue(6).ToDouble() ) / 255.;
      ioContainer.push_back(temp);
      }
    }
  query->Delete();
}
