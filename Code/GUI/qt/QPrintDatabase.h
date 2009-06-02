#ifndef QPRINTDATABASE_H
#define QPRINTDATABASE_H
#include <qwidget.h>

#include "ui_QPrintDatabase.h"
#include "MegaVTK2Configure.h"

class QPrintDatabase : public QWidget,
  private Ui::WidgetPrintDatabase
{
  Q_OBJECT

public:
  explicit QMEGAVTKADDON2_EXPORT QPrintDatabase(QString NameDB);
  virtual QMEGAVTKADDON2_EXPORT ~QPrintDatabase();


protected:
  QStringList GetTableContentFromDB(QString TableName);
  void QPrintColumnNames (QString TableName, QTableWidget* QTabname);
  QString m_NameDB;
};

#endif
