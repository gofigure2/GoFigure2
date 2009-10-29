#include "vtkLSMReader.h"
#include "vtkViewImage3D.h"

int main( intarc, char *argv )
{
  //read the LSM file - can be multiple channels and time-points:
  vtkLSMReader* reader=vtkLSMReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  // Get the number of time-points
  int NumberOfTimePoints = reader->GetNumberOfTimePoints();

  // Get the number of channels
  int NumberOfChannels = reader->GetNumberOfChannels();

  // Print out the number of time points and channels
  std::cout << NumberOfTimePoints << std::endl;
  std::cout << NumberOfChannels << std::endl;



return EXIT_SUCCESS;
}