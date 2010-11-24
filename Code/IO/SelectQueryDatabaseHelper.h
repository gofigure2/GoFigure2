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
#ifndef __SelectQueryDatabaseHelper_h
#define __SelectQueryDatabaseHelper_h

#include <vector>
#include <string>
#include <map>
#include <list>
#include "itkMacro.h"
#include "vtkMySQLDatabase.h"
#include "GoDBTraceInfoForVisu.h"
#include "ContourMeshStructure.h"
#include "TrackStructure.h"

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

//query: "SELECT * FROM TableName WHERE field = value"
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

//query: "SELECT ColumnName FROM TableName WHERE (field1 = value1
//AND field2 = value2 AND field3 = value3);
QGOIO_EXPORT
int FindOneID(vtkMySQLDatabase *DatabaseConnector,
              std::string TableName, std::string ColumnName,
              std::string ColumnNameOne, std::string valueOne,
              std::string ColumnNameTwo, std::string valueTwo,
              std::string ColumnNameThree, std::string valueThree);

//query: "SELECT ColumnName FROM TableName WHERE (field1 = value1
//AND field2 = value2);
QGOIO_EXPORT
int FindOneID(vtkMySQLDatabase *DatabaseConnector,
              std::string TableName, std::string ColumnName,
              std::string ColumnNameOne, std::string valueOne,
              std::string ColumnNameTwo, std::string valueTwo);

//query: "SELECT ColumnName FROM TableName WHERE (field1 = value1
//AND field2 = value2);
std::vector< std::string > FindSeveralIDs(vtkMySQLDatabase *DatabaseConnector,
                                          std::string TableName, std::string ColumnName,
                                          std::string ColumnNameOne, std::string valueOne,
                                          std::string ColumnNameTwo, std::string valueTwo);

//query: "SELECT ColumnName FROM TableName WHERE (field1 = value1
//AND field2 = value2 AND field3 = value3 AND field4 = value4);
QGOIO_EXPORT
int FindOneID(vtkMySQLDatabase *DatabaseConnector,
              std::string TableName, std::string ColumnName,
              std::string ColumnNameOne, std::string valueOne,
              std::string ColumnNameTwo, std::string valueTwo,
              std::string ColumnNameThree, std::string valueThree,
              std::string ColumnNameFour, std::string valueFour,
              std::string ColumnNameFive, std::string valueFive);

//query: "SELECT ColumnName FROM TableName WHERE (field1 = value1
//AND field2 = value2 AND field3 = value3 AND field4 = value4);
QGOIO_EXPORT
int FindOneID(vtkMySQLDatabase *DatabaseConnector,
              std::string TableName, std::string ColumnName,
              std::string ColumnNameOne, std::string valueOne,
              std::string ColumnNameTwo, std::string valueTwo,
              std::string ColumnNameThree, std::string valueThree,
              std::string ColumnNameFour, std::string valueFour,
              std::string ColumnNameFive, std::string valueFive,
              std::string ColumnNameSix, std::string valueSix,
              std::string ColumnNameSeven, std::string valueSeven,
              std::string ColumnNameEight, std::string valueEight,
              std::string ColumnNameNine, std::string valueNine,
              std::string ColumnNameTen, std::string valueTen);

//query: "SELECT ColumnName FROM TableName WHERE field = value"
//query: "SELECT ColumnName FROM TableName" if field == ""
QGOIO_EXPORT
std::vector< std::string > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::string value, bool Distinct = false,
  bool ExcludeZero = false);

//query: "SELECT ColumnName FROM TableName WHERE field = value
//ORDER BY ColumnNameOrder ASC"
QGOIO_EXPORT
std::vector< std::string > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::string value, std::string ColumnNameOrder);

//query: "SELECT ColumnName FROM TableName WHERE (field = value1
//or field = value2....)"
QGOIO_EXPORT
std::vector< std::string > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::vector< std::string > VectorValues,
  bool Distinct = false, bool ExcludeZero = false);

QGOIO_EXPORT
std::list< unsigned int > ListSpecificValuesForOneColumn(
  vtkMySQLDatabase *DatabaseConnector,
  std::string TableName, std::string ColumnName,
  std::string field, std::vector< unsigned int > ListValues,
  bool Distinct = false, bool ExcludeZero = false);

//query: "SELECT ColumnNameOne,ColumnName2 FROM TableName
//WHERE field = value ORDER BY ColumnNameOrder ASC"
QGOIO_EXPORT
std::vector< std::pair< std::string, std::string > >
ListSpecificValuesForTwoColumns(vtkMySQLDatabase *DatabaseConnector,
                                std::string TableName, std::string ColumnNameOne, std::string ColumnNameTwo,
                                std::string field, std::string value, std::string ColumnNameOrder);

//query: "SELECT MAX(ColumnName) FROM TableName WHERE (field =
//value1 or field = value2...."
QGOIO_EXPORT
int MaxValueForOneColumnInTable(vtkMySQLDatabase *DatabaseConnector,
                                std::string ColumnName, std::string TableName, std::string field,
                                std::vector< std::string > VectorValues);

