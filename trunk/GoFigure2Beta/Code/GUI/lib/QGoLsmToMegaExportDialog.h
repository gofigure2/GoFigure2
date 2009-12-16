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
