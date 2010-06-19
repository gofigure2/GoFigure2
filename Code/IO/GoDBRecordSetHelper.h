/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
  Date: $Date$  // Date of last commit
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
#ifndef __GoDBRecordSetHelper_h
#define __GoDBRecordSetHelper_h

#include "GoDBRecordSet.h"
#include "QueryDataBaseHelper.h"
#include "SelectQueryDatabaseHelper.h"

/** \brief help add a new object of type T in the table "TableName"
in the database: add a new ImagingSession, new Image, new Mesh...
\param[in] ServerName
\param[in] User
\param[in] Password
\param[in] DBName
\param[in] TableName
\param[in] myNewObject
*/
template< class T >
void AddOnlyOneNewObjectInTable( vtkMySQLDatabase* DatabaseConnector,
  const std::string& TableName, T& myNewObject )
{
  typedef GoDBRecordSet< T >   SetType;

  SetType mySet;
  mySet.SetConnector( DatabaseConnector );
  mySet.SetTableName( TableName );
  mySet.AddObject( myNewObject );
  mySet.SaveInDB();
}

template< class T >
int AddOnlyOneNewObjectInTable( vtkMySQLDatabase* DatabaseConnector,
  const std::string& TableName,T& myNewObject, const std::string IDColumnName )
{
  typedef GoDBRecordSet< T >   SetType;

  SetType mySet;
  mySet.SetConnector( DatabaseConnector );
  mySet.SetTableName( TableName );
  mySet.AddObject( myNewObject );
  mySet.SaveInDB();

  return MaxValueForOneColumnInTable(DatabaseConnector,IDColumnName,TableName );
}

template< class T >
int AddOnlyOneNewObjectInTable( vtkMySQLDatabase* DatabaseConnector,
  const std::string& TableName,T* myNewObject, const std::string IDColumnName )
{
  typedef GoDBRecordSet< T >   SetType;

  SetType mySet;
  mySet.SetConnector( DatabaseConnector );
  mySet.SetTableName( TableName );
  mySet.AddObject( *myNewObject );
  mySet.SaveInDB();

  return MaxValueForOneColumnInTable(DatabaseConnector,IDColumnName,TableName );
}

template< class T >
int UpdateOneNewObjectInTable(vtkMySQLDatabase* DatabaseConnector,
  T* myNewObject)
{
  typedef GoDBRecordSet< T >   SetType;

  SetType mySet;
  mySet.SetConnector( DatabaseConnector );
  mySet.SetTableName( myNewObject->GetTableName() );
  mySet.AddObject( *myNewObject );
  mySet.SaveInDB(true);

  return atoi(myNewObject->GetMapValue(myNewObject->GetTableIDName()).c_str());
}

#endif