//query: "SELECT MIN(ColumnName) FROM TableName WHERE (field =
//value1 or field = value2...."
QGOIO_EXPORT
int MinValueForOneColumnInTable(vtkMySQLDatabase *DatabaseConnector,
                                std::string ColumnName, std::string TableName, std::string field,
                                std::vector< std::string > VectorValues);

//query: "SELECT ColunmName FROM TableName WHERE field=value top 1"
//return only the first value if several are selected
QGOIO_EXPORT
std::string ReturnOnlyOneValue(vtkMySQLDatabase *DatabaseConnector,
                               std::string TableName, std::string ColumnName, std::string field,
                               std::string value);

//query: "SELECT LAST_INSERT_ID()"
QGOIO_EXPORT
int LastInsertID(std::string ServerName, std::string login,
                 std::string Password, std::string DBName);

//query: "SELECT MAX(ColumnName) FROM TableName"
QGOIO_EXPORT
int MaxValueForOneColumnInTable(
  vtkMySQLDatabase *DatabaseConnector,
  std::string ColumnName, std::string TableName);

//query: "SELECT MAX(ColumnName) FROM TableName WHERE field = value;"
QGOIO_EXPORT
int MaxValueForOneColumnInTable(vtkMySQLDatabase *DatabaseConnector,
                                std::string ColumnName, std::string TableName, std::string field,
                                std::string value);

//query: "SELECT TableOne.ColumnOne, TableTwo.ColumnTwo FROM TableOne
//JOIN TableTwo ON (TableOne.Foreignkey = TableTwo.PrimaryKey)
//WHERE field = value;
QGOIO_EXPORT
std::vector< std::pair< int, std::string > > ListSpecificValuesForTwoColumnsAndTwoTables(
  vtkMySQLDatabase *DatabaseConnector, std::string TableOne, std::string ColumnOne,
  std::string TableTwo, std::string ColumnTwo, std::string ForeignKey,
  std::string PrimaryKey, std::string field, std::string value);

QGOIO_EXPORT
void GetTracesInfoFromDBAndModifyContainer(
  std::list< ContourMeshStructure > & ioContainer,
  vtkMySQLDatabase *DatabaseConnector, std::string TraceName,
  std::string CollectionName, unsigned int ImgSessionID, int iTimePoint = -1,
  std::vector< int > iListIDs = std::vector< int >() );

QGOIO_EXPORT
void GetTracesInfoFromDBAndModifyContainer(
  std::list< TrackStructure > & ioContainer,
  vtkMySQLDatabase *DatabaseConnector, std::string TraceName,
  std::string CollectionName, unsigned int ImgSessionID,
  std::vector< int > iVectIDs = std::vector< int >() );

QGOIO_EXPORT
ContourMeshStructure GetTraceInfoFromDB(
  vtkMySQLDatabase *DatabaseConnector, std::string TraceName,
  std::string CollectionName, unsigned int TraceID);

/*QGOIO_EXPORT
ContourMeshStructureMultiIndexContainer* GetTracesInfoFromDBMultiIndex(
  vtkMySQLDatabase* DatabaseConnector, std::string TraceName,
  std::string CollectionName, std::string WhereField,
  unsigned int ImgSessionID, int iTimePoint = -1,
  std::vector<int> iListIDs = std::vector<int>());*/

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
std::list< unsigned int > GetColumnForBoundedValue(std::string iColumnName,
                                                   std::string iTableName,
                                                   std::string iImgSessionID,
                                                   std::string iCoordType,
                                                   std::string iValue,
                                                   vtkMySQLDatabase *DatabaseConnector);

std::list< unsigned int > GetSpecificValuesEqualToZero(
  vtkMySQLDatabase *iDatabaseConnection, std::string iColumnName, std::string iTableName,
  std::vector< std::string > iVectorConditionFieldOne,
  std::string iFieldTwo);

std::vector< std::string > ExecuteSelectQuery(vtkMySQLDatabase *iDatabaseConnector,
                                              std::string iQuery);

std::vector< std::string > GetAllSelectedValuesFromTwoTables(
  vtkMySQLDatabase *iDatabaseConnector, std::string iTableOne, std::string iTableTwo,
  std::vector< std::string > iSelectedFields, std::string iJoinCondition,
  std::string ifield, std::string ifieldValue);

//add the selected fields separated by ',' to the ioQueryStream
void GetAllSelectedFields(std::stringstream & ioQueryStream,
                          std::vector< std::string > iSelectedFields);

//select columnname FROM tablename WHERE field = value order by ASC/DESC limit
// iNumberLimit
std::vector< std::string > GetOrderByWithLimit(vtkMySQLDatabase *iDatabaseConnector,
                                               std::string iColumnName,
                                               std::string iTableName,
                                               std::string iField,
                                               std::string iValue,
                                               bool ASC,
                                               std::string iNumberLimit);

#endif
