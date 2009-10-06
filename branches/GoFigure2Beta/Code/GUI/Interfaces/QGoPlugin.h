#ifndef __QGoPlugin_h
#define __QGoPlugin_h

#include <vector>
#include <list>

#include <QString>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QDockWidget>
#include <QWidget>

#include "PluginInformationBase.h"

namespace GoFigure
{
  enum TabDimensionType
    {
    TWO_D = 2,
    THREE_D,
    THREE_D_WITH_T,
    FOUR_D
    };
}

/**
\class QGoPlugin
\brief Abstract class for any kind of plugins used by GoFigure2.
*/
class QGoPlugin
{
  public:
    /** \brief Default Constructor.*/
    QGoPlugin();

    /** \brief Destructor.*/
    virtual ~QGoPlugin();

    /** \brief return Plugin Name of the plugin.*/
    QString Name() const;

    /** \brief return Plugin Version of the plugin.*/
    QString Version() const;

    /** \brief return GoFigure version for compatibility with the plugin.*/
    QString GoFigureCompatibilityVersion() const;

    /** \brief return Distributor of the plugin.*/
    QString Distributor() const;

    /** \brief return Copyright of the plugin.*/
    QString Copyright() const;

    /** \brief return License of the plugin.*/
    QString License() const;

    /** \brief return Short description of the plugin.*/
    QString Description() const;

    /** \brief Write Settings for the plugin.*/
    virtual void WriteSettings() = 0;

    /** \brief Read Settings for the plugin.*/
    virtual void ReadSettings() = 0;

    virtual std::list< GoFigure::TabDimensionType > TabElementCompatibility() const = 0;

    /** \brief */
    virtual std::vector< QAction* > Actions() = 0;

    /** \brief */
    virtual QMenu* Menu() = 0;

    /** \brief */
    virtual QToolBar* ToolBar() = 0;

    /** \brief */
    virtual QDockWidget* DockWidget() = 0;

    /** \brief */
    virtual QWidget* AdditionalWidget() = 0;

    /** \brief */
    virtual void OnTabActivated( const int& ) = 0;

    /** \brief */
    virtual void OnTabMoved( const int& , const int& ) = 0;

    /** \brief */
    virtual void OnTabClosed( const int& ) = 0;

  protected:
    /** \brief contains all information related to the plugin.
        \todo it would not be a bad idea to use a xml file instead.
    */
    PluginInformationBase m_Information;

    /** \brief Set all required information (name, version, license...).
        \note This method MUST be called in the constructor of any plugin which
        inherits from QGoPlugin.
    */
    virtual void SetAllRequiredInformation( ) = 0;

  private:
    /** \brief purposely not implemented.*/
    QGoPlugin( const QGoPlugin& );

    /** \brief purposely not implemented.*/
    void operator = ( const QGoPlugin& );
};


Q_DECLARE_INTERFACE( QGoPlugin, "GoFigure2.QGoPlugin/1.0" )

#endif
