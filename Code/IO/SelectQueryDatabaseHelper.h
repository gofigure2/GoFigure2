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
#include "boost/unordered_map.hpp"
#include <list>
#include "itkMacro.h"
#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"
#include "GoDBTraceInfoForVisu.h"
#include "ContourMeshStructure.h"
#include "TrackStructure.h"
#include "LineageStructure.h"
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
  const std::string & ColumnName,
  const std::string & TableName,
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
                          const std::string & ColumnNameOne,
                          const std::string & ColumnNameTwo,
                          const std::string & TableName,
                          const std::string & OrderByColumnName);

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
boost::unordered_map< std::string, std::string > MapTwoColumnsFromTable(
  vtkMySQLDatabase *DatabaseConnector,
  const std::vector<std::string> & iColumnNames,
  const std::string & iTableName,
  std::string iField = "",
  std::string iValue = "");

/**
\brief SELECT * FROM TableName WHERE field = value
*/
//if field is a primary key, will return only the values for
//one row
//not used
QGOIO_EXPORT
std::vector< std::string > ListSpecificValuesForRow(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & TableName,
  const std::string & field,
  const std::string & value);

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
              const std::string & TableName,
              const std::string & ColumnName,
              const std::string & field,
              const std::string & value);

/**
\overload
\param[in] iConditions vector of fields = values
*/
QGOIO_EXPORT
int FindOneID(vtkMySQLDatabase *DatabaseConnector,
              const std::string & TableName,
              const std::string & ColumnName,
              const std::vector<FieldWithValue> & iConditions);

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
  vtkMySQLDatabase * iDatabaseConnector,
  const std::string & TableName,
  const std::string & ColumnName,
  const std::vector<FieldWithValue> & iConditions);

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
  const std::string & TableName,
  const std::string & ColumnName,
  const std::string & field,
  const std::string & value, //bool Distinct = false,
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
  const std::string & TableName,
  const std::string & ColumnName,
  const std::string & field,
  const std::string & value,
  const std::string & ColumnNameOrder);

/**
\brief SELECT ColumnName FROM TableName WHERE (field = value1
or field = value2 AND ColumnName <> 0 (if excludezero))
\overload
*/
QGOIO_EXPORT
std::vector< std::string > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::string & TableName,
  const std::string & ColumnName,
  const std::string & field,
  const std::vector< std::string > & VectorValues,
  bool Distinct = false,
  bool ExcludeZero = false);

/**
\brief SELECT ColumnName FROM TableName WHERE (field = value1
or field = value2 AND ColumnName <> 0 (if excludezero))
\overload
*/
QGOIO_EXPORT
std::list< unsigned int > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::string & TableName,
  const std::string & ColumnName,
  const std::string & field,
  const std::list< unsigned int > & ListValues,
  bool Distinct = false,
  bool ExcludeZero = false);

/**
\brief SELECT ColumnName FROM TableName WHERE (field1 = value1
or field = value2 AND fieldTwo = ValueFieldTwo )
\overload
*/
QGOIO_EXPORT
std::list< unsigned int > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::string & TableName,
  const std::string & ColumnName,
  const std::string & fieldOne,
  const std::list< unsigned int > & ListValuesOne,
  const std::string & fieldTwo,
  const std::string & ValueFieldTwo);


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
ListSpecificValuesForTwoColumns(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & TableName,
  const std::string & ColumnNameOne,
  const std::string & ColumnNameTwo,
  const std::string & field,
  const std::string & value,
  const std::string & ColumnNameOrder);

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
int MaxValueForOneColumnInTable(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & ColumnName,
  const std::string & TableName,
  const std::string & field,
  const std::vector< std::string > & VectorValues);

/**
\brief SELECT MAX(ColumnName) FROM TableName
\overload
*/
QGOIO_EXPORT
int MaxValueForOneColumnInTable(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & ColumnName,
  const std::string & TableName);

/**
\brief SELECT MAX(ColumnName) FROM TableName WHERE field = value
\overload
*/
QGOIO_EXPORT
int MaxValueForOneColumnInTable(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & ColumnName,
  const std::string & TableName,
  const std::string & field,
  const std::string & value);

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
int MinValueForOneColumnInTable(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & ColumnName,
  const std::string & TableName,
  const std::string & field,
  const std::vector< std::string > & VectorValues);

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
std::string ReturnOnlyOneValue(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & TableName,
  const std::string & ColumnName,
  const std::string & field,
  const std::string & value);

