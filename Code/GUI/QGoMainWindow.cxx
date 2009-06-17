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

#include "QGoMainWindow.h"
#include "QImagePageView4DTracer.h"

#include <iostream>

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qcolordialog.h>

#include "itkQtAdaptor.h"
#include "itkQtProgressBar.h"
#include "QGoPrintDatabase.h"

#include <itkImageFileReader.h>
#include <vnl/vnl_random.h>
#include <qsettings.h>

#include <vtkImageAppendComponents.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyDataReader.h>
#include <vtkPLYReader.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <QDialog>

//#include "ContourContainerFileSystem.h"

// *************************************************************************
QGoMainWindow::QGoMainWindow( )
{
  this->setupUi( this );
  this->setCentralWidget( this->CentralImageTabWidget );
  this->setWindowTitle( tr( "<*)0|00|0>< ~~ <*)0|00|0><     GoFigure    ><0|00|0(*> ~~ ><0|00|0(*>") );
  this->statusbar->showMessage( tr( "No data" ) );
  this->CentralImageTabWidget->clear();
  this->CentralImageTabWidget->setMovable(false);
  this->KishoreSegDockWidget->setVisible(false);
  this->ManualSegmentationDockWidget->setVisible(false);
  this->OneClickSegmentationDockWidget->setVisible(false);
  this->actionVolume_rendering_XYZ->setChecked(false);
  this->statusbar->addPermanentWidget( &m_Bar );
  m_Bar.hide();
  //setCurrentFile("");

  this->CentralImageTabWidget->setTabsClosable( true );

  m_LUTDialog = new QGoLUTDialog( this );

  m_DBTables = new QGoPrintDatabase() ;
  m_DBTables->hide();

  m_Wizard = new QGoWizardDB;
  m_Wizard->hide();

  CreateSignalSlotsConnection();
  Fullscreenbuttons();
  readSettings();
}

// *************************************************************************
QGoMainWindow::~QGoMainWindow()
{
  writeSettings();
  while( !m_PageView.empty() )
    {
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView.last() );
    if( myPageView )
      {
      //writeSettingsPageView(myPageView);
      delete myPageView;
      }
    else
      {
      QImagePageView4DTracer* myPageView4D =
        dynamic_cast<QImagePageView4DTracer*>( m_PageView.last() );
      if( myPageView4D )
        {
        delete myPageView4D;
        }
      }
    m_PageView.pop_back();
    }
  delete m_LUTDialog;
  delete m_Wizard;
  delete m_DBTables;
}

// *************************************************************************
void QGoMainWindow::CreateSignalSlotsConnection()
{
    /*  QObject::connect( this->TracerPolygonBtn, SIGNAL( released( ) ),
  this, SLOT( SetTracerToPolygonTracer() ) );
  QObject::connect( this->TracerFreeLineBtn, SIGNAL( released( ) ),
  this, SLOT( SetTracerToFreeLineTracer() ) );
  QObject::connect( this->TracerTranslationBtn, SIGNAL( released() ),
  this, SLOT( SetTracerToTranslateContourTracer() ) );
  QObject::connect( this->TracerRotationBtn, SIGNAL( released() ),
  this, SLOT( SetTracerToRotateContourTracer() ) );
  QObject::connect( this->TracerScalingBtn, SIGNAL( released() ),
  this, SLOT( SetTracerToScaleContourTracer() ) );*/
  QObject::connect( this->ManualSegmentationOnRadioBtn,
    SIGNAL( toggled(bool) ),
    this,
    SLOT( SetContourTracerOn(bool) ) );
  QObject::connect( this->ManualSegmentationOffRadioBtn,
    SIGNAL( toggled(bool) ),
    this,
    SLOT( SetContourTracerOff(bool) ) );

  QObject::connect( this->SeedWidgetOnRadioBtn,
    SIGNAL( toggled(bool) ),
    this,
    SLOT( SetSeedWidgetOn(bool) ) );
  QObject::connect( this->SeedWidgetOffRadioBtn,
    SIGNAL( toggled(bool) ),
    this,
    SLOT( SetSeedWidgetOff(bool) ) );

  QObject::connect( this->m_LUTDialog, SIGNAL( accepted( ) ),
    this, SLOT( ChangeLookupTable( ) ) );

  QObject::connect( this->IdContourColorBtn, SIGNAL( released( ) ),
    this, SLOT( SetColorForGivenId( ) ) );
  QObject::connect( this->TracerValidationBtn, SIGNAL( released( ) ),
    this, SLOT( ValidateContourTracer() ) );
  QObject::connect( this->TracerReinitializeBtn, SIGNAL( released() ),
    this, SLOT( ReinitializeContourTracer() ) );
  QObject::connect( this->TracerReinitializeIncrementBtn, SIGNAL( released() ),
    this, SLOT( ReinitializeAndIncrementContourTracer() ) );

  QObject::connect( this->OneClickBtnBox, SIGNAL( accepted() ),
    this, SLOT( OneClickSegmentation( ) ) );

  //QObject::connect( this->actionOpen, SIGNAL( activated( ) ),
    //this, SLOT( showprogressloading() ) );
  QObject::connect( this->CentralImageTabWidget,
    SIGNAL( currentChanged( int ) ),
    this, SLOT( UpdateToolBarViewButtons( int ) ) );
  QObject::connect( this->CentralImageTabWidget,
    SIGNAL( currentChanged( int ) ),
    this, SLOT( UpdateTracerButtons( int ) ) );

  QObject::connect(this->CentralImageTabWidget,
    SIGNAL(tabCloseRequested(int)),
    this, SLOT( on_actionClose_activated( ) ) );

  QObject::connect( m_Wizard, SIGNAL( accepted() ),
    this, SLOT( openFilesfromDB() ) );

  QObject::connect( &m_SignalAdaptor, SIGNAL(Signal()),
    &(this->m_Bar), SLOT(hide()) );

  for( int i = 0; i < MaxRecentFiles; ++i )
    {
    recentSingleFileActions[i] = new QAction(this);
    recentSingleFileActions[i]->setVisible(false);
    QObject::connect(this->recentSingleFileActions[i], SIGNAL(triggered()),
      this, SLOT(openRecentSingleFile()));

    recentMultipleFileActions[i] = new QAction(this);
    recentMultipleFileActions[i]->setVisible(false);
    QObject::connect(this->recentMultipleFileActions[i], SIGNAL(triggered()),
      this, SLOT(openRecentMultipleFile()));
    }

}

