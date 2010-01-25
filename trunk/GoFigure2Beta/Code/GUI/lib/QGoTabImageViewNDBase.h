#ifndef __QGoTabImageViewNDBase_h
#define __QGoTabImageViewNDBase_h

#include "QGoTabImageViewElementBase.h"

class vtkImageData;

class QGoTabImageViewNDBase : public QGoTabImageViewElementBase
{
  Q_OBJECT

  public:
    explicit QGoTabImageViewNDBase( QWidget* parent = 0 );
    virtual ~QGoTabImageViewNDBase();

    virtual void SetImage( vtkImageData* iImage );

  public slots:
    void ShowAllChannels( bool iChecked );
    void ShowOneChannel( int iChannel );
  protected:
    vtkImageData* m_Image;

    virtual void SetImageToImageViewer( vtkImageData* image ) = 0;
    
};
#endif
