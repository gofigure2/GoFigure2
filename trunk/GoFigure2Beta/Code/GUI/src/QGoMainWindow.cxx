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
#include "QGoPrintDatabase.h"

// Plugin stuff
#include "QGoPluginHelper.h"
#include "QGoImageFilterPluginBase.h"

#include "itkQtAdaptor.h"
#include "itkQtProgressBar.h"

#include <iostream>
#include <set>

// Qt includes
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialog>
#include <QPluginLoader>
#include <QSettings>

// Qt Dialog Box
#include "QGoLsmToMegaExportDialog.h"

// itk includes
#include "itkImageFileReader.h"

#include "itkLsm3DSerieImport.h"
#include "itkMegaCaptureImport.h"

// vtk includes
// #include "vtkImageAppendComponents.h"
// #include "vtkMarchingCubes.h"
#include "vtkLSMReader.h"
#include "vtkPolyDataReader.h"
#include "vtkPLYReader.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"

#include "vtkImageReader2Factory.h"
#include "vtkImageReader2.h"

#include "QGoTabManager.h"

//--------------------------------------------------------------------------
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
  this->m_ViewToolBar->setObjectName( tr("View") );
  this->addToolBar( Qt::TopToolBarArea, this->m_ViewToolBar );

  m_LSMReader = vtkLSMReader::New();
  m_DBWizard  = new QGoWizardDB(this);
  m_DBWizard->hide();

  m_Bar.hide();
  QString temp;
  SetCurrentSingleFile( temp );
  SetCurrentMultiFile( temp );

  CreateSignalSlotsConnection();
  ReadSettings();
//   LoadPlugins();
}