// *************************************************************************
void QGoMainWindow::on_actionOpen_Single_File_activated( )
{
  QString filename = QFileDialog::getOpenFileName(
    this,
    tr( "Select Image" ),"",
    tr( "Images (*.png *.bmp *.jpg *.jpeg *.tiff *.mha *.mhd *.img *.lsm)" )
    );

  if( !filename.isEmpty( ) )
    {
    SetFileName( filename, false );
    }
}

// *************************************************************************
void QGoMainWindow::on_actionOpen_Multiple_Files_activated( )
{
  QString filename = QFileDialog::getOpenFileName(
    this,
    tr( "Select one Image from the Dataset" ),"",
    tr( "Images (*.jpg *.jpeg *.lsm)" )
    );

  if( !filename.isEmpty( ) )
    {
    SetFileName( filename, true );
    }
}


//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionUse_DataBase_activated()
{
  m_Wizard->show();
}
//--------------------------------------------------------------------------------

// *************************************************************************
void QGoMainWindow::openFilesfromDB()
{
  std::cout<<"signal worked"<<std::endl;
  FileListType ListFilenames = m_Wizard->ListFilenames();

  if (!ListFilenames.empty())
    {
    m_PageView.push_back( new QImagePageView4DTracer );
    QImagePageView4DTracer* myPageView;
    myPageView = static_cast< QImagePageView4DTracer*>( m_PageView.last() );
    /*if( IsSerie )
      {
      myPageView->SetFileTypeToSerie( );
      if( Type == 0 ) myPageView->SetSerieTypeToLsm( );
      if( Type == 1 ) myPageView->SetSerieTypeToMegaCapture( );
      }*/
    myPageView->ReadMultiFileFromDB (ListFilenames,0);

    int idx = this->CentralImageTabWidget->addTab( m_PageView.last(),"" );
    this->CentralImageTabWidget->setCurrentIndex( idx );

    m_DBTables->Fill_Database(m_Wizard->Server(),m_Wizard->login(),
    m_Wizard->Password(), m_Wizard->NameDB(),
    m_Wizard->ExpID(), m_Wizard->ExpName());
    m_DBTables->show();
    }
}
// *************************************************************************
void QGoMainWindow::on_actionOpen_Mesh_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();

  if( idx >= 0 )
    {
    QStringList filename = QFileDialog::getOpenFileNames(
      this,
      tr( "Select one mesh or contour"), "",
      tr( "vtk - vtkPolyData (*.vtk);;ply - Polygon File Format (*.ply)" ) );

    if( !filename.isEmpty() )
      {
      QStringList::Iterator it = filename.begin();
      std::list< vtkPolyData* > mesh_list;
      std::list< vtkProperty* > property_list;
      property_list.push_back( 0 );

      while( it != filename.end() )
        {
        if( QFile::exists( *it ) )
          {
          QString extension = QFileInfo( *it ).suffix();

          vtkPolyData* mesh = vtkPolyData::New();

          if( extension.compare( "vtk", Qt::CaseInsensitive ) == 0 )
            {
            vtkPolyDataReader* mesh_reader = vtkPolyDataReader::New();
            mesh_reader->SetFileName( (*it).toAscii( ).data( ) );
            mesh_reader->Update();

            mesh->ShallowCopy( mesh_reader->GetOutput() );
            mesh_list.push_back( mesh );
            mesh_reader->Delete();
            }
          else
            {
            if( extension.compare( "ply", Qt::CaseInsensitive ) == 0 )
              {
              vtkPLYReader* mesh_reader = vtkPLYReader::New();
              mesh_reader->SetFileName( (*it).toAscii( ).data( ) );
              mesh_reader->Update();

              mesh->ShallowCopy( mesh_reader->GetOutput() );
              mesh_list.push_back( mesh );
              mesh_reader->Delete();
              }
            }
          }
        ++it;
        }

      if( !mesh_list.empty() )
        {
        double mesh_bounds[6];
        mesh_list.front()->GetBounds( mesh_bounds );
        bool IsContour = false;

        for( int i = 0; ( i < 3 ) && ( !IsContour ); ++i )
          {
          if( mesh_bounds[2*i] == mesh_bounds[2*i+1] )
            {
            IsContour = true;
            }
          }

        QImagePageViewTracer* myPageView =
          dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
        if( myPageView )
          {
          myPageView->AddContours( mesh_list, property_list, true, true );
          // if( IsContour )
          //{
          //
          // }
          }
        else
          {
          QImagePageView4DTracer* myPageView4D =
            dynamic_cast<QImagePageView4DTracer*>( m_PageView[ idx ] );
          if( myPageView4D )
            {
            // myPageView4D->AddContours< std::list< vtkPolyData* >,
            // std::list< vtkProperty* > >( mesh_list, property_list, true );
            // if( IsContour )
            //   {
            //   }
            // else
            //  {
            //  }
            }
          }
        }

      while( !mesh_list.empty() )
        {
        if( mesh_list.back() )
          {
          mesh_list.back()->Delete();
          }
        mesh_list.pop_back();
        }
      }
    }
}//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionClose_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  this->CentralImageTabWidget->removeTab( idx );
  if( idx >= 0 )
    {
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
    if( myPageView )
      {
      //writeSettingsPageView(myPageView);
      delete myPageView;
      }
    else
      {
      QImagePageView4DTracer* myPageView4D =
        dynamic_cast<QImagePageView4DTracer*>( m_PageView[ idx ] );
      if( myPageView4D )
        {
        //writeSettingsPageView(myPageView4D);
        delete myPageView4D;
        }
      }
    m_PageView.remove( idx );
    }

  // NOTE ALEX:
  // we should remove the datasets in m_ITK, m_VTK arrays
  // by default to release memory
  //
  // We should check if it was the last tab, in which case,
  // the close option shoudl be disactivated

  writeSettings();
}
//--------------------------------------------------------------------------------

