/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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

#include <QApplication>
#include <QTimer>

#include "vtkMetaImageReader.h"
#include "vtkSmartPointer.h"
#include "vtkImageCast.h"

#include "vtkImageData.h"
#include "QGoFilterChanAndVese.h"

int main(int argc, char **argv)
{
  if ( argc != 3 )
    {
    std::cout << "Usage : qgoimageview3d(.exe) " << std::endl;
    std::cout << "1-file.mhd" << std::endl;
    std::cout << "2-test (boolean)" << std::endl;
    return EXIT_FAILURE;
    }

  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain("http://gofigure2.sourceforge.net");

  vtkSmartPointer< vtkMetaImageReader > reader =
    vtkSmartPointer< vtkMetaImageReader >::New();
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkSmartPointer< vtkImageCast > castFilter = vtkSmartPointer< vtkImageCast >::New();
  castFilter->SetInput( reader->GetOutput() );
  castFilter->SetOutputScalarTypeToUnsignedChar();
  castFilter->Update();

  // Initialize the segmentation
  QGoFilterChanAndVese*         levelSet2DFilter = new QGoFilterChanAndVese(NULL, 2);
  std::vector< vtkSmartPointer< vtkImageData > > imagesVector(1);
  imagesVector[0] = castFilter->GetOutput();
  levelSet2DFilter->setOriginalImageMC(&imagesVector);
  levelSet2DFilter->setIterations(50);
  levelSet2DFilter->setRadius(2);
  levelSet2DFilter->setCurvature(10);

  double seedPos[3];
  seedPos[0] = 5;
  seedPos[1] = 5;
  seedPos[2] = 5;

  vtkPoints *seeds = vtkPoints::New();
  seeds->InsertNextPoint(seedPos);

  levelSet2DFilter->setCenter(seedPos);
  levelSet2DFilter->setPoints(seeds);

  // if there is an output
  int oResult;

  if ( levelSet2DFilter->getOutput() )
    {
    oResult = EXIT_SUCCESS;
    }
  else
    {
    oResult = EXIT_FAILURE;
    }

  delete levelSet2DFilter;
  seeds->Delete();

  return oResult;
}
