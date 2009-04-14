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

#include <itkImageFileReader.h>
#include <vnl/vnl_random.h>
#include <qsettings.h>

#include "vtkImageAppendComponents.h"
#include "ContourContainerFileSystem.h"

// *****************************************************************************
QGoMainWindow::QGoMainWindow( )
{
  this->setupUi( this );
  this->setCentralWidget( this->CentralImageTabWidget );
  this->setWindowTitle( tr( "<*)0|00|0>< ~~ <*)0|00|0><     GoFigure    ><0|00|0(*> ~~ ><0|00|0(*>") );
  this->statusbar->showMessage( tr( "No data" ) );
  this->CentralImageTabWidget->clear();
  this->KishoreSegDockWidget->setVisible(false);
  this->ManualSegmentationDockWidget->setVisible(false);
  this->OneClickSegmentationDockWidget->setVisible(false);
  this->statusbar->addPermanentWidget( &m_Bar );
  m_Bar.hide();
  //setCurrentFile("");

#if QT_VERSION_MAJOR == 4 && QT_VERSION_MINOR >= 5
  //NOTE: The next properties appear in Qt 4.5.
  this->CentralImageTabWidget->setTabsClosable( true );
  this->CentralImageTabWidget->setTabsMovable( true );
#endif

  m_LUTDialog = new QGoLUTDialog( this );

  QObject::connect( this->m_LUTDialog, SIGNAL( accepted( ) ),
    this, SLOT( ChangeLookupTable( ) ) );

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
  QObject::connect( this->ManualSegmentationOnRadioBtn, SIGNAL( toggled(bool) ),
    this, SLOT( SetContourTracerOn(bool) ) );
  QObject::connect( this->ManualSegmentationOffRadioBtn, SIGNAL( toggled(bool) ),
    this, SLOT( SetContourTracerOff(bool) ) );
  QObject::connect( this->IdContourColorBtn, SIGNAL( released( ) ),
    this, SLOT( SetColorForGivenId( ) ) );
  QObject::connect( this->TracerValidationBtn, SIGNAL( released( ) ),
    this, SLOT( ValidateContourTracer() ) );
  QObject::connect( this->TracerReinitializeBtn, SIGNAL( released() ),
    this, SLOT( ReinitializeContourTracer() ) );
  //QObject::connect( this->actionOpen, SIGNAL( activated( ) ),
    //this, SLOT( showprogressloading() ) );
  QObject::connect( this->CentralImageTabWidget, SIGNAL( currentChanged( int ) ),
    this, SLOT( UpdateFullScreenViewButtons( int ) ) );
  QObject::connect( this->CentralImageTabWidget, SIGNAL( currentChanged( int ) ),
    this, SLOT( UpdateTracerButtons( int ) ) );

  Fullscreenbuttons();

  for (int i = 0; i < MaxRecentFiles; ++i)
  {
    recentFileActions[i] = new QAction(this);
    recentFileActions[i]->setVisible(false);
    QObject::connect(this->recentFileActions[i], SIGNAL(triggered()),
      this, SLOT(openRecentFile()));
  }

 readSettings();
}

