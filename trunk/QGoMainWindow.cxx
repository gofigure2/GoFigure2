#include "QGoMainWindow.h"

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

// *****************************************************************************
QGoMainWindow::QGoMainWindow( ):
m_Bar( this, "Progress")
{
  this->setupUi( this );
  this->setCentralWidget( this->CentralImageTabWidget );
  this->setWindowTitle( tr( "~~>=O~~ GoFigure ~~O=<~~") );
  this->statusbar->showMessage( tr( "No data" ) );
  this->CentralImageTabWidget->clear();
  this->KishoreSegDockWidget->setVisible(false);
  this->ManualSegmentationDockWidget->setVisible(false);
  this->OneClickSegmentationDockWidget->setVisible(false);
  statusbar->addWidget(&(this->m_Bar),0);
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
  while( !m_PageView.empty() )
  {
    delete m_PageView.last();
    m_PageView.pop_back();
  }
  delete m_LUTDialog;
}

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
void QGoMainWindow::on_actionClose_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  this->CentralImageTabWidget->removeTab( idx );
  delete m_PageView[idx];
  m_PageView.remove( idx );
  writeSettings();
}

// *****************************************************************************
void QGoMainWindow::on_actionClose_all_activated( )
{
  this->CentralImageTabWidget->clear( );
  while( !m_PageView.empty() )
  {
    delete m_PageView.last();
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
  double* rgb = m_PageView[idx]->GetBackgroundColor();
  QColor color( static_cast< int >( 255. * rgb[0] ),
    static_cast< int >( 255. * rgb[1] ),
    static_cast< int >( 255. * rgb[2] ) );
  m_PageView[idx]->SetBackgroundColor( QColorDialog::getColor( color, this ) );
}

// *****************************************************************************
void QGoMainWindow::on_actionLookup_Table_activated( )
{
  m_LUTDialog->show();
}

// *****************************************************************************
void QGoMainWindow::on_actionQuad_View_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->quadview();
}

// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_XY_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->FullScreenViewXY();
}

// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_YZ_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->FullScreenView3();
}

// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_XZ_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->FullScreenView2();
}

// *****************************************************************************
void QGoMainWindow::on_actionFull_screen_XYZ_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->FullScreenViewXYZ();
}

// *****************************************************************************
void QGoMainWindow::on_actionVolume_rendering_XYZ_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  if (actionVolume_rendering_XYZ->isChecked())
    m_PageView[idx]->SetView3DToVolumeRenderingMode();
  else
    m_PageView[idx]->SetView3DToTriPlanarMode();
}

// *****************************************************************************
void QGoMainWindow::on_actionScale_bars_activated( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  if (actionScale_bars->isChecked())
    m_PageView[idx]->SetShowScalarBar(true);
  else
    m_PageView[idx]->SetShowScalarBar(false);
}

// *****************************************************************************
void QGoMainWindow::SetContourTracerOn(const bool& iChecked)
{
  if( iChecked )
  {
    int idx = this->CentralImageTabWidget->currentIndex();
    m_PageView[idx]->SetTracerON();
  }
}

// *****************************************************************************
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
  m_PageView[idx]->SetLookupTable( this->m_LUTDialog->GetLookupTable() );
}

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
void QGoMainWindow::ValidateContourTracer( )
{
  int idx = this->CentralImageTabWidget->currentIndex();
  unsigned int cell_id = IdContourBox->value();
  m_PageView[idx]->SetCellId( cell_id );

  if( m_IdColorMap.find( cell_id ) == m_IdColorMap.end() )
    SetColorForGivenId( false );

  m_PageView[idx]->ValidateContour(
    cell_id,
    m_IdColorMap[ cell_id ],
    this->SaveContourCheckBox->isChecked() );
}

// *****************************************************************************
void QGoMainWindow::ReinitializeContourTracer()
{
  int idx = this->CentralImageTabWidget->currentIndex();
  m_PageView[idx]->ReinitializeContour();
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
  reader->SetFileName( iFile.toAscii( ).constData( ) );
  this->ShowProgressLoading( reader );
  reader->Update();
  this->HideProgressLoading();

  m_ITKImage.push_back( reader->GetOutput() );
  m_ITKImage.last()->DisconnectPipeline();

}

// *****************************************************************************
void QGoMainWindow::DisplayImage( QString iTag )
{
  m_Convert.push_back( VTKConvertImageType::New() );
  m_Convert.last()->SetInput( m_ITKImage.last() );
  ShowProgressLoading( m_Convert.last() );
  m_Convert.last()->Update();
  HideProgressLoading();

  m_VTKImage.push_back( m_Convert.last()->GetOutput() );

  m_PageView.push_back( new QImagePageViewTracer );
  m_PageView.last()->SetImage( m_VTKImage.last() );

  int idx = this->CentralImageTabWidget->addTab( m_PageView.last(), iTag );
  this->CentralImageTabWidget->setCurrentIndex( idx );
}