QGOIO_EXPORT
std::vector< std::pair< int, std::string > >
ListSpecificValuesForTwoColumnsAndTwoTables(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & TableOne,
  const std::string & ColumnOne,
  const std::string & TableTwo,
  const std::string & ColumnTwo,
  const std::string & ForeignKey,
  const std::string & PrimaryKey,
  const std::string & field,
  const std::string & value);

/**
\brief fill the TCoord and the attributes of the structure obtained from Points
\param[in] ioStructure structure to be modified with TCoord and co.
\param[in] iTCoord one to be filled with
\param[in] iPoints points from which some attributes will be calculated
\param[in] iTraceName name of the trace
*/
QGOIO_EXPORT
void ModifyStructureWithSpecificities(
  ContourMeshStructure & ioStructure,
  unsigned int iTCoord,
  const std::string & iPoints,
  const std::string & iTraceName);

/**
\overload
*/
QGOIO_EXPORT
void ModifyStructureWithSpecificities(
  TrackStructure & ioStructure,
  unsigned int iTCoord,
  const std::string & iPoints,
  const std::string & iTraceName);

/**
\overload
*/
QGOIO_EXPORT
void ModifyStructureWithSpecificities(
  LineageStructure & ioStructure,
  unsigned int iTrackRootID,
  const std::string & iPoints,
  const std::string & iTraceName);
/**
\brief execute iQueryString and put the results in a list of T structure
\param[in] iDatabaseConnector
\param[in] iQueryString query to execute
\param[in,out] ioListStructure list to be filled with the results of the query
\param[in] iTableOne name of the main table (usually a trace name)
\tparam ContourMeshStructure or TrackStructure
*/
template <typename T>
void ExecuteQueryAndModifyListStructure(
  vtkMySQLDatabase* iDatabaseConnector,
  const std::string & iQueryString,
  std::list<T> & ioListStructure,
  const std::string & iTableOne)
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
      unsigned int SpecifiedValue;
      if (iTableOne == "lineage")
        {
        SpecifiedValue = query->DataValue(1).ToUnsignedInt();
        }
      else
        {
        temp.CollectionID = query->DataValue(1).ToUnsignedInt();
        SpecifiedValue = query->DataValue(7).ToUnsignedInt();
        }
       ModifyStructureWithSpecificities(temp, SpecifiedValue,
         query->DataValue(6).ToString(), iTableOne);
      /// \note For the visualization rgba values are supposed to be double in
      /// between 0 and 1; whereas in the database these values are in between
      /// 0 and 255.
      temp.rgba[0]      = ( query->DataValue(2).ToDouble() ) / 255.;
      temp.rgba[1]      = ( query->DataValue(3).ToDouble() ) / 255.;
      temp.rgba[2]      = ( query->DataValue(4).ToDouble() ) / 255.;
      temp.rgba[3]      = ( query->DataValue(5).ToDouble() ) / 255.;

      //ModifyStructureWithTCoordAndPoints(temp, query->DataValue(7).ToUnsignedInt(),
      //    query->DataValue(6).ToString(), iTableOne);
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
  vtkMySQLDatabase *iDatabaseConnector,
  const std::vector<std::string> & iSelectedAttributes,
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const std::string & iTableThree,
  const FieldWithValue & iJoinConditionOne,
  const FieldWithValue & iJoinConditionTwo,
  const std::string & iFieldOne,
  unsigned int iValueFieldOne,
  const std::string & iIDFieldName,
  const std::list< unsigned int > & iListIDs)
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
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & MainTable,
  const std::vector< std::string > & SelectFields,
  const std::string & field,
  const std::string & value,
  const std::vector< std::string > & JoinTablesOnTraceTable,
  bool Distinct);

//return a pair with the number of fields in the query and a vector of the
// results,
//the query includes the where conditions from the vector as AND conditions:
QGOIO_EXPORT
std::vector< std::vector< std::string > > GetValuesFromSeveralTables(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & MainTable,
  const std::vector< std::string > & SelectFields,
  const std::vector< std::string > & WhereAndConditions,
  const std::vector< std::string > & JoinTablesOnTraceTable,
  bool Distinct);

std::vector< std::vector< std::string > > GetValuesFromSeveralTables(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & MainTable,
  const std::vector< std::string > & SelectFields,
  const std::string & field,
  const std::string & value,
  const std::vector< std::string > & JoinTablesOnTraceTable,
  bool Distinct,
  const std::vector<FieldWithValue> & iWhereOrConditions);

//query: SELECT where condition1 UNION SELECT where condition1 and condition2
QGOIO_EXPORT
std::vector< std::string >
GetSamefieldFromTwoTables(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const std::string & iColumn,
  const std::string & iField,
  const std::string & iValue,
  const std::string & iFieldTwo,
  const std::vector< std::string > & iListConditionsTwo);

