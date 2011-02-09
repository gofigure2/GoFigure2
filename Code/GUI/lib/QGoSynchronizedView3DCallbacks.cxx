/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include "QGoSynchronizedView3DCallbacks.h"

#include "QGoImageView3D.h"
#include "QGoSynchronizedView3D.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include <algorithm>

//--------------------------------------------------------------------------
QGoSynchronizedView3DCallbacks::QGoSynchronizedView3DCallbacks(std::vector< QGoSynchronizedView3D * >
                                                               ioOpenSynchronizedViews,
                                                               QObject *iParent) :
  QObject                  (iParent)
{
  m_vtkCallBackCamSync.resize(4);

  // create the callback object
  SetupCallBack();

  // for every opened SynchronizedView :
  std::vector< QGoSynchronizedView3D * >::iterator
    SynchronizedViewIt = ioOpenSynchronizedViews.begin();

  while ( SynchronizedViewIt != ioOpenSynchronizedViews.end() )
    {
    m_openSynchronizedView.push_back(*SynchronizedViewIt);

    if ( ( *SynchronizedViewIt )->HasViewer() )
    // add the callback to the SynchronizedView's camera
      {
      for ( int i = 0; i < 4; i++ )
        {
        ( *SynchronizedViewIt )->GetCamera(i)
        ->AddObserver(vtkCommand::ModifiedEvent,
                      QGoSynchronizedView3DCallbacks::
                      m_vtkCallBackCamSync[i]);
        }

      // we connect the sliders to the synchronizer (monitor slider changes) :
      QObject::connect( ( *SynchronizedViewIt )->GetImageView(),
                        SIGNAL( SliceViewXYChanged(int) ),
                        this,
                        SIGNAL( SliceViewXYChanged(int) ) );

      QObject::connect( ( *SynchronizedViewIt )->GetImageView(),
                        SIGNAL( SliceViewXZChanged(int) ),
                        this,
                        SIGNAL( SliceViewXZChanged(int) ) );

      QObject::connect( ( *SynchronizedViewIt )->GetImageView(),
                        SIGNAL( SliceViewYZChanged(int) ),
                        this,
                        SIGNAL( SliceViewYZChanged(int) ) );

      // we connect the synchronizer to the SynchronizedView
      QObject::connect( this,
                        SIGNAL( SliceViewXYChanged(int) ),
                        ( *SynchronizedViewIt )->GetImageView(),
                        SLOT( SetSliceViewXY(int) ) );

      QObject::connect( this,
                        SIGNAL( SliceViewXZChanged(int) ),
                        ( *SynchronizedViewIt )->GetImageView(),
                        SLOT( SetSliceViewXZ(int) ) );

      QObject::connect( this,
                        SIGNAL( SliceViewYZChanged(int) ),
                        ( *SynchronizedViewIt )->GetImageView(),
                        SLOT( SetSliceViewYZ(int) ) );
      }
    ++SynchronizedViewIt;
    }
}

//--------------------------------------------------------------------------
// the destructor is very important here, we want to leave clean
// SynchronizedViews behind
QGoSynchronizedView3DCallbacks::
~QGoSynchronizedView3DCallbacks()
{
  std::vector< QGoSynchronizedView3D * >::iterator SynchronizedViewIt;

  // we remove the open synchronized SynchronizedViews
  while ( !m_openSynchronizedView.empty() )
    {
    // remove (AND NOT DELETE, this is the Orchestra's business)
    // all pointers in the vector

    // We remove the observer if any
    if ( m_openSynchronizedView.back()->HasViewer() )
      {
      // remove the callback object from each object's camera
      for ( int i = 0; i < 4; i++ )
        {
        m_openSynchronizedView.back()->GetCamera(i)
        ->RemoveObserver(QGoSynchronizedView3DCallbacks::
                         m_vtkCallBackCamSync[i]);
        }
      }
    m_openSynchronizedView.pop_back();
    }

  // we can now delete the callbacks !
  for ( int i = 0; i < 4; i++ )
    {
    m_vtkCallBackCamSync[i]->Delete();
    }
}

