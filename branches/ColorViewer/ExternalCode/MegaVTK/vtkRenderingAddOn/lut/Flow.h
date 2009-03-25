/*=========================================================================

Program:   vtkINRIA3D
Module:    $Id: Flow.h 477 2007-11-20 17:46:10Z filus $
Language:  C++
Author:    $Author: filus $
Date:      $Date: 2007-11-20 12:46:10 -0500 (Tue, 20 Nov 2007) $
Version:   $Revision: 477 $

Copyright (c) 2007 INRIA - Asclepios Project. All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

/*========================================================================
 Copyright (c) INRIA - ASCLEPIOS Project (http://www-sop.inria.fr/asclepios).
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 * Neither the name of INRIA or ASCLEPIOS, nor the names of any contributors
 may be used to endorse or promote products derived from this software 
 without specific prior written permission.
 
 * Modified source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 =========================================================================*/

#ifndef _lut_Flow_h_
#define _lut_Flow_h_

static const int Flow[3*256]={
32,
		32,
		32,
		32,
		31,
		31,
		31,
		31,
		31,
		30,
		30,
		30,
		30,
		30,
		29,
		29,
		29,
		29,
		29,
		28,
		28,
		28,
		28,
		28,
		27,
		27,
		27,
		27,
		27,
		26,
		26,
		26,
		26,
		26,
		25,
		25,
		25,
		25,
		25,
		24,
		24,
		24,
		24,
		24,
		23,
		23,
		23,
		23,
		23,
		22,
		22,
		22,
		22,
		22,
		21,
		21,
		21,
		21,
		21,
		20,
		20,
		20,
		20,
		19,
		19,
		19,
		19,
		18,
		18,
		18,
		17,
		17,
		17,
		16,
		16,
		16,
		16,
		15,
		15,
		15,
		14,
		14,
		14,
		13,
		13,
		13,
		13,
		12,
		12,
		12,
		11,
		11,
		11,
		10,
		10,
		10,
		10,
		9,
		9,
		9,
		8,
		8,
		8,
		7,
		7,
		7,
		7,
		6,
		6,
		6,
		5,
		5,
		5,
		4,
		4,
		4,
		4,
		3,
		3,
		3,
		2,
		2,
		2,
		1,
		1,
		1,
		1,
		0,
		0,
		0,
		4,
		8,
		12,
		16,
		20,
		24,
		28,
		32,
		36,
		40,
		45,
		49,
		53,
		57,
		61,
		65,
		69,
		73,
		77,
		81,
		85,
		89,
		93,
		97,
		101,
		105,
		109,
		113,
		117,
		121,
		125,
		130,
		134,
		138,
		142,
		146,
		150,
		154,
		158,
		162,
		166,
		170,
		174,
		178,
		182,
		186,
		190,
		194,
		198,
		202,
		206,
		210,
		215,
		219,
		223,
		227,
		231,
		235,
		239,
		243,
		247,
		251,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		200,
		197,
		194,
		191,
		188,
		185,
		182,
		179,
		176,
		173,
		170,
		167,
		164,
		161,
		158,
		155,
		152,
		149,
		146,
		143,
		140,
		137,
		134,
		131,
		128,
		125,
		122,
		119,
		116,
		113,
		110,
		107,
		104,
		101,
		98,
		95,
		92,
		89,
		86,
		83,
		80,
		77,
		74,
		71,
		68,
		65,
		62,
		59,
		56,
		53,
		50,
		47,
		44,
		41,
		38,
		35,
		32,
		29,
		26,
		23,
		20,
		20,
		20,
		19,
		19,
		19,
		19,
		18,
		18,
		18,
		17,
		17,
		17,
		16,
		16,
		16,
		16,
		15,
		15,
		15,
		14,
		14,
		14,
		13,
		13,
		13,
		13,
		12,
		12,
		12,
		11,
		11,
		11,
		10,
		10,
		10,
		10,
		9,
		9,
		9,
		8,
		8,
		8,
		7,
		7,
		7,
		7,
		6,
		6,
		6,
		5,
		5,
		5,
		4,
		4,
		4,
		4,
		3,
		3,
		3,
		2,
		2,
		2,
		1,
		1,
		1,
		1,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		1,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		2,
		6,
		10,
		14,
		18,
		22,
		26,
		30,
		34,
		38,
		42,
		46,
		50,
		54,
		58,
		62,
		66,
		70,
		74,
		78,
		82,
		86,
		90,
		94,
		98,
		102,
		106,
		110,
		114,
		118,
		122,
		126,
		130,
		134,
		138,
		142,
		146,
		150,
		154,
		158,
		162,
		166,
		170,
		174,
		178,
		182,
		186,
		190,
		194,
		198,
		202,
		206,
		210,
		214,
		218,
		222,
		226,
		230,
		234,
		238,
		242,
		246,
		250,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		255,
		251,
		247,
		244,
		240,
		236,
		232,
		228,
		225,
		221,
		217,
		213,
		209,
		206,
		202,
		198,
		194,
		190,
		186,
		183,
		179,
		175,
		171,
		167,
		164,
		160,
		156,
		152,
		148,
		145,
		141,
		137,
		133,
		129,
		126,
		122,
		118,
		114,
		110,
		107,
		103,
		99,
		95,
		91,
		88,
		84,
		80,
		76,
		72,
		69,
		65,
		61,
		57,
		53,
		49,
		46,
		42,
		38,
		34,
		30,
		27,
		23,
		19,
		15,
		11,
		8,
		4,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		0,
		8,
		16,
		24,
		32,
		40,
		48,
		56,
		64,
		72,
		80,
		88,
		96,
		104,
		112,
		120,
		128,
		136,
		144,
		152,
		160,
		168,
		176,
		184,
		192,
		200,
		208,
		216,
		224,
		232,
		240,
		248,
		255
};

#endif
