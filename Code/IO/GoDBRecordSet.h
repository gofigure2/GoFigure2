#include <algorithm>
#include <utility>
#include <vector>

#include "CreateDataBaseHelper.h"

template< class TObject >
class GoDBRecordSet
{
public:
  // row type
  typedef TObject                                OriginalObjectType;

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
    std::string Query = this->SetUpInsertQueryString();
    vtkMySQLDatabase * DataBaseConnector = vtkMySQLDatabase::New();
    DataBaseConnector->SetHostName(this->ServerName);
    DataBaseConnector->SetUser(this->User);
    DataBaseConnector->SetPassword(this->PassWord);
    DataBaseConnector->SetDatabaseName( this->DataBaseName );
    DataBaseConnector->Open();

    vtkSQLQuery* query = DataBaseConnector->GetQueryInstance();
    query->SetQuery( Query.c_str());
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

  std::string SetUpInsertQueryString()
    {
    std::string whatever( "whatever" );
    return whatever;
    };

  // underlying container
  std::vector< InternalObjectType > m_RowContainer;

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

