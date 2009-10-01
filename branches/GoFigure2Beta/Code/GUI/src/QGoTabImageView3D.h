#ifndef __QGoTabImageView3D_h
#define __QGoTabImageView3D_h

#include "QGoTabImageViewElementBase.h"

class QGoTabImageView3D : public QGoTabImageViewElementBase
{
  public:
    QGoTabImageView3D( QWidget* parent = 0 );
    virtual ~QGoTabImageView3D();

    GoFigure::TabDimensionType GetTabDimensionType( ) const;

    virtual void SetImage( vtkImageData* iImage ) {}
    virtual void Update() {}

    virtual std::vector< QAction* > ViewActions()
      {return std::vector< QAction* >(); }
    virtual std::list< QDockWidget* > DockWidget()
      {return std::list< QDockWidget* >(); }
    virtual std::list< QWidget* > AdditionalWidget()
      {return std::list< QWidget* >(); }

    virtual void WriteSettings() {}
    virtual void ReadSettings() {}

  protected:
  private:
};
#endif
