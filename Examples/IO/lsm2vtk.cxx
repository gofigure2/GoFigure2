#include "vtkLSMReader.h"
#include <iostream>
#include <sstream>
#include "vtkImageWriter.h"
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

  vtkImageWriter * writer = vtkImageWriter::New();
  writer->SetFileDimensionality( 3 );

  for( int timePoint = 0;timePoint < NumberOfTimePoints; timePoint++)
    {
    vtkImageData* myImage_ch1;
    vtkImageData* myImage_ch2;
    vtkImageData* myImage_ch3;
		
	{
	vtkLSMReader* treader=vtkLSMReader::New();
	treader->SetFileName(argv[1]);
	treader->SetUpdateTimePoint( timePoint );
	treader->SetUpdateChannel( 0 );
	treader->Update();
	myImage_ch1 = treader->GetOutput();
	}
		
	{
	vtkLSMReader* treader=vtkLSMReader::New();
	treader->SetFileName(argv[1]);
	treader->SetUpdateTimePoint( timePoint );
	treader->SetUpdateChannel( 1 );
	treader->Update();
	myImage_ch2 = treader->GetOutput();
	}
		
	{
	vtkLSMReader* treader=vtkLSMReader::New();
	treader->SetFileName(argv[1]);
	treader->SetUpdateTimePoint( timePoint );
	treader->SetUpdateChannel( 2 );
	treader->Update();
	std::cout << "Third reader done." << std::endl;
	int * dimensions = treader->GetDimensions();
	int  flatindex = dimensions[0] * dimensions[1] * dimensions[2];
	myImage_ch3 = treader->GetOutput();
	if( NumberOfChannels == 2 ) // dummy third channel 
	{
	// here we suppose the type to be char
	// to be improved
	char *ptr = (char*)( myImage_ch3->GetScalarPointer());
	for( int k=0; k < flatindex; k++ )
	{
	*ptr++ = 0; 
	}            
	}
	std::cout << "third channel ready." << std::endl;
        }
	
	vtkImageAppendComponents* appendFilter1 = vtkImageAppendComponents::New();
	appendFilter1->AddInput( myImage_ch1 );
	appendFilter1->AddInput( myImage_ch2 );
	appendFilter1->Update();
	std::cout << "Two first channel composed." << std::endl;
	
	vtkImageAppendComponents* appendFilter2 = vtkImageAppendComponents::New();
	appendFilter2->AddInput( appendFilter1->GetOutput() );
	appendFilter2->AddInput( myImage_ch3 );
	appendFilter2->Update(); 
	std::cout << "Third channel composed." << std::endl;
	
	std::stringstream namebuffer;
	namebuffer << argv[1];
	namebuffer << "_T_"  << timePoint;
	namebuffer << ".vtk";

	std::cout << namebuffer.str() << std::endl;;

	writer->SetFileName( namebuffer.str().c_str() );
	writer->SetInputConnection( appendFilter2->GetOutputPort( ) );
	writer->Write();
    }
  return EXIT_SUCCESS;
}
