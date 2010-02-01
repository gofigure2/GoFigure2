/*=========================================================================
  Author: $Author: lydiesouhait $  // Author of last commit
  Version: $Revision: 503 $  // Revision of last commit
  Date: $Date: 2010-01-29 17:42:52 -0400 (Mon, 03 Aug 2009) $  // Date of last commit
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

#ifndef __TraceInfoStructure_h
#define __TraceInfoStructure_h

#include "QTableWidgetChild.h"
#include "GoDBCollectionOfTraces.h"
#include "ContourMeshStructure.h"
#include <QWidget>

struct TraceInfoStructure
{
  std::string                        TraceName;
  std::string                        TraceNameID;
  std::string                        CollectionName;
  std::string                        CollectionNameID;
  std::string                        CollectionOf;
  std::string                        CollectionOfID;
  QTableWidgetChild*                 Table;
  GoDBCollectionOfTraces             CollectionOfTraces;
  std::vector<ContourMeshStructure>* ListTracesInfoForVisu;
  
  TraceInfoStructure()
    {ListTracesInfoForVisu = 0;
    Table = 0;}

  TraceInfoStructure(std::string iTraceName, QWidget* parent)
    {
    ListTracesInfoForVisu = 0;
    SetInfoStructure(iTraceName, parent);
    }
    
   ~TraceInfoStructure()
     {delete Table;
     delete ListTracesInfoForVisu;}
  void SetInfoStructure(std::string iTraceName, QWidget* parent)
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
    CollectionOfTraces.SetCollectionInfo(CollectionName,TraceName);
    Table = new QTableWidgetChild(parent);
    } 

 };
#endif


  