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
#ifndef __QueryBuilderHelper_h
#define __QueryBuilderHelper_h

#include <vector>
#include <string>
#include <sstream>
#include <list>
#include "QGoIOConfigure.h"

#include "QGoIOConfigure.h"

struct QGOIO_EXPORT FieldWithValue
  {
    std::string Field;
    std::string Value;
    std::string Operator;
  };
/**
\brief SELECT iWhat FROM iWhere WHERE iConditions
\param[in] iWhat list of attributes separated by commas
\param[in] iWhere list of tables, included joined tables
\param[in] iConditions list of conditions
\return the string corresponding to the query part
*/
QGOIO_EXPORT
std::string SelectGeneralQueryConditions(
  const std::string & iWhat,
  const std::string & iWhere,
  const std::string & iConditions);

/**
\brief SELECT iWhat FROM iWhere iOrderByQuery
\param[in] iWhat list of attributes separated by commas
\param[in] iWhere list of tables, included joined tables
\param[in] iOrderByQuery part of the query to order by
\return the string corresponding to the query part
*/
QGOIO_EXPORT
std::string SelectGeneralQuery(
  const std::string & iWhat,
  const std::string & iWhere,
  std::string iOrderByQuery = "");

/**
\brief DISTINCT iWhat
\param[in] iWhat attributes
\return part of the query to make iWhat distinct
*/
QGOIO_EXPORT
std::string AddDistinctToWhat(const std::string & iWhat);

/**
\brief ORDER BY iAttributes iAscDesc
\param[in] iAttribute attribute to be sorted
\param[in] iAscDesc ascendent or descendent sorting
\return the string corresponding to the query part
*/
QGOIO_EXPORT
std::string AddOrderBy(const std::string & iAttribute,
                       std::string iAscDesc = "ASC");

/**
\brief iListAttributes[i], iListAttributes[i+1]...
\param[in] iListAttributes list of the attributes to be selected
\return the string corresponding to the query part
*/
QGOIO_EXPORT
std::string GetSelectedAttributes(const std::vector<std::string> & iListAttributes);

/**
\brief (iField = iListValues[i] iConditionConnector iField = iListValues[i+1]...)
\param[in] iVectorValues list of all the values iAttribute can be equal to
\param[in] iConditionConnector AND/OR
\param[in] iField
\tparam T
\return the string corresponding to the query part
*/
template< typename T >
std::string GetConditions(const std::string & iField,
                          const std::vector<T> & iVectorValues,
                          std::string iConditionConnector = "AND")
{
  std::stringstream oConditions;
  oConditions << "";
  if (!iVectorValues.empty() )
    {
    oConditions << "(";
    unsigned int i;
      for ( i = 0; i < iVectorValues.size() - 1; i++ )
        {
        oConditions << iField;
        oConditions << " = '";
        oConditions << iVectorValues[i];
        oConditions << "' ";
        oConditions << iConditionConnector;
        oConditions  << " ";
        }
    oConditions << iField;
    oConditions << " = '";
    oConditions << iVectorValues[i];
    oConditions << "')";
    }
  return oConditions.str();
}

QGOIO_EXPORT
std::string GetConditions(const std::vector<FieldWithValue> & iConditions,
                          std::string iConditionConnector = "AND");

QGOIO_EXPORT
std::string GetConditions(const std::string & iField,
                          const std::string & iValue,
                          std::string iOperator = "=");

/**
\brief (iFirstPartCondition AND (iField = iOrVectorValues1 OR iField = iOrVectorValues1...))
*/
template< typename T >
std::string GetAndORConditions(
  const FieldWithValue & iFirtsPartCondition,
  const std::string & iField,
  const std::vector< T > & iOrVectorValues)
{
  std::string oConditions;
  std::vector<FieldWithValue> VectorConditions(1);
  //FieldWithValue AndCondition = {fieldTwo,ValueFieldTwo, "="};
  VectorConditions[0] = iFirtsPartCondition;
  oConditions = GetConditions( VectorConditions, "AND" );
  if (!iOrVectorValues.empty() )
    {
    oConditions = oConditions.substr(0, oConditions.size()-1);
    oConditions += " AND ";
    oConditions += GetConditions<T>(iField,iOrVectorValues,"OR");
    oConditions += ")";
    }
  return oConditions;
}

