#include "QGoMainWindow.h"

#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qcolordialog.h>

#include <itkImageFileReader.h>
#include <iostream>

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
QGoMainWindow::QGoMainWindow( ) : m_PageView( 0 )
{
  this->setupUi( this );
  this->setCentralWidget( this->CentralImageTabWidget );
  this->setWindowTitle( tr( "~~>=O~~ GoFigure ~~O=<~~") );
  this->statusbar->showMessage( tr( "No data" ) );
  this->CentralImageTabWidget->clear();
  this->KishoreSegDockWidget->setVisible(false);
  this->ManualSegmentationDockWidget->setVisible(false);


  m_Convert = VTKConvertImageType::New();

  m_PageView = new QImagePageViewTracer;
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
  delete m_PageView;
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
  delete m_PageView;
  int idx = this->CentralImageTabWidget->currentIndex();
  this->CentralImageTabWidget->removeTab( idx );
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionClose_all_activated( )
{
  delete m_PageView;
  this->CentralImageTabWidget->clear( );
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
  m_PageView->SetBackgroundColor( QColorDialog::getColor( Qt::black, this ) );
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
  m_PageView->quadview();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_XY_activated( )
{
  m_PageView->FullScreenViewXY();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_YZ_activated( )
{
  m_PageView->FullScreenView3();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_XZ_activated( )
{
  m_PageView->FullScreenView2();
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_XYZ_activated( )
{
  m_PageView->FullScreenViewXYZ();
}

void QGoMainWindow::on_actionVolume_rendering_XYZ_activated( )
{
  if (actionVolume_rendering_XYZ->isChecked())
    m_PageView->SetView3DToVolumeRenderingMode();
  else
    m_PageView->SetView3DToTriPlanarMode();
}

void QGoMainWindow::on_actionScale_bars_activated( )
{
  if (actionScale_bars->isChecked())
    m_PageView->SetShowScalarBar(true);
  else
    m_PageView->SetShowScalarBar(false);
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::SetContourTracerOn(const bool& iChecked)
{
  if( iChecked )
    m_PageView->SetTracerON();
}

void QGoMainWindow::SetContourTracerOff(const bool& iChecked)
{
  if( iChecked )
    m_PageView->SetTracerOFF();
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
  m_PageView->SetLookupTable( this->m_LUTDialog->GetLookupTable() );
}

// *****************************************************************************
// *****************************************************************************
// *****************************************************************************
void QGoMainWindow::SetColorForGivenId( )
{
//   unsigned int cell_id = IdContourBox->value();
  //   m_PageView->
  //   m_IdColorMap[ cell_id ] =
  //     QColorDialog::getColor( Qt::red, this );

}

void QGoMainWindow::ValidateContourTracer( )
{
  m_PageView->SetCellId( IdContourBox->value() );
  m_PageView->ValidateContour();
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

  m_PageView->SetImage( m_VTKImage );
  //   m_PageView->SetScalarBarVisibility( true );

  int idx = this->CentralImageTabWidget->addTab( this->m_PageView, iTag );
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
    SetFileName(action->data().toString());
  }
}
void QGoMainWindow::showprogressloading ()
{
  QProgressDialog* Progressloading = new QProgressDialog("Loading images...", "Abort loading", 0, 100, this,Qt::WindowStaysOnTopHint);
  QProgressBar* bar = new QProgressBar(this);
  
   //Progressloading.setWindowModality(Qt::WindowModal);
  Progressloading->setBar(bar);
     for (int i = 0; i < 100; i++) 
     {
       Progressloading->setValue(i);

       if (Progressloading->wasCanceled())
             break;
       bar->update();
       bar->show();
      
     }
     Progressloading->setValue(100);
}