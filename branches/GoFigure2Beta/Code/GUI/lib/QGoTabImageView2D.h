#ifndef __QGoTabImageView2D_h
#define __QGoTabImageView2D_h

class vtkImageData;

class QAction;
class QToolBar;
class QDockWidget;

class QVTKWidget;
class vtkEventQtSlotConnect;

#include "QGoTabImageViewElementBase.h"
#include "QGoLUTDialog.h"

#include "vtkViewImage2DWithContourWidget.h"

/**
\class QGoTabImageView2D
\brief Element of the QTabWidget to be used to visualized 2D images.
*/
class QGoTabImageView2D : public QGoTabImageViewElementBase
{
  Q_OBJECT
public:
  QGoTabImageView2D( QWidget* parent = 0 );
  virtual ~QGoTabImageView2D();

  void SetImage( vtkImageData* iImage );
  void Update();

  std::list< QAction* > Actions();
  std::list< QToolBar* > ToolBar();
  std::list< QDockWidget* > DockWidget();
  std::list< QWidget* > AdditionalWidget();

  void WriteSettings();
  void ReadSettings();

protected:
  QGoLUTDialog*                     m_LUTDialog;

  QVTKWidget*                       m_QVTKWidgetXY;
  vtkViewImage2DWithContourWidget*  m_View;
  vtkImageData*                     m_Image;
  vtkEventQtSlotConnect*            m_VTKEventQtConnector;

  void setupUi( QWidget* parent );


private:
  QGoTabImageView2D( const QGoTabImageView2D& );
  void operator = ( const QGoTabImageView2D& );
};
#endif