// *****************************************************************************
QGoMainWindow::~QGoMainWindow()
{
  writeSettings();
  while( !m_PageView.empty() )
  {
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView.last() );
    if( myPageView )
    {
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
}

// *****************************************************************************
void QGoMainWindow::on_actionOpen_activated( )
{
  QString filename = QFileDialog::getOpenFileName( 
    this, 
    tr( "Select Image" ),"",
    tr( "Images (*.png *.bmp *.jpg *.jpeg *.tiff *.mha *.mhd *img *.lsm)" ) 
    );
 
  if( !filename.isEmpty() )
    {
    SetFileName( filename );
    }  
}

// *****************************************************************************
void QGoMainWindow::on_actionClose_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  this->CentralImageTabWidget->removeTab( idx );
  if ( idx >= 0 )
  {
    QImagePageViewTracer* myPageView = dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
    if( myPageView )
    {
      delete myPageView;
    }
    else
    {
      QImagePageView4DTracer* myPageView4D = dynamic_cast<QImagePageView4DTracer*>( m_PageView[ idx ] );
      if( myPageView4D )
      {
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

// *****************************************************************************
void QGoMainWindow::on_actionClose_all_activated( )
{
  this->CentralImageTabWidget->clear( );
  while( !m_PageView.empty() )
    {
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView.last() );
    if( myPageView )
      {
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
    writeSettings();
    }
}



// *****************************************************************************
void QGoMainWindow::on_actionQuit_activated( )
{
  this->close();
  writeSettings();
}



// *****************************************************************************
void QGoMainWindow::on_actionBackground_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  QImagePageViewTracer* myPageView =
    dynamic_cast< QImagePageViewTracer* >( m_PageView[idx] );
  if( myPageView )
    {
    SetBackgroundColor< QImagePageViewTracer >( myPageView );
    return;
    }
  QImagePageView4DTracer* myPageView2 =
    dynamic_cast< QImagePageView4DTracer* >( m_PageView[idx] );
  if( myPageView2 )
    {
    SetBackgroundColor< QImagePageView4DTracer >( myPageView2 );
    return;
    }
}


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


// *****************************************************************************
void QGoMainWindow::on_actionLookup_Table_activated( )
{
  m_LUTDialog->show();
}



// *****************************************************************************
void QGoMainWindow::on_actionQuad_View_activated( )
{
  this->SetFullScreenDispatch( 0 );
  this->actionSnapshot->setEnabled(false);
}



// *****************************************************************************
template< class T >
void
QGoMainWindow::SetFullScreen( const int & ViewID, T* PageView )
{
  PageView->SetFullScreenView( ViewID );
}


// *****************************************************************************
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


// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_XY_activated( )
{
  this->SetFullScreenDispatch( 1 );
}

// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_YZ_activated( )
{
  this->SetFullScreenDispatch( 2 );
}

// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_XZ_activated( )
{
  this->SetFullScreenDispatch( 3 );
}

// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_XYZ_activated( )
{
  this->SetFullScreenDispatch( 4 );
}

// *****************************************************************************
void QGoMainWindow::on_actionVolume_rendering_XYZ_activated( )
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



// *****************************************************************************
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

// *****************************************************************************
void QGoMainWindow::on_actionScale_bars_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  QImagePageViewTracer* myPageView =
    dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
  if( myPageView )
  {
    if (actionScale_bars->isChecked())
      myPageView->SetShowScalarBar(true);
    else
      myPageView->SetShowScalarBar(false);
  }
}

// *****************************************************************************
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
      statusbar->showMessage( tr( "%1 has been saved").arg(SnapshotFileName) ) ;
    }
  }
}


// *****************************************************************************
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

// *****************************************************************************
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


// void QGoMainWindow::SetTracerToPolygonTracer()
// {
//   m_PageView->SetTracerToPolygon();
// }

// *****************************************************************************
// void QGoMainWindow::SetTracerToFreeLineTracer()
// {
//   m_PageView->SetTracerToFreeLine();
// }

// *****************************************************************************
// void QGoMainWindow::SetTracerToTranslateContourTracer()
// {
//   m_PageView->SetTracerToTranslateExistingContour();
// }

// *****************************************************************************
// void QGoMainWindow::SetTracerToRotateContourTracer()
// {
//   m_PageView->SetTracerToRotateExistingContour();
// }

// *****************************************************************************
// void QGoMainWindow::SetTracerToScaleContourTracer()
// {
//   m_PageView->SetTracerToScaleExistingContour();
// }


// *****************************************************************************
void QGoMainWindow::ChangeLookupTable( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  QImagePageViewTracer* myPageView =
    dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
  if( myPageView )
  {
    myPageView->SetLookupTable( this->m_LUTDialog->GetLookupTable() );
  }
}

