#ifndef QGoComparer3DSYNC_H
#define QGoComparer3DSYNC_H



#include "vtkCommand.h"
#include "QGoComparer3D.h"

class vtkObject;
class vtkCallbackCommand;

/*
This object takes two comparer, a master and a slave,
and setup a callback to keep slave's camera synchronized with master's
*/

class QGoComparer3DSync : public QObject
{
  // QT macro for signals and slots
  Q_OBJECT
public:
    // the constructor do most of the work :
    // add observer to each image contained in the vector of comparers
    // call to setup a callback and start it
  QGoComparer3DSync(std::vector<QGoComparer3D*> ioOpenComparers,
                    QObject*                    iParent = 0);

  ~QGoComparer3DSync();

  // callback function to synchornize cameras (has to be public)
  static void synchronizeCameras0 ( vtkObject* caller, long unsigned int eventId,
                                   void* clientData, void* callData );

  // callback function to synchornize cameras (has to be public)
  static void synchronizeCameras1 ( vtkObject* caller, long unsigned int eventId,
                                   void* clientData, void* callData );

  // callback function to synchornize cameras (has to be public)
  static void synchronizeCameras2 ( vtkObject* caller, long unsigned int eventId,
                                   void* clientData, void* callData );

  // callback function to synchornize cameras (has to be public)
  static void synchronizeCameras3 ( vtkObject* caller, long unsigned int eventId,
                                   void* clientData, void* callData );


  void removeComparer( QGoComparer3D* ioComparer );

  void addComparer( QGoComparer3D* ioComparer );

signals:

    // signals for synchronizing sliders
    void SliceViewXYChanged( int oSlice);
    void SliceViewXZChanged( int oSlice);
    void SliceViewYZChanged( int oSlice);

private:

  // setup the callback command object (client data, called function...etc)
  void SetupCallBack();

  // callback object to link callback function to master comparer's event
  vtkCallbackCommand*            m_vtkCallBackCamSync0;
  vtkCallbackCommand*            m_vtkCallBackCamSync1;
  vtkCallbackCommand*            m_vtkCallBackCamSync2;
  vtkCallbackCommand*            m_vtkCallBackCamSync3;

  // all open comparers are stored in this array,
  // to transmit it to the callback function
  std::vector<QGoComparer3D*>      m_openComparers;

};


#endif // QGoComparer3D3DSYNC_H
