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

#include "GoDBRow.h"
#include <iostream>

GoDBRow::GoDBRow()
{
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBRow::~GoDBRow()
{
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBRow::SetField(const std::string& key, const std::string& value)
{
  StringMapIterator it = m_MapRow.find(key);

  if ( it != m_MapRow.end() )
    {
    std::stringstream valueToQuery;
    valueToQuery << "\"" << value << "\"";
    it->second = valueToQuery.str();
    }
  else
    {
    std::cerr << "This field does not exist!!!" << std::endl;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GoDBRow::PrintValues()
{
  std::stringstream ListValues;

  for ( StringMapIterator iter = m_MapRow.begin();
        iter != m_MapRow.end(); ++iter )
    {
    ListValues << iter->second << ", ";
    }
  size_t n = ListValues.str().length() - 2;
  return ListValues.str().substr(0, n);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GoDBRow::PrintColumnNames()
{
  std::stringstream ListColumnNames;

  for ( StringMapIterator iter = m_MapRow.begin();
        iter != m_MapRow.end();
        ++iter )
    {
    ListColumnNames << iter->first << ", ";
    }
  size_t n = ListColumnNames.str().length() - 2;
  return ListColumnNames.str().substr(0, n);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GoDBRow::PrintColumnNamesWithValues()
{
  std::stringstream StringQuery;

  for ( StringMapIterator iter = m_MapRow.begin();
        iter != m_MapRow.end();
        ++iter )
    {
    StringQuery << iter->first;
    StringQuery << " = ";
    StringQuery << iter->second;
    StringQuery << ", ";
    }
  size_t n = StringQuery.str().length() - 2;
  return StringQuery.str().substr(0, n);
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector< std::string > GoDBRow::GetVectorColumnNames()
{
  std::vector< std::string > VectorColumnNames;
  for ( StringMapIterator iter = m_MapRow.begin();
        iter != m_MapRow.end();
        ++iter )
    {
    VectorColumnNames.push_back(iter->first);
    }
  return VectorColumnNames;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBRow::StringMapIterator
GoDBRow::MapBegin()
{
  return m_MapRow.begin();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBRow::StringMapIterator
GoDBRow::MapEnd()
{
  return m_MapRow.end();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBRow::StringMapConstIterator
GoDBRow::ConstMapBegin()
{
  return m_MapRow.begin();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
GoDBRow::StringMapConstIterator
GoDBRow::ConstMapEnd()
{
  return m_MapRow.end();
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GoDBRow::GetMapValue(const std::string& key)
{
  std::string oMapValue = "noValue";

  StringMapIterator iter = m_MapRow.find(key);

  if ( iter == m_MapRow.end() )
    {
    return oMapValue;
    }
  else
    {
    oMapValue = iter->second;

    // Need to test if the value is not a string previously put in the map by
    // SetField
    // if so, the value will be ""value"" and need to be transformed to "value".
    // First,
    // find the 1rst character and save it as CharacterToCompare:
    std::string CharacterToCompare = oMapValue.substr(0, 1);

    //test if it is equal to " :
    if ( CharacterToCompare == "\"" )
      {
      //if yes, remove the " at the beginning of the string and at the end:
      oMapValue = oMapValue.substr(1, oMapValue.size() - 2);
      }

    return oMapValue;
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
bool GoDBRow::SetValuesForSpecificID(int ID,
                                     vtkMySQLDatabase *iDatabaseConnector)
{
  std::vector< std::string > ResultQuery =
    ListSpecificValuesForOneColumn(
      iDatabaseConnector, this->m_TableName, this->PrintColumnNames(),
      this->m_TableIDName, ConvertToString< int >(ID) );

  if ( ResultQuery.empty() )
    {
    return false;
    }

  if ( this->m_MapRow.size() != ResultQuery.size() )
    {
    std::cout << "pb the map and the query results are not the same size";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return false;
    }

  std::vector< std::string >::iterator iterResultQuery = ResultQuery.begin();

  StringMapIterator iterMap = this->MapBegin();
  StringMapIterator iterEnd = this->MapEnd();

  while ( iterMap != iterEnd )
    {
    iterMap->second = *iterResultQuery;
    ++iterMap;
    ++iterResultQuery;
    }
  return true;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GoDBRow::GetTableName()
{
  return this->m_TableName;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GoDBRow::GetTableIDName()
{
  return this->m_TableIDName;
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBRow::AddConditions(
  const std::string& iNameOfField, std::vector< FieldWithValue > & ioFieldWithValue)
{
  FieldWithValue temp = { iNameOfField, this->GetMapValue(iNameOfField), "=" };

  ioFieldWithValue.push_back(temp);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBRow::DeleteFromDB(
  vtkMySQLDatabase *iDatabaseConnector)
{
  if (this->GetMapValue(this->m_TableIDName) != "0")
    {
    DeleteRow(iDatabaseConnector,
      this->m_TableName, this->m_TableIDName, this->GetMapValue(this->m_TableIDName) );
    }
}
