/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Revision: 503 $  // Revision of last commit
  Date: $Date: 2009-08-03 17:42:52 -0400 (Mon, 03 Aug 2009) $  // Date of last commit
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
#include "QGoTabElementBase.h"
#include "QGoTabImageView2D.h"
#include "QGoTabImageView3D.h"
#include "QGoTabImageView3DwT.h"

// Plugin stuff
#include "QGoPluginHelper.h"
#include "QGoImageFilterPluginBase.h"

#include "itkQtAdaptor.h"
#include "itkQtProgressBar.h"

#include <iostream>

// Qt includes
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialog>
#include <QPluginLoader>
#include <QSettings>

// itk includes
#include "itkImageFileReader.h"

#include "itkLsm3DSerieImport.h"

// vtk includes
// #include "vtkImageAppendComponents.h"
// #include "vtkMarchingCubes.h"
#include "vtkPolyDataReader.h"
#include "vtkPLYReader.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"

#include "vtkImageReader2Factory.h"
#include "vtkImageReader2.h"

#include "QGoTabManager.h"

// *************************************************************************
QGoMainWindow::QGoMainWindow( )
{
  QString title( "<*)0|00|0>< ~~ <*)0|00|0><     GoFigure    ><0|00|0(*> ~~ ><0|00|0(*>");
  this->setupUi( this );
  this->setCentralWidget( this->CentralTabWidget );
  this->setWindowTitle( title );
  this->statusbar->showMessage( tr( "No data" ) );

  this->CentralTabWidget->clear();
  this->CentralTabWidget->setTabsClosable( true );
  this->CentralTabWidget->setMovable( true );

  this->statusbar->addPermanentWidget( &m_Bar );

  m_TabManager = new QGoTabManager( this, this->CentralTabWidget );

  this->m_ViewToolBar = new QToolBar( tr("View"), this );
  this->addToolBar( Qt::TopToolBarArea, this->m_ViewToolBar );

  m_LSMReader = vtkLSMReader::New();
  m_DBWizard  = new QGoWizardDB;
  m_DBWizard->hide();

  m_Bar.hide();
  QString temp;
  SetCurrentSingleFile( temp );
  SetCurrentMultiFile( temp );

  CreateSignalSlotsConnection();
  ReadSettings();
  LoadPlugins();
}

// *************************************************************************
QGoMainWindow::~QGoMainWindow()
{
  m_LSMReader->Delete();
  this->WriteSettings();
}
// *************************************************************************

// *************************************************************************
void QGoMainWindow::CreateSignalSlotsConnection()
{
  //QObject::connect( this->actionOpen, SIGNAL( activated( ) ),
    //this, SLOT( showprogressloading() ) );
  QObject::connect( this->CentralTabWidget,
    SIGNAL( tabCloseRequested( int ) ),
    m_TabManager, SLOT( CloseTab( int ) ) );

  QObject::connect( this->CentralTabWidget,
    SIGNAL( currentChanged( int ) ),
    m_TabManager, SLOT( ChangeCurrentTab( int ) ) );

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
    this->SetSingleFileName( filename );
    }
}

// *************************************************************************
void QGoMainWindow::on_actionOpen_Multiple_Files_activated( )
{
  QString filename = QFileDialog::getOpenFileName(
    this,
    tr( "Select one Image from the Dataset" ),"",
    tr( "Images (*.mha *.lsm)" )
    );

  if( !filename.isEmpty( ) )
    {
    if( QFile::exists( filename ) )
      {
      QString ext = QFileInfo( filename ).suffix();
      if( ext.compare( "lsm", Qt::CaseInsensitive ) == 0 )
        {
        std::cout <<"Multifile LSM" <<std::endl;

        itk::Lsm3DSerieImport::Pointer  importFileInfoList = itk::Lsm3DSerieImport::New();
        importFileInfoList->SetFileName( filename.toStdString() );
        importFileInfoList->SetGroupId( 1 );
        importFileInfoList->Update();

        std::cout <<"** " <<importFileInfoList->GetOutput()->size() <<std::endl;
        CreateNewTabFor3DwtImage( *(importFileInfoList->GetOutput()),
          itk::MultiFileReader::LSM, 0 );
        }
      else
        {
        if( ext.compare( "mha", Qt::CaseInsensitive ) == 0 )
          {
          std::cout <<"Multifile mha" <<std::endl;
//           CreateNewTabFor3DwtImage( filelist, MHA, 0 );
          }
        }
      }
    }
}


//--------------------------------------------------------------------------
void QGoMainWindow::on_actionUse_DataBase_activated()
{
  m_DBWizard->show();
}
//--------------------------------------------------------------------------

