GoFigure2
============================================================================

1-Introduction
  The prime goal of GoFigure is the automatic segmentation of nuclei and 
cell membranes and in temporally tracking them amidst cell division to 
create lineages.

2-Installation
  2.1 From Package
  2.2 From Sources
    2.2.1 Prerequites
  User or developpers should first install the following libraries and
softwares:
      * CMake (>=2.6) visit http://www.cmake.org
      * Qt  (>=4.5)  visit http://www.qtsoftware.com
      * VTK (>=5.4) visit http://www.vtk.org
with Qt and MYSQL
      * ITK (>=3.16) visit http://www.itk.org

    2.2.2 Configuration

      * On Linux:
$ cd gofigure2/trunk/

Create a build directory where GoFigure will be compiled
$ mkdir BUILD

Launch cmake 
$ cd BUILD
$ ccmake ..

Build
$ make

Install
$ make install

