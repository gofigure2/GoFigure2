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
#ifndef __QGoOpenCreateImgSessionPage_h
#define __QGoOpenCreateImgSessionPage_h

#include <QWizardPage>
#include <QLineEdit>
#include <QLabel>
#include <QTextEdit>
#include <QRadioButton>
#include <QComboBox>
#include <QStringList>
#include <QString>
#include <string>
#include "vtkMySQLDatabase.h"


class QGoOpenCreateImgSessionPage : public QWizardPage
{
  Q_OBJECT

public:

  explicit QGoOpenCreateImgSessionPage(QWidget *parent = 0);
  ~QGoOpenCreateImgSessionPage();
  void initializePage();
  bool validatePage();
  int nextId() const;
  void cleanupPage();
  mutable vtkMySQLDatabase* m_DatabaseConnector;

private:

  /**
   * \brief fill the m_MapImgSessionIDName with the names of the existing imaging
   * session stored in the database and their corresponding ID, fill the m_ListImgSession
   * with the names only and return true if this list is not empty.
   */
  bool GetListImgSession();

  void OpenDBConnection()const;

  QLabel*       textDescription;
  QTextEdit*    lineDescription;
  QLabel*       textChoiceImgSession;
  QComboBox*    ChoiceImgSession;
  QString       OpenOrCreateImgSession;
  QRadioButton* OpenImgSessionRadioButton;
  QRadioButton* CreateImgSessionRadioButton;
  QLineEdit*    lineImgSessionID;
  QLineEdit*    lineImgSessionName;
  mutable bool  LeavingPage;

  QStringList m_ListImgSession;
  std::map<std::string,std::string> m_MapImgSessionIDName;

protected slots:
  /**
    * \brief display in the description text the existing description stored
    * in the database for a given Imaging Session.
    */
  void DisplayInfoImgSession(QString ImgSessionName);
  void ChangeToCreateImgSessionDisplay();
  void ChangeToOpenImgSessionDisplay();


};
#endif