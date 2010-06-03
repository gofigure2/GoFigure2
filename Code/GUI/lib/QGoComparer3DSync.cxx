#include "QGoComparer3DSync.h"

#include "QGoImageView3D.h"

#include "QGoComparer3D.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"

#include <algorithm>

//--------------------------------------------------------------------------
QGoComparer3DSync::
QGoComparer3DSync(std::vector<QGoComparer3D*> ioOpenComparers,QObject* iParent):
    QObject           (iParent),
    m_openComparers   (ioOpenComparers)


{

  // create the callback object
  SetupCallBack();

  // for every opened comparer :
  for(std::vector<QGoComparer3D*>::iterator ComparerIt = m_openComparers.begin();
        ComparerIt != m_openComparers.end();
        ComparerIt++)
    {
      if ( (*ComparerIt)->HasViewer() )
        // add the callback to the comparer's camera
        {
        (*ComparerIt)->GetCamera(0)
                      ->AddObserver(
            vtkCommand::ModifiedEvent, QGoComparer3DSync::m_vtkCallBackCamSync0 );
        (*ComparerIt)->GetCamera(1)
                      ->AddObserver(
            vtkCommand::ModifiedEvent, QGoComparer3DSync::m_vtkCallBackCamSync1 );
        (*ComparerIt)->GetCamera(2)
                      ->AddObserver(
            vtkCommand::ModifiedEvent, QGoComparer3DSync::m_vtkCallBackCamSync2 );
        (*ComparerIt)->GetCamera(3)
                      ->AddObserver(
            vtkCommand::ModifiedEvent, QGoComparer3DSync::m_vtkCallBackCamSync3 );


        // we connect the sliders to the synchronizer (monitor slider changes) :
        QObject::connect( (*ComparerIt)->GetImageView(), SIGNAL( SliceViewXYChanged( int ) ),
        this, SIGNAL( SliceViewXYChanged( int ) ) );
        QObject::connect( (*ComparerIt)->GetImageView(), SIGNAL( SliceViewXZChanged( int ) ),
        this, SIGNAL( SliceViewXZChanged( int ) ) );
        QObject::connect( (*ComparerIt)->GetImageView(), SIGNAL( SliceViewYZChanged( int ) ),
        this, SIGNAL( SliceViewYZChanged( int ) ) );

        // we connect the synchronizer to the comparer
        QObject::connect( this, SIGNAL( SliceViewXYChanged( int ) ),
        (*ComparerIt)->GetImageView(), SLOT( SetSliceViewXY( int ) ) );
        QObject::connect( this, SIGNAL( SliceViewXZChanged( int ) ),
        (*ComparerIt)->GetImageView(), SLOT( SetSliceViewXZ( int ) ) );
        QObject::connect( this, SIGNAL( SliceViewYZChanged( int ) ),
        (*ComparerIt)->GetImageView(), SLOT( SetSliceViewYZ( int ) ) );
        }
    }

}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// the destructor is very important here, we want to leave clean comparers
// behind
QGoComparer3DSync::
~QGoComparer3DSync()
{
std::vector<QGoComparer3D*>::iterator ComparerIt;

  // we remove the open synchronized comparers
  ComparerIt = m_openComparers.begin();
  while (!m_openComparers.empty())
    {
    // remove (AND NOT DELETE, this is the Orchestra's business)
    // all pointers in the vector

    // We remove the observer if any
    if ( (*ComparerIt)->HasViewer() )
      {
      // remove the callback object from each object's camera
      (*ComparerIt)->GetCamera(0)
                    ->RemoveObserver( QGoComparer3DSync::m_vtkCallBackCamSync0 );
      (*ComparerIt)->GetCamera(1)
                    ->RemoveObserver( QGoComparer3DSync::m_vtkCallBackCamSync1 );
      (*ComparerIt)->GetCamera(2)
                    ->RemoveObserver( QGoComparer3DSync::m_vtkCallBackCamSync2 );
      (*ComparerIt)->GetCamera(3)
                    ->RemoveObserver( QGoComparer3DSync::m_vtkCallBackCamSync3 );
      }
    m_openComparers.erase(ComparerIt);
    }

  // we can now delete the callbacks !
  m_vtkCallBackCamSync0->Delete();
  m_vtkCallBackCamSync1->Delete();
  m_vtkCallBackCamSync2->Delete();
  m_vtkCallBackCamSync3->Delete();

}
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
// this is the callback function : do shallow copies to keep track of
// the moving camera's position
void
QGoComparer3DSync::
synchronizeCameras0( vtkObject* caller, long unsigned int eventId, void* clientData, void* callData )
{
  // client data is a pointer to std::vector<QGoImageView2D*>
  // so client data is a std::vector<QGoComparer3D*>*
  // we get the p_m_QGoComparer3D array by the following cast :
  std::vector<QGoComparer3D*> p_m_QGoComparer3Ds
                    = *static_cast< std::vector<QGoComparer3D*>* >(clientData);

  // the observer are aet on cameras, so that the caller is a vtk camera*
  vtkCamera* movedCamera
                    = static_cast< vtkCamera* >(caller);

  // for every opened comparer :
  for(std::vector<QGoComparer3D*>::iterator ComparerIt = p_m_QGoComparer3Ds.begin();
        ComparerIt != p_m_QGoComparer3Ds.end();
        ComparerIt++)
    {
    // if the comparer is visible and the modified camera is rendrered
    if      ( (*ComparerIt)->isVisible()
        &&  (   ((*ComparerIt)->GetFullScreenView() == 0)
            ||  ((*ComparerIt)->GetFullScreenView() == 1) )  )
      {
      // we copy the position of the moved camera into each comparer's camera
      if  ( ((*ComparerIt)->GetCamera(0) != NULL )
        &&  ((*ComparerIt)->GetCamera(0) != movedCamera) )
        {
        (*ComparerIt)->GetCamera(0)->ShallowCopy(movedCamera);
        // we render all comparers
        (*ComparerIt)->Render(0);
        }
      }
    }

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// this is the callback function : do shallow copies to keep track of
// master's camera position
void
QGoComparer3DSync::
synchronizeCameras1( vtkObject* caller, long unsigned int eventId, void* clientData, void* callData )
{
  // client data is a pointer to std::vector<QGoImageView2D*>
  // so client data is a std::vector<QGoComparer3D*>*
  // we get the p_m_QGoComparer3D array by the following cast :
  std::vector<QGoComparer3D*> p_m_QGoComparer3Ds
                    = *static_cast< std::vector<QGoComparer3D*>* >(clientData);

  // the observer are aet on cameras, so that the caller is a vtk camera*
  vtkCamera* movedCamera
                    = static_cast< vtkCamera* >(caller);

  // for every opened comparer :
  for(std::vector<QGoComparer3D*>::iterator ComparerIt = p_m_QGoComparer3Ds.begin();
        ComparerIt != p_m_QGoComparer3Ds.end();
        ComparerIt++)
    {
    // if the comparer is visible and the modified camera is rendrered
    if      ( (*ComparerIt)->isVisible()
        &&  (   ((*ComparerIt)->GetFullScreenView() == 0)
            ||  ((*ComparerIt)->GetFullScreenView() == 2) )  )
      {
      // we copy the position of the moved camera into each comparer's camera
      if  ( ((*ComparerIt)->GetCamera(1) != NULL )
        &&  ((*ComparerIt)->GetCamera(1) != movedCamera) )
        {
        (*ComparerIt)->GetCamera(1)->ShallowCopy(movedCamera);
        // we render all comparers
        (*ComparerIt)->Render(1);
        }
      }
    }

}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// this is the callback function : do shallow copies to keep track of
// master's camera position
void
QGoComparer3DSync::
synchronizeCameras2( vtkObject* caller, long unsigned int eventId, void* clientData, void* callData )
{
  // client data is a pointer to std::vector<QGoImageView2D*>
  // so client data is a std::vector<QGoComparer3D*>*
  // we get the p_m_QGoComparer3D array by the following cast :
  std::vector<QGoComparer3D*> p_m_QGoComparer3Ds
                    = *static_cast< std::vector<QGoComparer3D*>* >(clientData);

  // the observer are aet on cameras, so that the caller is a vtk camera*
  vtkCamera* movedCamera
                    = static_cast< vtkCamera* >(caller);

  // for every opened comparer :
  for(std::vector<QGoComparer3D*>::iterator ComparerIt = p_m_QGoComparer3Ds.begin();
        ComparerIt != p_m_QGoComparer3Ds.end();
        ComparerIt++)
    {
    // if the comparer is visible and the modified camera is rendrered
    if      ( (*ComparerIt)->isVisible()
        &&  (   ((*ComparerIt)->GetFullScreenView() == 0)
            ||  ((*ComparerIt)->GetFullScreenView() == 3) )  )
      {
      // we copy the position of the moved camera into each comparer's camera
      if  ( ((*ComparerIt)->GetCamera(2) != NULL )
        &&  ((*ComparerIt)->GetCamera(2) != movedCamera) )
        {
        (*ComparerIt)->GetCamera(2)->ShallowCopy(movedCamera);
        // we render all comparers
        (*ComparerIt)->Render(2);
        }
      }
    }

}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// this is the callback function : do shallow copies to keep track of
// master's camera position
void
QGoComparer3DSync::
synchronizeCameras3( vtkObject* caller, long unsigned int eventId, void* clientData, void* callData )
{
  // client data is a pointer to std::vector<QGoImageView2D*>
  // so client data is a std::vector<QGoComparer3D*>*
  // we get the p_m_QGoComparer3D array by the following cast :
  std::vector<QGoComparer3D*> p_m_QGoComparer3Ds
                    = *static_cast< std::vector<QGoComparer3D*>* >(clientData);

  // the observer are aet on cameras, so that the caller is a vtk camera*
  vtkCamera* movedCamera
                    = static_cast< vtkCamera* >(caller);

  // for every opened comparer :
  for(std::vector<QGoComparer3D*>::iterator ComparerIt = p_m_QGoComparer3Ds.begin();
        ComparerIt != p_m_QGoComparer3Ds.end();
        ComparerIt++)
    {
    // if the comparer is visible and the modified camera is rendrered
    if      ( (*ComparerIt)->isVisible()
        &&  (   ((*ComparerIt)->GetFullScreenView() == 0)
            ||  ((*ComparerIt)->GetFullScreenView() == 4) )  )
      {
      // we copy the position of the moved camera into each comparer's camera
      if  ( ((*ComparerIt)->GetCamera(3) != NULL )
        &&  ((*ComparerIt)->GetCamera(3) != movedCamera) )
        {
        (*ComparerIt)->GetCamera(3)->ShallowCopy(movedCamera);
        // we render all comparers
        (*ComparerIt)->Render(3);
        }
      }
    }

}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
void
QGoComparer3DSync::
SetupCallBack()
{
  // create the callback object (connection event -> function )
  // for the 1st 2D cut
  m_vtkCallBackCamSync0 = vtkCallbackCommand::New();
  m_vtkCallBackCamSync0->SetCallback(QGoComparer3DSync::synchronizeCameras0 );
  m_vtkCallBackCamSync0->SetClientData( &m_openComparers );
  // for the 2nd 2D cut
  m_vtkCallBackCamSync1 = vtkCallbackCommand::New();
  m_vtkCallBackCamSync1->SetCallback(QGoComparer3DSync::synchronizeCameras1 );
  m_vtkCallBackCamSync1->SetClientData( &m_openComparers );
  // for the 3rd 2D cut
  m_vtkCallBackCamSync2 = vtkCallbackCommand::New();
  m_vtkCallBackCamSync2->SetCallback(QGoComparer3DSync::synchronizeCameras2 );
  m_vtkCallBackCamSync2->SetClientData( &m_openComparers );
  // for the 3D view
  m_vtkCallBackCamSync3 = vtkCallbackCommand::New();
  m_vtkCallBackCamSync3->SetCallback(QGoComparer3DSync::synchronizeCameras3 );
  m_vtkCallBackCamSync3->SetClientData( &m_openComparers );
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
void
QGoComparer3DSync::
removeComparer( QGoComparer3D* ioComparer )
{
  if (ioComparer!=NULL) // this should always be true
    {
    // We look for the comparer in the vector of synchronized comparers
    std::vector<QGoComparer3D*>::iterator 
      ComparerIt = std::find( m_openComparers.begin(), 
                              m_openComparers.end(),
                              ioComparer );
    if (ComparerIt != m_openComparers.end()) // if we found it
      {
      // remove the callback object from each object's camera
      ioComparer->GetCamera(0)
                    ->RemoveObserver( QGoComparer3DSync::m_vtkCallBackCamSync0 );
      ioComparer->GetCamera(1)
                    ->RemoveObserver( QGoComparer3DSync::m_vtkCallBackCamSync1 );
      ioComparer->GetCamera(2)
                    ->RemoveObserver( QGoComparer3DSync::m_vtkCallBackCamSync2 );
      ioComparer->GetCamera(3)
                    ->RemoveObserver( QGoComparer3DSync::m_vtkCallBackCamSync3 );

      // we remove the comparer
      m_openComparers.erase(ComparerIt);
      }
    }
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void
QGoComparer3DSync::
addComparer( QGoComparer3D* ioComparer )
{
  if (ioComparer!=NULL) // this should always be true
    {
    m_openComparers.push_back(ioComparer);

    if ( ioComparer->HasViewer() )
      // add the callback to the comparer's camera
      {
        ioComparer->GetCamera(0)
                      ->AddObserver(
            vtkCommand::ModifiedEvent, QGoComparer3DSync::m_vtkCallBackCamSync0 );
        ioComparer->GetCamera(1)
                      ->AddObserver(
            vtkCommand::ModifiedEvent, QGoComparer3DSync::m_vtkCallBackCamSync1 );
        ioComparer->GetCamera(2)
                      ->AddObserver(
            vtkCommand::ModifiedEvent, QGoComparer3DSync::m_vtkCallBackCamSync2 );
        ioComparer->GetCamera(3)
                      ->AddObserver(
            vtkCommand::ModifiedEvent, QGoComparer3DSync::m_vtkCallBackCamSync3 );


        // we connect the sliders to the synchronizer (monitor slider changes) :
        QObject::connect( ioComparer->GetImageView(), SIGNAL( SliceViewXYChanged( int ) ),
        this, SIGNAL( SliceViewXYChanged( int ) ) );
        QObject::connect( ioComparer->GetImageView(), SIGNAL( SliceViewXZChanged( int ) ),
        this, SIGNAL( SliceViewXZChanged( int ) ) );
        QObject::connect( ioComparer->GetImageView(), SIGNAL( SliceViewYZChanged( int ) ),
        this, SIGNAL( SliceViewYZChanged( int ) ) );

        // we connect the synchronizer to the comparer
        QObject::connect( this, SIGNAL( SliceViewXYChanged( int ) ),
        ioComparer->GetImageView(), SLOT( SetSliceViewXY( int ) ) );
        QObject::connect( this, SIGNAL( SliceViewXZChanged( int ) ),
        ioComparer->GetImageView(), SLOT( SetSliceViewXZ( int ) ) );
        QObject::connect( this, SIGNAL( SliceViewYZChanged( int ) ),
        ioComparer->GetImageView(), SLOT( SetSliceViewYZ( int ) ) );


      }
    }
}
//--------------------------------------------------------------------------
