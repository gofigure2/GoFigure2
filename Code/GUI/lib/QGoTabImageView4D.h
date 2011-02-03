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

#ifndef __QGoTabImageView4D_h
#define __QGoTabImageView4D_h

#include "QGoTabElementBase.h"

class QGoPrintDatabase;

class QGoImageView3D;
class QGoNavigationDockWidget;
class QGoContourManualSegmentationWidget;

#ifdef   ENABLEVIDEORECORD
class QGoVideoRecorder;
#endif

class vtkContourWidget;
class vtkOrientedGlyphContourRepresentation;
// class vtkQuadricLODActor;
class vtkActor;
class vtkProperty;
class vtkPolyData;

#include "vtkSmartPointer.h"

#include "itkMegaCaptureReader.h"
#include "GoFigureFileInfoMultiIndexContainerHelper.h"

#include "QGoGUILibConfigure.h"

#include <QResizeEvent>
#include <QSplitter>

/**
\class QGoTabImageView4D
\brief
\example GUI/lib/qgotabimageview4d.cxx
*/
class QGOGUILIB_EXPORT QGoTabImageView4D:public QGoTabElementBase
{
  Q_OBJECT
public:
  QGoTabImageView4D(QWidget *parent = 0);
  virtual ~QGoTabImageView4D();

  typedef QGoTabElementBase::QGoDockWidgetStatusPair QGoDockWidgetStatusPair;

  GoFigure::TabDimensionType GetTabDimensionType() const;

  void SetMegaCaptureFile(
    const GoFigureFileInfoHelperMultiIndexContainer & iContainer,
    const GoFigure::FileType & iFileType,
    const std::string & iHeader);

  virtual void Update();

  void setupUi(QWidget *parent);

  void retranslateUi(QWidget *parent);

  virtual std::vector< QAction * > ViewActions();

  virtual void WriteSettings() {}
  virtual void ReadSettings() {}

  virtual void ValidateContour(const int & iId);

  QGoPrintDatabase *m_DataBaseTables;
signals:
  void TimePointChanged(int TimePoint);

  void ZSliceChanged(int ZSlice);

  void YSliceChanged(int YSlice);

  void XSliceChanged(int XSlice);

  void FullScreenViewChanged(int FullScreen);

public slots:
  void SetTimePoint(const int &);

  void SetZSlice(const int &);

  void SetYSlice(const int &);

  void SetXSlice(const int &);

  QString SnapshotViewXY( const GoFigure::FileType & iType,
                          const QString & iBaseName = QString("snapshot-xy-") );

  QString SnapshotViewXZ( const GoFigure::FileType & iType,
                         const QString & iBaseName = QString("snapshot-xz-") );

  QString SnapshotViewYZ( const GoFigure::FileType & iType,
                         const QString & iBaseName = QString("snapshot-yz-") );

  QString SnapshotViewXYZ( const GoFigure::FileType & iType,
                           const QString & iBaseName = QString("snapshot-xyz-") );

  QString SnapshotViewXT( const GoFigure::FileType & iType,
                          const QString & iBaseName = QString("snapshot-xt-") );

  QString SnapshotViewYT( const GoFigure::FileType & iType,
                          const QString & iBaseName = QString("snapshot-yt-") );

  QString SnapshotViewXYT( const GoFigure::FileType & iType,
                           const QString & iBaseName = QString("snapshot-xyt-") );

  void SetFullScreenView(const int & iS);

  void Octview();

  void QuadviewXYZ();

  void QuadviewXYT();

  void FullScreenViewXY();

  void FullScreenViewXZ();

  void FullScreenViewYZ();

  void FullScreenViewXYZ();

  void FullScreenViewXT();

  void FullScreenViewYT();

  void FullScreenViewXYT();

  void ChangeLookupTable();

  void ShowScalarBar(const bool &);

  void ChangeBackgroundColor();

  void ShowAllChannels(bool iChecked);

  void ShowOneChannel(int iChannel);

  void ActivateManualSegmentationEditor(const bool & iActivate);

  void ValidateContour();

  void ChangeContourRepresentationProperty();

protected:
  QSplitter *                                    m_Splitter;
  QGoImageView3D *                               m_XYZImageView;
  QGoImageView3D *                               m_XYTImageView;
  vtkSmartPointer< vtkImageData >                m_XYZImage;
  vtkSmartPointer< vtkImageData >                m_XYTImage;
  std::vector< vtkSmartPointer< vtkImageData > > m_XYZInternalImages;
  std::vector< vtkSmartPointer< vtkImageData > > m_XYTInternalImages;

  QColor       m_BackgroundColor;
  int          m_TimePoint;
  int          m_ZSlice;
  unsigned int m_ContourId;

  itk::MegaCaptureReader::Pointer m_Reader1;
  itk::MegaCaptureReader::Pointer m_Reader2;

  GoFigureFileInfoHelperMultiIndexContainer m_FileList;
  GoFigure::FileType                        m_FileType;
  bool                                      m_FirstUpdate;

  std::vector< QAction * >            m_ViewActions;
  QGoNavigationDockWidget *           m_NavigationDockWidget;
  QGoContourManualSegmentationWidget *m_ManualSegmentationWidget;

    #ifdef   ENABLEVIDEORECORD
  QGoVideoRecorder *m_VideoRecorderWidget;
#endif

  std::vector< vtkSmartPointer< vtkContourWidget > >                      m_ContourWidget;
  std::vector< vtkSmartPointer< vtkOrientedGlyphContourRepresentation > > m_ContourRepresentation;

  void CreateAllViewActions();

  void CreateModeActions();

  void CreateVisuDockWidget();

  void CreateManualSegmentationdockWidget();

  void GetBackgroundColorFromImageViewer();

  void SetBackgroundColorToImageViewer();

//    std::vector< vtkQuadricLODActor* >
  std::vector< vtkActor * > AddContour(vtkPolyData *dataset,
                                       vtkProperty *property = NULL);

  int *GetImageCoordinatesFromWorldCoordinates(double pos[3]);

  virtual void resizeEvent(QResizeEvent *event);
};
#endif
