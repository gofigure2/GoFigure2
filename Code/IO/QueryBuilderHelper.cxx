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
#include "QueryBuilderHelper.h"

#include <sstream>


std::string SelectGeneralQueryConditions(std::string iWhat, std::string iWhere, std::string iConditions)
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
std::string SelectGeneralQuery(std::string iWhat, std::string iWhere,std::string iOrderByQuery)
{
  std::stringstream querystream;
  querystream << "SELECT ";
  querystream << iWhat;
  querystream << " FROM ";
  querystream << iWhere;
  if (!iOrderByQuery.empty())
    {
    querystream << iOrderByQuery;
    }
  return querystream.str();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string GetFirstPartQueryForTracesInfo(std::string iTraceName,std::string iCollectionName)
{
  std::stringstream Querystream;
  Querystream << "SELECT ";
  Querystream << iTraceName;
  Querystream << ".";
  Querystream << iTraceName;
  Querystream << "ID, ";
  Querystream << iTraceName;
  Querystream << ".";
  Querystream << iCollectionName;
  Querystream << "ID, ";
  Querystream << iTraceName;
  Querystream << ".Points, coordinate.TCoord, color.Red,\
                 color.Green, color.Blue, color.Alpha from (";
  Querystream << iTraceName;
  Querystream << " left join coordinate on coordinate.CoordID = ";
  Querystream << iTraceName;
  Querystream << ".coordIDMax) left join color on ";
  Querystream << iTraceName;

  Querystream << ".colorID = color.colorID  where ";
  return Querystream.str().c_str();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string GetSecondPartQueryForTracesInfo(std::string TraceName,
                                            std::vector<int> iVectIDs)
{
  std::stringstream Querystream;
  unsigned int i;
  if (iVectIDs.size()>1)
    {
    Querystream << "(";
    }
  for ( i = 0; i < iVectIDs.size() - 1; i++ )
    {
    Querystream << TraceName;
    Querystream << "ID = '";
    Querystream << iVectIDs[i];
    Querystream << "' OR ";
    }
  Querystream << TraceName;
  Querystream << "ID = '";
  Querystream << iVectIDs[i];
  if (iVectIDs.size()>1)
    {
    Querystream << "')";
    }
  else
    Querystream << "'";
return Querystream.str().c_str();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStream(std::string iTable, std::string iColumn,
                              std::string iOrderByColumnName, std::string iAscDesc)
{
  std::string OrderBy;
  if (!iOrderByColumnName.empty())
    {
    OrderBy = AddOrderBy(iOrderByColumnName,iAscDesc);
    }
  return SelectGeneralQuery(iColumn,iTable,OrderBy);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStream(std::string iTable, std::vector<std::string > iListAttributes,
                              std::string iOrderByColumnName, std::string iAscDesc)
{
  std::string SelectedFields = GetSelectedAttributes(iListAttributes);
  return SelectQueryStream(iTable,SelectedFields,iOrderByColumnName,iAscDesc);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStreamCondition(std::string iTable, std::string iColumn, std::string iField,
                              std::string iValue, std::string iOrderByColumnName, std::string iAscDesc)
{
  std::stringstream Conditions;
  Conditions << iField;
  Conditions <<  " = '";
  Conditions << iValue;
  Conditions << "'";
  if (!iOrderByColumnName.empty())
    {
    Conditions << AddOrderBy(iOrderByColumnName,iAscDesc);
    }
  return SelectGeneralQueryConditions(iColumn, iTable, Conditions.str());
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStreamCondition(std::string iTable, std::vector<std::string> iListAttributes,
                                       std::string iField, std::string iValue, std::string iOrderByColumnName,
                                       std::string iAscDesc)
{
  std::string What = GetSelectedAttributes(iListAttributes);
  return SelectQueryStreamCondition(iTable,What,iField,iValue,iOrderByColumnName,iAscDesc);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStreamListConditions(std::string iTable,
                                            std::string iColumn, std::string iField,
                                            std::vector< std::string > iListValues, bool Distinct,
                                            std::string iConditionConnector)
{
  std::string What = iColumn;
  if (Distinct)
    {
    What = AddDistinctToWhat (What);
    }
  std::string Conditions = GetConditions(iField, iListValues,iConditionConnector);

  return SelectGeneralQueryConditions(What,iTable,Conditions);
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string SelectQueryStreamListConditions(std::string iTable,
                                            std::vector<std::string> iListAttributes, 
                                            std::string iField,
                                            std::vector< std::string > iListValues, 
                                            bool Distinct,
                                            std::string iConditionConnector)
{
  std::string What = GetSelectedAttributes(iListAttributes);
  return SelectQueryStreamListConditions(iTable,What,iField,iListValues,Distinct,iConditionConnector);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string AddDistinctToWhat(std::string iWhat)
{
  std::string oWhat = "DISTINCT ";
  oWhat += iWhat;
  return oWhat;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string AddOrderBy(std::string iAttribute,std::string iAscDesc)
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
std::string GetConditions(std::string iField,
                          std::vector< std::string > iListValues,
                          std::string iConditionConnector)
{
  std::stringstream oConditions;
  oConditions << "(";
  unsigned int i;
    for ( i = 0; i < iListValues.size() - 1; i++ )
      {
      oConditions << iField;
      oConditions << " = '";
      oConditions << iListValues[i];
      oConditions << "' ";
      oConditions << iConditionConnector;
      oConditions  << " ";
      }
    oConditions << iField;
    oConditions << " = '";
    oConditions << iListValues[i];
    oConditions << "')";

  return oConditions.str();
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
std::string GetSelectedAttributes(std::vector<std::string> iListAttributes)
{
  std::stringstream oQueryStream;
  unsigned int i;
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
/*std::string SelectWithJoinNullIncluded(std::string iSelectQuery,
                                       std::string iJoinOn,
                                       bool doublon)
{
  std::stringstream QueryStream;

  QueryStream << iSelectQuery;
  QueryStream << " UNION ";
  if (doublon)
    {
    QueryStream << " ALL ";
    }
  //QueryStream << iSelectQuery;
  size_t            posWhere = iSelectQuery.find("WHERE", 0);
  std::stringstream SecondPart;
  if ( posWhere != std::string::npos )
    {
    size_t posCloseBracket = iSelectQuery.find(")", iSelectQuery.size() - 1);
      {
      //if an end bracket is found, remove it:
      if ( posCloseBracket != std::string::npos )
        {
        SecondPart << iSelectQuery.substr(0, posCloseBracket - 1);
        }
      //if an end bracket is not found, add an opening bracket after where:
      else
        {
        SecondPart << iSelectQuery.substr(0, posWhere + 6);
        SecondPart << "(";
        size_t BegEndQuery = posWhere + 6;
        size_t Length = iSelectQuery.size() - BegEndQuery;
        SecondPart << iSelectQuery.substr(posWhere + 6, Length);
        }
      }
    }
  else
    {
    SecondPart << iSelectQuery;
    SecondPart << "WHERE ";
    }
  QueryStream << SecondPart.str();
  QueryStream << " AND ";
  QueryStream << iJoinOn;
  QueryStream << " IS NULL";
  if ( posWhere != std::string::npos )
    {
    QueryStream << ");";
    }
  else
    {
    QueryStream << ";";
    }

  return QueryStream.str();
}*/

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------