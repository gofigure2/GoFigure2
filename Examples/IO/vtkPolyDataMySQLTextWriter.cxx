#include <iostream>
#include "vtkPolyDataReader.h"
#include "vtkPolyData.h"

#include "vtkPolyDataMySQLTextWriter.h"

int main( int argc, char** argv )
{
  vtkPolyDataReader* reader = vtkPolyDataReader::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  vtkPolyDataMYSQLTextWriter* convert = vtkPolyDataMYSQLTextWriter::New();
  std::cout <<convert->GetMySQLText( reader->GetOutput() ) <<std::endl;

  convert->Delete();
  reader->Delete();

  return EXIT_SUCCESS;
}

