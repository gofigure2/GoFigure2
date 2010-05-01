#ifndef __QGoNetworkUtilities_h
#define __QGoNetworkUtilities_h

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

class QGoNetworkUtilities : public QObject
{
  Q_OBJECT
public:
  explicit QGoNetworkUtilities( QObject* parent = 0 );
  ~QGoNetworkUtilities();
  void CheckForUpdates();

signals:
  void CheckForUpdatesDone( QString msg, bool error );

public slots:
  virtual void DisplayResults(QNetworkReply* reply);

protected:
  QNetworkAccessManager* m_Manager;
  QNetworkReply* m_Reply;
};

#endif // NETWORKUTILS_H


