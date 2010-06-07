#ifndef QGoComparer3DSYNC_H
#define QGoComparer3DSYNC_H



#include "vtkCommand.h"
#include "QGoComparer3D.h"

class vtkObject;
class vtkCallbackCommand;

/**
\class QGoComparer3DSync
\brief This object takes a list of comparers and synchronize their cameras
*  by setting up callbacks. it is recommended to let the QGoComparerOrchestre deal
*  with comparer synchronization.
*/

class QGoComparer3DSync : public QObject
{
  // QT macro for signals and slots
  Q_OBJECT
public:
  /** \brief the constructor do most of the work :
  *  add observer to master comparer
  *  call to setup a callback and start it
  */
  QGoComparer3DSync(std::vector<QGoComparer3D*> ioOpenComparers,
                    QObject*                    iParent = 0);

  ~QGoComparer3DSync();

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
  void removeComparer( QGoComparer3D* ioComparer );

  /** \brief add a comparer to the vector of synchronized comparers
  *  (this method takes care of adding the callback)
  */
  void addComparer( QGoComparer3D* ioComparer );

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
  std::vector<QGoComparer3D*>      m_openComparers;

};


#endif // QGoComparer3D3DSYNC_H
