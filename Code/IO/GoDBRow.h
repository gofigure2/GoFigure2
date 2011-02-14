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

#ifndef __GoDBRow_h
#define __GoDBRow_h

#include "ConvertToStringHelper.h"
#include "vtkMySQLDatabase.h"
#include <string>
#include <sstream>
#include <map>
#include <vector>

#include "QGoIOConfigure.h"
#include "SelectQueryDatabaseHelper.h"

/**
\class GoDBRow
\brief abstract class manages a map with keys matching fields of a gofiguredatabase table
and values of the map matching a row of a gofiguredatabase table
\ingroup DB
*/
class QGOIO_EXPORT GoDBRow
{
public:

  typedef std::map< std::string, std::string > StringMapType;
  typedef StringMapType::iterator              StringMapIterator;
  typedef StringMapType::const_iterator        StringMapConstIterator;

  GoDBRow();
  virtual ~GoDBRow();

  /** 
  \brief convert the value into a string and assign it to the key in the map
  \param[in] key key of the map
  \param[in] value value to be assigned to key
  \tparam T type of the value, anything but string
  */
  template< typename T >
  void SetField(std::string key, T value)
  {
    StringMapIterator it = m_MapRow.find(key);

    if ( it != m_MapRow.end() )
      {
      it->second = ConvertToString< T >(value);
      }
    else
      {
      std::cerr << "This field does not exist!!!" << std::endl;
      }
  }

  /**
  \brief set value as the value of map[key] after having put " at the beginning
  and at the end of the string, as value is a string and it will be needed for
  the database queries. map[key] = " "value" "
  \param[in] key key of the map
  \param[in] value value to be assigned to key which is a string
  */
  void SetField(std::string key, std::string value);

  /**
  \brief
  \return the table name
  */
  std::string GetTableName();

  /**
  \brief
  \return the tableID name
  */
  std::string GetTableIDName();

  /**
  \brief put all the values of the map in a string separated by ','
  \return all the map values separated by ',' but without a ',' at the end of the string
  */
  std::string PrintValues();

  /**
  \brief put all the keys of the map in a string separated by ','
  \return all the map keys separated by ',' but without a ',' at the end of the string
  */
  std::string PrintColumnNames();

  /**
  \brief put all the keys of the map in a vector
  \return all the map keys in a vector
  */
  std::vector< std::string > GetVectorColumnNames();
  
  /**
  \brief put all the keys and values of the map in a string as map[key] = value separated by ','
  \return all the map [key] = value separated by ',' but without a ',' at the end of the string
  */
  std::string PrintColumnNamesWithValues();

  /**
  \brief
  \return the iterator at the beginning of the class map as a const
  */
  StringMapConstIterator ConstMapBegin();

  /**
  \brief
  \return the iterator at the end of the class map as a const
  */
  StringMapConstIterator ConstMapEnd();

  /**
  \brief
  \return the iterator at the beginning of the class map
  */
  StringMapIterator MapBegin();

  /**
  \brief
  \return the iterator at the end of the class map 
  */
  StringMapIterator MapEnd();

  /**
  \brief return the value for the field map[key] after having removed the "
  at the beginning and at the end of the value if it is a string in order to get
  the original value. 
  \param[in] key key of the map for which the value is needed
  \return the corresponding value without "" if the value was a string 
  */
  std::string GetMapValue(std::string key);

  /**
  \brief print the keys and values of the map in a cout
  \param[in,out] os 
  \param[in,out] c the row to print
  */
  friend std::ostream & operator<<(std::ostream & os, const GoDBRow & c)
  {
    for ( StringMapConstIterator it = c.m_MapRow.begin();
          it != c.m_MapRow.end();
          ++it )
      {
      os << it->first << " = " << it->second << std::endl;
      }

    return os;
  }

  /**
  \brief get the data from the database corresponding to the specific ID and 
  put them in the map
  \param[in] ID ID for which the data are needed
  \param[in] iDatabaseConnector connection to the database
  \return false if the ID hasn't been found in the database
  */
  virtual bool SetValuesForSpecificID(int ID, vtkMySQLDatabase *iDatabaseConnector);

protected:

  /**
  \brief virtual pure. initialize all the values of the map
  */
  virtual void InitializeMap() = 0;

  /**
  \brief add as an element of ioFieldWithValue the name and value of the map with the key
  iNameOfField
  \param[in] iNameOfField key of the map
  \param[in,out] ioFieldWithValue vector of FieldWithValue
  */
  void AddConditions(std::string iNameOfField, std::vector<FieldWithValue> &ioFieldWithValue);

  StringMapType m_MapRow;
  std::string   m_TableName;
  std::string   m_TableIDName;

};

#endif
