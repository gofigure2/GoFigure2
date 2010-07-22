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
#ifndef __GoDBRecordSet_h
#define __GoDBRecordSet_h

#include <algorithm>
#include <utility>
#include <vector>
#include <string>
#include <sstream>

#include "vtkVariant.h"
#include "QueryDataBaseHelper.h"

#include "vtkMySQLDatabase.h"
#include "vtkSQLQuery.h"

template<class TObject>
class GoDBRecordSet
  {
public:
  // row type
  typedef TObject OriginalObjectType;

  // decorate the row type to know if it was modified
  // allows for optimization when synchronising the data
  typedef std::pair<bool, OriginalObjectType> InternalObjectType;
  typedef std::vector<InternalObjectType>     RowContainerType;

  GoDBRecordSet() : IsWhereStringSet(false) {}
  ~GoDBRecordSet() {}

  std::vector<std::string>  GetColumnNamesContainer()
  {
    return m_ColumnNamesContainer;
  }

  RowContainerType* GetRowContainer()
  {
    return &m_RowContainer;
  }

  /** \brief Add a new Object of OriginalObjectType (exp: GoDBprojectRow) in
  the m_RowContainer and set the bool to false*/
  void AddObject(OriginalObjectType& object)
  {
    m_RowContainer.push_back(InternalObjectType(false, object));
  }

  /* Insert Object
  note lydie:overwrite the object at the pos position in the
  m_RowContainer (not yet used)wouldn't that be better to call it "ReplaceObject" ?
  if the object has changed, why put the bool to true ??? */
  void InsertObject(const int& pos, OriginalObjectType& object)
  {
    if (pos > m_RowContainer.size())
      {
      AddObject(object);
      return;
      }
    InternalObjectType& temp =  m_RowContainer[pos];
    m_RowContainer[pos] = InternalObjectType(true, object);
    delete temp;
  }

  /** \brief is there to be used in case there is a "WHERE" condition
  to add for the selection in PopulateFromDB()*/
  void SetWhereString(std::string whereString)
  { this->m_WhereString = whereString; this->IsWhereStringSet = true; }

  void SetServerName(std::string iServerName)
  { this->ServerName = iServerName; }

  void SetDataBaseName(std::string iDataBaseName)
  { this->DataBaseName = iDataBaseName; }

  void SetTableName(std::string iTableName)
  { this->TableName = iTableName; }

  void SetUser(std::string iUser)
  { this->User = iUser; }

  void SetPassword(std::string iPassword)
  { this->PassWord = iPassword; }

  void SetConnector(vtkMySQLDatabase * iDatabaseConnector)
  { this->m_DatabaseConnector = iDatabaseConnector;}

  /** \brief help read content from DB: select all the fields for a given table
  (TableName)in the database and fills the m_RowContainer with the results:
  each row from the database will fill an InternalObjectType with true and an
  OriginalObjectType (exp; GoProjectRow).
  if there is a need to add a condition on the selection ( add a WHERE), have
  to use the function SetWhereString( std::string whereString ) from the same
  class. */
  void PopulateFromDB()
  {
    this->PopulateColumnNamesContainer();

    std::stringstream queryString;
    queryString << "SELECT * FROM " << this->TableName;
    if (IsWhereStringSet)
      {
      queryString << " WHERE " << m_WhereString;
      }
    queryString << ";";

    vtkSQLQuery* query = m_DatabaseConnector->GetQueryInstance();
    query->SetQuery(queryString.str().c_str());
    if (!query->Execute())
      {
      // replace by exception
      std::cerr << "Create query failed" << std::endl;
      }
    else
      {
      if (m_RowContainer.size() > 0)
        {
        m_RowContainer.clear();
        }

      //here the m_RowContainer is filled with all the results of the previous query:
      //SELECT * FROM table:
      while (query->NextRow())
        {
        OriginalObjectType object;
        for (unsigned int colID = 0; colID < m_ColumnNamesContainer.size(); colID++)
          {
          std::string ColumnName = m_ColumnNamesContainer[colID];
          object.SetField(ColumnName, query->DataValue(colID).ToString());
          }
        m_RowContainer.push_back(InternalObjectType(true, object));
        }
      }
    //DataBaseConnector->Delete();
    query->Delete();
  }

  // save content to DB - ASYNCHRONOUS
  bool SaveInDB(bool Update = false)
  {
    if (m_RowContainer.empty())
      {
      return true;
      }

    myIteratorType start = m_RowContainer.begin();
    myIteratorType end   = m_RowContainer.end();

    std::sort(start, end, IsLess());

    this->PopulateColumnNamesContainer();

    vtkSQLQuery* query = m_DatabaseConnector->GetQueryInstance();

    if (Update)
      {
      if (this->SaveEachRow(query, true))
        {
        query->Delete();
        return true;
        }
      else
        {
        query->Delete();
        return false;
        }
      }
    else
      {
      if (this->SaveEachRow(query, false))
        {
        query->Delete();
        return true;
        }
      else
        {
        query->Delete();
        return false;
        }
      }
  }

private:
  /** functor to sort our RowContainer and optimize SQL requests
  \todo Why not do the opposite: false first and true later?
  Like this, we won't have to iterate on all the true first!
  Well, it needs to be more investigated!!!*/
  struct IsLess
    {
    bool operator ()(const InternalObjectType& A, const InternalObjectType& B)
    {
      // Dirty first
      if (A.first && !B.first)
        {
        return true;
        }
      return false;
    }
    };

  // underlying container
  typedef typename std::vector<InternalObjectType>::iterator myIteratorType;
  std::vector<InternalObjectType> m_RowContainer;

  void PopulateColumnNamesContainer();

  bool SaveEachRow(vtkSQLQuery* query, bool Update = false);
  bool SaveRows(vtkSQLQuery* query, std::string what, myIteratorType start, myIteratorType end);
  bool UpdateRows(vtkSQLQuery * query, myIteratorType start, myIteratorType end);

  // colum names container
  std::vector<std::string> m_ColumnNamesContainer;

  // DB variables
  vtkMySQLDatabase* m_DatabaseConnector;
  std::string       ServerName;
  std::string       DataBaseName;
  std::string       TableName;
  std::string       User;
  std::string       PassWord;
  std::string       m_WhereString;
  bool              IsWhereStringSet;
  bool              IsOpen;

  };

