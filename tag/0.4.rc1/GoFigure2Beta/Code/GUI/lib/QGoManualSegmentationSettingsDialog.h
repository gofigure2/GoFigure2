#ifndef __QGoManualSegmentationSettingsDialog_h
#define __QGoManualSegmentationSettingsDialog_h

#include "ui_ManualSegmentationSettingsDlg.h"

#include <Qt>
#include "vtkSmartPointer.h"

class vtkOrientedGlyphContourRepresentation;
class vtkContourWidget;
class vtkRenderer;
class vtkPolyData;

class QGoManualSegmentationSettingsDialog :
  public QDialog,
  private Ui::ManualSegmentationSettingsDlg
{
  Q_OBJECT
public:
  QGoManualSegmentationSettingsDialog( QWidget* parent = 0,
    const double& iWidth = 1.,
    const QColor& iLineColor = Qt::magenta,
    const QColor& iNodeColor = Qt::cyan,
    const QColor& iActivatedNodeColor = Qt::yellow );
  ~QGoManualSegmentationSettingsDialog();

  double GetLineWidth( ) const;
  QColor GetLineColor( ) const;
  QColor GetNodeColor( ) const;
  QColor GetActivatedNodeColor( ) const;

public slots:
  void SetLineWidth( const double& );
  void SelectLineColor( );
  void SelectNodeColor( );
  void SelectActivatedNodeColor( );

private:
  double m_LineWidth;
  QColor m_LineColor;
  QColor m_NodeColor;
  QColor m_ActivatedNodeColor;

  vtkSmartPointer< vtkOrientedGlyphContourRepresentation > m_ContourRepresentation;
  vtkSmartPointer< vtkContourWidget > m_ContourWidget;
  vtkSmartPointer< vtkRenderer >      m_Renderer;
  vtkSmartPointer< vtkPolyData >      m_InitPD;
};
#endif
