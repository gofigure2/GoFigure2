#include "vtkLSMReader.h"
#include <iostream>
#include <sstream>
#include "vtkMetaImageWriter.h"
#include "vtkImageAppendComponents.h"
#include "vtkIndent.h"
#include "vtkImageData.h"

int main(int argc,char* argv[])
{
  if (argc != 2)
    {
    std::cout << "Usage: ";
    std::cout << "(exe) FileName";
    std::cout << std::endl;
    return 0;
    }

  //read the LSM file:
  vtkLSMReader* reader=vtkLSMReader::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  int NumberOfTimePoints = reader->GetNumberOfTimePoints();
  int NumberOfChannels = reader->GetNumberOfChannels();

  vtkMetaImageWriter * writer = vtkMetaImageWriter::New();
  writer->SetFileDimensionality( 3 );

  for( int timePoint = 0;timePoint < NumberOfTimePoints; timePoint++)
    {
    for( int channel = 0; channel < NumberOfChannels; channel++)
      {
      vtkLSMReader* treader=vtkLSMReader::New();
      treader->SetFileName(argv[1]);
      treader->SetUpdateTimePoint( timePoint );
      treader->SetUpdateChannel( channel );
      treader->Update();

      std::stringstream namebuffer;
      namebuffer << argv[1];
      namebuffer << "_T_"  << timePoint;
      namebuffer << "_Ch_" << channel;
      namebuffer << ".mha";

      std::cout << namebuffer.str() << std::endl;;

      writer->SetFileName( namebuffer.str().c_str() );
      writer->SetInputConnection( treader->GetOutputPort( ) );
      writer->Write();
      }
    }
  return EXIT_SUCCESS;
}
