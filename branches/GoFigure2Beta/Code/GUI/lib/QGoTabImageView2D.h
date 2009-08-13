#ifndef __QGoTabImageView2D_h
#define __QGoTabImageView2D_h

class vtkImageData;

class QAction;
class QToolBar;
class QDockWidget;
class QSplitter;

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

  void setupUi( QWidget* parent );
  void retranslateUi(QWidget *Form);

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

  QSplitter*                        m_Splitter;
  QWidget*                          m_LayOutWidget;
  QHBoxLayout*                      m_LayOut;
  QVTKWidget*                       m_QVTKWidgetXY;
  vtkViewImage2DWithContourWidget*  m_View;
  vtkImageData*                     m_Image;
  vtkEventQtSlotConnect*            m_VTKEventQtConnector;

private:
  QGoTabImageView2D( const QGoTabImageView2D& );
  void operator = ( const QGoTabImageView2D& );
};
#endif
