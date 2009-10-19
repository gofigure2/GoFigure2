#ifndef __QGoChannelVisualizationDockWidget_h
#define __QGoChannelVisualizationDockWidget_h

#include <QDockWidget>
#include "ui_ChannelVisualizationDockWidget.h"

class QGoChannelVisualizationDockWidget :
  public QDockWidget,
  private Ui::ChannelVisualizationDockWidget
{
  Q_OBJECT

  public:
    QGoChannelVisualizationDockWidget( QWidget* parent );
    ~QGoChannelVisualizationDockWidget();

    void SetNumberOfChannels( const unsigned int& iN );
    void SetChannel( const unsigned int& i, const QString& iText = QString() );

    void AddChannel( const QString& iText = QString() );

  public slots:
    void Accept();

  signals:
    void ChangeVisualizationToAllChannels();
    void ChangeVisualizationToOneChannel( unsigned int Channel );

  protected:
    bool m_AllChannels;
    unsigned int m_ChannelId;

};
#endif
