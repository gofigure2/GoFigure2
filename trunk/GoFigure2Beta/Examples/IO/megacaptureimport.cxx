#include "itkMegaCaptureImport.h"
#include <iostream>

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cout <<"Usage: ./megacaptureimport(.exe) filename(.jpg)" <<std::endl;
    return EXIT_FAILURE;
    }

  itk::MegaCaptureImport::Pointer importer = itk::MegaCaptureImport::New();
  importer->SetFileName( argv[1] );
  importer->Update();
  GoFigureFileInfoHelperMultiIndexContainer listoffiles = importer->GetOutput();

  for( GoFigureFileInfoHelperMultiIndexContainer::iterator it = listoffiles.begin();
    it != listoffiles.end();
    ++it )
    {
    std::cout <<it->m_Filename <<std::endl;
    }

  return EXIT_SUCCESS;
}
