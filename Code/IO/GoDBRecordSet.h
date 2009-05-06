#include <algorithm>
#include <utility>
#include <vector>

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
  void PopulateFromDB() { };

  // save content to DB - ASYNCHRONOUS
  void SaveInDB()
    {
    std::sort( m_RowContainer.begin(), m_RowContainer.end(), IsLess() );
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

