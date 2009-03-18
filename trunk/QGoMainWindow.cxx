#include "QGoMainWindow.h"

#include <iostream>

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qcolordialog.h>
#include <qprogressdialog.h>
#include <qprogressbar.h>

#include <itkImageFileReader.h>
#include <vnl/vnl_random.h>

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
QGoMainWindow::QGoMainWindow( ) : m_PageView( 1 )
{
  this->setupUi( this );
  this->setCentralWidget( this->CentralImageTabWidget );
  this->setWindowTitle( tr( "~~>=O~~ GoFigure ~~O=<~~") );
  this->statusbar->showMessage( tr( "No data" ) );
  this->CentralImageTabWidget->clear();
  this->KishoreSegDockWidget->setVisible(false);
  this->ManualSegmentationDockWidget->setVisible(false);

#if QT_VERSION_MAJOR == 4 && QT_VERSION_MINOR >= 5
  //NOTE: The next properties appear in Qt 4.5.
  this->CentralImageTabWidget->setTabsClosable( true );
  this->CentralImageTabWidget->setTabsMovable( true );
#endif

  m_Convert = VTKConvertImageType::New();

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
  QObject::connect( this->actionOpen, SIGNAL( activated( ) ),
    this, SLOT( showprogressloading() ) );

  Fullscreenbuttons();


  for (int i = 0; i < MaxRecentFiles; ++i)
  {
    recentFileActions[i] = new QAction(this);
    recentFileActions[i]->setVisible(false);
    connect(this->recentFileActions[i], SIGNAL(triggered()),
      this, SLOT(openRecentFile()));
  }
  //menuOpen_Recent_Files->setVisible(false);
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
QGoMainWindow::~QGoMainWindow()
{
  while( !m_PageView.empty() )
  {
    delete m_PageView.back();
    m_PageView.pop_back();
  }
  delete m_LUTDialog;
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionOpen_activated( )
{
  // NOTE: Handling symbolic links will be done later
  // (that way it requires much more code than expected)
  // QFileDialog filedialog( this, tr( "Select Image" ) );
  // filedialog.setResolveSymlinks( true );
  QString filename = QFileDialog::getOpenFileName( this, tr( "Select Image" ) );

  if( !filename.isEmpty() )
  {
    SetFileName( filename );
  }
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionClose_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  this->CentralImageTabWidget->removeTab( idx );
  delete m_PageView[idx];
  m_PageView.remove( idx );
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionClose_all_activated( )
{
  this->CentralImageTabWidget->clear( );
  while( !m_PageView.empty() )
  {
    delete m_PageView.back();
    m_PageView.pop_back();
  }
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionQuit_activated( )
{
  this->close();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionBackground_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->SetBackgroundColor( QColorDialog::getColor( Qt::black, this ) );
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionLookup_Table_activated( )
{
  m_LUTDialog->show();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionQuad_View_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->quadview();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_XY_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->FullScreenViewXY();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_YZ_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->FullScreenView3();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_XZ_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->FullScreenView2();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_XYZ_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->FullScreenViewXYZ();
}

void QGoMainWindow::on_actionVolume_rendering_XYZ_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  if (actionVolume_rendering_XYZ->isChecked())
    m_PageView[idx]->SetView3DToVolumeRenderingMode();
  else
    m_PageView[idx]->SetView3DToTriPlanarMode();
}

void QGoMainWindow::on_actionScale_bars_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  if (actionScale_bars->isChecked())
    m_PageView[idx]->SetShowScalarBar(true);
  else
    m_PageView[idx]->SetShowScalarBar(false);
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::SetContourTracerOn(const bool& iChecked)
{
  if( iChecked )
  {
    int idx = this->CentralImageTabWidget->currentIndex();
    m_PageView[idx]->SetTracerON();
  }
}

void QGoMainWindow::SetContourTracerOff(const bool& iChecked)
{
  if( iChecked )
  {
    int idx = this->CentralImageTabWidget->currentIndex();
    m_PageView[idx]->SetTracerOFF();
  }
}
// void QGoMainWindow::SetTracerToPolygonTracer()
// {
//   m_PageView->SetTracerToPolygon();
// }

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// void QGoMainWindow::SetTracerToFreeLineTracer()
// {
//   m_PageView->SetTracerToFreeLine();
// }

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// void QGoMainWindow::SetTracerToTranslateContourTracer()
// {
//   m_PageView->SetTracerToTranslateExistingContour();
// }

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// void QGoMainWindow::SetTracerToRotateContourTracer()
// {
//   m_PageView->SetTracerToRotateExistingContour();
// }

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
// void QGoMainWindow::SetTracerToScaleContourTracer()
// {
//   m_PageView->SetTracerToScaleExistingContour();
// }
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

void QGoMainWindow::ChangeLookupTable( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->SetLookupTable( this->m_LUTDialog->GetLookupTable() );
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::SetColorForGivenId( const bool& iSelect )
{
  unsigned int cell_id = IdContourBox->value();

  //   m_PageView->
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
        color.setHsv( random.lrand32( 0, 60 ), random.lrand32( 0, 255 ), random.lrand32( 0, 255 ) );
        break;
      case 1:
        color.setHsv( random.lrand32( 60, 120 ), random.lrand32( 0, 255 ), random.lrand32( 0, 255 ) );
        break;
      case 2:
        color.setHsv( random.lrand32( 120, 180 ), random.lrand32( 0, 255 ), random.lrand32( 0, 255 ) );
        break;
      case 3:
        color.setHsv( random.lrand32( 180, 240 ), random.lrand32( 0, 255 ), random.lrand32( 0, 255 ) );
        break;
      case 4:
        color.setHsv( random.lrand32( 240, 300 ), random.lrand32( 0, 255 ), random.lrand32( 0, 255 ) );
        break;
      case 5:
        color.setHsv( random.lrand32( 300, 360 ), random.lrand32( 0, 255 ), random.lrand32( 0, 255 ) );
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
// *****************************************************************************
// *****************************************************************************

void QGoMainWindow::ValidateContourTracer( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  unsigned int cell_id = IdContourBox->value();
  m_PageView[idx]->SetCellId( cell_id );

  if( m_IdColorMap.find( cell_id ) == m_IdColorMap.end() )
    SetColorForGivenId( false );

  m_PageView[idx]->ValidateContour( m_IdColorMap[ cell_id ] );
}
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************

void QGoMainWindow::ReinitializeContourTracer()
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->ReinitializeContour();
}
// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::SetFileName( const QString& iFile )
{
  if( QFile::exists( iFile ) )
  {
    setCurrentFile( iFile );
    OpenImage( m_CurrentFile );
    DisplayImage( m_VTKImage, m_CurrentFile );
  }
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::OpenImage( const QString& iFile )
{
  // NOTE ALEX: here have to check the file
  // and possibly define ImageType on the fly
  // then define a templated function to pass the image along
  // just as the cellinterface test for QEMesh
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef ImageReaderType::Pointer ImageReaderPointer;

  ImageReaderPointer reader = ImageReaderType::New();
  reader->SetFileName( iFile.toAscii( ).constData( ) );
  reader->Update();

  m_ITKImage = reader->GetOutput();
  m_ITKImage->DisconnectPipeline();

  this->statusbar->showMessage( iFile );
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::DisplayImage( vtkImageData* iImage, QString iTag )
{
  m_Convert->SetInput( m_ITKImage );
  m_Convert->Update();

  m_VTKImage = m_Convert->GetOutput();

  //NOTE 13 March 2009: Dirty temporary tricks to handle the change of image
  if( m_PageView[0] )
  {
    delete m_PageView[0];
    m_PageView[0] = 0;
  }
  if( !m_PageView[0] )
  {
    m_PageView[0] = new QImagePageViewTracer;
  }
  m_PageView[0]->SetImage( m_VTKImage );
  //   m_PageView->SetScalarBarVisibility( true );

  int idx = this->CentralImageTabWidget->addTab( this->m_PageView[0], iTag );
  this->CentralImageTabWidget->setCurrentIndex( idx );
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionAbout_activated( )
{
  QString version( tr( "v2.0" ) );

  QString about_gofigure( tr( "GoFigure rocks!!! it rocks, rules!!!\n\n" ) );

  QString authors( tr( "Authors in alphabetical order:\n" ) );
  authors.append( " * Arnaud Gelas\n" );
  authors.append( " * Alexandre Gouaillard\n" );
  authors.append( " * Sean Megason\n" );
  authors.append( " * Kishore Mosaliganti\n" );
  authors.append( " * Lydie Souhait\n" );

  QString message = QString( "GoFigure %1\n\n" ).arg( version );
  message.append( about_gofigure );
  message.append( authors );

  QMessageBox::about( this, tr( "About GoFigure" ), message );
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionAbout_Qt_activated( )
{
  QMessageBox::aboutQt( this, tr( "About Qt" ) );
}

void QGoMainWindow::Fullscreenbuttons()
{
  m_FullscreenGroup = new QActionGroup (this);
  m_FullscreenGroup->addAction( this->actionFull_screen_XY );
  m_FullscreenGroup->addAction( this->actionFull_screen_YZ );
  m_FullscreenGroup->addAction( this->actionFull_screen_XZ );
  m_FullscreenGroup->addAction( this->actionFull_screen_XYZ );
  m_FullscreenGroup->addAction( this->actionQuad_View );
}

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

QString QGoMainWindow::strippedName(const QString &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}

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
    else
    {
      recentFileActions[j]->setVisible(false);
    }
  }

}


void QGoMainWindow::openRecentFile()
{
  QAction *action = qobject_cast<QAction *>(sender());
  if (action)
  {
    if( m_PageView[0] )
    {
      delete m_PageView[0];
      m_PageView[0] = 0;
    }
    SetFileName(action->data().toString());
  }
}
void QGoMainWindow::showprogressloading ()
{
  QProgressDialog* Progressloading = new QProgressDialog("Loading images...", "Abort loading", 0, 100, this,Qt::WindowStaysOnTopHint);
  QProgressBar* bar = new QProgressBar(this);
  QLabel* message = new QLabel(this);
  message->setText("Image loading...");
  message->setAlignment(Qt::AlignLeft);
  Progressloading->setBar(bar);
  statusbar->addWidget(message,0);
  statusbar->addWidget(bar,0);
  //statusbar->insertWidget(1,bar,0);

   //Progressloading.setWindowModality(Qt::WindowModal);

     for (int i = 0; i < 100; i++)
     {
       Progressloading->setValue(i);

       if (Progressloading->wasCanceled())
       {
          break;
       }

       bar->update();
       bar->show();

     }
     Progressloading->setValue(100);
}
