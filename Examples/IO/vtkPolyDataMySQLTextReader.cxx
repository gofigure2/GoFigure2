#include <string>
#include "vtkPolyDataReader.h"
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

  vtkPolyDataMySQLTextWriter* convert_writer =
    vtkPolyDataMySQLTextWriter::New();
  std::string polydata_string = convert_writer->GetMySQLText( vtk_reader->GetOutput() );

  vtkPolyDataMySQLTextReader* convert_reader =
    vtkPolyDataMySQLTextReader::New();
  convert_reader->SetIsContour( convert_writer->GetIsContour() );
  vtkPolyData* output = convert_reader->GetPolyData( polydata_string );

  vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
  mapper->SetInput( output );

  vtkActor* actor = vtkActor::New();
  actor->SetMapper( mapper );

  vtkRenderer* ren = vtkRenderer::New();
  ren->AddActor( actor );

  vtkRenderWindow* renwin = vtkRenderWindow::New();
  renwin->AddRenderer( ren );

  vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow( renwin );

  ren->Render();
  iren->Start();

  iren->Delete();
  renwin->Delete();
  ren->Delete();
  actor->Delete();
  mapper->Delete();

  output->Delete();
  convert_reader->Delete();
  convert_writer->Delete();
  vtk_reader->Delete();

  return EXIT_SUCCESS;
}

