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
#ifndef __QGoSeedSegmentationBase_h
#define __QGoSeedSegmentationBase_h

#include <QObject>
#include <QWidget>
#include "QGoGUILibConfigure.h"

class QGoSeedBaseWidget;
class vtkPoints;
class vtkPolyData;

/**
 * \class QGoSeedSegmentation
 * \ingroup QGoSeed
 * \brief Base class for all segmentations from seed points
*/

class QGOGUILIB_EXPORT QGoSeedSegmentationBase:public QObject
{
  Q_OBJECT
public:
  /**
   * \brief Constructor
   */
  explicit QGoSeedSegmentationBase(QWidget *parentWidget = 0,
                                   vtkPoints *seeds = 0,
                                   int iSampling = 0);

  /**
   * \brief Destructor
   */
  ~QGoSeedSegmentationBase();

  /**
   * \brief Set the seeds from which the segmentation will be computed.
   *
   * \param[in] iSeed Seeds from which the segmentaion will be computed.
   * It can contain more than on seed.
   */
  void       setSeed(vtkPoints *iSeed);

  /**
   * \brief Get the seeds from which the segmentation will be computed.
   */
  vtkPoints * getSeed();

  /**
   * \brief Get the current seed from which the segmentation will be computed.
   */
  double * getSeedsPosition();

  /**
   * \brief Get the radius parameter for the segmentation.
   */
  double     getRadius();

  int        getSampling();

  /**
   * \brief Get the dockwidget associated to the segmentation method.
   */
  virtual QWidget * getWidget();

  /**
   * \brief Connect signals/slots to the segmentation dock widget
   * \param[in] iWidget Algorithm segmentation specific dock widget
   */
  void ConnectSignals(QGoSeedBaseWidget *iWidget);

  /**
   * \brief Specify on which one the segmentation will be applied since the input
   * data is multichannel.
   * \param[in] iText Channel on which want we want to apply the segmentation
   * algorithm
   */
  void SetChannel(int iChannel, const QString & iText);

  void SetNumberOfChannels(int iNumberOfChannels);

public slots:
  /**
   * \brief Set the value of the radius.
   * \param[in] iRadius Radius to be used for the segmentation.
   */
  void setRadius(double iRadius);

  /**
   * \brief Set the value of the radius.
   * \param[in] iSampling Radius to be used for the segmentation.
   */
  void setSampling(int iSampling);

signals:
  /**
   * \brief Signal sent to update the value of the pointed seeds.
   */
  void  getSeeds();

  /**
   * \brief Signal sent when the segmentation is done.
   */
  void  segmentationFinished();

  /**
   * \brief Signal to be send to the Widget to add channel to the channel
   * QComboBox
   * \param[in] iQString Name of the channel.
   */
  void addChannel(int iChannel, QString iQString);

  void setNumberOfChannels(int);

  void MeshCreated(vtkPolyData *, int);

  void ContourCreated(vtkPolyData *);

  void ImageProcessed();

  void UpdateSeeds();

  void SegmentationFinished();

  void CreateCorrespondingMesh(int);

  void AddContourForMeshToContours(vtkPolyData *);

  void Sampling(int);

protected:
  QGoSeedBaseWidget *m_BaseAlgorithmSegmentationWidget;
private:
  /**
   *
   */
  double     m_Radius;
  vtkPoints *m_Seeds;
  double     m_SeedsPosition[3];
  int        m_Sampling;
  int        m_NumberOfChannels;
};
#endif
