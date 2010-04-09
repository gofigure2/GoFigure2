#include "QGoPluginHelper.h"

#include <QApplication>
#include <QDir>
#include <QString>

//--------------------------------------------------------------------------
QDir FindPluginDirectory( const QString& iSubdir )
{
  QDir dir = QDir( QApplication::applicationDirPath() );

#if defined(Q_OS_WIN)
  if( ( dir.dirName().toLower() == "debug" ) ||
      ( dir.dirName().toLower() == "release" ) )
    {
    dir.cdUp();
    }
#elif defined(Q_OS_MAC)
  if( dir.dirName() == "MacOS" )
    {
    dir.cdUp();
    dir.cdUp();
    dir.cdUp();
    }
#endif

  if( dir.cd( iSubdir ) )
    {
    return dir;
    }
  else
    {
    QString temp = iSubdir;
    temp.toUpper();

    if( dir.cd( temp ) )
      {
      return dir;
      }
    else
      {
      temp.toLower();

      dir.cd( temp );
      return dir;
      }
    }
}
//--------------------------------------------------------------------------
