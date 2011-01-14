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

#ifndef __QGoNameDescriptionInputDialog_h
#define __QGoNameDescriptionInputDialog_h

#include <QDialog>
#include "ui_QNameDescriptionInputDialog.h"
#include "QTextEditChild.h"

/**
\class QNameDescriptionInputDialog
\brief for the creation of several entities in the database, the name and the
description are asked to the user.
*/
class QGoNameDescriptionInputDialog:public QDialog,
  private Ui::QNameDescriptionInputDialog
{
  Q_OBJECT
public:

  explicit QGoNameDescriptionInputDialog(QWidget *iParent = 0,
                                         QString iEntityName = "");

  virtual ~QGoNameDescriptionInputDialog();

  /** \brief open a messagebox to tell the user that the name he
  choose already exists*/
  void NameAlreadyExists();

signals:

  void NewNameDescription(std::string, std::string);

  //void CancelRequested();
protected:
  QTextEditChild *m_DescriptionTextEdit;
  QString         m_EntityName;

  std::string GetInputTextForName();

  std::string GetInputTextForDescription();

protected slots:
  /** \brief check that the qlineEdit for the name is not
  empty, if so, tell the user he needs to enter a name*/
  void ValidationRequested();
};

#endif
