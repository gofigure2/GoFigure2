#include "QGoComparer.h"




#include "vtkImageData.h"
#include "vtkViewImage2D.h"
#include "vtkEventQtSlotConnect.h"

#include "SnapshotHelper.h"
#include "QGoImageView2D.h"
#include "QGoCompareOrchestra.h"



//--------------------------------------------------------------------------
QGoComparer::QGoComparer( QString iComparerName, QWidget *parent )
 : QWidget                  (parent),
    m_currentComparerName   (iComparerName),
    m_currentImage          (NULL),
    m_currentView           (NULL),
    m_currentOrchestra      (NULL)
{
    setupUi(this);

    // the widget View is just for representing the place of the viewer
    // it is useless

    delete (View);

  gridLayout->setContentsMargins(1,1,1,1);
  gridLayout->setSpacing(1);
  //gridLayout->setRowStretch(0,0);
  //gridLayout->setRowStretch(1,10);


  this->setWindowTitle(iComparerName);

  this->resize(300,300);

}


//--------------------------------------------------------------------------
QGoComparer::
  ~QGoComparer()
{



  // delete the view if any
  if (HasViewer())
    {
    // we delete the viewer
    delete(m_currentView);
    m_currentView = NULL;
    }
}




//--------------------------------------------------------------------------
void QGoComparer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);

    switch (e->type())
      {
    case QEvent::LanguageChange:
        retranslateUi(this);
        break;
    default:
        break;
      }

}


//--------------------------------------------------------------------------
void
QGoComparer::
SetImage(vtkImageData* iImage)
{

  if (iImage == NULL)
    return;


  // if there is no viewer, we create one
  if (m_currentView == NULL)
    createViewer();


  // set the image to the current view
  m_currentView->SetImage(iImage);
  // update current image
  m_currentImage = iImage;

  this->Update();

}




//--------------------------------------------------------------------------
void
QGoComparer::
Update()
{
  if (m_currentView == NULL)
    return;

  m_currentView->Update();

}


//--------------------------------------------------------------------------
void
QGoComparer::
Render()
{
  if (m_currentView == NULL)
    return;

  m_currentView->GetImageViewer(0)->Render();

}


//--------------------------------------------------------------------------
// get the camera of the current viewer;
vtkCamera*
QGoComparer::
GetCamera()
{
  if (m_currentView == NULL)
    return NULL;

  return m_currentView->GetImageViewer(0)
                      ->GetRenderer()
                      ->GetActiveCamera();
}


//--------------------------------------------------------------------------
// get the name of the comparer
QString*
QGoComparer::
GetName()
{
  return &m_currentComparerName;
}

    QString* GetName();
//--------------------------------------------------------------------------
// true if the widget has a viewer
bool
QGoComparer::
HasViewer()
{
  if (m_currentView != NULL)
    return true;
  else
    return false;
}




/*
  Private
*/


//--------------------------------------------------------------------------
// delete the viewer contained in the widget
void
QGoComparer::
deleteViewer()
{
  // if there is no viewer
  if (m_currentView == NULL)
    return;

  // delete object
  delete (m_currentView);
  // set pointer to NULL
  m_currentView = NULL;


}




//--------------------------------------------------------------------------
/** Set the address of the current orchestra
*/
void
QGoComparer::
SetCurrentOrchestra(QGoCompareOrchestra* iCurrentOrchetra)
{

  m_currentOrchestra = iCurrentOrchetra;

}



