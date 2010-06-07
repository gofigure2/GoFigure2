#ifndef QGoSynchronizedView2DCallbacks_H
#define QGoSynchronizedView2DCallbacks_H

#include "vtkCommand.h"
#include "QGoSynchronizedView2D.h"

class vtkObject;
class vtkCallbackCommand;


/**
\class QGoSynchronizedView2DCallbacks
\brief This object takes a list of QGoSynchronizedView and synchronize their cameras
*  by setting up callbacks. it is recommended to let the QGoSynchronizedViewManager deal
*  with SynchronizedView synchronization.
*/
class QGoSynchronizedView2DCallbacks
{
public:
  /** \brief the constructor do most of the work :
  *  add observers & callbacks to QGoSynchronizedViews of the vector
  */
  QGoSynchronizedView2DCallbacks(std::vector<QGoSynchronizedView2D*> ioOpenSynchronizedViews);

  ~QGoSynchronizedView2DCallbacks();

  /** \brief callback function to synchornize cameras (has to be public)
  */
  static void synchronizeCameras ( vtkObject* caller, long unsigned int eventId,
                                   void* clientData, void* callData );

  /** \brief remove a SynchronizedView from the vector of synchronized SynchronizedViews
  *  (this method takes care of removing the callback)
  */
  void removeSynchronizedView( QGoSynchronizedView2D* ioSynchronizedView );

  /** \brief add a SynchronizedView to the vector of synchronized SynchronizedViews
  * (this method takes care of adding the callback)
  */
  void addSynchronizedView( QGoSynchronizedView2D* ioSynchronizedView );

private:
  /** \brief setup the callback command object (client data, called function...etc)
  */
  void SetupCallBack();

  /** \brief callback object to link callback function to SynchronizedView's event
  */
  vtkCallbackCommand*            m_vtkCallBackCamSync;

  /** \brief all open SynchronizedViews are stored in this array,
  *  to transmit it to the callback function
  */
  std::vector<QGoSynchronizedView2D*>      m_openSynchronizedView;
};

#endif // QGoSynchronizedView2DCallbacks_H