//--------------------------------------------------------------------------
// this is the callback function : do deep copies to keep track of
// the moving camera's position
void
QGoSynchronizedView3DCallbacks::synchronizeCameras0(vtkObject *caller,
                                                    long unsigned int eventId,
                                                    void *clientData,
                                                    void *callData)
{
  synchronizeCamera(0, caller, eventId, clientData, callData);
}

//--------------------------------------------------------------------------
// this is the callback function : do deep copies to keep track of
//  master's camera position
//
void
QGoSynchronizedView3DCallbacks::synchronizeCameras1(vtkObject *caller,
                                                    long unsigned int eventId,
                                                    void *clientData,
                                                    void *callData)
{
  synchronizeCamera(1, caller, eventId, clientData, callData);
}

//--------------------------------------------------------------------------
// this is the callback function : do deep copies to keep track of
// master's camera position
void
QGoSynchronizedView3DCallbacks::synchronizeCameras2(vtkObject *caller,
                                                    long unsigned int eventId,
                                                    void *clientData,
                                                    void *callData)
{
  synchronizeCamera(2, caller, eventId, clientData, callData);
}

//--------------------------------------------------------------------------
// this is the callback function : do deep copies to keep track of
// master's camera position
void
QGoSynchronizedView3DCallbacks::synchronizeCameras3(vtkObject *caller,
                                                    long unsigned int eventId,
                                                    void *clientData,
                                                    void *callData)
{
  synchronizeCamera(3, caller, eventId, clientData, callData);
}

//--------------------------------------------------------------------------
void
QGoSynchronizedView3DCallbacks::synchronizeCamera(int iCamera,
                                                  vtkObject *caller,
                                                  long unsigned int eventId,
                                                  void *clientData,
                                                  void *callData)
{
  (void)eventId;
  (void)callData;

  // client data is a pointer to std::vector<QGoImageView2D*>
  // so client data is a std::vector<QGoSynchronizedView3D*>*
  // we get the p_m_QGoSynchronizedView3D array by the following cast :
  std::vector< QGoSynchronizedView3D * > p_m_QGoSynchronizedView3Ds =
    *static_cast< std::vector< QGoSynchronizedView3D * > * >( clientData );
  // the observer are set on cameras, so that the caller is a vtk camera*
  vtkCamera *movedCamera =
    static_cast< vtkCamera * >( caller );
  // for every opened SynchronizedView :
  std::vector< QGoSynchronizedView3D * >::iterator SynchronizedViewIt =
    p_m_QGoSynchronizedView3Ds.begin();

  while ( SynchronizedViewIt != p_m_QGoSynchronizedView3Ds.end() )
    {
    /*
     * To inspect : if we don't move even the non visible views,
     * the positionning is not correct when going back to quadview
     * solution => a reset camera taking reference at the last fullscreenview ?
     */
    /*
    // if the SynchronizedView is visible and the modified camera is rendrered
    if      ((*SynchronizedViewIt)->isVisible()
             &&  (((*SynchronizedViewIt)->GetFullScreenView() == 0)
                  ||  ((*SynchronizedViewIt)->GetFullScreenView() == 1)))
    */
      {
      // we copy the position of the moved camera
      // into each SynchronizedView's camera
      if  ( ( ( *SynchronizedViewIt )->GetCamera(iCamera) )
            &&  ( ( *SynchronizedViewIt )->GetCamera(iCamera) != movedCamera ) )
        {
        ( *SynchronizedViewIt )->GetCamera(iCamera)->DeepCopy(movedCamera);
        // we render all SynchronizedViews
        ( *SynchronizedViewIt )->Render(iCamera);
        }
      }
    ++SynchronizedViewIt;
    }
}

