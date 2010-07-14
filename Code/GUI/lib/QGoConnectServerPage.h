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
#ifndef __QGoConnectServerPage_h
#define __QGoConnectServerPage_h

#include <QWizard>
#include <QWizardPage>
#include <QLineEdit>
#include "vtkMySQLDatabase.h"

/**
 * \class QGoConnectServerPage
 * \brief
 * \image html QGoConnectServerPage.png
 * */
class QGoConnectServerPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit QGoConnectServerPage(QWidget *parent = 0);
  ~QGoConnectServerPage();

  bool validatePage();
  int nextId() const;
  mutable std::pair<bool,vtkMySQLDatabase*> m_ConnectionServer;
  void SetImgSessionName(std::string iImgSessionName);
  void SetIsAnOpenRecentFile(bool iIsAnOpenRecentFile);

private:
  Q_DISABLE_COPY( QGoConnectServerPage );

  void OpenConnectionToServer()const;
  void CloseServerConnection()const;

  QLineEdit*          lineServerName;
  QLineEdit*          lineUserName;
  QLineEdit*          linePassword;
  QLineEdit*          lineDBName;
  mutable std::string m_ImgSessionName;
  bool                m_IsAnOpenRecentFile;
 
  /** 
 * \brief return the list of the names of the databases of 
 * gofigure type
 * */
  std::list<std::string> ListGofigureDatabases() const;

signals:
  void NoGofigureDatabase()const;

};
#endif
