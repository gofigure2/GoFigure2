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

#include <qactiongroup.h>
#include <qvector.h>
#include <qhash.h>
#include <qprogressbar.h>

#include "QGoLUTDialog.h"
#include "ui_go.h"


class QGoMainWindow : public QMainWindow,
  private Ui::go_mainwindow
{
  Q_OBJECT

public:
  typedef itk::Image< unsigned char, 3 > ImageType;
  typedef ImageType::Pointer ImagePointer;

  typedef itk::ImageToVTKImageFilter< ImageType > VTKConvertImageType;
  typedef VTKConvertImageType::Pointer VTKConvertImagePointer;

  QGoMainWindow( );
  virtual ~QGoMainWindow();

  void SetFileName( const QString& iFileName );


protected slots:
  void on_actionOpen_activated( );
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
  void ShowProgressLoading( itk::Object * myFilter );
  void HideProgressLoading();

  /**\Update the full screen button group when the current tab is changed*/
  void UpdateFullScreenViewButtons( const int& idx);
  void UpdateTracerButtons( const int& idx);


//   void SetTracerToPolygonTracer();
//   void SetTracerToFreeLineTracer();
//   void SetTracerToTranslateContourTracer();
//   void SetTracerToRotateContourTracer();
//   void SetTracerToScaleContourTracer();

  void ChangeLookupTable( );

  void SetColorForGivenId( const bool& iSelected = true );
  void ValidateContourTracer();
  void ReinitializeContourTracer();
  void openRecentFile();

protected:
  template< class T > void ValidateContourTracerHelper( T* PageView );
  template< class T > void SetBackgroundColor( T* PageView );
  template< class T > void UpdateFullScreenViewButtonsHelper( T* PageView );
  template< class T > void SetFullScreen( const int & ViewID, T* PageView );
  template< class T > void SetRendering( T* myPageView );

  void SetFullScreenDispatch( const int & ViewID );

  QVector< ImagePointer >   m_ITKImage;
  QVector< vtkImageData* >  m_VTKImage;

  /** \brief Quad View*/
  QVector< QWidget* > m_PageView;

  /** \brief Convert itk::Image format to vtkImageData */
  QVector< VTKConvertImagePointer > m_Convert;

  /** \brief Look-up Table dialog */
  QGoLUTDialog* m_LUTDialog;

  QHash< unsigned int, QColor > m_IdColorMap;
  QActionGroup* m_FullscreenGroup;

  /** \brief */
  void setCurrentFile(const QString &fileName);

  /** \brief */
  void updateRecentFileActions();

  /** \brief Open Image with given iFileName
      \param[in] iFileName
  */
  void OpenImage( const QString& iFileName );

  /** \brief Display Image in a "quad-view" widget in a tab.
    The title of the tab is given by iTag.
      \param[in] iImage
      \param[in] iTag
  */
  void DisplayImage( const QString& iTag );

  /** \brief Create Fullscreengroup*/
  void Fullscreenbuttons();

  void readSettings();
  void writeSettings();

  itk::QtSignalAdaptor m_SignalAdaptor;

  /** \brief list of recent files */
  QStringList m_RecentFiles;

  /** \brief current file name */
  QString m_CurrentFile;

  /** \brief Remove path from a given FileName*/
  QString strippedName(const QString &fullFileName);

  itk::QtProgressBar m_Bar;

  enum { MaxRecentFiles = 5 };
  QAction *recentFileActions[MaxRecentFiles];

private:
  QGoMainWindow( const QGoMainWindow& );
  QGoMainWindow operator = ( const QGoMainWindow& );

  void DisplayInTab( vtkImageData* myImage, const int& TabIndex );
  void OpenAndDisplayLSMFile( const QString& iTag,
  const int& timePoint,
  const bool& ComposeChannels );

};
#endif
