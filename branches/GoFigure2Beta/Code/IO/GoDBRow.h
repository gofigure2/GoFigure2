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
#ifndef __GoDBRow_h
#define __GoDBRow_h

#include "ConvertToStringHelper.h"
#include "vtkMySQLDatabase.h"
#include <string>
#include <sstream>
#include <map>
#include <vector>

/**
\class GoDBRow
\todo define \code std::map< std::string, std::string > \endcode
*/
class GoDBRow
{
public:

  GoDBRow();
  ~GoDBRow();

  template<typename T>
  void SetField( std::string key, T value )
    {
    m_MapRow[key] = ConvertToString<T>(value);
    }

  /**
  \brief set value as the value of map[key] after having put " at the beginning
  and at the end of the string, as value is a string and it will be needed for
  the database queries */
  void SetField( std::string key, std::string value );

  std::string PrintValues();
  std::string PrintColumnNames();
  std::vector<std::string> GetVectorColumnNames();
  std::map<std::string,std::string>::iterator MapBegin();
  std::map<std::string,std::string>::const_iterator ConstMapBegin();
  std::map<std::string,std::string>::iterator MapEnd();
  std::map<std::string,std::string>::const_iterator ConstMapEnd();

  /**
  \brief return the value for the field map[key] after having removed the "
  at the beginning and at the end of the value if it is a string in order to get
  the original value. */
  std::string GetMapValue (std::string key);

  friend std::ostream& operator << ( std::ostream& os, const GoDBRow& c )
    {
    for( std::map<std::string,std::string>::const_iterator it = c.m_MapRow.begin();
        it != c.m_MapRow.end();
        ++it )
      {
      os <<it->first <<" = " <<it->second <<std::endl;
      }

    return os;
    }

protected:
  virtual void InitializeMap() = 0;
  //virtual void SaveInDB (vtkMySQLDatabase* DatabaseConnector) = 0;
  std::map<std::string,std::string> m_MapRow;

};

#endif

