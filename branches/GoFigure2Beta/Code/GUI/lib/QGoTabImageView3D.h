#ifndef __QGoTabImageView3D_h
#define __QGoTabImageView3D_h

#include "QGoTabImageViewElementBase.h"
#include "SnapshotHelper.h"

#include <QHBoxLayout>
#include <QSpinBox>

class vtkImageData;

class QAction;
class QToolBar;
class QDockWidget;

class QGoImageView3D;

class QGoTabImageView3D : public QGoTabImageViewElementBase
{
  Q_OBJECT
  public:
    QGoTabImageView3D( QWidget* parent = 0 );
    virtual ~QGoTabImageView3D();

    GoFigure::TabDimensionType GetTabDimensionType( ) const;

    virtual void SetImage( vtkImageData* iImage );
    virtual void Update();

    void setupUi( QWidget* parent );
    void retranslateUi( QWidget *parent );

    virtual std::vector< QAction* > ViewActions();

    virtual std::list< QDockWidget* > DockWidget();

    virtual std::list< QWidget* > AdditionalWidget()
      {return std::list< QWidget* >(); }

    virtual void WriteSettings() {}
    virtual void ReadSettings() {}

  public slots:
    QString SnapshotViewXY( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) );
    QString SnapshotView2( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) );
    QString SnapshotView3( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) );
    QString SnapshotViewXYZ( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) );

    void SetSliceViewXY( const int& );
    void SetSliceViewXZ( const int& );
    void SetSliceViewYZ( const int& );

    void SetFullScreenView( const int& iS );
    void Quadview();
    void FullScreenViewXY();
    void FullScreenViewXZ();
    void FullScreenViewYZ();
    void FullScreenViewXYZ();

    void ChangeLookupTable();
    void ShowScalarBar( const bool& );

  protected:
    QHBoxLayout*      m_LayOut;
    QGoImageView3D*   m_ImageView;
    vtkImageData*     m_Image;
    QSpinBox*         m_XSliceSpinBox;
    QSpinBox*         m_YSliceSpinBox;
    QSpinBox*         m_ZSliceSpinBox;

    std::vector< QAction* > m_ViewActions;
    QDockWidget* m_DockWidget;

    void GetBackgroundColorFromImageViewer( );
    void SetBackgroundColorToImageViewer( );

//   private:
};
#endif
