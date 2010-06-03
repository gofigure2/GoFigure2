#include "QGoComparer3D.h"


#include "QGoComparer3DSync.h"


#include "vtkImageData.h"
#include "vtkViewImage3D.h"
#include "vtkViewImage2D.h"
#include "vtkEventQtSlotConnect.h"


#include "QGoImageView3D.h"
#include "QGoCompareOrchestra.h"

#include "SnapshotHelper.h"

//--------------------------------------------------------------------------
QGoComparer3D::QGoComparer3D( QString iComparerName, QWidget *parent )
 : QGoComparer( iComparerName, parent )
 {

 }

//--------------------------------------------------------------------------
QGoComparer3D::
  ~QGoComparer3D()
{

  // remove the comparer from the orchestra
  if (m_currentOrchestra != NULL)
    {
    m_currentOrchestra->removeComparer3D(this);
    m_currentOrchestra = NULL ;
    }

  // delete the view if any
  if (HasViewer())
    {
    // we delete the viewer
    delete(static_cast<QGoImageView3D*>(m_currentView));
    m_currentView = NULL;
    }

}


//--------------------------------------------------------------------------
int
QGoComparer3D::
// returns the type of comparer (2 for 2D, 3 for 3D)
GetComparerType()
{
  return 3;
}


//--------------------------------------------------------------------------
void
QGoComparer3D::
SetImage(vtkImageData* iImage)
{

  if (iImage == NULL)
    return;


  // if there is no viewer, we create one
  if (static_cast<QGoImageView3D*>(m_currentView) == NULL)
    createViewer();


  // set the image to the current view
  static_cast<QGoImageView3D*>(m_currentView)->SetImage(iImage);
  // update current image
  m_currentImage = iImage;

  this->Update();

}



//--------------------------------------------------------------------------
void
QGoComparer3D::
Render(const int& iId)
{
  if (static_cast<QGoImageView3D*>(m_currentView) == NULL)
    return;
 if (iId <= 2)
  static_cast<QGoImageView3D*>(m_currentView)->GetImageViewer(iId)->Render();
 else
  static_cast<QGoImageView3D*>(m_currentView)->GetImageViewer3D()->Render();
}

//--------------------------------------------------------------------------
// get the camera of the current viewer;
vtkCamera*
QGoComparer3D::
GetCamera(const int& iId)
{
  if (static_cast<QGoImageView3D*>(m_currentView) == NULL)
    return NULL;
 if (iId <= 2)
  return static_cast<QGoImageView3D*>(m_currentView)->GetImageViewer(iId)
                      ->GetRenderer()
                      ->GetActiveCamera();
 else
  return static_cast<QGoImageView3D*>(m_currentView)->GetImageViewer3D()
                      ->GetRenderer()
                      ->GetActiveCamera();

}





//--------------------------------------------------------------------------
int
QGoComparer3D::
GetFullScreenView()

{
if (HasViewer())
  return static_cast<QGoImageView3D*>(m_currentView)->GetFullScreenView();
else
  return -1;
}





//--------------------------------------------------------------------------
QGoImageView3D*
QGoComparer3D::
GetImageView()
{
  if (HasViewer())
    return static_cast<QGoImageView3D*>(m_currentView);
  else
    return NULL;
}



QString
QGoComparer3D::
SnapshotViewXY( const GoFigure::FileType& iType,
      const QString& iBaseName )
{
  return GetImageView()->SnapshotViewXY( iType, iBaseName);
}

QString
QGoComparer3D::
SnapshotView2( const GoFigure::FileType& iType,
    const QString& iBaseName)
{
  return GetImageView()->SnapshotView2( iType, iBaseName);
}

QString
QGoComparer3D::
SnapshotView3( const GoFigure::FileType& iType,
    const QString& iBaseName)
{
  return GetImageView()->SnapshotView3(iType,iBaseName);
}

QString
QGoComparer3D::
SnapshotViewXYZ( const GoFigure::FileType& iType,
    const QString& iBaseName)

{
  return GetImageView()->SnapshotViewXYZ(iType,iBaseName);
}

//--------------------------------------------------------------------------
void
QGoComparer3D::
SetFullScreenView(const int& iId)
{
if ( HasViewer() )
  {
  static_cast<QGoImageView3D*>(m_currentView)->SetFullScreenView(iId);
  }
}

//--------------------------------------------------------------------------
void
QGoComparer3D::
SetFullXYScreenView()
{
  SetFullScreenView(1);
}


//--------------------------------------------------------------------------
void
QGoComparer3D::
SetFullXZScreenView()
{
  SetFullScreenView(2);
}

//--------------------------------------------------------------------------
void
QGoComparer3D::
SetFullYZScreenView()
{
  SetFullScreenView(3);
}


//--------------------------------------------------------------------------
void
QGoComparer3D::
SetFullXYZScreenView()
{
  SetFullScreenView(4);
}

//--------------------------------------------------------------------------
void
QGoComparer3D::
SetQuadView()
{

  SetFullScreenView(0);

}


//--------------------------------------------------------------------------
void
QGoComparer3D::
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



/*
  Private
*/

/*
//--------------------------------------------------------------------------
// delete the viewer contained in the widget
void
QGoComparer3D::
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
*/

//--------------------------------------------------------------------------
  // create the viewer contained in the widget
void
QGoComparer3D::
createViewer()
{
  // if there is already a viewer
  if (static_cast<QGoImageView3D*>(m_currentView) != NULL)
    return;

  // else we create one
  m_currentView = new QGoImageView3D(this);

  static_cast<QGoImageView3D*>(m_currentView)->setContentsMargins( 1, 1, 1, 1 );


  // setup position of the widget
  gridLayout->addWidget(static_cast<QGoImageView3D*>(m_currentView) );


}


