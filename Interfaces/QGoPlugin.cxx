#include "QGoPlugin.h"

QGoPlugin::QGoPlugin( QObject* iParent ) : QObject( iParent )
{
}

QGoPlugin::~QGoPlugin()
{}

QString QGoPlugin::Name() const
{
  return m_Information.Name;
}

QString QGoPlugin::Version() const
{
  return m_Information.Version;
}

QString QGoPlugin::GoFigureCompatibilityVersion() const
{
  return m_Information.GoFigureCompatibilityVersion;
}

QString QGoPlugin::Distributor() const
{
  return m_Information.Distributor;
}

QString QGoPlugin::Copyright() const
{
  return m_Information.Copyright;
}

QString QGoPlugin::License() const
{
  return m_Information.License;
}

QString QGoPlugin::Description() const
{
  return m_Information.Description;
}
