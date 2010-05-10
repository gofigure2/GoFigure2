#include "GoDBMeshRow.h"

int main( int argc, char** argv )
{
  (void) argc;
  (void) argv;

  GoDBMeshRow row;
  std::cout <<"std::cout TEST... " <<std::endl;
  std::cout <<row <<std::endl;
  std::cout <<"DONE!" <<std::endl;
  std::cout <<std::endl;

  std::cout <<"GetTableName TEST... " <<std::endl; 
  std::cout <<row.GetTableName() <<std::endl;
  std::cout <<"DONE!" <<std::endl;

  std::cout <<"GetTableIDName TEST... " <<std::endl;
  std::cout <<row.GetTableIDName() <<std::endl;
  std::cout <<"DONE!" <<std::endl;
  std::cout <<std::endl;

  std::cout <<"PrintValues TEST... " <<std::endl;
  std::cout <<row.PrintValues() <<std::endl;
  std::cout <<"DONE!" <<std::endl;
  std::cout <<std::endl;

  std::cout <<"PrintColumnNames TEST... " <<std::endl;
  std::cout <<row.PrintColumnNames() <<std::endl;
  std::cout <<"DONE!" <<std::endl;
  std::cout <<std::endl;


  std::cout <<"iterator TEST... " <<std::endl;
  std::map< std::string, std::string >::iterator it = row.MapBegin();
  while( it != row.MapEnd() )
    {
    ++it;
    }
  std::cout <<"DONE!" <<std::endl;
  std::cout <<std::endl;


  std::cout <<"const iterator TEST... " <<std::endl;
  std::map< std::string, std::string >::const_iterator 
    c_it = row.ConstMapBegin();

  while( c_it != row.ConstMapEnd() )
    {
    ++c_it;
    }

  std::cout <<"DONE!" <<std::endl;
  std::cout <<std::endl;

  std::cout <<"erroneous GetMapValue TEST... " <<std::endl;
  std::cout <<row.GetMapValue( "contour" ) <<std::endl;
  std::cout <<"DONE!" <<std::endl;
  std::cout <<std::endl;


  std::cout <<"GetMapValue TEST... " <<std::endl;
  std::cout <<row.GetMapValue( "CellTypeID" ) <<std::endl;
  std::cout <<"DONE!" <<std::endl;
  std::cout <<std::endl;


  return EXIT_SUCCESS;
}

