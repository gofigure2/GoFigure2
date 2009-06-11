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
  explicit QMEGAVTKADDON2_EXPORT QGoPrintDatabase();
  virtual QMEGAVTKADDON2_EXPORT ~QGoPrintDatabase();
  void Fill_Database(QString ServerName,QString login,
      QString Password, QString DBName, QString ExpID);


protected:
  QStringList GetTableContentFromDB(QString TableName);
  void QPrintColumnNames (QString TableName);

  QString m_NameDB;
  QString m_Server;
  QString m_User;
  QString m_Password;
};

#endif
