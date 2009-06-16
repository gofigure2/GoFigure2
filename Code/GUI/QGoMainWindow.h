/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
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

#ifndef __QGoMainWindow_h
#define __QGoMainWindow_h

#include <qmainwindow.h>

#include <vtkImageData.h>
#include <itkImage.h>

// ALEX: Have to use decorator pattern here
#include "QImagePageViewTracer.h"
#include "itkImageToVTKImageFilter.h"
#include "itkQtProgressBar.h"
#include "itkQtAdaptor.h"
#include "vtkLSMReader.h"

// ALEX: have to make a conditional flag to check if
// vtk has been compiled with the good drivers
// #ifdef VTK_USE_MYSQL
  #include "CreateDataBaseHelper.h"
// #endif
#include "QGoCreateDBDialog.h"
#include "QGoWizardDB.h"
#include "QGoPrintDatabase.h"

#include <qactiongroup.h>
#include <qvector.h>
#include <qhash.h>
#include <qprogressbar.h>

#include "QGoLUTDialog.h"
#include "ui_go.h"

//#include "itkChanAndVeseSegmentationFilter.h"
// #include <qsettings.h>

class QGoMainWindow : public QMainWindow,
  private Ui::go_mainwindow
{
  Q_OBJECT

public:
  typedef itk::Image< unsigned char, 3 >  ImageType;
  typedef ImageType::Pointer              ImagePointer;

 // typedef itk::Image< float, 3 >          LevelSetImageType;
 // typedef itk::ChanAndVeseSegmentationFilter< LevelSetImageType, ImageType >
//    LocalChanAndVeseSegmentationFilterType;

  typedef itk::ImageToVTKImageFilter< ImageType > VTKConvertImageType;
  typedef VTKConvertImageType::Pointer            VTKConvertImagePointer;

  QGoMainWindow( );
  virtual ~QGoMainWindow();

  void SetFileName( const QString& iFileName, const bool& IsSerie  );


protected slots:
  void on_actionOpen_Single_File_activated( );
  void openRecentSingleFile();
  void on_actionOpen_Multiple_Files_activated( );
  void openRecentMultipleFile();

  void on_actionOpen_Mesh_activated( );

  // #ifdef VTK_USE_MYSQL
  //void on_actionCreate_DataBase_activated( ) { QGoCreateDBDialog dialog; }
  void on_actionUse_DataBase_activated();
  void openFilesfromDB();
 // void on_actionImport_Dataset_in_DataBase_activated( ) {};
  //void on_actionOpen_DataBase_activated( ) {};
  // #endif

  void on_actionClose_activated( );
  void on_actionClose_all_activated( );
  void on_actionQuit_activated( );

  void on_actionBackground_activated( );
  void on_actionLookup_Table_activated( );

  void on_actionQuad_View_activated( );
  void on_actionFull_screen_XY_activated( );
  void on_actionFull_screen_YZ_activated( );
  void on_actionFull_screen_XZ_activated( );
  void on_actionFull_screen_XYZ_activated( );
  void on_actionVolume_rendering_XYZ_activated( );
  void on_actionScale_bars_activated( );
  void on_actionSnapshot_activated( );

  void on_actionAbout_activated( );
  void on_actionAbout_Qt_activated( );

  void SetContourTracerOn( const bool& iChecked );
  void SetContourTracerOff( const bool& iChecked );

  void SetSeedWidgetOn( const bool& iChecked );
  void SetSeedWidgetOff( const bool& iChecked );

  void OneClickSegmentation( );

  void ShowProgressLoading( itk::Object * myFilter );
  void HideProgressLoading();

  /** \brief Update the full screen button group when the current tab is changed.
  */
  void UpdateToolBarViewButtons( const int& idx);
  void UpdateTracerButtons( const int& idx);


//   void SetTracerToPolygonTracer();
//   void SetTracerToFreeLineTracer();
//   void SetTracerToTranslateContourTracer();
//   void SetTracerToRotateContourTracer();
//   void SetTracerToScaleContourTracer();

  void ChangeLookupTable( );

  void SetColorForGivenId( const bool& iSelected = true );
  void ValidateContourTracer();
  void ReinitializeAndIncrementContourTracer();
  void ReinitializeContourTracer();


protected:
  template< class T > void ValidateContourTracerHelper( T* PageView );
  template< class T > void SetBackgroundColor( T* PageView );
  template< class T > void UpdateFullScreenViewButtonsHelper( T* PageView );
  template< class T > void SetFullScreen( const int & ViewID, T* PageView );
  template< class T > void SetRendering( T* myPageView );
  template< class T > void UpdateVolumeRenderingButton( T* PageView);
  template< class T > void writeSettingsPageView(T* PageView );


  /** \brief */
  void SetFullScreenDispatch( const int & ViewID );

  /** \brief */
  QVector< ImagePointer >   m_ITKImage;

  /** \brief */
  QVector< vtkImageData* >  m_VTKImage;

  /** \brief Quad View*/
  QVector< QWidget* > m_PageView;

  /** \brief Convert itk::Image format to vtkImageData */
  QVector< VTKConvertImagePointer > m_Convert;

  /** \brief Look-up Table dialog */
  QGoLUTDialog* m_LUTDialog;

  QGoWizardDB* m_Wizard;
  QGoPrintDatabase* m_DBTables;

  /** \brief */
  QHash< unsigned int, QColor > m_IdColorMap;

  /** \brief */
  QActionGroup* m_FullscreenGroup;

  //QSettings settings("MegasonLab", "Gofigure2");

  /** \brief */
  void openRecentFile(const bool& IsSerie);

  /** \brief */
  void setCurrentFile(const QString &fileName,const bool& IsSerie);

  /** \brief */
  enum { MaxRecentFiles = 5 };
  QAction *recentSingleFileActions[MaxRecentFiles];
  QAction *recentMultipleFileActions[MaxRecentFiles];

  void updateRecentFileActions( QStringList list, QMenu *menu, QAction *recentFileActions[MaxRecentFiles]);

  /** \brief Open Image with given iFileName
  \param[in] iFileName  */
  void OpenImage( const QString& iFileName );

  /** \brief Display Image in a "quad-view" widget in a tab.
  The title of the tab is given by iTag.
  \param[in] iTag  */
  void DisplayImage( const QString& iTag );

  /** \brief Create Fullscreengroup */
  void Fullscreenbuttons();

  /** \brief */
  void readSettings();

  /** \brief */
  void writeSettings();

  void StartSeedWidget();

  void CreateSignalSlotsConnection();

  /** \brief */
  itk::QtSignalAdaptor m_SignalAdaptor;

  /** \brief list of recent files */
  QStringList m_RecentSingleFiles;
  QStringList m_RecentMultipleFiles;

  /** \brief current file name */
  QString m_CurrentFile;

  /** \brief Remove path from a given FileName*/
  QString strippedName(const QString &fullFileName);

  itk::QtProgressBar m_Bar;

private:
  QGoMainWindow( const QGoMainWindow& );
  QGoMainWindow operator = ( const QGoMainWindow& );

  void DisplayInTab( vtkImageData* myImage, const int& TabIndex );
  void OpenAndDisplay( const QString& iTag, const bool& IsSerie, const int& Type );
};
#endif
