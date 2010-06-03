#include "QGoComparer2D.h"

#include "QGoComparer2DSync.h"


#include "vtkImageData.h"
#include "vtkViewImage2D.h"
#include "vtkEventQtSlotConnect.h"

#include "SnapshotHelper.h"
#include "QGoImageView2D.h"
#include "QGoCompareOrchestra.h"



//--------------------------------------------------------------------------
QGoComparer2D::QGoComparer2D( QString iComparerName, QWidget *parent )
 : QGoComparer(iComparerName,parent)
{
}

//--------------------------------------------------------------------------
// print funtction
void
QGoComparer2D::
PrintOs(ostream &os)
{
// if we have an imageview, the we print its image information
if (m_currentImage != NULL)
  {
  os << "Comparer " << this << " contains :" << std::endl;
  m_currentImage->Print(os);
  }
else
    os << "Comparer " << this << " contains no Image :"<< std::endl;
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
// returns the type of comparer (2 for 2D, 3 for 3D)
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
    return;


  // if there is no viewer, we create one
  if (static_cast<QGoImageView2D*>(m_currentView) == NULL)
    createViewer();


  // set the image to the current view
  static_cast<QGoImageView2D*>(m_currentView)->SetImage(iImage);
  // update current image
  m_currentImage = iImage;

  this->Update();

}





/*
  Private
*/



//--------------------------------------------------------------------------
  // create the viewer contained in the widget
void
QGoComparer2D::
createViewer()
{
  // if there is already a viewer
  if (static_cast<QGoImageView2D*>(m_currentView) != NULL)
    return;

  // else we create one
  m_currentView = new QGoImageView2D(this);

  static_cast<QGoImageView2D*>(m_currentView)->setContentsMargins( 1, 1, 1, 1 );


  // setup position of the widget
  gridLayout->addWidget(static_cast<QGoImageView2D*>(m_currentView));


}

//--------------------------------------------------------------------------
// Set the address of the current orchestra
void
QGoComparer2D::
SetCurrentOrchestra(QGoCompareOrchestra* iCurrentOrchetra)
{

  m_currentOrchestra = iCurrentOrchetra;

}

//--------------------------------------------------------------------------
// returns the imageview contained in the Comparer
QGoImageView2D*
QGoComparer2D::
GetImageView()
{
if (HasViewer())
  return static_cast<QGoImageView2D*>(m_currentView);
else
  return NULL;
}



//--------------------------------------------------------------------------
// save a snapshot of the dsiplaid view
QString
QGoComparer2D::
SnapshotViewXY( const GoFigure::FileType& iType,
    const QString& iBaseName)
{

if (!HasViewer())
  return tr("");

return GetImageView()->SnapshotViewXY( iType, iBaseName );

}



