/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
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
#ifndef __GoDBRecordSet_h
#define __GoDBRecordSet_h

#include <algorithm>
#include <utility>
#include <vector>
#include <string>
#include <sstream>

#include "vtkVariant.h"
#include "CreateDataBaseHelper.h"

#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"

template< class TObject >
class GoDBRecordSet
{
public:
  // row type
  typedef TObject OriginalObjectType;

  GoDBRecordSet()
  { IsWhereStringSet = false; }

  // decorate the row type to know if it was modified
  // allows for optimization when synchronising the data
  typedef std::pair< bool, OriginalObjectType > InternalObjectType;
  typedef std::vector< InternalObjectType >     RowContainerType;

  std::vector< std::string >  GetColumnNamesContainer()
    {
    return m_ColumnNamesContainer;
    }

  RowContainerType* GetRowContainer()
    {
    return &m_RowContainer;
    }

  // Add New Object
  void AddObject( OriginalObjectType& object )
    {
    m_RowContainer.push_back( InternalObjectType( false, object ) );
    }

  // Insert Object
  void InsertObject( const int& pos, OriginalObjectType& object )
    {
    if( pos > m_RowContainer.size() )
      {
      AddObject( object );
      return;
      }
    InternalObjectType& temp =  m_RowContainer[pos];
    m_RowContainer[pos] = InternalObjectType( true, object );
    delete temp;
    }

  void SetWhereString( std::string whereString )
    { this->m_WhereString = whereString; this->IsWhereStringSet = true; } 

  void SetServerName( std::string ServerName )
    { this->ServerName = ServerName; }

  void SetDataBaseName( std::string DataBaseName )
    { this->DataBaseName = DataBaseName; }

  void SetTableName( std::string TableName )
    { this->TableName = TableName; }

  void SetUser( std::string User )
    { this->User = User; }

  void SetPassword( std::string Password )
    { this->PassWord = Password; }

  // read content from DB
  void PopulateFromDB()
    {
    if( !CanConnectToServer(
        this->ServerName,
        this->User,
        this->PassWord )
        /*
        !CanConnectToDatabase(
        this->ServerName,
        this->User,
        this->PassWord,
        this->DataBaseName
        )*/ )
      {
      // throw exception
      return;
      }

    vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
    DataBaseConnector->SetHostName(this->ServerName.c_str());
    DataBaseConnector->SetUser(this->User.c_str());
    DataBaseConnector->SetPassword(this->PassWord.c_str());
    DataBaseConnector->SetDatabaseName( this->DataBaseName.c_str() );

    if (!DataBaseConnector->Open())
      {
      std::cerr << "Can not open DB"  << std::endl;
      DataBaseConnector->Delete();
      return;
      }

    this->PopulateColumnNamesContainer();
    std::stringstream queryString;
    queryString << "SELECT * FROM " << this->TableName;
    if( IsWhereStringSet )
      {
      queryString << " WHERE " << m_WhereString;
      }
    queryString << ";";

    vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
    query->SetQuery( queryString.str().c_str() );
    if ( !query->Execute() )
      {
      // replace by exception
      std::cerr << "Create query failed" << std::endl;
      }
    else
      {
      if( m_RowContainer.size() > 0 )
        {
        m_RowContainer.clear();
        }
      while( query->NextRow() )
        {
        OriginalObjectType object;
        for( unsigned int colID = 0; colID < m_ColumnNamesContainer.size(); colID++ )
          {
          object.SetFieldValueAsString( colID, query->DataValue( colID ).ToString() );
          }
        m_RowContainer.push_back( InternalObjectType( true, object ) );
        }
      }
    DataBaseConnector->Delete();
    query->Delete();
    }

  // save content to DB - ASYNCHRONOUS
  bool SaveInDB()
    {
    if( m_RowContainer.empty() )
      {
      return true;
      }

    if( !CanConnectToServer(
        this->ServerName,
        this->User,
        this->PassWord )
        /*
        !CanConnectToDatabase(
        this->ServerName,
        this->User,
        this->PassWord,
        this->DataBaseName
        )*/ )
      {
      // throw exception
      return false;
      }

    myIteratorType start = m_RowContainer.begin();
    myIteratorType end   = m_RowContainer.end();

    std::sort( start, end, IsLess() );

    this->PopulateColumnNamesContainer();

    vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
    DataBaseConnector->SetHostName(this->ServerName.c_str());
    DataBaseConnector->SetUser(this->User.c_str());
    DataBaseConnector->SetPassword(this->PassWord.c_str());
    DataBaseConnector->SetDatabaseName( this->DataBaseName.c_str() );

    if( !DataBaseConnector->Open() )
      {
      std::cerr << "Could not open database." << std::endl;
      std::cerr << "DB will not be created."  << std::endl;
      DataBaseConnector->Delete();
      return false;
      }

    vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();

    if( this->SaveEachRow( query ) )
      {
      DataBaseConnector->Close();
      DataBaseConnector->Delete();
      query->Delete();
      return true;
      }
    else
      {
      DataBaseConnector->Close();
      DataBaseConnector->Delete();
      query->Delete();
      return false;
      }
    }

private:
  // functor to sort our RowContainer and optimize SQL requests
  class IsLess
    {
    public:
    bool operator()( const InternalObjectType& A, const InternalObjectType& B )
      {
      // Dirty first
      if( A.first && !B.first ) return true;
      return false;
      }
    };

