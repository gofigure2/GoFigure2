#include "QGoLsmToMegaExportDialog.h"

#include <QFileDialog>

QGoLsmToMegaExportDialog::
QGoLsmToMegaExportDialog( QWidget* iParent) : QDialog( iParent )
{
  this->setupUi( this );
}

QGoLsmToMegaExportDialog::
~QGoLsmToMegaExportDialog()
{
}

void
QGoLsmToMegaExportDialog::
on_selectLsmFile_clicked()
{
  QString filename = QFileDialog::getOpenFileName( this,
      tr( "Select the LSM file to convert" ), "file name", 0 );
  m_LsmPath = filename.toStdString();
  std::cout<< "m_LsmPath: " << m_LsmPath << std::endl;

  // Extract the name of the file from the full path
  // From LSMToMegaCapture.cxx

  size_t point_idx = m_LsmPath.rfind( ".lsm" );

  if( point_idx != std::string::npos )
    {
    size_t slash_idx = m_LsmPath.rfind( '/' );
    if( point_idx != std::string::npos )
      {
      m_LsmName = m_LsmPath.substr( slash_idx + 1, point_idx + 3 - slash_idx );
      }
    else
      {
      slash_idx = m_LsmPath.rfind( "\\" );
      if( point_idx != std::string::npos )
        {
    	m_LsmName = m_LsmPath.substr( slash_idx + 1, point_idx + 3 - slash_idx );
        }
      else
        {
    	m_LsmName = m_LsmPath.substr( 0, point_idx + 3);
        }
      }
    }

  std::cout<< "m_LsmName: " << m_LsmName << std::endl;

  // Write the lsm file name in the dialog window
  lsmFileName->setText( QString(QString::fromLocal8Bit(m_LsmName.c_str())) );
}

void
QGoLsmToMegaExportDialog::
on_selectMegaPath_clicked()
{
  QString filename = QFileDialog::getSaveFileName( this,
      tr( "Select the folder to store the Mega file" ), "path", 0 );
  m_MegaPath = filename.toStdString();
  m_MegaPath += "/";

  megaFilePath->setText( QString(QString::fromLocal8Bit(m_MegaPath.c_str())) );
}
