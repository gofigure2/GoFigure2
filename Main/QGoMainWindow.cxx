/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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
#include "QGoNetworkUtilities.h"

#include "ContourMeshContainer.h"

// Plugin stuff
#include "QGoPluginHelper.h"
#include "QGoImageFilterPluginBase.h"

#include <iostream>
#include <set>

// Qt includes
#include <QDir>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialog>
#include <QPluginLoader>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QScrollArea>

// Qt Dialog Box
#include "QGoLsmToMegaExportDialog.h"

// itk includes
#include "itkImageFileReader.h"

#include "itkLsm3DSerieImport.h"
#include "itkMegaCaptureImport.h"

// vtk includes
#include "vtkLSMReader.h"
#include "vtkPLYReader.h"

#include "vtkImageData.h"
#include "vtkImageReader2Factory.h"
#include "vtkImageReader2.h"
#include "vtkSmartPointer.h"

#include "QGoTabManager.h"
#include "QGoWizardDB.h"

#include <list>

//--------------------------------------------------------------------------
QGoMainWindow::QGoMainWindow(QWidget *iParent, Qt::WindowFlags iFlags):
  QMainWindow(iParent, iFlags)
{
  QString title("<*)0|00|0>< ~~ <*)0|00|0><     GoFigure    ><0|00|0(*> ~~ ><0|00|0(*>");

  this->setupUi(this);
  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
  setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

  QRect screen = QApplication::desktop()->availableGeometry(this);
  QSize MaximumSize = screen.size();
  this->setMaximumSize(MaximumSize);
  // QSize IconSize = this->iconSize();
  QSize SizeIcon(22, 22);
  this->setIconSize(SizeIcon);
  QSize IconSize = this->iconSize();

  this->setCentralWidget(this->CentralTabWidget);

  this->setWindowTitle(title);
  this->statusbar->showMessage( tr("No data") );

  this->CentralTabWidget->clear();
  this->CentralTabWidget->setTabsClosable(true);
  this->CentralTabWidget->setMovable(true);

  this->statusbar->addPermanentWidget(&m_Bar);

  m_TabManager = new QGoTabManager(this, this->CentralTabWidget);
  this->m_ViewToolBar = new QToolBar(tr("View"), this);
  this->m_ViewToolBar->setObjectName( tr("View") );
  this->addToolBar(Qt::TopToolBarArea, this->m_ViewToolBar);

  this->m_ModeToolBar = new QToolBar(tr("Mode"), this);
  this->m_ModeToolBar->setObjectName( tr("Mode") );
  this->addToolBar(Qt::TopToolBarArea, this->m_ModeToolBar);

//   m_LSMReader = vtkLSMReader::New();
  m_DBWizard  = new QGoWizardDB(this);
  m_DBWizard->hide();

  m_AboutWidget = new QGoAboutWidget;
  this->m_AboutWidget->hide();

  m_Bar.hide();
  QString temp;
  SetCurrentSingleFile(temp);
  SetCurrentMultiFile(temp);
  SetCurrentDatabaseFile(temp);
  //temp:
  this->actionExportMesh->setVisible(false);
  this->actionExportLineage->setVisible(false);
  this->actionExportTrack->setVisible(false);
  this->actionImportMesh->setVisible(false);
  this->actionImportTrack->setVisible(false);
  this->actionImportLineage->setVisible(false);

  m_NetworkUtilities = new QGoNetworkUtilities(this);

  CreateSignalSlotsConnection();
  ReadSettings();

  // Updates -------------------------------
  QObject::connect( m_NetworkUtilities,
                    SIGNAL( CheckForUpdatesDone(QString, bool) ),
                    this,
                    SLOT( DisplayUpdateResults(QString, bool) ) );

  m_ManualUpdate = false;

  m_NetworkUtilities->CheckForUpdates();
  // ---------------------------------------
  if ( !this->m_DatabaseSetUp )
    {
    this->AddSetUpDatabaseMenu();
    }
  else
    {
    QObject::connect(this->m_DBWizard, SIGNAL ( NoGofigureDatabase () ),
                     this, SLOT(AddSetUpDatabaseMenu() ) );
    }
  // LoadPlugins();
}

