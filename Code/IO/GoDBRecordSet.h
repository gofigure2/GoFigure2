#include <algorithm>
#include <utility>
#include <vector>
#include <string>
#include <sstream>

#include "CreateDataBaseHelper.h"

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
    };

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
    };

  void SetServerName( const char* ServerName )
    { this->ServerName = ServerName; };

  void SetDataBaseName( const char* DataBaseName )
    { this->DataBaseName = DataBaseName; };

  void SetTableName( const char* TableName )
    { this->TableName = TableName; };

  void SetUser( const char* User )
    { this->User = User; };

  void SetPassword( const char* Password )
    { this->PassWord = Password; };

  // read content from DB
  void PopulateFromDB()
    { 
    if( !CanConnectToDatabase(
        this->ServerName,
        this->User,
        this->PassWord,
        this->DataBaseName
        ) ) 
      {
      // throw exception
      return;
      }
    };

  // save content to DB - ASYNCHRONOUS
  void SaveInDB()
    {
    std::sort( m_RowContainer.begin(), m_RowContainer.end(), IsLess() );
    if( !CanConnectToDatabase(
        this->ServerName,
        this->User,
        this->PassWord,
        this->DataBaseName
        ) ) 
      {
      // throw exception
      return;
      }
    std::pair<std::string, std::string> Query = this->SetUpInsertQueryStrings();
    vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
    DataBaseConnector->SetHostName(this->ServerName);
    DataBaseConnector->SetUser(this->User);
    DataBaseConnector->SetPassword(this->PassWord);
    DataBaseConnector->SetDatabaseName( this->DataBaseName );
    DataBaseConnector->Open();

    vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
    query->SetQuery( Query.first.c_str());
    if ( !query->Execute() )
      {
      // replace by exception
      std::cerr << "Create query failed" << std::endl;
      }
    query->SetQuery( Query.second.c_str());
    if ( !query->Execute() )
      {
      // replace by exception
      std::cerr << "Create query failed" << std::endl;
      }
    DataBaseConnector->Close();
    DataBaseConnector->Delete();
    query->Delete();
  };

private:
  // functor to sort our RowContainer and optimize SQL requests
  class IsLess
    {
    public:
    bool operator()( const InternalObjectType& A, const InternalObjectType& B )
      {
      // Dirty first
      if( A.first || !B.first ) return true;
      return false;
      };
    };

  // underlying container
  typedef typename std::vector< InternalObjectType >::iterator  myIteratorType;
  std::vector< InternalObjectType > m_RowContainer;

  std::pair<std::string, std::string> SetUpInsertQueryStrings();
  std::string ComputeQuery( std::string, myIteratorType start, myIteratorType end );

  // colum names container
  std::vector< std::string >        m_ColumnNamesContainer;

  // DB variables
  const char* ServerName;
  const char* DataBaseName;
  const char* TableName;
  const char* User;
  const char* PassWord; 
  bool  IsOpen;

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
  while( (*end).first || end != m_RowContainer.end() ) end++;

  std::string firstQuery = ComputeQuery( "REPLACE ", start, end );
  
  start = end;
  end = m_RowContainer.end();
 
  std::string secondQuery = ComputeQuery( "INSERT ", start, end );;
  
  std::pair< std::string, std::string > result( firstQuery, secondQuery );

  return result;
};


template< class TObject >
std::string 
GoDBRecordSet<TObject>::
ComputeQuery( std::string what, myIteratorType start, myIteratorType end )
{
  std::stringstream query;

  // main query 
  query << what  << "INTO " << this->TableName;

  // column names
  query << " ( ";
  unsigned int NbOfCol = m_ColumnNamesContainer.size();
  std::vector<std::string>::iterator It = m_ColumnNamesContainer.begin();
  for( unsigned int i = 0; i < NbOfCol-1 ; i++, It++ )
    {
    query << (*It) << ", ";
    }
  query << (*It);
  query << " ) ";

  // now the values
  query << " VALUES ";
  myIteratorType rowIt = start;
  for( int i = 0; i < end-start-1, rowIt != end; i++, rowIt++ )
    {
    query << "(" << (*rowIt).second.PrintValues() << "),";
    } 
  query << "(" << (*rowIt).second.PrintValues() << ")";

  // and ... voila!
  query << ";";
  
  return query.str(); 
};


