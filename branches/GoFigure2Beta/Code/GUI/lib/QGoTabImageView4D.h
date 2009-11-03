#ifndef __QGoTabImageView4D_h
#define __QGoTabImageView4D_h

#include "QGoTabElementBase.h"

#include "QGoPlugin.h"
#include "QGoImageView3D.h"

// #include "itkMultiFileReader.h"
#include "GoFigureFileInfoMultiIndexContainerHelper.h"
#include "SnapshotHelper.h"

#include <QResizeEvent>

/**
\class QGoTabImageView4D
\brief
\example GUI/lib/qgotabimageview4d.cxx
*/
class QGoTabImageView4D : public QGoTabElementBase
{
  Q_OBJECT
  public:
    QGoTabImageView4D( QWidget* parent = 0 );
    virtual ~QGoTabImageView4D();

    GoFigure::TabDimensionType GetTabDimensionType( ) const;

//     void SetMultiFiles( FileListType* iFileList,
//       const int& iTimePoint );

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
    void SetTimePoint( const int& );

    QString SnapshotViewXY( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) );
    QString SnapshotView2( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) );
    QString SnapshotView3( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) );
    QString SnapshotViewXYZ( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) );

    /** \todo to be implemented */
    QString SnapshotViewXT( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) ) {return QString();}
    /** \todo to be implemented */
    QString SnapshotViewYT( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) ) {return QString();}
    /** \todo to be implemented */
    QString SnapshotViewXYT( const GoFigure::SnapshotImageType& iType,
      const QString& iBaseName = QString( "snapshot" ) ) {return QString();}

    void SetSliceViewXY( const int& );
    void SetSliceViewXZ( const int& );
    void SetSliceViewYZ( const int& );
    void SetSliceViewXT( const int& ) {}
    void SetSliceViewYT( const int& ) {}

    void SetFullScreenView( const int& iS );

    void Octview();
    void QuadviewXYZ();
    void QuadviewXYT();
    void FullScreenViewXY();
    void FullScreenViewXZ();
    void FullScreenViewYZ();
    void FullScreenViewXYZ();
    void FullScreenViewXT();
    void FullScreenViewYT();
    void FullScreenViewXYT();

    void ChangeLookupTable();
    void ShowScalarBar( const bool& );

//   protected slots:
//     void ReceiveXYZImage();
//     void ReceiveXYTImage();

  protected:
    QSplitter*            m_Splitter;
    QGoImageView3D*       m_XYZImageView;
    QGoImageView3D*       m_XYTImageView;
    vtkImageData*         m_XYZImage;
    vtkImageData*         m_XYTImage;
    QColor                m_BackgroundColor;
    int                   m_TimePoint;
    int                   m_ZDepth;

//     itk::MultiFileReader::Pointer m_Reader1;
//     itk::MultiFileReader::Pointer m_Reader2;

    GoFigureFileInfoHelperMultiIndexContainer m_FileList;
//     FileListType* m_XYZFileList;
//     FileListType* m_XYTFileList;
//     QGoThreadedMultiFileReader* m_ThreadedReader1;
//     QGoThreadedMultiFileReader* m_ThreadedReader2;

    std::vector< QAction* > m_ViewActions;
    QDockWidget* m_DockWidget;

    void CreateAllViewActions();
    void GetBackgroundColorFromImageViewer( );
    void SetBackgroundColorToImageViewer( );

    virtual void resizeEvent( QResizeEvent* event );
};
#endif