//--------------------------------------------------------------------------
QGoMainWindow::~QGoMainWindow()
{
//   m_LSMReader->Delete();
  this->WriteSettings();
  delete m_TabManager;
  delete m_DBWizard;
  delete m_AboutWidget;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::CreateSignalSlotsConnection()
{
  //QObject::connect( this->actionOpen, SIGNAL( triggered( ) ),
  //this, SLOT( showprogressloading() ) );
  QObject::connect( this->CentralTabWidget,
                    SIGNAL( tabCloseRequested(int) ),
                    m_TabManager, SLOT( CloseTab(int) ) );

  QObject::connect( this->CentralTabWidget,
                    SIGNAL( currentChanged(int) ),
                    m_TabManager, SLOT( ChangeCurrentTab(int) ) );

  QObject::connect( &m_SignalAdaptor, SIGNAL( Signal() ),
                    &( this->m_Bar ), SLOT( hide() ) );

  for ( int i = 0; i < MaxRecentFiles; ++i )
    {
    recentSingleFileActions[i] = new QAction(this);
    recentSingleFileActions[i]->setVisible(false);
    QObject::connect( this->recentSingleFileActions[i], SIGNAL( triggered() ),
                      this, SLOT( openRecentSingleFile() ) );

    recentMultipleFileActions[i] = new QAction(this);
    recentMultipleFileActions[i]->setVisible(false);
    QObject::connect( this->recentMultipleFileActions[i], SIGNAL( triggered() ),
                      this, SLOT( openRecentMultipleFile() ) );

    recentDatabaseFileActions[i] = new QAction(this);
    recentDatabaseFileActions[i]->setVisible(false);
    QObject::connect( recentDatabaseFileActions[i], SIGNAL( triggered() ),
                      this, SLOT( openRecentDatabaseFile() ) );
    }

  QObject::connect( m_DBWizard, SIGNAL( accepted() ),
                    this, SLOT( openFilesfromDB() ) );
}

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionOpen_Single_File_triggered()
{
  QString filename = QFileDialog::getOpenFileName(
    this,
    tr("Select Image"), "",
    tr("Images (*.png *.bmp *.jpg *.jpeg *.tiff *.tif *.mha *.mhd *.img *.lsm)")
    );

  if ( !filename.isEmpty() )
    {
    this->SetSingleFileName(filename);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionOpen_MegaCapture_Files_triggered()
{
  QString filename = QFileDialog::getOpenFileName(
    this,
    tr("Select One Image from the Dataset"), "",
    tr("Images (*.png *.tif *.tiff *.jpg *.jpeg)")
    );

  if ( !filename.isEmpty() )
    {
    if ( QFile::exists(filename) )
      {
      itk::MegaCaptureImport::Pointer importer = itk::MegaCaptureImport::New();
      importer->SetFileName( filename.toStdString() );
      importer->Update();

      GoFigure::FileType filetype;

      if ( !ComputeFileType(filename, filetype) )
        {
        return;
        }
      int TimePoint = importer->GetOutput().get< m_TCoord >().begin()->m_TCoord;

      CreateNewTabFor3DwtImage(importer->GetOutput(), filetype,
                               importer->GetHeaderFilename(), TimePoint,
                               false); // do not use the database
      }
    }
}

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionUse_DataBase_triggered()
{
  if ( !m_DBWizard->isVisible() )
    {
    m_DBWizard->SetIsAnOpenRecentFile(false);
    m_DBWizard->show();
    m_DBWizard->restart();
    }
}

//--------------------------------------------------------------------------

bool
QGoMainWindow::ComputeFileType(const QString & iFileName, GoFigure::FileType & oFileType)
{
  QString extension = QFileInfo(iFileName).suffix();

  if ( extension.compare("png", Qt::CaseInsensitive) == 0 )
    {
    oFileType = GoFigure::PNG;
    return true;
    }
  else
    {
    if ( ( extension.compare("tif", Qt::CaseInsensitive) == 0 )
         || ( extension.compare("tiff", Qt::CaseInsensitive) == 0 ) )
      {
      oFileType = GoFigure::TIFF;
      return true;
      }
    else
      {
      if ( ( extension.compare("jpg", Qt::CaseInsensitive) == 0 )
           || ( extension.compare("jpeg", Qt::CaseInsensitive) == 0 ) )
        {
        oFileType = GoFigure::JPEG;
        return true;
        }
      else
        {
        std::cerr << "file not supported for megacapture!!!" << std::endl;
        return false;
        }
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::openFilesfromDB()
{
  if ( this->m_DBWizard->GetIsAnOpenRecentFile() )
    {
    this->openRecentFilesfromDB();
    }
  else
    {
    std::string temp = "";
    this->DisplayFilesfromDB(temp);
    this->SetCurrentDatabaseFile(
      this->m_DBWizard->GetImagingSessionName() );
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::DisplayFilesfromDB(std::string iFirst_Filename)
{
  std::string                               Header_FileName;
  GoFigureFileInfoHelperMultiIndexContainer file_container =
    this->GetFileContainerForMultiFiles(
      Header_FileName, iFirst_Filename);

  if ( file_container.size() == 0 )
    {
    std::cout << "GoFigureFileInfoHelperMultiIndexContainer empty ";
    std::cout << "Debug: In " << __FILE__ << ", line " << __LINE__;
    std::cout << std::endl;
    return;
    }
  GoFigureFileInfoHelperMultiIndexContainer::iterator
    temp_it = file_container.begin();

  QString temp_filename = QString::fromStdString(temp_it->m_Filename);

  GoFigure::FileType filetype;

  if ( !ComputeFileType(temp_filename, filetype) )
    {
    return;
    }
  // note: do not need to call w3t->Update(); since it is internally called
  // when using CreateNewTabFor3DwtImage
  int                  TimePoint = file_container.get< m_TCoord >().begin()->m_TCoord;
  QGoTabImageView3DwT *w3t = CreateNewTabFor3DwtImage(file_container,
                                                      filetype, Header_FileName, TimePoint,
                                                      true); // Use the database

  QObject::connect( w3t, SIGNAL( UpdateBookmarkOpenActions(std::vector< QAction * > ) ),
                    this->m_TabManager, SLOT( UpdateBookmarkMenu(std::vector< QAction * > ) ) );

  // Load all contours and only display the ones from the first time point
  LoadAllTracesFromDatabaseManager(TimePoint);

  this->menuBookmarks->setEnabled(true);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoMainWindow::LoadAllTracesFromDatabaseManager(const int & iT)
{
  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );

  // Loads contours
  LoadContoursFromDatabase( iT );
  // Loads meshes
  LoadMeshesFromDatabase( iT );
  // Loads tracks
  LoadTracksFromDatabase( iT );

  QApplication::restoreOverrideCursor();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoMainWindow::LoadContoursFromDatabase( const int & iT )
{
  /// \note let's keep for the time being iT parameter in the case where
  /// we would only load traces for a given time point (that could be usefule
  /// somehow).
  (void)iT;

  QGoTabImageView3DwT *w3t =
    dynamic_cast< QGoTabImageView3DwT * >( this->CentralTabWidget->currentWidget() );

  if ( w3t )
    {
    ContourMeshContainer *temp = w3t->GetContourContainer();

    if ( temp )
      {
      // let's iterate on the container with increasing TraceID
      ContourMeshContainer::MultiIndexContainerType::index< TraceID >::type::iterator
        contourmesh_list_it = temp->m_Container.get< TraceID >().begin();

      // we don't need here to save this contour in the database,
      // since they have just been extracted from it!
      while ( contourmesh_list_it != temp->m_Container.get< TraceID >().end() )
        {
        w3t->AddContourFromNodes< TraceID >(
          contourmesh_list_it );

        ++contourmesh_list_it;
        }
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoMainWindow::LoadMeshesFromDatabase( const int & iT )
{
  /// \note let's keep for the time being iT parameter in the case where
  /// we would only load traces for a given time point (that could be usefule
  /// somehow).
  (void)iT;

  QGoTabImageView3DwT *w3t =
    dynamic_cast< QGoTabImageView3DwT * >( this->CentralTabWidget->currentWidget() );

  if ( w3t )
    {
 // std::cout << "IN W3T" << std::endl;
    ContourMeshContainer *temp =  w3t->GetMeshContainer();
    if ( temp )
      {
    //std::cout << "IN TEMP" << std::endl;
      // let's iterate on the container with increasing TraceID
      ContourMeshContainer::MultiIndexContainerType::index< TraceID >::type::iterator
        contourmesh_list_it = temp->m_Container.get< TraceID >().begin();
      // we don't need here to save this contour in the database,
      // since they have just been extracted from it!
      while ( contourmesh_list_it != temp->m_Container.get< TraceID >().end() )
        {
        // note here it only makes sense when the trace is a mesh (for now)
      //std::cout << "IN WHILE" << std::endl;

        if ( contourmesh_list_it->Nodes )
          {
          GoFigureMeshAttributes attributes =
            w3t->ComputeMeshAttributes(
                contourmesh_list_it->Nodes, // mesh
                false ); // do not need to compute intensity based measure
          w3t->m_DataBaseTables->PrintVolumeAreaForMesh(
            &attributes, contourmesh_list_it->TraceID);
          }

        w3t->AddMeshFromNodes< TraceID >(
          contourmesh_list_it );

        ++contourmesh_list_it;
        }
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoMainWindow::LoadTracksFromDatabase( const int & iT )
{
  /// \note let's keep for the time being iT parameter in the case where
  /// we would only load traces for a given time point (that could be usefule
  /// somehow).
  (void)iT;

  QGoTabImageView3DwT *w3t =
    dynamic_cast< QGoTabImageView3DwT * >( this->CentralTabWidget->currentWidget() );

  if ( w3t )
    {
    TrackContainer *temp = w3t->GetTrackContainer();
    //temp->Print();
    if ( temp )
      {
      // let's iterate on the container with increasing TraceID
      TrackContainer::MultiIndexContainerType::index< TraceID >::type::iterator
        track_list_it = temp->m_Container.get< TraceID >().begin();

      // we don't need here to save this contour in the database,
      // since they have just been extracted from it!
      while ( track_list_it != temp->m_Container.get< TraceID >().end() )
        {
        w3t->AddTrackFromNodes< TraceID >(track_list_it);// Name of the trace to add
        ++track_list_it;
        }
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
GoFigureFileInfoHelperMultiIndexContainer
QGoMainWindow::GetFileContainerForMultiFiles(std::string & ioHeader_Filename,
                                             std::string iFirst_FileName)
{
  GoFigureFileInfoHelperMultiIndexContainer ofile_container;

  if ( iFirst_FileName.empty() )
    {
    std::string ImgSessionName =
      this->m_DBWizard->GetImagingSessionName().toStdString();
    //this->m_DBWizard->setImgSessionName(ImgSessionName);
    ofile_container = this->m_DBWizard->GetMultiIndexFileContainer();
    }
  iFirst_FileName = this->m_DBWizard->GetFirstFileName();

  // if the size is diiferent than 0, the imagingsession has
  // just been created and the wizard has already filled the
  // GoFigureFileInfoHelperMultiIndexContainer using MegcaptureImport
  if ( ofile_container.size() == 0 || !iFirst_FileName.empty() )
    {
    itk::MegaCaptureImport::Pointer importer = itk::MegaCaptureImport::New();
    importer->SetFileName(iFirst_FileName);
    importer->Update();
    ofile_container = importer->GetOutput();
    ioHeader_Filename = importer->GetHeaderFilename();
    }
  else
    {
    ioHeader_Filename = m_DBWizard->GetMegaCaptureHeaderFilename();
    }
  return ofile_container;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoMainWindow::on_actionExport_LSM_to_MegaFile_triggered()
{
  //Open the dialog window
  QGoLsmToMegaExportDialog *dlg =
    new QGoLsmToMegaExportDialog(this);

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

  m_TabManager->CloseTab(idx);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionQuit_triggered()
{
  this->close();
  this->WriteSettings();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::SetSingleFileName(const QString & iFile)
{
  if ( QFile::exists(iFile) )
    {
    this->SetCurrentSingleFile(iFile);

    // parse extension
    QString ext = QFileInfo(iFile).suffix();
    if ( ext.compare("lsm", Qt::CaseInsensitive) == 0 )
      {
      this->OpenLSMImage(m_CurrentFile, 0);
      }
    else
      {
      vtkImageReader2Factory *r_factory = vtkImageReader2Factory::New();
      vtkImageReader2 *       reader = r_factory->CreateImageReader2( iFile.toAscii().data() );

      reader->SetFileName( iFile.toAscii().data() );
      reader->Update();

      vtkImageData *image = reader->GetOutput();

      int dim[3];
      image->GetDimensions(dim);

      if ( ( dim[0] != 1 ) && ( dim[1] != 1 ) && ( dim[2] != 1 ) )
        {
        CreateNewTabFor3DImage(image, iFile);
        }
      else
        {
        CreateNewTabFor2DImage(image, iFile);
        }
      reader->Delete();
      r_factory->Delete();
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::OpenLSMImage(const QString & iFile, const int & iTimePoint)
{
  m_LSMReader.push_back( vtkLSMReader::New() );
  m_LSMReader.back()->SetFileName( iFile.toAscii().data() );
  m_LSMReader.back()->SetUpdateTimePoint(iTimePoint);
  m_LSMReader.back()->Update();

  int dim[5];
  m_LSMReader.back()->GetDimensions(dim);

  int ImageDimensionality = 4;
//   bool Color = ( dim[4] > 1 );

  if ( dim[3] == 1 )  // only one time point
    {
    if ( dim[2] == 1 )  // only one z slice
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

  switch ( ImageDimensionality )
    {
    case 2:
      {
      CreateNewTabFor2DImage(m_LSMReader.back()->GetOutput(), iFile);
      break;
      }
    case 3:
      {
      CreateNewTabFor3DImage(m_LSMReader.back()->GetOutput(), iFile);
      break;
      }
    case 4:
      {
      CreateNewTabFor3DwtImage(m_LSMReader.back(), iFile);
      break;
      }
    }
}

//--------------------------------------------------------------------------
QGoTabImageView3DwT *
QGoMainWindow::CreateNewTabFor3DwtImage(
  const GoFigureFileInfoHelperMultiIndexContainer & iFileList,
  const GoFigure::FileType & iFileType,
  const std::string & iHeader,
  const int & iTimePoint,
  const bool & iUseDatabase)
{
  // note: do not need to call w3t->Update() since it is internally called in
  // w3t->SetMegaCaptureFile
  QGoTabImageView3DwT *w3t = new QGoTabImageView3DwT;
  w3t->SetStatusBarPointer(this->statusBar());
  w3t->SetMegaCaptureFile(iFileList, iFileType, iHeader, iTimePoint);

  if ( iUseDatabase )
    {
    // **********************
    // Database information
    //get the content of the tables fron the database to fill the table widget:
    std::string ImgSessionName = m_DBWizard->GetImagingSessionName().toStdString();
    //in case the files are opened from the open recent files, the
    // imgsessionname
    //can no more be gotten from the wizard:
    if ( this->m_DBWizard->GetIsAnOpenRecentFile() )
      {
      ImgSessionName = this->m_CurrentFile.toStdString();
      this->m_DBWizard->SetIsAnOpenRecentFile(false);
      }
    w3t->m_DataBaseTables->SetDatabaseVariables("gofiguredatabase",
                                                m_DBWizard->GetServer().toStdString(),
                                                m_DBWizard->GetLogin().toStdString(),
                                                m_DBWizard->GetPassword().toStdString(),
                                                m_DBWizard->GetImagingSessionID(),
                                                ImgSessionName);

    w3t->m_DataBaseTables->FillTableFromDatabase();
    w3t->setWindowTitle( QString::fromStdString(ImgSessionName) );
    // **********************
    }
  else
    {
    w3t->setWindowTitle( QFileInfo( QString::fromStdString(iHeader) ).fileName() );
    }

  SetupMenusFromTab(w3t);

  return w3t;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoMainWindow::SetupMenusFromTab(QGoTabElementBase *iT)
{
  for ( std::list< QAction * >::iterator
        list_it = m_TabDimPluginActionMap[iT->GetTabDimensionType()].begin();
        list_it != m_TabDimPluginActionMap[iT->GetTabDimensionType()].end();
        list_it++
        )
    {
    ( *list_it )->setEnabled(true);
    }

  iT->SetPluginActions(m_TabDimPluginActionMap[iT->GetTabDimensionType()]);

  std::list< QGoTabElementBase::QGoDockWidgetStatusPair > dock_list = iT->DockWidget();

  for ( std::list< QGoTabElementBase::QGoDockWidgetStatusPair >::iterator
        dck_it = dock_list.begin();
        dck_it != dock_list.end();
        ++dck_it )
    {
    if ( dck_it->first->m_Attached )
      {
      this->addDockWidget(dck_it->first->m_Area, dck_it->second);
      }
    dck_it->second->setVisible(dck_it->first->m_Visibility);
    }

  int idx = this->CentralTabWidget->addTab( iT, iT->windowTitle() );
  this->menuView->setEnabled(true);
  this->menuFiltering->setEnabled(true);
  this->menuSegmentation->setEnabled(true);
  this->menuTools->setEnabled(true);
  this->menuMode->setEnabled(true);

  this->CentralTabWidget->setCurrentIndex(idx);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView3DwT *
QGoMainWindow::CreateNewTabFor3DwtImage(vtkLSMReader *iReader, const QString & iFile)
{
  QGoTabImageView3DwT *w3t = new QGoTabImageView3DwT;

  w3t->setWindowTitle( QFileInfo(iFile).fileName() );
  w3t->SetLSMReader(iReader, 0);

  SetupMenusFromTab(w3t);

  // w3t->m_DataBaseTables->hide();
  w3t->SetStatusBarPointer(this->statusBar());

  return w3t;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView3D *
QGoMainWindow::CreateNewTabFor3DImage(vtkImageData *iInput, const QString & iFile)
{
  QGoTabImageView3D *w3 = new QGoTabImageView3D;

  w3->SetImage(iInput);
  w3->setWindowTitle( QFileInfo(iFile).fileName() );
  w3->Update();

  SetupMenusFromTab(w3);

  return w3;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoTabImageView2D *
QGoMainWindow::CreateNewTabFor2DImage(vtkImageData *iInput, const QString & iFile)
{
  QGoTabImageView2D *w2 = new QGoTabImageView2D;

  w2->SetImage(iInput);
  w2->setWindowTitle( QFileInfo(iFile).fileName() );
  w2->Update();

  SetupMenusFromTab(w2);

  return w2;
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::on_actionAbout_triggered()
{
  this->m_AboutWidget->show();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionAbout_Qt_triggered()
{
  QMessageBox::aboutQt( this, tr("About Qt") );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionGoFigure2_Website_triggered()
{
  QDesktopServices::openUrl( QUrl("http://gofigure2.sourceforge.net") );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionUser_mailing_list_triggered()
{
  QDesktopServices::openUrl( QUrl("mailto:gofigure2-users@lists.sourceforge.net?subject=About GoFigure2") );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionDeveloper_mailing_list_triggered()
{
  QDesktopServices::openUrl( QUrl("mailto:gofigure2-developers@lists.sourceforge.net?subject=About GoFigure2") );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::SetUpDatabase()
{
  this->m_DBInitializationWizard->show();
  this->m_DBInitializationWizard->exec();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::SetCurrentSingleFile(const QString & fileName)
{
  m_CurrentFile = fileName;
  this->setWindowModified(false);
  QString shownName = "Untitled";
  if ( !m_CurrentFile.isEmpty() )
    {
    shownName = strippedName(m_CurrentFile);
    m_RecentSingleFiles.removeAll(m_CurrentFile);
    m_RecentSingleFiles.prepend(m_CurrentFile);
    UpdateRecentFileActions(m_RecentSingleFiles, menuSingle_Files,
                            recentSingleFileActions);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::LoadPlugins()
{
  foreach( QObject * plugin, QPluginLoader::staticInstances() )
    {
    this->PopulateMenus(plugin);
    }

  m_PluginsDir = FindPluginDirectory("plugins");

  foreach( QString fileName, m_PluginsDir.entryList(QDir::Files) )
    {
    QPluginLoader loader( m_PluginsDir.absoluteFilePath(fileName) );
    QObject *     plugin = loader.instance();

    if ( plugin )
      {
      this->PopulateMenus(plugin);
      m_PluginFileNames += fileName;
      }
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::PopulateMenus(QObject *plugin)
{
  QGoImageFilterPluginBase *filter =
    qobject_cast< QGoImageFilterPluginBase * >(plugin);

  if ( filter )
    {
    this->AddToMenu(plugin, QStringList( filter->Name() ),
                    this->menuFiltering, SLOT( ApplyImageFilter() ), 0);
    }
  else
    {
    std::cout << "This is not QGoImageFilterPlugin" << std::endl;
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::AddToMenu(
  QObject *plugin, const QStringList & texts,
  QMenu *menu, const char *member,
  QActionGroup *actionGroup)
{
  std::list< GoFigure::TabDimensionType > dim_list;

  QGoPlugin *temp = dynamic_cast< QGoPlugin * >( plugin );

  if ( temp )
    {
    dim_list = temp->TabElementCompatibility();
    }

  foreach(QString text, texts)
    {
    QAction *taction = new QAction(text, plugin);

    taction->setDisabled(true);
    QObject::connect(taction, SIGNAL( triggered() ), this, member);
//    QObject::connect( plugin, SIGNAL( Done( std::vector< vtkImageData* > ) ),
//      this, SLOT( tobedone( std::vector< vtkImageData* > ) ) );

    menu->addAction(taction);

    for ( std::list< GoFigure::TabDimensionType >::iterator it = dim_list.begin();
          it != dim_list.end();
          ++it )
      {
      m_TabDimPluginActionMap[*it].push_back(taction);
      }

    if ( actionGroup )
      {
      taction->setCheckable(true);
      actionGroup->addAction(taction);
      }
    }
}

//--------------------------------------------------------------------------

void QGoMainWindow::tobedone(std::vector< vtkImageData * > iImages)
{
  QWidget *w = this->CentralTabWidget->currentWidget();

  QGoTabImageViewNDBase *WnD = dynamic_cast< QGoTabImageViewNDBase * >( w );

  if ( WnD )
    {
    WnD->SetImage(iImages[0]);
    }
}

//--------------------------------------------------------------------------
void QGoMainWindow::ApplyImageFilter()
{
  QAction *                 taction = qobject_cast< QAction * >( sender() );
  QGoImageFilterPluginBase *filter =
    qobject_cast< QGoImageFilterPluginBase * >( taction->parent() );

  QWidget *w = this->CentralTabWidget->currentWidget();

  QGoTabImageViewNDBase *WnD = dynamic_cast< QGoTabImageViewNDBase * >( w );

  if ( WnD )
    {
    filter->SetInput( WnD->GetImage() );
    filter->Update();
//     WnD->SetImage( filter->GetOutput()[0] );
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::SetCurrentMultiFile(const QString & fileName)
{
  m_CurrentFile = fileName;
  this->setWindowModified(false);
  QString shownName = "Untitled";
  if ( !m_CurrentFile.isEmpty() )
    {
    shownName = strippedName(m_CurrentFile);
    m_RecentMultipleFiles.removeAll(m_CurrentFile);
    m_RecentMultipleFiles.prepend(m_CurrentFile);
    UpdateRecentFileActions(m_RecentMultipleFiles,
                            menuMultiple_Files,
                            recentMultipleFileActions);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::SetCurrentDatabaseFile(const QString & fileName)
{
  m_CurrentFile = fileName;
  this->setWindowModified(false);
  if ( !m_CurrentFile.isEmpty() )
    {
    this->m_RecentDatabaseFiles.removeAll(m_CurrentFile);
    this->m_RecentDatabaseFiles.prepend(m_CurrentFile);
    UpdateRecentFileActions(this->m_RecentDatabaseFiles,
                            menuDatabase_Files,
                            recentDatabaseFileActions);
    }
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QString QGoMainWindow::strippedName(const QString & fullFileName)
{
  return QFileInfo(fullFileName).fileName();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::UpdateRecentFileActions(QStringList list,
                                            QMenu *menu,
                                            QAction *recentFileActions[MaxRecentFiles])
{
  //if the items in the list are imagingsession names,from
  // the menuDatabase_Files, they don't corresponds to real
  //files, the test bellow will then remove
  //them from the list
  if ( menu != this->menuDatabase_Files )
    {
    QMutableStringListIterator i(list);
    while ( i.hasNext() )
      {
      if ( !QFile::exists( i.next() ) )
        {
        i.remove();
        }
      }
    }

  if ( !list.isEmpty() )
    {
    menu->setEnabled(true);
    }

  for ( int j = 0; j < MaxRecentFiles; ++j )
    {
    if ( j < list.count() )
      {
      QString text = tr("&%1 %2 ")
                     .arg(j + 1)
                     .arg( strippedName(list[j]) );

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
  QAction *taction = qobject_cast< QAction * >( sender() );

  if ( action )
    {
    this->SetSingleFileName( taction->data().toString() );
    }
}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::openRecentMultipleFile()
{
  QAction *taction = qobject_cast< QAction * >( sender() );

  if ( taction )
    {
//     this->SetMultiFileName( taction->data().toString() );
    }
}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::openRecentDatabaseFile()
{
  QAction *taction = qobject_cast< QAction * >( sender() );

  if ( taction )
    {
    std::string ImgSessionName = taction->data().toString().toStdString();
    this->SetCurrentDatabaseFile( ImgSessionName.c_str() );
    this->m_DBWizard->setImgSessionName(ImgSessionName);
    this->m_DBWizard->restart();
    this->m_DBWizard->show();
    }
}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::openRecentFilesfromDB()
{
  this->DisplayFilesfromDB( this->m_CurrentFile.toStdString() );
}

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::RemoveSetUpDatabaseMenu()
{
  this->menuDatabase->removeAction(this->actionSet_Up_Database);
  this->m_DatabaseSetUp = true;
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::AddSetUpDatabaseMenu()
{
  //enter number of actions in the Database menu here:
  unsigned int NumberOfActionsIfSetUpDB = 1;

  unsigned int NumberOfCurrentActions = this->menuDatabase->actions().size();
  if (NumberOfCurrentActions != NumberOfActionsIfSetUpDB)
    {
    actionSet_Up_Database = new QAction(
        tr("Set Up Database"), this->menuDatabase);
  this->m_DatabaseSetUp = false;
    this->menuDatabase->addAction(actionSet_Up_Database);
    this->actionSet_Up_Database->setEnabled(true);
    m_DBInitializationWizard = new QGoDBInitializationWizard(this);
    this->m_DBInitializationWizard->hide();
    QObject::connect( this->actionSet_Up_Database, SIGNAL( triggered() ),
                      SLOT( SetUpDatabase() ) );
    QObject::connect( this->m_DBInitializationWizard, SIGNAL( DatabaseAndUserCreated() ),
                      this, SLOT( RemoveSetUpDatabaseMenu() ) );
    QObject::connect( this->m_DBWizard, SIGNAL( GofigureDatabaseExists() ),
                      this, SLOT(RemoveSetUpDatabaseMenu() ) );
    }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void QGoMainWindow::ReadSettings()
{
  QSettings settings;

  m_RecentSingleFiles =
    settings.value("RecentSingleFiles").toStringList();
  m_RecentMultipleFiles =
    settings.value("RecentMultipleFiles").toStringList();
  m_RecentDatabaseFiles =
    settings.value("RecentDatabaseFiles").toStringList();
  m_DatabaseSetUp =
    settings.value("DatabaseSetUp").toBool();

  this->UpdateRecentFileActions(m_RecentSingleFiles,
                                this->menuSingle_Files, this->recentSingleFileActions);
  this->UpdateRecentFileActions(m_RecentMultipleFiles,
                                this->menuMultiple_Files, this->recentMultipleFileActions);
  this->UpdateRecentFileActions(this->m_RecentDatabaseFiles,
                                this->menuDatabase_Files, this->recentDatabaseFileActions);

  settings.beginGroup("MainWindow");
  QSize tsize = settings.value("size").toSize();

  if ( tsize.isValid() )
    {
    this->resize(tsize);
    this->move( settings.value("pos").toPoint() );
    }
  else
    {
    this->resize(1450, 750);
    }

  //QByteArray state = settings.value("state", QByteArray()).toByteArray();
  //this->restoreState(state);

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
  settings.setValue("RecentDatabaseFiles", m_RecentDatabaseFiles);
  settings.beginGroup("MainWindow");
  settings.setValue( "size", size() );
  settings.setValue( "pos", pos() );
  settings.setValue( "state", saveState() );
  settings.endGroup();
  settings.setValue("DatabaseSetUp", this->m_DatabaseSetUp);
}

//-------------------------------------------------------------------------

void
QGoMainWindow::on_actionCheck_For_Updates_triggered()
{
  m_ManualUpdate = true;
  m_NetworkUtilities->CheckForUpdates();
}

//-------------------------------------------------------------------------
void
QGoMainWindow::DisplayUpdateResults(QString result, bool noerror)
{
  if ( !noerror )  // error during the connection to the server
    {
    if ( m_ManualUpdate )
      {
      QMessageBox::warning( this, tr("GoFigure2 Updates"),
                            tr("There was an error while trying to GoFigure2's update website!\n " \
                               "Check your internet connection and try again later!") );
      }
    }
  else
    {
    // no new version!
    if ( result.compare(tr("no-update\n"), Qt::CaseInsensitive) == 0 )
      {
      if ( m_ManualUpdate )
        {
        QMessageBox::information( this, tr("GoFigure2 Updates"),
                                  tr("You have the lastest version!") );
        }
      }
    else
      {
      // there is one new version!
      if ( result.compare(tr("update\n"), Qt::CaseInsensitive) == 0 )
        {
        QMessageBox msgBox( QMessageBox::Information, tr("GoFigure2 Updates"),
                            tr("There is a new version of GoFigure2 available for download!") );

        QPushButton *goButton =
          msgBox.addButton(tr("Go to GoFigure2's website!"),
                           QMessageBox::ActionRole);

        QPushButton *notnowButton =
          msgBox.addButton(tr("Not Now!"),
                           QMessageBox::ActionRole);

        msgBox.exec();

        if ( msgBox.clickedButton() == goButton )
          {
          QUrl address("http://sourceforge.net/projects/gofigure2/files/");
          QDesktopServices::openUrl(address);
          }
        else
          {
          if ( msgBox.clickedButton() == notnowButton )
            {
            msgBox.close();
            }
          }
        }
      }
    }
}
