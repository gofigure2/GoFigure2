#ifndef QGoComparer3D_H
#define QGoComparer3D_H

#include "QGoComparer.h"

#include "vtkSmartPointer.h"
class vtkCamera;
class vtkImageData;
class vtkEventQtSlotConnect;

class QGoImageView3D;
class QGoCompareOrchestra;


class QGoComparer3D : public QGoComparer 
{
  Q_OBJECT
public:
  QGoComparer3D(QString iComparerName, QWidget *parent = 0);

  ~QGoComparer3D();

    // returns the type of comparer (2 for 2D, 3 for 3D)
    int GetComparerType();

    /** Set image displayed by the comparer
    */
    void SetImage(vtkImageData* iImage);

    /** render the iId'th imageview:
    *  3D visualization usually contains 4 imageviewers :
    *  three 2D projection and a 3D view : iId=[0-3]
    */
    void Render(const int& iId);

    /** get the camera of the current viewer;
    * iId=[0-3]
    */
    vtkCamera* GetCamera(const int& iId);
    /** get the camera of the current viewer;
    * if several viewers, get camera of viewer 0
    */
    //vtkCamera* GetCamera();

    /** print the comparer information :
    *  it consists in the image information if any.
    */
    void PrintOs(ostream &os);




    /** Get the fullscreen view : iId = [0-4]
    * 0 : Quadview (all 4 views)
    * 1 : XY
    * 2 : XZ
    * 3 : YZ
    * 4 : 3D view
    */
    int GetFullScreenView();

    QGoImageView3D* GetImageView();

public slots:

    QString SnapshotViewXY( const GoFigure::FileType& iType,
      const QString& iBaseName = tr( "Snapshot" ) );

    QString SnapshotView2( const GoFigure::FileType& iType,
      const QString& iBaseName = QString( "snapshot" ) );
    QString SnapshotView3( const GoFigure::FileType& iType,
      const QString& iBaseName = QString( "snapshot" ) );
    QString SnapshotViewXYZ( const GoFigure::FileType& iType,
      const QString& iBaseName = QString( "snapshot" ) );

    /** Set the fullscreen view : iId = [0-4]
    * 0 : Quadview (all 4 views)
    * 1 : XY
    * 2 : XZ
    * 3 : YZ
    * 4 : 3D view
    */
    void SetFullScreenView(const int& iId);

    void SetFullXYScreenView();
    void SetFullXZScreenView();
    void SetFullYZScreenView();
    void SetFullXYZScreenView();
    void SetQuadView();



private:

  // create the viewer contained in the widget
  void createViewer();


};


#endif // QGoComparer3D_H
