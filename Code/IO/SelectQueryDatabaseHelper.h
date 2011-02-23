/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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
#ifndef __SelectQueryDatabaseHelper_h
#define __SelectQueryDatabaseHelper_h

#include <vector>
#include <string>
#include <map>
#include <list>
#include "itkMacro.h"
#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "GoDBTraceInfoForVisu.h"
#include "ContourMeshStructure.h"
#include "TrackStructure.h"
#include "QueryBuilderHelper.h"
#include "ConvertToStringHelper.h"

#include "QGoIOConfigure.h"

/**
\brief SELECT ColumnName from TableName ORDER BY OrderbyColumnName
\param[in] DatabaseConnector connection to the database
\param[in] ColumnName name of the field in the database
\param[in] TableName name of the database table
\param{in] OrderByColumnName sorting
\return all the values for the column sorted or not
*/
QGOIO_EXPORT
std::vector< std::string > ListAllValuesForOneColumn(
  vtkMySQLDatabase *DatabaseConnector,
  std::string ColumnName, std::string TableName,
  std::string OrderByColumnName = "");

/**
\brief SELECT ColumnNameOne,ColumnNameTwo FROM TableName ORDER BY
ColumnName ASC
\param[in] DatabaseConnector connection to the database
\param[in] ColumnNameOne first value of the pair
\param[in] ColumnNameTwo second value of the pair
\param[in] TableName name of the database table
\param{in] OrderByColumnName sorting
\return all the values sorted by OrderByColumnName in a vector of pair 
*/
QGOIO_EXPORT
std::vector< std::pair< std::string, std::string > >
VectorTwoColumnsFromTable(vtkMySQLDatabase *DatabaseConnector,
                          std::string ColumnNameOne, std::string ColumnNameTwo,
                          std::string TableName, std::string OrderByColumnName);

/**
\brief query: "SELECT ColumnName1, ColumnName2 FROM TableName"
\param[in] DatabaseConnector connection to the database
\param[in] iColumnNames vector[0] = ColumnName1, vector[1] = ColumnName2
\param[in] iTableName name of the database table
\param[in] iField field for the condition if there is one
\param[in] iValue value of the condition if there is one
\return map[Value from ColumnName1] = Value from ColumnName2
*/
QGOIO_EXPORT
std::map< std::string, std::string > MapTwoColumnsFromTable(
  vtkMySQLDatabase *DatabaseConnector,std::vector<std::string> iColumnNames,
  std::string iTableName, std::string iField = "", std::string iValue = "");

/**
\brief SELECT * FROM TableName WHERE field = value
*/
//if field is a primary key, will return only the values for
//one row
//not used
QGOIO_EXPORT
std::vector< std::string > ListSpecificValuesForRow(
  vtkMySQLDatabase *DatabaseConnector,
  std::string TableName, std::string field, std::string value);

/**
\brief SELECT ColumnName FROM TableName WHERE field = value
\param[in] DatabaseConnector connection to the database
\param[in] TableName name of the database table
\param[in] ColumnName name of the field in the database
\param[in] field field for the condition
\param[in] value value of the condition
\return only one ID 
*/
QGOIO_EXPORT
int FindOneID(vtkMySQLDatabase *DatabaseConnector,
              std::string TableName, std::string ColumnName,
              std::string field, std::string value);

/**
\overload 
\param[in] iConditions vector of fields = values
*/
QGOIO_EXPORT
int FindOneID(vtkMySQLDatabase *DatabaseConnector,
              std::string TableName, std::string ColumnName,
              std::vector<FieldWithValue> iConditions);

/**
\brief "SELECT ColumnName FROM TableName WHERE (field1 = value1
AND field2 = value2...);
\param[in] DatabaseConnector connection to the database
\param[in] TableName name of the database table
\param[in] ColumnName name of the field in the database
\param[in] iConditions vector of fields = values
\return all the values in ColumnName that fit the conditions
*/
QGOIO_EXPORT
std::vector< std::string > FindSeveralIDs(
  vtkMySQLDatabase * iDatabaseConnector,std::string TableName, 
  std::string ColumnName, std::vector<FieldWithValue> iConditions);

