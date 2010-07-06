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
#include "vtkMetaImageReader.h"
#include "vtkImageData.h"

#include "vtkExtractVOI.h"
#include "vtkImageData.h"
#include "vtkPNGWriter.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>



int main( int argc, char** argv )
{
  if( argc != 5 )
    {
    std::cerr <<"1-filename (mha)" <<std::endl;
    std::cerr <<"2-megacapture header (.meg)" <<std::endl;
    std::cerr <<"3-time point" <<std::endl;
    std::cerr <<"4-channel" <<std::endl;
    return EXIT_FAILURE;
    }

  vtkMetaImageReader* reader = vtkMetaImageReader::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  vtkImageData* image = reader->GetOutput();

  double spacing[3];
  image->GetSpacing( spacing );

  int dim[3];
  image->GetDimensions( dim );

  int extent[6];
  image->GetExtent( extent );

  std::ofstream file( argv[2], std::ios_base::app );

  int iTimePoint = atoi( argv[3] );
  int iChannel = atoi( argv[4] );

  int plaque = 0;
  int column = 0;
  int row = 0;
  int ztile = 0;
  int ytile = 0;
  int xtile = 0;

  char timeStr[ 100 ] = "";
  struct stat buf;

  if( !stat( argv[1], &buf ) )
    {
    strftime( timeStr, 100, "%Y-%m-%d %H:%M:%S", localtime( &buf.st_mtime ) );
    }

  for( int i = extent[4]; i <= extent[5]; i++ )
    {
    std::stringstream filename;
    filename <<"image-PL" << setfill('0') << setw(2) << plaque;
    filename <<"-CO" << setfill('0') << setw(2) << column;
    filename <<"-RO" << setfill('0') << setw(2) << row;
    filename <<"-ZT" << setfill('0') << setw(2) << ztile;
    filename <<"-YT" << setfill('0') << setw(2) << ytile;
    filename <<"-XT" << setfill('0') << setw(2) << xtile;
    filename <<"-TM" << setfill('0') << setw(4) << iTimePoint;
    filename <<"-ch" << setfill('0') << setw(2) << iChannel;
    filename <<"-zs" << setfill('0') << setw(4) << i;
    filename <<".png";

    file <<"<Image>"<<std::endl;
    file <<"Filename " <<filename.str() <<std::endl;
    file <<"DateTime " <<timeStr <<std::endl;
    file <<"StageX 1000" <<std::endl;
    file <<"StageY -1000" <<std::endl;
    file <<"Pinhole 44.216" <<std::endl;
    file <<"</Image>"<<std::endl;

    vtkExtractVOI* extract = vtkExtractVOI::New();
    extract->SetSampleRate( 1, 1, 1 );
    extract->SetInput( image );

    extract->SetVOI( extent[0], extent[1], extent[2], extent[3], i, i );
    extract->Update();

    vtkPNGWriter* writer2d = vtkPNGWriter::New();
    writer2d->SetInput( extract->GetOutput() );
    writer2d->SetFileName( filename.str().c_str() );
    writer2d->Write();

    writer2d->Delete();
    extract->Delete();
    }
  file.close();

  reader->Delete();

  return EXIT_SUCCESS;
}
