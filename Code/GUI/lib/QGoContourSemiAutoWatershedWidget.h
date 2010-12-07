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

#ifndef __QGoContourSemiAutoWatershedWidget_h
#define __QGoContourSemiAutoWatershedWidget_h

#include <QWidget>
#include "ui_WatershedWidget.h"

/**
 * \class QGoContourSemiAutoWatershedWidget
 * \ingroup QGoContourSeed
 * \brief Widget suited for the watershed segmentation
*/

class QGoContourSemiAutoWatershedWidget:
  public QWidget,
  protected Ui::WatershedWidget
{
  Q_OBJECT
public:
  explicit QGoContourSemiAutoWatershedWidget(QWidget *iParent = 0);
  ~QGoContourSemiAutoWatershedWidget();

  signals:
    /*
     * \brief Signal sent when the treshmin is modified
     */
    void TreshMin(int);

    /*
     * \brief Signal sent when the treshmax is modified
     */
    void TreshMax(int);

    /*
     * \brief Signal sent when the corTresh is modified
     */
    void CorrTresh(double);

    /*
     * \brief Signal sent when the Alpha is modified
     */
    void Alpha(double);

    /*
     * \brief Signal sent when the Beta is modified
     */
    void Beta(double);
};

#endif
