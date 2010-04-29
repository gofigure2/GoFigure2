#include "vtkLSMReader.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include "vtkMetaImageWriter.h"
#include "vtkImageAppendComponents.h"
#include "vtkIndent.h"
#include "vtkImageData.h"
using namespace std;

// Note alex
// This code works but is an early version of the
// code now in GoFigure App.
// /todo update this code with the more efficient
//       code from GoFigure

int main(int argc,char* argv[])
  {
  const unsigned int Dimension = 3;

  if ( argc<5 )
    {
    std::cout << "Usage: ";
    std::cout << "(exe) FileName IsChannelToBeBlended DirCh1 DirCh2";
    std::cout << std::endl;
    return 0;
    }

  if( !( atoi(argv[2]) == 0 || atoi(argv[2]) == 1 ) )
    {
    std::cout << "Second argument must be either 0 or 1." << std::endl;
    return 0;
    } 

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

  // Get the name of the file without the path
  unsigned int j = 0;
  unsigned int len = strlen(argv[1]);
  for( unsigned int i = len-1; i != 0; i-- )
    {
    if ( argv[1][i] == '/' )
      {
      j = i;
      break;
      }
    }
  j++;

  char* fname = new char[len-j+1];
  for( unsigned int i = 0; i < len-j+1; i++ )
    {
    fname[i]  = argv[1][j+i];
    }

  vtkMetaImageWriter * writer = vtkMetaImageWriter::New();
  writer->SetFileDimensionality( Dimension );

  for( int timePoint = 0;timePoint < NumberOfTimePoints; timePoint++)
    {
    if( atoi(argv[2]) == 0 || NumberOfChannels == 1 )
      {
      for( int channel = 0; channel < NumberOfChannels; channel++)
        {
        // Select the specified time-point and channel for write-out
        vtkLSMReader* treader=vtkLSMReader::New();
        treader->SetFileName(argv[1]);
        treader->SetUpdateTimePoint( timePoint );
        treader->SetUpdateChannel( channel );
        treader->Update();

        // Determine the name of the file
        std::stringstream namebuffer;
        namebuffer << argv[3 + channel];
        namebuffer << fname;
        namebuffer << "_T_"  << timePoint;
        namebuffer << "_C_"  << channel;
        namebuffer << ".mha";
        std::cout << namebuffer.str().c_str() << std::endl;

        // Write-out
        writer->SetFileName(namebuffer.str().c_str());
        writer->SetInputConnection( treader->GetOutputPort( ) );
        writer->Write();
	}
      }
    else
      {
      // read the first channel, then Append other channel as components
      // of myImage. Finally, write it.
      // note that jpgwriter only handles 1 or 3 comp images.
      // if only one channel, no compositing
      // if two channel, compose with a dummy (empty) third channel
      // if three channels, no problem
      // if more than three channel, we should blend (TBD)
      if( NumberOfChannels < 4 )
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
      namebuffer << ".mha";
      std::cout << namebuffer.str().c_str() << std::endl;
      writer->SetFileName(namebuffer.str().c_str());
      writer->SetInput( appendFilter2->GetOutput( ) );
      writer->Write();
      }
    else
      {
      // num of channel > 3
      // TBD
      }
    }
  }

  return EXIT_SUCCESS;
}
