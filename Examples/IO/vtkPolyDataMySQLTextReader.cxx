#include <string>
#include "vtkPolyDataReader.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMySQLTextWriter.h"
#include "vtkPolyDataMySQLTextReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cout <<"Usage:" <<std::endl;
    std::cout <<"./vtkPolyDataMySQLTextReader vtkfile" <<std::endl;
    return EXIT_FAILURE;
    }

  vtkPolyDataReader* vtk_reader = vtkPolyDataReader::New();
  vtk_reader->SetFileName( argv[1] );
  vtk_reader->Update();

  vtkPolyData* input = vtk_reader->GetOutput();

  vtkPolyDataMySQLTextWriter* convert_writer =
    vtkPolyDataMySQLTextWriter::New();
  std::string polydata_string = convert_writer->GetMySQLText( input );

  std::cout <<polydata_string <<std::endl;

  vtkPolyDataMySQLTextReader* convert_reader =
    vtkPolyDataMySQLTextReader::New();
  convert_reader->SetIsContour( convert_writer->GetIsContour() );
  vtkPolyData* output = convert_reader->GetPolyData( polydata_string );

  if( output->GetNumberOfPoints() != input->GetNumberOfPoints() )
    {
    std::cout <<"Number of points have changed!!!" <<std::endl;
    std::cout <<"output->GetNumberOfPoints() " <<output->GetNumberOfPoints()
      <<std::endl;
    std::cout <<"input->GetNumberOfPoints() " <<input->GetNumberOfPoints()
      <<std::endl;

    output->Delete();
    convert_reader->Delete();
    convert_writer->Delete();
    vtk_reader->Delete();

    return EXIT_FAILURE;
    }

  output->Delete();
  convert_reader->Delete();
  convert_writer->Delete();
  vtk_reader->Delete();

  return EXIT_SUCCESS;
}

