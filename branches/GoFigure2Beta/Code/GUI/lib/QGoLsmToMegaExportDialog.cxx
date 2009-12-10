#include "QGoLsmToMegaExportDialog.h"

#include <QFileDialog>
#include <QProgressDialog>

#include "LSMToMegaCapture.h"

/**
 * \brief Constructor
 */
QGoLsmToMegaExportDialog::
QGoLsmToMegaExportDialog( QWidget* iParent) : QDialog( iParent ), m_LsmPath(""),
		m_LsmName(""), m_MegaPath(""), m_FileFormatIsPNG(true)
{
  this->setupUi( this );

  // Create connection signal/slot
  QObject::connect( this, SIGNAL( accepted() ),
      this, SLOT(OK_pressed() ) );
}

/**
 * \brief Desctructor
 */
QGoLsmToMegaExportDialog::
~QGoLsmToMegaExportDialog()
{
}

/**
 * \brief Get and print the name of the lsm file selected
 */
void
QGoLsmToMegaExportDialog::
on_selectLsmFile_clicked()
{
  QString filename = QFileDialog::getOpenFileName( this,
      tr( "Select the LSM file to convert" ), "file name", 0 );
  m_LsmPath = filename.toStdString();

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

  // Write the lsm file name in the dialog window
  lsmFileName->setText( QString(QString::fromLocal8Bit(m_LsmName.c_str())) );
}

/**
 * \brief Get and print the location to store MegaCapture file
 */
void
QGoLsmToMegaExportDialog::
on_selectMegaPath_clicked()
{
  QString filename = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
          "/home",QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  m_MegaPath = filename.toStdString();
  m_MegaPath += "/";

  megaFilePath->setText( QString(QString::fromLocal8Bit(m_MegaPath.c_str())) );
}

/**
 * \brief Select the output format
 */
void
QGoLsmToMegaExportDialog::
on_outputFormat_activated( int index )
{
  if ( index == 0 )
    {
    m_FileFormatIsPNG = true;
    }
  else
    {
    m_FileFormatIsPNG = false;
    }
}

/**
 * \brief Start conversion when OK pressed
 */
void
QGoLsmToMegaExportDialog::
OK_pressed()
{

  GoFigure::FileType filetype = GoFigure::PNG;

  if( !m_FileFormatIsPNG )
    {
    filetype = GoFigure::TIFF;
    }

  /*int numFiles = 100000;
  QProgressDialog progress("Copying files...", "Abort Copy", 0, numFiles, this);
  progress.setWindowModality(Qt::WindowModal);

  for (int i = 0; i < numFiles; i++)
    {
    progress.setValue(i);

    //if (progress.wasCanceled())
    //           break;
           //... copy one file
    }

  progress.setValue(numFiles);*/

  LSMToMegaCapture converter;
  converter.SetFileName( m_LsmPath );
  converter.SetOutputFileType( filetype );
  converter.Export( m_MegaPath );

}
