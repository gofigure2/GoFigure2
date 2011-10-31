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
#include "QueryBuilderHelper.h"
#include "ConvertToStringHelper.h"

#include <sstream>

std::string SelectGeneralQueryConditions(
  const std::string & iWhat,
  const std::string & iWhere,
  const std::string & iConditions)
{
  std::stringstream querystream;

  querystream << "SELECT ";
  querystream << iWhat;
  querystream << " FROM ";
  querystream << iWhere;
  querystream << " WHERE ";
  querystream << iConditions;
  return querystream.str();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectGeneralQuery(
  const std::string & iWhat,
  const std::string & iWhere,
  std::string iOrderByQuery)
{
  std::stringstream querystream;

  querystream << "SELECT ";
  querystream << iWhat;
  querystream << " FROM ";
  querystream << iWhere;
  if ( !iOrderByQuery.empty() )
    {
    querystream << iOrderByQuery;
    }
  return querystream.str();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStream(
  const std::string & iTable,
  const std::string & iColumn,
  std::string iOrderByColumnName,
  std::string iAscDesc)
{
  std::string OrderBy;

  if ( !iOrderByColumnName.empty() )
    {
    OrderBy = AddOrderBy(iOrderByColumnName, iAscDesc);
    }
  return SelectGeneralQuery(iColumn, iTable, OrderBy);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStream(
  const std::string & iTable,
  const std::vector< std::string > & iListAttributes,
  std::string iOrderByColumnName,
  std::string iAscDesc)
{
  std::string SelectedFields = GetSelectedAttributes(iListAttributes);

  return SelectQueryStream(iTable, SelectedFields, iOrderByColumnName, iAscDesc);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStreamCondition(
  const std::string & iTable,
  const std::string & iColumn,
  const std::string & iConditions,
  bool Distinct,
  std::string iOrderByColumnName,
  std::string iAscDesc)
{
  std::string What = iColumn;
  std::string Condition = iConditions;

  if ( Distinct )
    {
    What = AddDistinctToWhat(What);
    }
  if ( !iOrderByColumnName.empty() )
    {
    Condition += AddOrderBy(iOrderByColumnName, iAscDesc);
    }
  return SelectGeneralQueryConditions(What, iTable, Condition);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStreamCondition(
  const std::string & iTable,
  const std::string & iColumn,
  const std::string & iField,
  const std::string & iValue,
  std::string iOrderByColumnName,
  std::string iAscDesc,
  bool Distinct)
{
  std::stringstream Conditions;

  Conditions << iField;
  Conditions <<  " = '";
  Conditions << iValue;
  Conditions << "'";
  return SelectQueryStreamCondition(iTable, iColumn, Conditions.str(), Distinct, iOrderByColumnName, iAscDesc);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStreamCondition(
  const std::string & iTable,
  const std::vector< std::string > & iListAttributes,
  const std::string & iField,
  const std::string & iValue,
  std::string iOrderByColumnName,
  std::string iAscDesc)
{
  std::string What = GetSelectedAttributes(iListAttributes);

  return SelectQueryStreamCondition(iTable, What, iField, iValue, iOrderByColumnName, iAscDesc);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStreamListConditions(
  const std::string & iTable,
  const std::string & iColumn,
  const std::string & iField,
  const std::vector< std::string > & iListValues,
  bool Distinct,
  std::string iConditionConnector)
{
  std::string Conditions = GetConditions(iField, iListValues, iConditionConnector);

  return SelectQueryStreamCondition(iTable, iColumn, Conditions, Distinct);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStreamListConditions(
  const std::string & iTable,
  const std::vector< std::string > & iListAttributes,
  const std::string & iField,
  const std::vector< std::string > & iListValues,
  std::string iConditionConnector,
  bool Distinct)
{
  std::string What = GetSelectedAttributes(iListAttributes);

  return SelectQueryStreamListConditions(iTable, What, iField, iListValues,
                                         Distinct, iConditionConnector);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStreamListConditions(
  const std::string & iTable,
  const std::string & iColumn,
  const std::vector< FieldWithValue > & iConditions,
  std::string iConditionConnector,
  bool Distinct)
{
  std::string What = iColumn;

  if ( Distinct )
    {
    What = AddDistinctToWhat (What);
    }
  std::string Conditions = GetConditions(iConditions, iConditionConnector);
  return SelectQueryStreamCondition(iTable, iColumn, Conditions, Distinct);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStreamListConditions(
  const std::string & iTable,
  const std::vector< std::string > & iListAttributes,
  const std::vector< FieldWithValue > & iConditions,
  std::string iConditionConnector,
  bool Distinct,
  std::string iOrderByColumnName)
{
  std::string What = GetSelectedAttributes(iListAttributes);
  std::string oQueryString = SelectQueryStreamListConditions(iTable, What, iConditions, iConditionConnector,
                                         Distinct);
  if (!iOrderByColumnName.empty())
    {
    oQueryString += AddOrderBy(iOrderByColumnName);
    }

  return oQueryString;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string AddDistinctToWhat(const std::string & iWhat)
{
  std::string oWhat = "DISTINCT ";

  oWhat += iWhat;
  return oWhat;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string AddOrderBy(const std::string & iAttribute, std::string iAscDesc)
{
  std::string oOrderBy = " ORDER BY ";

  oOrderBy += iAttribute;
  oOrderBy += " ";
  oOrderBy += iAscDesc;
  oOrderBy += " ";
  return oOrderBy;
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string GetConditions(
  const std::vector< FieldWithValue > & iConditions,
  std::string iConditionConnector)
{
  std::stringstream oConditions;

  oConditions << "(";
  unsigned int i;
  // why -1??
  for ( i = 0; i < iConditions.size() - 1; i++ )
    {

    oConditions << iConditions[i].Field;
    oConditions << iConditions[i].Operator;
    oConditions << " '";
    oConditions << iConditions[i].Value;
    oConditions << "' ";
    oConditions << iConditionConnector;
    oConditions << " ";
    }
  oConditions << iConditions[i].Field;
  oConditions << iConditions[i].Operator;
  oConditions << " '";
  oConditions << iConditions[i].Value;
  oConditions << "')";
  return oConditions.str();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string GetConditions(
  const std::string & iField,
  const std::string & iValue,
  std::string iConnector)
{
  std::vector< FieldWithValue > Condition(1);
  FieldWithValue                Field = { iField, iValue, iConnector };
  Condition[0] = Field;
  return GetConditions(Condition);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GetSelectedAttributes(const std::vector< std::string > & iListAttributes)
{
  std::stringstream oQueryStream;
  unsigned int      i;

  for ( i = 0; i < iListAttributes.size() - 1; i++ )
    {
    oQueryStream << iListAttributes[i];
    oQueryStream << ", ";
    }
  oQueryStream << iListAttributes[i];
  return oQueryStream.str();
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::vector< std::string > ListUnsgIntToVectorString(
  const std::list< unsigned int > & iList)
{
  std::list< unsigned int >::const_iterator iter = iList.begin();
  std::vector< std::string >          oVector;
  while ( iter != iList.end() )
    {
    unsigned int temp = *iter;
    oVector.push_back( ConvertToString< unsigned int >(temp) );
    ++iter;
    }
  return oVector;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::list< unsigned int > VectorStringToUnsgInt(
  const std::vector< std::string > & iVector)
{
  std::vector< std::string >::const_iterator iter = iVector.begin();
  std::list< unsigned int >            oList;
  while ( iter != iVector.end() )
    {
    oList.push_back( ss_atoi< unsigned int >( *iter ) );
    ++iter;
    }
  return oList;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< std::string > VectorUnsgIntToVectorString(
  const std::vector< unsigned int > & iVector)
{
  std::vector< unsigned int >::const_iterator iter = iVector.begin();
  std::vector< std::string >            oVector;
  while ( iter != iVector.end() )
    {
    oVector.push_back( ConvertToString< unsigned int >( *iter ) );
    ++iter;
    }
  return oVector;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GetLeftJoinTwoTables(
  const std::string & iTableOne,
  const std::string & iTableTwo,
  const FieldWithValue & iOnCondition,
  bool NonNULLRows)
{
  std::stringstream oQueryStream;

  oQueryStream << iTableOne;
  if (NonNULLRows)
  {
  oQueryStream << " JOIN ";
  }
  else
  {
  oQueryStream << " LEFT JOIN ";
  }
  oQueryStream << iTableTwo;
  oQueryStream << " ON ";
  oQueryStream << iTableOne;
  oQueryStream << ".";
  oQueryStream << iOnCondition.Field;
  oQueryStream << iOnCondition.Operator;
  oQueryStream << iTableTwo;
  oQueryStream << ".";
  oQueryStream << iOnCondition.Value;

  return oQueryStream.str();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GetLeftJoinThreeTables(
  const std::string & iTable,
  const std::string & iTableTwo,
  const std::string & iTableThree,
  const FieldWithValue & iOnConditionOne,
  const FieldWithValue & iOnConditionTwo)
{
  std::stringstream oQueryStream;

  oQueryStream << "(";
  oQueryStream << GetLeftJoinTwoTables(iTable, iTableTwo, iOnConditionOne);
  oQueryStream << ")";
  std::string LeftJoinTwo =  GetLeftJoinTwoTables(iTable, iTableThree, iOnConditionTwo);
  oQueryStream << LeftJoinTwo.substr( iTable.size() + 1, LeftJoinTwo.size() );
  return oQueryStream.str();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GetGroupBy(const std::string & iColumn, unsigned int iNumberDoublons)
{
  std::stringstream oQueryStream;

  oQueryStream << " group by ";
  oQueryStream << iColumn;
  if ( iNumberDoublons != 0 )
    {
    oQueryStream << " HAVING COUNT(";
    oQueryStream << iColumn;
    oQueryStream << ") > ";
    oQueryStream << iNumberDoublons;
    }
  return oQueryStream.str();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string SelectForTracesInfo(
  const std::vector< std::string > & iSelectedAttributes,
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
  std::vector< unsigned int > VectIDs( iListIDs.begin(), iListIDs.end() );
  std::string                 What = GetSelectedAttributes(iSelectedAttributes);
  std::string                 Where = GetLeftJoinThreeTables(iTableOne, iTableTwo,
                                                             iTableThree,
                                                             iJoinConditionOne,
                                                             iJoinConditionTwo);
  FieldWithValue FirstPartCondition = { iFieldOne,
                                        ConvertToString< unsigned int >(iValueFieldOne),
                                        "=" };

  std::string    Conditions = GetAndORConditions< unsigned int >(FirstPartCondition,
                                                                 iIDFieldName,
                                                                 VectIDs);
  std::string QueryString = SelectQueryStreamCondition(Where, What, Conditions);
  return QueryString;
}
