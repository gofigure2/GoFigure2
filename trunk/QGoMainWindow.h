#ifndef __QGoMainWindow_h
#define __QGoMainWindow_h

#include <qmainwindow.h>

#include <vtkImageData.h>
#include <itkImage.h>

#include <QImagePageViewTracer.h>
#include <itkImageToVTKImageFilter.h>

#include <qactiongroup.h>
#include <qvector.h>
#include <qhash.h>

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
//   void on_actionSave_activated();
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

  void on_actionAbout_activated( );
  void on_actionAbout_Qt_activated( );

  void SetContourTracerOn(const bool& iChecked);
  void SetContourTracerOff(const bool& iChecked);
  void showprogressloading ();


//   void SetTracerToPolygonTracer();
//   void SetTracerToFreeLineTracer();
//   void SetTracerToTranslateContourTracer();
//   void SetTracerToRotateContourTracer();
//   void SetTracerToScaleContourTracer();

  void ChangeLookupTable( );

  void SetColorForGivenId( const bool& iSelected = true );
  void ValidateContourTracer();
  void openRecentFile();

protected:
  ImagePointer m_ITKImage;
  vtkImageData* m_VTKImage;

  /** \brief Quad View*/
  QVector< QImagePageViewTracer* > m_PageView;

  /** \brief Convert itk::Image format to vtkImageData */
  VTKConvertImagePointer m_Convert;

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
  void DisplayImage( vtkImageData* iImage, QString iTag );

  /** \brief Create Fullscreengroup*/
  void Fullscreenbuttons();

  /** \brief list of recent files */
  QStringList m_RecentFiles;

  /** \brief current file name */
  QString m_CurrentFile;

  /** \brief Remove path from a given FileName*/
  QString strippedName(const QString &fullFileName);

  enum { MaxRecentFiles = 5 };
  QAction *recentFileActions[MaxRecentFiles];

private:
  QGoMainWindow( const QGoMainWindow& );
  QGoMainWindow operator = ( const QGoMainWindow& );

};

#endif
