#include "GoDBRecordSet.h"
#include "GoDBFigureRow.h"
#include "CreateDataBaseHelper.h"

template< class myT >
void Test( char* DBName, char* TableName )
{
  typedef GoDBRecordSet< myT >   SetType;

  CreateDataBaseMain( "localhost", "gofigure", "gofigure", DBName );

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
  Test< GoDBFigureRow >( "myTestDB", "figure" );
  return 1;
}



