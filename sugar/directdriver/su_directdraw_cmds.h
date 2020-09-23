/*LIC-HDR********************************************************************
**
** Copyright (C) 2001-2008 AtOnce Technologies
**
** This file is part of the Sugar Framework.
**
** This file and the associated product may be used and distributed
** under the terms of a current License as defined by AtOnce Technologies
** and appearing in the file LICENSE.TXT included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See http://www.atonce-technologies.com or email info@atonce-technologies.com
** for informations about Sugar Framework and AtOnce Technologies others products
** License Agreements.
**
** Contact info@atonce-technologies.com if any conditions of this license
** are not clear to you.
**
*****************************************************************LIC-HDR*/



#ifndef _su_ddraw_cmds_h_
#define _su_ddraw_cmds_h_



namespace su { namespace dd
{

//
// SU_DD_BREAK: bit mask to set for breaking before any upcode
//
#define SU_DD_BREAK						(1<<31)


//
// CMD_END: End of command list
//
// When this command is read, the command list is ended.
// Then the ddraw engine resumes the caller command list from the shading callstack, if exists.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0|
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o

#define SU_DD_END_CODE					0
#define SU_DD_END_REV					1
#define SU_DD_END()						(SU_DD_CMD_DEFINE(SU_DD_END_CODE))




//
// CMD_NOP: No operation
//
// This End does nothing.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0|
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o

#define	SU_DD_NOP_CODE					1
#define SU_DD_NOP_REV					1
#define SU_DD_NOP()						(SU_DD_CMD_DEFINE(SU_DD_NOP_CODE))




//
// CMD_FLUSH: Flush the commands pipeline.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0|
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o

#define SU_DD_FLUSH_CODE				2
#define SU_DD_FLUSH_REV					1
#define SU_DD_FLUSH()					(SU_DD_CMD_DEFINE(SU_DD_FLUSH_CODE))




//
// CMD_BRA: Branch always.
//
// This an unconditionnal branchment
// ip = ip + (OFFSET+1)*4
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        |                               OFFSET                                  |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  OFFSET       : Signed branch offset
// ----------------------------------------------------------------------------

#define SU_DD_BRA_CODE					3
#define SU_DD_BRA_REV					1
#define SU_DD_BRA(_offst)				(SU_DD_CMD_DEFINE(SU_DD_BRA_CODE)|(uint(_offst)&0xffffff))




//
// CMD_JMP: Jump
//
// Unconditional jump with a 32 bits address.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        |                                                                       |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                             ADDR                                              |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  ADDR         : Memory address of the jump destination.
// ----------------------------------------------------------------------------

#define SU_DD_JMP_CODE					4
#define SU_DD_JMP_REV					2
#define SU_DD_JMP_1()					(SU_DD_CMD_DEFINE(SU_DD_JMP_CODE))
#define SU_DD_JMP_2(_addr)				(uint(_addr))




//
// CMD_CALL: List call.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        |                                                                       |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                             ADDR                                              |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  ADDR         : Memory address of the call destination.
// ----------------------------------------------------------------------------

#define SU_DD_CALL_CODE					5
#define SU_DD_CALL_REV					2
#define SU_DD_CALL_1()					(SU_DD_CMD_DEFINE(SU_DD_CALL_CODE))
#define SU_DD_CALL_2(_addr)				(uint(_addr))




//
// CMD_RET: List return.
//
// When this command is read, the engine returns from the called list back to the caller code.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0|
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o

#define SU_DD_RET_CODE					6
#define SU_DD_RET_REV					1
#define SU_DD_RET()						(SU_DD_CMD_DEFINE(SU_DD_RET_CODE))




//
// CMD_FUNC: Set trap user function.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0|         COUNT         |           FI          |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// |                                              UFA                                              |
// o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  FI           : The function index to write in the trap table
//				 : This value must be defined in the range 0 to 255
// ----------------------------------------------------------------------------
//  COUNT        : Number of vectors to be written back
// ----------------------------------------------------------------------------
//  FA           : Function Address, as a UserFunc object pointer
// ----------------------------------------------------------------------------

#define SU_DD_FUNC_CODE					7
#define SU_DD_FUNC_REV					2
#define SU_DD_FUNC_1(_fi,_count)		(SU_DD_CMD_DEFINE(SU_DD_FUNC_CODE)|((_count)<<8)|(_fi))
#define SU_DD_FUNC_n(_fa)				(uint(_fa))

struct UserFunc
{
	virtual uint* Action ( uint* ip, uint opcode, void* ctxt ) = 0;
};




//
// CMD_TRAP: Trap a user function.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// |BR|        CODE        |                     PARAM                     |           FI          |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  FI           : The function index to trap, in the range 0 to 255
// ----------------------------------------------------------------------------
//  PARAM        : A 16bits unsigned integer user argument
// ----------------------------------------------------------------------------

#define SU_DD_TRAP_CODE					8
#define SU_DD_TRAP_REV					1
#define SU_DD_TRAP(_fi,_param)			(SU_DD_CMD_DEFINE(SU_DD_TRAP_CODE)|(((_param)&0xffff)<<8)|(_fi))




//
// CMD_SHADE: Set the current shading node
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0|          SI           |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  SI           : Shader index selected in the shading tree
// ----------------------------------------------------------------------------

#define SU_DD_SHADE_CODE				20
#define SU_DD_SHADE_REV					1
#define SU_DD_SHADE(_si)				(SU_DD_CMD_DEFINE(SU_DD_SHADE_CODE)|(_si))




//
// CMD_TARGET: Set the current target.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0|                       PBI                     |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  PBI          : Pixel Buffer Index
// ----------------------------------------------------------------------------

#define SU_DD_TARGET_CODE				21
#define SU_DD_TARGET_REV				1
#define SU_DD_TARGET(_pbi)				(SU_DD_CMD_DEFINE(SU_DD_TARGET_CODE)|((_pbi)&0xffff))




//
// CMD_TEXTURE: Set the current texture.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- --o--o-- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0|BS|    STG    |                       PBI                     |
// o--o-- -- -- -- -- -- --o-- -- --o--o-- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  PBI          : Pixel Buffer Index
// ----------------------------------------------------------------------------
//  STG          : Stage index. A Zero-based value indicating the stage
//               : to which the texture is assigned
// ----------------------------------------------------------------------------
//  BS           : Buffer Selector
//               : (0) Pixel buffer
//               : (1) Depth buffer
// ----------------------------------------------------------------------------

#define SU_DD_TEXTURE_CODE					22
#define SU_DD_TEXTURE_REV					1
#define SU_DD_TEXTURE(_pbi,_stg,_bs)		(SU_DD_CMD_DEFINE(SU_DD_TEXTURE_CODE)|(_bs<<20)|(((_stg)&0xf)<<16)|((_pbi)&0xffff))




//
// CMD_SCISSOR: Set scissor area.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|       CODE         |                  SY               |               SX                  |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// | 0  0  0  0  0  0  0  0|                  SH               |               SW                  |
// o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  SX           : X coordinate of the upper-left corner of the scissoring area
//               : 12 unsigned integer, from 0 to 4095
// ----------------------------------------------------------------------------
//  SY           : Y coordinate of the upper-left corner of the scissoring area
//               : 12 unsigned integer, from 0 to 4095
// ----------------------------------------------------------------------------
//  SW           : Width of the scissoring area in pixel
//               : 12 unsigned integer, from 0 to 4095
// ----------------------------------------------------------------------------
//  SH           : Height of the scissoring area in pixel
//               : 12 unsigned integer, from 0 to 4095
// ----------------------------------------------------------------------------

#define SU_DD_SCISSOR_CODE				23
#define SU_DD_SCISSOR_REV				1
#define SU_DD_SCISSOR_1(_sx,_sy)		(SU_DD_CMD_DEFINE(SU_DD_SCISSOR_CODE)|((_sy)<<12)|(_sx))
#define SU_DD_SCISSOR_2(_sw,_sh)		(((_sh)<<12) | (_sw))


//
// CMD_VIEWPORT: Set viewport area.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|       CODE         |                  SY               |               SX                  |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// | 0  0  0  0  0  0  0  0|                  SH               |               SW                  |
// o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  SX           : X coordinate of the upper-left corner of the scissoring area
//               : 12 unsigned integer, from 0 to 4095
// ----------------------------------------------------------------------------
//  SY           : Y coordinate of the upper-left corner of the scissoring area
//               : 12 unsigned integer, from 0 to 4095
// ----------------------------------------------------------------------------
//  SW           : Width of the scissoring area in pixel
//               : 12 unsigned integer, from 0 to 4095
// ----------------------------------------------------------------------------
//  SH           : Height of the scissoring area in pixel
//               : 12 unsigned integer, from 0 to 4095
// ----------------------------------------------------------------------------

#define SU_DD_VIEWPORT_CODE				24
#define SU_DD_VIEWPORT_REV				1
#define SU_DD_VIEWPORT_1(_sx,_sy)		(SU_DD_CMD_DEFINE(SU_DD_VIEWPORT_CODE)|((_sy&0x7ff)<<12)|(_sx&0x7ff))
#define SU_DD_VIEWPORT_2(_sw,_sh)		(((_sh&0x7ff)<<12) | (_sw&0x7ff))




//
// CMD_CLEAR: Clear area.
//
// Clear the current drawing area. This operation is performed according to the scissoring area.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- --o--o--o-- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0  0  0  0  0  0  0|DE|CE|           CI          |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- --o--o--o-- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  CI           : Color index referenced in the shared CLUT
//               : in the range 0 to 255
// ----------------------------------------------------------------------------
//  CE           : Color enable
//               : (0) Disable color buffer clearing
//               : (1) Enable color buffer clearing
// ----------------------------------------------------------------------------
//  DE           : Depth enable
//               : (0) Disable depth buffer clearing
//               : (1) Enable depth buffer clearing
// ----------------------------------------------------------------------------

#define SU_DD_CLEAR_CODE					25
#define SU_DD_CLEAR_REV						1
#define SU_DD_CLEAR(_ci,_ce,_de)			(SU_DD_CMD_DEFINE(SU_DD_CLEAR_CODE)|((_de)<<9)|((_ce)<<8)|(_ci))




//
// CMD_COLOR: Set color.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0|         COUNT         |           CI          |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// |           A           |           B           |           G           |            R          |
// o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  CI           : Color index to write in the shared CLUT
//               : in the range 0 to 255
// ----------------------------------------------------------------------------
//  COUNT        : Number of colors to write
// ----------------------------------------------------------------------------
//  R            : Red component (0 to 255 range)
// ----------------------------------------------------------------------------
//  G            : Green component (0 to 255 range)
// ----------------------------------------------------------------------------
//  B            : Blue component (0 to 255 range)
// ----------------------------------------------------------------------------
//  A            : Alpha component (0 to 255 range)
// ----------------------------------------------------------------------------

#define SU_DD_COLOR_CODE					26
#define SU_DD_COLOR_REV						1
#define SU_DD_COLOR_1(_ci,_count)			(SU_DD_CMD_DEFINE(SU_DD_COLOR_CODE)|((_count)<<8)|(_ci))
#define SU_DD_COLOR_n(_r,_g,_b,_a)			(((_a)<<24)|((_b)<<16)|((_g)<<8)|((_r)<<0))





//
// CMD_RENDERSTATE:
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o--o
// |BR|        CODE        |                              COUNT                                 |PM|
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o--o
//                                                 +
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                              ???                                              |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//                                                 +
//                                                ...
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  COUNT        : Number of render state to setup.
// ----------------------------------------------------------------------------
//  PM           : Packing Mode
//               : (0) Immediate: Render state's values follow ( cf SU_DD_RS_XXX ).
//               : (1) Pointer: A C pointer on the render state's values follow.
// ----------------------------------------------------------------------------

#define SU_DD_RENDERSTATE_CODE					27
#define SU_DD_RENDERSTATE_REV					1
#define SU_DD_RENDERSTATE_1(_count,_pm)			(SU_DD_CMD_DEFINE(SU_DD_RENDERSTATE_CODE)|((_count)<<1)|(_pm))
#define SU_DD_RENDERSTATE_reg(_rv)				(su::util::asuint32(_rv))
#define SU_DD_RENDERSTATE_addr(_a)				(su::util::asuint32(_a))







//
// CMD_BEGINFRAME: Begin the frame construction
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0|
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o

#define SU_DD_BEGINFRAME_CODE				30
#define SU_DD_BEGINFRAME_REV				1
#define SU_DD_BEGINFRAME()					(SU_DD_CMD_DEFINE(SU_DD_BEGINFRAME_CODE))




//
// CMD_ENDFRAME: Finish the frame construction
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0|
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o

#define SU_DD_ENDFRAME_CODE					31
#define SU_DD_ENDFRAME_REV					1
#define SU_DD_ENDFRAME()					(SU_DD_CMD_DEFINE(SU_DD_ENDFRAME_CODE))




//
// CMD_PRESENT: Present a view back-buffer to the user display
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0|                       PBI                     |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  PBI          : Pixel Buffer Index
// ----------------------------------------------------------------------------

#define SU_DD_PRESENT_CODE					32
#define SU_DD_PRESENT_REV					2
#define SU_DD_PRESENT(_pbi)					(SU_DD_CMD_DEFINE(SU_DD_PRESENT_CODE)|((_pbi)&0xffff))




//
// CMD_READPIXELS: Get a pixel area.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|       CODE         |                  SY               |               SX                  |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// | 0  0  0  0  0  0  0  0|                  SH               |               SW                  |
// o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// | 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0|                     PBI                       |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                             ADDR                                              |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  X            : X coordinate of the upper-left corner of the area
//               : 12 unsigned integer, from 0 to 4095
// ----------------------------------------------------------------------------
//  Y            : Y coordinate of the upper-left corner of the area
//               : 12 unsigned integer, from 0 to 4095
// ----------------------------------------------------------------------------
//  W            : Width of the area in pixel
//               : 12 unsigned integer, from 0 to 4095
// ----------------------------------------------------------------------------
//  H            : Height of the area in pixel
//               : 12 unsigned integer, from 0 to 4095
// ----------------------------------------------------------------------------
//  PBI          : pixel buffer index
// ----------------------------------------------------------------------------
//  ADDR         : pointer on the out pixel data which will be filled during the call
// ----------------------------------------------------------------------------

#define SU_DD_READPIXELS_CODE				33
#define SU_DD_READPIXELS_REV				1
#define SU_DD_READPIXELS_1(_sx,_sy)			(SU_DD_CMD_DEFINE(SU_DD_READPIXELS_CODE)|((_sy)<<12)|(_sx))
#define SU_DD_READPIXELS_2(_sw,_sh)			(((_sh)<<12) | (_sw))
#define SU_DD_READPIXELS_3(_pbi)			(_pbi)
#define SU_DD_READPIXELS_4(_addr)			(uint(_addr))




//
// CMD_VBOCREATE: Create a vertex buffer object.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- --o--o-- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0|DU|   PTYPE   |                      VF                          |
// o--o-- -- -- -- -- -- --o-- --o--o-- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                             COUNT                                             |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                             IADDR                                             |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  VF           : Vertex format of the vbo
//               : cf enum dd::VertexFormat
// ----------------------------------------------------------------------------
//  PTYPE        : primitive type
//               : cf enum dd::PrimType
// ----------------------------------------------------------------------------
//  DU           : Dynamic Usage
//               : (0) Static
//               : (1) Dynamic
// ----------------------------------------------------------------------------
//  COUNT        : Vertex count
// ----------------------------------------------------------------------------
//  IADDR        : pointer on the buffer id which will be filled during the call
// ----------------------------------------------------------------------------

#define SU_DD_VBOCREATE_CODE				37
#define SU_DD_VBOCREATE_REV					2
#define SU_DD_VBOCREATE_1(_vf,_ptype,_du)	(SU_DD_CMD_DEFINE(SU_DD_VBOCREATE_CODE)|((_du)<<21)|((_ptype)<<17)|(_vf))
#define SU_DD_VBOCREATE_2(_co)				(_co)
#define SU_DD_VBOCREATE_3(_iaddr)			(uint(_iaddr))




//
// CMD_VBOLOAD: Create a vbo and load it with input data.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        |                                                                       |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                            INTADDR                                            |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                            IDADDR                                             |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ---------------------------------------------------------------------------------
//  INTADDR      : Pointer on the address corresponding to a rsc_collection_mesh_vbo
// ---------------------------------------------------------------------------------
//  IDADDR       : Pointer on the buffer id which will be filled during the call
// ---------------------------------------------------------------------------------

#define SU_DD_VBOLOAD_CODE					38
#define SU_DD_VBOLOAD_REV					1
#define SU_DD_VBOLOAD_1()					(SU_DD_CMD_DEFINE(SU_DD_VBOLOAD_CODE))
#define SU_DD_VBOLOAD_2(_intaddr)			(uint(_intaddr))
#define SU_DD_VBOLOAD_3(_idaddr)			(uint(_idaddr))


//
// CMD_VBOFILL: Update the whole content of a vertex buffer object.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0|		              VBO                      |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// | 0  0  0  0  0  0  0  0|                                  VF                                   |
// o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                              INVTX                                            |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  VBO          : Vertex Buffer Object index
// ----------------------------------------------------------------------------
//  VF           : Vertex format of the vbo
//               : cf enum dd::VertexFormat
// ----------------------------------------------------------------------------
//  INVTX        : Address of the vbo data
// ----------------------------------------------------------------------------

#define SU_DD_VBOFILL_CODE					39
#define SU_DD_VBOFILL_REV					1
#define SU_DD_VBOFILL_1(_vbo)				(SU_DD_CMD_DEFINE(SU_DD_VBOFILL_CODE)|((_vbo)&0xffff))
#define SU_DD_VBOFILL_2(_vf)				(_vf)
#define SU_DD_VBOFILL_3(_addr)				(uint(_addr))



//
// CMD_VBOUPDATE: Update content of a vertex buffer object.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0|		              VBO                      |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// | 0  0  0  0  0  0  0  0|                                  COUNT                                |
// o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// | 0  0  0  0  0  0  0  0|                                  VOFFS                                |
// o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// | 0  0  0  0  0  0  0  0|                                  VF                                   |
// o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                              INVTX                                            |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  VBO          : Vertex Buffer Object Index
// ----------------------------------------------------------------------------
//  COUNT        : Vertex count
// ----------------------------------------------------------------------------
//  VOFFS        : Vertex offset
// ----------------------------------------------------------------------------
//  VF           : Vertex format of the vbo
//               : cf enum dd::VertexFormat
// ----------------------------------------------------------------------------
//  INVTX        : Address of the vbo data
// ----------------------------------------------------------------------------

#define SU_DD_VBOUPDATE_CODE					40
#define SU_DD_VBOUPDATE_REV						1
#define SU_DD_VBOUPDATE_1(_vbo)					(SU_DD_CMD_DEFINE(SU_DD_VBOUPDATE_CODE)|((_vbo)&0xffff))
#define SU_DD_VBOUPDATE_2(_vcount)				(_vcount)
#define SU_DD_VBOUPDATE_3(_voffset)				(_voffset)
#define SU_DD_VBOUPDATE_4(_vf)					(_vf)
#define SU_DD_VBOUPDATE_5(_addr)				(uint(_addr))





//
// CMD_PCREATE: Create a texture.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o--o--o--o--o-- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// |BR|        CODE        |DX|DT|RT|DU|    PU     |            HE         |           WI          |
// o--o-- -- -- -- -- -- --o--o--o--o--o-- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// |                                              IADDR                                            |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  WI           : Surface width /16
// ----------------------------------------------------------------------------
//  HE           : Surface height /16
// ----------------------------------------------------------------------------
//  PU           : Pixel usage
//               : (0) lossless with alpha component (RGBA)
//               : (1) lossless without alpha component (RGB)
//               : (2) low compression
//               : (3) medium compression
//               : (4) high compression
//               : (5) Luminance
//               : (6) ... [hardware specific]
//               : (15) ... [hardware specific]
// ----------------------------------------------------------------------------
//  DU           : Dynamic usage
//               : (0) Static
//               : (1) Dynamic
// ----------------------------------------------------------------------------
//  RT           : Render Target
//               : (0) Disable
//               : (1) Enable
// ----------------------------------------------------------------------------
//  DT           : Depth Target
//               : (0) Disable
//               : (1) Enable
// ----------------------------------------------------------------------------
//  DX           : Depth texture
//               : (0) Disable
//               : (1) Enable
// ----------------------------------------------------------------------------
//  IADDR        : Pointer on the buffer id which will be filled during the call
// ----------------------------------------------------------------------------

#define SU_DD_PCREATE_CODE									42
#define SU_DD_PCREATE_REV									2
#define SU_DD_PCREATE_1(_wi,_he,_pu,_du,_rt,_dt,_dx)		(SU_DD_CMD_DEFINE(SU_DD_PCREATE_CODE)|((_dx)<<23)|((_dt)<<22)|((_rt)<<21)|((_du)<<20)|((_pu)<<16)|((_he)<<8)|((_wi)<<0))
#define SU_DD_PCREATE_2(_iaddr)								(uint(_iaddr))



//
// CMD_PLOAD: Create a texture and load it with input data.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        |                                                                       |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                            INTADDR                                            |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                            IDADDR                                             |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ---------------------------------------------------------------------------------
//  INTADDR      : Pointer on the address corresponding to a rsc_collection_bitmap
// ---------------------------------------------------------------------------------
//  IDADDR       : Pointer on the buffer id which will be filled during the call
// ---------------------------------------------------------------------------------

#define SU_DD_PLOAD_CODE					43
#define SU_DD_PLOAD_REV						3
#define SU_DD_PLOAD_1()						(SU_DD_CMD_DEFINE(SU_DD_PLOAD_CODE))
#define SU_DD_PLOAD_2(_intaddr)				(uint(_intaddr))
#define SU_DD_PLOAD_3(_idaddr)				(uint(_idaddr))


//
// CMD_PFORMAT: Get the texture Pixel Storage Mode
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// |BR|        CODE        |                       |                      PBI                      |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// |                                              OPSM                                             |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  PBI          : Pixel Buffer Index
// ----------------------------------------------------------------------------
//  OPSM         : Pointer on the psm which will be filled during the call
// ----------------------------------------------------------------------------

#define SU_DD_PFORMAT_CODE					44
#define SU_DD_PFORMAT_REV					1
#define SU_DD_PFORMAT_1(_pbi)				(SU_DD_CMD_DEFINE(SU_DD_PFORMAT_CODE)|(_pbi&0xffff))
#define SU_DD_PFORMAT_2(_opsm)				(uint(_opsm))







//
// CMD_WCREATE: Create a windowed graphical resource. The generated resource id is a valid PBI for the CMD_TARGET command.
//			    The XID parameter is optional.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        |               WI                  |               HE                  |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- --o
// |                                               XID                                             |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                              IDADDR                                           |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ---------------------------------------------------------------------------------
//  WI           : Width
// ---------------------------------------------------------------------------------
//  HE           : Height
// ---------------------------------------------------------------------------------
//  XID          : OS specific window data
//               : win -> HWND
//               : ios -> UIView*
//               : mac -> NSView*
//               : if NULL is given a window is created with the specified dimensions
// ---------------------------------------------------------------------------------
//  IADDR        : Pointer on the resource id which will be filled during the call
// ---------------------------------------------------------------------------------

#define SU_DD_WCREATE_CODE					45
#define SU_DD_WCREATE_REV					1
#define SU_DD_WCREATE_1(_w,_h)				(SU_DD_CMD_DEFINE(SU_DD_WCREATE_CODE)|(((_w)&0xfff)<<12)|((_h)&0xfff))
#define SU_DD_WCREATE_2(_xid)				(uint(_xid))
#define SU_DD_WCREATE_3(_iaddr)				(uint(_iaddr))


//
// CMD_WUPDATE: Update window for match back buffer size frame buffer size
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0|                      WID                      |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o

#define SU_DD_WUPDATE_CODE					46
#define SU_DD_WUPDATE_REV					2
#define SU_DD_WUPDATE( _wid )				(SU_DD_CMD_DEFINE(SU_DD_WUPDATE_CODE) | (_wid&0xffff))



//
// CMD_PUPDATE: Update a pixel buffer.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- --o-- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        |   LEV  |     PSM      |                      PBI                      |
// o--o-- -- -- -- -- -- --o-- -- --o-- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                       OY                      |                      OX                       |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                       HE                      |                      WI                       |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                             INADDR                                            |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  PBI          : Pixel Buffer id
// ----------------------------------------------------------------------------
//  PSM          : Pixel Storage Mode
//               : A value from the Psm enumeration type (see the file su_psm.h)
// ----------------------------------------------------------------------------
//  LEV          : Mipmap level, in the range [0,7]
// ----------------------------------------------------------------------------
//  OX           : X-offset of the area where the update begins in the texture
// ----------------------------------------------------------------------------
//  OY           : Y-offset of the area where the update begins in the texture
// ----------------------------------------------------------------------------
//  WI           : Width of the area to be updated
// ----------------------------------------------------------------------------
//  HE           : Height of the area to be updated
// ----------------------------------------------------------------------------
//  INADDR       : Address of the data
// ----------------------------------------------------------------------------

#define SU_DD_PUPDATE_CODE						49
#define SU_DD_PUPDATE_REV						3
#define SU_DD_PUPDATE_1(_pbi,_psm,_lev)			(SU_DD_CMD_DEFINE(SU_DD_PUPDATE_CODE)|((_lev)<<21)|((_psm)<<16)|((_pbi)&0xffff))
#define SU_DD_PUPDATE_2(_ox,_oy)				(((_oy)<<16)|(_ox))
#define SU_DD_PUPDATE_3(_wi,_he)				(((_he)<<16)|(_wi))
#define SU_DD_PUPDATE_4(_addr)					(uint(_addr))



//
// CMD_SURFACE: Bind a program with a vertex buffer
// 
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- --o-- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0|  VBCOUNT  |                     PRGI                      |
// o--o-- -- -- -- -- -- --o-- -- -- --o-- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                             IADDR                                             |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//                                                 +
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                       VF                      |                      VBI                      |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  VBCOUNT      : Vertex-Buffer Count
// ----------------------------------------------------------------------------
//  PRGI         : Program index
// ----------------------------------------------------------------------------
//  IADDR        : Pointer on the buffer id which will be filled during the call
// -----------------------------------------------------------------------------
//  VBI          : Vertex-Buffer Index
// ----------------------------------------------------------------------------
//  VF           : Vertex-Format used or zero for global mapping
// ----------------------------------------------------------------------------

#define SU_DD_SURFACE_CODE						50
#define SU_DD_SURFACE_REV						1
#define SU_DD_SURFACE_1(_vbc,_pri)				(SU_DD_CMD_DEFINE(SU_DD_SURFACE_CODE)|(_vbc<<16)|(_pri&0xffff))
#define SU_DD_SURFACE_2(_iaddr)					(uint(_iaddr))
#define SU_DD_SURFACE_n(_vbi,_vf)				((_vf<<16)|(_vbi&0xffff))




//
// CMD_CONSTANT: Set constants by value.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- --o--o-- -- --o-- -- --o-- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0  0|PM|   VN   |   VL   |        COUNT          |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- --o--o-- -- --o-- -- --o-- -- -- -- -- -- -- --o
// |                                                                                               |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//                                                 +
// o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
// |          ??           |          ??           |           ??          |           ??          |
// o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- --o
//                                                 +
//                                                ...
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  COUNT        : Number of vector to unpack
// ----------------------------------------------------------------------------
//  VL           : Vector Length
//               : (0) IEEE 754 float
//               : (1) signed 32bits
//               : (2) signed 16bits
//               : (3) signed 8bits
//               : (4) unsigned 32bits
//               : (5) unsigned 16bits
//               : (6) unsigned 8bits
// ----------------------------------------------------------------------------
//  VN           : Vector Number of elements
//               : (0) reserved
//               : (1) V1
//               : (2) V2
//               : (3) V3
//               : (4) V4
// ----------------------------------------------------------------------------
//  PM           : Packing Mode
//               : (0) Immediate: Component's values follow.
//               : (1) Pointer: A C pointer on the component's values follow
// ----------------------------------------------------------------------------

#define SU_DD_CONSTANT_CODE								52
#define SU_DD_CONSTANT_REV								3
#define SU_DD_CONSTANT_1(_count,_vl,_vn,_pm)			(SU_DD_CMD_DEFINE(SU_DD_CONSTANT_CODE)|((_pm)<<14)|((_vn)<<11)|((_vl)<<8)|(_count))
#define SU_DD_CONSTANT_2(_crc)							(_crc)
#define SU_DD_CONSTANT_n(_v)							(su::util::asuint32(_v))




//
// CMD_PROGRAMLOAD: Create and load a program shader from input map data.
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- --o
// |BR|        CODE        |                                                                 | DT  |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- --o
// |                                             INDATA                                            |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                             IDADDR                                            |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// -----------------------------------------------------------------------------------
//  DT           : Type of the INDATA
//               :  - 0 -> pointer on a tag rsc_collection_shading_effect
//               :  - 1 -> pointer on a single *PIXEL-SHADER* source code ( glsl, hlsl according IDirectDraw implementation )
// -----------------------------------------------------------------------------------
//  INDATA       : Pointer on the input data
// -----------------------------------------------------------------------------------
//  IDADDR       : Pointer on the buffer id array which will be filled during the call
// -----------------------------------------------------------------------------------

#define	SU_DD_PROGRAMLOAD_CODE						55
#define SU_DD_PROGRAMLOAD_REV						2
#define SU_DD_PROGRAMLOAD_1(_dtype)					(SU_DD_CMD_DEFINE(SU_DD_PROGRAMLOAD_CODE)|(_dtype))
#define SU_DD_PROGRAMLOAD_2(_indata)				(uint(_indata))
#define SU_DD_PROGRAMLOAD_3(_idaddr)				(uint(_idaddr))




//
// CMD_PRIM: Output a graphic primitive
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0|                     SRFI                      |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ------------------------------------------------------------------------------
//  SRFI         : Index of a SURFACE resource
// ------------------------------------------------------------------------------

#define SU_DD_PRIM_CODE					60
#define SU_DD_PRIM_REV					3
#define SU_DD_PRIM(_srfi)				(SU_DD_CMD_DEFINE(SU_DD_PRIM_CODE)|(_srfi&0xffff))




// CMD_PRIMIMM: Output a graphic primitive
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0|                      VBOI                     |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// | 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0|                      PRGI                     |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ------------------------------------------------------------------------------
//  SRFI         : Index of a SURFACE resource
// ------------------------------------------------------------------------------

#define SU_DD_PRIMIMM_CODE				61
#define SU_DD_PRIMIMM_REV				1
#define SU_DD_PRIMIMM_1(_vboi)			(SU_DD_CMD_DEFINE(SU_DD_PRIMIMM_CODE)|(_vboi&0xffff))
#define SU_DD_PRIMIMM_2(_progi)			(_progi&0xffff)




//
// CMD_CAMERA: Take advantage of the camera
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- --o-- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0|   MODE    |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- --o-- -- -- --o
// |                                             OCDESC                                            |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  MODE         : Usage mode
//               : (0) Off : turn off the camera
//               : (1) Prepare : allocates internal memory
//               : (2) Grab : grab the camera data
// ----------------------------------------------------------------------------
//  OADDR        : pointer on a CameraDescriptor which will be filled during the call
// ----------------------------------------------------------------------------

#define SU_DD_CAMERA_CODE						65
#define SU_DD_CAMERA_REV						3
#define SU_DD_CAMERA_1(_mode)					(SU_DD_CMD_DEFINE(SU_DD_CAMERA_CODE)|(_mode))
#define SU_DD_CAMERA_2(_oaddr)					(uint(_oaddr))





//
// CMD_TILEGROUP: Rasterize a list of tiles
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- --o-- -- -- --o
// |BR|        CODE        | 0  0  0  0  0  0  0  0  0  0  0  0  0  0  0  0|    DIR    |    GRP    |
// o--o-- -- -- -- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o-- -- -- --o-- -- -- --o
// |                                             PARAM                                             |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  GRP          : Group id
// ----------------------------------------------------------------------------
//  DIR          : TileGroupDirective (see enumerations)
// ----------------------------------------------------------------------------
//  PARAM        : The directive optional parameter
//				 : At the moment, only the RESERVE directive requires a parameter.
//				 : The RESERVE parameter value is given in amount of tiles.
// ----------------------------------------------------------------------------


#define SU_DD_TILEGROUP_CODE				70
#define SU_DD_TILEGROUP_REV					2
#define SU_DD_TILEGROUP_1(_grp,_dir)        (SU_DD_CMD_DEFINE(SU_DD_TILEGROUP_CODE)| (((_dir)&0xf)<<4)|((_grp)&0xf))
#define SU_DD_TILEGROUP_2_Dirty()           (0)
#define SU_DD_TILEGROUP_2_Reserve(_count)   (_count)
#define SU_DD_TILEGROUP_2_Reset()           (0)
#define SU_DD_TILEGROUP_2_Locate(_x,_y)     ( ((_y)<<16) | ((_x)<<0) )



//
// CMD_TILE: Rasterize a list of tiles
//
//  31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00
// o--o-- -- -- -- -- -- --o-- -- -- --o-- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |BR|        CODE        | 0  0  0  0|    GRP    |                      PBI                      |
// o--o-- -- -- -- -- -- --o-- -- -- --o-- -- -- --o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
// |                                              TLA                                              |
// o-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --o
//
//  Field name   : Description
// ----------------------------------------------------------------------------
//  GRP          : Group id of the current tile list
// ----------------------------------------------------------------------------
//  PBI          : Pixel Buffer Index as the tile source
// ----------------------------------------------------------------------------
//  TLA          : Tiles list address (TileList* pointer)
// ----------------------------------------------------------------------------

#define SU_DD_TILE_CODE						71
#define SU_DD_TILE_REV						3
#define SU_DD_TILE_1(_grp,_pbi)				(SU_DD_CMD_DEFINE(SU_DD_TILE_CODE)|(((_grp)&0xf)<<16)|((_pbi)&0xffff))
#define SU_DD_TILE_2(_tla)					(uint(_tla))


struct TileMap
{
	short		x, y;		// in texels
	short		w, h;		// in texels
};

struct TilePose
{
	ushort		mi;			// source index in the map
	short		x, y;		// in pixels
	ushort		w, h;		// in pixels
	Color32		color;		// fragment color
};

struct TileList
{
	ushort		mcount;
	TileMap*	mlist;
	ushort		pcount;
	TilePose*	plist;
};


} }



#endif // _su_ddraw_cmds_h_



