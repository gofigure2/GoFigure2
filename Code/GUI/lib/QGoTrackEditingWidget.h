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

#ifndef __QGoTrackEditingWidget_h
#define __QGoTrackEditingWidget_h

#include <QDialog>

#include "ui_TrackEditingWidget.h"

#include "QGoGUILibConfigure.h"

#include "vtkActor.h"
#include "vtkPolyData.h"
#include "vtkPoints.h"

#include "vtkInteractorStyleImage3D.h"
#include "vtkEventQtSlotConnect.h"

#include "vtkSmartPointer.h"
#include "vtkPolyData.h"

//New
#include "MeshContainer.h"
//#include <boost/bimap.hpp>

/**
\class QGoTrackEditingWidget
\brief This dialog allows the user to split/merge the tracks using a GUI
\ingroup GUI
*/
class QGOGUILIB_EXPORT QGoTrackEditingWidget:
  public QDialog,
  private Ui::TrackEditingWidget
{
  Q_OBJECT
public:
  explicit QGoTrackEditingWidget( MeshContainer* imeshContainer = NULL, QWidget *parent = 0 );
  ~QGoTrackEditingWidget();

  void    setMeshContainer( MeshContainer* imeshContainer );
  void    initializeVisualization();
  void    cutTrack( vtkActor* );
  void    mergeTrack( vtkActor*, vtkActor* );
  void    removeLineActors();
  void    mapContainerIDs2RealIDs();

  std::pair< std::pair<unsigned int, unsigned int>, std::pair<unsigned int, unsigned int> >
      isOnBorder( unsigned int iMeshID);

  void updateTracksIDs( unsigned int iIDToDelete, unsigned int iIDToUpdate);

  signals:
    void cutTracks( std::list< std::pair< int,  int> > );
    void mergeTracks( std::list< std::pair< std::pair< int,  int>, std::pair< int,  int> > > );

  public slots:
    void preview();
    void UpdateCurrentActorSelection(vtkObject *caller);

private:
  vtkActor* CreateSphereActor( double* iCenter, const double* iColor);
  vtkActor* CreatePolylineActor( double* iCenter1, double* iCenter2, const double* iColor1 = NULL, const double* iColor2 = NULL);

  bool m_SecondClick;
  std::pair<int, int> m_FirstPair;
  vtkActor* m_FirstActor;
  vtkActor* m_SecondActor;

  vtkInteractorStyleImage3D* m_InteractorStyle3D;
  vtkEventQtSlotConnect*     m_VtkEventQtConnector;
  vtkActor*                  m_CurrentActor;

  vtkSmartPointer<vtkRenderer> renderer;
  MeshContainer* m_MeshContainer;
  std::map< vtkActor* , int >          m_Line2MeshID;

  // Split/merge behavior
  std::map< unsigned int , std::pair<unsigned int, unsigned int> >  m_MeshID2Neigbours;

  unsigned int m_NumberOfTracks;
  bool m_FirstRender;
  std::map< unsigned int, unsigned int> m_TrackIDsMapping;
  unsigned int m_MaxTrackID;
  vtkPolyData* m_LabelData;

  //typedef boost::bimap< vtkActor* , unsigned int > bm_type;
  //bm_type m_Actor2MeshID;
  std::map < vtkActor* , unsigned int > m_Actor2MeshID;

};
#endif