//query: SELECT iColumnOne FROM TableOne WHERE...UNION SELECT iColumnTwo FROM
// TableOne...
//UNION SELECT iColumnOne FROM TableTwo WHERE....UNION SELECT iColumnTwo FROM
// TableTwo WHERE.
QGOIO_EXPORT
std::vector< std::string >
GetSamefieldsFromTwoTables(vtkMySQLDatabase *DatabaseConnector,
                           const std::string & iTableOne,
                           const std::string & iTableTwo,
                           const std::string & iColumnOne,
                           const std::string & iColumnTwo,
                           const std::string & iField,
                           const std::string & iValue);

//query: select distinct iColumnOne FROM TableOne where ifield = listconditions union select
//distinct icolumntwo from tableOne left join tabletwo on iOnCondition where ifield = listconditions
QGOIO_EXPORT
std::list< unsigned int > GetTwoFieldsFromTwoTables(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const FieldWithValue & iOnCondition,
  const std::string & iColumnOne,
  const std::string & iColumnTwo,
  const std::string & iField,
  const std::vector< std::string > & iListValues,
  bool Distinct);

//query: SELECT iColumnOne FROM TableOne WHERE...UNION SELECT iColumnTwo FROM
// TableOne...
//UNION SELECT iColumnOne FROM TableTwo WHERE listconditions2....
//UNION SELECT iColumnTwo FROM TableTwo WHERE listconditions2.
QGOIO_EXPORT
std::vector< std::string >
GetSamefieldsFromTwoTables(vtkMySQLDatabase *DatabaseConnector,
                           const std::string & iTableOne,
                           const std::string & iTableTwo,
                           const std::string & iColumnOne,
                           const std::string & iColumnTwo,
                           const std::string & iField,
                           const std::string & iValue,
                           const std::string & iFieldTwo,
                           const std::vector< std::string > & iListConditionsTwo);

QGOIO_EXPORT
std::vector< std::string >
GetSameFieldsFromSeveralTables(vtkMySQLDatabase *DatabaseConnector,
                               const std::vector< std::string > & iColumnNames,
                               const std::vector< std::string > & iVectorTablesNames,
                               const std::vector< std::string > & iVectorConditionFieldNames,
                               const std::vector< std::vector< std::string > > & iVectorConditionsValues);

//query: SELECT iColumnName FROM TableName WHERE ( (iFieldOne =
// iVectorConditionFieldOne(i)
// OR iFieldOne = iVectorConditionFieldOne(i+1...) AND (iFieldTwo =
// iVectorConditionFieldTwo(j) OR
//iVectorConditionFieldTwo(j+1)... ) );
QGOIO_EXPORT
std::vector< std::string >
GetSpecificValueFromOneTableWithConditionsOnTwoColumns(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & iColumnName,
  const std::string & iTableName,
  const std::string & iFieldOne,
  const std::vector< std::string > & iVectorConditionFieldOne,
  const std::string & iFieldTwo,
  const std::vector< std::string > &iVectorConditionFieldTwo);

//Select t1 from (select mesh.meshid,coordinate.zcoord from mesh left join
// coordinate
//on mesh.coordidmax = coordinate.coordid where( imagingsessionid = 2 and
//coordinate.zcoord > 20) ) AS t1 INNER JOIN
//(select mesh.meshid,coordinate.zcoord from mesh left join
//coordinate on mesh.coordidmin = coordinate.coordid
//where ( imagingsessionid = 2 and coordinate.zcoord < 20) )
//AS t2 on t1.meshid = t2.meshid;
QGOIO_EXPORT
std::list< unsigned int >
GetColumnForBoundedValue(const std::string & iColumnName,
                         const std::string & TableName,
                         const std::string & iImgSessionID,
                         const std::string & iCoordType,
                         const std::string & iValue,
                         vtkMySQLDatabase *DatabaseConnector);

QGOIO_EXPORT
std::list< unsigned int >
GetSpecificValuesEqualToZero(
  vtkMySQLDatabase *iDatabaseConnection,
  const std::string & iColumnName,
  const std::string & iTableName,
  const std::vector< std::string > & iVectorConditionFieldOne,
  const std::string & iFieldTwo);

