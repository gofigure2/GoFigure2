#ifndef QGOPRINTDATABASE_H
#define QGOPRINTDATABASE_H
#include <qwidget.h>

#include "ui_QGoPrintDatabase.h"
#include "MegaVTK2Configure.h"

class QGoPrintDatabase : public QWidget,
  private Ui::WidgetPrintDatabase
{
  Q_OBJECT

public:
  explicit QMEGAVTKADDON2_EXPORT QGoPrintDatabase(QString NameDB);
  virtual QMEGAVTKADDON2_EXPORT ~QGoPrintDatabase();


protected:
  QStringList GetTableContentFromDB(QString TableName);
  void QPrintColumnNames (QString TableName, QTableWidget* QTabname);
  QString m_NameDB;
};

#endif
