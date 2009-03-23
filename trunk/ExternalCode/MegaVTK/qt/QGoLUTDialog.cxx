/*========================================================================
 Copyright (c) INRIA - ASCLEPIOS Project (http://www-sop.inria.fr/asclepios).
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of INRIA or ASCLEPIOS, nor the names of any contributors
 may be used to endorse or promote products derived from this software 
 without specific prior written permission.
 
 * Modified source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 =========================================================================*/

/*=========================================================================
 Modifications were made by 
 A. Gouaillard and A. Gelas 
 while at Megason Lab, Systems biology, Harvard Medical school, 2009 
 
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

#include "QGoLUTDialog.h"

#include <vtkLookupTableManager.h>
#include <vtkTextProperty.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>

QGoLUTDialog::QGoLUTDialog( QWidget* parent ) : QDialog( parent ), LUT( 0 )
{
  setupUi( this );

  this->LUT = vtkLookupTableManager::GetBWLookupTable();
  this->Renderer = vtkRenderer::New();

  vtkRenderWindow* renwin = this->qvtkWidget->GetRenderWindow();
  renwin->AddRenderer( this->Renderer );

  this->LUTActor = vtkScalarBarActor::New();
  this->LUTActor->SetLookupTable( this->LUT );
  this->LUTActor->SetOrientationToHorizontal();
  this->LUTActor->SetWidth( 0.8 );
  this->LUTActor->SetHeight( 0.9 );
  this->LUTActor->SetPosition( 0.1, 0.1 );
  this->LUTActor->SetTitle( "LUT" );

  this->Renderer->AddActor2D( this->LUTActor );
  
  renwin->Render();
}

QGoLUTDialog::~QGoLUTDialog()
{
  this->Renderer->Delete();
  this->LUTActor->Delete();
  this->LUT->Delete();
  delete this->qvtkWidget;
}

vtkLookupTable* QGoLUTDialog::GetLookupTable( )
{
  return this->LUT;
}

void QGoLUTDialog::setupUi( QDialog* LUTDialog )
{
  if( LUTDialog->objectName().isEmpty() )
    LUTDialog->setObjectName( QString::fromUtf8("LUTDialog") );

  LUTDialog->resize(321, 183);
  LUTDialog->setMinimumSize( 200, 150 );
  LUTDialog->setModal( true );

  this->verticalLayout = new QVBoxLayout( LUTDialog );
  this->verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
  this->verticalLayout->setContentsMargins(10, 10, 10, 10);
  this->verticalLayout->setSpacing( 20 );

  this->horizontalLayout = new QHBoxLayout;
  this->horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
  this->horizontalLayout->setContentsMargins(0, 0, 0, 0);

  this->label = new QLabel;
  this->label->setObjectName(QString::fromUtf8("label"));
  this->label->setText( tr("Lookup Table") );

  this->horizontalLayout->addWidget(label);

  this->LUTComboBox = new QComboBox;
  this->LUTComboBox->setObjectName(QString::fromUtf8("LUTComboBox"));
  this->LUTComboBox->setLayoutDirection(Qt::RightToLeft);
  this->LUTComboBox->setAutoFillBackground(false);
  this->LUTComboBox->setEditable(false);
  this->LUTComboBox->setFrame(true);

  int k = 0;
  this->LUTComboBox->insertItem( k++, tr( "B/W" ) );
  this->LUTComboBox->insertItem( k++, tr( "B/W Inverse" ) );
  this->LUTComboBox->insertItem( k++, tr( "Spectrum" ) );
  this->LUTComboBox->insertItem( k++, tr( "Hot Metal" ) );
  this->LUTComboBox->insertItem( k++, tr( "GE Color" ) );
  this->LUTComboBox->insertItem( k++, tr( "Flow" ) );
  this->LUTComboBox->insertItem( k++, tr( "LONI" ) );
  this->LUTComboBox->insertItem( k++, tr( "LONI2" ) );
  this->LUTComboBox->insertItem( k++, tr( "Asymmetry" ) );
  this->LUTComboBox->insertItem( k++, tr( "P-Value" ) );

  this->horizontalLayout->addWidget(this->LUTComboBox);

  this->verticalLayout->addLayout( this->horizontalLayout );

  this->qvtkWidget = new QVTKWidget;
  this->qvtkWidget->setObjectName(QString::fromUtf8("qvtkWidget"));
  this->qvtkWidget->setGeometry(QRect(10, 50, 301, 100));

  this->verticalLayout->addWidget( this->qvtkWidget );

  this->horizontalLayout_2 = new QHBoxLayout;
  this->horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
  this->horizontalLayout_2->setContentsMargins(0, 0, 0, 0);

  this->horizontalSpacer = new QSpacerItem(166, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
  this->horizontalLayout_2->addItem(horizontalSpacer);

  this->buttonBox = new QDialogButtonBox;
  this->buttonBox->setObjectName( QString::fromUtf8("buttonBox") );
  this->buttonBox->setGeometry(QRect(60, 160, 252, 32));
  this->buttonBox->setOrientation(Qt::Horizontal);
  this->buttonBox->setStandardButtons( QDialogButtonBox::Cancel |
  QDialogButtonBox::Ok );
  this->buttonBox->setCenterButtons(false);
  this->horizontalLayout_2->addWidget(buttonBox);

  this->verticalLayout->addLayout( this->horizontalLayout_2 );

  LUTDialog->setLayout( this->verticalLayout );

  QObject::connect( this->buttonBox, SIGNAL(accepted()),
                    LUTDialog, SLOT(accept()) );
  QObject::connect( this->buttonBox, SIGNAL(rejected()),
                    LUTDialog, SLOT(reject()) );
  QObject::connect( this->LUTComboBox, SIGNAL( currentIndexChanged( int ) ),
                    this, SLOT( ChangeLookupTable( int ) ) );

  LUTComboBox->setCurrentIndex( 0 );

  QMetaObject::connectSlotsByName(LUTDialog);
}

void QGoLUTDialog::ChangeLookupTable( const int& idx )
{
  this->LUT->Delete();
  this->LUT = vtkLookupTableManager::GetLookupTable( idx );
  this->LUTActor->SetLookupTable( this->LUT );

  this->qvtkWidget->GetRenderWindow()->Render( ) ;
}
