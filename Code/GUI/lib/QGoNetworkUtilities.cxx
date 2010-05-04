#include "QGoNetworkUtilities.h"

#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QDebug>

QGoNetworkUtilities::
QGoNetworkUtilities( QObject* iParent ) : QObject( iParent )
{
  m_Manager = new QNetworkAccessManager( this );

  QObject::connect( m_Manager, SIGNAL( finished( QNetworkReply* ) ),
    this, SLOT( DisplayResults( QNetworkReply* ) ) );
}

QGoNetworkUtilities::~QGoNetworkUtilities()
{
  // segfault here ...
  if( m_Reply )
    {
    delete m_Reply;
    m_Reply = 0;
    }
}

void QGoNetworkUtilities::CheckForUpdates()
{
  // address
  QUrl url( "http://gofigure2.sourceforge.net" );
  m_Reply = m_Manager->get( QNetworkRequest( url ) );
}

void
QGoNetworkUtilities::
DisplayResults( QNetworkReply* reply )
{
  QString response;
  bool ok = (reply->error() == QNetworkReply::NoError);

  // no error received?
  if( ok )
    {
    // read data from QNetworkReply here
    QByteArray bytes = reply->readAll();
    response = QString(bytes);
    }
  // Some http error received
  //else
  //  {
  //  error = true;
  //  }


  emit CheckForUpdatesDone( response, ok );

}

