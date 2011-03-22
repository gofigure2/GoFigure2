/*=========================================================================
 Authors: The GoFigure Dev. Team.
 at Megason Lab, Systems biology, Harvard Medical school, 2009-11

 Copyright (c) 2009-11, President and Fellows of Harvard College.
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

#ifndef __TreeNodeStructure_h
#define __TreeNodeStructure_h

#include "TraceStructure.h"

#include "QGoIOConfigure.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include "StructureHelper.h"
#endif

#include "vtkActor.h"

/**
 * \struct TreeNodeStructure
 * \brief  Structure which represent a tree node.
 * \ingroup Track Trace
 */
template <class T>
class QGOIO_EXPORT TreeNodeStructure : public TraceStructure
{
public:
  TreeNodeStructure() : TraceStructure()
  {
    m_Mother = NULL;
    m_Child[0] = NULL;
    m_Child[1] = NULL;

    m_DivisionActor.resize(4);
    m_DivisionActor[0] = NULL;
    m_DivisionActor[1] = NULL;
    m_DivisionActor[2] = NULL;
    m_DivisionActor[3] = NULL;
  }

  TreeNodeStructure( const TreeNodeStructure & iE ) : TraceStructure( iE ),
      m_Mother(iE.m_Mother)
  {
    m_Child[0] = iE.m_Child[0];
    m_Child[1] = iE.m_Child[1];

    m_DivisionActor.resize(4);
    m_DivisionActor[0] = iE.m_DivisionActor[0];
    m_DivisionActor[1] = iE.m_DivisionActor[1];
    m_DivisionActor[2] = iE.m_DivisionActor[2];
    m_DivisionActor[3] = iE.m_DivisionActor[3];
  }

  ~TreeNodeStructure(){};

  void DeleteActors()
    {
    std::vector<vtkActor* >::iterator it = m_DivisionActor.begin();
        while(it != m_DivisionActor.end() )
          {
          if(*it)
            {
            (*it)->Delete();
            (*it) = NULL;
            std::cout << "division actor deleted" << std::endl;
            }
          it++;
          }
    }

  T* m_Mother;
  T* m_Child[2];
  std::vector< vtkActor* > m_DivisionActor;
};

#endif
