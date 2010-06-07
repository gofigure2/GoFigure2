#include "QGoSynchronizedView2D.h"

#include "QGoSynchronizedView2DCallbacks.h"


#include "vtkImageData.h"
#include "vtkViewImage2D.h"
#include "vtkEventQtSlotConnect.h"

#include "SnapshotHelper.h"
#include "QGoImageView2D.h"
#include "QGoSynchronizedViewManager.h"



//--------------------------------------------------------------------------
/**
 * \brief Default Constructor.
 * \param iSynchronizedViewName
 * \param iParent
 */
QGoSynchronizedView2D::QGoSynchronizedView2D( QString iViewName, QWidget *iParent )
 : QGoSynchronizedView( iViewName,iParent )
{
}

//--------------------------------------------------------------------------
/** \brief Print self informations
*/
void
QGoSynchronizedView2D::
PrintOs(ostream &os)
{
  // if we have an imageview, the we print its image information
  if (m_currentImage != NULL)
    {
    os << "SynchronizedView 2D " << this << " contains :" << std::endl;
    m_currentImage->Print(os);
    }
  else
    {
    os << "SynchronizedView 2D " << this << " contains no Image :"<< std::endl;
    }
}


//--------------------------------------------------------------------------
QGoSynchronizedView2D::
  ~QGoSynchronizedView2D()
{
  // remove the comparer from the orchestra
  if (m_currentViewManager != NULL)
    {
    m_currentViewManager->removeSynchronizedView2D(this);
    m_currentViewManager = NULL;
    }
}


//--------------------------------------------------------------------------
/** \brief returns the type of comparer (2 for 2D, 3 for 3D)
*/
int
QGoSynchronizedView2D::
GetSynchronizedViewType()
{
  return 2;
}


//--------------------------------------------------------------------------
// set the image to be displaid
void
QGoSynchronizedView2D::
SetImage(vtkImageData* iImage)
{
  if (iImage == NULL)
    {
    return;
    }
  else
    {
    // if there is no viewer, we create one
    if (static_cast<QGoImageView2D*>(m_currentView) == NULL)
    createViewer();

    // set the image to the current view
    static_cast<QGoImageView2D*>(m_currentView)->SetImage(iImage);
    // update current image
    m_currentImage = iImage;

    this->Update();
    }
}


/*
  Private
*/


//--------------------------------------------------------------------------
/** \brief create the viewer contained in the widget
*/
void
QGoSynchronizedView2D::
createViewer()
{
  // if there is already a viewer
  if (static_cast<QGoImageView2D*>(m_currentView) != NULL)
    {
    return;
    }
  else
    {
    // else we create one
    m_currentView = new QGoImageView2D(this);
    static_cast<QGoImageView2D*>
      (m_currentView)->setContentsMargins( 1, 1, 1, 1 );
    // setup position of the widget
    this->gridLayout->addWidget(static_cast<QGoImageView2D*>(m_currentView));
    }
}


//--------------------------------------------------------------------------
QGoImageView2D*
QGoSynchronizedView2D::
GetImageView()
{
  if ( HasViewer() )
    {
    return static_cast<QGoImageView2D*>(m_currentView);
    }
  else
    {
    return NULL;
    }
}


//--------------------------------------------------------------------------
QString
QGoSynchronizedView2D::
SnapshotViewXY( const GoFigure::FileType& iType,
    const QString& iBaseName)
{
  if (!HasViewer())
    {
    return tr("");
    }
  else
    {
    return GetImageView()->SnapshotViewXY( iType, iBaseName );
    }
}
