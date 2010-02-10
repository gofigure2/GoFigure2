/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 455 $  // Revision of last commit
  Date: $Date: 2009-07-28 14:31:26 -0400 (Tue, 28 Jul 2009) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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
#include <map>
#include <iostream>
#include "SelectQueryDatabaseHelper.h"

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
void GoDBRow::SetField( std::string key, std::string value )
{
   std::stringstream valueToQuery;
   valueToQuery <<"\"" << value <<"\"";
   m_MapRow[key] = valueToQuery.str();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GoDBRow::PrintValues()
{
  std::stringstream ListValues;
  for( std::map<std::string, std::string>::iterator iter = m_MapRow.begin();
    iter != m_MapRow.end(); ++iter )
    {
    ListValues << iter->second<< ", ";
    }
  int n = ListValues.str().length() -2;
  return ListValues.str().substr(0,n);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::string GoDBRow::PrintColumnNames()
    {
    std::stringstream ListColumnNames;
    for( std::map<std::string, std::string>::iterator iter = m_MapRow.begin();
      iter != m_MapRow.end(); ++iter )
      {
      ListColumnNames << iter->first<< ", ";
      }
    int n = ListColumnNames.str().length() -2;
    return ListColumnNames.str().substr(0,n);
    }
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<std::string> GoDBRow::GetVectorColumnNames()
{
  std::vector<std::string> VectorColumnNames; 
  for( std::map<std::string, std::string>::iterator iter = m_MapRow.begin();
      iter != m_MapRow.end(); ++iter )
    {
    VectorColumnNames.push_back(iter->first);
    }
  return VectorColumnNames;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
/*std::list<std::string> GoDBRow::GetListColumnNames()
{
  std::list<std::string> ListColumnNames;
  for( std::map<std::string, std::string>::iterator iter = m_MapRow.begin();
      iter != m_MapRow.end(); ++iter )
    {
    ListColumnNames.push_back(iter->first);
    }
  return ListColumnNames;

}*/
 //-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::map<std::string,std::string>::iterator GoDBRow::MapBegin()
{
  return m_MapRow.begin();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::map<std::string,std::string>::iterator GoDBRow::MapEnd()
{
  return m_MapRow.end();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::map<std::string,std::string>::const_iterator GoDBRow::ConstMapBegin()
{
  return m_MapRow.begin();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::map<std::string,std::string>::const_iterator GoDBRow::ConstMapEnd()
{
  return m_MapRow.end();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// std::string GoDBRow::GetMapValue( const std::string& key )
std::string GoDBRow::GetMapValue( std::string key )
{
  std::string oMapValue = "noValue";

/// \todo check this alternative. It may be more efficient to use std::map::find
//   std::map< std::string, std::string >::iterator iter = m_MapRow.find( key );
//
//   if( iter = m_MapRow.end() )
//     {
//     return oMapValue;
//     }
//   else
//     {
//     oMapValue = iter->second;
//
//     // Need to test if the value is not a string previously put in the map by SetField
//     // if so, the value will be ""value"" and need to be transformed to "value". First,
//     // find the 1rst character and save it as CharacterToCompare:
//     std::string CharacterToCompare = MapValue.substr(0,1);
//
//     //test if it is equal to " :
//     if (CharacterToCompare == "\"")
//       {
//       //if yes, remove the " at the beginning of the string and at the end:
//       oMapValue = MapValue.substr(1,MapValue.size()-2);
//       }
//     // remove the std::cout
//     else
//       {
//       std::cout<<MapValue.c_str()<<std::endl;
//       }
//
//     return oMapValue;
//     }

  for( std::map<std::string, std::string>::iterator iter = this->MapBegin();
    iter != this->MapEnd(); ++iter )
    {
     //if the key has been found in the map:
      if (iter->first == key)
      {
      oMapValue = iter->second;
      // Need to test if the value is not a string previously put in the map by SetField
      // if so, the value will be ""value"" and need to be transformed to "value". First,
      // find the 1rst character and save it as CharacterToCompare:
      std::string CharacterToCompare = oMapValue.substr(0,1);

      //test if it is equal to " :
      if (CharacterToCompare == "\"")
        {
        //if yes, remove the " at the beginning of the string and at the end:
        oMapValue = oMapValue.substr(1,oMapValue.size()-2);
        return oMapValue;
        }
      std::cout<<oMapValue.c_str()<<std::endl;

      return oMapValue;
      }
    }
  return oMapValue;
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void GoDBRow::SetValuesForSpecificID(int ID, 
  vtkMySQLDatabase* iDatabaseConnector)
{ 
  std::vector<std::string> ResultQuery = ListSpecificValuesForOneColumn(
    iDatabaseConnector,this->m_TableName, this->PrintColumnNames(),
    this->m_TableIDName, ConvertToString<int>(ID));
  std::vector<std::string>::iterator iterResultQuery = ResultQuery.begin();
  if (this->m_MapRow.size() != ResultQuery.size())
    {
    std::cout<<"pb the map and the query results are not the same size";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    }
  else
    {
    std::map<std::string,std::string>::iterator iterMap = this->MapBegin();
    while ( iterMap != this->MapEnd())
      {
      iterMap->second = *iterResultQuery;
      iterMap++;
      iterResultQuery++;
      }
    }
}


