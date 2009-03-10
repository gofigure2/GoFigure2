#ifndef __QGoMainWindow_h
#define __QGoMainWindow_h

#include <qmainwindow.h>

#include <vtkImageData.h>
#include <itkImage.h>

#include <QImagePageViewTracer.h>
#include <itkImageToVTKImageFilter.h>
#include <qactiongroup.h>

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

  void on_actionAbout_activated( );
  void on_actionAbout_Qt_activated( );

  void SetContourTracerOn(const bool& iChecked);
  void SetContourTracerOff(const bool& iChecked);

//   void SetTracerToPolygonTracer();
//   void SetTracerToFreeLineTracer();
//   void SetTracerToTranslateContourTracer();
//   void SetTracerToRotateContourTracer();
//   void SetTracerToScaleContourTracer();

  void ChangeLookupTable( );

  void SetColorForGivenId();
  void ValidateContourTracer();

protected:
  QString m_FileName;
  ImagePointer m_ITKImage;
  vtkImageData* m_VTKImage;
  QImagePageViewTracer* m_PageView;
  VTKConvertImagePointer m_Convert;
  QGoLUTDialog* m_LUTDialog;
  std::map< unsigned int, QColor > m_IdColorMap;
  QActionGroup *Fullscreengroup;

  void OpenImage( const QString& iFileName );
  void DisplayImage( vtkImageData* iImage, QString iTag );
  void Fullscreenbuttons();

private:
  QGoMainWindow( const QGoMainWindow& );
  QGoMainWindow operator = ( const QGoMainWindow& );

};

#endif