//--------------------------------------------------------------------------
QGoMainWindow::~QGoMainWindow()
{
  m_LSMReader->Delete();
  this->WriteSettings();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::CreateSignalSlotsConnection()
{
  //QObject::connect( this->actionOpen, SIGNAL( triggered( ) ),
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

  QObject::connect( m_DBWizard, SIGNAL( accepted() ),
    this, SLOT( openFilesfromDB() ) );

}

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionOpen_Single_File_triggered( )
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
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
void QGoMainWindow::on_actionOpen_MegaCapture_Files_triggered()
{
  QString filename = QFileDialog::getOpenFileName(
    this,
    tr( "Select One Image from the Dataset" ), "",
    tr( "Images (*.png *.tif *.tiff *.jpg *.jpeg)")
    );

  if( !filename.isEmpty() )
    {
    if( QFile::exists( filename ) )
      {
      itk::MegaCaptureImport::Pointer importer = itk::MegaCaptureImport::New();
      importer->SetFileName( filename.toStdString() );
      importer->Update();

      GoFigure::FileType filetype;

      if( !ComputeFileType( filename, filetype ) )
        {
        return;
        }

      CreateNewTabFor3DwtImage( importer->GetOutput(), filetype,
        importer->GetHeaderFilename(), 0 );
      }
    }
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
void QGoMainWindow::on_actionUse_DataBase_triggered()
{
  m_DBWizard->restart();
  m_DBWizard->show();
}
//--------------------------------------------------------------------------

bool
QGoMainWindow::
ComputeFileType( const QString& iFileName, GoFigure::FileType& oFileType )
{
  QString extension = QFileInfo( iFileName ).suffix();

  if( extension.compare( "png", Qt::CaseInsensitive ) == 0 )
    {
    oFileType = GoFigure::PNG;
    return true;
    }
  else
    {
    if( ( extension.compare( "tif", Qt::CaseInsensitive ) == 0 ) ||
        ( extension.compare( "tiff", Qt::CaseInsensitive ) == 0 ) )
      {
      oFileType = GoFigure::TIFF;
      return true;
      }
    else
      {
      if( ( extension.compare( "jpg", Qt::CaseInsensitive ) == 0 ) ||
          ( extension.compare( "jpeg", Qt::CaseInsensitive ) == 0 ) )
        {
        oFileType = GoFigure::JPEG;
        return true;
        }
      else
        {
        std::cerr << "file not supported for megacapture!!!" <<std::endl;
        return false;
        }
      }
    }
}

//--------------------------------------------------------------------------
void QGoMainWindow::openFilesfromDB()
{ 
  GoFigureFileInfoHelperMultiIndexContainer file_container =
    m_DBWizard->GetMultiIndexFileContainer();

  std::string header_filename;

  unsigned int temp_size = file_container.size();

  if( temp_size == 0 )
    {
    /// \todo Right now, we first get the complete list of files from the wizard
    /// then use megacapture import, then visualize. This should be done in a
    /// single step.
    std::vector< std::vector< std::string > >
      listoffiles = m_DBWizard->GetFilenamesFromDB();

    itk::MegaCaptureImport::Pointer importer = itk::MegaCaptureImport::New();
    importer->SetFileName( listoffiles[0][0] );
    importer->Update();

    file_container = importer->GetOutput();

    header_filename = importer->GetHeaderFilename();
    }
  else
    {
    header_filename = m_DBWizard->GetMegaCaptureHeaderFilename();
    }

  GoFigureFileInfoHelperMultiIndexContainer::iterator
    temp_it = file_container.begin();

  QString temp_filename = QString::fromStdString( temp_it->m_Filename );

  GoFigure::FileType filetype;

  if( !ComputeFileType( temp_filename, filetype ) )
    {
    return;
    }

  // note: do not need to call w3t->Update(); since it is internally called
  // when using CreateNewTabFor3DwtImage
  QGoTabImageView3DwT* w3t = CreateNewTabFor3DwtImage( file_container,
    filetype, header_filename, 0 );

  // Load all contours from the first time point
  //std::vector< ContourMeshStructure >::iterator
  //contourmesh_list_it = w3t->m_DataBaseTables->m_ContoursInfo.begin();
  std::vector< ContourMeshStructure >* temp;
  std::vector<ContourMeshStructure>::iterator contourmesh_list_it;

  temp = w3t->m_DataBaseTables->GetTracesInfoListForVisu("contour");

  if( temp )
    {
    contourmesh_list_it = temp->begin();

    std::set< unsigned int > temp_time_set;

    // we don't need here to save this contour in the database,
    // since they have just been extracted from it!
    //while( contourmesh_list_it != w3t->m_DataBaseTables->m_ContoursInfo.end() )
    while( contourmesh_list_it != temp->end() )
      {
      w3t->AddContourFromNodes(
          contourmesh_list_it->TraceID,
          contourmesh_list_it->Nodes,
          contourmesh_list_it->rgba,
          contourmesh_list_it->Highlighted,
          contourmesh_list_it->TCoord, // timepoint
          false ); // not to be saved in the database

      if( contourmesh_list_it->TCoord != 0 )
        {
        temp_time_set.insert( contourmesh_list_it->TCoord );
        }

      ++contourmesh_list_it;
      }

    for( std::set< unsigned int >::iterator time_it = temp_time_set.begin();
      time_it != temp_time_set.end();
      ++time_it )
      {
      w3t->RemoveAllContoursForGivenTimePoint( *time_it );
      }
    }

  w3t->ReinitializeContour();

  temp = w3t->m_DataBaseTables->GetTracesInfoListForVisu("mesh");

  if( temp )
    {
    contourmesh_list_it = temp->begin();

    // Let's load all the mesh from the first time point
    //contourmesh_list_it = w3t->m_DataBaseTables->m_MeshesInfo.begin();
    contourmesh_list_it = temp->begin();

    while( contourmesh_list_it != temp->end() )
      {
      if( contourmesh_list_it->Nodes )
        {
        w3t->AddPolyData( contourmesh_list_it->Nodes );
        }
      ++contourmesh_list_it;      
      }
    }
  this->menuBookmarks->setEnabled(true);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/**
 * \brief Open dialog window to set the file output path and format
 */
void
QGoMainWindow::
on_actionExport_LSM_to_MegaFile_triggered( )
{
  //Open the dialog window
  QGoLsmToMegaExportDialog* dlg =
	    new QGoLsmToMegaExportDialog( this );
  dlg->show();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionClose_all_triggered()
{
  m_TabManager->CloseAllTabs();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionClose_triggered()
{
  int idx = this->CentralTabWidget->currentIndex();
  m_TabManager->CloseTab( idx );
}
//--------------------------------------------------------------------------
void QGoMainWindow::on_actionOpen_Mesh_triggered( )
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
    /// \todo this action must be enabled only if there are tabs
    QMessageBox::warning( this, tr( "Open Mesh / Contour Warning" ),
      tr( "One image needs to be opened first to be able to load contours or meshes" ) );
    }

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::
on_actionQuit_triggered( )
{
  this->close();
  this->WriteSettings();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::
SetSingleFileName( const QString& iFile )
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
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::
OpenLSMImage( const QString& iFile, const int& iTimePoint )
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
/**
 *
 * @param iFileList
 * @param iFileType
 * @param iHeader
 * @param iTimePoint
 * \todo Change the way to provide information about the database
 */
QGoTabImageView3DwT*
QGoMainWindow::
CreateNewTabFor3DwtImage(
  const GoFigureFileInfoHelperMultiIndexContainer& iFileList,
  const GoFigure::FileType& iFileType,
  const std::string& iHeader,
  const int& iTimePoint )
{
  // note: do not need to call w3t->Update() since it is internally called in
  // w3t->SetMegaCaptureFile
  QGoTabImageView3DwT* w3t = new QGoTabImageView3DwT;
  w3t->SetMegaCaptureFile( iFileList, iFileType, iHeader, iTimePoint );
  w3t->setWindowTitle( QString::fromStdString( iHeader ) );

  if( w3t->m_DataBaseTables->IsDatabaseUsed() )
    {
    // **********************
    // Database information
    //get the content of the tables fron the database to fill the table widget:
    w3t->m_DataBaseTables->SetDatabaseVariables( "gofiguredatabase",
      m_DBWizard->GetServer().toStdString(), m_DBWizard->GetLogin().toStdString(),
      m_DBWizard->GetPassword().toStdString(), m_DBWizard->GetImagingSessionID(),
      m_DBWizard->GetImagingSessionName().toStdString() );

    w3t->m_DataBaseTables->FillTableFromDatabase();

    w3t->setWindowTitle( m_DBWizard->GetImagingSessionName() );
    // **********************
    }
  else
    {
    w3t->m_DataBaseTables->hide();
    }

  SetupMenusFromTab( w3t );

  return w3t;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoMainWindow::
SetupMenusFromTab( QGoTabElementBase* iT )
{
  for( std::list< QAction* >::iterator
    list_it = m_TabDimPluginActionMap[iT->GetTabDimensionType()].begin();
    list_it != m_TabDimPluginActionMap[iT->GetTabDimensionType()].end();
    list_it++
    )
    {
    (*list_it)->setEnabled( true );
    }

  iT->SetPluginActions( m_TabDimPluginActionMap[iT->GetTabDimensionType()] );

  std::list< std::pair< Qt::DockWidgetArea, QDockWidget* > > dock_list = iT->DockWidget();

  for( std::list< std::pair< Qt::DockWidgetArea, QDockWidget* > >::iterator
    dck_it = dock_list.begin();
    dck_it != dock_list.end();
    ++dck_it )
    {
    this->addDockWidget( dck_it->first, dck_it->second );
    dck_it->second->hide();
    }

  int idx = this->CentralTabWidget->addTab( iT, iT->windowTitle() );
  this->menuView->setEnabled( true );
  this->menuFiltering->setEnabled( true );
  this->menuSegmentation->setEnabled( true );
  this->menuTools->setEnabled( true );

  this->CentralTabWidget->setCurrentIndex( idx );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView3DwT*
QGoMainWindow::
CreateNewTabFor3DwtImage( vtkLSMReader* iReader, const QString& iFile )
{
  QGoTabImageView3DwT* w3t = new QGoTabImageView3DwT;
  w3t->setWindowTitle( iFile );
  w3t->SetLSMReader( iReader, 0 );

  SetupMenusFromTab( w3t );

  w3t->m_DataBaseTables->hide();

  return w3t;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView3D*
QGoMainWindow::
CreateNewTabFor3DImage( vtkImageData* iInput, const QString& iFile )
{
  QGoTabImageView3D* w3 = new QGoTabImageView3D;
  w3->SetImage( iInput );
  w3->setWindowTitle( iFile );
  w3->Update();

  SetupMenusFromTab( w3 );

  return w3;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView2D*
QGoMainWindow::
CreateNewTabFor2DImage( vtkImageData* iInput, const QString& iFile )
{
  QGoTabImageView2D* w2 = new QGoTabImageView2D;
  w2->SetImage( iInput );
  w2->setWindowTitle( iFile );
  w2->Update();

  SetupMenusFromTab( w2 );

  return w2;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionAbout_triggered( )
{
  QString version( "0.4" );
  QString date( "Date: 02/17/2010\n\n" );

  QString about_gofigure(
    tr( "GoFigure2: Software for the visualization and the analysis of biological microscope images. \n\n" ) );

//   QString authors( tr( "Authors in alphabetical order:\n" ) );
//   authors.append( " * 2008 ~ A. Gelas\n" );
//   authors.append( " * 2007-2009 ~ A. Gouaillard\n" );
//   authors.append( " * 2008 ~ K. Mosaliganti\n" );
//   authors.append( " * 2008 ~ L. Souhait\n\n" );
//   authors.append( " * 2009 ~ N. Rannou\n\n" );
//   authors.append( " * Principal Investigator\n" );
//   authors.append( " * S. Megason\n" );

  QString message = QString( "GoFigure2-%1\n\n" ).arg( version );
  message.append( date );
  message.append( about_gofigure );
//   message.append( authors );

  QMessageBox::about( this, tr( "<*)0|00|0>< About GoFigure" ), message );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionAbout_Qt_triggered( )
{
  QMessageBox::aboutQt( this, tr( "About Qt" ) );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionGoFigure2_Website_triggered( )
{
  QDesktopServices::openUrl( QUrl("http://gofigure2.sourceforge.net") );
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionUser_mailing_list_triggered( )
{
//   QDesktopServices::openUrl( QUrl("mailto:users@gofigure2.com?subject=About GoFigure2") );
}
//--------------------------------------------------------------------------
void QGoMainWindow::on_actionDeveloper_mailing_list_triggered( )
{
//   QDesktopServices::openUrl( QUrl("mailto:developers@gofigure2.com?subject=About Gofigure2" ) );
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

  QByteArray state = settings.value("state", QByteArray()).toByteArray();
  this->restoreState(state);

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
  settings.setValue("state", saveState());
  settings.endGroup();
}
//--------------------------------------------------------------------------------

