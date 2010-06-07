#include "QGoSynchronizedViewManager.h"

#include "QGoSynchronizedView2D.h"
#include "QGoSynchronizedView3D.h"
#include "QGoSynchronizedView2DCallbacks.h"
#include "QGoSynchronizedView3DCallbacks.h"


//--------------------------------------------------------------------------
QGoSynchronizedViewManager::QGoSynchronizedViewManager(QObject *parent) :
    QObject           (parent),
    m_Synchronizer    (NULL),
    m_Synchronizer3D  (NULL)
{}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

QGoSynchronizedViewManager::
~QGoSynchronizedViewManager()
{
  std::vector<QGoSynchronizedView2D*>::iterator SynchronizedViewIt;
  std::vector<QGoSynchronizedView3D*>::iterator SynchronizedView3DIt;
  QGoSynchronizedView2D*  tempComp;
  QGoSynchronizedView3D*  tempComp3D;

  // first we delete the synchronizer if any
  if (m_Synchronizer != NULL)
    delete(m_Synchronizer);
  if (m_Synchronizer3D != NULL)
    delete(m_Synchronizer3D);

  // and the we delete the SynchronizedViews
  SynchronizedViewIt = m_openSynchronizedViews.begin();
  while (!m_openSynchronizedViews.empty())
    {
    // delete all pointers in the vector
    tempComp = *SynchronizedViewIt;
    m_openSynchronizedViews.erase(SynchronizedViewIt);
    delete(tempComp);
    }

  SynchronizedView3DIt = m_openSynchronizedViews3D.begin();
  while (!m_openSynchronizedViews3D.empty())
    {
    // delete all pointers in the vector
    tempComp3D = *SynchronizedView3DIt;
    m_openSynchronizedViews3D.erase(SynchronizedView3DIt);
    delete(tempComp3D);
    }


}
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
// add a SynchronizedView to QGoSynchronizedViewManager's parent Object/Widget
QGoSynchronizedView2D*
QGoSynchronizedViewManager::
newSynchronizedView2D(QString iSynchronizedViewName, vtkImageData* iImage)
{
  if ( (iSynchronizedViewName.isEmpty()) || (iImage == NULL) )
    return NULL;
  // we create a new SynchronizedView with the same parent windows as this
  QGoSynchronizedView2D* newSynchronizedView = new QGoSynchronizedView2D(iSynchronizedViewName,static_cast<QWidget*>(this->parent()) );

  // we add the vtkimage to it
  newSynchronizedView->SetImage(iImage);

  // we add the SynchronizedView to the list of open SynchronizedViews
  m_openSynchronizedViews.push_back(newSynchronizedView);

  // if we are synchronizing different SynchronizedViews
  if (m_Synchronizer != NULL)
    // we tell the synchronizer to also synchronize the new SynchronizedView
    m_Synchronizer->addSynchronizedView(newSynchronizedView);


  // tell the SynchronizedView who is his Manager
  newSynchronizedView->SetCurrentViewManager(this);

  return newSynchronizedView;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// add a SynchronizedView to QGoSynchronizedViewManager's parent Object/Widget
QGoSynchronizedView3D*
QGoSynchronizedViewManager::
newSynchronizedView3D(QString iSynchronizedViewName, vtkImageData* iImage)
{
  if ( (iSynchronizedViewName.isEmpty()) || (iImage == NULL) )
    return NULL;
  // we create a new SynchronizedView with the same parent windows as this
  QGoSynchronizedView3D* newSynchronizedView = new QGoSynchronizedView3D(iSynchronizedViewName,static_cast<QWidget*>(this->parent()) );

  // we add the vtkimage to it
  newSynchronizedView->SetImage(iImage);

  // we add the SynchronizedView to the list of open SynchronizedViews
  m_openSynchronizedViews3D.push_back(newSynchronizedView);

  // if we are synchronizing different SynchronizedViews
  if (m_Synchronizer3D != NULL)
    // we tell the synchronizer to also synchronize the new SynchronizedView
    m_Synchronizer3D->addSynchronizedView(newSynchronizedView);

  // tell the SynchronizedView who is his Manager
  newSynchronizedView->SetCurrentViewManager(this);

  return newSynchronizedView;
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// delete a SynchronizedView from QGoSynchronizedViewManager's parent Object/Widget
void
QGoSynchronizedViewManager::
deleteSynchronizedView2D(QGoSynchronizedView2D* ioSynchronizedView)
{
  std::vector<QGoSynchronizedView2D*>::iterator SynchronizedViewIt;

  if ( ioSynchronizedView == NULL )
    return;


  // We look for the SynchronizedView in the vector of synchronized SynchronizedViews
  SynchronizedViewIt = std::find( m_openSynchronizedViews.begin(), m_openSynchronizedViews.end(),
                            ioSynchronizedView );

  if (SynchronizedViewIt == m_openSynchronizedViews.end())
    return;


    // if we are synchronizing different SynchronizedViews
    if (m_Synchronizer != NULL)
      // we tell the synchronizer to remove the SynchronizedView
      m_Synchronizer->removeSynchronizedView(ioSynchronizedView);

    ioSynchronizedView->close();

    delete ioSynchronizedView;

    // we remove the SynchronizedView to the list of open SynchronizedViews
    m_openSynchronizedViews.erase(SynchronizedViewIt);

}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// delete a SynchronizedView from QGoSynchronizedViewManager's parent Object/Widget
void
QGoSynchronizedViewManager::
deleteSynchronizedView2D(const int& iIndex)
{
  QGoSynchronizedView2D* tempComp;

  // if there is a problem acessing the SynchronizedView
  if ( m_openSynchronizedViews.at(iIndex) == NULL )
    return;


  tempComp = m_openSynchronizedViews.at(iIndex);
  // if we are synchronizing different SynchronizedViews
  if (m_Synchronizer != NULL)
    // we tell the synchronizer to remove the SynchronizedView
    m_Synchronizer->removeSynchronizedView(tempComp);


  tempComp->close();


  delete tempComp;

    // we remove the SynchronizedView to the list of open SynchronizedViews
  m_openSynchronizedViews.erase(m_openSynchronizedViews.begin()+iIndex);

}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// take a SynchronizedView out of the Manager lists, without deleting it.
void
QGoSynchronizedViewManager::
removeSynchronizedView2D(QGoSynchronizedView2D* ioSynchronizedView)
{
  if ( ioSynchronizedView == NULL )
    return;

  std::vector<QGoSynchronizedView2D*>::iterator SynchronizedViewIt;

  // We look for the SynchronizedView in the vector of synchronized SynchronizedViews
  SynchronizedViewIt = std::find( m_openSynchronizedViews.begin(), m_openSynchronizedViews.end(),
                            ioSynchronizedView );
  // if we don't find it
  if (SynchronizedViewIt == m_openSynchronizedViews.end())
    return;



  // if we are synchronizing SynchronizedViews
  if (m_Synchronizer != NULL)  // we tell the synchronizer to remove the SynchronizedView
    m_Synchronizer->removeSynchronizedView(ioSynchronizedView);

  ioSynchronizedView->close();


  // we remove the SynchronizedView to the list of open SynchronizedViews
  m_openSynchronizedViews.erase(SynchronizedViewIt);

}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// delete a SynchronizedView from QGoSynchronizedViewManager's parent Object/Widget
void
QGoSynchronizedViewManager::
deleteSynchronizedView3D(QGoSynchronizedView3D* ioSynchronizedView)
{
  std::vector<QGoSynchronizedView3D*>::iterator SynchronizedView3DIt;

  if ( ioSynchronizedView == NULL )
    return;


  // We look for the SynchronizedView in the vector of synchronized SynchronizedViews
  SynchronizedView3DIt = std::find( m_openSynchronizedViews3D.begin(), m_openSynchronizedViews3D.end(),
                            ioSynchronizedView );
  // if we don't find it
  if (SynchronizedView3DIt == m_openSynchronizedViews3D.end())
    return;


  // if we are synchronizing SynchronizedViews
  if (m_Synchronizer3D != NULL)
    // we tell the synchronizer to remove the SynchronizedView
    m_Synchronizer3D->removeSynchronizedView(ioSynchronizedView);

  ioSynchronizedView->close();

  delete ioSynchronizedView;

  // we remove the SynchronizedView fromo the list of open SynchronizedViews
  m_openSynchronizedViews3D.erase(SynchronizedView3DIt);

}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// delete a SynchronizedView from QGoSynchronizedViewManager's parent Object/Widget
void
QGoSynchronizedViewManager::
deleteSynchronizedView3D(const int& iIndex)
{
  QGoSynchronizedView3D* tempComp3D;
  // if there is a problem acessing the SynchronizedView
  if ( m_openSynchronizedViews3D.at(iIndex) == NULL )
    return;

  tempComp3D = m_openSynchronizedViews3D.at(iIndex);

  // if we are synchronizing different SynchronizedViews
  if (m_Synchronizer3D != NULL)
    // we tell the synchronizer to remove the SynchronizedView
    m_Synchronizer3D->removeSynchronizedView(tempComp3D);


  tempComp3D->close();


  delete tempComp3D;

    // we remove the SynchronizedView to the list of open SynchronizedViews
  m_openSynchronizedViews3D.erase(m_openSynchronizedViews3D.begin()+iIndex);

}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// take a SynchronizedView out of the Manager lists, without deleting it.
void
QGoSynchronizedViewManager::
removeSynchronizedView3D(QGoSynchronizedView3D* ioSynchronizedView)
{
  if ( ioSynchronizedView == NULL )
    return;

  std::vector<QGoSynchronizedView3D*>::iterator SynchronizedView3DIt;

  // We look for the SynchronizedView in the vector of synchronized SynchronizedViews
  SynchronizedView3DIt = std::find( m_openSynchronizedViews3D.begin(), m_openSynchronizedViews3D.end(),
                            ioSynchronizedView );
  // if we don't find it
  if (SynchronizedView3DIt == m_openSynchronizedViews3D.end())
    return;


  // if we are synchronizing different SynchronizedViews
  if (m_Synchronizer3D != NULL)
    // we tell the synchronizer to remove the SynchronizedView
    m_Synchronizer3D->removeSynchronizedView(ioSynchronizedView);

  ioSynchronizedView->close();


  // we remove the SynchronizedView to the list of open SynchronizedViews
  m_openSynchronizedViews3D.erase(SynchronizedView3DIt);

}
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
// remove the synchronizer leaving the SynchronizedViews independent
void
QGoSynchronizedViewManager::
unSynchronizeOpenSynchronizedViews()
{
  if ( m_Synchronizer != NULL)
    delete(m_Synchronizer);

  if ( m_Synchronizer3D != NULL)
    delete(m_Synchronizer3D);

  m_Synchronizer = NULL;
  m_Synchronizer3D = NULL;

}
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
// synchronize all open SynchronizedViews
void
QGoSynchronizedViewManager::
synchronizeOpenSynchronizedViews()
{
  if ( m_Synchronizer == NULL )
      m_Synchronizer = new QGoSynchronizedView2DCallbacks(m_openSynchronizedViews);

  if ( m_Synchronizer3D == NULL )
      m_Synchronizer3D = new QGoSynchronizedView3DCallbacks(m_openSynchronizedViews3D);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// true if the orchestra synchronizes copmarers
bool
QGoSynchronizedViewManager::
isSynchronizing()
{
if ( ( m_Synchronizer != NULL ) ||  ( m_Synchronizer3D != NULL ) )
  return true;
else
  return false;
}

//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// synchronize all open SynchronizedViews
void
QGoSynchronizedViewManager::
Update()
{
  std::vector<QGoSynchronizedView2D*>::iterator SynchronizedViewIt;

  // we update all SynchronizedViews
  for(SynchronizedViewIt = m_openSynchronizedViews.begin();
        SynchronizedViewIt != m_openSynchronizedViews.end();
        SynchronizedViewIt++)
    (*SynchronizedViewIt)->Update();

  std::vector<QGoSynchronizedView3D*>::iterator SynchronizedView3DIt;

  // we update all SynchronizedViews
  for(SynchronizedView3DIt = m_openSynchronizedViews3D.begin();
        SynchronizedView3DIt != m_openSynchronizedViews3D.end();
        SynchronizedView3DIt++)
    (*SynchronizedView3DIt)->Update();

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//show all the SynchronizedViews
void
QGoSynchronizedViewManager::
show()
{
  std::vector<QGoSynchronizedView2D*>::iterator SynchronizedViewIt;

  // we update all SynchronizedViews
  for(SynchronizedViewIt = m_openSynchronizedViews.begin();
        SynchronizedViewIt != m_openSynchronizedViews.end();
        SynchronizedViewIt++)
    (*SynchronizedViewIt)->show();

  std::vector<QGoSynchronizedView3D*>::iterator SynchronizedView3DIt;

  // we update all SynchronizedViews
  for(SynchronizedView3DIt = m_openSynchronizedViews3D.begin();
        SynchronizedView3DIt != m_openSynchronizedViews3D.end();
        SynchronizedView3DIt++)
    (*SynchronizedView3DIt)->show();

}
//--------------------------------------------------------------------------
