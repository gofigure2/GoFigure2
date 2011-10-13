/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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

#ifndef __QGoMainWindow_h
#define __QGoMainWindow_h

#include <QMainWindow>
#include <QDir>

// #include "itkImage.h"
// #include "itkImageToVTKImageFilter.h"

#include "itkQtProgressBar.h"
#include "itkQtAdaptor.h"

#include "GoFigureFileInfoMultiIndexContainerHelper.h"
#include "GoFigureGlobalDefinition.h"
#include "QGoAboutWidget.h"
#include "QGoDBInitializationWizard.h"
#include "QGoTabImageView3DwT.h"

class vtkImageData;

class vtkLSMReader;

class QGoWizardDB;
class QGoTabManager;
class QGoTabImageView3D;
class QGoTabImageView2D;
class QGoTabElementBase;
class QGoNetworkUtilities;

#include "ui_go.h"

/**
 * \class QGoMainWindow
 * \brief GoFigure2's main window
 * \image html QGoMainWindow.png
 * */
class QGoMainWindow:public QMainWindow,
  private Ui::go_mainwindow
{
  Q_OBJECT
public:
  friend class QGoTabManager;

//   typedef itk::Image< unsigned char, 3 > ImageType;
//   typedef itk::ImageToVTKImageFilter< ImageType > VTKConvertImageType;
//   typedef VTKConvertImageType::Pointer            VTKConvertImagePointer;

//   typedef itk::MultiFileReader::FILETYPE FILETYPE;

  explicit QGoMainWindow(QWidget *iParent = 0, Qt::WindowFlags iFlags = 0);
  ~QGoMainWindow();

  void SetSingleFileName(const QString & iFileName);

  void SetMaxNumberOfTraces( unsigned int iN );

private slots:
  void on_actionOpen_Single_File_triggered();

  void openRecentSingleFile();

//   void on_actionOpen_Multiple_Files_triggered( );
  void on_actionOpen_MegaCapture_Files_triggered();

  void openRecentMultipleFile();

  void openRecentDatabaseFile();

  void openRecentFilesfromDB();

  void on_actionGoFigure2_Website_triggered();

  void on_actionReport_a_bug_triggered();
  
  void on_actionUser_mailing_list_triggered();

  void on_actionDeveloper_mailing_list_triggered();

  void on_actionUse_DataBase_triggered();

  void openFilesfromDB();

  /**
  * \brief Open dialog window to set the file output path and format
  */
  void on_actionExport_LSM_to_MegaFile_triggered();

  void on_actionClose_triggered();

  void on_actionClose_all_triggered();

  void on_actionQuit_triggered();

  void on_actionAbout_triggered();

  void on_actionAbout_Qt_triggered();

  void SetUpDatabase();

  void on_actionCheck_For_Updates_triggered();

  void DisplayUpdateResults(QString result, bool noerror);

  void ApplyImageFilter();

  void tobedone(std::vector< vtkImageData * > );

  /**
  \brief remove the action 'Set up Database' from the Database menu.
  */
  void RemoveSetUpDatabaseMenu();

  /**
  \brief add the action 'Set up Database' to the Database menu if it doesn't
  have been already added.
  */
  void AddSetUpDatabaseMenu();

private:

  std::map< GoFigure::TabDimensionType, std::list< QAction * > >
  m_TabDimPluginActionMap;

  /** \brief */
  void openRecentFile(const bool & IsSerie);

  void DisplayFilesfromDB(std::string iFirst_Filename);

  /** \brief */
  void SetCurrentSingleFile(const QString & fileName);

  void SetCurrentMultiFile(const QString & fileName);

  void SetCurrentDatabaseFile(const QString & fileName);

  /** \brief */
  enum { MaxRecentFiles = 5 };
  QAction *recentSingleFileActions[MaxRecentFiles];
  QAction *recentMultipleFileActions[MaxRecentFiles];
  QAction *recentDatabaseFileActions[MaxRecentFiles];

  void UpdateRecentFileActions(QStringList list, QMenu * menu,
                               QAction * recentFileActions[MaxRecentFiles]);

   /**
    * \brief Create a new tab in the TabWidget for a 3DwT image from one megacapture
    * (from the database, or from the filesystem directly).
    * \param[in] iFileList    Megacapture files Container
    * \param[in] iFileType    Type of images
    * \param[in] iHeader      Megacapture header (*.meg)
    * \param[in] iTimePoint   Time point to show
    * \param[in] iUseDatabase Use the database
  */
  QGoTabImageView3DwT * CreateNewTabFor3DwtImage(
    const GoFigureFileInfoHelperMultiIndexContainer & iFileList,
    const GoFigure::FileType & iFileType,
    const std::string & iHeader,
    const int & iTimePoint,
    const bool & iUseDatabase);

  /**
   * \brief Create a new tab in the TabWidget for a 3DwT LSM file.
   * \param[in] iReader the vtkLSMReader to be duplicated
   * \param[in] iFile filename
  */
  QGoTabImageView3DwT * CreateNewTabFor3DwtImage(
    vtkLSMReader *iReader, const QString & iFile);

  QGoTabImageView3D * CreateNewTabFor3DImage(vtkImageData *, const QString &);

  QGoTabImageView2D * CreateNewTabFor2DImage(vtkImageData *, const QString &);

  /* \brief Open Image with given iFileName
  \param[in] iFileName  */
//   void OpenImageWithITK( const QString& iFileName );

  /** \brief Open LSM image
 * \param[in] iFile filename
 * \param[in] iTimePoint time point
 * */
  void OpenLSMImage(const QString & iFile, const int & iTimePoint);

  void SetupPluginsAndDockWidgetFromTab(QGoTabElementBase *iT);

  void SetUpGeneralMenusToolBars(QGoTabElementBase *iT);

  void SetUpMenusToolBarsFor3dwtImage(QGoTabImageView3DwT* iT);

  /** \brief get the file container and the header filename for one file
 * part of a megacapture imaging session
 * \param[in,out] ioHeader_Filename detected *.meg file
 * \param[in] iFirstFileName one file part of a megacapture imaging session
 * \return multi index container with all file names
 * */
  GoFigureFileInfoHelperMultiIndexContainer
  GetFileContainerForMultiFiles(
    std::string & ioHeader_Filename,
    std::string iFirstFileName);

  /**
   *
   */
  void LoadAllTracesFromDatabaseManager(const int & iT);

  /**
   *
   */
  //void LoadAllTracesFromDatabase(const int & iT, const std::string & iTrace);

  void LoadContoursFromDatabase(const int & iT );
  void LoadMeshesFromDatabase(const int & iT);
  void LoadTracksFromDatabase(const int & iT);

  /**
   * \brief Compute GoFigure file type from a given filename
   * \param[in] iFileName filename
   * \param[out] oFileType file type
   * \return true if (png, jpeg or tiff)
   * \return false else
   */
  bool ComputeFileType(const QString & iFileName, GoFigure::FileType & oFileType);

  void LoadPlugins();

  void PopulateMenus(QObject *plugin);

  void AddToMenu(QObject *, const QStringList &, QMenu *,
                 const char *, QActionGroup *);

  QMenu *                     m_FilteringMenu;
  QDir                        m_PluginsDir;
  QStringList                 m_PluginFileNames;
  QGoTabManager *             m_TabManager;
  QToolBar *                  m_ModeToolBar;
  QToolBar *                  m_TracesToolBar;
  QToolBar *                  m_TraceSettingsToolBar;
  QToolBar *                  m_ViewToolBar;
  std::list< vtkLSMReader * > m_LSMReader;
  QGoWizardDB *               m_DBWizard;
  QGoAboutWidget *            m_AboutWidget;
  QGoDBInitializationWizard * m_DBInitializationWizard;

  QGoNetworkUtilities *m_NetworkUtilities;
  bool                 m_ManualUpdate;

  /** \brief */
  void ReadSettings();

  /** \brief */
  void WriteSettings();

  void CreateSignalSlotsConnection();

  /** \brief */
  itk::QtSignalAdaptor m_SignalAdaptor;
  itk::QtProgressBar   m_Bar;

  /** \brief list of recent files */
  QStringList m_RecentSingleFiles;
  QStringList m_RecentMultipleFiles;
  QStringList m_RecentDatabaseFiles;
  bool        m_DatabaseSetUp;
  QAction *   actionSet_Up_Database;

  /** \brief current file name */
  QString m_CurrentFile;

  /** \brief Remove path from a given FileName*/
  QString strippedName(const QString & fullFileName);

private:
  unsigned int m_MaxNumberOfTraces;
  Q_DISABLE_COPY(QGoMainWindow);
};
#endif
