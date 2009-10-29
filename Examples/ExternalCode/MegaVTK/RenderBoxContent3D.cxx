#include "vtkLSMReader.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkViewImage3D.h"

int main( int argc, char *argv[] )
{
  //read the LSM file - can be multiple channels and time-points:
  vtkLSMReader* reader=vtkLSMReader::New();
  reader->SetFileName( argv[1]);
  reader->Update();

  // Get the number of time-points
  int NumberOfTimePoints = reader->GetNumberOfTimePoints();

  // Get the number of channels
  int NumberOfChannels = reader->GetNumberOfChannels();

  // Print out the number of time points and channels
  std::cout << NumberOfTimePoints << std::endl;
  std::cout << NumberOfChannels << std::endl;

  // Create a rendering window
  vtkRenderWindow *renWin = vtkRenderWindow::New();

  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
  iren->SetRenderWindow ( renWin );

  vtkViewImage3D* viewer = vtkViewImage3D::New();
  viewer->SetInput( reader->GetOutput() );
  viewer->SetRenderWindow( renWin );
  viewer->SetupInteractor( iren );
  viewer->SetTriPlanarRenderingOff();
  viewer->SetVolumeRenderingOn();
  viewer->Render();

  iren->Start();

  viewer->Delete();
  iren->Delete();
  renWin->Delete();

return EXIT_SUCCESS;
}
