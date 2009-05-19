#include "GoDBRecordSet.h"

#include "GoDBFigureRow.h"
#include "GoDBExperimentRow.h"
#include "GoDBSeriesGridRow.h"

#include "CreateDataBaseHelper.h"

template< class myT >
void Test( const char* DBName,  const char* TableName )
{
  typedef GoDBRecordSet< myT >   SetType;

  SetType* mySet = new SetType;
  mySet->SetServerName( "localhost" );
  mySet->SetDataBaseName( DBName );
  mySet->SetTableName( TableName );
  mySet->SetUser( "gofigure" );
  mySet->SetPassword( "gofigure" );
  mySet->PopulateFromDB();
  myT myNewObject;
  mySet->AddObject( myNewObject );
  mySet->SaveInDB();

  delete mySet;
  return;
}


int main( int argc, char * argv[] )
{
  std::string ServerName( "localhost" );
  std::string DataBaseName( "MyTestDB" );
  std::string Login( "gofigure" );
  std::string Password( "gofigure" );

  CreateDataBaseMain( ServerName.c_str(), Login.c_str(),
    Password.c_str(), DataBaseName.c_str() );

  std::cout
    <<IsDatabaseOfGoFigureType( ServerName.c_str(), Login.c_str(),
        Password.c_str(), DataBaseName.c_str() )
    << std::endl;

  Test< GoDBExperimentRow >( DataBaseName.c_str(), "experiment" );
  Test< GoDBSeriesGridRow >( DataBaseName.c_str(), "seriesgrid" );
  Test< GoDBFigureRow     >( DataBaseName.c_str(), "figure"     );

  DropDatabase( ServerName.c_str(), Login.c_str(),
    Password.c_str(), DataBaseName.c_str() );

  return 1;
}



