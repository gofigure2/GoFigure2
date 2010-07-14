/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "itkMegaCaptureImport.h"
#include "itkMultiFileReader.h"

#include "vtkImageData.h"

int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cout <<"Usage: ./multifilereader(.exe) takes 2 arguments" <<std::endl;
    std::cout <<"1-filename (.jpg)" <<std::endl;
    std::cout <<"2-timebased (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }

  itk::MegaCaptureImport::Pointer importer = itk::MegaCaptureImport::New();
  importer->SetFileName( argv[1] );
  importer->Update();
  GoFigureFileInfoHelperMultiIndexContainer listoffiles = importer->GetOutput();

//   bool timebased = ( atoi( argv[2] ) == 0 );
//
//   itk::MultiFileReader::Pointer reader = itk::MultiFileReader::New();
//   reader->SetTimeBased( timebased );
//   reader->SetDimensionality( 2 );
//   reader->SetFileType( itk::MultiFileReader::JPEG );
//   reader->MultiChannelImagesOn();
//
//   if( timebased )
//     {
//     std::sort( listoffiles->begin(), listoffiles->end(),
//       GoFigureFileInfoHelperTimeBasedCompare() );
//     }
//   else
//     {
//     std::sort( listoffiles->begin(), listoffiles->end(),
//       GoFigureFileInfoHelperZCoordBasedCompare() );
//     }
//
//   reader->SetInput( listoffiles );
//
//   if( timebased )
//     {
//     reader->SetTimePoint( 0 );
//     }
//   else
//     {
//     reader->SetZDepth( 0 );
//     }
//
//   reader->Update();
//
//   vtkImageData* image = reader->GetOutput();
//   std::cout <<*image <<std::endl;
//
//   image->Delete();

  return EXIT_SUCCESS;
}
