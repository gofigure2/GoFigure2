#ifndef QGoSynchronizedView3DCallbacks_H
#define QGoSynchronizedView3DCallbacks_H



#include "vtkCommand.h"
#include "QGoSynchronizedView3D.h"

class vtkObject;
class vtkCallbackCommand;

/**
\class QGoSynchronizedView3DCallbacks
\brief This object takes a list of comparers and synchronize their cameras
*  by setting up callbacks. it is recommended to let the QGoSynchronizedViewOrchestre deal
*  with comparer synchronization.
*/

class QGoSynchronizedView3DCallbacks : public QObject
{
  // QT macro for signals and slots
  Q_OBJECT
public:
  /** \brief the constructor do most of the work :
  *  add observer to master comparer
  *  call to setup a callback and start it
  */
  QGoSynchronizedView3DCallbacks( std::vector<QGoSynchronizedView3D*>
                                    ioOpenSynchronizedViews,
                                  QObject*  iParent = 0);

  ~QGoSynchronizedView3DCallbacks();

  /** \brief callback function to synchornize cameras (has to be public)
  */
  static void synchronizeCameras0 ( vtkObject* caller, long unsigned int eventId,
                                   void* clientData, void* callData );


  static void synchronizeCameras1 ( vtkObject* caller, long unsigned int eventId,
                                   void* clientData, void* callData );

  static void synchronizeCameras2 ( vtkObject* caller, long unsigned int eventId,
                                   void* clientData, void* callData );

  // callback function to synchornize cameras (has to be public)
  static void synchronizeCameras3 ( vtkObject* caller, long unsigned int eventId,
                                   void* clientData, void* callData );

  /** \brief remove a comparer from the vector of synchronized comparers
  *  (this method takes care of removing the callback)
  */
  void removeSynchronizedView( QGoSynchronizedView3D* ioSynchronizedView );

  /** \brief add a comparer to the vector of synchronized comparers
  *  (this method takes care of adding the callback)
  */
  void addSynchronizedView( QGoSynchronizedView3D* ioSynchronizedView );

signals:

    /** \brief signals for synchronizing sliders (gives current XYslice
    *  of any changing comparer)
    */
    void SliceViewXYChanged( int oSlice);

    /** \brief signals for synchronizing sliders (gives current XZ slice
    *  of any changing comparer)
    */
    void SliceViewXZChanged( int oSlice);

    /** \brief signals for synchronizing sliders (gives current YZ slice
    *  of any changing comparer)
    */
    void SliceViewYZChanged( int oSlice);

private:

  /** \brief setup the callback command object (client data, called function...etc)
  */
  void SetupCallBack();

  /** callback object to link callback function to master comparer's event
  */
  vtkCallbackCommand*            m_vtkCallBackCamSync0;
  vtkCallbackCommand*            m_vtkCallBackCamSync1;
  vtkCallbackCommand*            m_vtkCallBackCamSync2;
  vtkCallbackCommand*            m_vtkCallBackCamSync3;

  /** all open comparers are stored in this array,
  *  to transmit it to the callback function
  */
  std::vector<QGoSynchronizedView3D*>      m_openSynchronizedView;

};


#endif // QGoSynchronizedView3D3DSYNC_H
