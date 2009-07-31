/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
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

#define NOT_ALEX_DEBUG 0

#include "QImagePageView4DTracer.h"

#include "QSplitterchild.h"
#include "vtkViewImage2DCommand.h"
#include "QGoPrintDatabase.h"

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

#include "itkMultiFileReader.h"
#include "itkMegaCaptureImport.h"
#include "itkLsm3DSerieImport.h"

#include <ctime>

//------------------------------------------------------------------------------
QImagePageView4DTracer::QImagePageView4DTracer( QWidget* parent ) :
  QWidget( parent )
{
  this->NumberOfTimePoints = 0;
  this->Image = (vtkImageData*)(0);

  this->Whatever = new QImagePageViewTracer( );

  this->Slider1 = new QSlider( Qt::Horizontal );

  this->Button2 = new QPushButton("Switch between greyscale and color when multichannel");

//   this->Button1 = new QPushButton("Run &Movie");

  this->LayOut1 = new QVBoxLayout;
  this->LayOut1->addWidget( this->Whatever );
  this->LayOut1->addWidget( this->Button2 );
  this->LayOut1->addWidget( this->Slider1 );
  //NOTE ALEX: button for movie
  // this->LayOut1->addWidget( this->Button1 );

  this->LayOutWidget1 = new QWidget( this );
  this->LayOutWidget1->setLayout( this->LayOut1 );
  // this->setLayout( this->LayOut1 );

  QObject::connect( this->Slider1, SIGNAL( valueChanged( int ) ),
    this, SLOT( SetView( int ) ) );
//Commented for the time being (Button1 is not created)
//  QObject::connect( this->Button1, SIGNAL( clicked( ) ),
//    this, SLOT( RunMovie( ) ) );
  QObject::connect( this->Button2, SIGNAL( clicked( ) ),
    this, SLOT( SwitchColorMode( ) ) );

  this->ColorVizu = false;
  this->FileName = NULL;

  this->IsSerie = false;
  this->IsLsm = false;
  this->IsMegaCapture = false;
  this->IsFileListComputed = false;
  m_DBTables = new QGoPrintDatabase;

  m_DBTables->hide();


 QObject::connect(this->m_DBTables->FigureTable, SIGNAL(itemSelectionChanged ()),
    this, SLOT(ConnectSelectContoursWidgetAndHighlightContours()));

 QObject::connect(this->m_DBTables, SIGNAL(TableContentChanged()),
   this, SLOT(DeleteContourVisu()));

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
QImagePageView4DTracer::~QImagePageView4DTracer()
{
  delete this->Whatever;
  if( this->Image )
    {
    this->Image->Delete();
    this->Image = (vtkImageData*)(0);
    }
  delete m_DBTables;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void
QImagePageView4DTracer::
ReadMultiFile( const int& TimePoint )
{
  if( this->Image )
    {
    this->Image->Delete();
    }

  itk::MultiFileReader* reader = new itk::MultiFileReader;
  QProgressBar pBar;
  reader->SetProgressBar( &pBar );

  if( !this->IsFileListComputed )
    {
    if( this->IsLsm )
      {
      itk::Lsm3DSerieImport::Pointer  importFileInfoList = itk::Lsm3DSerieImport::New();
      importFileInfoList->SetFileName( this->FileName );
      importFileInfoList->SetGroupId( 1 );
      importFileInfoList->Update();
      this->FileList = *(importFileInfoList->GetOutput());
      }
    if( this->IsMegaCapture )
      {
      itk::MegaCaptureImport::Pointer  importFileInfoList = itk::MegaCaptureImport::New();
      importFileInfoList->SetFileName( this->FileName );
      QProgressBar pBar;
      importFileInfoList->SetProgressBar( &pBar );
      importFileInfoList->Update();
      this->FileList = *(importFileInfoList->GetOutput());
      }
    }

  reader->SetInput( &(this->FileList) );
  if( this->IsLsm )
    {
    reader->SetDimensionality( 3 );
    reader->SetFileType( LSM );
    reader->SetChannel( 0 );
    }
  if( this->IsMegaCapture )
    {
    reader->SetDimensionality( 2 );
    reader->SetFileType( JPEG );
    }
  if( this->ColorVizu )
    {
    reader->SetMultiChannelImagesON();
    }
  else
    {
    reader->SetMultiChannelImagesOFF();
    }
  reader->SetTimePoint( TimePoint );
  reader->Update();

  this->Slider1->setMinimum( 0 );
  this->NumberOfTimePoints = reader->GetNumberOfTimePoints();
  if( this->NumberOfTimePoints > 1)
    {
    this->Slider1->setMaximum( this->NumberOfTimePoints );
    }
  else
    {
    this->Slider1->hide();
    }

  // disconnect the pipeline
  this->Image = vtkImageData::New();
  this->Image->ShallowCopy( reader->GetOutput() );
  delete reader;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
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
    this->FileName = new char[strlen(name) + 1]; /** \todo TO BE DELETED */
    strcpy(this->FileName, name);
    }

  this->SetView( 0 );
  this->IsFileListComputed = false;
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void QImagePageView4DTracer::ReadLSMFile( const int& TimePoint )
{
  // have to redirect that to a Multifile reader with only one file,
  // so we will have less duplicated code, and less maintenance.

  vtkImageData* myImage_ch1 = vtkImageData::New();
  vtkLSMReader* reader = vtkLSMReader::New();
  reader->SetFileName( this->FileName );
  reader->SetUpdateTimePoint( TimePoint );
  reader->Update();
  this->Slider1->setMinimum( 0 );
  this->NumberOfTimePoints = reader->GetNumberOfTimePoints();
  if( this->NumberOfTimePoints > 1)
    {
    this->Slider1->setMaximum( this->NumberOfTimePoints );
    }
  else
    {
    this->Slider1->hide();
    }

  int NumberOfChannels = reader->GetNumberOfChannels();
  myImage_ch1->ShallowCopy( reader->GetOutput() );
  reader->Delete();

  vtkImageData* myImage_ch2;
  if( ( NumberOfChannels == 1 ) || ( !this->ColorVizu ) )
    {
    if( this->Image )
      {
      this->Image->Delete();
      }
    this->Image = myImage_ch1;
    if( NumberOfChannels == 1 )
      {
      this->Button2->hide();
      }
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
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void QImagePageView4DTracer::SetView( const int& value )
{
  clock_t start, finish;
  double time;

  start = clock();
  if( this->IsSerie )
    {
    this->ReadMultiFile( value );
    }
  else
    {
    this->ReadLSMFile( value );
    }
  finish = clock();
  time = (double(finish)-double(start))/CLOCKS_PER_SEC;
  std::cout << "Reading Time: " << time << "s" << std::endl;

// #if NOT_ALEX_DEBUG
//   start = clock();
  // NOTE ALEX:
  // Should be able to update the ImageData without recreating
  // a complete object.
  // this makes a lot of problem (on top of the obvious speed issue)
  // as all orientation, DB informations and so on are reseted in the process.
//   this->LayOut1->removeWidget( this->Whatever );
//   QImagePageViewTracer* tempPVT = new QImagePageViewTracer( );
//   tempPVT->SetDatabaseRelatedVariables(
//     this->Whatever->m_DBServer,
//     this->Whatever->m_DBLogin,
//     this->Whatever->m_DBPassword,
//     this->Whatever->m_DBName,
//     this->Whatever->m_DBExperimentID,
//     this->Whatever->m_DBExperimentName
//     );
//   delete this->Whatever;
//   this->Whatever = tempPVT;
//   this->LayOut1->insertWidget( 0, this->Whatever );
//   finish = clock();
//   time = (double(finish)-double(start))/CLOCKS_PER_SEC;
//   std::cout << "Replace widget: " << time << "s" << std::endl;
// #endif

  start = clock();
  this->Whatever->SetImage( this->Image );
  this->Whatever->SetTimePoint( value );
  this->Whatever->LoadFiguresFromDB();
  finish = clock();
  time = (double(finish)-double(start))/CLOCKS_PER_SEC;
  std::cout << "Set image in widget and load contours from DB: ";
  std::cout << time << "s" << std::endl;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QImagePageView4DTracer::resizeEvent( QResizeEvent* event )
{
  QWidget::resizeEvent( event );
  this->LayOutWidget1->resize( event->size() );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QImagePageView4DTracer::RunMovie( )
{
  // NOTE ALEX: movie trials
  // std::cout << "Enjoy movie mode." << std::endl;
  // for( unsigned int i = 0; i < this->NumberOfTimePoints; i++)
  //  {
  //  this->Slider1->setValue( i );
  //  }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QImagePageView4DTracer::SwitchColorMode( )
{
  this->ColorVizu = !this->ColorVizu;
  this->SetView( this->Slider1->value() );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QImagePageView4DTracer::SetDatabaseRelatedVariables( const QString& iServer,
  const QString& iLogin, const QString& iPassword, const QString& iDatabaseName,
  const int& iExperimentID, const QString& iExperimentName )
{
  this->Whatever->SetDatabaseRelatedVariables( iServer, iLogin, iPassword,
    iDatabaseName, iExperimentID, iExperimentName );
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QImagePageView4DTracer::FillTablesWidget()
{
  m_DBTables->FillTableFromDatabase(Whatever->m_DBName,Whatever->m_DBServer,Whatever->m_DBLogin,
           Whatever->m_DBPassword,Whatever->m_DBExperimentID,Whatever->m_DBExperimentName);
  m_DBTables->show();

}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void QImagePageView4DTracer::ConnectSelectContoursWidgetAndHighlightContours()
{
  this->HighlightContours(m_DBTables->FigureTable->ContoursToHighlight());
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
void QImagePageView4DTracer::DeleteContourVisu()
{
  QStringList QContourToDelete;
  QContourToDelete = this->m_DBTables->FigureTable->ValuesForSelectedRows("figureID");
  //todo: after implementation of the method to delete the contour, use it here.
}