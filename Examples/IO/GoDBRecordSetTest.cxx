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
  CreateDataBaseMain( "localhost", "gofigure", "gofigure", "MyTestDB" );
  std::cout << 
    IsDatabaseOfGoFigureType( "localhost", "gofigure", "gofigure", "MyTestDB")
	<< std::endl;

  Test< GoDBExperimentRow >( "myTestDB", "experiment" );
  Test< GoDBSeriesGridRow >( "myTestDB", "seriesgrid" );
  Test< GoDBFigureRow     >( "myTestDB", "figure"     );

  //DropDatabase( "localhost", "gofigure", "gofigure", "MyTestDB" );

  return 1;
}



