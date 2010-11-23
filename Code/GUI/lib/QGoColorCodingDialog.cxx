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


QGoColorCodingDialog::
QGoColorCodingDialog( std::string iTraceName,
                     bool iRandomIncluded,
                     QWidget *iParent) : QDialog(iParent)
{

  this->SetUpUi(iTraceName,iRandomIncluded);
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
 this->m_LUT = QGoLUTDialog::GetLookupTable(this,
                                         tr("Choose your range of colors"));
 if (this->m_LUT != NULL)
  {
  this->accept();
  }
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoColorCodingDialog::SetUpUi(std::string iTraceName,
  bool iRandomIncluded)
{
  QLabel* ChooseYourWay = new QLabel(tr("Choose how you want to color your %1s :").arg(iTraceName.c_str()) );
  m_DefaultButton = new QRadioButton(tr("Default"),this);
  m_DefaultButton->setChecked(true);
  m_LUTButton = new QRadioButton(tr("Choose a range of colors"),this);

  QVBoxLayout* VLayout = new QVBoxLayout(this);
  VLayout->addWidget(ChooseYourWay);
  VLayout->addWidget(m_DefaultButton);
  VLayout->addWidget(m_LUTButton);

  if (iRandomIncluded)
    {
    m_RandomButton = new QRadioButton(tr("Randomly"),this);
    VLayout->addWidget(m_RandomButton);
    }

  QDialogButtonBox* ButtonBox = new QDialogButtonBox(this);
  ButtonBox->setStandardButtons(QDialogButtonBox::Cancel
                                      | QDialogButtonBox::Ok);
  VLayout->addWidget(ButtonBox);

  QObject::connect(this->m_LUTButton,SIGNAL(clicked()),this,SLOT(OpenLUTDialog()));
  QObject::connect(ButtonBox, SIGNAL( accepted() ),
                    this, SLOT( accept() ) );
  QObject::connect( ButtonBox, SIGNAL( rejected() ),
                    this, SLOT( reject() ) );
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoColorCodingDialog::ColorWay
QGoColorCodingDialog::GetColorWay(
    std::string iTraceName, vtkLookupTable **ioLUT,
    bool iRandomIncluded, QWidget *iiParent)
{
  QGoColorCodingDialog* ColorDialog =
      new QGoColorCodingDialog(iTraceName, iRandomIncluded, iiParent);

  ColorWay oNameWay = QGoColorCodingDialog::Nothing;

  if( ColorDialog->exec() == QDialog::Accepted )
    {
    if( ColorDialog->m_DefaultButton->isChecked() )
      {
      oNameWay = QGoColorCodingDialog::Default;
      }
    if (iRandomIncluded)
      {
      if( ColorDialog->m_RandomButton->isChecked() )
        {
        oNameWay = QGoColorCodingDialog::Random;
        }
      }
    if( ColorDialog->m_LUTButton->isChecked() )
      {
      oNameWay = QGoColorCodingDialog::LUT;
      *ioLUT = ColorDialog->m_LUT;
      ColorDialog->accept();
      }
    }

  return oNameWay;
}
