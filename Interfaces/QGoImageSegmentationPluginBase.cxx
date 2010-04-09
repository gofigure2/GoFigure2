#include "QGoImageSegmentationPluginBase.h"
#include "vtkImageData.h"

#include <QMessageBox>

/**
 * \class QGoImageSegmentationPluginBase
 * \brief
*/
QGoImageSegmentationPluginBase::
QGoImageSegmentationPluginBase() : m_VTKInput( 0 ), m_Output( 0 )
{
}

QGoImageSegmentationPluginBase::~QGoImageSegmentationPluginBase()
{}

void QGoImageSegmentationPluginBase::SetInput( vtkImageData* iInput )
{
  QMessageBox::information( 0, "Title", "QGoImageSegmentationPluginBase::setInput" );
  m_VTKInput = iInput;
}

void QGoImageSegmentationPluginBase::Update()
{
  if( m_VTKInput )
    {
    this->Process();
    }
}

CellVectorType QGoImageSegmentationPluginBase::GetOutput()
{
  return m_Output;
}


