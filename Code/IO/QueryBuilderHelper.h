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
#ifndef __QueryBuilderHelper_h
#define __QueryBuilderHelper_h

#include <vector>
#include <string>

/**
\brief SELECT iWhat FROM iWhere WHERE iConditions
\param[in] iWhat list of attributes separated by commas
\param[in] iWhere list of tables, included joined tables
\param[in] iConditions list of conditions
\return the string corresponding to the query part
*/
std::string SelectGeneralQueryConditions(std::string iWhat, std::string iWhere, std::string iConditions);

/**
\brief SELECT iWhat FROM iWhere iOrderByQuery
\param[in] iWhat list of attributes separated by commas
\param[in] iWhere list of tables, included joined tables
\param[in] iOrderByQuery part of the query to order by
\return the string corresponding to the query part
*/
std::string SelectGeneralQuery(std::string iWhat, std::string iWhere,std::string iOrderByQuery = "");

/**
\brief DISTINCT iWhat
\param[in] iWhat attributes
\return part of the query to make iWhat distinct
*/
std::string AddDistinctToWhat(std::string iWhat);

/**
\brief ORDER BY iAttributes iAscDesc
\param[in] iAttribute attribute to be sorted
\param[in] iAscDesc ascendent or descendent sorting 
\return the string corresponding to the query part
*/
std::string AddOrderBy(std::string iAttribute,std::string iAscDesc = "ASC");

/**
\brief iListAttributes[i], iListAttributes[i+1]...
\param[in] iListAttributes list of the attributes to be selected
\return the string corresponding to the query part
*/
std::string GetSelectedAttributes(std::vector<std::string> iListAttributes);

/**
\brief (iAttribute = iListValues[i] iConditionConnector iAttribute = iListValues[i+1]...)
\param[in] iAttribute attribute who has to be equal to the iListValues
\param[in] iListValues list of all the values iAttribute can be equal to
\param[in] iConditionConnector AND/OR 
\return the string corresponding to the query part
*/
std::string GetConditions(std::string iAttribute,
                          std::vector< std::string > iListValues,
                          std::string iConditionConnector);

std::string GetFirstPartQueryForTracesInfo(std::string iTraceName,std::string iCollectionName);

std::string GetSecondPartQueryForTracesInfo(std::string TraceName,
                                            std::vector<int> iVectIDs);

/**
\brief SELECT iColumn FROM iTable ORDER BY iOrderByColumnName iAscDesc;
\param[in] iTable name of the database table
\param[in] iColumn name of the attribute to select
\param[in] iOrderByColumnName name of the column by which the results will be sorted, if
empty, no sorting 
\param[in] iAscDesc order to sort the results, ascendent by default
\return the string corresponding to the query part
*/
std::string SelectQueryStream(std::string iTable, std::string iColumn,
                              std::string iOrderByColumnname = "", std::string iAscDesc = "ASC");

/**
\brief SELECT iColumn[i],iColumn[i=1]... FROM iTable ORDER BY iOrderByColumnName iAscDesc;
\param[in] iTable name of the database table
\param[in] iListAttributes name of the attributes to select
\param[in] iOrderByColumnName name of the column by which the results will be sorted, if
empty, no sorting 
\param[in] iAscDesc order to sort the results, ascendent by default
\return the string corresponding to the query part
*/
std::string SelectQueryStream(std::string iTable, std::vector<std::string > iListAttributes,
                              std::string iOrderByColumnName = "", std::string iAscDesc = "ASC");
//basic
/**
\brief SELECT iColumn FROM iTable WHERE iField = iValue ORDER BY iColumn iAscDesc;
*/
std::string SelectQueryStreamCondition(std::string iTable, std::string iColumn, std::string iField,
                              std::string iValue,std::string iOrderByColumnName = "", std::string iAscDesc = "ASC");

std::string SelectQueryStreamCondition(std::string iTable, std::vector<std::string> iListAttributes,
                                       std::string iField, std::string iValue, std::string iOrderByColumnName = "",
                                       std::string iAscDesc = "ASC");

std::string SelectQueryStreamListConditions(std::string iTable,
                                            std::string iColumn, std::string iField,
                                            std::vector< std::string > iListValues, bool Distinct = false,
                                            std::string iConditionConnector = "OR");

std::string SelectQueryStreamListConditions(std::string iTable,
                                            std::vector<std::string> iListAttributes, 
                                            std::string iField,
                                            std::vector< std::string > iListValues, 
                                            bool Distinct = false,
                                            std::string iConditionConnector = "OR");

std::string SelectQueryStreamListConditions(std::string iTable,
                                            std::vector<std::string> iListAttributes, 
                                            std::string iField,
                                            std::string iValue, 
                                            bool Distinct = false,
                                            std::string iConditionConnector = "OR");

//iselectquery union iselectquery where ijoinon IS NULL (with or without
// brackets in the
//where clause, it will work
//std::string SelectWithJoinNullIncluded(std::string iSelectQuery, std::string iJoinOn,
                                       //bool doublon = true);

#endif