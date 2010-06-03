#ifndef QGOCOMPARER2DSYNC_H
#define QGOCOMPARER2DSYNC_H

#include "vtkCommand.h"
#include "QGoComparer2D.h"

class vtkObject;
class vtkCallbackCommand;

/*
This object takes two comparer, a master and a slave,
and setup a callback to keep slave's camera synchronized with master's
*/

class QGoComparer2DSync
{
public:
    // the constructor do most of the work :
    // add observer to master comparer
    // call to setup a callback and start it
  QGoComparer2DSync(std::vector<QGoComparer2D*> ioOpenComparers);

  ~QGoComparer2DSync();

  // callback function to synchornize cameras (has to be public)
  static void synchronizeCameras ( vtkObject* caller, long unsigned int eventId,
                                   void* clientData, void* callData );


  void removeComparer( QGoComparer2D* ioComparer );

  void addComparer( QGoComparer2D* ioComparer );


private:

  // setup the callback command object (client data, called function...etc)
  void SetupCallBack();

  // callback object to link callback function to master comparer's event
  vtkCallbackCommand*            m_vtkCallBackCamSync;

  // all open comparers are stored in this array,
  // to transmit it to the callback function
  std::vector<QGoComparer2D*>      m_openComparers;

};

#endif // QGOCOMPARER2DSYNC_H
