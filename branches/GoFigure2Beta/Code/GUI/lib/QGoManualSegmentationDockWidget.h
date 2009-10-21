#ifndef __QGoManualSegmentationDockWidget_h
#define __QGoManualSegmentationDockWidget_h

#include <QDockWidget>
#include "ui_ManualSegmentationDockWidget.h"

#include "QGoManualSegmentationSettingsDialog.h"

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
    void SetContourColor();
    void GenerateRandomColorForGivenId( int iId );

  signals:
    void ReinitializePressed();
    void ValidatePressed();
    void ReinitializeAndIncrementPressed();

  protected:
    std::vector< unsigned int >   m_OrderVector;
    QColor                        m_ValidatedColor;
//     QHash< unsigned int, QColor > m_IdValidatedColor;
    QGoManualSegmentationSettingsDialog* m_SettingsDialog;

    void GenerateOrderVector( const QColor& iSelectedColor );


  private:
};

#endif

