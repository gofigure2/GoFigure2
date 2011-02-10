/*=========================================================================
  Author: $Author: lsouhait $  // Author of last commit
  Version: $Rev: 542 $  // Revision of last commit
  Date: $Date: 2009-08-06 16:08:10 -0400 (Thu, 06 Aug 2009) $  // Date of last commit
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
#include "GoDBImport.h"
#include <string>



int main(int argc, char *argv[])
{

  std::string ServerName = "localhost";
  std::string filename;
  std::string Login;
  std::string Password;
  std::string DBName = "gofiguredatabase";
  int ImgSessionID;
 
  std::cout<<"Enter your mysql user:"<<std::endl;
  std::cin >> Login;
  std::cout<<"Enter your mysql password:"<<std::endl;
  std::cin >> Password;
  std::cout<<"Enter the path to your file to import:"<<std::endl;
  std::cin >> filename;
  std::cout<<"Enter your imagingsessionID the traces will be imported to:"<<std::endl;
  std::cin >> ImgSessionID;
 
  /*std::string ServerName = "localhost";
  std::string filename = argv[1];
  std::string Login = argv[2];
  std::string Password = argv[3];
  std::string DBName = "gofiguredatabase";
  int ImgSessionID = argv[4];*/

  //import into the database:
  GoDBImport ImportHelper(ServerName, Login,
                          Password, ImgSessionID, filename, 0);
  ImportHelper.ImportTracks();  
}
