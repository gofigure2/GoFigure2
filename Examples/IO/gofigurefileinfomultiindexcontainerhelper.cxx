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
#include "GoFigureFileInfoMultiIndexContainerHelper.h"

#include <iostream>

int main(int, char**)
{
  GoFigureFileInfoHelperMultiIndexContainer container;

  int plaque = 0;
  int r = 0;
  int c = 0;
  int xtile = 0;
  int ytile = 0;
  int ztile = 0;
  int xs = 0;
  int ys = 0;

  for (int t = 0; t < 10; t++)
    {
    for (int ch = 0; ch < 5; ch++)
      {
      for (int z = 0; z < 20; z++)
        {
        std::stringstream filename;
        filename << "yo_p0_r0_c0_xt0_yt0_zt0_zs" << z << "_ch" << ch << "_t" << t << ".png";
        container.insert(GoFigureFileInfoHelper(plaque, r, c, xtile, ytile, ztile,
                                                xs, ys, z, t, ch, filename.str()));
        }
      }
    }

  std::list<std::string> list =
    GetAllFileNamesForGivenTCoordAndChannel(container, 5, 2);

  for (std::list<std::string>::iterator it = list.begin();
       it != list.end();
       ++it)
    {
    std::cout << *it << std::endl;
    }

  list = GetAllFileNamesForGivenZCoordPointAndChannel(container, 12, 4);

  for (std::list<std::string>::iterator it = list.begin();
       it != list.end();
       ++it)
    {
    std::cout << *it << std::endl;
    }

  return EXIT_SUCCESS;
}
