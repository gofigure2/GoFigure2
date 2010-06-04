#include "QGoComparer2D.h"

#include "QGoComparer2DSync.h"


#include "vtkImageData.h"
#include "vtkViewImage2D.h"
#include "vtkEventQtSlotConnect.h"

#include "SnapshotHelper.h"
#include "QGoImageView2D.h"
#include "QGoCompareOrchestra.h"



//--------------------------------------------------------------------------
/**
 * \brief Default Constructor.
 * \param iComparerName
 * \param iParent
 */
QGoComparer2D::QGoComparer2D( QString iComparerName, QWidget *iParent )
 : QGoComparer(iComparerName,iParent)
{
}

//--------------------------------------------------------------------------
/** \brief Print self informations
*/
void
QGoComparer2D::
PrintOs(ostream &os)
{
  // if we have an imageview, the we print its image information
  if (m_currentImage != NULL)
    {
    os << "Comparer 2D " << this << " contains :" << std::endl;
    m_currentImage->Print(os);
    }
  else
    {
    os << "Comparer 2D " << this << " contains no Image :"<< std::endl;
    }
}


//--------------------------------------------------------------------------
QGoComparer2D::
  ~QGoComparer2D()
{
  // remove the comparer from the orchestra
  if (m_currentOrchestra != NULL)
    {
    m_currentOrchestra->removeComparer2D(this);
    m_currentOrchestra = NULL;
    }
}


//--------------------------------------------------------------------------
/** \brief returns the type of comparer (2 for 2D, 3 for 3D)
*/
int
QGoComparer2D::
GetComparerType()
{
  return 2;
}


//--------------------------------------------------------------------------
// set the image to be displaid
void
QGoComparer2D::
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
QGoComparer2D::
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
    gridLayout->addWidget(static_cast<QGoImageView2D*>(m_currentView));
    }
}


//--------------------------------------------------------------------------
void
QGoComparer2D::
SetCurrentOrchestra(QGoCompareOrchestra* iCurrentOrchetra)
{
  m_currentOrchestra = iCurrentOrchetra;
}


//--------------------------------------------------------------------------
QGoImageView2D*
QGoComparer2D::
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
QGoComparer2D::
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
