/*=========================================================================
  Author: $Author: krm15 $  // Author of last commit
  Version: $Revision: 1163 $  // Revision of last commit
  Date: $Date: 2010-04-01 15:07:18 -0400 (Thu, 01 Apr 2010) $  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-10

 Copyright (c) 2009-10, President and Fellows of Harvard College.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 Neither the name of the  President and Fellows of Harvard College
 nor the names of its contributors may be used to endorse or promote
 products derived from this software without specific prior written
 permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/

#include "QGoMainWindow.h"
#include "QGoTabElementBase.h"
#include "QGoTabImageView2D.h"
#include "QGoTabImageView3D.h"
#include "QGoTabImageView3DwT.h"
#include "QGoPrintDatabase.h"

#include "ContourMeshStructure.h"

// Plugin stuff
#include "QGoPluginHelper.h"
#include "QGoImageFilterPluginBase.h"

#include <iostream>
#include <set>

// Qt includes
#include <QDir>
#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QDialog>
#include <QPluginLoader>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QScrollArea>

// Qt Dialog Box
#include "QGoLsmToMegaExportDialog.h"

// itk includes
#include "itkImageFileReader.h"

#include "itkLsm3DSerieImport.h"
#include "itkMegaCaptureImport.h"

// vtk includes
// #include "vtkImageAppendComponents.h"
// #include "vtkMarchingCubes.h"
#include "vtkLSMReader.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataMySQLTextReader.h"
#include "vtkPolyDataMySQLTextWriter.h"
#include "vtkPLYReader.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"

#include "vtkImageData.h"
#include "vtkImageReader2Factory.h"
#include "vtkImageReader2.h"
#include "vtkPolyDataWriter.h"
#include "vtkSmartPointer.h"
#include "vtkCell.h"
#include "vtkPoints.h"

#include "QGoTabManager.h"
#include "QGoWizardDB.h"

#include <list>

//--------------------------------------------------------------------------
QGoMainWindow::QGoMainWindow( )
{
}

//--------------------------------------------------------------------------
QGoMainWindow::~QGoMainWindow()
{
}
//--------------------------------------------------------------------------


//--------------------------------------------------------------------------
void QGoMainWindow::on_actionExportContour_triggered( )
{
  typedef std::vector<ContourMeshStructure> ContourMeshVectorType;
  typedef ContourMeshVectorType::iterator ContourMeshIteratorType;

  typedef std::vector < int > RGBAType;
  typedef std::pair < std::string, RGBAType > ColorType;
  typedef std::list< ColorType > ColorComboBoxType;
  typedef std::list< std::pair < std::string, std::vector < int > > >::iterator iter;

  //get current tab widget
  QWidget* w = this->CentralTabWidget->currentWidget();

  QGoTabImageView3DwT* w3t = dynamic_cast< QGoTabImageView3DwT* >( w );

  if( w3t )
  {
    if( w3t->m_DataBaseTables->IsDatabaseUsed() )
    {
      ContourMeshVectorType* contours = w3t->m_DataBaseTables->GetTracesInfoListForVisu("contour");

      vtkPolyDataMySQLTextWriter* convert_writer =
        vtkPolyDataMySQLTextWriter::New();

      // Set file name
      QString p = QFileDialog::getSaveFileName(
      this,
      tr( "Save Contour Export File" ),"",
      tr( "TextFile (*.txt)" )
      );

      if ( ! p.isNull() )
      {
        QFileInfo pathInfo( p );
        std::string filename = p.toStdString();

        // Create an xml file
        std::fstream outfile;
        outfile.open ( filename.c_str(), std::ios::out );

        // Missing information in the export
//         ColorComboBoxType colorComboBoxInfo = w3t->m_DataBaseTables->GetColorComboBoxInfofromDB ();
//         for( std::list< ColorType >::iterator it = colorComboBoxInfo.begin();
//           it != colorComboBoxInfo.end();
//           ++it )
//         {
//           std::cout << (*it) << std::endl;
//         }

        // 2. GetCurrentCollectionData ()
        // 3. GetListExistingCollectionIDFromDB (std::string TraceName, int iTimePoint)

        unsigned int contourId, meshId, timePt;
        ContourMeshIteratorType It = contours->begin();
        outfile << contours->size() << std::endl;
        while( It != contours->end() )
        {
          contourId = (*It).TraceID;
          meshId = (*It).CollectionID;
          timePt = (*It).TCoord;
          outfile << "<contour>" << std::endl;
          outfile << contourId << std::endl;
          outfile << "<TCoord> " << std::endl;
          outfile << timePt << std::endl;
          outfile << "</TCoord>" << std::endl;
          outfile << "<MeshId> " << std::endl;
          outfile << meshId << std::endl;
          outfile << "</MeshId>" << std::endl;

          outfile << "<Nodes>" << std::endl;
          //with the class here
          std::string str = convert_writer->GetMySQLText( (*It).Nodes );
          outfile << str << std::endl;
          outfile << "</Nodes>" << std::endl;
          outfile << "</contour>" << std::endl;
          ++It;
        }
        outfile.close();
      }
    }
  }
}

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionExportMesh_triggered( )
{}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void QGoMainWindow::on_actionExportTrack_triggered( )
{}

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionExportLineage_triggered( )
{}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionImportContour_triggered( )
{
  typedef std::vector<ContourMeshStructure> ContourMeshVectorType;
  typedef ContourMeshVectorType::iterator ContourMeshIteratorType;

  // get current tab widget
  QWidget* w = this->CentralTabWidget->currentWidget();

  QGoTabImageView3DwT* w3t = dynamic_cast< QGoTabImageView3DwT* >( w );

  //get default color
  double rgba[4];
  rgba[0] = rgba[1] = rgba[2] = rgba[3] = 1;

// Arnauds Suggestions
//   typedef std::list< ContourMeshStructure > ContourMeshListType;
//   typedef std::map< int, ContourMeshListType > MapOfContourMeshListType;
// 1. Fill Container
// 2. Iterate]
//    Create a new mesh
//    Get corresponding id
//    Add the list with the id
// Add subcellular type
// Add

  if( w3t )
    {
    if( w3t->m_DataBaseTables->IsDatabaseUsed() )
      {
      // Get file name
      QString p = QFileDialog::getOpenFileName(
      this,
      tr( "Open Contour Export File" ),"",
      tr( "TextFile (*.txt)" )
      );

      if ( ! p.isNull() )
        {
        std::string filename = p.toStdString();

        int contourId, meshId, timePt, NumberOfContours;
        //Open an xml file
        std::ifstream infile( filename.c_str(), std::ifstream::in );
        std::string line, nodes;
        if( infile.is_open() )
          {
          vtkPolyDataMySQLTextReader* convert_reader =
            vtkPolyDataMySQLTextReader::New();
          convert_reader->SetIsContour( true );

          // Get number of contours
          getline( infile, line );
          std::istringstream nC(line);
          nC >> NumberOfContours;
//           std::cout << "# of contours: " << NumberOfContours << std::endl;
          for( int i = 0; i < NumberOfContours; i++ )
            {
            //<contour>
            getline( infile, line );
            getline( infile, line );
            std::istringstream c(line);
            c >> contourId;
//             std::cout << contourId << std::endl;

            // <TCoord>
            getline( infile, line );
            getline( infile, line );
            std::istringstream t(line);
            t >> timePt;
            getline( infile, line );
//             std::cout << timePt << std::endl;

            // <MeshId>
            getline( infile, line );
            getline( infile, line );
            std::istringstream m(line);
            m >> meshId;
            getline( infile, line );
//             std::cout << meshId << std::endl;

            // <Nodes>
            getline( infile, line );
            getline( infile, nodes );
            getline( infile, line );
//             std::cout << nodes << std::endl;

            //create a vtkPolyData*
            vtkPolyData* ptr = convert_reader->GetPolyData( nodes );

// Fill container

            w3t->AddContourFromNodes(
                contourId,
                ptr,
                rgba, // what default color to set
                false,   // no highlighting
                timePt, // timepoint
                true ); // to be saved in the database

            getline( infile, line );
            }
         }
       }
     }
   }
}

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionImportMesh_triggered( )
{}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionImportTrack_triggered( )
{}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
void QGoMainWindow::on_actionImportLineage_triggered( )
{}
//--------------------------------------------------------------------------

