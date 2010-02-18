#include <iostream>
#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"

#include "vtkPolyDataMySQLTextWriter.h"

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cout <<"Usage:" <<std::endl;
    std::cout <<"./vtkPolyDataMySQLTextWriter vtkfile" <<std::endl;
    return EXIT_FAILURE;
    }

  vtkPolyDataReader* reader = vtkPolyDataReader::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  vtkPolyDataMySQLTextWriter* convert = vtkPolyDataMySQLTextWriter::New();
  std::cout <<convert->GetMySQLText( reader->GetOutput() ) <<std::endl;

  convert->Delete();
  reader->Delete();

  return EXIT_SUCCESS;
}