// *****************************************************************************
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

// *****************************************************************************
void QGoMainWindow::ValidateContourTracer( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  QImagePageViewTracer* myPageView =
    dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
  if( myPageView )
    {
    ValidateContourTracerHelper< QImagePageViewTracer >( myPageView );
    return;
    }
  QImagePageView4DTracer* myPageView2 =
    dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
  if( myPageView2 )
    {
    ValidateContourTracerHelper< QImagePageView4DTracer >( myPageView2 );
    return;
    }
}

template< class T >
void QGoMainWindow::ValidateContourTracerHelper( T* PageView )
{
  unsigned int cell_id = IdContourBox->value();
  PageView->SetCellId( cell_id );

  if( m_IdColorMap.find( cell_id ) == m_IdColorMap.end() )
    {
    SetColorForGivenId( false );
    }
  //DATABASE
  PageView->ValidateContour(
    cell_id,
    m_IdColorMap[ cell_id ],
    this->SaveContourCheckBox->isChecked() );
}



// *****************************************************************************
void QGoMainWindow::ReinitializeContourTracer()
{
  int idx = this->CentralImageTabWidget->currentIndex();
  QImagePageViewTracer* myPageView =
    dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
  if( myPageView )
    {
    myPageView->ReinitializeContour();
    }
  QImagePageView4DTracer* myPageView2 =
    dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
  if( myPageView2 )
    {
    myPageView2->ReinitializeContour();
    }
}


// *****************************************************************************
void QGoMainWindow::SetFileName( const QString& iFile )
{
  if( QFile::exists( iFile ) )
  {
    this->setCurrentFile( iFile );
    // parse extension
    QFileInfo fi(iFile);
    QString ext = fi.suffix();
    if( ext.compare("lsm") == 0 )
    {
      this->OpenAndDisplayLSMFile( m_CurrentFile, 0, true );
    }
    else
    {
      this->OpenImage( m_CurrentFile );
      this->DisplayImage( m_CurrentFile );
    }
  }
}

