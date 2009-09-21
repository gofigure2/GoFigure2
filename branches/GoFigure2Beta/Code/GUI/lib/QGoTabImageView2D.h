#ifndef __QGoTabImageView2D_h
#define __QGoTabImageView2D_h

class vtkImageData;

class QAction;
class QToolBar;
class QDockWidget;

#include "QGoTabImageViewElementBase.h"

#include "QGoImageView2D.h"

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

  void setupUi( QWidget* parent );
  void retranslateUi( QWidget *parent );

  std::vector< QMenu* > Menus();
  std::list< QToolBar* > ToolBar();
  std::list< QDockWidget* > DockWidget();
  std::list< QWidget* > AdditionalWidget();

  void WriteSettings();
  void ReadSettings();

public slots:
  void ChangeBackgroundColor();
  void ChangeLookupTable();
  void ShowScalarBar( const bool& );

protected:
  QHBoxLayout*      m_LayOut;
  QGoImageView2D*   m_ImageView;

  QMenu* m_ViewMenu;
  QMenu* m_PropertiesMenu;
  QMenu* m_FilteringMenu;
  QMenu* m_SegmentationMenu;

  void PopulateMenus( QObject* );

private:
  QGoTabImageView2D( const QGoTabImageView2D& );
  void operator = ( const QGoTabImageView2D& );
};
#endif
