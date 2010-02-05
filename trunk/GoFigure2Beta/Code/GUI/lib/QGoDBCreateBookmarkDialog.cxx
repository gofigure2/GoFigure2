/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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

#include "QGoDBCreateBookmarkDialog.h"
#include "GoDBBookmarkRow.h"
#include <QMessageBox>
#include <QDateTime>
#include "QueryDataBaseHelper.h"

QGoDBCreateBookmarkDialog::QGoDBCreateBookmarkDialog(QWidget* iParent,
  vtkMySQLDatabase* iDatabaseConnector, int iImgSessionID,int iCoordID):
  QDialog( iParent)
{
  this->setupUi( this);
  this->m_DatabaseConnector = iDatabaseConnector;
  this->m_ImgSessionID = iImgSessionID;
  this->m_CoordID = iCoordID;
  QObject::connect(this,SIGNAL(accepted()),this,SLOT(validate()));
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoDBCreateBookmarkDialog::~QGoDBCreateBookmarkDialog()
{
  this->m_DatabaseConnector->Delete();
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBCreateBookmarkDialog::validate()
{
  if (this->NameLineEdit->text().toStdString().empty())
    {
    QMessageBox msgBox;
    msgBox.setText(
      tr("Please enter the name for the bookmark to add"));
    msgBox.exec();
    this->open();
    }
  else
    {
    SaveNewBookmarkInDB();
    }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoDBCreateBookmarkDialog::SaveNewBookmarkInDB()
{
  GoDBBookmarkRow NewBookmark;
  NewBookmark.SetField("Name",this->NameLineEdit->text().toStdString());
  /** \todo set a restriction for the number of characters: at that
  time only 45 allowed in the database*/
  NewBookmark.SetField("Description",
    this->DescriptionLineEdit->toPlainText().toStdString());
  QDateTime CreationDate = QDateTime::currentDateTime();
  std::string CreationDateStr = 
    CreationDate.toString(Qt::ISODate).toStdString();
  NewBookmark.SetField("CreationDate",CreationDateStr);
  /** \todo Get the coordid from the visu*/
  NewBookmark.SetField<int>("CoordID",this->m_CoordID);
  NewBookmark.SetField<int>("ImagingSessionID",this->m_ImgSessionID);
  NewBookmark.SaveInDB(this->m_DatabaseConnector);
}
