#ifndef QGOCOMPARER2DSYNC_H
#define QGOCOMPARER2DSYNC_H

#include "vtkCommand.h"
#include "QGoComparer2D.h";
class vtkObject;
class vtkCallbackCommand;


/**
\class QGoComparer2DSync
\brief This object takes a list of comparers and synchronize their cameras
*  by setting up callbacks. it is recommended to let the QGoComparerOrchestre deal
*  with comparer synchronization.
*/
class QGoComparer2DSync
{
public:
  /** \brief the constructor do most of the work :
  *add observer to master comparer
  *call to setup a callback and start it
  */
  QGoComparer2DSync(std::vector<QGoComparer2D*> ioOpenComparers);

  ~QGoComparer2DSync();

  /** \brief callback function to synchornize cameras (has to be public)
  */
  static void synchronizeCameras ( vtkObject* caller, long unsigned int eventId,
                                   void* clientData, void* callData );

  /** \brief remove a comparer from the vector of synchronized comparers
  *  (this method takes care of removing the callback)
  */
  void removeComparer( QGoComparer2D* ioComparer );

  /** \brief add a comparer to the vector of synchronized comparers
  * (this method takes care of adding the callback)
  */
  void addComparer( QGoComparer2D* ioComparer );

private:
  /** \brief setup the callback command object (client data, called function...etc)
  */
  void SetupCallBack();

  /** \brief callback object to link callback function to comparer's event
  */
  vtkCallbackCommand*            m_vtkCallBackCamSync;

  /** \brief all open comparers are stored in this array,
  *  to transmit it to the callback function
  */
  std::vector<QGoComparer2D*>      m_openComparers;
};

#endif // QGOCOMPARER2DSYNC_H
