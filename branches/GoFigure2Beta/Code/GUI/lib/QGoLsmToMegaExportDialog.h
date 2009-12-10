#ifndef __QGoLsmToMegaExportDialog_h
#define __QGoLsmToMegaExportDialog_h

#include "ui_LsmToMegaExportDialog.h"

#include <Qt>

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

public slots:
  void on_selectLsmFile_clicked();
  void on_selectMegaPath_clicked();

private:
  std::string m_LsmPath;
  std::string m_LsmName;
  std::string m_MegaPath;

};
#endif
