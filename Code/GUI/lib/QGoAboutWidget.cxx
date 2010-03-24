/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Revision: 503 $  // Revision of last commit
  Date: $Date: 2009-08-03 17:42:52 -0400 (Mon, 03 Aug 2009) $  // Date of last commit
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

#include "QGoAboutWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>

QGoAboutWidget::QGoAboutWidget()
{
  QString version( "0.4" );
  QString date( "Date: 02/17/2010\n\n" );

  QString about_gofigure(
    tr( "GoFigure2: Software for the visualization and the analysis of biological microscope images. \n\n" ) );

//   QString authors( tr( "Authors in alphabetical order:\n" ) );
//   authors.append( " * 2008 ~ A. Gelas\n" );
//   authors.append( " * 2007-2009 ~ A. Gouaillard\n" );
//   authors.append( " * 2008 ~ K. Mosaliganti\n" );
//   authors.append( " * 2008 ~ L. Souhait\n\n" );
//   authors.append( " * 2009 ~ N. Rannou\n\n" );
//   authors.append( " * Principal Investigator\n" );
//   authors.append( " * S. Megason\n" );

  QString message = QString( "GoFigure2-%1\n\n" ).arg( version );
  message.append( date );
  message.append( about_gofigure );
//   message.append( authors );
  //QWidget* About = new QWidget;
  QVBoxLayout* vlayout = new QVBoxLayout;
  QLabel* GofigureLabel = new QLabel(message,this);
  QTabWidget* TabWidget = new QTabWidget(this);
  QString AboutQtLicense(tr("to be filled..."));
  QString TabTitle(tr("About Icons"));
  this->AddTabAbout(AboutQtLicense,TabTitle,TabWidget);
  vlayout->addWidget(GofigureLabel);
  vlayout->addWidget(TabWidget);
  this->setLayout(vlayout);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
QGoAboutWidget::~QGoAboutWidget()
{
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoAboutWidget::AddTabAbout(QString iTextLicense, QString iTabTitle,
                                QTabWidget* iTabWidget)
{
  QLabel* TextLicense = new QLabel(iTextLicense);
  QScrollArea* scrollArea = new QScrollArea;
  //scrollArea->setBackgroundRole(QPalette::Dark);
  scrollArea->setWidget(TextLicense);
  iTabWidget->addTab(scrollArea,iTabTitle);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------