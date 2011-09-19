
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
#include "QGoSplitSegmentationAlgo.h"

//-------------------------------------------------------------------------
QGoSplitSegmentationAlgo::
QGoSplitSegmentationAlgo(
  std::vector< vtkPoints* >* iSeeds,
  QWidget *iParent) : QGoSegmentationAlgo( iParent ), m_Seeds( iSeeds )
{}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
QGoSplitSegmentationAlgo::~QGoSplitSegmentationAlgo()
{

}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
void QGoSplitSegmentationAlgo::SetAlgoWidget(QWidget* iParent)
{

}

//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
std::vector<double>
QGoSplitSegmentationAlgo::
GetBounds(const std::vector<double>& iCenter,
          const double& iRadius,
          const unsigned int& iOrientation)
{
  assert( iCenter.size() == 3 );
  assert( iRadius >= 0. );
  assert( iOrientation < 4 );

  std::vector<double> boundingBox( 6, 0. );
  unsigned int k = 0;

  for(unsigned int i=0; i<3; i++)
    {
    if(i == iOrientation)
      {
      boundingBox[k++] = iCenter[i];
      boundingBox[k++] = iCenter[i];
      }
    else
      {
      boundingBox[k++] = iCenter[i] - iRadius;
      boundingBox[k++] = iCenter[i] + iRadius;
      }
    }

 return boundingBox;
}
//-------------------------------------------------------------------------