/**
\brief SELECT ColumnName FROM TableName WHERE field = value and 
ColumnName <> 0 (if excludezero)
\param[in] iDatabaseConnector connection to the database
\param[in] TableName name of the database table
\param[in] ColumnName name of the field in the database
\param[in] field field for the condition
\param[in] value value of the condition
\param[in] distinct set to true if doublon are not allowed
\param[in] ExcludeZero set to true if ColumnName has to be 
different than 0
\return all the values in ColumnName that fit the conditions
*/
QGOIO_EXPORT
std::vector< std::string > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *iDatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::string value, //bool Distinct = false,
  bool ExcludeZero = false);

/**
\brief SELECT ColumnName FROM TableName WHERE field = value
ORDER BY ColumnNameOrder ASC
\overload
\param[in] ColumnNameOrder name of the column for sorting
*/
QGOIO_EXPORT
std::vector< std::string > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *iDatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::string value, std::string ColumnNameOrder);

/**
\brief SELECT ColumnName FROM TableName WHERE (field = value1
or field = value2 AND ColumnName <> 0 (if excludezero))
\overload
*/
QGOIO_EXPORT
std::vector< std::string > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *iDatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::vector< std::string > VectorValues,
  bool Distinct = false, bool ExcludeZero = false);

/**
\brief SELECT ColumnName FROM TableName WHERE (field = value1
or field = value2 AND ColumnName <> 0 (if excludezero))
\overload
*/
QGOIO_EXPORT
std::list< unsigned int > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *iDatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::list< unsigned int > ListValues,
  bool Distinct = false, bool ExcludeZero = false);

/**
\brief SELECT ColumnName FROM TableName WHERE (field1 = value1
or field = value2 AND fieldTwo = ValueFieldTwo )
\overload
*/
QGOIO_EXPORT
std::list< unsigned int > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *iDatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string fieldOne, std::list< unsigned int > ListValuesOne,
  std::string fieldTwo, std::string ValueFieldTwo);


/**
\brief SELECT ColumnNameOne,ColumnName2 FROM TableName
WHERE field = value ORDER BY ColumnNameOrder ASC"
\param[in] iDatabaseConnector connection to the database
\param[in] TableName name of the database table
\param[in] ColumnNameOne name of the first field in the database
\param[in] ColumnNameTwo name of the second field in the database
\param[in] field field for the condition
\param[in] value value of the condition
\param{in] OrderByColumnName sorting
\return a vector of pair with the values of each Column
*/
QGOIO_EXPORT
std::vector< std::pair< std::string, std::string > >
ListSpecificValuesForTwoColumns(vtkMySQLDatabase *DatabaseConnector,
                                std::string TableName, std::string ColumnNameOne, std::string ColumnNameTwo,
                                std::string field, std::string value, std::string ColumnNameOrder);

/**
\brief SELECT MAX(ColumnName) FROM TableName WHERE (field =
value1 or field = value2...."
\param[in] DatabaseConnector connection to the database
\param[in] TableName name of the database table
\param[in] ColumnName name of the field in the database
\param[in] field field for the condition
\param[in] VectorValues values of the condition
\return the max value
*/
QGOIO_EXPORT
int MaxValueForOneColumnInTable(vtkMySQLDatabase *DatabaseConnector,
                                std::string ColumnName, std::string TableName, std::string field,
                                std::vector< std::string > VectorValues);

/**
\brief SELECT MAX(ColumnName) FROM TableName
\overload
*/
QGOIO_EXPORT
int MaxValueForOneColumnInTable(
  vtkMySQLDatabase *DatabaseConnector,
  std::string ColumnName, std::string TableName);

/**
\brief SELECT MAX(ColumnName) FROM TableName WHERE field = value
\overload
*/
QGOIO_EXPORT
int MaxValueForOneColumnInTable(vtkMySQLDatabase *DatabaseConnector,
                                std::string ColumnName, std::string TableName, std::string field,
                                std::string value);

