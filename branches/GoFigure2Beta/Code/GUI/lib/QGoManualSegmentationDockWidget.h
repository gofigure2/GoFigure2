#ifndef __QGoManualSegmentationDockWidget_h
#define __QGoManualSegmentationDockWidget_h

#include <QDockWidget>
#include "ui_ManualSegmentationDockWidget.h"

class QGoManualSegmentationDockWidget :
  public QDockWidget,
  private Ui::ManualSegmentationDockWidget
{
  Q_OBJECT
  public:
    QGoManualSegmentationDockWidget( QWidget* parent = 0 );
    ~QGoManualSegmentationDockWidget();

  public slots:
    void OnSetColorPressed();
    void OnReinitializePressed();
    void OnValidatePressed();
    void OnReinitializeAndIncrementPressed();

  protected:
    QHash< unsigned int, QColor > m_IdValidatedColor;
  private:
};

#endif

