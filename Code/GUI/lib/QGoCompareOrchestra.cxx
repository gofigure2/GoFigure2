#include "QGoCompareOrchestra.h"

#include "QGoComparer2D.h"
#include "QGoComparer3D.h"
#include "QGoComparer2DSync.h"
#include "QGoComparer3DSync.h"


//--------------------------------------------------------------------------
QGoCompareOrchestra::QGoCompareOrchestra(QObject *parent) :
    QObject           (parent),
    m_Synchronizer    (NULL),
    m_Synchronizer3D  (NULL)
{}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------

QGoCompareOrchestra::
~QGoCompareOrchestra()
{
  std::vector<QGoComparer2D*>::iterator ComparerIt;
  std::vector<QGoComparer3D*>::iterator Comparer3DIt;
  QGoComparer2D*  tempComp;
  QGoComparer3D*  tempComp3D;

  // first we delete the synchronizer if any
  if (m_Synchronizer != NULL)
    delete(m_Synchronizer);
  if (m_Synchronizer3D != NULL)
    delete(m_Synchronizer3D);

  // and the we delete the comparers
  ComparerIt = m_openComparers.begin();
  while (!m_openComparers.empty())
    {
    // delete all pointers in the vector
    tempComp = *ComparerIt;
    m_openComparers.erase(ComparerIt);
    delete(tempComp);
    }

  Comparer3DIt = m_openComparers3D.begin();
  while (!m_openComparers3D.empty())
    {
    // delete all pointers in the vector
    tempComp3D = *Comparer3DIt;
    m_openComparers3D.erase(Comparer3DIt);
    delete(tempComp3D);
    }


}
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
// add a comparer to QGoCompareOrchestra's parent Object/Widget
QGoComparer2D*
QGoCompareOrchestra::
newComparer2D(QString iComparerName, vtkImageData* iImage)
{
  if ( (iComparerName.isEmpty()) || (iImage == NULL) )
    return NULL;
  // we create a new comparer with the same parent windows as this
  QGoComparer2D* newComparer = new QGoComparer2D(iComparerName,static_cast<QWidget*>(this->parent()) );

  // we add the vtkimage to it
  newComparer->SetImage(iImage);

  // we add the comparer to the list of open comparers
  m_openComparers.push_back(newComparer);

  // if we are synchronizing different comparers
  if (m_Synchronizer != NULL)
    // we tell the synchronizer to also synchronize the new comparer
    m_Synchronizer->addComparer(newComparer);


  // tell the comparer who is his Orchestra
  newComparer->SetCurrentOrchestra(this);

  return newComparer;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// add a comparer to QGoCompareOrchestra's parent Object/Widget
QGoComparer3D*
QGoCompareOrchestra::
newComparer3D(QString iComparerName, vtkImageData* iImage)
{
  if ( (iComparerName.isEmpty()) || (iImage == NULL) )
    return NULL;
  // we create a new comparer with the same parent windows as this
  QGoComparer3D* newComparer = new QGoComparer3D(iComparerName,static_cast<QWidget*>(this->parent()) );

  // we add the vtkimage to it
  newComparer->SetImage(iImage);

  // we add the comparer to the list of open comparers
  m_openComparers3D.push_back(newComparer);

  // if we are synchronizing different comparers
  if (m_Synchronizer3D != NULL)
    // we tell the synchronizer to also synchronize the new comparer
    m_Synchronizer3D->addComparer(newComparer);

  // tell the comparer who is his Orchestra
  newComparer->SetCurrentOrchestra(this);

  return newComparer;
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// delete a comparer from QGoCompareOrchestra's parent Object/Widget
void
QGoCompareOrchestra::
deleteComparer2D(QGoComparer2D* ioComparer)
{
  std::vector<QGoComparer2D*>::iterator ComparerIt;

  if ( ioComparer == NULL )
    return;


  // We look for the comparer in the vector of synchronized comparers
  ComparerIt = std::find( m_openComparers.begin(), m_openComparers.end(),
                            ioComparer );

  if (ComparerIt == m_openComparers.end())
    return;


    // if we are synchronizing different comparers
    if (m_Synchronizer != NULL)
      // we tell the synchronizer to remove the comparer
      m_Synchronizer->removeComparer(ioComparer);

    ioComparer->close();

    delete ioComparer;

    // we remove the comparer to the list of open comparers
    m_openComparers.erase(ComparerIt);

}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// delete a comparer from QGoCompareOrchestra's parent Object/Widget
void
QGoCompareOrchestra::
deleteComparer2D(const int& iIndex)
{
  QGoComparer2D* tempComp;

  // if there is a problem acessing the comparer
  if ( m_openComparers.at(iIndex) == NULL )
    return;


  tempComp = m_openComparers.at(iIndex);
  // if we are synchronizing different comparers
  if (m_Synchronizer != NULL)
    // we tell the synchronizer to remove the comparer
    m_Synchronizer->removeComparer(tempComp);


  tempComp->close();


  delete tempComp;

    // we remove the comparer to the list of open comparers
  m_openComparers.erase(m_openComparers.begin()+iIndex);

}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// take a comparer out of the Orchestra lists, without deleting it.
void
QGoCompareOrchestra::
removeComparer2D(QGoComparer2D* ioComparer)
{
  if ( ioComparer == NULL )
    return;

  std::vector<QGoComparer2D*>::iterator ComparerIt;

  // We look for the comparer in the vector of synchronized comparers
  ComparerIt = std::find( m_openComparers.begin(), m_openComparers.end(),
                            ioComparer );
  // if we don't find it
  if (ComparerIt == m_openComparers.end())
    return;



  // if we are synchronizing comparers
  if (m_Synchronizer != NULL)  // we tell the synchronizer to remove the comparer
    m_Synchronizer->removeComparer(ioComparer);

  ioComparer->close();


  // we remove the comparer to the list of open comparers
  m_openComparers.erase(ComparerIt);

}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// delete a comparer from QGoCompareOrchestra's parent Object/Widget
void
QGoCompareOrchestra::
deleteComparer3D(QGoComparer3D* ioComparer)
{
  std::vector<QGoComparer3D*>::iterator Comparer3DIt;

  if ( ioComparer == NULL )
    return;


  // We look for the comparer in the vector of synchronized comparers
  Comparer3DIt = std::find( m_openComparers3D.begin(), m_openComparers3D.end(),
                            ioComparer );
  // if we don't find it
  if (Comparer3DIt == m_openComparers3D.end())
    return;


  // if we are synchronizing comparers
  if (m_Synchronizer3D != NULL)
    // we tell the synchronizer to remove the comparer
    m_Synchronizer3D->removeComparer(ioComparer);

  ioComparer->close();

  delete ioComparer;

  // we remove the comparer fromo the list of open comparers
  m_openComparers3D.erase(Comparer3DIt);

}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// delete a comparer from QGoCompareOrchestra's parent Object/Widget
void
QGoCompareOrchestra::
deleteComparer3D(const int& iIndex)
{
  QGoComparer3D* tempComp3D;
  // if there is a problem acessing the comparer
  if ( m_openComparers3D.at(iIndex) == NULL )
    return;

  tempComp3D = m_openComparers3D.at(iIndex);

  // if we are synchronizing different comparers
  if (m_Synchronizer3D != NULL)
    // we tell the synchronizer to remove the comparer
    m_Synchronizer3D->removeComparer(tempComp3D);


  tempComp3D->close();


  delete tempComp3D;

    // we remove the comparer to the list of open comparers
  m_openComparers3D.erase(m_openComparers3D.begin()+iIndex);

}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
// take a comparer out of the Orchestra lists, without deleting it.
void
QGoCompareOrchestra::
removeComparer3D(QGoComparer3D* ioComparer)
{
  if ( ioComparer == NULL )
    return;

  std::vector<QGoComparer3D*>::iterator Comparer3DIt;

  // We look for the comparer in the vector of synchronized comparers
  Comparer3DIt = std::find( m_openComparers3D.begin(), m_openComparers3D.end(),
                            ioComparer );
  // if we don't find it
  if (Comparer3DIt == m_openComparers3D.end())
    return;


  // if we are synchronizing different comparers
  if (m_Synchronizer3D != NULL)
    // we tell the synchronizer to remove the comparer
    m_Synchronizer3D->removeComparer(ioComparer);

  ioComparer->close();


  // we remove the comparer to the list of open comparers
  m_openComparers3D.erase(Comparer3DIt);

}
//--------------------------------------------------------------------------



//--------------------------------------------------------------------------
// remove the synchronizer leaving the comparers independent
void
QGoCompareOrchestra::
unSynchronizeOpenComparers()
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
// synchronize all open comparers
void
QGoCompareOrchestra::
synchronizeOpenComparers()
{
  if ( m_Synchronizer == NULL )
      m_Synchronizer = new QGoComparer2DSync(m_openComparers);

  if ( m_Synchronizer3D == NULL )
      m_Synchronizer3D = new QGoComparer3DSync(m_openComparers3D);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// true if the orchestra synchronizes copmarers
bool
QGoCompareOrchestra::
isSynchronizing()
{
if ( ( m_Synchronizer != NULL ) ||  ( m_Synchronizer3D != NULL ) )
  return true;
else
  return false;
}

//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
// synchronize all open comparers
void
QGoCompareOrchestra::
Update()
{
  std::vector<QGoComparer2D*>::iterator ComparerIt;

  // we update all comparers
  for(ComparerIt = m_openComparers.begin();
        ComparerIt != m_openComparers.end();
        ComparerIt++)
    (*ComparerIt)->Update();

  std::vector<QGoComparer3D*>::iterator Comparer3DIt;

  // we update all comparers
  for(Comparer3DIt = m_openComparers3D.begin();
        Comparer3DIt != m_openComparers3D.end();
        Comparer3DIt++)
    (*Comparer3DIt)->Update();

}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//show all the comparers
void
QGoCompareOrchestra::
show()
{
  std::vector<QGoComparer2D*>::iterator ComparerIt;

  // we update all comparers
  for(ComparerIt = m_openComparers.begin();
        ComparerIt != m_openComparers.end();
        ComparerIt++)
    (*ComparerIt)->show();

  std::vector<QGoComparer3D*>::iterator Comparer3DIt;

  // we update all comparers
  for(Comparer3DIt = m_openComparers3D.begin();
        Comparer3DIt != m_openComparers3D.end();
        Comparer3DIt++)
    (*Comparer3DIt)->show();

}
//--------------------------------------------------------------------------
