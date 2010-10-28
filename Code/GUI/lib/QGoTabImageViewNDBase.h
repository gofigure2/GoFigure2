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

#ifndef __QGoTabImageViewNDBase_h
#define __QGoTabImageViewNDBase_h

#include "QGoTabImageViewElementBase.h"

#include "QGoGUILibConfigure.h"

class vtkImageData;

/** \class QGoTabImageViewNDBase
 *  \brief Abstract class for representing one tab element which
 *  contains 2D or 3D image (without any temporal component).
 */
class QGOGUILIB_EXPORT QGoTabImageViewNDBase:public QGoTabImageViewElementBase
{
  Q_OBJECT
public:
  /** \brief Constructor */
  explicit QGoTabImageViewNDBase(QWidget *parent = 0);

  /** \brief Destructor */
  virtual ~QGoTabImageViewNDBase();

  typedef QGoTabImageViewElementBase::QGoDockWidgetStatusPair QGoDockWidgetStatusPair;

  /** \brief */
  virtual void SetImage(vtkImageData *iImage);

  /** \brief */
  vtkImageData * GetImage();

public slots:
  /** \brief */
  void ShowAllChannels(bool iChecked);

  /** \brief */
  void ShowOneChannel(int iChannel);

protected:
  vtkSmartPointer< vtkImageData > m_Image;

  /** \brief */
  virtual void SetImageToImageViewer(vtkImageData *image) = 0;

private:
  Q_DISABLE_COPY(QGoTabImageViewNDBase);
};
#endif
