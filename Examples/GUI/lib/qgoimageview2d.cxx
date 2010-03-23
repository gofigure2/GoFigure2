/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

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

#include "vtkSmartPointer.h"
#include "vtkPNGReader.h"
#include "vtkImageData.h"
#include "vtkLookupTable.h"

#include "vtkLookupTableManager.h"

#include "QGoImageView2D.h"

int main( int argc, char** argv )
{
  if( argc != 3 )
    {
    std::cout <<"Usage : qgoimageview2d(.exe) " <<std::endl;
    std::cout << "1-file.png" <<std::endl;
    std::cout << "2-test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );

  QGoImageView2D* viewer = new QGoImageView2D;

  vtkSmartPointer< vtkPNGReader > reader = vtkSmartPointer< vtkPNGReader >::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  QTimer* timer = new QTimer;
  timer->setSingleShot( true );
  QObject::connect( timer, SIGNAL( timeout() ), viewer, SLOT( close() ) );

  viewer->SetImage( reader->GetOutput() );
  viewer->Update();
  viewer->show();

  if( atoi( argv[2] ) == 1 )
    {
    timer->start( 1000 );
    }

  viewer->ShowScalarBar( true );

  std::cout <<viewer->GetImageActor( 0 ) <<std::endl;
  std::cout <<viewer->GetImageActor( 1 ) <<std::endl;

  std::cout <<viewer->GetInteractor( 0 ) <<std::endl;
  std::cout <<viewer->GetInteractor( 1 ) <<std::endl;

  std::cout <<viewer->GetImage() <<std::endl;

  viewer->SetLookupTable( vtkLookupTableManager::GetHotMetalLookupTable() );

  app.processEvents();

  int output = app.exec();

  app.closeAllWindows();

  delete timer;
  delete viewer;

  return output;
}
