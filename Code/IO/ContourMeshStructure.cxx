/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Rev$  // Revision of last commit
  Date: $Date$  // Date of last commit
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

#include "ContourMeshStructure.h"
//#include <QDebug>

ContourMeshStructure::ContourMeshStructure( ) : TraceID( 0 ), CollectionID( 0 ),
    TCoord( 0 ), Highlighted( false ), Visibility( true ), Direction( 0 )
{
    this->Actor = 0;
    this->Nodes = 0;
    this->rgba[0] = 1.;
    this->rgba[1] = 1.;
    this->rgba[2] = 1.;
    this->rgba[3] = 1.;
}

// Obsolete...
ContourMeshStructure::ContourMeshStructure( const unsigned int& iTraceID, vtkActor* iActor,
    vtkPolyData* iNodes, const unsigned int& iCollectionID, const unsigned int& iT,
    const bool& iHighlighted, const double& r, const double& g, const double& b,
    const double& alpha, const int& iDir )
    : TraceID( iTraceID ), Actor( iActor ), Nodes( iNodes ), CollectionID( iCollectionID ),
      TCoord( iT ), Highlighted( iHighlighted ), Visibility( true ), Direction( iDir )
    {
    //qDebug() << "deprecated:";
    //qDebug() << "ContourMeshStructure( iTraceID, iActor, iNodes, iCollectionID, iT, iHighlighted, r, g, b, alpha, iDir )";
    //qDebug() << "use:";
    //qDebug() << "ContourMeshStructure( iTraceID, iActor, iNodes, iCollectionID, iT, iHighlighted, iVisibility, r, g, b, alpha, iDir ):";
    this->rgba[0] = r;
    this->rgba[1] = g;
    this->rgba[2] = b;
    this->rgba[3] = alpha;
    }

ContourMeshStructure::ContourMeshStructure( const unsigned int& iTraceID, vtkActor* iActor,
    vtkPolyData* iNodes, const unsigned int& iCollectionID, const unsigned int& iT,
    const bool& iHighlighted, const bool& iVisibility,
    const double& r, const double& g, const double& b, const double& alpha,
    const int& iDir )
    : TraceID( iTraceID ), Actor( iActor ), Nodes( iNodes ), CollectionID( iCollectionID ),
      TCoord( iT ), Highlighted( iHighlighted ), Visibility( iVisibility ), Direction( iDir )
    {
    this->rgba[0] = r;
    this->rgba[1] = g;
    this->rgba[2] = b;
    this->rgba[3] = alpha;
    }

ContourMeshStructure::ContourMeshStructure( const ContourMeshStructure& iE ) :
    TraceID( iE.TraceID ), Actor( iE.Actor ), Nodes( iE.Nodes ), CollectionID( iE.CollectionID ),
    TCoord( iE.TCoord ), Highlighted( iE.Highlighted ), Visibility( iE.Visibility ),
    Direction( iE.Direction )
    {
    for( int i = 0; i < 4; i++ )
      {
      this->rgba[i] = iE.rgba[i];
      }
    }

    ContourMeshStructure::~ContourMeshStructure()
    {}
