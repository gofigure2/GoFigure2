/*=========================================================================
  Author: $Author: lydiesouhait $  // Author of last commit
  Version: $Rev: 1873 $  // Revision of last commit
  Date: $Date: 2010-07-29 13:12:06 -0400 (Thu, 29 Jul 2010) $  // Date of last commit
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
#include "QGoSelectedColorComboBox.h"
#include <iostream>
#include <QColorDialog>

QGoSelectedColorComboBox::QGoSelectedColorComboBox(QWidget *parent)
:QGoColorComboBox("Add a new color...",parent,"Delete colors...")
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoSelectedColorComboBox::~QGoSelectedColorComboBox()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoSelectedColorComboBox::ActionWhenNewOneRequested()
{
  emit AddNewColorActivated();
  /*QColor col = QColorDialog::getColor(Qt::white, NULL, "Pick a new color",
                                          QColorDialog::ShowAlphaChannel);
      if (col.isValid())
        {
        bool ok = false;
        QString ColorName = QInputDialog::getText(this, tr("New Color Name:"),
                                                  tr("Please enter the name for your new color:"),
                                                  QLineEdit::Normal, "", &ok);
        if (ok && ColorName.isEmpty())
          {
          QMessageBox msgBox;
          msgBox.setText(
          tr("Please enter the name for the Color to add"));
          msgBox.exec();
          }
        if(ok && !ColorName.isEmpty())
          {
          //if the color already exist in combobox, no need to do anything:
          if (!this->NewColorNameAlreadyExist(ColorName))
            {
            ItemColorComboboxData NewItemData;
            NewItemData.first = ColorName.toStdString();
            NewItemData.second = col;
          //pass the data for the new color to be saved in the database
            //this->m_LastActivated = ColorName;
            emit NewColorToBeSaved(NewItemData);           
            emit activated(this->findText(ColorName));
            }
          else
            {
            this->setCurrentIndex(this->findText(this->m_LastActivated));
            }
          }
        this->setCurrentIndex(this->findText(this->m_LastActivated));
        }
      else
        {
        // The user pressed cancel - reset the current color to
        // what it was before the color dialog was shown.
        this->setCurrentIndex(this->findText(this->m_LastActivated));
        }*/
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*bool QGoSelectedColorComboBox::NewColorNameAlreadyExist(QString iText)
{
  if (this->findText(iText) == -1)
  {
    return false;
  }
  return true;
}*/
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
/*void QGoSelectedColorComboBox::SetActivatedItem()
{
  //set the selected item to the last one activated if it still exists, if not, to the one
  // selected automatically by the combobox:
  int index;
  int IndexLastActivated = this->findText(this->m_LastActivated);
  if ( IndexLastActivated != -1)
    {
    index = IndexLastActivated;
    }
  else
    {
    index = this->currentIndex();
    }
  emit ItemSelected(this->GetTheItemColorComboBoxData(index));
}
*/