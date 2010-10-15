/*=========================================================================
  Author: $Author: nicolasrannou $  // Author of last commit
  Version: $Rev: 2037 $  // Revision of last commit
  Date: $Date: 2010-08-23 16:33:20 -0400 (Mon, 23 Aug 2010) $  // Date of last commit
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
#ifndef __QGoContourManualSegmentation_h
#define __QGoContourManualSegmentation_h

#include <QObject>
#include <QWidget>
#include "QGoGUILibConfigure.h"

class QGoContourManualSegmentationWidget;

/**
 * \class QGoManualSegmentation
 * \ingroup QGoManual
 * \brief Base class for manual segmentations
*/

class QGOGUILIB_EXPORT QGoContourManualSegmentation:public QObject
{
  Q_OBJECT
public:
  /**
   * \brief Constructor
   */
  explicit QGoContourManualSegmentation(QWidget *parent = 0);

  /**
   * \brief Destructor
   */
  ~QGoContourManualSegmentation();

  QWidget * getWidget();

  void SetReeditMode(bool iReeditMode);

  bool GetReeditMode();

public slots:
  void GenerateContourRepresentationProperties(bool iModified = false);

signals:
  void ContourRepresentationPropertiesChanged();

  void validateContour();

  void reinitializeContour();

  void changeContourRepresentationProperty(float iLinewidth, QColor iLinecolor,
                                           QColor iNodecolor, QColor iActivenodecolor);

protected:
  QGoContourManualSegmentationWidget *m_ContourSegmentationWidget;

  double m_LinesWidth;
  QColor m_LinesColor;
  QColor m_NodesColor;
  QColor m_ActiveNodesColor;

  bool m_ReeditMode;
private:
  Q_DISABLE_COPY(QGoContourManualSegmentation);
};
#endif
