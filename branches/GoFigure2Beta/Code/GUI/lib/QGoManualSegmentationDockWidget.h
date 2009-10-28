#ifndef __QGoManualSegmentationDockWidget_h
#define __QGoManualSegmentationDockWidget_h

#include <QDockWidget>
#include "ui_ManualSegmentationDockWidget.h"

class vtkProperty;

#include "QGoManualSegmentationSettingsDialog.h"

class QGoManualSegmentationDockWidget :
  public QDockWidget,
  private Ui::ManualSegmentationDockWidget
{
  Q_OBJECT
  public:
    QGoManualSegmentationDockWidget( QWidget* parent = 0 );
    ~QGoManualSegmentationDockWidget();

    QColor GetValidatedColor() const;
    unsigned int GetMeshId() const;

    double GetLinesWidth() const;
    QColor GetLinesColor() const;
    QColor GetNodesColor() const;
    QColor GetActiveNodesColor() const;

  public slots:
    void OnSetColorPressed();
    void SetContourColor();
    void GenerateRandomColorForGivenId( int iId );
    void GenerateContourRepresentationProperties();

  signals:
    void ReinitializePressed();
    void ValidatePressed();
    void ReinitializeAndIncrementPressed();
    void ActivateManualSegmentationToggled( bool );
    void ContourRepresentationPropertiesChanged();

  protected:
    std::vector< unsigned int >   m_OrderVector;
    QColor                        m_ValidatedColor;
//     QHash< unsigned int, QColor > m_IdValidatedColor;
    QGoManualSegmentationSettingsDialog* m_SettingsDialog;
    double m_LinesWidth;
    QColor m_LinesColor;
    QColor m_NodesColor;
    QColor m_ActiveNodesColor;

    void GenerateOrderVector( const QColor& iSelectedColor );


  private:
};

#endif

