/*=========================================================================
  Author: $Author:$  // Author of last commit
  Version: $Rev:$  // Revision of last commit
  Date: $Date:$  // Date of last commit
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

#ifndef __QGoMainWindow_h
#define __QGoMainWindow_h

#include <QMainWindow>
#include <QDir>

#include "vtkImageData.h"
#include "itkImage.h"

#include "itkImageToVTKImageFilter.h"
#include "itkQtProgressBar.h"
#include "itkQtAdaptor.h"
#include "vtkLSMReader.h"

#include "GoFigureFileInfoMultiIndexContainerHelper.h"
#include "QGoWizardDB.h"

#include "CreateDataBaseHelper.h"

#include "GoFigureGlobalDefinition.h"

class QGoTabManager;
class QGoTabImageView3DwT;
class QGoTabImageView3D;
class QGoTabImageView2D;

#include "ui_go.h"

class QGoMainWindow : public QMainWindow,
  private Ui::go_mainwindow
{
  Q_OBJECT

public:
  friend class QGoTabManager;

  typedef itk::Image< unsigned char, 3 > ImageType;
  typedef itk::ImageToVTKImageFilter< ImageType > VTKConvertImageType;
  typedef VTKConvertImageType::Pointer            VTKConvertImagePointer;

//   typedef itk::MultiFileReader::FILETYPE FILETYPE;

  QGoMainWindow( );
  virtual ~QGoMainWindow();

  void SetSingleFileName( const QString& iFileName );

private slots:
  void on_actionOpen_Single_File_triggered( );
  void openRecentSingleFile();
//   void on_actionOpen_Multiple_Files_triggered( );
  void on_actionOpen_MegaCapture_Files_triggered( );
  void openRecentMultipleFile();

  void on_actionGoFigure2_Website_triggered( );
  void on_actionUser_mailing_list_triggered();
  void on_actionDeveloper_mailing_list_triggered();

  void on_actionOpen_Mesh_triggered( );

  void on_actionUse_DataBase_triggered();
  void openFilesfromDB();

  void on_actionExport_LSM_to_MegaFile_triggered( );

  void on_actionClose_triggered( );
  void on_actionClose_all_triggered( );
  void on_actionQuit_triggered( );

  void on_actionAbout_triggered( );
  void on_actionAbout_Qt_triggered( );

  void ApplyImageFilter();

private:

  std::map< GoFigure::TabDimensionType, std::list< QAction* > >
    m_TabDimPluginActionMap;

  /** \brief */
  void openRecentFile(const bool& IsSerie);

  /** \brief */
  void SetCurrentSingleFile( const QString &fileName );
  void SetCurrentMultiFile( const QString &fileName );

  /** \brief */
  enum { MaxRecentFiles = 5 };
  QAction *recentSingleFileActions[MaxRecentFiles];
  QAction *recentMultipleFileActions[MaxRecentFiles];

  void UpdateRecentFileActions( QStringList list, QMenu *menu,
    QAction *recentFileActions[MaxRecentFiles] );

  QGoTabImageView3DwT* CreateNewTabFor3DwtImage(
    const GoFigureFileInfoHelperMultiIndexContainer& iFileList,
    const GoFigure::FileType& iFileType,
    const std::string& iHeader,
    const int& iTimePoint );

  QGoTabImageView3DwT* CreateNewTabFor3DwtImage(
    vtkLSMReader* iReader, const QString& iFile );
  QGoTabImageView3D* CreateNewTabFor3DImage( vtkImageData*, const QString& );
  QGoTabImageView2D* CreateNewTabFor2DImage( vtkImageData*, const QString& );

  /** \brief Open Image with given iFileName
  \param[in] iFileName  */
//   void OpenImageWithITK( const QString& iFileName );
  void OpenLSMImage( const QString& iFile, const int& iTimePoint );

  void LoadPlugins();
  void PopulateMenus( QObject *plugin );
  void AddToMenu( QObject*, const QStringList&, QMenu*,
    const char*, QActionGroup* );

  QMenu*            m_FilteringMenu;
  QDir              m_PluginsDir;
  QStringList       m_PluginFileNames;
  QGoTabManager*    m_TabManager;
  QToolBar*         m_ViewToolBar;
  vtkLSMReader*     m_LSMReader;
  QGoWizardDB*      m_DBWizard;

  /** \brief */
  void ReadSettings();

  /** \brief */
  void WriteSettings();

  void CreateSignalSlotsConnection();

  /** \brief */
  itk::QtSignalAdaptor m_SignalAdaptor;
  itk::QtProgressBar m_Bar;

  /** \brief list of recent files */
  QStringList m_RecentSingleFiles;
  QStringList m_RecentMultipleFiles;

  /** \brief current file name */
  QString m_CurrentFile;

  /** \brief Remove path from a given FileName*/
  QString strippedName(const QString &fullFileName);

private:
  QGoMainWindow( const QGoMainWindow& );
  QGoMainWindow operator = ( const QGoMainWindow& );
};
#endif
