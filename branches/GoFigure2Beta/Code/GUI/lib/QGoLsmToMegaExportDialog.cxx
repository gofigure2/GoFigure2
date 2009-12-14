#include "QGoLsmToMegaExportDialog.h"

#include <QFileDialog>
#include <QProgressDialog>

/**
 * \brief Constructor
 */
QGoLsmToMegaExportDialog::
QGoLsmToMegaExportDialog( QWidget* iParent) : QDialog( iParent ), m_LsmPath(""),
		m_LsmName(""), m_MegaPath(""), m_FileFormatIsPNG(true), m_ProgressDialog(NULL),
		m_Counter(0)
{
  this->setupUi( this );

  m_ProgressDialog = new QProgressDialog("Conversion in progress.", "Cancel", 0, 100);

  QObject::connect( &ConversionLsmToMegaThreadSend, SIGNAL( ConversionTerminatedSent() ),
      this, SLOT( ConversionTerminatedReceived() ) );

  QObject::connect(&ConversionLsmToMegaThreadSend, SIGNAL(InitialisationProgressSent()), this, SLOT(InitialisationProgressReceived()));

  QObject::connect(&ConversionLsmToMegaThreadSend, SIGNAL(ProgressSent()), this, SLOT(ProgressReceived()));

  QObject::connect(m_ProgressDialog, SIGNAL(canceled()), this, SLOT(CanceledReceived()));
}

/**
 * \brief Desctructor
 */
QGoLsmToMegaExportDialog::
~QGoLsmToMegaExportDialog()
{
  delete m_ProgressDialog;
}

/**
 * \brief Get and print the name of the lsm file selected
 */
void
QGoLsmToMegaExportDialog::
on_selectLsmFile_clicked()
{
/**
 * \todo add a filter to see only lsm files
 */

  QString fullFilename = QFileDialog::getOpenFileName( this,
      tr( "Select the LSM file to convert" ), "fileName.lsm", 0 );

  QFileInfo fileInfo( fullFilename );

  m_LsmPath = fullFilename.toStdString();

  QString fileName = fileInfo.fileName();
  m_LsmName = fileName.toStdString();

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

  this->convertLabel->setText( QString(QString::fromLocal8Bit("READS LSM READERS"))  );


  // Set conversion parameters
  GoFigure::FileType filetype = GoFigure::PNG;
  if( !m_FileFormatIsPNG )
	{
	filetype = GoFigure::TIFF;
	}

  // conversion fonction called from there to enable progress bar
  ConversionLsmToMegaThreadSend.SetBaseName(m_LsmName);
  ConversionLsmToMegaThreadSend.SetLsmPath(m_LsmPath);
  ConversionLsmToMegaThreadSend.SetOutputFileType(filetype);
  ConversionLsmToMegaThreadSend.SetMegaPath(m_MegaPath);

  ConversionLsmToMegaThreadSend.start();
}

/**
 * \brief Catch thread, proceed conversion and enable all buttons
 */
void
QGoLsmToMegaExportDialog::
ConversionTerminatedReceived()
{
  m_ProgressDialog->cancel();

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

/**
 * \brief Catch thread, initialize the progress bar
 */
void
QGoLsmToMegaExportDialog::
InitialisationProgressReceived()
{
  this->convertLabel->setText( QString(QString::fromLocal8Bit("CONVERSION in PROGRESS"))  );

  int sizeProgressBar = ConversionLsmToMegaThreadSend.GetNumberOfPoints();

  // if starts at 0 strange things happen...
  m_Counter = 1;

  // sizeProgressBar because
  // +1 because m_Counter starts at 1
  // +1 to prevent that the dialoProgressWindow automatically close
  //    then re-open when 100% reached
  m_ProgressDialog->setRange(0,sizeProgressBar+2);
  m_ProgressDialog->setValue(m_Counter);

}

/**
 * \brief Catch thread, update the progress bar
 */
void
QGoLsmToMegaExportDialog::
ProgressReceived()
{
  m_Counter++;
  m_ProgressDialog->setValue(m_Counter);
}

/**
 * \brief Catch thread, cancel the conversion
 */
void
QGoLsmToMegaExportDialog::
CanceledReceived()
{
  ConversionLsmToMegaThreadSend.terminate();
  ConversionLsmToMegaThreadSend.exit();
  this->ConversionTerminatedReceived();
}