// *****************************************************************************
void QGoMainWindow::OpenAndDisplayLSMFile( QString iTag, int timePoint, bool ComposeChannels )
{
  vtkImageData* readImage = this->OpenLSMFile( iTag, timePoint, ComposeChannels );
  m_VTKImage.push_back( readImage );

  m_PageView.push_back( new QImagePageViewTracer );
  m_PageView.last()->SetImage( m_VTKImage.last() );

  int idx = this->CentralImageTabWidget->addTab( m_PageView.last(), iTag );
  this->CentralImageTabWidget->setCurrentIndex( idx );
}

// *****************************************************************************
vtkImageData* QGoMainWindow::OpenLSMFile( QString iTag, int timePoint, bool ComposeChannels )
{
  vtkLSMReader* reader=vtkLSMReader::New();
  reader->SetFileName( iTag.toAscii( ).constData( ) );
  reader->SetUpdateTimePoint( timePoint );
  reader->Update();
  vtkImageData* myImage_ch1 = reader->GetOutput();
  int NumberOfChannels = reader->GetNumberOfChannels();
   
  if( !ComposeChannels || NumberOfChannels < 2 )
    {
    return( myImage_ch1 );
    }
  else
    {
    vtkImageData* myImage_ch2;
    vtkImageData* myImage_ch3;
      
    {
    vtkLSMReader* treader=vtkLSMReader::New();
    treader->SetFileName( iTag.toAscii( ).constData( ) );
    treader->SetUpdateTimePoint( timePoint );
    treader->SetUpdateChannel( 1 );
    treader->Update();
    myImage_ch2 = treader->GetOutput();
    }

    {
    vtkLSMReader* treader=vtkLSMReader::New();
    treader->SetFileName( iTag.toAscii( ).constData( ) );
    treader->SetUpdateTimePoint( timePoint );
    treader->SetUpdateChannel( 2 );
    treader->Update();
    int * dimensions = treader->GetDimensions();
    int  flatindex = dimensions[0] * dimensions[1] * dimensions[2];
    myImage_ch3 = treader->GetOutput();
    if( NumberOfChannels == 2 ) // dummy third channel 
      {
      // here we suppose the type to be char
      // to be improved
      char *ptr = (char*)( myImage_ch3->GetScalarPointer());
      for( int k=0; k < flatindex; k++ )
        {
        *ptr++ = 0; 
        }            
      }
    }
   
    vtkImageAppendComponents* appendFilter1 = vtkImageAppendComponents::New();
    appendFilter1->AddInput( myImage_ch1 );
    appendFilter1->AddInput( myImage_ch2 );
    appendFilter1->Update();

    vtkImageAppendComponents* appendFilter2 = vtkImageAppendComponents::New();
    appendFilter2->AddInput( appendFilter1->GetOutput() );
    appendFilter2->AddInput( myImage_ch3 );
    appendFilter2->Update(); 
    
    return( appendFilter2->GetOutput( ) );
    }
}

// *****************************************************************************
void QGoMainWindow::DisplayInTab( vtkImageData* myImage, int TabIndex )
{
  m_PageView[ TabIndex ]->SetImage( myImage );
}



// *****************************************************************************
void QGoMainWindow::on_actionAbout_activated( )
{
  QString version( tr( "v2.0" ) );

  QString about_gofigure( tr( "GoFigure V2 \n\n" ) );

  QString authors( tr( "Authors in alphabetical order:\n" ) );
  authors.append( " * 2008 ~ Arnaud Gelas\n" );
  authors.append( " * 2007 ~ Alexandre Gouaillard\n" );
  authors.append( " * 2002 ~ Sean Megason\n" );
  authors.append( " * 2008 ~ Kishore Mosaliganti\n" );
  authors.append( " * 2008 ~ Lydie Souhait\n" );

  QString message = QString( "GoFigure %1\n\n" ).arg( version );
  message.append( about_gofigure );
  message.append( authors );

  QMessageBox::about( this, tr( "About GoFigure" ), message );
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
    QSettings settings("MegasonLab", "Gofigure2");
    m_RecentFiles = settings.value("recentFiles").toStringList();
    updateRecentFileActions();
}

// *****************************************************************************
void QGoMainWindow::writeSettings()
{
    QSettings settings("MegasonLab", "Gofigure2");
    settings.setValue("recentFiles", m_RecentFiles);
}

// *****************************************************************************
void QGoMainWindow::ShowProgressLoading( itk::Object * myFilter )
{
  m_Bar.show();
  m_Bar.Observe( myFilter );

  typedef itk::QtSignalAdaptor SignalAdaptorType;
  SignalAdaptorType signalAdaptor;
  myFilter->AddObserver( itk::EndEvent(),  signalAdaptor.GetCommand() );
  QObject::connect( &signalAdaptor, SIGNAL(Signal()), &(this->m_Bar), SLOT(hide()) );
}

// *****************************************************************************
void QGoMainWindow::HideProgressLoading()
{
  m_Bar.hide();
}
