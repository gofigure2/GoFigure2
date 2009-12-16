/*=========================================================================
  Author: $Author: arnaudgelas $  // Author of last commit
  Version: $Revision: 193 $  // Revision of last commit
  Date: $Date: 2009-05-19 11:46:16 -0400 (Tue, 19 May 2009) $  // Date of last commit
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

#ifndef __QGoLUTDialog_h
#define __QGoLUTDialog_h

#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QWidget>
#include <QtGui/QSpacerItem>

#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>

#include "QVTKWidget.h"

/**
\class QGoLUTDialog
\brief Look-up Table Dialog
\example GUI/lib/qgolutdialog.cxx
*/
class QGoLUTDialog : public QDialog
{
  Q_OBJECT
public:
  explicit QGoLUTDialog( QWidget* parent = 0 );
  virtual ~QGoLUTDialog();

  vtkLookupTable* GetLookupTable( );

  static vtkLookupTable* GetLookupTable( QWidget* iParent,
    const QString& iTitle );

public slots:
  void ChangeLookupTable( const int& );

protected:
  vtkLookupTable*    LUT;
  vtkScalarBarActor* LUTActor;
  vtkRenderer*       Renderer;
  QDialogButtonBox*  ButtonBox;
  QVTKWidget*        QvtkWidget;
  QHBoxLayout*       HorizontalLayout;
  QHBoxLayout*       HorizontalLayout_2;
  QVBoxLayout*       VerticalLayout;
  QSpacerItem*       HorizontalSpacer;
  QLabel*            Label;
  QComboBox*         LUTComboBox;

  void setupUi( QDialog *LUTDialog );
};
#endif
