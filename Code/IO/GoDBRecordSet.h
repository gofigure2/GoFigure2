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

  // decorate the row type to know if it was modified
  // allows for optimization when synchronising the data
  typedef std::pair< bool , OriginalObjectType > InternalObjectType;

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
    }

  // save content to DB - ASYNCHRONOUS
  bool SaveInDB()
    {
    if( m_RowContainer.size() == 0 ) return true;

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
    // while( start != end )
      {
      // std::cout << (*start).second.PrintValues()  << std::endl;
      // start++;
      }

    // start = m_RowContainer.begin();
    std::sort( start, end, IsLess() );

    this->PopulateColumnNamesContainer();

    std::pair<std::string, std::string> Query = this->SetUpInsertQueryStrings();

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
    if( Query.first != "")
      {
      query->SetQuery( Query.first.c_str());
      if ( !query->Execute() )
        {
        // replace by exception
        std::cerr << "Create query failed" << std::endl;
        DataBaseConnector->Close();
        DataBaseConnector->Delete();
        query->Delete();
        return false;
        }
      }
    if( Query.second != "" )
      {
      query->SetQuery( Query.second.c_str());
      if ( !query->Execute() )
        {
        // replace by exception
        std::cerr << "Create query failed" << std::endl;
        DataBaseConnector->Close();
        DataBaseConnector->Delete();
        query->Delete();
        return false;
        }
      }
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
    return true;
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

  std::pair<std::string, std::string> SetUpInsertQueryStrings();
  std::string ComputeQuery( std::string, myIteratorType start, myIteratorType end );
  void PopulateColumnNamesContainer();

  // colum names container
  std::vector< std::string >        m_ColumnNamesContainer;

  // DB variables
  std::string ServerName;
  std::string DataBaseName;
  std::string TableName;
  std::string User;
  std::string PassWord;
  bool        IsOpen;

};

// this method generates two SQL query strings
// one using the (mysql only) REPLACE command which overwrite existing
// entry with the same primary Key
// another one with the usual INSERT command for new entries.
template< class TObject >
std::pair<std::string, std::string>
GoDBRecordSet<TObject>::
SetUpInsertQueryStrings()
{

  myIteratorType start = m_RowContainer.begin();
  myIteratorType end   = m_RowContainer.begin();
  while( (*end).first && end != m_RowContainer.end() ) end++;

  std::string firstQuery;
  if( end-start > 0 )
    {
    firstQuery = ComputeQuery( "REPLACE ", start, end );
    }
  else
    {
    firstQuery = "";
    }

  start = end;
  end = m_RowContainer.end();

  std::string secondQuery;
  if( end-start > 0 )
    {
    secondQuery = ComputeQuery( "INSERT ", start, end );
    }
  else
    {
    secondQuery = "";
    }

  std::pair< std::string, std::string > result( firstQuery, secondQuery );

  return result;
}


template< class TObject >
std::string
GoDBRecordSet<TObject>::
ComputeQuery( std::string what, myIteratorType start, myIteratorType end )
{
  unsigned int NbOfCol = m_ColumnNamesContainer.size();
  if( NbOfCol == 0 )
    {
    // throw exception
    std::cerr << "Could not extract column names." << std::endl;
    }

  std::stringstream query;

  // main query
  query << what  << "INTO " << this->TableName;

  // column names
  query << " ( ";
  std::vector<std::string>::iterator It = m_ColumnNamesContainer.begin();
  for( unsigned int i = 0; i < NbOfCol-1; i++, It++ )
    {
    query << (*It) << ", ";
    }
  query << (*It);
  query << " ) ";

  // now the values
  query << " VALUES ";
  myIteratorType rowIt = start;
  for( int i = 0; i < end-start-1 && rowIt != end; i++, rowIt++ )
    {
    query << "(" << (*rowIt).second.PrintValues() << "),";
    }
  query << "(" << (*rowIt).second.PrintValues() << ")";

  // and ... voila!
  query << ";";

  // std::cout << query.str() << std::endl;

  return query.str();
}

template< class TObject >
void
GoDBRecordSet<TObject>::
PopulateColumnNamesContainer()
{
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

}
#endif
