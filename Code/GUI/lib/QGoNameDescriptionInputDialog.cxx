/*=========================================================================
  Author: $Author: nicolasrannou $  // Author of last commit
  Version: $Rev: 1803 $  // Revision of last commit
  Date: $Date: 2010-07-15 13:48:08 -0400 (Thu, 15 Jul 2010) $  // Date of last commit
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
#include "QGoNameDescriptionInputDialog.h"
#include <QLabel>
#include <QMessageBox>
#include "QTextEditChild.h"

QGoNameDescriptionInputDialog::QGoNameDescriptionInputDialog(QWidget *iParent,
                                                             QString iEntityName):QDialog(iParent)
{
  this->setupUi(this);
  this->m_EntityName = iEntityName;
  this->setWindowTitle( tr("Create a %1").arg(this->m_EntityName) );
  this->EntityLabel->setText( tr("new %1 you want to save:").arg(iEntityName) );
  this->NameLineEdit->setMaxLength(45);
  this->m_DescriptionTextEdit = new QTextEditChild(this, 1000);
  this->formLayout->addRow("Description:", this->m_DescriptionTextEdit);

  QObject::connect( this->NameDescriptionButtonBox, SIGNAL( accepted() ),
                    this, SLOT( ValidationRequested() ) );
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoNameDescriptionInputDialog::~QGoNameDescriptionInputDialog()
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string QGoNameDescriptionInputDialog::GetInputTextForName()
{
  return this->NameLineEdit->text().toStdString();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
std::string QGoNameDescriptionInputDialog::GetInputTextForDescription()
{
  return this->m_DescriptionTextEdit->toPlainText().toStdString();
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoNameDescriptionInputDialog::ValidationRequested()
{
  if ( this->GetInputTextForName().empty() )
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please enter the name for the %1 to add").arg(this->m_EntityName) );
    msgBox.exec();
    }
  else
    {
    emit NewNameDescription(
      this->GetInputTextForName(), this->GetInputTextForDescription() );
    }
}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoNameDescriptionInputDialog::NameAlreadyExists()
{
  QMessageBox msgBox;

  msgBox.setText(
    tr("This name already exists, please choose another one") );
  msgBox.exec();
}