/**
\brief SELECT MIN(ColumnName) FROM TableName WHERE (field =
value1 or field = value2...."
\param[in] DatabaseConnector connection to the database
\param[in] TableName name of the database table
\param[in] ColumnName name of the field in the database
\param[in] field field for the condition
\param[in] VectorValues values of the condition
\return the minimum value
*/
QGOIO_EXPORT
int MinValueForOneColumnInTable(vtkMySQLDatabase *DatabaseConnector,
                                std::string ColumnName, std::string TableName, std::string field,
                                std::vector< std::string > VectorValues);

/**
\brief SELECT ColunmName FROM TableName WHERE field=value limit 1
\param[in] iDatabaseConnector connection to the database
\param[in] TableName name of the database table
\param[in] ColumnName name of the field in the database
\param[in] field field for the condition
\param[in] value value of the condition
\return only one value if several are selected
*/
QGOIO_EXPORT
std::string ReturnOnlyOneValue(vtkMySQLDatabase *DatabaseConnector,
                               std::string TableName, std::string ColumnName, std::string field,
                               std::string value);

QGOIO_EXPORT
std::vector< std::pair< int, std::string > > ListSpecificValuesForTwoColumnsAndTwoTables(
  vtkMySQLDatabase *DatabaseConnector, std::string TableOne, std::string ColumnOne,
  std::string TableTwo, std::string ColumnTwo, std::string ForeignKey,
  std::string PrimaryKey, std::string field, std::string value);

/**
\brief fill the TCoord and the attributes of the structure obtained from Points
\param[in] ioStructure structure to be modified with TCoord and co.
\param[in] iTCoord one to be filled with
\param[in] iPoints points from which some attributes will be calculated
\param[in] iTraceName name of the trace
*/
QGOIO_EXPORT
void ModifyStructureWithTCoordAndPoints(ContourMeshStructure & ioStructure,
  unsigned int iTCoord, std::string iPoints, std::string iTraceName);

/**
\overload
*/
QGOIO_EXPORT
void ModifyStructureWithTCoordAndPoints(TrackStructure & ioStructure,
  unsigned int iTCoord, std::string iPoints, std::string iTraceName);
/**
\brief execute iQueryString and put the results in a list of T structure
\param[in] iDatabaseConnector
\param[in] iQueryString query to execute
\param[in,out] ioListStructure list to be filled with the results of the query
\param[in] iTableOne name of the main table (usually a trace name)
\tparam ContourMeshStructure or TrackStructure
*/
template <typename T>
void ExecuteQueryAndModifyListStructure(vtkMySQLDatabase* iDatabaseConnector,
  std::string iQueryString, std::list<T> & ioListStructure, std::string iTableOne)
{
  vtkSQLQuery *query = iDatabaseConnector->GetQueryInstance();
  query->SetQuery( iQueryString.c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "get info traces query failed"
      << query->GetLastErrorText() );
    iDatabaseConnector->Close();
    iDatabaseConnector->Delete();
    query->Delete();
    return;
    }
  while ( query->NextRow() )
    {
      {
      T temp;
      temp.TraceID = query->DataValue(0).ToUnsignedInt();
      temp.CollectionID = query->DataValue(1).ToUnsignedInt();     
      /// \note For the visualization rgba values are supposed to be double in
      /// between 0 and 1; whereas in the database these values are in between
      /// 0 and 255.
      temp.rgba[0]      = ( query->DataValue(2).ToDouble() ) / 255.;
      temp.rgba[1]      = ( query->DataValue(3).ToDouble() ) / 255.;
      temp.rgba[2]      = ( query->DataValue(4).ToDouble() ) / 255.;
      temp.rgba[3]      = ( query->DataValue(5).ToDouble() ) / 255.;
   
      ModifyStructureWithTCoordAndPoints(temp, query->DataValue(7).ToUnsignedInt(),  
          query->DataValue(6).ToString(), iTableOne);
      ioListStructure.push_back(temp);
      }
    }
  query->Delete();
}

