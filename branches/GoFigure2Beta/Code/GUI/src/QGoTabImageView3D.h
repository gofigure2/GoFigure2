#ifndef __QGoTabImageView3D_h
#define __QGoTabImageView3D_h

#include "QGoTabImageViewElementBase.h"

#include <QHBoxLayout>
class vtkImageData;

class QAction;
class QToolBar;
class QDockWidget;

class QGoImageView3D;

class QGoTabImageView3D : public QGoTabImageViewElementBase
{
  public:
    QGoTabImageView3D( QWidget* parent = 0 );
    virtual ~QGoTabImageView3D();

    GoFigure::TabDimensionType GetTabDimensionType( ) const;

    virtual void SetImage( vtkImageData* iImage );
    virtual void Update();

    void setupUi( QWidget* parent );
    void retranslateUi( QWidget *parent );

    virtual std::vector< QAction* > ViewActions()
      {return std::vector< QAction* >(); }
    virtual std::list< QDockWidget* > DockWidget()
      {return std::list< QDockWidget* >(); }
    virtual std::list< QWidget* > AdditionalWidget()
      {return std::list< QWidget* >(); }

    virtual void WriteSettings() {}
    virtual void ReadSettings() {}

  protected:
    QHBoxLayout*      m_LayOut;
    QGoImageView3D*   m_ImageView;
    vtkImageData*     m_Image;

    std::vector< QAction* > m_ViewActions;

    void GetBackgroundColorFromImageViewer( ) {}
    void SetBackgroundColorToImageViewer( ) {}

  private:
};
#endif