  // underlying container
  typedef typename std::vector< InternalObjectType >::iterator  myIteratorType;
  std::vector< InternalObjectType > m_RowContainer;

  void PopulateColumnNamesContainer();

  bool SaveEachRow( vtkSQLQuery* query );
  bool SaveRows( vtkSQLQuery* query, std::string what, myIteratorType start, myIteratorType end );

  // colum names container
  std::vector< std::string >        m_ColumnNamesContainer;

  // DB variables
  std::string ServerName;
  std::string DataBaseName;
  std::string TableName;
  std::string User;
  std::string PassWord;
  std::string m_WhereString;
  bool        IsWhereStringSet;
  bool        IsOpen;

};

template< class TObject >
bool
GoDBRecordSet<TObject>::
SaveEachRow( vtkSQLQuery *query )
{
  // modified rows
  myIteratorType start = m_RowContainer.begin();
  myIteratorType end   = m_RowContainer.begin();
  while( (*end).first && end != m_RowContainer.end() ) end++;

  // Here we suppose read and write only, no overwrite
#if 0
  if( end-start > 0 )
    {
    if( !SaveRows( query, "REPLACE ", start, end ) )
      {
      return false;
      }
    }
#endif

  // new rows
  start = end;
  end = m_RowContainer.end();
  if( end-start > 0 )
    {
    if( !SaveRows( query, "INSERT ", start, end ) )
      {
      return false;
      }
    }

  return true;
}


template< class TObject >
bool
GoDBRecordSet<TObject>::
SaveRows( vtkSQLQuery * query, std::string what, myIteratorType start, myIteratorType end )
{
  // safe test
  unsigned int NbOfCol = m_ColumnNamesContainer.size();
  if( NbOfCol == 0 )
    {
    // throw exception
    std::cerr << "Could not extract column names." << std::endl;
    return false;
    }

  // invariant part of the query
  std::stringstream queryString;
  queryString << what  << "INTO " << this->TableName;
  queryString << " ( ";
  std::vector<std::string>::iterator It = m_ColumnNamesContainer.begin();
  for( unsigned int i = 0; i < NbOfCol-1; i++, It++ )
    {
    queryString << (*It) << ", ";
    }
  queryString << (*It);
  queryString << " ) ";
  queryString << " VALUES ";

  // row dependent part of the query
  myIteratorType rowIt = start;
  while( rowIt != end )
    {
    std::stringstream rowQueryString;
    rowQueryString << queryString.str();
    rowQueryString << "(" << (*rowIt).second.PrintValues() << ");";
    query->SetQuery( rowQueryString.str().c_str());
    if ( !query->Execute() )
      {
      // replace by exception
      std::cerr << "Save query failed: ";
      std::cerr << rowQueryString.str().c_str() << std::endl;
      return false;
      }
    rowIt++;
    }

  return true;
}

template< class TObject >
void
GoDBRecordSet<TObject>::
PopulateColumnNamesContainer()
{

  if( m_ColumnNamesContainer.size() > 0 )
    {
    m_ColumnNamesContainer.clear();
    }

  vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
  DataBaseConnector->SetHostName(this->ServerName.c_str());
  DataBaseConnector->SetUser(this->User.c_str());
  DataBaseConnector->SetPassword(this->PassWord.c_str());
  DataBaseConnector->SetDatabaseName( this->DataBaseName.c_str() );

  if (!DataBaseConnector->Open())
    {
    std::cerr << "Can not open DB"  << std::endl;
    DataBaseConnector->Delete();
    return;
    }

  vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SHOW COLUMNS FROM ";
  querystream << this->TableName;
  querystream << " FROM ";
  querystream << this->DataBaseName;
  querystream << ";";

  query->SetQuery( querystream.str().c_str() );
  if ( !query->Execute() )
    {
    // replace by exception
    std::cerr << "Create query failed" << std::endl;
    }
  else
    {
    while( query->NextRow() )
      {
      m_ColumnNamesContainer.push_back( query->DataValue( 0 ).ToString() );
      }
    }

  DataBaseConnector->Close();
  DataBaseConnector->Delete();
  query->Delete();

};

#endif
