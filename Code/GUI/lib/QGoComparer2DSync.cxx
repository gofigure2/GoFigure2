/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
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
#include "QGoComparer2DSync.h"
#include "QGoComparer.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"

#include <algorithm>

//--------------------------------------------------------------------------
QGoComparer2DSync::QGoComparer2DSync(std::vector<QGoComparer2D*> ioOpenComparers)
:   m_openComparers  (ioOpenComparers)
{
  // create the callback object
  SetupCallBack();

  // for every opened comparer :
  for(std::vector<QGoComparer2D*>::iterator ComparerIt = m_openComparers.begin();
        ComparerIt != m_openComparers.end();
        ComparerIt++)
    {
    // add the callback object as an observer of each comparer's camera
    (*ComparerIt)->GetCamera()
                  ->AddObserver(
                  vtkCommand::ModifiedEvent, QGoComparer2DSync::m_vtkCallBackCamSync );
    }
}


//--------------------------------------------------------------------------
/** the destructor is very important here, we want to leave clean comparers
*  behind
*/
QGoComparer2DSync::
~QGoComparer2DSync()
{
  std::vector<QGoComparer2D*>::iterator ComparerIt;
  // we remove the open synchronized comparers
  ComparerIt = m_openComparers.begin();

  while (!m_openComparers.empty())
    {
    // remove (AND NOT DELETE, this is the Orchestra's business)
    // all pointers in the vector

    // We remove the observer if any
    if ( (*ComparerIt)->HasViewer() )
      // remove the callback object from each object's camera
      (*ComparerIt)->GetCamera()
                    ->RemoveObserver( QGoComparer2DSync::m_vtkCallBackCamSync );
    // we remove the comparer from the vector
    m_openComparers.erase(ComparerIt);
    }

  // we can now delete the callback !
  m_vtkCallBackCamSync->Delete();
}


//--------------------------------------------------------------------------
/** this is the callback function : do shallow copies to keep track of
*  master's camera position
*/
void
QGoComparer2DSync::
synchronizeCameras( vtkObject* caller, long unsigned int eventId, void* clientData, void* callData )
{
  // client data is a pointer to std::vector<QGoImageView2D*>
  // so client data is a std::vector<QGoComparer2D*>*
  // we get the p_m_QGoComparer2D array by the following cast :
  std::vector<QGoComparer2D*> p_m_QGoComparers
                    = *static_cast< std::vector<QGoComparer2D*>* >(clientData);
  // the observer are set on cameras, so that the caller is a vtk camera*
  vtkCamera* movedCamera
                    = static_cast< vtkCamera* >(caller);
  // for every opened comparer :
  for(std::vector<QGoComparer2D*>::iterator ComparerIt = p_m_QGoComparers.begin();
        ComparerIt != p_m_QGoComparers.end();
        ++ComparerIt)
    {
    // we copy the position of the moved camera into each comparer's camera
    if  ( ((*ComparerIt)->GetCamera() != NULL )
      &&  ((*ComparerIt)->GetCamera() != movedCamera) )
      {
      (*ComparerIt)->GetCamera()->ShallowCopy(movedCamera);
      // we render all comparers
      (*ComparerIt)->Render();
      }
    }
}


//--------------------------------------------------------------------------
void
QGoComparer2DSync::
SetupCallBack()
{
  // create the callback object (connection event -> function )
  m_vtkCallBackCamSync = vtkCallbackCommand::New();
  m_vtkCallBackCamSync->SetCallback(QGoComparer2DSync::synchronizeCameras );
  m_vtkCallBackCamSync->SetClientData( &m_openComparers );
}


//--------------------------------------------------------------------------
void
QGoComparer2DSync::
removeComparer( QGoComparer2D* ioComparer )
{
  std::vector<QGoComparer2D*>::iterator ComparerIt;

  if (ioComparer!=NULL) // this should always be true
    {
    // We look for the comparer in the vector of synchronized comparers
    ComparerIt = std::find( m_openComparers.begin(), m_openComparers.end(),
                            ioComparer );
    if (ComparerIt != m_openComparers.end()) // if we found it
      {
      if ( ioComparer->HasViewer() )
        // remove the callback object from each object's camera
        ioComparer->GetCamera()
                  ->RemoveObserver( QGoComparer2DSync::m_vtkCallBackCamSync );
      (*ComparerIt)= NULL;
      // we remove the comparer
      m_openComparers.erase(ComparerIt);
      }
    }
}


//--------------------------------------------------------------------------
void
QGoComparer2DSync::
addComparer( QGoComparer2D* ioComparer )
{

  if (ioComparer!=NULL) // this should always be true
    {
    m_openComparers.push_back(ioComparer);
    // if this comparer has a viewer, we add an observer
    if ( ioComparer->HasViewer() )
      // add the callback to the comparer's camera
      ioComparer->GetCamera()
                ->AddObserver(
            vtkCommand::ModifiedEvent, QGoComparer2DSync::m_vtkCallBackCamSync );
    else
      {
      std::cerr <<"trying to synchronize a visualization object missing a QGoImageView"
                << std::endl;
      }
    }
}