/**
\brief select iselectedattributes from (tableone left join tabletwo ijoinconditionone)
left join tablethree ijoinconditiontwo where (ifieldone = ivaluefieldone and (iIDfieldname
= ivectids1 or ivectids2...) );
\param[in] iDatabaseConnector connection to the database
\param[in,out] ioListStructure list of Structure to be filled
\param[in] iSelectedAttributes vector of all the attributes to be fetched from the db
\param[in] iTableOne main table involved (usually the table for the trace)
\param[in] iTableTwo table attached to the main table
\param[in] iTableThree table attached to the main table
\param[in] iJoinConditionOne describes how the tabletwo is attached to the main table
\param[in] iJoinConditionTwo describes how the tablethree is attached to the main table
\param[in] iFieldOne first condition
\param[in] iValueFieldOne value for the first condition
\param[in] iIDFieldName field for the IDName where there is a condition
\param[in] iListIDs values for the iIDFieldname
*/
template<typename T>
void GetInfoFromDBAndModifyListStructure(
  std::list< T > & ioListStructure,
  vtkMySQLDatabase *iDatabaseConnector, std::vector<std::string> iSelectedAttributes,
  std::string iTableOne, std::string iTableTwo, std::string iTableThree,
  FieldWithValue iJoinConditionOne, FieldWithValue iJoinConditionTwo, std::string iFieldOne,
  unsigned int iValueFieldOne, std::string iIDFieldName, std::list< unsigned int > iListIDs)
{
  std::string QueryString = SelectForTracesInfo(iSelectedAttributes, iTableOne, iTableTwo, 
    iTableThree, iJoinConditionOne, iJoinConditionTwo, iFieldOne, iValueFieldOne, iIDFieldName, 
    iListIDs);
  ExecuteQueryAndModifyListStructure<T>( 
    iDatabaseConnector, QueryString, ioListStructure, iTableOne);
}

//return a pair with the number of fields in the query and a vector of the
// results:
QGOIO_EXPORT
std::vector< std::vector< std::string > > GetValuesFromSeveralTables(
  vtkMySQLDatabase *DatabaseConnector, std::string MainTable,
  std::vector< std::string > SelectFields, std::string field,
  std::string value, std::vector< std::string > JoinTablesOnTraceTable, bool Distinct);

//return a pair with the number of fields in the query and a vector of the
// results,
//the query includes the where conditions from the vector as AND conditions:
QGOIO_EXPORT
std::vector< std::vector< std::string > > GetValuesFromSeveralTables(
  vtkMySQLDatabase *DatabaseConnector, std::string MainTable,
  std::vector< std::string > SelectFields, std::vector< std::string > WhereAndConditions,
  std::vector< std::string > JoinTablesOnTraceTable, bool Distinct);

//query: SELECT where condition1 UNION SELECT where condition1 and condition2
QGOIO_EXPORT
std::vector< std::string > GetSamefieldFromTwoTables(vtkMySQLDatabase *DatabaseConnector,
                                                     std::string iTableOne, std::string iTableTwo, std::string iColumn,
                                                     std::string iField, std::string iValue, std::string iFieldTwo,
                                                     std::vector< std::string > iListConditionsTwo);

//query: SELECT iColumnOne FROM TableOne WHERE...UNION SELECT iColumnTwo FROM
// TableOne...
//UNION SELECT iColumnOne FROM TableTwo WHERE....UNION SELECT iColumnTwo FROM
// TableTwo WHERE.
QGOIO_EXPORT
std::vector< std::string > GetSamefieldsFromTwoTables(vtkMySQLDatabase *DatabaseConnector,
                                                      std::string iTableOne,
                                                      std::string iTableTwo,
                                                      std::string iColumnOne,
                                                      std::string iColumnTwo,
                                                      std::string iField,
                                                      std::string iValue);

//query: SELECT iColumnOne FROM TableOne WHERE...UNION SELECT iColumnTwo FROM
// TableOne...
//UNION SELECT iColumnOne FROM TableTwo WHERE listconditions2....
//UNION SELECT iColumnTwo FROM TableTwo WHERE listconditions2.
QGOIO_EXPORT
std::vector< std::string > GetSamefieldsFromTwoTables(vtkMySQLDatabase *DatabaseConnector,
                                                      std::string iTableOne,
                                                      std::string iTableTwo,
                                                      std::string iColumnOne,
                                                      std::string iColumnTwo,
                                                      std::string iField,
                                                      std::string iValue,
                                                      std::string iFieldTwo,
                                                      std::vector< std::string > iListConditionsTwo);

