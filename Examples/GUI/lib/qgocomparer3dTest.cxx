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
#include "vtkMetaImageReader.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageGradient.h"
#include "QGoComparer3D.h"

#include <QStringList>
#include <QString>


int main( int argc, char** argv )
{

  if( argc != 3 )
    {
    std::cout <<"Usage : qgocomparer3Dtest(.exe) " <<std::endl;
    std::cout << "1-file.mhd" <<std::endl;
    std::cout << "2-test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }

  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );


  // create 3 images from 1

  vtkSmartPointer< vtkMetaImageReader > reader = vtkSmartPointer< vtkMetaImageReader >::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  vtkSmartPointer< vtkImageGaussianSmooth > filter1 =
                            vtkSmartPointer< vtkImageGaussianSmooth >::New();
  filter1->SetInputConnection(reader->GetOutputPort());
  filter1->Update();

  vtkSmartPointer< vtkImageGradient > filter2 =
                            vtkSmartPointer< vtkImageGradient >::New();
  filter2->SetInputConnection(reader->GetOutputPort());
  filter2->Update();



  QString cp0 = "comp0";
  QString cp1 = "comp1";
  QString cp2 = "comp3";


  QGoComparer3D* Comparer0 = new QGoComparer3D(cp0,0);
  QGoComparer3D* Comparer1 = new QGoComparer3D("cp1",0);
  QGoComparer3D* Comparer2 = new QGoComparer3D("cp2",0);


  QTimer* timer = new QTimer;
  timer->setSingleShot( true );
  QObject::connect( timer, SIGNAL( timeout() ), Comparer0, SLOT( close() ) );
  QObject::connect( timer, SIGNAL( timeout() ), Comparer1, SLOT( close() ) );
  QObject::connect( timer, SIGNAL( timeout() ), Comparer2, SLOT( close() ) );

  Comparer0->SetImage(reader->GetOutput());
  Comparer1->SetImage(filter1->GetOutput());
  Comparer2->SetImage(filter2->GetOutput());


  Comparer0->Update();
  Comparer0->show();

  Comparer1->Update();
  Comparer1->show();

  Comparer2->Update();
  Comparer2->show();


  if( atoi( argv[2] ) == 1 )
    {
    timer->start( 1000 );

    Comparer0->SetFullScreenView(1);
    if( Comparer0->GetFullScreenView() != 1 )
      {
      std::cerr <<"Comparer0->GetFullScreenView() = " <<Comparer0->GetFullScreenView();
      std::cerr <<" != 1" <<std::endl;
      app.closeAllWindows();

      reader->Delete();
      delete timer;
      delete Comparer0;
      delete Comparer1;
      delete Comparer2;
      return EXIT_FAILURE;
      }

    Comparer0->SetFullScreenView(2);
    if( Comparer0->GetFullScreenView() != 2 )
      {
      std::cerr <<"Comparer0->GetFullScreenView() = " <<Comparer0->GetFullScreenView();
      std::cerr <<" != 2" <<std::endl;
      app.closeAllWindows();

      delete timer;
      delete Comparer0;
      delete Comparer1;
      delete Comparer2;
      return EXIT_FAILURE;
      }

    Comparer0->SetFullScreenView(3);
    if( Comparer0->GetFullScreenView() != 3 )
      {
      std::cerr <<"Comparer0->GetFullScreenView() = " <<Comparer0->GetFullScreenView();
      std::cerr <<" != 3" <<std::endl;
      app.closeAllWindows();

      delete timer;
      delete Comparer0;
      delete Comparer1;
      delete Comparer2;
      return EXIT_FAILURE;
      }

    Comparer0->SetFullScreenView(4);
    if( Comparer0->GetFullScreenView() != 4 )
      {
      std::cerr <<"Comparer0->GetFullScreenView() = " <<Comparer0->GetFullScreenView();
      std::cerr <<" != 4" <<std::endl;
      app.closeAllWindows();

      delete timer;
      delete Comparer0;
      delete Comparer1;
      delete Comparer2;
      return EXIT_FAILURE;
      }

    Comparer0->SetFullScreenView(0);
    if( Comparer0->GetFullScreenView() != 0 )
      {
      std::cerr <<"Comparer0->GetFullScreenView() = " <<Comparer0->GetFullScreenView();
      std::cerr <<" != 0" <<std::endl;
      app.closeAllWindows();

      delete timer;
      delete Comparer0;
      delete Comparer1;
      delete Comparer2;
      return EXIT_FAILURE;
      }
  }


  app.processEvents();



  int output = app.exec();

  delete timer;

  delete Comparer0;
  delete Comparer1;
  delete Comparer2;


  return output;
}