// *************************************************************************
void QGoMainWindow::openFilesfromDB()
{
  std::vector<std::vector<std::string> > listFilenames = 
    m_DBWizard->GetFilenamesFromDB();
}
// *************************************************************************

// *************************************************************************
void QGoMainWindow::on_actionClose_all_activated()
{
  m_TabManager->CloseAllTabs();
}
// *************************************************************************

// *************************************************************************
void QGoMainWindow::on_actionClose_activated()
{
  int idx = this->CentralTabWidget->currentIndex();
  m_TabManager->CloseTab( idx );
}
// *************************************************************************
void QGoMainWindow::on_actionOpen_Mesh_activated( )
{
  if( this->CentralTabWidget->count() > 0 )
    {
    int idx = this->CentralTabWidget->currentIndex();

    if( idx >= 0 )
    {
      QStringList filename = QFileDialog::getOpenFileNames(
        this,
        tr( "Select meshes or contours"), "",
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

//           QImagePageViewTracer* myPageView =
//             dynamic_cast<QImagePageViewTracer*>( m_PageView[idx] );
//           if( myPageView )
//             {
//             myPageView->AddContours( mesh_list, property_list, true, true );
//             // if( IsContour )
//             //{
//             //
//             // }
//             }
//           else
//             {
//             QImagePageView4DTracer* myPageView4D =
//               dynamic_cast<QImagePageView4DTracer*>( m_PageView[ idx ] );
//             if( myPageView4D )
//               {
//   //             myPageView4D->AddContours( mesh_list, property_list, true, true );
//               // if( IsContour )
//               //   {
//               //   }
//               // else
//               //  {
//               //  }
//               }
//             }
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
    }
  else
    {
    /// \note Do we need to create a view mesh if there is no opened image?
    QMessageBox::warning( this, tr( "Open Mesh / Contour Warning" ),
      tr( "One image needs to be opened first to be able to load contours or meshes" ) );
    }

}//--------------------------------------------------------------------------------

// *************************************************************************
void QGoMainWindow::on_actionQuit_activated( )
{
  this->close();
  this->WriteSettings();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::SetSingleFileName( const QString& iFile )
{
  if( QFile::exists( iFile ) )
    {
    this->SetCurrentSingleFile( iFile );

    // parse extension
    QString ext = QFileInfo( iFile ).suffix();
    if( ext.compare( "lsm", Qt::CaseInsensitive ) == 0 )
      {
      this->OpenLSMImage( m_CurrentFile, 0 );
      }
    else
      {
      vtkImageReader2Factory* r_factory = vtkImageReader2Factory::New();
      vtkImageReader2* reader = r_factory->CreateImageReader2( iFile.toAscii().data() );

      reader->SetFileName( iFile.toAscii().data() );
      reader->Update();

      vtkImageData* image = reader->GetOutput();

      int dim[3];
      image->GetDimensions( dim );

      if( ( dim[0] != 1 ) && ( dim[1] != 1 ) && ( dim[2] != 1 ) )
        {
        CreateNewTabFor3DImage( image, iFile );
        }
      else
        {
        CreateNewTabFor2DImage( image, iFile );
        }
      reader->Delete();
      r_factory->Delete();
      }

//       this->OpenImageWithITK( m_CurrentFile );
//       this->DisplayImage( m_CurrentFile );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::OpenLSMImage( const QString& iFile, const int& iTimePoint )
{
  m_LSMReader->SetFileName( iFile.toAscii().data() );
  m_LSMReader->SetUpdateTimePoint( iTimePoint );
  m_LSMReader->Update();

  int dim[5];
  m_LSMReader->GetDimensions( dim );

  int ImageDimensionality = 4;
//   bool Color = ( dim[4] > 1 );

  if( dim[3] == 1 ) // only one time point
    {
    if( dim[2] == 1 ) // only one z slice
      {
      ImageDimensionality = 2;
      }
    else
      {
      ImageDimensionality = 4;
      }
    }
  else
    {
    ImageDimensionality = 4;
    }

  switch( ImageDimensionality )
    {
    case 2:
      {
      CreateNewTabFor2DImage( m_LSMReader->GetOutput(), iFile );
      break;
      }
    case 3:
      {
      CreateNewTabFor3DImage( m_LSMReader->GetOutput(), iFile );
      break;
      }
    case 4:
      {
      CreateNewTabFor3DwtImage( m_LSMReader, iFile );
      break;
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/** \todo why not using iTimePoint instead of 0, in SetMultiFiles? */
void QGoMainWindow::CreateNewTabFor3DwtImage( FileListType& iFileList,
  const FILETYPE& iFileType, const int& iTimePoint )
{
  QGoTabImageView3DwT* w3t = new QGoTabImageView3DwT;
  w3t->SetMultiFiles( iFileList, iFileType, 0 );
  w3t->Update();

  for( std::list< QAction* >::iterator
    list_it = m_TabDimPluginActionMap[w3t->GetTabDimensionType()].begin();
    list_it != m_TabDimPluginActionMap[w3t->GetTabDimensionType()].end();
    list_it++
    )
    {
    (*list_it)->setEnabled( true );
    }

  w3t->SetPluginActions( m_TabDimPluginActionMap[w3t->GetTabDimensionType()] );

  std::list< QDockWidget* > dock_list = w3t->DockWidget();

  for( std::list< QDockWidget* >::iterator
    dck_it = dock_list.begin();
    dck_it != dock_list.end();
    ++dck_it )
    {
    this->addDockWidget( Qt::LeftDockWidgetArea, (*dck_it) );
    (*dck_it)->show();
    }

  int idx = this->CentralTabWidget->addTab( w3t, QString() );//iFile );
  this->menuView->setEnabled( true );
  this->menuFiltering->setEnabled( true );
  this->menuSegmentation->setEnabled( true );
  this->CentralTabWidget->setCurrentIndex( idx );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::CreateNewTabFor3DwtImage( vtkLSMReader* iReader, const QString& iFile )
{
  QGoTabImageView3DwT* w3t = new QGoTabImageView3DwT;
  w3t->SetLSMReader( iReader, 0 );
  w3t->Update();

  for( std::list< QAction* >::iterator
    list_it = m_TabDimPluginActionMap[w3t->GetTabDimensionType()].begin();
    list_it != m_TabDimPluginActionMap[w3t->GetTabDimensionType()].end();
    list_it++
    )
    {
    (*list_it)->setEnabled( true );
    }

  w3t->SetPluginActions( m_TabDimPluginActionMap[w3t->GetTabDimensionType()] );

  std::list< QDockWidget* > dock_list = w3t->DockWidget();

  for( std::list< QDockWidget* >::iterator
    dck_it = dock_list.begin();
    dck_it != dock_list.end();
    ++dck_it )
    {
    this->addDockWidget( Qt::LeftDockWidgetArea, (*dck_it) );
    (*dck_it)->show();
    }

  int idx = this->CentralTabWidget->addTab( w3t, iFile );
  this->menuView->setEnabled( true );
  this->menuFiltering->setEnabled( true );
  this->menuSegmentation->setEnabled( true );
  this->CentralTabWidget->setCurrentIndex( idx );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::CreateNewTabFor3DImage( vtkImageData* iInput, const QString& iFile )
{
  QGoTabImageView3D* w3 = new QGoTabImageView3D;
  w3->SetImage( iInput );
  w3->Update();

  for( std::list< QAction* >::iterator
    list_it = m_TabDimPluginActionMap[w3->GetTabDimensionType()].begin();
    list_it != m_TabDimPluginActionMap[w3->GetTabDimensionType()].end();
    list_it++
    )
    {
    (*list_it)->setEnabled( true );
    }

  w3->SetPluginActions( m_TabDimPluginActionMap[w3->GetTabDimensionType()] );

  std::list< QDockWidget* > dock_list = w3->DockWidget();

  for( std::list< QDockWidget* >::iterator
    dck_it = dock_list.begin();
    dck_it != dock_list.end();
    ++dck_it )
    {
    this->addDockWidget( Qt::LeftDockWidgetArea, (*dck_it) );
    (*dck_it)->show();
    }

  int idx = this->CentralTabWidget->addTab( w3, iFile );
  this->menuView->setEnabled( true );
  this->menuFiltering->setEnabled( true );
  this->menuSegmentation->setEnabled( true );
  this->CentralTabWidget->setCurrentIndex( idx );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::CreateNewTabFor2DImage( vtkImageData* iInput, const QString& iFile )
{
  QGoTabImageView2D* w2 = new QGoTabImageView2D;
  w2->SetImage( iInput );
  w2->setWindowTitle( iFile );
  w2->Update();

  for( std::list< QAction* >::iterator
    list_it = m_TabDimPluginActionMap[w2->GetTabDimensionType()].begin();
    list_it != m_TabDimPluginActionMap[w2->GetTabDimensionType()].end();
    list_it++
    )
    {
    (*list_it)->setEnabled( true );
    }

  w2->SetPluginActions( m_TabDimPluginActionMap[w2->GetTabDimensionType()] );

  std::list< QDockWidget* > dock_list = w2->DockWidget();

  for( std::list< QDockWidget* >::iterator
    dck_it = dock_list.begin();
    dck_it != dock_list.end();
    ++dck_it )
    {
    this->addDockWidget( Qt::LeftDockWidgetArea, (*dck_it) );
    (*dck_it)->show();
    }

  int idx = this->CentralTabWidget->addTab( w2, iFile );
  this->menuView->setEnabled( true );
  this->menuFiltering->setEnabled( true );
  this->menuSegmentation->setEnabled( true );
  this->CentralTabWidget->setCurrentIndex( idx );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::OpenImageWithITK( const QString& iFile )
{
  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  typedef ImageReaderType::Pointer          ImageReaderPointer;

  ImageReaderPointer reader = ImageReaderType::New();
  reader->SetFileName( iFile.toAscii( ).data( ) );

  //BUG 03/23: This next line is commented for the time being since
  // it makes gofigure crashing.
  // this->ShowProgressLoading( reader );
  reader->Update();

  ImageType::Pointer itkImage = reader->GetOutput();
  itkImage->DisconnectPipeline();

  VTKConvertImagePointer convert = VTKConvertImageType::New();
  convert->SetInput( itkImage );
//   this->ShowProgressLoading( convert );
  convert->Update();
//   this->HideProgressLoading();

  vtkImageData* VTKImage = convert->GetOutput();

  int dim[3];
  VTKImage->GetDimensions( dim );

  if( ( dim[0] == 1 ) || ( dim[1] == 1 ) || ( dim[2] == 1 ) )
    {
    QGoTabImageView2D* w2 = new QGoTabImageView2D;
    w2->SetImage( VTKImage );
    w2->setWindowTitle( iFile );
    w2->Update();

    for( std::list< QAction* >::iterator
            list_it = m_TabDimPluginActionMap[w2->GetTabDimensionType()].begin();
          list_it != m_TabDimPluginActionMap[w2->GetTabDimensionType()].end();
          list_it++
         )
      {
      (*list_it)->setEnabled( true );
      }

    w2->SetPluginActions( m_TabDimPluginActionMap[w2->GetTabDimensionType()] );

    int idx = this->CentralTabWidget->addTab( w2, iFile );
    this->menuView->setEnabled( true );
    this->menuFiltering->setEnabled( true );
    this->CentralTabWidget->setCurrentIndex( idx );
    }
  else
    {
    QGoTabImageView3D* w3 = new QGoTabImageView3D;
    w3->SetImage( VTKImage );
    w3->Update();

    for( std::list< QAction* >::iterator
            list_it = m_TabDimPluginActionMap[w3->GetTabDimensionType()].begin();
          list_it != m_TabDimPluginActionMap[w3->GetTabDimensionType()].end();
          list_it++
         )
        {
        (*list_it)->setEnabled( true );
        }

      w3->SetPluginActions( m_TabDimPluginActionMap[w3->GetTabDimensionType()] );

      int idx = this->CentralTabWidget->addTab( w3, iFile );
      this->menuView->setEnabled( true );
      this->menuFiltering->setEnabled( true );
      this->CentralTabWidget->setCurrentIndex( idx );
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionAbout_activated( )
{
  QString version( "v0.5" );

  QString about_gofigure( tr( "GoFigure V2 \n\n" ) );

  QString authors( tr( "Authors in alphabetical order:\n" ) );
  authors.append( " * 2008 ~ A. Gelas\n" );
  authors.append( " * 2007-2009 ~ A. Gouaillard\n" );
  authors.append( " * 2008 ~ K. Mosaliganti\n" );
  authors.append( " * 2008 ~ L. Souhait\n\n" );
  authors.append( " * 2009 ~ N. Rannou\n\n" );
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

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionGoFigure2_Website_activated( )
{
  QDesktopServices::openUrl( QUrl("https://sourceforge.net/projects/gofigure2/") );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionUser_mailing_list_activated( )
{
  QDesktopServices::openUrl( QUrl("mailto:users@gofigure2.com?subject=About GoFigure2") );
}
//--------------------------------------------------------------------------
void QGoMainWindow::on_actionDeveloper_mailing_list_activated( )
{
  QDesktopServices::openUrl( QUrl("mailto:developers@gofigure2.com?subject=About Gofigure2" ) );
}

//--------------------------------------------------------------------------
void QGoMainWindow::SetCurrentSingleFile( const QString &fileName )
{
  m_CurrentFile = fileName;
  this->setWindowModified( false );
  QString shownName = "Untitled";
  if( !m_CurrentFile.isEmpty() )
    {
    shownName = strippedName( m_CurrentFile );
    m_RecentSingleFiles.removeAll( m_CurrentFile );
    m_RecentSingleFiles.prepend( m_CurrentFile );
    UpdateRecentFileActions( m_RecentSingleFiles, menuSingle_Files,
      recentSingleFileActions );
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::LoadPlugins()
{
  foreach( QObject *plugin, QPluginLoader::staticInstances() )
    {
    this->PopulateMenus( plugin );
    }

  m_PluginsDir = FindPluginDirectory( "plugins" );

  foreach( QString fileName, m_PluginsDir.entryList( QDir::Files ) )
    {
    QPluginLoader loader( m_PluginsDir.absoluteFilePath( fileName ) );
    QObject* plugin = loader.instance();
    if( plugin )
      {
      this->PopulateMenus( plugin );
      m_PluginFileNames += fileName;
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::PopulateMenus( QObject *plugin )
{
  QGoImageFilterPluginBase* filter =
    qobject_cast< QGoImageFilterPluginBase* >( plugin );
  if( filter )
    {
    this->AddToMenu( plugin, QStringList( filter->Name() ),
      this->menuFiltering, SLOT( ApplyImageFilter() ), 0 );
    }
  else
    {
    std::cout <<"This is not QGoImageFilterPlugin" <<std::endl;
    }
 }
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::AddToMenu(
  QObject *plugin, const QStringList &texts,
  QMenu* menu, const char *member,
  QActionGroup *actionGroup )
{
  std::list< GoFigure::TabDimensionType > dim_list;

  QGoPlugin* temp = dynamic_cast< QGoPlugin* >( plugin );

  if( temp )
    {
    dim_list = temp->TabElementCompatibility();
    }

  foreach( QString text, texts )
    {
    QAction *taction = new QAction(text, plugin);
    taction->setDisabled( true );
    connect( action, SIGNAL(triggered()), this, member);
    menu->addAction(taction);

    for( std::list< GoFigure::TabDimensionType >::iterator it = dim_list.begin();
      it != dim_list.end();
      ++it )
      {
      m_TabDimPluginActionMap[ *it ].push_back( taction );
      }

    if (actionGroup)
      {
      taction->setCheckable(true);
      actionGroup->addAction(taction);
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::ApplyImageFilter()
{
  QAction *taction = qobject_cast< QAction* >( sender( ) );
  QGoImageFilterPluginBase* filter =
    qobject_cast< QGoImageFilterPluginBase* >( taction->parent() );
//   filter->SetInput( this->m_Image );
  filter->Update();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::SetCurrentMultiFile( const QString &fileName )
{
  m_CurrentFile = fileName;
  this->setWindowModified( false );
  QString shownName = "Untitled";
  if( !m_CurrentFile.isEmpty() )
    {
    shownName = strippedName( m_CurrentFile );
    m_RecentMultipleFiles.removeAll( m_CurrentFile );
    m_RecentMultipleFiles.prepend( m_CurrentFile );
    UpdateRecentFileActions( m_RecentMultipleFiles,
      menuMultiple_Files,
      recentMultipleFileActions );
    }
}

//--------------------------------------------------------------------------
QString QGoMainWindow::strippedName(const QString &fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::UpdateRecentFileActions( QStringList list,
  QMenu *menu,
  QAction *recentFileActions[MaxRecentFiles] )
{
  QMutableStringListIterator i(list);
  while (i.hasNext())
    {
    if (!QFile::exists(i.next()))
      {
      i.remove();
      }
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
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::openRecentSingleFile()
{
  QAction* taction = qobject_cast< QAction* >( sender() );
  if( action )
    {
    this->SetSingleFileName( taction->data().toString() );
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::openRecentMultipleFile()
{
  QAction* taction = qobject_cast< QAction* >( sender() );
  if( taction )
    {
//     this->SetMultiFileName( taction->data().toString() );
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::ReadSettings()
{
  QSettings settings;
  m_RecentSingleFiles =
    settings.value( "RecentSingleFiles" ).toStringList( );
  m_RecentMultipleFiles =
    settings.value( "RecentMultipleFiles" ).toStringList( );

  this->UpdateRecentFileActions( m_RecentSingleFiles,
    this->menuSingle_Files, this->recentSingleFileActions );
  this->UpdateRecentFileActions( m_RecentMultipleFiles,
    this->menuMultiple_Files, this->recentMultipleFileActions );

  settings.beginGroup( "MainWindow" );
  QSize tsize = settings.value( "size" ).toSize();

  if( tsize.isValid() )
    {
    this->resize( tsize );
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
void QGoMainWindow::WriteSettings()
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

