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

#ifndef __QGoAboutWidget_h
#define __QGoAboutWidget_h

#include <QWidget>
#include <QTabWidget>
#include <QLabel>
#include <QLayout>

#include "QGoGUILibConfigure.h"

/**
 * \class QGoAboutWidget
 * \brief About Widget which includes the list of authors, licenses,
 * copyrights, dates, versions...
 */
class QGOGUILIB_EXPORT QGoAboutWidget:public QWidget
{
  Q_OBJECT
public:
  /** \brief Constructor */
  explicit QGoAboutWidget(QWidget *iParent = NULL);

  /** \brief Destructor */
  ~QGoAboutWidget();
protected:
  /** \brief Add one tab (for the license) in the widget.
  * \param[in] iTextLicense License / Copyright
  * \param[in] iTabTitle Title of the tab (Library Name)
  * \param[in] iTabWidget TabWidget to add this element to.
  */
  void AddTabAbout(QString iTextLicense, QString iTabTitle,
                   QTabWidget *iTabWidget);

  /** \brief Get the list of contributors. */
  QString GetContributors();

  /** \brief */
  void SetTabWidget(QTabWidget *iTabWidget);

  /** \brief */
  void SetTheBackGround(QLabel *iLabel, QTabWidget *TabWidget,
                        QLabel *iLabeltwo);

  /** \brief Read License file.
 * \param[in] iFilename input license filename
 * \return Contents of the license file.
 * */
  QString ReadLicenseText(QString iFilename);

  /**
 * \brief
 */
  void WriteLicenseText(QTabWidget *iTabWidget);

private:
  Q_DISABLE_COPY(QGoAboutWidget);
};
#endif
