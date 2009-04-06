/*=========================================================================
 Authors: The GoFigure Dev. Team.
 while at Megason Lab, Systems biology, Harvard Medical school, 2009

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

#include "QImagePageViewTracer.h"
#include "QImagePageView4DTracer.h"

#include "QSplitterchild.h"
#include "vtkViewImage2DCommand.h"

#include <QResizeEvent>
#include <vtkImageAppendComponents.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkCamera.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h>

#include <vtkImageData.h>
#include <vtkImageClip.h>
#include <vtkImagePermute.h>
#include <vtkImageResample.h>
#include <vtkWindowToImageFilter.h>

#include <vtkLSMReader.h>
#include <vtkBMPWriter.h>
#include <vtkPostScriptWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkTIFFWriter.h>

#include <vtkPolyDataWriter.h>

#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkMath.h>

#include <ctime>


QImagePageView4DTracer::QImagePageView4DTracer( QWidget* parent ) : QWidget( parent )
{
  this->NumberOfTimePoints = 0;
  this->Image = (vtkImageData*)(0);

  this->Whatever = new QImagePageViewTracer( );

  this->slider1 = new QSlider( Qt::Horizontal );

  this->button2 = new QPushButton("Switch between greyscale and color when multichannel");

  this->button1 = new QPushButton("Run &Movie");

  this->LayOut1 = new QVBoxLayout;
  this->LayOut1->addWidget( this->Whatever );
  this->LayOut1->addWidget( this->slider1 );
  this->LayOut1->addWidget( this->button2 );
  //this->LayOut1->addWidget( this->button1 );

  this->LayOutWidget1 = new QWidget( this );
  this->LayOutWidget1->setLayout( this->LayOut1 );

  QObject::connect( this->slider1, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetView( int ) ) );
  QObject::connect( this->button1, SIGNAL( clicked( ) ),
    this, SLOT( RunMovie( ) ) );
  QObject::connect( this->button2, SIGNAL( clicked( ) ),
    this, SLOT( SwitchColorMode( ) ) );

  this->ColorVizu = false;
  this->FileName = NULL;
}

QImagePageView4DTracer::~QImagePageView4DTracer()
{
  delete this->Whatever;
  //delete this->LayOut1;
  //delete this->LayOutWidget1;
  //delete this->slider1;
  //delete this->button1;
  //delete this->button2;
}

void
QImagePageView4DTracer::SetFileName(const char* name )
{
  if ( this->FileName && name && (!strcmp(this->FileName,name)))
    {
    return;
    }
  if (!name && !this->FileName)
    {
    return;
    }
  if (this->FileName)
    {
    delete [] this->FileName;
    this->FileName = NULL;
    }
  if (name)
    {
    this->FileName = new char[strlen(name) + 1];
    strcpy(this->FileName, name);
    }

  this->SetView( 0 );
}

void QImagePageView4DTracer::ReadLSMFile( const int& TimePoint )
{
  vtkImageData* myImage_ch1 = vtkImageData::New();
  vtkLSMReader* reader=vtkLSMReader::New();
  reader->SetFileName( this->FileName );
  reader->SetUpdateTimePoint( TimePoint );
  reader->Update();
  this->slider1->setMinimum( 0 );
  this->NumberOfTimePoints = reader->GetNumberOfTimePoints();
  this->slider1->setMaximum( this->NumberOfTimePoints );

  int NumberOfChannels = reader->GetNumberOfChannels();
  myImage_ch1->ShallowCopy( reader->GetOutput() );
  reader->Delete();

  vtkImageData* myImage_ch2;
  if( ( NumberOfChannels == 1 ) || ( !ColorVizu ) )
  {
    if( this->Image )
    {
      this->Image->Delete();
    }
    this->Image = myImage_ch1;
    return;
    }
  else
  {
    myImage_ch2 = vtkImageData::New();
      vtkLSMReader* reader2=vtkLSMReader::New();
    reader2->SetFileName( this->FileName );
    reader2->SetUpdateTimePoint( TimePoint );
    reader2->SetUpdateChannel( 1 );
    reader2->Update();
    myImage_ch2->ShallowCopy( reader2->GetOutput() );
    reader2->Delete();
  }

  vtkImageData* myImage2 = vtkImageData::New();
  vtkImageAppendComponents* appendFilter1 = vtkImageAppendComponents::New();
  appendFilter1->AddInput( myImage_ch1 );
  appendFilter1->AddInput( myImage_ch2 );
  appendFilter1->Update();
  myImage2->ShallowCopy( appendFilter1->GetOutput() );
  appendFilter1->Delete();
  myImage_ch2->Delete();

  vtkImageData* myImage_ch3 = vtkImageData::New();
  if( NumberOfChannels == 2 )
  {
    myImage_ch3->ShallowCopy( myImage_ch1 );
  }
  else
  {
    vtkLSMReader* reader3=vtkLSMReader::New();
    reader3->SetFileName( this->FileName );
    reader3->SetUpdateTimePoint( TimePoint );
    reader3->SetUpdateChannel( 2 );
    reader3->Update();
    myImage_ch3->ShallowCopy( reader3->GetOutput() );
    reader3->Delete();
  }
  myImage_ch1->Delete();

  vtkImageData* myImage3 = vtkImageData::New();
  vtkImageAppendComponents* appendFilter2 = vtkImageAppendComponents::New();
  appendFilter2->AddInput( myImage2    );
  appendFilter2->AddInput( myImage_ch3 );
  appendFilter2->Update();
  myImage3->ShallowCopy( appendFilter2->GetOutput() );
  appendFilter2->Delete();
  myImage2->Delete();
  myImage_ch3->Delete();

  if( this->Image )
  {
    this->Image->Delete();
  }

  this->Image = myImage3;
}

void QImagePageView4DTracer::SetView( const int& value )
{
  clock_t start,finish;
  double time;

  start = clock();
  this->ReadLSMFile( value );
  finish = clock();
  time = (double(finish)-double(start))/CLOCKS_PER_SEC;
  std::cout << "Reading Time: " << time << "s" << std::endl;

  start = clock();
  this->LayOut1->remove( this->Whatever );
  delete this->Whatever;
  this->Whatever = new QImagePageViewTracer( );
  this->LayOut1->insertWidget( 0, this->Whatever );
  finish = clock();
  time = (double(finish)-double(start))/CLOCKS_PER_SEC;
  std::cout << "Replace widget: " << time << "s" << std::endl;

  start = clock();
  this->Whatever->SetImage( this->Image );
  finish = clock();
  time = (double(finish)-double(start))/CLOCKS_PER_SEC;
  std::cout << "Set image in widget: " << time << "s" << std::endl;
}

void QImagePageView4DTracer::resizeEvent( QResizeEvent* event )
{
  QWidget::resizeEvent( event );
  this->LayOutWidget1->resize( event->size() );
}

void QImagePageView4DTracer::RunMovie( )
{
  // std::cout << "Enjoy movie mode." << std::endl;
  // for( unsigned int i = 0; i < this->NumberOfTimePoints; i++)
  //  {
  //  this->slider1->setValue( i );
  //  }
}

void QImagePageView4DTracer::SwitchColorMode( )
{
  this->ColorVizu = !this->ColorVizu;
  this->SetView( this->slider1->value() );
}

