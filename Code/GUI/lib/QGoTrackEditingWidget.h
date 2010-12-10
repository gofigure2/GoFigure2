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

#include "vtkInteractorStyleImage3D.h"
#include "vtkEventQtSlotConnect.h"


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
  explicit QGoTrackEditingWidget(QWidget *parent = 0);
  ~QGoTrackEditingWidget();

  /*
   * \todo Need Color in the track information??
   */
  typedef std::pair<  int, std::pair<  int,double*> > Mesh;
  typedef std::pair<  int, std::list<Mesh> > Track;

  void setTracks( std::list<Track> iListOfTracks );

  void generateTrackRepresentation();

  bool findInCutList(int iTrackId, int iMeshID);

  /*
   * Useless....?
  // when pick actor update cut list + actor color
  void cutMode();

  // When pick actors, update merge list + actor color
  void mergeMode();
*/

  signals:
    void cutTracks( std::list< std::pair< int,  int> > );
    void mergeTracks( std::list< std::pair< std::pair< int,  int>, std::pair< int,  int> > > );

  public slots:
    void preview();
    void UpdateCurrentActorSelection(vtkObject *caller);

private:
  vtkActor* CreateSphereActor( double* iCenter);
  vtkActor* CreatePolylineActor( double* iCenter1, double* iCenter2);

  std::list<Track>                                             m_ListOfTracks;
  std::list< std::pair< int,  int> >                           m_CutList;
  std::list< std::pair< std::pair< int,  int>, std::pair< int,  int> > >           m_MergeList;
  bool                                                         m_MergeMode;
  bool                                                         m_CutMode;
  std::map< vtkActor*, std::pair< bool, std::pair< int,  int> > > m_Actor2IDMap;

  vtkInteractorStyleImage3D* m_InteractorStyle3D;
  vtkEventQtSlotConnect*     m_VtkEventQtConnector;
  vtkActor*                  m_CurrentActor;

};
#endif
