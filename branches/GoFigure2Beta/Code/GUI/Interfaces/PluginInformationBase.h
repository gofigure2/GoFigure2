#ifndef __PluginInformationBase_h
#define __PluginInformationBase_h

class QString;

/**
\class PluginInformationBase
\brief
*/
struct PluginInformationBase
{
  public:
    QString Name;
    QString Version;
    QString GoFigureCompatibilityVersion;
    QString Distributor;
    QString Copyright;
    QString License;
    QString Description;
};
#endif