// *****************************************************************************
void QGoMainWindow::OpenImage( const QString& iFile )
{
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef ImageReaderType::Pointer ImageReaderPointer;

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

// *****************************************************************************
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

// *****************************************************************************
void QGoMainWindow::OpenAndDisplayLSMFile( const QString& iTag,
  const int& timePoint,
  const bool& ComposeChannels )
{
  (void)(timePoint);
  (void)(ComposeChannels);

  m_PageView.push_back( new QImagePageView4DTracer );
  QImagePageView4DTracer* myPageView;
  myPageView = static_cast< QImagePageView4DTracer*>( m_PageView.last() );
  myPageView->SetFileName( iTag.toAscii( ).data( ) );

  int idx = this->CentralImageTabWidget->addTab( m_PageView.last(), iTag );
  this->CentralImageTabWidget->setCurrentIndex( idx );
}

// *****************************************************************************
void QGoMainWindow::on_actionAbout_activated( )
{
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

// *****************************************************************************
void QGoMainWindow::on_actionAbout_Qt_activated( )
{
  QMessageBox::aboutQt( this, tr( "About Qt" ) );
}

// *****************************************************************************
void QGoMainWindow::Fullscreenbuttons()
{
  m_FullscreenGroup = new QActionGroup (this);
  m_FullscreenGroup->addAction( this->actionFull_screen_XY );
  m_FullscreenGroup->addAction( this->actionFull_screen_YZ );
  m_FullscreenGroup->addAction( this->actionFull_screen_XZ );
  m_FullscreenGroup->addAction( this->actionFull_screen_XYZ );
  m_FullscreenGroup->addAction( this->actionQuad_View );
}

// *****************************************************************************
void QGoMainWindow::setCurrentFile(const QString &fileName)
{
  m_CurrentFile = fileName;
  this->setWindowModified( false );
  QString shownName = "Untitled";
  if( !m_CurrentFile.isEmpty() )
  {
    shownName = strippedName( m_CurrentFile );
    m_RecentFiles.removeAll( m_CurrentFile );
    m_RecentFiles.prepend( m_CurrentFile );
    updateRecentFileActions();
  }
}

// *****************************************************************************
QString QGoMainWindow::strippedName(const QString &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}

// *****************************************************************************
void QGoMainWindow::updateRecentFileActions()
{
  QMutableStringListIterator i(m_RecentFiles);
  while (i.hasNext())
  {
    if (!QFile::exists(i.next()))
      i.remove();
  }
  if (!m_RecentFiles.isEmpty())
  {
    menuOpen_Recent_Files->setEnabled(true);
  }

  for (int j = 0; j < MaxRecentFiles; ++j)
  {
    if (j < m_RecentFiles.count())
    {
      QString text = tr("&%1 %2 ")
        .arg(j + 1)
        .arg(strippedName(m_RecentFiles[j]));

      recentFileActions[j]->setText(text);
      recentFileActions[j]->setData(m_RecentFiles[j]);
      recentFileActions[j]->setVisible(true);
      menuOpen_Recent_Files->addAction(recentFileActions[j]);
    }
  }
}

// *****************************************************************************
void QGoMainWindow::openRecentFile()
{
  QAction* action = qobject_cast< QAction* >( sender() );
  if( action )
    {
    SetFileName(action->data().toString());
    }
}


// *****************************************************************************
void QGoMainWindow::readSettings()
{
  QSettings settings( "MegasonLab", "Gofigure2" );
  m_RecentFiles = settings.value( "recentFiles" ).toStringList( );
  updateRecentFileActions( );
  
  settings.beginGroup( "MainWindow" );
  
  if( settings.value( "size" ) != ( 0, 0 ) )
    {
    resize( settings.value( "size" ).toSize( ) );
    move(settings.value("pos").toPoint());
    }
  else
    {
    resize( 1450, 750 );
    }
  
   settings.endGroup();
}

// *****************************************************************************
void QGoMainWindow::writeSettings()
{
  QSettings settings("MegasonLab", "Gofigure2");
  settings.setValue("recentFiles", m_RecentFiles);
  settings.beginGroup("MainWindow");
  settings.setValue("size", size());
  settings.setValue("pos", pos());
  settings.endGroup();
}

// *****************************************************************************
void QGoMainWindow::ShowProgressLoading( itk::Object * myFilter )
{
  m_Bar.setValue(50);
  m_Bar.show();
  m_Bar.Observe( myFilter );

  myFilter->AddObserver( itk::EndEvent(),  m_SignalAdaptor.GetCommand() );

  QObject::connect( &m_SignalAdaptor, SIGNAL(Signal()),
    &(this->m_Bar), SLOT(hide()) );
}

// *****************************************************************************
void QGoMainWindow::HideProgressLoading()
{
  m_Bar.hide();
}

// *****************************************************************************
void QGoMainWindow::UpdateTracerButtons( const int& idx)
{
  if( (idx>=0) && (idx<m_PageView.size()))
    {
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
    if( myPageView )
      {
      if( myPageView->GetTracerStatus( ) )
        this->ManualSegmentationOnRadioBtn->toggle();
      else
        this->ManualSegmentationOffRadioBtn->toggle();
      }
    }
}
// *****************************************************************************
void QGoMainWindow::UpdateFullScreenViewButtons( const int& idx )
{
  if( (idx>=0) && (idx<m_PageView.size()))
    {
    QImagePageViewTracer* myPageView =
      dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
    if( myPageView )
      {
      UpdateFullScreenViewButtonsHelper( myPageView );
      return;
      }
    QImagePageView4DTracer* myPageView2 =
      dynamic_cast<QImagePageView4DTracer*>( m_PageView[idx] );
    if( myPageView2 )
      {
      UpdateFullScreenViewButtonsHelper( myPageView2 );
      return;
      }
    }
}

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