// *************************************************************************
void QGoMainWindow::on_actionClose_all_activated( )
{
  this->CentralImageTabWidget->clear( );
  while( !m_PageView.empty() )
    {
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView.last() );
    if( myPageView )
      {
      //writeSettingsPageView(myPageView);
      delete myPageView;
      }
    else
      {
      QImagePageView4DTracer* myPageView4D =
        dynamic_cast<QImagePageView4DTracer*>( m_PageView.last() );
      if( myPageView4D )
        {
        //writeSettingsPageView(myPageView4D);
        delete myPageView4D;
        }
      }
    m_PageView.pop_back();
    writeSettings();
    }
}
//--------------------------------------------------------------------------------

// *************************************************************************
void QGoMainWindow::on_actionQuit_activated( )
{
  this->close();
  writeSettings();
}
//--------------------------------------------------------------------------------

// *************************************************************************
void QGoMainWindow::on_actionBackground_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  QImagePageViewTracer* myPageView =
    dynamic_cast< QImagePageViewTracer* >( m_PageView[idx] );
  if( myPageView )
    {
    SetBackgroundColor< QImagePageViewTracer >( myPageView );
    }
  else
    {
    QImagePageView4DTracer* myPageView2 =
      dynamic_cast< QImagePageView4DTracer* >( m_PageView[idx] );
    if( myPageView2 )
      {
      SetBackgroundColor< QImagePageView4DTracer >( myPageView2 );
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
template< class T >
void QGoMainWindow::SetBackgroundColor( T* PageView )
{
  double* rgb = PageView->GetBackgroundColor();
  QColor color(
    static_cast< int >( 255. * rgb[0] ),
    static_cast< int >( 255. * rgb[1] ),
    static_cast< int >( 255. * rgb[2] ) );
  PageView->SetBackgroundColor( QColorDialog::getColor( color, this ) );
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionLookup_Table_activated( )
{
  m_LUTDialog->show();
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionQuad_View_activated( )
{
  if (this->CentralImageTabWidget->count() != 0)
    {
    this->SetFullScreenDispatch( 0 );
    this->actionSnapshot->setEnabled(false);
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
template< class T >
void
QGoMainWindow::SetFullScreen( const int & ViewID, T* PageView )
{
  PageView->SetFullScreenView( ViewID );
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void
QGoMainWindow::SetFullScreenDispatch( const int & ViewID )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  QImagePageViewTracer* myPageView =
    dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
  if( myPageView )
    {
    this->SetFullScreen<QImagePageViewTracer>( ViewID, myPageView );
    }
  else
    {
    QImagePageView4DTracer* myPageView2 =
      dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
    if( myPageView2 )
      {
      this->SetFullScreen<QImagePageView4DTracer>( ViewID, myPageView2 );
      }
    }
  actionSnapshot->setEnabled(true);
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionFull_screen_XY_activated( )
{
  if (this->CentralImageTabWidget->count() != 0)
    {
    this->SetFullScreenDispatch( 1 );
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionFull_screen_YZ_activated( )
{
  if (this->CentralImageTabWidget->count() != 0)
    {
    this->SetFullScreenDispatch( 2 );
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionFull_screen_XZ_activated( )
{
  if (this->CentralImageTabWidget->count() != 0)
    {
    this->SetFullScreenDispatch( 3 );
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionFull_screen_XYZ_activated( )
{
  if (this->CentralImageTabWidget->count() != 0)
    {
    this->SetFullScreenDispatch( 4 );
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionVolume_rendering_XYZ_activated( )
{
  if (this->CentralImageTabWidget->count() != 0)
    {
    int idx = this->CentralImageTabWidget->currentIndex();
    QImagePageViewTracer* pageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
    if( pageView )
      {
      SetRendering<QImagePageViewTracer>( pageView );
      }
    else
      {
      QImagePageView4DTracer* pageViewColor =
      dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
      if( pageViewColor )
        {
        SetRendering<QImagePageView4DTracer>( pageViewColor );
        }
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
template< class T >
void
QGoMainWindow::SetRendering( T* myPageView )
{
  if (actionVolume_rendering_XYZ->isChecked())
    {
    myPageView->SetView3DToVolumeRenderingMode();
    }
  else
    {
    myPageView->SetView3DToTriPlanarMode();
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionScale_bars_activated( )
{
  if (this->CentralImageTabWidget->count() != 0)
    {
    int idx = this->CentralImageTabWidget->currentIndex();
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
    if( myPageView )
      {
      myPageView->SetShowScalarBar( actionScale_bars->isChecked() );
      }
    else
      {
      QImagePageView4DTracer* pageViewColor =
        dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
      if( pageViewColor )
        {
        pageViewColor->SetShowScalarBar( actionScale_bars->isChecked() );
        }
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
// \todo ALEX: implement support for LSM
//
void QGoMainWindow::on_actionSnapshot_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  QImagePageViewTracer* myPageView =
    dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
  if( myPageView )
    {
    int whichview = myPageView->GetFullScreenView();
    if( whichview != 0 )
      {
      QString SnapshotFileName;
      switch( whichview )
        {
        case 1:
          {
          SnapshotFileName = myPageView->SnapshotViewXY(
            QImagePageViewTracer::PNG);
          break;
          }
        case 2:
          {
          SnapshotFileName = myPageView->SnapshotView2(
            QImagePageViewTracer::PNG);
          break;
          }
        case 3:
          {
          SnapshotFileName = myPageView->SnapshotView3(
            QImagePageViewTracer::PNG);
          break;
          }
        case 4:
          {
          SnapshotFileName = myPageView->SnapshotViewXYZ(
            QImagePageViewTracer::PNG);
          break;
          }
        }
      statusbar->showMessage(
        tr("%1 has been saved").arg(SnapshotFileName) );
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::SetContourTracerOn(const bool& iChecked)
{
  if( iChecked )
    {
    int idx = this->CentralImageTabWidget->currentIndex();
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
    if( myPageView )
      {
      myPageView->SetTracerON();
      return;
      }
    QImagePageView4DTracer* myPageView2 =
      dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
    if( myPageView2 )
      {
      myPageView2->SetTracerON();
      return;
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::SetSeedWidgetOn(const bool& iChecked)
{
  if( iChecked )
    {
    int idx = this->CentralImageTabWidget->currentIndex();
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
    if( myPageView )
      {
      myPageView->SetSeedingON();
      return;
      }
    QImagePageView4DTracer* myPageView2 =
      dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
    if( myPageView2 )
      {
      myPageView2->SetSeedingON();
      return;
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::SetSeedWidgetOff(const bool& iChecked)
{
  if( iChecked )
    {
    int idx = this->CentralImageTabWidget->currentIndex();
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
    if( myPageView )
      {
      myPageView->SetSeedingOFF();
      return;
      }
    QImagePageView4DTracer* myPageView2 =
      dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
    if( myPageView2 )
      {
      myPageView2->SetSeedingOFF();
      return;
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::SetContourTracerOff(const bool& iChecked)
{
  if( iChecked )
    {
    int idx = this->CentralImageTabWidget->currentIndex();
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
    if( myPageView )
      {
      myPageView->SetTracerOFF();
      return;
      }
    QImagePageView4DTracer* myPageView2 =
      dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
    if( myPageView2 )
      {
      myPageView2->SetTracerOFF();
      return;
      }
    }
}
//--------------------------------------------------------------------------------

// void QGoMainWindow::SetTracerToPolygonTracer()
// {
//   m_PageView->SetTracerToPolygon();
// }

// *************************************************************************
// void QGoMainWindow::SetTracerToFreeLineTracer()
// {
//   m_PageView->SetTracerToFreeLine();
// }

// *************************************************************************
// void QGoMainWindow::SetTracerToTranslateContourTracer()
// {
//   m_PageView->SetTracerToTranslateExistingContour();
// }

// *************************************************************************
// void QGoMainWindow::SetTracerToRotateContourTracer()
// {
//   m_PageView->SetTracerToRotateExistingContour();
// }

// *************************************************************************
// void QGoMainWindow::SetTracerToScaleContourTracer()
// {
//   m_PageView->SetTracerToScaleExistingContour();
// }


//--------------------------------------------------------------------------------
void QGoMainWindow::ChangeLookupTable( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  QImagePageViewTracer* myPageView =
    dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
  if( myPageView )
    {
    myPageView->SetLookupTable( this->m_LUTDialog->GetLookupTable() );
    }
  else
    {
    QImagePageView4DTracer* myPageView2 =
      dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
    if( myPageView2 )
      {
      myPageView2->SetLookupTable( this->m_LUTDialog->GetLookupTable() );
      return;
      }
    }
}

//--------------------------------------------------------------------------------
void QGoMainWindow::SetColorForGivenId( const bool& iSelect )
{
  unsigned int cell_id = IdContourBox->value();

  vnl_random random( 12 );
  QColor color;

  if( cell_id < 7 )
    {
    switch( cell_id )
      {
    default:
      case 1:
        color.setHsv( 0, 255, 255 );
        break;
      case 2:
        color.setHsv( 60, 255, 255 );
        break;
      case 3:
        color.setHsv( 120, 255, 255 );
        break;
      case 4:
        color.setHsv( 180, 255, 255 );
        break;
      case 5:
        color.setHsv( 240, 255, 255 );
        break;
      case 6:
        color.setHsv( 300, 255, 255 );
        break;
      }
    }
  else
    {
    switch( cell_id % 6 )
      {
      case 0:
        color.setHsv( random.lrand32( 0, 60 ),
          random.lrand32( 0, 255 ),
          random.lrand32( 0, 255 ) );
        break;
      case 1:
        color.setHsv( random.lrand32( 60, 120 ),
          random.lrand32( 0, 255 ),
          random.lrand32( 0, 255 ) );
        break;
      case 2:
        color.setHsv( random.lrand32( 120, 180 ),
          random.lrand32( 0, 255 ),
          random.lrand32( 0, 255 ) );
        break;
      case 3:
        color.setHsv( random.lrand32( 180, 240 ),
          random.lrand32( 0, 255 ),
          random.lrand32( 0, 255 ) );
        break;
      case 4:
        color.setHsv( random.lrand32( 240, 300 ),
          random.lrand32( 0, 255 ),
          random.lrand32( 0, 255 ) );
        break;
      case 5:
        color.setHsv( random.lrand32( 300, 360 ),
          random.lrand32( 0, 255 ),
          random.lrand32( 0, 255 ) );
        break;
      }
    }

  if( iSelect )
    {
    m_IdColorMap[ cell_id ] = QColorDialog::getColor( color, this );
    }
  else
    {
    m_IdColorMap[ cell_id ] = color;
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::ValidateContourTracer( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  QImagePageViewTracer* myPageView =
    dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
  if( myPageView )
    {
    ValidateContourTracerHelper< QImagePageViewTracer >( myPageView );
    }
  else
    {
    QImagePageView4DTracer* myPageView2 =
      dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
    if( myPageView2 )
      {
      ValidateContourTracerHelper< QImagePageView4DTracer >( myPageView2 );
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::ReinitializeAndIncrementContourTracer( )
{
  ReinitializeContourTracer( );
  this->IdContourBox->setValue( 1 + this->IdContourBox->value() );
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
template< class T >
void QGoMainWindow::ValidateContourTracerHelper( T* PageView )
{
  unsigned int cell_id = IdContourBox->value();

  if( m_IdColorMap.find( cell_id ) == m_IdColorMap.end() )
    {
    SetColorForGivenId( false );
    }
  //DATABASE
  if( m_Wizard )
    {
    PageView->SetDatabaseRelatedVariables( m_Wizard->Server(), m_Wizard->login(),
      m_Wizard->Password(), m_Wizard->NameDB(), m_Wizard->ExpID(),
      m_Wizard->ExpName() );
    }
  PageView->ValidateContour(
    cell_id,
    m_IdColorMap[ cell_id ],
    this->SaveContourCheckBox->isChecked() );
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::ReinitializeContourTracer()
{
  int idx = this->CentralImageTabWidget->currentIndex();
  QImagePageViewTracer* myPageView =
    dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
  if( myPageView )
    {
    myPageView->ReinitializeContour();
    }
  else
    {
    QImagePageView4DTracer* myPageView2 =
      dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
    if( myPageView2 )
      {
      myPageView2->ReinitializeContour();
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::SetFileName( const QString& iFile, const bool& IsSerie )
{
  if( QFile::exists( iFile ) )
    {
    this->setCurrentFile( iFile, IsSerie );

    // parse extension
    QString ext = QFileInfo( iFile ).suffix();
    if( ext.compare( "lsm", Qt::CaseInsensitive ) == 0 )
      {
      this->OpenAndDisplay( m_CurrentFile, IsSerie, 0 );
      }
    else
      {
      if( IsSerie )
        {
        this->OpenAndDisplay( m_CurrentFile, IsSerie, 1 );
        }
      else
        {
        this->OpenImage( m_CurrentFile );
        this->DisplayImage( m_CurrentFile );
        }
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::OpenImage( const QString& iFile )
{
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef ImageReaderType::Pointer          ImageReaderPointer;

  ImageReaderPointer reader = ImageReaderType::New();
  reader->SetFileName( iFile.toAscii( ).data( ) );

  //BUG 03/23: This next line is commented for the time being since
  // it makes gofigure crashing.
  // this->ShowProgressLoading( reader );
  reader->Update();
  this->HideProgressLoading();

  m_ITKImage.push_back( reader->GetOutput() );
  m_ITKImage.last()->DisconnectPipeline();
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::DisplayImage( const QString& iTag )
{
  m_Convert.push_back( VTKConvertImageType::New() );
  m_Convert.last()->SetInput( m_ITKImage.last() );
  ShowProgressLoading( m_Convert.last() );
  m_Convert.last()->Update();
  HideProgressLoading();

  m_VTKImage.push_back( m_Convert.last()->GetOutput() );

  m_PageView.push_back( new QImagePageViewTracer );
  QImagePageViewTracer* myPageView =
    dynamic_cast<QImagePageViewTracer*>( m_PageView.last() );
  if( myPageView )
    {
    myPageView->SetImage( m_VTKImage.last() );
    }

  int idx = this->CentralImageTabWidget->addTab( m_PageView.last(), iTag );
  this->CentralImageTabWidget->setCurrentIndex( idx );
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::OpenAndDisplay(
  const QString& iTag,
  const bool& IsSerie,
  const int& Type )
{
  m_PageView.push_back( new QImagePageView4DTracer );
  QImagePageView4DTracer* myPageView;
  myPageView = static_cast< QImagePageView4DTracer*>( m_PageView.last() );
  if( IsSerie )
    {
    myPageView->SetFileTypeToSerie( );
    if( Type == 0 ) myPageView->SetSerieTypeToLsm( );
    if( Type == 1 ) myPageView->SetSerieTypeToMegaCapture( );
    }
  myPageView->SetFileName( iTag.toAscii( ).data( ) );

  int idx = this->CentralImageTabWidget->addTab( m_PageView.last(), iTag );
  this->CentralImageTabWidget->setCurrentIndex( idx );
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionAbout_activated( )
{
  // NOTE ALEX: is there anyway to link that with version definition
  // in the cmakelist.txt?
  QString version( tr( "v0.5" ) );

  QString about_gofigure( tr( "GoFigure V2 \n\n" ) );

  QString authors( tr( "Authors in alphabetical order:\n" ) );
  authors.append( " * 2008 ~ A. Gelas\n" );
  authors.append( " * 2007 ~ A. Gouaillard\n" );
  authors.append( " * 2008 ~ K. Mosaliganti\n" );
  authors.append( " * 2008 ~ L. Souhait\n\n" );
  authors.append( " * Principal Investigator\n" );
  authors.append( " * S. Megason\n" );

  QString message = QString( "GoFigure V2 %1\n\n" ).arg( version );
  message.append( about_gofigure );
  message.append( authors );

  QMessageBox::about( this, tr( "<*)0|00|0>< About GoFigure" ), message );
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionAbout_Qt_activated( )
{
  QMessageBox::aboutQt( this, tr( "About Qt" ) );
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::Fullscreenbuttons()
{
  m_FullscreenGroup = new QActionGroup (this);
  m_FullscreenGroup->addAction( this->actionFull_screen_XY );
  m_FullscreenGroup->addAction( this->actionFull_screen_YZ );
  m_FullscreenGroup->addAction( this->actionFull_screen_XZ );
  m_FullscreenGroup->addAction( this->actionFull_screen_XYZ );
  m_FullscreenGroup->addAction( this->actionQuad_View );
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::setCurrentFile(const QString &fileName,const bool& IsSerie)
{
  m_CurrentFile = fileName;
  this->setWindowModified( false );
  QString shownName = "Untitled";
  if( !m_CurrentFile.isEmpty() )
    {
    shownName = strippedName( m_CurrentFile );
    if (IsSerie)
      {
      m_RecentMultipleFiles.removeAll( m_CurrentFile );
      m_RecentMultipleFiles.prepend( m_CurrentFile );
      updateRecentFileActions(m_RecentMultipleFiles, menuMultiple_Files, recentMultipleFileActions);
      }
    else
      {
      m_RecentSingleFiles.removeAll( m_CurrentFile );
      m_RecentSingleFiles.prepend( m_CurrentFile );
      updateRecentFileActions(m_RecentSingleFiles,menuSingle_Files, recentSingleFileActions);
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
QString QGoMainWindow::strippedName(const QString &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::updateRecentFileActions(QStringList list, QMenu *menu, QAction *recentFileActions[MaxRecentFiles] )
{
  QMutableStringListIterator i(list);
  while (i.hasNext())
    {
    if (!QFile::exists(i.next()))
      i.remove();
    }
  if (!list.isEmpty())
    {
    menu->setEnabled(true);
    }

  for (int j = 0; j < MaxRecentFiles; ++j)
    {
    if (j < list.count())
      {
      QString text = tr("&%1 %2 ")
        .arg(j + 1)
        .arg(strippedName(list[j]));

      recentFileActions[j]->setText(text);
      recentFileActions[j]->setData(list[j]);
      recentFileActions[j]->setVisible(true);
      menu->addAction(recentFileActions[j]);
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::openRecentFile(const bool& IsSerie)
{
  QAction* action = qobject_cast< QAction* >( sender() );
  if( action )
    {
    SetFileName( action->data().toString(), IsSerie );
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::openRecentSingleFile()
{
  openRecentFile(false);
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::openRecentMultipleFile()
{
  openRecentFile(true);
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::readSettings()
{
  QSettings settings( "MegasonLab", "Gofigure2" );
  m_RecentSingleFiles = settings.value( "RecentSingleFiles" ).toStringList( );
  m_RecentMultipleFiles = settings.value( "RecentMultipleFiles" ).toStringList( );
  updateRecentFileActions(m_RecentSingleFiles, menuSingle_Files, recentSingleFileActions );
  updateRecentFileActions(m_RecentMultipleFiles, menuMultiple_Files, recentMultipleFileActions );

  settings.beginGroup( "MainWindow" );
  QSize size = settings.value( "size" ).toSize();

  if( size.isValid() )
    {
    this->resize( size );
    this->move( settings.value("pos").toPoint() );
    }
  else
    {
    this->resize( 1450, 750 );
    }

  //  settings.setValue("vsplitterSizes", vSplitter->saveState());
  settings.endGroup();
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::writeSettings()
{
  QSettings settings;
  settings.setValue("RecentSingleFiles", m_RecentSingleFiles);
  settings.setValue("RecentMultipleFiles", m_RecentMultipleFiles);
  settings.beginGroup("MainWindow");
  settings.setValue("size", size());
  settings.setValue("pos", pos());
  settings.endGroup();
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
template< class T >
void QGoMainWindow::writeSettingsPageView(T* PageView )
{
  PageView->SaveStateSplitters();
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::ShowProgressLoading( itk::Object * myFilter )
{
  m_Bar.setValue(50);
  m_Bar.show();
  m_Bar.Observe( myFilter );

  myFilter->AddObserver( itk::EndEvent(),  m_SignalAdaptor.GetCommand() );
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::HideProgressLoading()
{
  m_Bar.hide();
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::UpdateTracerButtons( const int& idx)
{
  if( (idx>=0) && (idx<m_PageView.size()))
    {
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
    if( myPageView )
      {
      this->IdContourBox->setValue( myPageView->GetCellId() );
      if( myPageView->GetTracerStatus( ) )
        {
        this->ManualSegmentationOnRadioBtn->toggle();
        }
      else
        {
        this->ManualSegmentationOffRadioBtn->toggle();
        }
      }

    QImagePageView4DTracer* myPageView2 =
      dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
    if( myPageView2 )
      {
      this->IdContourBox->setValue( myPageView2->GetCellId() );
      if( myPageView2->GetTracerStatus( ) )
        {
        this->ManualSegmentationOnRadioBtn->toggle();
        }
      else
        {
        this->ManualSegmentationOffRadioBtn->toggle();
        }
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::UpdateToolBarViewButtons( const int& idx )
{
  if( (idx>=0) && (idx<m_PageView.size()))
    {
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
    if( myPageView )
      {
      UpdateFullScreenViewButtonsHelper( myPageView );
      UpdateVolumeRenderingButton( myPageView );
      }
    else
      {
      QImagePageView4DTracer* myPageView2 =
        dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
      if( myPageView2 )
        {
        UpdateFullScreenViewButtonsHelper( myPageView2 );
        UpdateVolumeRenderingButton ( myPageView2 );
        }
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
template< class T >
void QGoMainWindow::UpdateFullScreenViewButtonsHelper( T* PageView )
{
  int whichview = PageView->GetFullScreenView();
  switch( whichview )
    {
    default:
    case 0:
      {
      this->actionQuad_View->setChecked(true);
      actionSnapshot->setEnabled(false);
      break;
      }
    case 1:
      {
      this->actionFull_screen_XY->setChecked(true);
      actionSnapshot->setEnabled(true);
      break;
      }
    case 2:
      {
      this->actionFull_screen_XZ->setChecked(true);
      actionSnapshot->setEnabled(true);
      break;
      }
    case 3:
      {
      this->actionFull_screen_YZ->setChecked(true);
      actionSnapshot->setEnabled(true);
      break;
      }
    case 4:
      {
      this->actionFull_screen_XYZ->setChecked(true);
      actionSnapshot->setEnabled(true);
      break;
      }
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
template< class T >
void QGoMainWindow::UpdateVolumeRenderingButton( T* PageView)
{
  actionVolume_rendering_XYZ->setChecked( PageView->GetVolumeRendering() );
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::StartSeedWidget()
{

}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::OneClickSegmentation()
{
#ifdef ITKLEVELSETCODE

  int idx = this->CentralImageTabWidget->currentIndex();
  QImagePageViewTracer* myPageView =
    dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );

  if( m_ITKImage[idx].IsNotNull() )
    {
    LocalChanAndVeseSegmentationFilterType::Pointer localsegmentation =
      LocalChanAndVeseSegmentationFilterType::New();
    localsegmentation->SetFeatureImage( m_ITKImage[idx] );
    localsegmentation->SetRadius( this->RadiusSpinBox->value() );

    LevelSetImageType::PointType itk_pt;
    double* pos = itk_pt.GetDataPointer();
    std::list< vtkPolyData* > results;
    std::list< vtkProperty* > property_list;

    if( myPageView )
      {
      vtkPoints* seeds = myPageView->GetAllSeeds();
      for( int i = 0; i < seeds->GetNumberOfPoints(); i++ )
        {
        seeds->GetPoint( i, pos );
        localsegmentation->SetCenter( itk_pt );
        localsegmentation->Update();

        vtkImageData* image = localsegmentation->GetOutput();

        // create iso-contours
        vtkMarchingCubes *contours = vtkMarchingCubes::New();
        contours->SetInput( image );
        contours->GenerateValues ( 1, 0, 0 );

        results.push_back( contours->GetOutput() );
        property_list.push_back( vtkProperty::New() );
//         std::cout <<i <<" [" <<pos[0] <<" " <<pos[1] <<" " <<pos[2] <<"]" <<std::endl;
        }
      myPageView->AddContours( results, property_list );
  //  myPageView->ClearAllSeeds();
      }

    QImagePageView4DTracer* myPageView2 =
      dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
    if( myPageView2 )
      {
      vtkPoints* seeds = myPageView2->GetAllSeeds();
      for( int i = 0; i < seeds->GetNumberOfPoints(); i++ )
        {
        seeds->GetPoint( i, pos );
        std::cout <<i <<" [" <<pos[0] <<" " <<pos[1] <<" " <<pos[2] <<"]" <<std::endl;
        }
      }
    }
#endif
}
//--------------------------------------------------------------------------------
