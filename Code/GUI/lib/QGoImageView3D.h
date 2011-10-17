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

#ifndef __QGoImageView3D_h
#define __QGoImageView3D_h

#include "QGoImageView.h"
#include "SnapshotHelper.h"

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QSplitter>
#include <QSlider>

#include <map>

#include "vtkInteractorStyleImage2D.h"
#include "QGoGUILibConfigure.h"

#include "vtkCommand.h"

class QVTKWidget;
class vtkEventQtSlotConnect;
class QSplitterChild;

class vtkProp3D;
class vtkProperty;
class vtkImageData;
class vtkViewImage2D;
class vtkViewImage3D;
class vtkViewImage2DCollection;

// For the box widget
class vtkOrientedBoxWidget;

// For the plane widget
class vtkImplicitPlaneWidget;

// volumre rendering
class vtkPiecewiseFunction;

/**
\class QGoImageView3D
\brief class for the visualization of 3D Image represented by one vtkImageData*.
\example GUI/lib/qgoimageview3d.cxx
*/
class QGOGUILIB_EXPORT QGoImageView3D:public QGoImageView
{
  Q_OBJECT
public:
  /** \brief Constructor by default */
  explicit QGoImageView3D(QWidget *parent = 0);

  /** \brief Destructor. */
  virtual ~QGoImageView3D();

  void Update();

  vtkViewImage3D * GetImageViewer3D();

  QVTKInteractor * GetInteractor(const int &);

  /** \brief Set the image to displaid. */
  virtual void SetImage(vtkImageData *iImage);

  virtual void RemoveActor(const int & iId, vtkActor *iActor);

  virtual void AddActor(const int & iId, vtkActor *iActor);

//   std::vector< vtkQuadricLODActor* >
  std::vector< vtkActor * > AddContour(vtkPolyData *iDataset,
                                       vtkProperty *iProperty = NULL);

  virtual void setupUi(QWidget *parent);

  virtual void retranslateUi(QWidget *parent);

  int GetFullScreenView() const;

  void SaveStateSplitters();

  int GetSliceViewXY() const;

  int GetSliceViewXZ() const;

  int GetSliceViewYZ() const;

  // MODES
  /**
   * \brief Use the default interactor style
   */
  void DefaultMode();

  /**
   * \brief Use the zoom interactor style
   */
  void ZoomMode();

  /**
   * \brief Use the pan interactor style
   */
  void PanMode();

  /**
   * \brief switch to contour picking mode
   */
  void ActorPickingMode();

  // WIDGETS
  /**
   * \brief Use the one click interactor style
   */
  void EnableSeedWidget(bool iActivate);

  /**
   * \brief Creates a box in 3d view to allow multiple meshes selection
   */
  void EnableBoxWidget(bool);

  /**
   * \brief Creates a box in 3d view to allow multiple meshes selection
   */
  void EnablePlaneWidget(bool);

  /**
   * \brief Creates a box in 3d view to allow multiple meshes selection
   */
  vtkProp * GetPickedActor();

  vtkActor * GetCurrentActor();

  bool      GetCurrentState();

  void ChangeCursorShape(QCursor iCursorShape);

  void UpdateImage();

signals:
  void SliceViewXYChanged(int Slice);

  void SliceViewXZChanged(int Slice);

  void SliceViewYZChanged(int Slice);

  void FullScreenViewChanged(int View);

  void SelectionChanged();

  void VisibilityChanged();

  void CurrentActorUpdated();

  void UpdateRenderEvent();

  void NewWindowLevel(double, double);

public slots:
  QString SnapshotViewXY( const GoFigure::FileType & iType,
                          const QString & iBaseName = QString("snapshot-xy-") );

  QString SnapshotViewXZ( const GoFigure::FileType & iType,
                         const QString & iBaseName = QString("snapshot-xz-") );

  QString SnapshotViewYZ( const GoFigure::FileType & iType,
                         const QString & iBaseName = QString("snapshot-yz") );

  QString SnapshotViewXYZ( const GoFigure::FileType & iType,
                           const QString & iBaseName = QString("snapshot-xyz-") );

  void SetSliceViewXY(const int &);

  void SetSliceViewXZ(const int &);

  void SetSliceViewYZ(const int &);

  void SetFullScreenView(const int & iS);

  void SetCamera(int);

  void UpdateLUT();

  void ShowSplinePlane();

  void ShowCube3D();

  /**
   * \brief Creates a box in 3d view to allow multiple meshes selection
   */
  void EnableVolumeRendering(const std::vector<vtkImageData*>& iImages,
                             const std::vector<vtkPiecewiseFunction*>& iOpacities);

  void DisableVolumeRendering();

  void UpdateCurrentActorSelection(vtkObject *caller);

  void UpdateCurrentActorVisibility(vtkObject *caller);

  virtual void SetLookupTable(vtkLookupTable *);

  virtual void ShowScalarBar(const bool &);

  /*
   * \brief Synchronize the views
   * \param[in] iSynchronize Enable/disable synchronization
   */
  void SynchronizeViews( bool iSynchronize);

protected:
  QSplitter *     VSplitter;
  QSplitterChild *HtSplitter;
  QSplitterChild *HbSplitter;

  QWidget *    LayOutWidget1;
  QHBoxLayout *LayOut1;
  QSlider *    SliderXY;
  QVTKWidget * QvtkWidget_XY;

  QWidget *    LayOutWidget2;
  QHBoxLayout *LayOut2;
  QSlider *    SliderXZ;
  QVTKWidget * QvtkWidget_XZ;

  QWidget *    LayOutWidget3;
  QHBoxLayout *LayOut3;
  QSlider *    SliderYZ;
  QVTKWidget * QvtkWidget_YZ;

  QWidget *    LayOutWidget4;
  QHBoxLayout *LayOut4;
  QVTKWidget * QvtkWidget_XYZ;

  vtkViewImage3D *m_View3D;

  vtkEventQtSlotConnect *VtkEventQtConnector;
  vtkProperty *          m_HighlightedContourProperty;

  int  IsFullScreen;
  bool m_FirstRender;
  bool m_Initialized;

  bool m_ShowCube;

  vtkOrientedBoxWidget *  m_BoxWidget;
  vtkImplicitPlaneWidget *m_PlaneWidget;

  vtkActor *m_CurrentActor;
  bool      m_CurrentState;

  virtual void resizeEvent(QResizeEvent *event);

  void SetupVTKtoQtConnections();

  void UpdateOnFirstRender();

  void Quadview();

  void FullScreenViewXY();

  void FullScreenViewXZ();

  void FullScreenViewYZ();

  void FullScreenViewXYZ();

  void InitializeBoxWidget();

  void InitializePlaneWidget();

protected slots:
  void MoveSliderXY();

  void MoveSliderXZ();

  void MoveSliderYZ();

private:
  Q_DISABLE_COPY(QGoImageView3D);
};

#endif
