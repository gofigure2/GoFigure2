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
#include "vtkImageGaussianSmooth.h"
#include "vtkImageGradient.h"
#include "QGoSynchronizedView2D.h"

#include <QStringList>
#include <QString>

#include "vtksys/SystemTools.hxx"


#include "itkImage.h"
#include "itkCastImageFilter.h"
#include "itkSmartPointer.h"
#include "itkImageFileReader.h"



bool CheckSnapshot( QGoSynchronizedView* iViewer, GoFigure::FileType iType )
{
  QString filename = iViewer->SnapshotViewXY( iType );
  std::string path =
        vtksys::SystemTools::GetCurrentWorkingDirectory();
  path += "/";
  path += filename.toStdString();

  if( vtksys::SystemTools::FileExists( path.c_str() ) )
    {
    vtksys::SystemTools::RemoveFile( path.c_str() );
    return true;
    }
  else
    {
    std::cerr << "FAILURE * viewer->SnapshotViewXY( " << iType
      <<" )" <<std::endl;
    return false;
    }
}



int main( int argc, char** argv )
{

  if( argc != 3 )
    {
    std::cout <<"Usage : qgocomparertest(.exe) " <<std::endl;
    std::cout << "1-file.png" <<std::endl;
    std::cout << "2-test (boolean)" <<std::endl;
    return EXIT_FAILURE;
    }

  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName("MegasonLab");
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );



// ITK Typedefs
  // ITK Reader and filters Typedef
  typedef double       InputPixelType;
  typedef itk::Image< InputPixelType, 2>  InputImage2DType;
  typedef InputImage2DType::Pointer       InputImage2DPointer;

  typedef unsigned char VisuPixelType;
  typedef itk::Image< VisuPixelType, 2>  VisuImage2DType;
  typedef VisuImage2DType::Pointer       VisuImage2DPointer;

  // for visualization purpose we only need unsigned char images
  typedef itk::Image< VisuPixelType, 2>  VisuImage2DType;

  // cast the input to a reasonable pixel type for visualization
  typedef itk::CastImageFilter< InputImage2DType, VisuImage2DType >
          Cast2DFilterType;

  //itk reader
  typedef itk::ImageFileReader< InputImage2DType> itkReaderType;

  itkReaderType::Pointer itkReader = itkReaderType::New();

  itkReader->SetFileName( argv[1] );



  // create 3 images from 1
  vtkSmartPointer< vtkPNGReader > reader = vtkSmartPointer< vtkPNGReader >::New();
  reader->SetFileName( argv[1] );
  reader->Update();


  QString cp0 = "comp0";
  QGoSynchronizedView2D* SynchronizedView0 = new QGoSynchronizedView2D(cp0,0);

  QTimer* timer = new QTimer;
  timer->setSingleShot( true );
  QObject::connect( timer, SIGNAL( timeout() ), SynchronizedView0, SLOT( close() ) );

  SynchronizedView0->SetImage(reader->GetOutput());
  //SynchronizedView0->SetITKImage<InputImage2DType>(itkReader->GetOutput());
  SynchronizedView0->Update();
  SynchronizedView0->show();

  if( atoi( argv[2] ) == 1 )
    {
    if( !CheckSnapshot( SynchronizedView0, GoFigure::BMP ) )
      {
      return EXIT_FAILURE;
      }
    if( !CheckSnapshot( SynchronizedView0, GoFigure::PNG ) )
      {
      return EXIT_FAILURE;
      }
    if( !CheckSnapshot( SynchronizedView0, GoFigure::JPEG ) )
      {
      return EXIT_FAILURE;
      }
    if( !CheckSnapshot( SynchronizedView0, GoFigure::EPS ) )
      {
      return EXIT_FAILURE;
      }
    if( !CheckSnapshot( SynchronizedView0, GoFigure::TIFF) )
      {
      return EXIT_FAILURE;
      }

    timer->start( 1000 );
    }


  app.processEvents();

  int output = app.exec();

  delete timer;

  delete SynchronizedView0;


  return output;
}