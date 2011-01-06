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

  explicit QGoTrackEditingWidget( MeshContainer* imeshContainer = NULL,
                                  QWidget *parent = 0 );
  ~QGoTrackEditingWidget();

  void    setMeshContainer( MeshContainer* imeshContainer );

  std::list< std::list< unsigned int > > GetListOfTracksToBeCreated();
  std::map< unsigned int, std::list< unsigned int > > GetListOfTracksToBeUpdated();
  std::list< unsigned int > GetListOfTracksToBeDeleted();

signals:
  void cutTracks( std::list< std::pair< int,  int> > );
  void mergeTracks( std::list< std::pair< std::pair< int,  int>,
                               std::pair< int,  int> > > );

public slots:
  void preview();
  void UpdateCurrentActorSelection(vtkObject *caller);
  void mapContainerIDs2RealIDs();

private:
  void    initializeVisualization();

  vtkActor* CreatePolylineActor( double* iCenter1, double* iCenter2,
                                 const double* iColor1 = NULL,
                                 const double* iColor2 = NULL );
  void    cutTrack( vtkActor* );
  bool    mergeTrack( const unsigned int&, const unsigned int& );

  std::pair< std::pair<unsigned int, unsigned int>,
             std::pair<unsigned int, unsigned int> >
  GetTrackBorders( const unsigned int& iCollectionID );

  void updateTracksIDs( const unsigned int& iIDToDelete,
                        const unsigned int& iIDToUpdate);

  void    removeLineActors();
  void    computeLineActors();

  void    computeLabelActor( vtkSmartPointer<vtkDoubleArray> iScalars,
                            vtkSmartPointer<vtkPoints> iPts);

  void    reassignTrackIDs();
  //void    restoreTrackIDs();

  std::list< std::list< unsigned int > > m_ListOfNewTrack;
  std::map< unsigned int, std::list< unsigned int > > m_ListOfUpdatedTracks;
  std::list< unsigned int > m_ListOfDeletedTracks;


  MeshContainer* m_MeshContainer;

  unsigned int m_MaxTrackID;
  unsigned int m_NumberOfTracks;

  bool m_SecondClick;
  //std::pair<int, int> m_FirstPair;
  vtkActor* m_FirstMeshActor;
  unsigned int m_FirstMeshID;

  vtkActor* m_SecondMeshActor;
  unsigned int m_SecondMeshID;

  vtkInteractorStyleImage3D* m_InteractorStyle3D;
  vtkSmartPointer<vtkEventQtSlotConnect>     m_VtkEventQtConnector;
  vtkActor*                  m_CurrentActor;

  vtkSmartPointer<vtkRenderer> renderer;

  typedef std::map< vtkActor*, unsigned int > ActorMeshIDMapType;
  typedef ActorMeshIDMapType::iterator ActorMeshIDMapIterator;
  ActorMeshIDMapType m_Line2MeshID;

  enum TrackStatusType
    {
    NEW_TRACK = 0,
    UPDATED_TRACK,
    DELETED_TRACK
    };

  std::map< unsigned int, TrackStatusType > m_TrackStatus;

  // Split/merge behavior
  std::map< unsigned int ,
    std::pair<unsigned int, unsigned int> >  m_MeshID2Neigbours;

  std::map< unsigned int, unsigned int> m_TrackIDsMapping;

  //typedef boost::bimap< vtkActor* , unsigned int > bm_type;
  //bm_type m_Actor2MeshID;
  std::map < vtkActor* , unsigned int > m_Actor2MeshID;

private:
  Q_DISABLE_COPY( QGoTrackEditingWidget );

};
#endif
