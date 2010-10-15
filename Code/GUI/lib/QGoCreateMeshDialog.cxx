/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
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
#include "QGoCreateMeshDialog.h"

QGoCreateMeshDialog::QGoCreateMeshDialog(QWidget *iParent):
  QDialog(iParent)
{
  /*this->VLayoutLabel       = new QVBoxLayout;
  this->VLayoutCombobox    = new QVBoxLayout;
  this->VLayoutDescription = new QVBoxLayout;
  this->HorizontalLayout   = new QHBoxLayout;*/
  //this->HorizontalSpacer   = new QSpacerItem;
  this->GridLayout         = new QGridLayout;
  this->LineCellType       = new QLabel( tr("Select the Cell Type:") );
  this->LineSubCellType    = new QLabel( tr("Select the SubCellular type:") );
  this->LineColor          = new QLabel( tr("Select the color") );
  this->SelectCellType     = new QComboBox;
  this->SelectSubCellType  = new QComboBox;
  this->SelectColor        = new QComboBox;
  this->ScrollDescrCell    = new QScrollArea;
  this->ScrollDescrSubCell = new QScrollArea;
  this->ScrollDescrColor   = new QScrollArea;
  this->DescriptionCell    = new QLineEdit;
  this->DescriptionSubCell = new QLineEdit;
  this->DescriptionColor   = new QLineEdit;

  this->ScrollDescrCell->setWidget(this->DescriptionCell);
  this->ScrollDescrSubCell->setWidget(this->DescriptionSubCell);
  this->ScrollDescrColor->setWidget(this->DescriptionColor);
  //this->DescriptionCell    = new QTextEdit;
  //this->DescriptionSubCell = new QTextEdit;
  //this->DescriptionColor   = new QTextEdit;
  this->ButtonBox          = new QDialogButtonBox;
  this->OkButton           = new QPushButton( tr("OK") );
  this->CancelButton       = new QPushButton( tr("Cancel") );
  this->NewCellType        = new QPushButton( tr("New One") );
  this->NewSubCellType     = new QPushButton( tr("New One") );
  this->NewColor           = new QPushButton( tr("New One") );
  this->ButtonBox->addButton(OkButton, QDialogButtonBox::AcceptRole);
  this->ButtonBox->addButton(CancelButton, QDialogButtonBox::RejectRole);

  /*this->VLayoutLabel->addWidget(this->LineCellType);
  this->VLayoutLabel->addWidget(this->LineSubCellType);
  this->VLayoutLabel->addWidget(this->LineColor);

  this->VLayoutCombobox->addWidget(this->SelectCellType);
  this->VLayoutCombobox->addWidget(this->SelectSubCellType);
  this->VLayoutCombobox->addWidget(this->SelectColor);

  this->VLayoutDescription->addWidget(this->DescriptionCell);
  this->VLayoutDescription->addWidget(this->DescriptionSubCell);
  this->VLayoutDescription->addWidget(this->DescriptionColor);

  this->HorizontalLayout->addLayout(this->VLayoutLabel);
  this->HorizontalLayout->addLayout(this->VLayoutCombobox);
  this->HorizontalLayout->addLayout(this->VLayoutDescription);
  this->HorizontalLayout->addWidget(this->ButtonBox);

  this->setLayout(this->HorizontalLayout);*/
  this->GridLayout->addWidget(this->LineCellType, 1, 1);
  this->GridLayout->addWidget(this->LineSubCellType, 2, 1);
  this->GridLayout->addWidget(this->LineColor, 3, 1);

  this->GridLayout->addWidget(this->SelectCellType, 1, 2);
  this->GridLayout->addWidget(this->SelectSubCellType, 2, 2);
  this->GridLayout->addWidget(this->SelectColor, 3, 2);

  /*this->GridLayout->addWidget(this->DescriptionCell,1,3);
  this->GridLayout->addWidget(this->DescriptionSubCell,2,3);
  this->GridLayout->addWidget(this->DescriptionColor,3,3);*/

  //this->GridLayout->addWidget(this->ScrollDescrCell,1,3);
  //this->GridLayout->addWidget(this->ScrollDescrSubCell,2,3);
  //this->GridLayout->addWidget(this->ScrollDescrColor,3,3);

  this->GridLayout->addWidget(this->NewCellType, 1, 4);
  this->GridLayout->addWidget(this->NewSubCellType, 2, 4);
  this->GridLayout->addWidget(this->NewColor, 3, 4);

  this->GridLayout->addWidget(this->ButtonBox, 4, 1);

  this->setLayout(this->GridLayout);
}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoCreateMeshDialog::~QGoCreateMeshDialog()
{}

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
