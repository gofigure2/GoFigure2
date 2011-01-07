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

#include "vtkSmartPointer.h"

#include "MeshContainer.h"

class vtkPoints;

/**
\class QGoTrackEditingWidget
\brief This dialog allows the user to split/merge the tracks using a GUI
\ingroup GUI Track
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

  /**
   * \brief Initialize the renderer, the rendering window, the interactor style
   * and add the meshes, polylines and labels to the visualization
   */
  void    init();

  /**
   * \brief Returns the list of tracks to be created
   * \return    The list of tracks to be created
  */
  std::list< std::list< unsigned int > >              GetListOfTracksToBeCreated();

  /**
   * \brief Returns the list of tracks to be updated
   * \return    The list of tracks to be updated
  */
  std::map< unsigned int, std::list< unsigned int > > GetListOfTracksToBeUpdated();

  /**
   * \brief Returns the list of tracks to be deleted
   * \return    The list of tracks to be deleted
  */
  std::list< unsigned int >                           GetListOfTracksToBeDeleted();

public slots:

  /**
  \brief Defines behavior when we pick an actor in the visualization
  */
  void updateCurrentActorSelection(vtkObject *caller);

  /**
  \brief Restore the track IDs to theirs original values (ie before entering the widget).
  Creates a TracksToBeCreated list, a TracksToBeUpdated list and a TracksToBeDeleted list.
  */
  void restoreTrackIDs();

private:

  void HighlightFirstActor( bool iHighlight );
  void ModifyMeshCollectionID( unsigned int iMeshID, unsigned int iCollectionID);
  void computeMeshActors();

  /**
   * \brief Reassigns track IDs and add the meshes, polylines and labels actors
   * to the visualization
   */
  void    initializeVisualization();

  /**
   * \brief Cut a track at the current actor. The actor represents a polyline
   * between 2 meshes.
   * \param[in] iActor position where we want to split the track
   */
  void    cutTrack( vtkActor* iActor);

  /**
   * \brief Merge 2 tracks, given 2 mesh IDs.
   * Requierements for a successful merge:
   * -Each mesh must belong to different tracks.
   * -The tracks can't overlap.
   * -The mesh has to be a border of its own track.
   * \param[in] iFirstMesh ID of a mesh belonging to the first track
   * \param[in] iSecondMesh ID of a mesh belonging to the second track
   * \return true is the merge was successful
   */
  bool    mergeTrack( const unsigned int& iFirstMesh, const unsigned int& iSecondMesh);

  /**
   * \brief Get the borders of the given track.
   * \param[in] iCollectionID track IDs we are interested in
   * \return time point and mesh id of the first and last mesh of the track
   */
  std::pair< std::pair<unsigned int, unsigned int>,
             std::pair<unsigned int, unsigned int> >
  getTrackBorders( const unsigned int& iCollectionID );

  /**
   * \brief Reassing real track IDs to temporary ones for convenience
   */
  void    reassignTrackIDs();

  /**
   * \brief Update track IDs after a merge
   * -The mesh has to be a border of its own track.
   * \param[in] iIDToDelete ID of the track to be deleted
   * \param[in] iIDToUpdate ID of the track to be updated
   */
  void    updateTracksIDs( const unsigned int& iIDToDelete,
                        const unsigned int& iIDToUpdate);

  /**
   * \brief Create the polyLines actors according to the current MeshContainer
   */
  void      computeLineActors();

  /**
   * \brief Remove the polyLines actors. Usefull after a merge or a cut. An
   * "Update" method would be more efficient than remove then compute.
   */
  void      removeLineActors();

  /**
   * \brief Create a line between 2 points
   * \param[in] iCenter1 Center of the first mesh
   * \param[in] iCenter2 Center of the second mesh
   * \param[in] iColor1 Color of the first mesh
   * \param[in] iColor2 Color of the second mesh
   * \return a pointer to the new actor. It has to be deleted somewhere.
   */
  vtkActor* createPolylineActor( double* iCenter1, double* iCenter2,
                                 const double* iColor1 = NULL,
                                 const double* iColor2 = NULL );

  /**
   * \brief Create label actors to see the temporal information of each mesh.
   * It is very useful for the merge. Note that the 2 input parameters have to
   * ordered in the same way.
   */
  void      computeLabelActor();

  MeshContainer* m_MeshContainer;

  std::list< std::list< unsigned int > >              m_ListOfNewTrack;
  std::map< unsigned int, std::list< unsigned int > > m_ListOfUpdatedTracks;
  std::list< unsigned int >                           m_ListOfDeletedTracks;

  unsigned int m_MaxTrackID;
  unsigned int m_NumberOfTracks;

  bool m_SecondClick;

  vtkActor*    m_CurrentActor;
  vtkActor*    m_FirstMeshActor;
  unsigned int m_FirstMeshID;

  vtkInteractorStyleImage3D*                 m_InteractorStyle3D;
  vtkSmartPointer<vtkEventQtSlotConnect>     m_VtkEventQtConnector;
  vtkSmartPointer<vtkRenderer>               renderer;

  enum TrackStatusType
    {
    NEW_TRACK = 0,
    UPDATED_TRACK,
    DELETED_TRACK
    };

  std::map< unsigned int, TrackStatusType > m_TrackStatus;

  std::map< unsigned int, unsigned int> m_TrackIDsMapping;
  typedef std::map< vtkActor*, unsigned int > ActorMeshIDMapType;
  typedef ActorMeshIDMapType::iterator ActorMeshIDMapIterator;
  ActorMeshIDMapType                    m_Line2MeshID;

private:
  Q_DISABLE_COPY( QGoTrackEditingWidget );

};
#endif