template <class TResultsQuery>
inline
TResultsQuery ExecuteSelectQuery(vtkMySQLDatabase *iDatabaseConnector,
                                 const std::string & iQuery)
{
//  std::cout << "execute query:" << iQuery << std::endl;

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
  int NumberOfFields = query->GetNumberOfFields();
  while ( query->NextRow() )
    {
    for ( int k = 0; k < NumberOfFields; k++ )
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
                             const std::string & iQuery)
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
std::string WhereAndOrConditions(
  const std::vector<std::string> & iWhereAndConditions,
  bool iAnd = true);

QGOIO_EXPORT
std::list<unsigned int> GetAllSelectedValuesFromTwoTables(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const std::string & iColumn,
  const FieldWithValue & iJoinCondition,
  const std::vector<FieldWithValue> & iFieldsWithValues,
  bool Distinct = false);

QGOIO_EXPORT
std::vector<std::string> GetAllSelectedValuesFromTwoTables(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const std::vector<std::string> & iListAttributes,
  const FieldWithValue & iJoinCondition,
  const std::vector<FieldWithValue> & iFieldsWithValues,
  std::string iConditionConnector = "AND",
  std::string ColumnNameOrder = "");

QGOIO_EXPORT
std::list< unsigned int > GetAllSelectedValuesFromTwoTables(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const std::string & iColumn,
  const FieldWithValue & iJoinCondition,
  const std::string & iField,
  const std::vector<std::string> & iVectorValues,
  bool Distinct = false ,
  bool NonNULLRows = false);

QGOIO_EXPORT
std::list< unsigned int > GetAllSelectedValuesFromTwoTables(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const std::string & iColumn,
  const FieldWithValue & iJoinCondition,
  const std::string & iField,
  const std::vector<std::string> & iVectorValues,
  const FieldWithValue & iAndCondition);

QGOIO_EXPORT
std::list<unsigned int> GetAllSelectedValuesFromTwoTables(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const std::vector<std::string> & iSelectedFields,
  const FieldWithValue & iJoinCondition,
  const std::string & iField,
  const std::string & iValue,
  bool NonNULLRows);

QGOIO_EXPORT
std::list< unsigned int > GetDoublonValuesFromTwoTables(
  vtkMySQLDatabase* iDatabaseConnector,
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const std::string & iColumn,
  const FieldWithValue & iJoinCondition,
  const std::string & iField,
  const std::vector<std::string> & iVectValues);//, std::string GroupByColumn = "");

QGOIO_EXPORT
int GetMaxValueFromTwoTables(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const std::string & iColumn,
  const FieldWithValue & iJoinCondition,
  const std::string & iField,
  const std::vector<std::string> & iVectorValues,
  const FieldWithValue & iAndCondition);

//add the selected fields separated by ',' to the ioQueryStream
//void GetAllSelectedFields(std::stringstream & ioQueryStream,
//                          std::vector< std::string > iSelectedFields);

//select columnname FROM tablename WHERE field = value order by ASC/DESC limit
// iNumberLimit
QGOIO_EXPORT
std::vector< std::string > GetOrderByWithLimit(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::string & iColumnName,
  const std::string & iTableName,
  const std::string & iField,
  const std::string & iValue,
  bool ASC,
  const std::string & iNumberLimit);

QGOIO_EXPORT
std::string GetCoordinateValuesQueryString(
  const std::string & iTableName,
  const std::string & iField,
  const std::string & iValue,
  bool iMin);

//get the center of the bounding boxes for tableName with restriction of iField = iValue
QGOIO_EXPORT
std::list< double* > GetCenterBoundingBoxes(
  vtkMySQLDatabase *DatabaseConnector,
  const std::string & iTableName,
  const std::string & iField,
  const std::string & iValue);

QGOIO_EXPORT
std::list<unsigned int> GetListValuesFromTwoTablesAndCondition(
  vtkMySQLDatabase *iDatabaseConnector,
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const std::string & iColumn,
  const FieldWithValue & iJoinCondition,
  const std::string & iField,
  const std::vector<std::string> & iVectorValues,
  const FieldWithValue & iAndCondition);

QGOIO_EXPORT
int NumberOfElementForGivenImagingSessionAndTrace(
  vtkMySQLDatabase *DatabaseConnector,
  unsigned int iImagingSession,
  const std::string & iTrace);

QGOIO_EXPORT
int NumberOfElementForGivenImagingSessionAndTraceForGivenTimePoint(
  vtkMySQLDatabase *DatabaseConnector,
  unsigned int iImagingSession,
  const std::string & iTrace,
  int iTimePoint);

/*
QGOIO_EXPORT
int NumberOfTimePointsForGivenImagingSession(
        vtkMySQLDatabase *DatabaseConnector,
        unsigned int iImagingSession);
        */

#endif