//std::string GetConditions(std::vector<FieldWithValue> iConditions,
 //                         std::string iConditionConnector = "AND");

//std::string GetConditions(std::string iField, std::string iValue,std::string iOperator = "=");

/**
\brief SELECT iColumn FROM iTable ORDER BY iOrderByColumnName iAscDesc;
\param[in] iTable name of the database table
\param[in] iColumn name of the attribute to select
\param[in] iOrderByColumnName name of the column by which the results will be sorted, if
empty, no sorting
\param[in] iAscDesc order to sort the results, ascendent by default
\return the string corresponding to the query part
*/
QGOIO_EXPORT
std::string SelectQueryStream(
  const std::string & iTable,
  const std::string & iColumn,
  std::string iOrderByColumnName = "",
  std::string iAscDesc = "ASC");

/**
\brief SELECT iColumn[i],iColumn[i=1]... FROM iTable ORDER BY iOrderByColumnName iAscDesc;
\param[in] iTable name of the database table
\param[in] iListAttributes name of the attributes to select
\param[in] iOrderByColumnName name of the column by which the results will be sorted, if
empty, no sorting
\param[in] iAscDesc order to sort the results, ascendent by default
\return the string corresponding to the query part
*/
QGOIO_EXPORT
std::string SelectQueryStream(
  const std::string & iTable,
  const std::vector<std::string > & iListAttributes,
  std::string iOrderByColumnName = "",
  std::string iAscDesc = "ASC");

/**
\brief SELECT (Distinct) iColumn FROM iTable WHERE iConditions
(ORDER BY irderByColumnName iAscDesc);
\param[in] iTable name of the database table
\param[in] iColumn name of the attribute to select
\param[in] iOrderByColumnName name of the column by which the results will be sorted, if
empty, no sorting
\param[in] iAscDesc order to sort the results, ascendent by default
\param[in] iConditions name of the columns separated by comma that are part of the 'where'
\param[in] Distinct true if no doublon allowed
\return the string corresponding to the query part
*/
QGOIO_EXPORT
std::string SelectQueryStreamCondition(
  const std::string & iTable,
  const std::string & iColumn,
  const std::string & iConditions,
  bool Distinct = false,
  std::string iOrderByColumnName = "",
  std::string iAscDesc = "ASC");
/**
\overload
*/
QGOIO_EXPORT
std::string SelectQueryStreamCondition(
  const std::string & iTable,
  const std::string & iColumn,
  const std::string & iField,
  const std::string & iValue,
  std::string iOrderByColumnName = "",
  std::string iAscDesc = "ASC",
  bool Distinct = false);

/**
\overload
*/
QGOIO_EXPORT
std::string SelectQueryStreamCondition(
  const std::string & iTable,
  const std::vector<std::string> & iListAttributes,
  const std::string & iField,
  const std::string & iValue,
  std::string iOrderByColumnName = "",
  std::string iAscDesc = "ASC");

/**
\brief SELECT (Distinct) iColumn FROM iTable WHERE iField = iValue Or/And...
\param[in] iTable name of the database table
\param[in] iColumn name of the attribute to select
\param[in] iField name of the condition
\param[in] iListValues values of the condition
\param[in] Distinct if set to true no doublon allowed
\param[in] iConditionConnector or/and
\return the string corresponding to the query part
*/
QGOIO_EXPORT
std::string SelectQueryStreamListConditions(
  const std::string & iTable,
  const std::string & iColumn,
  const std::string & iField,
  const std::vector< std::string > & iListValues,
  bool Distinct = false,
  std::string iConditionConnector = "OR");