QGOIO_EXPORT
std::vector< std::string > GetSameFieldsFromSeveralTables(vtkMySQLDatabase *DatabaseConnector,
                                                          std::vector< std::string > iColumnNames,
                                                          std::vector< std::string > iVectorTablesNames,
                                                          std::vector< std::string > iVectorConditionFieldNames,
                                                          std::vector< std::vector< std::string > >
                                                          iVectorConditionsValues);

//query: SELECT iColumnName FROM TableName WHERE ( (iFieldOne =
// iVectorConditionFieldOne(i)
// OR iFieldOne = iVectorConditionFieldOne(i+1...) AND (iFieldTwo =
// iVectorConditionFieldTwo(j) OR
//iVectorConditionFieldTwo(j+1)... ) );
QGOIO_EXPORT
std::vector< std::string > GetSpecificValueFromOneTableWithConditionsOnTwoColumns(
  vtkMySQLDatabase *DatabaseConnector, std::string iColumnName, std::string iTableName,
  std::string iFieldOne, std::vector< std::string > iVectorConditionFieldOne,
  std::string iFieldTwo, std::vector< std::string > iVectorConditionFieldTwo);

//Select t1 from (select mesh.meshid,coordinate.zcoord from mesh left join
// coordinate
//on mesh.coordidmax = coordinate.coordid where( imagingsessionid = 2 and
//coordinate.zcoord > 20) ) AS t1 INNER JOIN
//(select mesh.meshid,coordinate.zcoord from mesh left join
//coordinate on mesh.coordidmin = coordinate.coordid
//where ( imagingsessionid = 2 and coordinate.zcoord < 20) )
//AS t2 on t1.meshid = t2.meshid;
QGOIO_EXPORT
std::list< unsigned int > GetColumnForBoundedValue(std::string iColumnName,
                                                   std::string iTableName,
                                                   std::string iImgSessionID,
                                                   std::string iCoordType,
                                                   std::string iValue,
                                                   vtkMySQLDatabase *DatabaseConnector);

QGOIO_EXPORT
std::list< unsigned int > GetSpecificValuesEqualToZero(
  vtkMySQLDatabase *iDatabaseConnection, std::string iColumnName, std::string iTableName,
  std::vector< std::string > iVectorConditionFieldOne,
  std::string iFieldTwo);

template <class TResultsQuery>
TResultsQuery ExecuteSelectQuery(vtkMySQLDatabase *iDatabaseConnector,
                                              std::string iQuery)
{
  vtkSQLQuery *query = iDatabaseConnector->GetQueryInstance();

  TResultsQuery oResults;
  typedef typename TResultsQuery::value_type ValueType;
  query->SetQuery( iQuery.c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Execute select query failed"
      << query->GetLastErrorText() );
    iDatabaseConnector->Close();
    iDatabaseConnector->Delete();
    query->Delete();
    return oResults;
    }
  while ( query->NextRow() )
    {
    for ( int k = 0; k < query->GetNumberOfFields(); k++ )
      {
      ValueType temp = ss_atoi<ValueType>(query->DataValue(k).ToString());
      oResults.push_back( temp );
      }
    }

  query->Delete();
  return oResults;
}

template <class T>
T ExecuteSelectQueryOneValue(vtkMySQLDatabase *iDatabaseConnector,
                                              std::string iQuery)
{
  vtkSQLQuery *query = iDatabaseConnector->GetQueryInstance();
  T oResults = ss_atoi<T>("-1");
  query->SetQuery( iQuery.c_str() );
  if ( !query->Execute() )
    {
    itkGenericExceptionMacro(
      << "Execute select query failed"
      << query->GetLastErrorText() );
    iDatabaseConnector->Close();
    iDatabaseConnector->Delete();
    query->Delete();
    return oResults;
    }

  if ( query->NextRow() )
    {
    oResults = ss_atoi<T>(query->DataValue(0).ToString() );
    }

  query->Delete();
  return oResults;
}

