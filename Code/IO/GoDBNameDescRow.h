/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

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
#ifndef __GoDBNameDescRow_h
#define __GoDBNameDescRow_h

#include "GoDBRow.h"
#include "GoDBRecordSetHelper.h"
#include "vtkMySQLDatabase.h"

/**
\class GoDBNameDescRow
\brief Abstract. manages the map with the keys matching the fields for the
DBtable containing a name and description fields.
*/
class QGOIO_EXPORT GoDBNameDescRow:public GoDBRow
{
public:
  GoDBNameDescRow();

  ~GoDBNameDescRow()
  {}
  /**
  \brief Pure Virtual :check if the entity already exits in the DB, if yes,
  return the existing ID, if not, save it in the DB and return the
  ID for the new created entity
  \return int existing or new created ID for the entity
  */
  virtual int SaveInDB(vtkMySQLDatabase *iDatabaseConnector) = 0;

  /**
  \brief Pure Virtual :check if the entity already exists in the database
  based on its own uniqueness definition, return the ID of the
  entity already existing or -1 if not yet created.
  \return int existing ID or -1 if the entity doesn't exists
  */
  virtual int DoesThisEntityAlreadyExists(
    vtkMySQLDatabase *iDatabaseConnector) = 0;

  /**
  \brief check if the entity already exists in the database
  based on its own uniqueness definition, return the ID of the
  entity already exiting or -1 if not yet created and change the
  ioName with the name of the existing entity
  \param[in|out] ioName modified if the entity already exists with the
  name of the existing entity
  \return int existing ID or -1 if the entity doesn't exists
 */
  virtual int DoesThisEntityAlreadyExists2(
    vtkMySQLDatabase *iDatabaseConnector, std::string & ioName);

  /*
  \brief check if the entity already exists in the database
  based on its own uniqueness definition, return the ID of the
  entity already exiting or -1 if not yet created and change the
  ioName with the name of the existing entity
  \param[in|out] ioName modified if the entity already exists with the
  name of the existing entity
  \return int existing ID or -1 if the entity doesn't exists
 */
  //virtual int DoesThisEntityAlreadyExists(
  // vtkMySQLDatabase* iDatabaseConnector, std::string& ioName) = 0;

  /**
  \brief check if the name already exits in the database, if yes,
  return the corresponding ID, if not -1
  \return int return the ID of the existing entity with the same name
  or -1 if no entity has the same name
  */
  virtual int DoesThisNameAlreadyExists(vtkMySQLDatabase *iDatabaseConnector);

protected:
  //mother class method
  virtual void InitializeMap();

  /**
  \brief check if the GoDBNameDescRow already exists, if yes, return the ID of the
  existing one, if not, save it in the database and return the new ID
  \param[in] iDatabaseConnector connection to the database
  \tparam iNewEntity children of NameDescRow
  */
  template< typename T >
  int SaveInDBTemplate(vtkMySQLDatabase *iDatabaseConnector, T iNewEntity)
  {
    int NewEntityID = iNewEntity.DoesThisEntityAlreadyExists(iDatabaseConnector);

    if ( NewEntityID == -1 )
      {
      NewEntityID =
        AddOnlyOneNewObjectInTable< T >(iDatabaseConnector,
                                        iNewEntity.m_TableName,
                                        iNewEntity, iNewEntity.m_TableIDName);
      }
    return NewEntityID;
  }
};
#endif
