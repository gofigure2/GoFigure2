#ifndef __QGoTabImageView3D_h
#define __QGoTabImageView3D_h

#include "QGoTabImageViewElementBase.h"

class QGoTabImageView3D : public QGoTabImageViewElementBase
{
  public:
    QGoTabImageView3D( QWidget* parent = 0 );
    virtual ~QGoTabImageView3D();

    void SetImage( vtkImageData* iImage ) {}
    void Update() {}

    std::vector< QMenu* > Menus() {}
    std::list< QToolBar* > ToolBar() {}
    std::list< QDockWidget* > DockWidget() {}
    std::list< QWidget* > AdditionalWidget() {}

    void WriteSettings() {}
    void ReadSettings() {}

  protected:
  private:
};
#endif
