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

#ifndef __QGoLsmToMegaExportDialog_h
#define __QGoLsmToMegaExportDialog_h

#include "ui_LsmToMegaExportDialog.h"

#include <Qt>
#include <QProgressDialog>
#include "ConversionLsmToMegaThread.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

class QGoLsmToMegaExportDialog :
  public QDialog,
  private Ui::LsmToMegaExportDialog
{
  Q_OBJECT
public:
  explicit QGoLsmToMegaExportDialog( QWidget* parent = 0);
  ~QGoLsmToMegaExportDialog();

  ConversionLsmToMegaThread ConversionLsmToMegaThreadSend;

protected slots:
  void on_selectLsmFile_clicked();
  void on_selectMegaPath_clicked();
  void on_outputFormat_activated( int index );
  void on_convert_clicked();
  void ConversionTerminatedReceived();
  void InitialisationProgressReceived();
  void ProgressReceived();
  void CanceledReceived();

private:
  QString          m_LsmPath;
  QString          m_LsmName;
  QString          m_MegaPath;
  bool             m_FileFormatIsPNG;
  QProgressDialog* m_ProgressDialog;
  int              m_Counter;

};
#endif