//--------------------------------------------------------------------------
void
QGoSynchronizedView3DCallbacks::SetupCallBack()
{
  // create the callback object (connection event -> function )
  for ( int i = 0; i < 4; i++ )
    {
    m_vtkCallBackCamSync[i] = vtkCallbackCommand::New();
    m_vtkCallBackCamSync[i]->SetClientData(&m_openSynchronizedView);
    }

  m_vtkCallBackCamSync[0]->SetCallback(QGoSynchronizedView3DCallbacks::
                                       synchronizeCameras0);
  m_vtkCallBackCamSync[1]->SetCallback(QGoSynchronizedView3DCallbacks::
                                       synchronizeCameras1);
  m_vtkCallBackCamSync[2]->SetCallback(QGoSynchronizedView3DCallbacks::
                                       synchronizeCameras2);
  m_vtkCallBackCamSync[3]->SetCallback(QGoSynchronizedView3DCallbacks::
                                       synchronizeCameras3);
}

//--------------------------------------------------------------------------
void
QGoSynchronizedView3DCallbacks::removeSynchronizedView(QGoSynchronizedView3D *ioSynchronizedView)
{
  if ( ioSynchronizedView ) // this should always be true
    {
    // We look for the SynchronizedView
    // in the vector of synchronized SynchronizedViews
    std::vector< QGoSynchronizedView3D * >::iterator
      SynchronizedViewIt = std::find(m_openSynchronizedView.begin(),
                                     m_openSynchronizedView.end(),
                                     ioSynchronizedView);
    if ( SynchronizedViewIt != m_openSynchronizedView.end() ) // if we found it
      {
      // remove the callback object from each object's camera
      for ( int i = 0; i < 4; i++ )
        {
        ioSynchronizedView->GetCamera(i)
        ->RemoveObserver(QGoSynchronizedView3DCallbacks::
                         m_vtkCallBackCamSync[i]);
        }

      // we remove the SynchronizedView
      m_openSynchronizedView.erase(SynchronizedViewIt);
      }
    }
}

//--------------------------------------------------------------------------
void
QGoSynchronizedView3DCallbacks::addSynchronizedView(QGoSynchronizedView3D *ioSynchronizedView)
{
  if ( ioSynchronizedView ) // this should always be true
    {
    if ( ioSynchronizedView->HasViewer() )
    // add the callback to the SynchronizedView's camera
      {
      m_openSynchronizedView.push_back(ioSynchronizedView);

      for ( int i = 0; i < 4; i++ )
        {
        ioSynchronizedView->GetCamera(i)
        ->AddObserver(vtkCommand::ModifiedEvent,
                      QGoSynchronizedView3DCallbacks::
                      m_vtkCallBackCamSync[i]);
        }

      // we connect the sliders to the synchronizer (monitor slider changes) :
      QObject::connect( ioSynchronizedView->GetImageView(),
                        SIGNAL( SliceViewXYChanged(int) ),
                        this,
                        SIGNAL( SliceViewXYChanged(int) ) );
      QObject::connect( ioSynchronizedView->GetImageView(),
                        SIGNAL( SliceViewXZChanged(int) ),
                        this,
                        SIGNAL( SliceViewXZChanged(int) ) );
      QObject::connect( ioSynchronizedView->GetImageView(),
                        SIGNAL( SliceViewYZChanged(int) ),
                        this,
                        SIGNAL( SliceViewYZChanged(int) ) );

      // we connect the synchronizer to the SynchronizedView
      QObject::connect( this, SIGNAL( SliceViewXYChanged(int) ),
                        ioSynchronizedView->GetImageView(),
                        SLOT( SetSliceViewXY(int) ) );
      QObject::connect( this, SIGNAL( SliceViewXZChanged(int) ),
                        ioSynchronizedView->GetImageView(),
                        SLOT( SetSliceViewXZ(int) ) );
      QObject::connect( this, SIGNAL( SliceViewYZChanged(int) ),
                        ioSynchronizedView->GetImageView(),
                        SLOT( SetSliceViewYZ(int) ) );
      }
    else
      {
      std::cerr << "trying to synchronize a visualization object missing a QGoImageView"
                << std::endl;
      }
    }
}