/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
=========================================================================*/

/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009

 Copyright (c) 2009, President and Fellows of Harvard College.
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
#include <QApplication>

#include "QGoTabImageViewElementBase.h"

#include "vtkImageData.h"

class QGoTabImageViewElementBaseTestHelper :
  public QGoTabImageViewElementBase
{
public:
  QGoTabImageViewElementBaseTestHelper( QWidget* iParent = 0 )
    : QGoTabImageViewElementBase( iParent ) {}
  virtual ~QGoTabImageViewElementBaseTestHelper() {}

  GoFigure::TabDimensionType GetTabDimensionType( ) const
    { return GoFigure::TWO_D; }

  virtual void Update() {}

  virtual void WriteSettings() {}
  virtual void ReadSettings() {}

public slots:
  virtual void ShowAllChannels( bool iChecked )
    {
    (void) iChecked;
    }
  virtual void ShowOneChannel( int iChannel )
    {
    (void) iChannel;
    }
  virtual void TakeSnapshot()
    {}
protected:
  virtual void PopulateMenus( QObject* iPlugin )
    {
    (void) iPlugin;
    }
  virtual void GetBackgroundColorFromImageViewer( ) {}
  virtual void SetBackgroundColorToImageViewer( ) {}
  virtual void SetImageToImageViewer( vtkImageData* ) {}
  virtual int* GetImageCoordinatesFromWorldCoordinates( double iPos[3] )
    {
    (void) iPos;
    return 0;
    }

  virtual void RemoveActorFromViewer( const int& , vtkActor* )
    {}
  virtual void DisplayActorInViewer( const int& , vtkActor* )
    {}

  virtual std::vector< vtkActor* > AddContour( const int&,
      vtkPolyData*,
      vtkProperty* iProperty = NULL )
    {
    (void) iProperty;
    return std::vector< vtkActor* >();
    }

  virtual void SetSlice( int, int* ) {}
};

int main( int argc, char** argv )
{
  QApplication app( argc, argv );
  QCoreApplication::setOrganizationName( "MegasonLab" );
  QCoreApplication::setOrganizationDomain( "http://gofigure2.sourceforge.net" );
  QCoreApplication::setApplicationName( "qgotabimageviewelementbase" );

  QGoTabImageViewElementBaseTestHelper* test =
    new QGoTabImageViewElementBaseTestHelper;
  test->SetColor( true );

  delete test;

  return EXIT_SUCCESS;
}
