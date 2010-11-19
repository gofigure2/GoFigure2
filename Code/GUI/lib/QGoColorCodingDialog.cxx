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
#include "QGoColorCodingDialog.h"
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>


QGoColorCodingDialog::QGoColorCodingDialog(std::string iTraceName,QWidget *iParent):
  QDialog(iParent)
{
  QLabel* ChooseYourWay = new QLabel(tr("Choose how you want to color your %1s").arg(iTraceName.c_str()) );
  QRadioButton* DefaultButton = new QRadioButton(tr("Default"),this);
  QRadioButton* LUTButton = new QRadioButton(tr("Choose a range of colors"),this);
  QRadioButton* RandomButton = new QRadioButton(tr("Randomly"),this);

  QVBoxLayout* VLayout = new QVBoxLayout(this);
  VLayout->addWidget(ChooseYourWay);
  VLayout->addWidget(DefaultButton);
  VLayout->addWidget(LUTButton);
  VLayout->addWidget(RandomButton);

  QObject::connect(LUTButton,SIGNAL(clicked()),this,SLOT(OpenLUTDialog()));

  this->setLayout(VLayout);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoColorCodingDialog::~QGoColorCodingDialog()
{
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoColorCodingDialog::OpenLUTDialog()
{
}