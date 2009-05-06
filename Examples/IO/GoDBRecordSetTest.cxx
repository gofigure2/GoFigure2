#include "GoDBRecordSet.h"
#include "GoDBFigureRow.h"


template< class myT >
void Test( char* DBName, char* TableName )
{
  typedef GoDBRecordSet< myT >   SetType;

  SetType* mySet = new SetType;
  mySet->SetServerName( "localhost" );
  mySet->SetDataBaseName( DBName ); 
  mySet->SetTableName( TableName );
  mySet->SetUser( "gofigure" );
  mySet->SetPassword( "gofigure" );
  mySet->PopulateFromDB();
  // do stuff
  mySet->SaveInDB();

  delete mySet;
  return;
}


int main( int argc, char * argv[] )
{
  Test< GoDBFigureRow >( "myTestDB", "figure" );
  return 1;
}



