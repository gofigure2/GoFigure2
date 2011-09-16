===========
 GoFigure2
===========

1. Introduction
===============

The prime goal of **GoFigure2** is the automatic segmentation of nuclei and 
cell membranes and in temporally tracking them amidst cell division to 
create lineages.

2. Installation
===============

2.1. From Package
-----------------

2.2. From Sources
-----------------

Please refer to
* http://sourceforge.net/apps/trac/gofigure2/wiki/DeveloperSetUp
* http://sourceforge.net/apps/trac/gofigure2/wiki/Compilation
* http://sourceforge.net/apps/trac/gofigure2/wiki/VideoSupport

2.2.1. Prerequites
~~~~~~~~~~~~~~~~~~

User or developpers should first install the following libraries and
softwares:
* CMake (>=2.8.2) visit http://www.cmake.org
* Qt  (>=4.5)   visit http://www.qtsoftware.com
* MySQL (=5.1) visit http://www.mysql.com
* FFMPEG (optional Linux or Mac) visit visit http://www.ffmpeg.org
* VTK with Qt and MYSQL (>=5.6)   visit http://www.vtk.org 
* ITK (>=3.20)  visit http://www.itk.org
* Boost (>=1.38) visit http://www.boost.org

2.2.2. Configuration
~~~~~~~~~~~~~~~~~~~~

* On Linux and Mac:

Create a build directory (where **GoFigure2** will be compiled)::
  $ mkdir BUILD

Launch cmake:: 
  $ cd BUILD
  $ ccmake path/to/source/directory

Build::
  $ make -j8

Install::
  # make install

