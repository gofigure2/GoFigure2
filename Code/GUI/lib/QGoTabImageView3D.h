/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

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
#ifndef __QGoTabImageView3D_h
#define __QGoTabImageView3D_h

#include "QGoTabImageViewNDBase.h"
#include "SnapshotHelper.h"

#include <QHBoxLayout>
#include <QSpinBox>

class vtkImageData;
class vtkActor;
// class vtkQuadricLODActor;

class QAction;
class QToolBar;
class QDockWidget;

class QGoImageView3D;

#include "QGoGUILibConfigure.h"

/**
\class QGoTabImageView3D
\brief
\example GUI/lib/qgotabimageview3d.cxx
*/
class QGOGUILIB_EXPORT QGoTabImageView3D : public QGoTabImageViewNDBase
  {
  Q_OBJECT
public:
  explicit QGoTabImageView3D(QWidget* parent = 0);
  virtual ~QGoTabImageView3D();

  typedef QGoTabImageViewNDBase::QGoDockWidgetStatusPair QGoDockWidgetStatusPair;

  GoFigure::TabDimensionType GetTabDimensionType() const;

  virtual void Update();

  void setupUi(QWidget* parent);
  void retranslateUi(QWidget *parent);

  virtual std::list<QWidget*> AdditionalWidget()
  { return std::list<QWidget*>(); }

  virtual void WriteSettings() {}
  virtual void ReadSettings() {}

signals:
  void SliceViewXYChanged(int Slice);
  void SliceViewXZChanged(int Slice);
  void SliceViewYZChanged(int Slice);

  void FullScreenViewChanged(int View);

public slots:
  QString SnapshotViewXY(const GoFigure::FileType& iType,
                         const QString& iBaseName = QString("snapshot"));
  QString SnapshotView2(const GoFigure::FileType& iType,
                        const QString& iBaseName = QString("snapshot"));
  QString SnapshotView3(const GoFigure::FileType& iType,
                        const QString& iBaseName = QString("snapshot"));
  QString SnapshotViewXYZ(const GoFigure::FileType& iType,
                          const QString& iBaseName = QString("snapshot"));

  void SetSliceViewXY(const int&);
  void SetSliceViewXZ(const int&);
  void SetSliceViewYZ(const int&);

  void SetFullScreenView(const int& iS);
  void Quadview();
  void FullScreenViewXY();
  void FullScreenViewXZ();
  void FullScreenViewYZ();
  void FullScreenViewXYZ();

  void ChangeLookupTable();
  void ShowScalarBar(const bool&);
  void ChangeBackgroundColor();

  void DisplayAnnotations();
  void DisplaySplinePlanes();
  void DisplayCube();

  void Change3DPerspectiveToAxial();
  void Change3DPerspectiveToCoronal();
  void Change3DPerspectiveToSagittal();

  /**
   * \brief Mouse interaction style set as default
   */
  virtual void DefaultInteractorBehavior(bool);
  /**
   * \brief Mouse interaction style allows user to zoom in/out volume with all
   * buttons
   */
  virtual void ZoomInteractorBehavior(bool);
  /**
   * \brief Mouse interaction style allows user to pan volume with all buttons
   */
  virtual void PanInteractorBehavior(bool);

  void TakeSnapshot();

protected:
  QGoImageView3D* m_ImageView;

  QAction* m_BackgroundColorAction;

  void GetBackgroundColorFromImageViewer();
  void SetBackgroundColorToImageViewer();
  void CreateAllViewActions();
  void CreateModeActions();

  void SetImageToImageViewer(vtkImageData* image);
  int* GetImageCoordinatesFromWorldCoordinates(double pos[3]);

  virtual void RemoveActorFromViewer(const int& iId, vtkActor* iActor);
  virtual void DisplayActorInViewer(const int& iId, vtkActor* iActor);

//     virtual std::vector< vtkQuadricLODActor* >
  virtual std::vector<vtkActor*> AddContour(vtkPolyData* dataset,
                                            vtkProperty* property = NULL);

  virtual void SetSlice(int iDir, int* iIdx);

private:
  Q_DISABLE_COPY(QGoTabImageView3D);
  };
#endif