// WHERE (iWhereAndConditions[i] = iWhereAndConditions[i+1] and/or ....)
//if only 1 condition: WHERE iWhereAndConditions[i] = iWhereAndConditions[i+1]
QGOIO_EXPORT
std::string WhereAndOrConditions(std::vector<std::string> iWhereAndConditions,
  bool iAnd = true);

QGOIO_EXPORT
std::list<unsigned int> GetAllSelectedValuesFromTwoTables(
  vtkMySQLDatabase *iDatabaseConnector, std::string iTableOne, std::string iTableTwo,
  std::string iColumn, FieldWithValue iJoinCondition,
  std::vector<FieldWithValue> iFieldsWithValues, bool Distinct = false);

QGOIO_EXPORT
std::vector<std::string> GetAllSelectedValuesFromTwoTables(
  vtkMySQLDatabase *iDatabaseConnector, std::string iTableOne, std::string iTableTwo,
  std::vector<std::string> iListAttributes, FieldWithValue iJoinCondition,
  std::vector<FieldWithValue> iFieldsWithValues, std::string ColumnNameOrder = "");

QGOIO_EXPORT
std::list< unsigned int > GetAllSelectedValuesFromTwoTables(vtkMySQLDatabase *iDatabaseConnector,
  std::string iTableOne, std::string iTableTwo,
  std::string iColumn, FieldWithValue iJoinCondition,
  std::string iField, std::vector<std::string> iVectorValues, bool Distinct = false);

QGOIO_EXPORT
std::list< unsigned int > GetAllSelectedValuesFromTwoTables(vtkMySQLDatabase *iDatabaseConnector,
  std::string iTableOne, std::string iTableTwo,
  std::string iColumn, FieldWithValue iJoinCondition,
  std::string iField, std::vector<std::string> iVectorValues,FieldWithValue iAndCondition);

QGOIO_EXPORT
std::list< unsigned int > GetDoublonValuesFromTwoTables(
      vtkMySQLDatabase* iDatabaseConnector, std::string iTableOne, std::string iTableTwo,
      std::string iColumn, FieldWithValue iJoinCondition,std::string iField,
      std::vector<std::string> iVectValues);//, std::string GroupByColumn = "");

QGOIO_EXPORT
int GetMaxValueFromTwoTables(vtkMySQLDatabase *iDatabaseConnector,
  std::string iTableOne, std::string iTableTwo,std::string iColumn,
  FieldWithValue iJoinCondition,std::string iField,
  std::vector<std::string> iVectorValues,FieldWithValue iAndCondition);

//add the selected fields separated by ',' to the ioQueryStream
//void GetAllSelectedFields(std::stringstream & ioQueryStream,
//                          std::vector< std::string > iSelectedFields);

//select columnname FROM tablename WHERE field = value order by ASC/DESC limit
// iNumberLimit
QGOIO_EXPORT
std::vector< std::string > GetOrderByWithLimit(vtkMySQLDatabase *iDatabaseConnector,
                                               std::string iColumnName,
                                               std::string iTableName,
                                               std::string iField,
                                               std::string iValue,
                                               bool ASC,
                                               std::string iNumberLimit);

QGOIO_EXPORT
std::string GetCoordinateValuesQueryString(std::string iTableName, std::string iField,
  std::string iValue,bool iMin);

//get the center of the bounding boxes for tableName with restriction of iField = iValue
QGOIO_EXPORT
std::list< double* > GetCenterBoundingBoxes(vtkMySQLDatabase *DatabaseConnector,
  std::string iTableName,std::string iField,std::string iValue);

QGOIO_EXPORT
std::list<unsigned int> GetListValuesFromTwoTablesAndCondition(
  vtkMySQLDatabase *iDatabaseConnector,
  std::string iTableOne, std::string iTableTwo,std::string iColumn,
  FieldWithValue iJoinCondition,std::string iField,
  std::vector<std::string> iVectorValues, FieldWithValue iAndCondition);

#endif