/**
\overload
*/
QGOIO_EXPORT
std::string SelectQueryStreamListConditions(
  const std::string & iTable,
  const std::vector<std::string> & iListAttributes,
  const std::string & iField,
  std::vector< std::string > & iListValues,
  bool Distinct = false,
  std::string iConditionConnector = "OR");

/**
\overload
*/
QGOIO_EXPORT
std::string SelectQueryStreamListConditions(
  const std::string & iTable,
  const std::vector<std::string> & iListAttributes,
  const std::string & iField,
  const std::string & iValue,
  bool Distinct = false,
  std::string iConditionConnector = "OR");
/**
\overload
*/
QGOIO_EXPORT
std::string SelectQueryStreamListConditions(
  const std::string & iTable,
  const std::string & iColumn,
  const std::vector<FieldWithValue> & iConditions,
  std::string iConditionConnector = "OR",
  bool Distinct = false);

/**
\overload
*/
QGOIO_EXPORT
std::string SelectQueryStreamListConditions(
  const std::string & iTable,
  const std::vector<std::string> & iListAttributes,
  const std::vector<FieldWithValue> & iConditions,
  std::string iConditionConnector = "OR",
  bool Distinct = false,
  std::string iOrderByColumnName = "");

QGOIO_EXPORT
std::vector< std::string > ListUnsgIntToVectorString(const std::list< unsigned int > & iList);

QGOIO_EXPORT
std::list< unsigned int > VectorStringToUnsgInt(const std::vector< std::string > & iVector);

QGOIO_EXPORT
std::vector< std::string > VectorUnsgIntToVectorString(const std::vector<unsigned int> & iVector);

/**
\brief iTableOne LEFT JOIN iTableTwo ON iTableOne.iOnCondition/Field = iTableTwo.iOnCondition/Value
\param[in] iTableOne table to be joined
\param[in] iTableTwo table to be joined to
\param[in] iOnCondition join on which condition
\param[in] NonNULLRows if the connection is not found for 2 tables, there won't be a result
\return the string corresponding to the query part
*/
QGOIO_EXPORT
std::string GetLeftJoinTwoTables(
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const FieldWithValue & iOnCondition,
  bool NonNULLRows = false);

/**
\brief (iTable LEFT JOIN iTableTwo ON iTable.iOnCondition/Field = iTableTwo.iOnCondition/Value)
LEFT JOIN iTableThree ON iTable.iOnCondition/Field = iTableThree.iOnCondition/Value)
\param[in] iTable table to be joined
\param[in] iTableTwo table to be joined to the 1rst one
\param[in] iTableThree table to be joined ot the 1rst one
\param[in] iOnConditionOne join on which condition between table and tableTwo
\param[in] iOnConditionTwo join on which condition between table and tableThree
\return the string corresponding to the query part
*/
QGOIO_EXPORT
std::string GetLeftJoinThreeTables(
  const std::string & iTable,
  const std::string & iTableTwo,
  const std::string & iTableThree,
  const FieldWithValue & iOnConditionOne,
  const FieldWithValue & iOnConditionTwo);

QGOIO_EXPORT
std::string GetGroupBy(const std::string & iColumn, unsigned int iNumberDoublons);

/**
\brief SELECT iSelectedAttributes[0], iSelectedAttributes[1]...FROM (iTableOne left join
iTableTwo on iJoinConditionOne) left join tableThree on iJoinConditionTwo where (iFieldOne
= iValueFieldOne) AND (IDFieldName = iListIDs[0] OR IDFieldName = iListIDs[1] OR....);
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
\return the string corresponding to the query part
*/
QGOIO_EXPORT
std::string SelectForTracesInfo(
  const std::vector<std::string> & iSelectedAttributes,
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const std::string & iTableThree,
  const FieldWithValue & iJoinConditionOne,
  const FieldWithValue & iJoinConditionTwo,
  const std::string & iFieldOne,
  unsigned int iValueFieldOne,
  const std::string & iIDFieldName,
  const std::list< unsigned int > & iListIDs);

#endif