template<class TObject>
bool
GoDBRecordSet<TObject>::
SaveEachRow(vtkSQLQuery *query, bool Update)
{
  (void) Update;

  // modified rows
  myIteratorType start = m_RowContainer.begin();
  myIteratorType firstFalseElement = start;
  myIteratorType end = m_RowContainer.end();

  while ((*firstFalseElement).first && firstFalseElement  != end)
    {
    firstFalseElement++;
    }

  // Here we suppose read and write only, no overwrite
  //if( end-start > 0 )
  //  {
  //  if( !SaveRows( query, "REPLACE ", start, end ) )
  //    {
  //    return false;
  //    }
  //  }

  // new rows
  if (end - firstFalseElement > 0)
    {
    if (!SaveRows(query, "INSERT ", firstFalseElement, end))
      {
      if (!UpdateRows(query, firstFalseElement, end))
        {
        return false;
        }
      }
    else
      {
      if (!SaveRows(query, "INSERT ", firstFalseElement, end))
        {
        return false;
        }
      }
    }

  return true;
}

/**\brief uses the INSERT or REPLACE query to save all the objects GoDB..Row currently
in the m_RowContainer located between start and end */
template<class TObject>
bool
GoDBRecordSet<TObject>::
SaveRows(vtkSQLQuery * query, std::string what, myIteratorType start, myIteratorType end)
{
  // safe test
  /*unsigned int NbOfCol = m_ColumnNamesContainer.size();
  if( NbOfCol == 0 )
    {
    // throw exception
    std::cerr << "Could not extract column names." << std::endl;
    return false;
    }*/

  // invariant part of the query: corresponds to the insert/replace into table
  //(names of all the columns):
  myIteratorType rowIt = start;

  std::stringstream queryString;
  queryString << what  << "INTO " << this->TableName;
  queryString << " ( ";
  queryString << rowIt->second.PrintColumnNames();
  queryString << " ) ";
  queryString << " VALUES ";

  // row dependent part of the query: one row corresponds to the values of one
  //OriginalObjectType (exp:GoProjectRow)to be saved into the Database. So this part
  //saves the values for all the OriginalObjectType contained in the vector m_RowContainer:
  while (rowIt != end)
    {
    std::stringstream rowQueryString;
    rowQueryString << queryString.str();
    rowQueryString << "(";
    rowQueryString << rowIt->second.PrintValues();
    rowQueryString << ");";
    query->SetQuery(rowQueryString.str().c_str());
    if (!query->Execute())
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

/**\brief uses the INSERT or REPLACE query to save all the objects GoDB..Row currently
in the m_RowContainer located between start and end */
template<class TObject>
bool
GoDBRecordSet<TObject>::
UpdateRows(vtkSQLQuery * query, myIteratorType start, myIteratorType end)
{
  myIteratorType rowIt = start;

  std::stringstream queryString;
  queryString << "UPDATE ";
  queryString << this->TableName << " SET ";
  queryString << rowIt->second.PrintColumnNamesWithValues();
  queryString << " WHERE ";
  queryString << rowIt->second.GetTableIDName();
  queryString << " = ";
  queryString << rowIt->second.GetMapValue(rowIt->second.GetTableIDName());

  // row dependent part of the query: one row corresponds to the values of one
  //OriginalObjectType (exp:GoProjectRow)to be saved into the Database. So this part
  //saves the values for all the OriginalObjectType contained in the vector m_RowContainer:
  while (rowIt != end)
    {
    queryString << ";";
    query->SetQuery(queryString.str().c_str());
    if (!query->Execute())
      {
      // replace by exception
      std::cerr << "Save query failed: ";
      std::cerr << queryString.str().c_str() << std::endl;
      return false;
      }
    rowIt++;
    }
  return true;
}

/** \brief fills the vector m_ColumnNamesContainer with the column names gotten from
the database and in the same order as the query results will be given:(only way
to retrieve which query->datavalue corresponds to which field). */
template<class TObject>
void
GoDBRecordSet<TObject>::
PopulateColumnNamesContainer()
{
  if (m_ColumnNamesContainer.size() > 0)
    {
    m_ColumnNamesContainer.clear();
    }

  vtkSQLQuery*      query = m_DatabaseConnector->GetQueryInstance();
  std::stringstream querystream;
  querystream << "SHOW COLUMNS FROM ";
  querystream << this->TableName;
  //querystream << " FROM ";
  //querystream << this->DataBaseName;
  querystream << ";";

  query->SetQuery(querystream.str().c_str());
  if (!query->Execute())
    {
    // replace by exception
    std::cerr << "Create query failed" << std::endl;
    }
  else
    {
    while (query->NextRow())
      {
      m_ColumnNamesContainer.push_back(query->DataValue(0).ToString());
      }
    }
  query->Delete();

}

#endif
