#ifndef __QGoLsmToMegaExportDialog_h
#define __QGoLsmToMegaExportDialog_h

#include "ui_LsmToMegaExportDialog.h"

#include <Qt>

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
  QGoLsmToMegaExportDialog( QWidget* parent = 0);
  ~QGoLsmToMegaExportDialog();

  ConversionLsmToMegaThread ConversionLsmToMegaThreadSend;

protected slots:
  void on_selectLsmFile_clicked();
  void on_selectMegaPath_clicked();
  void on_outputFormat_activated( int index );
  void on_convert_clicked();
  void ConversionTerminatedReceived();

private:
  std::string m_LsmPath;
  std::string m_LsmName;
  std::string m_MegaPath;
  bool        m_FileFormatIsPNG;

};
#endif
