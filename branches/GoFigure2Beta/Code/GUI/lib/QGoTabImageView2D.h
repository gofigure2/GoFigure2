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

  GoFigure::TabDimensionType GetTabDimensionType( ) const;

  virtual void Update();

  void setupUi( QWidget* parent );
  void retranslateUi( QWidget *parent );

  virtual std::vector< QAction* > ViewActions();
  virtual std::list< QDockWidget* > DockWidget();
  virtual std::list< QWidget* > AdditionalWidget();

  virtual void WriteSettings();
  virtual void ReadSettings();

public slots:
  void ChangeLookupTable();
  void ShowScalarBar( const bool& );

protected:
  QGoImageView2D*   m_ImageView;

  std::vector< QAction* > m_ViewActions;

  void GetBackgroundColorFromImageViewer( );
  void SetBackgroundColorToImageViewer( );

  void SetImageToImageViewer( vtkImageData* image );

private:
  QGoTabImageView2D( const QGoTabImageView2D& );
  void operator = ( const QGoTabImageView2D& );
};
#endif
