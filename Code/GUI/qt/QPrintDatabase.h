#ifndef QPRINTDATABASE_H
#define QPRINTDATABASE_H
#include <qwidget.h>

#include "ui_QPrintDatabase.h"

class QPrintDatabase : public QWidget,
  private Ui::WidgetPrintDatabase
{
  Q_OBJECT

public:
  QPrintDatabase(QString NameDB);
  virtual ~QPrintDatabase();


protected:
  QStringList GetTableContentFromDB(QString TableName);
  void QPrintColumnNames (QString TableName, QTableWidget* QTabname);
  QString m_NameDB;
};

#endif
