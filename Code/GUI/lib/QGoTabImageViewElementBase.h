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

#ifndef __QGoTabImageViewElementBase_h
#define __QGoTabImageViewElementBase_h

#include <QHBoxLayout>
#include "vtkSmartPointer.h"

#include "QGoTabElementBase.h"

#include "QGoGUILibConfigure.h"

class ContourMeshContainer;
class vtkPolyData;
class vtkContourWidget;
class vtkOrientedGlyphContourRepresentation;
class vtkDataSet;
class vtkProperty;
// class vtkQuadricLODActor;
class vtkActor;

class QGoNavigationDockWidget;
class QGoContourManualSegmentationWidget;

/**
\class QGoTabImageViewElementBase
\brief
\example GUI/lib/qgotabimageviewelementbase.cxx
*/
class QGOGUILIB_EXPORT QGoTabImageViewElementBase:public QGoTabElementBase
{
  Q_OBJECT
public:
  /** \brief Constructor */
  explicit QGoTabImageViewElementBase(QWidget *parent = 0);
  /** \brief Destructor */
  virtual ~QGoTabImageViewElementBase();

  typedef QGoTabElementBase::QGoDockWidgetStatusPair QGoDockWidgetStatusPair;

  /** \brief Update the rendering of the tab */
  virtual void Update() = 0;

  /** \brief */
  virtual void SetColor(const bool & iColor);

  /** \brief Write Settings */
  virtual void WriteSettings();

  /** \brief Read Settings */
  virtual void ReadSettings();

  /** \brief Validate one contour traced by using the ContourWidget.*/
  virtual void ValidateContour(const int & iId);

public slots:
  /** \brief Change the background color. */
  void ChangeBackgroundColor();

  /** \brief Show all channels if iChecked is true.*/
  virtual void ShowAllChannels(bool iChecked) = 0;

  /** \brief Show only one channel (iChannel).*/
  virtual void ShowOneChannel(int iChannel) = 0;

  /** \brief Activate the manual segmentation editor (ContourWidget).*/
  void ActivateManualSegmentationEditor(const bool & iActivate);

  /** \brief Validate contour traced in the ContourWidget.*/
  virtual void ValidateContour();

  /** \brief Reinitialize contour in the ContourWidget (delete contour,
   *  and restart with no contours.
   *  */
  void ReinitializeContour();

  /** \brief Change contour representation property (color, line width, etc.) */
  void ChangeContourRepresentationProperty();

  /** \brief Re-edit the iId^th contour. */
  void ReEditContour(const unsigned int & iId);

  virtual void TakeSnapshot() = 0;

protected:
  bool         m_Color;
  QColor       m_BackgroundColor;
  unsigned int m_ContourId;
  bool         m_ReEditContourMode;

  double m_LinesWidth;
  QColor m_LinesColor;
  QColor m_NodesColor;
  QColor m_ActiveNodesColor;

  QHBoxLayout *m_LayOut;
  QAction *    m_TakeSnapshotAction;

  std::vector< vtkSmartPointer< vtkContourWidget > >                      m_ContourWidget;
  std::vector< vtkSmartPointer< vtkOrientedGlyphContourRepresentation > > m_ContourRepresentation;
  ContourMeshContainer *                                                  m_ContourMeshContainer;

  QGoNavigationDockWidget *           m_NavigationDockWidget;
  QGoContourManualSegmentationWidget *m_ManualSegmentationWidget;

  virtual void CreateManualSegmentationdockWidget();

  virtual void CreateToolsActions();

  virtual void GetBackgroundColorFromImageViewer() = 0;

  virtual void SetBackgroundColorToImageViewer() = 0;

  virtual int *GetImageCoordinatesFromWorldCoordinates(double pos[3]) = 0;

  virtual void RemoveActorFromViewer(const int & iId, vtkActor *iActor) = 0;

  virtual void DisplayActorInViewer(const int & iId, vtkActor *iActor) = 0;

  //   virtual std::vector< vtkQuadricLODActor* >
  virtual std::vector< vtkActor * >  AddContour(vtkPolyData *dataset,
                                                vtkProperty *property = NULL) = 0;

  virtual void SetSlice(int iDir, int *iIdx) = 0;

private:
  Q_DISABLE_COPY(QGoTabImageViewElementBase);
};
#endif
