/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
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

#ifndef __QGoLsmToMegaExportDialog_h
#define __QGoLsmToMegaExportDialog_h

#include "ui_LsmToMegaExportDialog.h"

#include <Qt>
#include <QProgressDialog>
#include "ConversionLsmToMegaThread.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "QGoGUILibConfigure.h"

class QGOGUILIB_EXPORT QGoLsmToMegaExportDialog:
  public QDialog,
  private Ui::LsmToMegaExportDialog
{
  Q_OBJECT
public:
  /**
   * \brief Constructor
   */
  explicit QGoLsmToMegaExportDialog(QWidget *parent = 0);
  /**
   * \brief Desctructor
   */
  ~QGoLsmToMegaExportDialog();

  ConversionLsmToMegaThread *ConversionLsmToMegaThreadSend;
protected slots:
  /**
   * \brief Get the name of the lsm file selected
   */
  void on_selectLsmFile_clicked();

  /**
   * \brief Get the location to store MegaCapture file
   */
  void on_selectMegaPath_clicked();

  /**
   * \brief Select the output format
   * \param[in] index 0: PNG, 1:TIFF
   */
  void on_outputFormat_activated(int index);

  /**
   * \brief Disable buttons and send thread to launch conversion
   */
  void on_convert_clicked();

  /**
   * \brief Catch thread, close progress bar and dialog box
   */
  void ConversionTerminatedReceived();

  /**
   * \brief Catch thread, initialize the progress bar
   */
  void InitialisationProgressReceived();

  /**
   * \brief Catch thread, update the progress bar content
   */
  void ProgressReceived();

  /**
   * \brief Catch thread, cancel the conversion
   */
  void CanceledReceived();

private:
  QString          m_LsmPath;
  QString          m_LsmName;
  QString          m_MegaPath;
  bool             m_FileFormatIsPNG;
  QProgressDialog *m_ProgressDialog;
  int              m_Counter;
};
#endif
