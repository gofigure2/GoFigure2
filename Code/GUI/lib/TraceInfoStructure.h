/*=========================================================================
  Author: $Author$  // Author of last commit
  Version: $Revision$  // Revision of last commit
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

#ifndef __TraceInfoStructure_h
#define __TraceInfoStructure_h

#include "QTableWidgetChild.h"
#include "GoDBCollectionOfTraces.h"
#include "ContourMeshStructureHelper.h"
#include <QWidget>

struct TraceInfoStructure
{
  std::string                               TraceName;
  std::string                               TraceNameID;
  std::string                               CollectionName;
  std::string                               CollectionNameID;
  std::string                               CollectionOf;
  std::string                               CollectionOfID;
  QTableWidgetChild*                        Table;
  GoDBCollectionOfTraces*                   CollectionOfTraces;
  ContourMeshStructureMultiIndexContainer*  ListTracesInfoForVisu;
  
  TraceInfoStructure() : Table( NULL ), CollectionOfTraces( NULL ),
    ListTracesInfoForVisu( NULL )
    {}

  TraceInfoStructure( const std::string& iTraceName, QWidget* parent ) :
    Table( NULL ), CollectionOfTraces( NULL ), ListTracesInfoForVisu( NULL )
    {
    SetInfoStructure( iTraceName, parent );
    }
    
   ~TraceInfoStructure()
     {
     // Table has a parent that is supposed to delete it
     //if( Table )
     //   {
     //   delete Table;
     //   }
      if( ListTracesInfoForVisu )
        {
        delete ListTracesInfoForVisu;
        }
      if( CollectionOfTraces )
        {
        delete CollectionOfTraces;
        }
      }

  void SetInfoStructure( const std::string& iTraceName, QWidget* iParent )
    {
    TraceName = iTraceName;
    TraceNameID = iTraceName;
    TraceNameID += "ID";

    if (TraceName == "contour")
      {
      CollectionName = "mesh";
      CollectionOf = "None";
      }
    if (TraceName == "mesh")
      {
      CollectionName = "track";
      CollectionOf = "contour";
      }
    if (TraceName == "track")
      {
      CollectionName = "lineage";
      CollectionOf = "mesh";
      }
    if (TraceName == "lineage")
      {
      CollectionName = "None";
      CollectionOf = "mesh";
      }
    CollectionNameID = CollectionName;
    CollectionNameID += "ID";
    CollectionOfID = CollectionOf;
    CollectionOfID += "ID";
    CollectionOfTraces = new GoDBCollectionOfTraces( CollectionName, TraceName );
    Table = new QTableWidgetChild(iParent);
    } 

 };
#endif
