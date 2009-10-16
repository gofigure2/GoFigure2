#include "itkMegaCaptureImport.h"
#include <iostream>

int main( int argc, char** argv )
{
  if( argc != 2 )
    {
    std::cout <<"Usage: ./megacaptureimport(.exe) filename" <<std::endl;
    return EXIT_FAILURE;
    }

  itk::MegaCaptureImport::Pointer importer = itk::MegaCaptureImport::New();
  importer->SetFileName( argv[1] );
  importer->Update();
  FileListType* listoffiles = importer->GetOutput();

  for( FileListType::iterator it = listoffiles->begin();
    it != listoffiles->end();
    ++it )
    {
    std::cout <<it->m_Filename <<std::endl;
    }

  return EXIT_SUCCESS;
}
