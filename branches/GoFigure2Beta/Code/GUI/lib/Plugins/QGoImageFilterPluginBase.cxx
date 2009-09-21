#include "QGoImageFilterPluginBase.h"
#include "vtkImageData.h"

/**
 * \class QGoImageFilterPluginBase
 * \brief
*/
QGoImageFilterPluginBase::QGoImageFilterPluginBase()
{
  this->m_VTKInput = 0;
  this->m_VTKOutput = 0;

  this->SetAllRequiredInformation();
}

QGoImageFilterPluginBase::~QGoImageFilterPluginBase()
{}

void QGoImageFilterPluginBase::SetInput( vtkImageData* iInput )
{
  m_VTKInput = iInput;
}

vtkImageData* QGoImageFilterPluginBase::GetOutput()
{
  return m_VTKOutput;
}

QString QGoImageFilterPluginBase::Name() const
{
  return m_Information.Name;
}

QString QGoImageFilterPluginBase::Version() const
{
  return m_Information.Version;
}

QString QGoImageFilterPluginBase::GoFigureCompatibilityVersion() const
{
  return m_Information.GoFigureCompatibilityVersion;
}

QString QGoImageFilterPluginBase::Distributor() const
{
  return m_Information.Distributor;
}

QString QGoImageFilterPluginBase::Copyright() const
{
  return m_Information.Copyright;
}

QString QGoImageFilterPluginBase::License() const
{
  return m_Information.License;
}

QString QGoImageFilterPluginBase::Description() const
{
  return m_Information.Description;
}
