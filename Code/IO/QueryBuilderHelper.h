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

std::string GetFirstPartQueryForTracesInfo(std::string iTraceName,std::string iCollectionName);

std::string GetSecondPartQueryForTracesInfo(std::string TraceName,
                                            std::vector<int> iVectIDs);

std::string SelectQueryStream(std::string iTable, std::string iColumn, std::string iField,
                              std::string iValue);

std::string SelectQueryStreamListConditions(std::string iTable,
                                            std::string iColumn, std::string iField,
                                            std::vector< std::string > iListValues, bool Distinct = false);

std::string SelectQueryStreamListConditions(std::string iTable,
                                            std::vector< std::string > iListColumn, std::string iField,
                                            std::vector< std::string > iListValues, bool Distinct = false);

//iselectquery union iselectquery where ijoinon IS NULL (with or without
// brackets in the
//where clause, it will work
std::string SelectWithJoinNullIncluded(std::string iSelectQuery, std::string iJoinOn,
                                       bool doublon = true);

#endif