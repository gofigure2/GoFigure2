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

  QObject::connect( &ConversionLsmToMegaThreadSend, SIGNAL( ConversionTerminatedSent() ),
      this, SLOT( ConversionTerminatedReceived() ) );
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

  /// \todo check in QGoMainWindow about to get the extension of a file
  /// it will really simplify this code!!!
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
 * \brief Disable button and send thread to launch conversion
 */
void
QGoLsmToMegaExportDialog::
on_convert_clicked()
{
  // Disable everything
  this->lsmFileName->setEnabled(false);
  this->megaFilePath->setEnabled(false);
  this->selectLsmFile->setEnabled(false);
  this->selectMegaPath->setEnabled(false);
  this->outputFormat->setEnabled(false);
  this->buttonBox->setEnabled(false);
  this->convert->setEnabled(false);
  this->label->setEnabled(false);
  this->selectLSMLabel->setEnabled(false);
  this->megaFilePath_2->setEnabled(false);
  this->outputFormatLabel->setEnabled(false);
  this->label_2->setEnabled(false);

  this->convertLabel->setText( QString(QString::fromLocal8Bit("CONVERSION in PROGRESS"))  );


  ConversionLsmToMegaThreadSend.start();
}

/**
 * \brief Catch thread, proceed conversion and enable all buttons
 */
void
QGoLsmToMegaExportDialog::
ConversionTerminatedReceived()
{
  // Start conversion
  GoFigure::FileType filetype = GoFigure::PNG;

  if( !m_FileFormatIsPNG )
	{
	filetype = GoFigure::TIFF;
	}
  LSMToMegaCapture converter;
  converter.SetFileName( m_LsmPath );
  converter.SetOutputFileType( filetype );
  converter.Export( m_MegaPath );

  //Enable everything
  lsmFileName->setEnabled(true);
  megaFilePath->setEnabled(true);
  selectLsmFile->setEnabled(true);
  selectMegaPath->setEnabled(true);
  outputFormat->setEnabled(true);
  this->buttonBox->setEnabled(true);
  this->convert->setEnabled(true);
  this->label->setEnabled(true);
  this->selectLSMLabel->setEnabled(true);
  this->megaFilePath_2->setEnabled(true);
  this->outputFormatLabel->setEnabled(true);
  this->label_2->setEnabled(true);

  this->convertLabel->setText( QString(QString::fromLocal8Bit(""))  );
}

