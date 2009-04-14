/*=========================================================================
 Authors: The GoFigure Dev. Team.
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
#include "MegaVTK2Configure.h"

/**
  \class QGoLUTDialog
  \brief Look-up Table Dialog
*/
class QGoLUTDialog : public QDialog
{
  Q_OBJECT
public:
  explicit QMEGAVTKADDON2_EXPORT QGoLUTDialog( QWidget* parent = 0 );
  virtual QMEGAVTKADDON2_EXPORT ~QGoLUTDialog();

  QMEGAVTKADDON2_EXPORT vtkLookupTable* GetLookupTable( );

public slots:
  QMEGAVTKADDON2_EXPORT void ChangeLookupTable( const int& );

protected:
  vtkLookupTable* LUT;
  vtkScalarBarActor* LUTActor;
  vtkRenderer*  Renderer;
  QDialogButtonBox* buttonBox;
  QVTKWidget* qvtkWidget;
  QHBoxLayout* horizontalLayout;
  QHBoxLayout* horizontalLayout_2;
  QVBoxLayout* verticalLayout;
  QSpacerItem* horizontalSpacer;
  QLabel* label;
  QComboBox* LUTComboBox;

  void setupUi( QDialog *LUTDialog );
};
#endif
