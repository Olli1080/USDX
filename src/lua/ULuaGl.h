#pragma once
/* UltraStar Deluxe - Karaoke Game
 *
 * UltraStar Deluxe is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 */
#include <map>

#include "../switches.h"

#include <sol/sol.hpp>
#include <gl/GL.h>

namespace ULuaGl
{

#define MACRO_GL_KEY_VALUE(s) { #s, s }
const std::map<std::string, int> gl_map = {
    //(Text:"GL_VERSION_1_1";Value:GL_VERSION_1_1),
    MACRO_GL_KEY_VALUE(GL_ACCUM),
    MACRO_GL_KEY_VALUE(GL_LOAD),
    MACRO_GL_KEY_VALUE(GL_RETURN),
    MACRO_GL_KEY_VALUE(GL_MULT),
    MACRO_GL_KEY_VALUE(GL_ADD),
    MACRO_GL_KEY_VALUE(GL_NEVER),
    MACRO_GL_KEY_VALUE(GL_LESS),
    MACRO_GL_KEY_VALUE(GL_EQUAL),
    MACRO_GL_KEY_VALUE(GL_LEQUAL),
    MACRO_GL_KEY_VALUE(GL_GREATER),
    MACRO_GL_KEY_VALUE(GL_NOTEQUAL),
    MACRO_GL_KEY_VALUE(GL_GEQUAL),
    MACRO_GL_KEY_VALUE(GL_ALWAYS),
    MACRO_GL_KEY_VALUE(GL_CURRENT_BIT),
    MACRO_GL_KEY_VALUE(GL_POINT_BIT),
    MACRO_GL_KEY_VALUE(GL_LINE_BIT),
    MACRO_GL_KEY_VALUE(GL_POLYGON_BIT),
    MACRO_GL_KEY_VALUE(GL_POLYGON_STIPPLE_BIT),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MODE_BIT),
    MACRO_GL_KEY_VALUE(GL_LIGHTING_BIT),
    MACRO_GL_KEY_VALUE(GL_FOG_BIT),
    MACRO_GL_KEY_VALUE(GL_DEPTH_BUFFER_BIT),
    MACRO_GL_KEY_VALUE(GL_ACCUM_BUFFER_BIT),
    MACRO_GL_KEY_VALUE(GL_STENCIL_BUFFER_BIT),
    MACRO_GL_KEY_VALUE(GL_VIEWPORT_BIT),
    MACRO_GL_KEY_VALUE(GL_TRANSFORM_BIT),
    MACRO_GL_KEY_VALUE(GL_ENABLE_BIT),
    MACRO_GL_KEY_VALUE(GL_COLOR_BUFFER_BIT),
    MACRO_GL_KEY_VALUE(GL_HINT_BIT),
    MACRO_GL_KEY_VALUE(GL_EVAL_BIT),
    MACRO_GL_KEY_VALUE(GL_LIST_BIT),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_BIT),
    MACRO_GL_KEY_VALUE(GL_SCISSOR_BIT),
    MACRO_GL_KEY_VALUE(GL_ALL_ATTRIB_BITS),
    MACRO_GL_KEY_VALUE(GL_POINTS),
    MACRO_GL_KEY_VALUE(GL_LINES),
    MACRO_GL_KEY_VALUE(GL_LINE_LOOP),
    MACRO_GL_KEY_VALUE(GL_LINE_STRIP),
    MACRO_GL_KEY_VALUE(GL_TRIANGLES),
    MACRO_GL_KEY_VALUE(GL_TRIANGLE_STRIP),
    MACRO_GL_KEY_VALUE(GL_TRIANGLE_FAN),
    MACRO_GL_KEY_VALUE(GL_QUADS),
    MACRO_GL_KEY_VALUE(GL_QUAD_STRIP),
    MACRO_GL_KEY_VALUE(GL_POLYGON),
    MACRO_GL_KEY_VALUE(GL_ZERO),
    MACRO_GL_KEY_VALUE(GL_ONE),
    MACRO_GL_KEY_VALUE(GL_SRC_COLOR),
    MACRO_GL_KEY_VALUE(GL_ONE_MINUS_SRC_COLOR),
    MACRO_GL_KEY_VALUE(GL_SRC_ALPHA),
    MACRO_GL_KEY_VALUE(GL_ONE_MINUS_SRC_ALPHA),
    MACRO_GL_KEY_VALUE(GL_DST_ALPHA),
    MACRO_GL_KEY_VALUE(GL_ONE_MINUS_DST_ALPHA),
    MACRO_GL_KEY_VALUE(GL_DST_COLOR),
    MACRO_GL_KEY_VALUE(GL_ONE_MINUS_DST_COLOR),
    MACRO_GL_KEY_VALUE(GL_SRC_ALPHA_SATURATE),
    //MACRO_GL_KEY_VALUE(GL_CLIP_PLANE0),//(Text:"GL_TRUE";Value:GL_TRUE),
    //MACRO_GL_KEY_VALUE(GL_CLIP_PLANE0),//(Text:"GL_FALSE";Value:GL_FALSE),
    MACRO_GL_KEY_VALUE(GL_CLIP_PLANE0),
    MACRO_GL_KEY_VALUE(GL_CLIP_PLANE1),
    MACRO_GL_KEY_VALUE(GL_CLIP_PLANE2),
    MACRO_GL_KEY_VALUE(GL_CLIP_PLANE3),
    MACRO_GL_KEY_VALUE(GL_CLIP_PLANE4),
    MACRO_GL_KEY_VALUE(GL_CLIP_PLANE5),
    MACRO_GL_KEY_VALUE(GL_BYTE),
    MACRO_GL_KEY_VALUE(GL_UNSIGNED_BYTE),
    MACRO_GL_KEY_VALUE(GL_SHORT),
    MACRO_GL_KEY_VALUE(GL_UNSIGNED_SHORT),
    MACRO_GL_KEY_VALUE(GL_INT),
    MACRO_GL_KEY_VALUE(GL_UNSIGNED_INT),
    MACRO_GL_KEY_VALUE(GL_FLOAT),
    MACRO_GL_KEY_VALUE(GL_2_BYTES),
    MACRO_GL_KEY_VALUE(GL_3_BYTES),
    MACRO_GL_KEY_VALUE(GL_4_BYTES),
    MACRO_GL_KEY_VALUE(GL_DOUBLE),
    MACRO_GL_KEY_VALUE(GL_NONE),
    MACRO_GL_KEY_VALUE(GL_FRONT_LEFT),
    MACRO_GL_KEY_VALUE(GL_FRONT_RIGHT),
    MACRO_GL_KEY_VALUE(GL_BACK_LEFT),
    MACRO_GL_KEY_VALUE(GL_BACK_RIGHT),
    MACRO_GL_KEY_VALUE(GL_FRONT),
    MACRO_GL_KEY_VALUE(GL_BACK),
    MACRO_GL_KEY_VALUE(GL_LEFT),
    MACRO_GL_KEY_VALUE(GL_RIGHT),
    MACRO_GL_KEY_VALUE(GL_FRONT_AND_BACK),
    MACRO_GL_KEY_VALUE(GL_AUX0),
    MACRO_GL_KEY_VALUE(GL_AUX1),
    MACRO_GL_KEY_VALUE(GL_AUX2),
    MACRO_GL_KEY_VALUE(GL_AUX3),
    MACRO_GL_KEY_VALUE(GL_NO_ERROR),
    MACRO_GL_KEY_VALUE(GL_INVALID_ENUM),
    MACRO_GL_KEY_VALUE(GL_INVALID_VALUE),
    MACRO_GL_KEY_VALUE(GL_INVALID_OPERATION),
    MACRO_GL_KEY_VALUE(GL_STACK_OVERFLOW),
    MACRO_GL_KEY_VALUE(GL_STACK_UNDERFLOW),
    MACRO_GL_KEY_VALUE(GL_OUT_OF_MEMORY),
    MACRO_GL_KEY_VALUE(GL_2D),
    MACRO_GL_KEY_VALUE(GL_3D),
    MACRO_GL_KEY_VALUE(GL_3D_COLOR),
    MACRO_GL_KEY_VALUE(GL_3D_COLOR_TEXTURE),
    MACRO_GL_KEY_VALUE(GL_4D_COLOR_TEXTURE),
    MACRO_GL_KEY_VALUE(GL_PASS_THROUGH_TOKEN),
    MACRO_GL_KEY_VALUE(GL_POINT_TOKEN),
    MACRO_GL_KEY_VALUE(GL_LINE_TOKEN),
    MACRO_GL_KEY_VALUE(GL_POLYGON_TOKEN),
    MACRO_GL_KEY_VALUE(GL_BITMAP_TOKEN),
    MACRO_GL_KEY_VALUE(GL_DRAW_PIXEL_TOKEN),
    MACRO_GL_KEY_VALUE(GL_COPY_PIXEL_TOKEN),
    MACRO_GL_KEY_VALUE(GL_LINE_RESET_TOKEN),
    MACRO_GL_KEY_VALUE(GL_EXP),
    MACRO_GL_KEY_VALUE(GL_EXP2),
    MACRO_GL_KEY_VALUE(GL_CW),
    MACRO_GL_KEY_VALUE(GL_CCW),
    MACRO_GL_KEY_VALUE(GL_COEFF),
    MACRO_GL_KEY_VALUE(GL_ORDER),
    MACRO_GL_KEY_VALUE(GL_DOMAIN),
    MACRO_GL_KEY_VALUE(GL_CURRENT_COLOR),
    MACRO_GL_KEY_VALUE(GL_CURRENT_INDEX),
    MACRO_GL_KEY_VALUE(GL_CURRENT_NORMAL),
    MACRO_GL_KEY_VALUE(GL_CURRENT_TEXTURE_COORDS),
    MACRO_GL_KEY_VALUE(GL_CURRENT_RASTER_COLOR),
    MACRO_GL_KEY_VALUE(GL_CURRENT_RASTER_INDEX),
    MACRO_GL_KEY_VALUE(GL_CURRENT_RASTER_TEXTURE_COORDS),
    MACRO_GL_KEY_VALUE(GL_CURRENT_RASTER_POSITION),
    MACRO_GL_KEY_VALUE(GL_CURRENT_RASTER_POSITION_VALID),
    MACRO_GL_KEY_VALUE(GL_CURRENT_RASTER_DISTANCE),
    MACRO_GL_KEY_VALUE(GL_POINT_SMOOTH),
    MACRO_GL_KEY_VALUE(GL_POINT_SIZE),
    MACRO_GL_KEY_VALUE(GL_POINT_SIZE_RANGE),
    MACRO_GL_KEY_VALUE(GL_POINT_SIZE_GRANULARITY),
    MACRO_GL_KEY_VALUE(GL_LINE_SMOOTH),
    MACRO_GL_KEY_VALUE(GL_LINE_WIDTH),
    MACRO_GL_KEY_VALUE(GL_LINE_WIDTH_RANGE),
    MACRO_GL_KEY_VALUE(GL_LINE_WIDTH_GRANULARITY),
    MACRO_GL_KEY_VALUE(GL_LINE_STIPPLE),
    MACRO_GL_KEY_VALUE(GL_LINE_STIPPLE_PATTERN),
    MACRO_GL_KEY_VALUE(GL_LINE_STIPPLE_REPEAT),
    MACRO_GL_KEY_VALUE(GL_LIST_MODE),
    MACRO_GL_KEY_VALUE(GL_MAX_LIST_NESTING),
    MACRO_GL_KEY_VALUE(GL_LIST_BASE),
    MACRO_GL_KEY_VALUE(GL_LIST_INDEX),
    MACRO_GL_KEY_VALUE(GL_POLYGON_MODE),
    MACRO_GL_KEY_VALUE(GL_POLYGON_SMOOTH),
    MACRO_GL_KEY_VALUE(GL_POLYGON_STIPPLE),
    MACRO_GL_KEY_VALUE(GL_EDGE_FLAG),
    MACRO_GL_KEY_VALUE(GL_CULL_FACE),
    MACRO_GL_KEY_VALUE(GL_CULL_FACE_MODE),
    MACRO_GL_KEY_VALUE(GL_FRONT_FACE),
    MACRO_GL_KEY_VALUE(GL_LIGHTING),
    MACRO_GL_KEY_VALUE(GL_LIGHT_MODEL_LOCAL_VIEWER),
    MACRO_GL_KEY_VALUE(GL_LIGHT_MODEL_TWO_SIDE),
    MACRO_GL_KEY_VALUE(GL_LIGHT_MODEL_AMBIENT),
    MACRO_GL_KEY_VALUE(GL_SHADE_MODEL),
    MACRO_GL_KEY_VALUE(GL_COLOR_MATERIAL_FACE),
    MACRO_GL_KEY_VALUE(GL_COLOR_MATERIAL_PARAMETER),
    MACRO_GL_KEY_VALUE(GL_COLOR_MATERIAL),
    MACRO_GL_KEY_VALUE(GL_FOG),
    MACRO_GL_KEY_VALUE(GL_FOG_INDEX),
    MACRO_GL_KEY_VALUE(GL_FOG_DENSITY),
    MACRO_GL_KEY_VALUE(GL_FOG_START),
    MACRO_GL_KEY_VALUE(GL_FOG_END),
    MACRO_GL_KEY_VALUE(GL_FOG_MODE),
    MACRO_GL_KEY_VALUE(GL_FOG_COLOR),
    MACRO_GL_KEY_VALUE(GL_DEPTH_RANGE),
    MACRO_GL_KEY_VALUE(GL_DEPTH_TEST),
    MACRO_GL_KEY_VALUE(GL_DEPTH_WRITEMASK),
    MACRO_GL_KEY_VALUE(GL_DEPTH_CLEAR_VALUE),
    MACRO_GL_KEY_VALUE(GL_DEPTH_FUNC),
    MACRO_GL_KEY_VALUE(GL_ACCUM_CLEAR_VALUE),
    MACRO_GL_KEY_VALUE(GL_STENCIL_TEST),
    MACRO_GL_KEY_VALUE(GL_STENCIL_CLEAR_VALUE),
    MACRO_GL_KEY_VALUE(GL_STENCIL_FUNC),
    MACRO_GL_KEY_VALUE(GL_STENCIL_VALUE_MASK),
    MACRO_GL_KEY_VALUE(GL_STENCIL_FAIL),
    MACRO_GL_KEY_VALUE(GL_STENCIL_PASS_DEPTH_FAIL),
    MACRO_GL_KEY_VALUE(GL_STENCIL_PASS_DEPTH_PASS),
    MACRO_GL_KEY_VALUE(GL_STENCIL_REF),
    MACRO_GL_KEY_VALUE(GL_STENCIL_WRITEMASK),
    MACRO_GL_KEY_VALUE(GL_MATRIX_MODE),
    MACRO_GL_KEY_VALUE(GL_NORMALIZE),
    MACRO_GL_KEY_VALUE(GL_VIEWPORT),
    MACRO_GL_KEY_VALUE(GL_MODELVIEW_STACK_DEPTH),
    MACRO_GL_KEY_VALUE(GL_PROJECTION_STACK_DEPTH),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_STACK_DEPTH),
    MACRO_GL_KEY_VALUE(GL_MODELVIEW_MATRIX),
    MACRO_GL_KEY_VALUE(GL_PROJECTION_MATRIX),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_MATRIX),
    MACRO_GL_KEY_VALUE(GL_ATTRIB_STACK_DEPTH),
    MACRO_GL_KEY_VALUE(GL_CLIENT_ATTRIB_STACK_DEPTH),
    MACRO_GL_KEY_VALUE(GL_ALPHA_TEST),
    MACRO_GL_KEY_VALUE(GL_ALPHA_TEST_FUNC),
    MACRO_GL_KEY_VALUE(GL_ALPHA_TEST_REF),
    MACRO_GL_KEY_VALUE(GL_DITHER),
    MACRO_GL_KEY_VALUE(GL_BLEND_DST),
    MACRO_GL_KEY_VALUE(GL_BLEND_SRC),
    MACRO_GL_KEY_VALUE(GL_BLEND),
    MACRO_GL_KEY_VALUE(GL_LOGIC_OP_MODE),
    MACRO_GL_KEY_VALUE(GL_INDEX_LOGIC_OP),
    MACRO_GL_KEY_VALUE(GL_COLOR_LOGIC_OP),
    MACRO_GL_KEY_VALUE(GL_AUX_BUFFERS),
    MACRO_GL_KEY_VALUE(GL_DRAW_BUFFER),
    MACRO_GL_KEY_VALUE(GL_READ_BUFFER),
    MACRO_GL_KEY_VALUE(GL_SCISSOR_BOX),
    MACRO_GL_KEY_VALUE(GL_SCISSOR_TEST),
    MACRO_GL_KEY_VALUE(GL_INDEX_CLEAR_VALUE),
    MACRO_GL_KEY_VALUE(GL_INDEX_WRITEMASK),
    MACRO_GL_KEY_VALUE(GL_COLOR_CLEAR_VALUE),
    MACRO_GL_KEY_VALUE(GL_COLOR_WRITEMASK),
    MACRO_GL_KEY_VALUE(GL_INDEX_MODE),
    MACRO_GL_KEY_VALUE(GL_RGBA_MODE),
    MACRO_GL_KEY_VALUE(GL_DOUBLEBUFFER),
    MACRO_GL_KEY_VALUE(GL_STEREO),
    MACRO_GL_KEY_VALUE(GL_RENDER_MODE),
    MACRO_GL_KEY_VALUE(GL_PERSPECTIVE_CORRECTION_HINT),
    MACRO_GL_KEY_VALUE(GL_POINT_SMOOTH_HINT),
    MACRO_GL_KEY_VALUE(GL_LINE_SMOOTH_HINT),
    MACRO_GL_KEY_VALUE(GL_POLYGON_SMOOTH_HINT),
    MACRO_GL_KEY_VALUE(GL_FOG_HINT),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_GEN_S),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_GEN_T),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_GEN_R),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_GEN_Q),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_I_TO_I),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_S_TO_S),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_I_TO_R),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_I_TO_G),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_I_TO_B),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_I_TO_A),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_R_TO_R),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_G_TO_G),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_B_TO_B),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_A_TO_A),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_I_TO_I_SIZE),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_S_TO_S_SIZE),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_I_TO_R_SIZE),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_I_TO_G_SIZE),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_I_TO_B_SIZE),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_I_TO_A_SIZE),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_R_TO_R_SIZE),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_G_TO_G_SIZE),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_B_TO_B_SIZE),
    MACRO_GL_KEY_VALUE(GL_PIXEL_MAP_A_TO_A_SIZE),
    MACRO_GL_KEY_VALUE(GL_UNPACK_SWAP_BYTES),
    MACRO_GL_KEY_VALUE(GL_UNPACK_LSB_FIRST),
    MACRO_GL_KEY_VALUE(GL_UNPACK_ROW_LENGTH),
    MACRO_GL_KEY_VALUE(GL_UNPACK_SKIP_ROWS),
    MACRO_GL_KEY_VALUE(GL_UNPACK_SKIP_PIXELS),
    MACRO_GL_KEY_VALUE(GL_UNPACK_ALIGNMENT),
    MACRO_GL_KEY_VALUE(GL_PACK_SWAP_BYTES),
    MACRO_GL_KEY_VALUE(GL_PACK_LSB_FIRST),
    MACRO_GL_KEY_VALUE(GL_PACK_ROW_LENGTH),
    MACRO_GL_KEY_VALUE(GL_PACK_SKIP_ROWS),
    MACRO_GL_KEY_VALUE(GL_PACK_SKIP_PIXELS),
    MACRO_GL_KEY_VALUE(GL_PACK_ALIGNMENT),
    MACRO_GL_KEY_VALUE(GL_MAP_COLOR),
    MACRO_GL_KEY_VALUE(GL_MAP_STENCIL),
    MACRO_GL_KEY_VALUE(GL_INDEX_SHIFT),
    MACRO_GL_KEY_VALUE(GL_INDEX_OFFSET),
    MACRO_GL_KEY_VALUE(GL_RED_SCALE),
    MACRO_GL_KEY_VALUE(GL_RED_BIAS),
    MACRO_GL_KEY_VALUE(GL_ZOOM_X),
    MACRO_GL_KEY_VALUE(GL_ZOOM_Y),
    MACRO_GL_KEY_VALUE(GL_GREEN_SCALE),
    MACRO_GL_KEY_VALUE(GL_GREEN_BIAS),
    MACRO_GL_KEY_VALUE(GL_BLUE_SCALE),
    MACRO_GL_KEY_VALUE(GL_BLUE_BIAS),
    MACRO_GL_KEY_VALUE(GL_ALPHA_SCALE),
    MACRO_GL_KEY_VALUE(GL_ALPHA_BIAS),
    MACRO_GL_KEY_VALUE(GL_DEPTH_SCALE),
    MACRO_GL_KEY_VALUE(GL_DEPTH_BIAS),
    MACRO_GL_KEY_VALUE(GL_MAX_EVAL_ORDER),
    MACRO_GL_KEY_VALUE(GL_MAX_LIGHTS),
    MACRO_GL_KEY_VALUE(GL_MAX_CLIP_PLANES),
    MACRO_GL_KEY_VALUE(GL_MAX_TEXTURE_SIZE),
    MACRO_GL_KEY_VALUE(GL_MAX_PIXEL_MAP_TABLE),
    MACRO_GL_KEY_VALUE(GL_MAX_ATTRIB_STACK_DEPTH),
    MACRO_GL_KEY_VALUE(GL_MAX_MODELVIEW_STACK_DEPTH),
    MACRO_GL_KEY_VALUE(GL_MAX_NAME_STACK_DEPTH),
    MACRO_GL_KEY_VALUE(GL_MAX_PROJECTION_STACK_DEPTH),
    MACRO_GL_KEY_VALUE(GL_MAX_TEXTURE_STACK_DEPTH),
    MACRO_GL_KEY_VALUE(GL_MAX_VIEWPORT_DIMS),
    MACRO_GL_KEY_VALUE(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH),
    MACRO_GL_KEY_VALUE(GL_SUBPIXEL_BITS),
    MACRO_GL_KEY_VALUE(GL_INDEX_BITS),
    MACRO_GL_KEY_VALUE(GL_RED_BITS),
    MACRO_GL_KEY_VALUE(GL_GREEN_BITS),
    MACRO_GL_KEY_VALUE(GL_BLUE_BITS),
    MACRO_GL_KEY_VALUE(GL_ALPHA_BITS),
    MACRO_GL_KEY_VALUE(GL_DEPTH_BITS),
    MACRO_GL_KEY_VALUE(GL_STENCIL_BITS),
    MACRO_GL_KEY_VALUE(GL_ACCUM_RED_BITS),
    MACRO_GL_KEY_VALUE(GL_ACCUM_GREEN_BITS),
    MACRO_GL_KEY_VALUE(GL_ACCUM_BLUE_BITS),
    MACRO_GL_KEY_VALUE(GL_ACCUM_ALPHA_BITS),
    MACRO_GL_KEY_VALUE(GL_NAME_STACK_DEPTH),
    MACRO_GL_KEY_VALUE(GL_AUTO_NORMAL),
    MACRO_GL_KEY_VALUE(GL_MAP1_COLOR_4),
    MACRO_GL_KEY_VALUE(GL_MAP1_INDEX),
    MACRO_GL_KEY_VALUE(GL_MAP1_NORMAL),
    MACRO_GL_KEY_VALUE(GL_MAP1_TEXTURE_COORD_1),
    MACRO_GL_KEY_VALUE(GL_MAP1_TEXTURE_COORD_2),
    MACRO_GL_KEY_VALUE(GL_MAP1_TEXTURE_COORD_3),
    MACRO_GL_KEY_VALUE(GL_MAP1_TEXTURE_COORD_4),
    MACRO_GL_KEY_VALUE(GL_MAP1_VERTEX_3),
    MACRO_GL_KEY_VALUE(GL_MAP1_VERTEX_4),
    MACRO_GL_KEY_VALUE(GL_MAP2_COLOR_4),
    MACRO_GL_KEY_VALUE(GL_MAP2_INDEX),
    MACRO_GL_KEY_VALUE(GL_MAP2_NORMAL),
    MACRO_GL_KEY_VALUE(GL_MAP2_TEXTURE_COORD_1),
    MACRO_GL_KEY_VALUE(GL_MAP2_TEXTURE_COORD_2),
    MACRO_GL_KEY_VALUE(GL_MAP2_TEXTURE_COORD_3),
    MACRO_GL_KEY_VALUE(GL_MAP2_TEXTURE_COORD_4),
    MACRO_GL_KEY_VALUE(GL_MAP2_VERTEX_3),
    MACRO_GL_KEY_VALUE(GL_MAP2_VERTEX_4),
    MACRO_GL_KEY_VALUE(GL_MAP1_GRID_DOMAIN),
    MACRO_GL_KEY_VALUE(GL_MAP1_GRID_SEGMENTS),
    MACRO_GL_KEY_VALUE(GL_MAP2_GRID_DOMAIN),
    MACRO_GL_KEY_VALUE(GL_MAP2_GRID_SEGMENTS),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_1D),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_2D),
    MACRO_GL_KEY_VALUE(GL_FEEDBACK_BUFFER_POINTER),
    MACRO_GL_KEY_VALUE(GL_FEEDBACK_BUFFER_SIZE),
    MACRO_GL_KEY_VALUE(GL_FEEDBACK_BUFFER_TYPE),
    MACRO_GL_KEY_VALUE(GL_SELECTION_BUFFER_POINTER),
    MACRO_GL_KEY_VALUE(GL_SELECTION_BUFFER_SIZE),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_WIDTH),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_HEIGHT),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_INTERNAL_FORMAT),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_BORDER_COLOR),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_BORDER),
    MACRO_GL_KEY_VALUE(GL_DONT_CARE),
    MACRO_GL_KEY_VALUE(GL_FASTEST),
    MACRO_GL_KEY_VALUE(GL_NICEST),
    MACRO_GL_KEY_VALUE(GL_LIGHT0),
    MACRO_GL_KEY_VALUE(GL_LIGHT1),
    MACRO_GL_KEY_VALUE(GL_LIGHT2),
    MACRO_GL_KEY_VALUE(GL_LIGHT3),
    MACRO_GL_KEY_VALUE(GL_LIGHT4),
    MACRO_GL_KEY_VALUE(GL_LIGHT5),
    MACRO_GL_KEY_VALUE(GL_LIGHT6),
    MACRO_GL_KEY_VALUE(GL_LIGHT7),
    MACRO_GL_KEY_VALUE(GL_AMBIENT),
    MACRO_GL_KEY_VALUE(GL_DIFFUSE),
    MACRO_GL_KEY_VALUE(GL_SPECULAR),
    MACRO_GL_KEY_VALUE(GL_POSITION),
    MACRO_GL_KEY_VALUE(GL_SPOT_DIRECTION),
    MACRO_GL_KEY_VALUE(GL_SPOT_EXPONENT),
    MACRO_GL_KEY_VALUE(GL_SPOT_CUTOFF),
    MACRO_GL_KEY_VALUE(GL_CONSTANT_ATTENUATION),
    MACRO_GL_KEY_VALUE(GL_LINEAR_ATTENUATION),
    MACRO_GL_KEY_VALUE(GL_QUADRATIC_ATTENUATION),
    MACRO_GL_KEY_VALUE(GL_COMPILE),
    MACRO_GL_KEY_VALUE(GL_COMPILE_AND_EXECUTE),
    MACRO_GL_KEY_VALUE(GL_CLEAR),
    MACRO_GL_KEY_VALUE(GL_AND),
    MACRO_GL_KEY_VALUE(GL_AND_REVERSE),
    MACRO_GL_KEY_VALUE(GL_COPY),
    MACRO_GL_KEY_VALUE(GL_AND_INVERTED),
    MACRO_GL_KEY_VALUE(GL_NOOP),
    MACRO_GL_KEY_VALUE(GL_XOR),
    MACRO_GL_KEY_VALUE(GL_OR),
    MACRO_GL_KEY_VALUE(GL_NOR),
    MACRO_GL_KEY_VALUE(GL_EQUIV),
    MACRO_GL_KEY_VALUE(GL_INVERT),
    MACRO_GL_KEY_VALUE(GL_OR_REVERSE),
    MACRO_GL_KEY_VALUE(GL_COPY_INVERTED),
    MACRO_GL_KEY_VALUE(GL_OR_INVERTED),
    MACRO_GL_KEY_VALUE(GL_NAND),
    MACRO_GL_KEY_VALUE(GL_SET),
    MACRO_GL_KEY_VALUE(GL_EMISSION),
    MACRO_GL_KEY_VALUE(GL_SHININESS),
    MACRO_GL_KEY_VALUE(GL_AMBIENT_AND_DIFFUSE),
    MACRO_GL_KEY_VALUE(GL_COLOR_INDEXES),
    MACRO_GL_KEY_VALUE(GL_MODELVIEW),
    MACRO_GL_KEY_VALUE(GL_PROJECTION),
    MACRO_GL_KEY_VALUE(GL_TEXTURE),
    MACRO_GL_KEY_VALUE(GL_COLOR),
    MACRO_GL_KEY_VALUE(GL_DEPTH),
    MACRO_GL_KEY_VALUE(GL_STENCIL),
    MACRO_GL_KEY_VALUE(GL_COLOR_INDEX),
    MACRO_GL_KEY_VALUE(GL_STENCIL_INDEX),
    MACRO_GL_KEY_VALUE(GL_DEPTH_COMPONENT),
    MACRO_GL_KEY_VALUE(GL_RED),
    MACRO_GL_KEY_VALUE(GL_GREEN),
    MACRO_GL_KEY_VALUE(GL_BLUE),
    MACRO_GL_KEY_VALUE(GL_ALPHA),
    MACRO_GL_KEY_VALUE(GL_RGB),
    MACRO_GL_KEY_VALUE(GL_RGBA),
    MACRO_GL_KEY_VALUE(GL_LUMINANCE),
    MACRO_GL_KEY_VALUE(GL_LUMINANCE_ALPHA),
    MACRO_GL_KEY_VALUE(GL_BITMAP),
    MACRO_GL_KEY_VALUE(GL_POINT),
    MACRO_GL_KEY_VALUE(GL_LINE),
    MACRO_GL_KEY_VALUE(GL_FILL),
    MACRO_GL_KEY_VALUE(GL_RENDER),
    MACRO_GL_KEY_VALUE(GL_FEEDBACK),
    MACRO_GL_KEY_VALUE(GL_SELECT),
    MACRO_GL_KEY_VALUE(GL_FLAT),
    MACRO_GL_KEY_VALUE(GL_SMOOTH),
    MACRO_GL_KEY_VALUE(GL_KEEP),
    MACRO_GL_KEY_VALUE(GL_REPLACE),
    MACRO_GL_KEY_VALUE(GL_INCR),
    MACRO_GL_KEY_VALUE(GL_DECR),
    MACRO_GL_KEY_VALUE(GL_VENDOR),
    MACRO_GL_KEY_VALUE(GL_RENDERER),
    MACRO_GL_KEY_VALUE(GL_VERSION),
    MACRO_GL_KEY_VALUE(GL_EXTENSIONS),
    MACRO_GL_KEY_VALUE(GL_S),
    MACRO_GL_KEY_VALUE(GL_T),
    MACRO_GL_KEY_VALUE(GL_R),
    MACRO_GL_KEY_VALUE(GL_Q),
    MACRO_GL_KEY_VALUE(GL_MODULATE),
    MACRO_GL_KEY_VALUE(GL_DECAL),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_ENV_MODE),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_ENV_COLOR),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_ENV),
    MACRO_GL_KEY_VALUE(GL_EYE_LINEAR),
    MACRO_GL_KEY_VALUE(GL_OBJECT_LINEAR),
    MACRO_GL_KEY_VALUE(GL_SPHERE_MAP),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_GEN_MODE),
    MACRO_GL_KEY_VALUE(GL_OBJECT_PLANE),
    MACRO_GL_KEY_VALUE(GL_EYE_PLANE),
    MACRO_GL_KEY_VALUE(GL_NEAREST),
    MACRO_GL_KEY_VALUE(GL_LINEAR),
    MACRO_GL_KEY_VALUE(GL_NEAREST_MIPMAP_NEAREST),
    MACRO_GL_KEY_VALUE(GL_LINEAR_MIPMAP_NEAREST),
    MACRO_GL_KEY_VALUE(GL_NEAREST_MIPMAP_LINEAR),
    MACRO_GL_KEY_VALUE(GL_LINEAR_MIPMAP_LINEAR),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_MAG_FILTER),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_MIN_FILTER),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_WRAP_S),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_WRAP_T),
    MACRO_GL_KEY_VALUE(GL_CLAMP),
    MACRO_GL_KEY_VALUE(GL_REPEAT),
    MACRO_GL_KEY_VALUE(GL_CLIENT_PIXEL_STORE_BIT),
    MACRO_GL_KEY_VALUE(GL_CLIENT_VERTEX_ARRAY_BIT),
    MACRO_GL_KEY_VALUE(GL_CLIENT_ALL_ATTRIB_BITS),
    MACRO_GL_KEY_VALUE(GL_POLYGON_OFFSET_FACTOR),
    MACRO_GL_KEY_VALUE(GL_POLYGON_OFFSET_UNITS),
    MACRO_GL_KEY_VALUE(GL_POLYGON_OFFSET_POINT),
    MACRO_GL_KEY_VALUE(GL_POLYGON_OFFSET_LINE),
    MACRO_GL_KEY_VALUE(GL_POLYGON_OFFSET_FILL),
    MACRO_GL_KEY_VALUE(GL_ALPHA4),
    MACRO_GL_KEY_VALUE(GL_ALPHA8),
    MACRO_GL_KEY_VALUE(GL_ALPHA12),
    MACRO_GL_KEY_VALUE(GL_ALPHA16),
    MACRO_GL_KEY_VALUE(GL_LUMINANCE4),
    MACRO_GL_KEY_VALUE(GL_LUMINANCE8),
    MACRO_GL_KEY_VALUE(GL_LUMINANCE12),
    MACRO_GL_KEY_VALUE(GL_LUMINANCE16),
    MACRO_GL_KEY_VALUE(GL_LUMINANCE4_ALPHA4),
    MACRO_GL_KEY_VALUE(GL_LUMINANCE6_ALPHA2),
    MACRO_GL_KEY_VALUE(GL_LUMINANCE8_ALPHA8),
    MACRO_GL_KEY_VALUE(GL_LUMINANCE12_ALPHA4),
    MACRO_GL_KEY_VALUE(GL_LUMINANCE12_ALPHA12),
    MACRO_GL_KEY_VALUE(GL_LUMINANCE16_ALPHA16),
    MACRO_GL_KEY_VALUE(GL_INTENSITY),
    MACRO_GL_KEY_VALUE(GL_INTENSITY4),
    MACRO_GL_KEY_VALUE(GL_INTENSITY8),
    MACRO_GL_KEY_VALUE(GL_INTENSITY12),
    MACRO_GL_KEY_VALUE(GL_INTENSITY16),
    MACRO_GL_KEY_VALUE(GL_R3_G3_B2),
    MACRO_GL_KEY_VALUE(GL_RGB4),
    MACRO_GL_KEY_VALUE(GL_RGB5),
    MACRO_GL_KEY_VALUE(GL_RGB8),
    MACRO_GL_KEY_VALUE(GL_RGB10),
    MACRO_GL_KEY_VALUE(GL_RGB12),
    MACRO_GL_KEY_VALUE(GL_RGB16),
    MACRO_GL_KEY_VALUE(GL_RGBA2),
    MACRO_GL_KEY_VALUE(GL_RGBA4),
    MACRO_GL_KEY_VALUE(GL_RGB5_A1),
    MACRO_GL_KEY_VALUE(GL_RGBA8),
    MACRO_GL_KEY_VALUE(GL_RGB10_A2),
    MACRO_GL_KEY_VALUE(GL_RGBA12),
    MACRO_GL_KEY_VALUE(GL_RGBA16),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_RED_SIZE),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_GREEN_SIZE),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_BLUE_SIZE),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_ALPHA_SIZE),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_LUMINANCE_SIZE),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_INTENSITY_SIZE),
    MACRO_GL_KEY_VALUE(GL_PROXY_TEXTURE_1D),
    MACRO_GL_KEY_VALUE(GL_PROXY_TEXTURE_2D),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_PRIORITY),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_RESIDENT),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_BINDING_1D),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_BINDING_2D),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY),
    MACRO_GL_KEY_VALUE(GL_NORMAL_ARRAY),
    MACRO_GL_KEY_VALUE(GL_COLOR_ARRAY),
    MACRO_GL_KEY_VALUE(GL_INDEX_ARRAY),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_COORD_ARRAY),
    MACRO_GL_KEY_VALUE(GL_EDGE_FLAG_ARRAY),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_SIZE),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_TYPE),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_STRIDE),
    MACRO_GL_KEY_VALUE(GL_NORMAL_ARRAY_TYPE),
    MACRO_GL_KEY_VALUE(GL_NORMAL_ARRAY_STRIDE),
    MACRO_GL_KEY_VALUE(GL_COLOR_ARRAY_SIZE),
    MACRO_GL_KEY_VALUE(GL_COLOR_ARRAY_TYPE),
    MACRO_GL_KEY_VALUE(GL_COLOR_ARRAY_STRIDE),
    MACRO_GL_KEY_VALUE(GL_INDEX_ARRAY_TYPE),
    MACRO_GL_KEY_VALUE(GL_INDEX_ARRAY_STRIDE),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_COORD_ARRAY_SIZE),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_COORD_ARRAY_TYPE),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_COORD_ARRAY_STRIDE),
    MACRO_GL_KEY_VALUE(GL_EDGE_FLAG_ARRAY_STRIDE),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_POINTER),
    MACRO_GL_KEY_VALUE(GL_NORMAL_ARRAY_POINTER),
    MACRO_GL_KEY_VALUE(GL_COLOR_ARRAY_POINTER),
    MACRO_GL_KEY_VALUE(GL_INDEX_ARRAY_POINTER),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_COORD_ARRAY_POINTER),
    MACRO_GL_KEY_VALUE(GL_EDGE_FLAG_ARRAY_POINTER),
    MACRO_GL_KEY_VALUE(GL_V2F),
    MACRO_GL_KEY_VALUE(GL_V3F),
    MACRO_GL_KEY_VALUE(GL_C4UB_V2F),
    MACRO_GL_KEY_VALUE(GL_C4UB_V3F),
    MACRO_GL_KEY_VALUE(GL_C3F_V3F),
    MACRO_GL_KEY_VALUE(GL_N3F_V3F),
    MACRO_GL_KEY_VALUE(GL_C4F_N3F_V3F),
    MACRO_GL_KEY_VALUE(GL_T2F_V3F),
    MACRO_GL_KEY_VALUE(GL_T4F_V4F),
    MACRO_GL_KEY_VALUE(GL_T2F_C4UB_V3F),
    MACRO_GL_KEY_VALUE(GL_T2F_C3F_V3F),
    MACRO_GL_KEY_VALUE(GL_T2F_N3F_V3F),
    MACRO_GL_KEY_VALUE(GL_T2F_C4F_N3F_V3F),
    MACRO_GL_KEY_VALUE(GL_T4F_C4F_N3F_V4F),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_EXT),//(Text:"GL_EXT_vertex_array";Value:GL_EXT_vertex_arra),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_EXT),//(Text:"GL_WIN_swap_hint";Value:GL_WIN_swap_hint),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_EXT),//(Text:"GL_EXT_bgra";Value:GL_EXT_bgra),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_EXT),//(Text:"GL_EXT_paletted_texture";Value:GL_EXT_paletted_texture),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_EXT),
    MACRO_GL_KEY_VALUE(GL_NORMAL_ARRAY_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_ARRAY_EXT),
    MACRO_GL_KEY_VALUE(GL_INDEX_ARRAY_EXT),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_COORD_ARRAY_EXT),
    MACRO_GL_KEY_VALUE(GL_EDGE_FLAG_ARRAY_EXT),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_SIZE_EXT),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_TYPE_EXT),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_STRIDE_EXT),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_COUNT_EXT),
    MACRO_GL_KEY_VALUE(GL_NORMAL_ARRAY_TYPE_EXT),
    MACRO_GL_KEY_VALUE(GL_NORMAL_ARRAY_STRIDE_EXT),
    MACRO_GL_KEY_VALUE(GL_NORMAL_ARRAY_COUNT_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_ARRAY_SIZE_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_ARRAY_TYPE_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_ARRAY_STRIDE_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_ARRAY_COUNT_EXT),
    MACRO_GL_KEY_VALUE(GL_INDEX_ARRAY_TYPE_EXT),
    MACRO_GL_KEY_VALUE(GL_INDEX_ARRAY_STRIDE_EXT),
    MACRO_GL_KEY_VALUE(GL_INDEX_ARRAY_COUNT_EXT),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_COORD_ARRAY_SIZE_EXT),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_COORD_ARRAY_TYPE_EXT),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_COORD_ARRAY_STRIDE_EXT),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_COORD_ARRAY_COUNT_EXT),
    MACRO_GL_KEY_VALUE(GL_EDGE_FLAG_ARRAY_STRIDE_EXT),
    MACRO_GL_KEY_VALUE(GL_EDGE_FLAG_ARRAY_COUNT_EXT),
    MACRO_GL_KEY_VALUE(GL_VERTEX_ARRAY_POINTER_EXT),
    MACRO_GL_KEY_VALUE(GL_NORMAL_ARRAY_POINTER_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_ARRAY_POINTER_EXT),
    MACRO_GL_KEY_VALUE(GL_INDEX_ARRAY_POINTER_EXT),
    MACRO_GL_KEY_VALUE(GL_TEXTURE_COORD_ARRAY_POINTER_EXT),
    MACRO_GL_KEY_VALUE(GL_EDGE_FLAG_ARRAY_POINTER_EXT),
    //MACRO_GL_KEY_VALUE(GL_BGR_EXT),//(Text:"GL_DOUBLE_EXT";Value:GL_DOUBLE_EXT),
    MACRO_GL_KEY_VALUE(GL_BGR_EXT),
    MACRO_GL_KEY_VALUE(GL_BGRA_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_TABLE_FORMAT_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_TABLE_WIDTH_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_TABLE_RED_SIZE_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_TABLE_GREEN_SIZE_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_TABLE_BLUE_SIZE_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_TABLE_ALPHA_SIZE_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_TABLE_LUMINANCE_SIZE_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_TABLE_INTENSITY_SIZE_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_INDEX1_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_INDEX2_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_INDEX4_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_INDEX8_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_INDEX12_EXT),
    MACRO_GL_KEY_VALUE(GL_COLOR_INDEX16_EXT)
};
#undef MACRO_GL_KEY_VALUE


    void registerGLEnums(sol::state& state)
    {
        state.create
    }

    /*
uses
  SysUtils,
  dglOpenGL,
  ULua;
  */
int luaopen_gl(Plua_State L);
//GLenum ULuaGl_StringToEnum(std::string Str);

//{ lua lib functions }
int ULuaGl_Begin(Plua_State L);
int ULuaGl_BindTexture(Plua_State L);
int ULuaGl_BlendFunc(Plua_State L);
int ULuaGl_Clear(Plua_State L);
int ULuaGl_ClearAccum(Plua_State L);
int ULuaGl_ClearColor(Plua_State L);
int ULuaGl_Color(Plua_State L);
int ULuaGl_CullFace(Plua_State L);
int ULuaGl_DepthFunc(Plua_State L);
int ULuaGl_DepthRange(Plua_State L);
int ULuaGl_Disable(Plua_State L);
int ULuaGl_DisableClientState(Plua_State L);
int ULuaGl_DrawBuffer(Plua_State L);
int ULuaGl_Enable(Plua_State L);
int ULuaGl_EnableClientState(Plua_State L);
int ULuaGl_End(Plua_State L);
int ULuaGl_EndList(Plua_State L);
int ULuaGl_Finish(Plua_State L);
int ULuaGl_Flush(Plua_State L);
int ULuaGl_FrontFace(Plua_State L);
int ULuaGl_InitNames(Plua_State L);
int ULuaGl_LoadIdentity(Plua_State L);
int ULuaGl_LogicOp(Plua_State L);
int ULuaGl_MatrixMode(Plua_State L);
int ULuaGl_Ortho(Plua_State L);
int ULuaGl_PopAttrib(Plua_State L);
int ULuaGl_PopClientAttrib(Plua_State L);
int ULuaGl_PopMatrix(Plua_State L);
int ULuaGl_PopName(Plua_State L);
int ULuaGl_PushMatrix(Plua_State L);
int ULuaGl_RasterPos(Plua_State L);
int ULuaGl_ReadBuffer(Plua_State L);
int ULuaGl_Rect(Plua_State L);
int ULuaGl_Rotate(Plua_State L);
int ULuaGl_Scale(Plua_State L);
int ULuaGl_ShadeModel(Plua_State L);
int ULuaGl_TexCoord(Plua_State L);
int ULuaGl_Translate(Plua_State L);
int ULuaGl_Vertex(Plua_State L);
int ULuaGl_Viewport(Plua_State L);
int ULuaGl_Dummy(Plua_State L);

    /*
const
  ULuaGl_Lib_f: array [0..40] of lual_reg = (
   (name:"Begin";func:ULuaGl_Begin),
   (name:"BindTexture";func:ULuaGl_BindTexture),
   (name:"BlendFunc";func:ULuaGl_BlendFunc),
   (name:"Clear";func:ULuaGl_Clear),
   (name:"ClearAccum";func:ULuaGl_ClearAccum),
   (name:"ClearColor";func:ULuaGl_ClearColor),
   (name:"Color";func:ULuaGl_Color),
   (name:"CullFace";func:ULuaGl_CullFace),
   (name:"DepthFunc";func:ULuaGl_DepthFunc),
   (name:"DepthRange";func:ULuaGl_DepthRange),
   (name:"Disable";func:ULuaGl_Disable),
   (name:"DisableClientState";func:ULuaGl_DisableClientState),
   (name:"DrawBuffer";func:ULuaGl_DrawBuffer),
   (name:"Enable";func:ULuaGl_Enable),
   (name:"EnableClientState";func:ULuaGl_EnableClientState),
   (name:"End";func:ULuaGl_End),
   (name:"EndList";func:ULuaGl_EndList),
   (name:"Finish";func:ULuaGl_Finish),
   (name:"Flush";func:ULuaGl_Flush),
   (name:"FrontFace";func:ULuaGl_FrontFace),
   (name:"InitNames";func:ULuaGl_InitNames),
   (name:"LoadIdentity";func:ULuaGl_LoadIdentity),
   (name:"LogicOp";func:ULuaGl_LogicOp),
   (name:"MatrixMode";func:ULuaGl_MatrixMode),
   (name:"Ortho";func:ULuaGl_Ortho),
   (name:"PopAttrib";func:ULuaGl_PopAttrib),
   (name:"PopClientAttrib";func:ULuaGl_PopClientAttrib),
   (name:"PopMatrix";func:ULuaGl_PopMatrix),
   (name:"PopName";func:ULuaGl_PopName),
   (name:"PushMatrix";func:ULuaGl_PushMatrix),
   (name:"RasterPos";func:ULuaGl_RasterPos),
   (name:"ReadBuffer";func:ULuaGl_ReadBuffer),
   (name:"Rotate";func:ULuaGl_Rotate),
   (name:"Rect";func:ULuaGl_Rect),
   (name:"Scale";func:ULuaGl_Scale),
   (name:"ShadeModel";func:ULuaGl_ShadeModel),
   (name:"TexCoord";func:ULuaGl_TexCoord),
   (name:"Translate";func:ULuaGl_Translate),
   (name:"Vertex";func:ULuaGl_Vertex),
   (name:"Viewport";func:ULuaGl_Viewport),
   (name:nil;func:nil)
   );*/
    /*
uses
   ULog;
   */

    /*
type
  TULuaGl_Enums = record
    Text:       string;
    Value:      GLenum;
  }
const
  ULuaGl_EnumERROR = $fffffffe;
  */
int ULuaGl_Begin(Plua_State L)
/*var
  e : GLenum;*/
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.Begin\"");

  glBegin(e);

  result:=0; // number of results
}

int ULuaGl_BindTexture(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.BindTexture\"");

  glBindTexture(e,lual_checkinteger(L,2));

  result:=0; // number of results
}

int ULuaGl_BlendFunc(Plua_State L)
var
  e : GLenum;
  f : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));
  f := ULuaGl_StringToEnum(lual_checkstring(L,2));

  if (e = ULuaGl_EnumERROR) or (f = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.BlendFunc\"");

  glBlendFunc(e,f);

  result:=0; // number of results
}

int ULuaGl_Clear(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.Clear\"");

  glClear(e);

  result:=0; // number of results
}

int ULuaGl_ClearAccum(Plua_State L)
var
  i: int;
{
  if lua_istable(L, 1) then
    for i := 1 to lua_objlen(L,1) do
      lua_rawgeti(L,1,i);

  if (lua_istable(L, 1) and (lua_objlen(L,1) = 4)) or (lua_gettop(L) = 4) then
    glClearAccum(lual_checknumber(L,-4),
                 lual_checknumber(L,-3),
                 lual_checknumber(L,-2),
                 lual_checknumber(L,-1))
  else
    luaL_error(L, "incorrect argument to function \"gl.ClearAccum\"");
  result:=0; // number of results
}

int ULuaGl_ClearColor(Plua_State L)
var
  i: int;
{
  if lua_istable(L, 1) then
    for i := 1 to lua_objlen(L,1) do
      lua_rawgeti(L,1,i);

  if (lua_istable(L, 1) and (lua_objlen(L,1) = 4)) or (lua_gettop(L) = 4) then
    glClearColor(lual_checknumber(L,-4),
                 lual_checknumber(L,-3),
                 lual_checknumber(L,-2),
                 lual_checknumber(L,-1))
  else
    luaL_error(L, "incorrect argument to function \"gl.ClearColor\"");
  result:=0; // number of results
}

int ULuaGl_Color(Plua_State L)
var
  i: int;
{
  if lua_istable(L, 1) then
    for i := 1 to lua_objlen(L,1) do
      lua_rawgeti(L,1,i);

  if (lua_istable(L, 1) and (lua_objlen(L,1) = 3)) or (lua_gettop(L) = 3) then
    glColor3d(GLdouble(lual_checknumber(L,-3)),
              GLdouble(lual_checknumber(L,-2)),
              GLdouble(lual_checknumber(L,-1)))
  else if (lua_istable(L, 1) and (lua_objlen(L,1) = 4)) or (lua_gettop(L) = 4) then
    glColor4d(GLdouble(lual_checknumber(L,-4)),
              GLdouble(lual_checknumber(L,-3)),
              GLdouble(lual_checknumber(L,-2)),
              GLdouble(lual_checknumber(L,-1)))
  else
    luaL_error(L, "incorrect argument to function \"gl.Color\"");
  result:=0; // number of results
}

int ULuaGl_CullFace(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.CullFace\"");

  glCullFace(e);

  result:=0; // number of results
}

int ULuaGl_DepthFunc(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.DepthFunc\"");

  glDepthFunc(e);

  result:=0; // number of results
}

int ULuaGl_DepthRange(Plua_State L)
var
  i: int;
{
  if lua_istable(L, 1) then
    for i := 1 to lua_objlen(L,1) do
      lua_rawgeti(L,1,i);

  if  (lua_istable(L, 1) and (lua_objlen(L,1) = 2))
  or  (lua_gettop(L) = 2) then
    glDepthRange(lual_checkinteger(L,-2),
                 lual_checkinteger(L,-1))
  else
    luaL_error(L, "incorrect argument to function \"gl.DepthRange\"");
  result:=0; // number of results
}

int ULuaGl_Disable(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.Disable\"");

  glDisable(e);

  result:=0; // number of results
}

int ULuaGl_DisableClientState(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.DisableClientState\"");

  glDisableClientState(e);

  result:=0; // number of results
}

int ULuaGl_DrawBuffer(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.DrawBuffer\"");

  glDrawBuffer(e);

  result:=0; // number of results
}

int ULuaGl_Enable(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.Enable\"");

  glEnable(e);
  result:=0; // number of results
}

int ULuaGl_EnableClientState(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.EnableClientState\"");

  glEnableClientState(e);

  result:=0; // number of results
}

int ULuaGl_End(Plua_State L)
{
  glEnd();
  result:=0; // number of results
}

int ULuaGl_EndList(Plua_State L)
{
  glEndList();
  result:=0; // number of results
}

int ULuaGl_Finish(Plua_State L)
{
  glFinish();
  result:=0; // number of results
}

int ULuaGl_Flush(Plua_State L)
{
  glFlush();
  result:=0; // number of results
}

int ULuaGl_FrontFace(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.FrontFace\"");

  glFrontFace(e);

  result:=0; // number of results
}

int ULuaGl_InitNames(Plua_State L)
{
  glInitNames();
  result:=0; // number of results
}

int ULuaGl_LoadIdentity(Plua_State L)
{
  glLoadIdentity();
  result:=0; // number of results
}

int ULuaGl_LogicOp(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.LogicOp\"");

  glLogicOp(e);

  result:=0; // number of results
}

int ULuaGl_MatrixMode(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.MatrixMode\"");

  glMatrixMode(e);

  result:=0; // number of results
}

int ULuaGl_Ortho(Plua_State L)
{
  if  (lua_gettop(L) = 6) then
    glOrtho(lual_checkinteger(L,-6),
            lual_checkinteger(L,-5),
            lual_checkinteger(L,-4),
            lual_checkinteger(L,-3),
            lual_checkinteger(L,-2),
            lual_checkinteger(L,-1))
  else
    luaL_error(L, "incorrect argument to function \"gl.Ortho\"");
  result:=0; // number of results
}

int ULuaGl_PopAttrib(Plua_State L)
{
  glPopAttrib();
  result:=0; // number of results
}

int ULuaGl_PopClientAttrib(Plua_State L)
{
  glPopClientAttrib();
  result:=0; // number of results
}

int ULuaGl_PopMatrix(Plua_State L)
{
  glPopMatrix();
  result:=0; // number of results
}

int ULuaGl_PopName(Plua_State L)
{
  glPopName();
  result:=0; // number of results
}

int ULuaGl_PushMatrix(Plua_State L)
{
  glPopName();
  result:=0; // number of results
}

int ULuaGl_RasterPos(Plua_State L)
var
  i: int;
{
  if lua_istable(L, 1) then
    for i := 1 to lua_objlen(L,1) do
      lua_rawgeti(L,1,i);

  if (lua_istable(L, 1) and (lua_objlen(L,1) = 2)) or (lua_gettop(L) = 2) then
    glRasterPos2d(GLdouble(lual_checknumber(L,-2)),
                  GLdouble(lual_checknumber(L,-1)))
  else if (lua_istable(L, 1) and (lua_objlen(L,1) = 3)) or (lua_gettop(L) = 3) then
    glRasterPos3d(GLdouble(lual_checknumber(L,-3)),
                  GLdouble(lual_checknumber(L,-2)),
                  GLdouble(lual_checknumber(L,-1)))
  else if (lua_istable(L, 1) and (lua_objlen(L,1) = 4)) or (lua_gettop(L) = 4) then
    glRasterPos4d(GLdouble(lual_checknumber(L,-4)),
                  GLdouble(lual_checknumber(L,-3)),
                  GLdouble(lual_checknumber(L,-2)),
                  GLdouble(lual_checknumber(L,-1)))
  else
    luaL_error(L, "incorrect argument to function \"gl.RasterPos\"");
  result:=0; // number of results
}

int ULuaGl_ReadBuffer(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.ReadBuffer\"");

  glReadBuffer(e);

  result:=0; // number of results
}

int ULuaGl_Rect(Plua_State L)
var
  i: int;
{
  if lua_istable(L, 1) and lua_istable(L, 2) then
  {
    for i := 1 to lua_objlen(L,1) do
      lua_rawgeti(L,1,i);
    for i := 1 to lua_objlen(L,2) do
      lua_rawgeti(L,2,i);
  }

  if  (lua_istable(L, 1) and (lua_objlen(L,1) = 2))
  and (lua_istable(L, 2) and (lua_objlen(L,2) = 2))
  or  (lua_gettop(L) = 4) then
    glRectD(lual_checknumber(L,-4),
            lual_checknumber(L,-3),
            lual_checknumber(L,-2),
            lual_checknumber(L,-1))
  else
    luaL_error(L, "incorrect argument to function \"gl.Rect\"");
  result:=0; // number of results
}

int ULuaGl_Rotate(Plua_State L)
{
  if  (lua_gettop(L) = 3) then
    glRotated(lual_checkinteger(L,-4),
              lual_checkinteger(L,-3),
              lual_checkinteger(L,-2),
              lual_checkinteger(L,-1))
  else
    luaL_error(L, "incorrect argument to function \"gl.Rotate\"");
  result:=0; // number of results
}

int ULuaGl_Scale(Plua_State L)
{
  if  (lua_gettop(L) = 3) then
    glScaled(lual_checkinteger(L,-3),
             lual_checkinteger(L,-2),
             lual_checkinteger(L,-1))
  else
    luaL_error(L, "incorrect argument to function \"gl.Scale\"");
  result:=0; // number of results
}

int ULuaGl_ShadeModel(Plua_State L)
var
  e : GLenum;
{
  e := ULuaGl_StringToEnum(lual_checkstring(L,1));

  if (e = ULuaGl_EnumERROR) then
    luaL_error(L, "incorrect string argument to function \"gl.ShadeModel\"");

  glShadeModel(e);

  result:=0; // number of results
}

int ULuaGl_TexCoord(Plua_State L)
var
  i: int;
{
  if lua_istable(L, 1) then
    for i := 1 to lua_objlen(L,1) do
      lua_rawgeti(L,1,i);

  if (lua_istable(L, 1) and (lua_objlen(L,1) = 1)) or (lua_gettop(L) = 1) then
    glTexCoord1d(GLdouble(lual_checknumber(L,-1)))
  else if (lua_istable(L, 1) and (lua_objlen(L,1) = 2)) or (lua_gettop(L) = 2) then
    glTexCoord2d(GLdouble(lual_checknumber(L,-2)),
                 GLdouble(lual_checknumber(L,-1)))
  else if (lua_istable(L, 1) and (lua_objlen(L,1) = 3)) or (lua_gettop(L) = 3) then
    glTexCoord3d(GLdouble(lual_checknumber(L,-3)),
                 GLdouble(lual_checknumber(L,-2)),
                 GLdouble(lual_checknumber(L,-1)))
  else if (lua_istable(L, 1) and (lua_objlen(L,1) = 4)) or (lua_gettop(L) = 4) then
    glTexCoord4d(GLdouble(lual_checknumber(L,-4)),
                 GLdouble(lual_checknumber(L,-3)),
                 GLdouble(lual_checknumber(L,-2)),
                 GLdouble(lual_checknumber(L,-1)))
  else
    luaL_error(L, "incorrect argument to function \"gl.TexCoord\"");
  result:=0; // number of results
}

int ULuaGl_Translate(Plua_State L)
{
  if  (lua_gettop(L) = 3) then
    glTranslated(lual_checkinteger(L,-3),
                 lual_checkinteger(L,-2),
                 lual_checkinteger(L,-1))
  else
    luaL_error(L, "incorrect argument to function \"gl.Translate\"");
  result:=0; // number of results
}

int ULuaGl_Vertex(Plua_State L)
var
  i: int;
{
  if lua_istable(L, 1) then
    for i := 1 to lua_objlen(L,1) do
      lua_rawgeti(L,1,i);

  if (lua_istable(L, 1) and (lua_objlen(L,1) = 2)) or (lua_gettop(L) = 2) then
    glVertex2d(GLdouble(lual_checknumber(L,-2)),
               GLdouble(lual_checknumber(L,-1)))
  else if (lua_istable(L, 1) and (lua_objlen(L,1) = 3)) or (lua_gettop(L) = 3) then
    glVertex3d(GLdouble(lual_checknumber(L,-3)),
               GLdouble(lual_checknumber(L,-2)),
               GLdouble(lual_checknumber(L,-1)))
  else if (lua_istable(L, 1) and (lua_objlen(L,1) = 4)) or (lua_gettop(L) = 4) then
    glVertex4d(GLdouble(lual_checknumber(L,-4)),
               GLdouble(lual_checknumber(L,-3)),
               GLdouble(lual_checknumber(L,-2)),
               GLdouble(lual_checknumber(L,-1)))
  else
    luaL_error(L, "incorrect argument to function \"gl.Vertex\"");
  result:=0; // number of results
}

int ULuaGl_Viewport(Plua_State L)
var
  i: int;
{
  if lua_istable(L, 1) and lua_istable(L, 2) then
  {
    for i := 1 to lua_objlen(L,1) do
      lua_rawgeti(L,1,i);
    for i := 1 to lua_objlen(L,2) do
      lua_rawgeti(L,2,i);
  }

  if  (lua_istable(L, 1) and (lua_objlen(L,1) = 2))
  and (lua_istable(L, 2) and (lua_objlen(L,2) = 2))
  or  (lua_gettop(L) = 4) then
    glViewport(lual_checkinteger(L,-4),
               lual_checkinteger(L,-3),
               lual_checkinteger(L,-2),
               lual_checkinteger(L,-1))
  else
    luaL_error(L, "incorrect argument to function \"gl.Viewport\"");
  result:=0; // number of results
}

int ULuaGl_Dummy(Plua_State L)
{
  result:=0; // number of results
}

int luaopen_gl(Plua_State L)
{
    luaL_register(L,"gl",@ULuaGl_Lib_f[0]);
    result:=1;
}

/*
  glAccum: procedure(op: GLenum; value: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glAlphaFunc: procedure(func: GLenum; ref: GLclampf); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glAreTexturesResident: function (n: GLsizei; const textures: PGLuint; residences: PGLboolean): GLboolean; {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glArrayElement: procedure(i: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glBitmap: procedure (width, height: GLsizei; xorig, yorig: GLfloat; xmove, ymove: GLfloat; const bitmap: PGLubyte); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glCallList: procedure(list: GLuint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glCallLists: procedure(n: GLsizei; atype: GLenum; const lists: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glClearDepth: procedure(depth: GLclampd); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glClearIndex: procedure(c: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glClearStencil: procedure(s: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glClipPlane: procedure(plane: GLenum; const equation: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}

  glColorMask: procedure(red, green, blue, alpha: GLboolean); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glColorMaterial: procedure(face, mode: GLenum); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glColorPointer: procedure(size: GLint; atype: GLenum; stride: GLsizei; const pointer: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glCopyPixels: procedure(x, y: GLint; width, height: GLsizei; atype: GLenum); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glCopyTexImage1D: procedure (target: GLenum; level: GLint; internalFormat: GLenum; x, y: GLint; width: GLsizei; border: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glCopyTexImage2D: procedure(target: GLenum; level: GLint; internalFormat: GLenum; x, y: GLint; width, height: GLsizei; border: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glCopyTexSubImage1D: procedure(target: GLenum; level, xoffset, x, y: GLint; width: GLsizei); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glCopyTexSubImage2D: procedure(target: GLenum; level, xoffset, yoffset, x, y: GLint; width, height: GLsizei); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glDeleteLists: procedure(list: GLuint; range: GLsizei); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glDeleteTextures: procedure(n: GLsizei; const textures: PGLuint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glDepthMask: procedure(flag: GLboolean); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glDrawArrays: procedure(mode: GLenum; first: GLint; count: GLsizei); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glDrawElements: procedure(mode: GLenum; count: GLsizei; atype: GLenum; const indices: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glDrawPixels: procedure(width, height: GLsizei; format, atype: GLenum; const pixels: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEdgeFlag: procedure(flag: GLboolean); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEdgeFlagPointer: procedure(stride: GLsizei; const pointer: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEdgeFlagv: procedure(const flag: PGLboolean); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}

  glEvalCoord1d: procedure(u: GLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEvalCoord1dv: procedure(const u: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEvalCoord1f: procedure(u: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEvalCoord1fv: procedure(const u: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEvalCoord2d: procedure(u, v: GLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEvalCoord2dv: procedure(const u: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEvalCoord2f: procedure(u, v: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEvalCoord2fv: procedure(const u: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}

  glEvalMesh1: procedure(mode: GLenum; i1, i2: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEvalMesh2: procedure(mode: GLenum; i1, i2, j1, j2: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEvalPoint1: procedure(i: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glEvalPoint2: procedure(i, j: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glFeedbackBuffer: procedure(size: GLsizei; atype: GLenum; buffer: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glFogf: procedure(pname: GLenum; param: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glFogfv: procedure(pname: GLenum; const params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glFogi: procedure(pname: GLenum; param: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glFogiv: procedure(pname: GLenum; const params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glFrustum: procedure(left, right, bottom, top, zNear, zFar: GLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGenLists: function(range: GLsizei): GLuint; {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGenTextures: procedure(n: GLsizei; textures: PGLuint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetBooleanv: procedure(pname: GLenum; params: PGLboolean); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetClipPlane: procedure(plane: GLenum; equation: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetDoublev: procedure(pname: GLenum; params: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
//  glGetError: function: GLenum; {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetFloatv: procedure(pname: GLenum; params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetIntegerv: procedure(pname: GLenum; params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetLightfv: procedure(light, pname: GLenum; params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetLightiv: procedure(light, pname: GLenum; params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetMapdv: procedure(target, query: GLenum; v: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetMapfv: procedure(target, query: GLenum; v: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetMapiv: procedure(target, query: GLenum; v: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetMaterialfv: procedure(face, pname: GLenum; params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetMaterialiv: procedure(face, pname: GLenum; params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetPixelMapfv: procedure(map: GLenum; values: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetPixelMapuiv: procedure(map: GLenum; values: PGLuint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetPixelMapusv: procedure(map: GLenum; values: PGLushort); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetPointerv: procedure(pname: GLenum; params: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetPolygonStipple: procedure(mask: PGLubyte); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
/  glGetString: function(name: GLenum): PChar; {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetTexEnvfv: procedure(target, pname: GLenum; params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetTexEnviv: procedure(target, pname: GLenum; params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetTexGendv: procedure(coord, pname: GLenum; params: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetTexGenfv: procedure(coord, pname: GLenum; params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetTexGeniv: procedure(coord, pname: GLenum; params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetTexImage: procedure(target: GLenum; level: GLint; format: GLenum; atype: GLenum; pixels: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetTexLevelParameterfv: procedure(target: GLenum; level: GLint; pname: GLenum; params: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetTexLevelParameteriv: procedure(target: GLenum; level: GLint; pname: GLenum; params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetTexParameterfv: procedure(target, pname: GLenum; params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glGetTexParameteriv: procedure(target, pname: GLenum; params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glHint: procedure(target, mode: GLenum); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIndexMask: procedure(mask: GLuint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}

  glIndexPointer: procedure(atype: GLenum; stride: GLsizei; const pointer: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIndexd: procedure(c: GLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIndexdv: procedure(const c: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIndexf: procedure(c: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIndexfv: procedure(const c: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIndexi: procedure(c: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIndexiv: procedure(const c: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIndexs: procedure(c: GLshort); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIndexsv: procedure(const c: PGLshort); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIndexub: procedure(c: GLubyte); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIndexubv: procedure(const c: PGLubyte); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}

  glInterleavedArrays: procedure(format: GLenum; stride: GLsizei; const pointer: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
/  glIsEnabled: function(cap: GLenum): GLboolean; {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIsList: function(list: GLuint): GLboolean; {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glIsTexture: function(texture: GLuint): GLboolean; {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLightModelf: procedure(pname: GLenum; param: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLightModelfv: procedure(pname: GLenum; const params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLightModeli: procedure(pname: GLenum; param: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLightModeliv: procedure(pname: GLenum; const params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLightf: procedure(light, pname: GLenum; param: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLightfv: procedure(light, pname: GLenum; const params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLighti: procedure(light, pname: GLenum; param: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLightiv: procedure(light, pname: GLenum; const params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLineStipple: procedure(factor: GLint; pattern: GLushort); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLineWidth: procedure(width: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glListBase: procedure(base: GLuint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLoadMatrixd: procedure(const m: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLoadMatrixf: procedure(const m: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glLoadName: procedure(name: GLuint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMap1d: procedure(target: GLenum; u1, u2: GLdouble; stride, order: GLint; const points: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMap1f: procedure(target: GLenum; u1, u2: GLfloat; stride, order: GLint; const points: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMap2d: procedure(target: GLenum; u1, u2: GLdouble; ustride, uorder: GLint; v1, v2: GLdouble; vstride, vorder: GLint; const points: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMap2f: procedure(target: GLenum; u1, u2: GLfloat; ustride, uorder: GLint; v1, v2: GLfloat; vstride, vorder: GLint; const points: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMapGrid1d: procedure(un: GLint; u1, u2: GLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMapGrid1f: procedure(un: GLint; u1, u2: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMapGrid2d: procedure(un: GLint; u1, u2: GLdouble; vn: GLint; v1, v2: GLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMapGrid2f: procedure(un: GLint; u1, u2: GLfloat; vn: GLint; v1, v2: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMaterialf: procedure(face, pname: GLenum; param: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMaterialfv: procedure(face, pname: GLenum; const params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMateriali: procedure(face, pname: GLenum; param: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMaterialiv: procedure(face, pname: GLenum; const params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMultMatrixd: procedure(const m: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glMultMatrixf: procedure(const m: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glNewList: procedure(list: GLuint; mode: GLenum); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glNormal3b: procedure(nx, ny, nz: GLbyte); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glNormal3bv: procedure(const v: PGLbyte); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glNormal3d: procedure(nx, ny, nz: GLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glNormal3dv: procedure(const v: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glNormal3f: procedure(nx, ny, nz: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glNormal3fv: procedure(const v: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glNormal3i: procedure(nx, ny, nz: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glNormal3iv: procedure(const v: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glNormal3s: procedure(nx, ny, nz: GLshort); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glNormal3sv: procedure(const v: PGLshort); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glNormalPointer: procedure(atype: GLenum; stride: GLsizei; const pointer: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPassThrough: procedure(token: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPixelMapfv: procedure(map: GLenum; mapsize: GLsizei; const values: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPixelMapuiv: procedure(map: GLenum; mapsize: GLsizei; const values: PGLuint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPixelMapusv: procedure(map: GLenum; mapsize: GLsizei; const values: PGLushort); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPixelStoref: procedure(pname: GLenum; param: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPixelStorei: procedure(pname: GLenum; param: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPixelTransferf: procedure(pname: GLenum; param: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPixelTransferi: procedure(pname: GLenum; param: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPixelZoom: procedure(xfactor, yfactor: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPointSize: procedure(size: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPolygonMode: procedure(face, mode: GLenum); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPolygonOffset: procedure(factor, units: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPolygonStipple: procedure(const mask: PGLubyte); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPrioritizeTextures: procedure(n: GLsizei; const textures: PGLuint; const priorities: PGLclampf); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPushAttrib: procedure(mask: GLbitfield); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPushClientAttrib: procedure(mask: GLbitfield); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glPushName: procedure(name: GLuint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glReadPixels: procedure(x, y: GLint; width, height: GLsizei; format, atype: GLenum; pixels: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}

  glRenderMode: function(mode: GLint): GLint; {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glScissor: procedure(x, y: GLint; width, height: GLsizei); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glSelectBuffer: procedure(size: GLsizei; buffer: PGLuint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glStencilFunc: procedure(func: GLenum; ref: GLint; mask: GLuint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glStencilMask: procedure(mask: GLuint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glStencilOp: procedure(fail, zfail, zpass: GLenum); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}

  glTexCoordPointer: procedure(size: GLint; atype: GLenum; stride: GLsizei; const pointer: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexEnvf: procedure(target: GLenum; pname: GLenum; param: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexEnvfv: procedure(target: GLenum; pname: GLenum; const params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexEnvi: procedure(target: GLenum; pname: GLenum; param: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexEnviv: procedure(target: GLenum; pname: GLenum; const params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexGend: procedure(coord: GLenum; pname: GLenum; param: GLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexGendv: procedure(coord: GLenum; pname: GLenum; const params: PGLdouble); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexGenf: procedure(coord: GLenum; pname: GLenum; param: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexGenfv: procedure(coord: GLenum; pname: GLenum; const params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexGeni: procedure(coord: GLenum; pname: GLenum; param: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexGeniv: procedure(coord: GLenum; pname: GLenum; const params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexImage1D: procedure(target: GLenum; level, internalformat: GLint; width: GLsizei; border: GLint; format, atype: GLenum; const pixels: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexImage2D: procedure(target: GLenum; level, internalformat: GLint; width, height: GLsizei; border: GLint; format, atype: GLenum; const pixels: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexParameterf: procedure(target: GLenum; pname: GLenum; param: GLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexParameterfv: procedure(target: GLenum; pname: GLenum; const params: PGLfloat); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexParameteri: procedure(target: GLenum; pname: GLenum; param: GLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexParameteriv: procedure(target: GLenum; pname: GLenum; const params: PGLint); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexSubImage1D: procedure(target: GLenum; level, xoffset: GLint; width: GLsizei; format, atype: GLenum; const pixels: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glTexSubImage2D: procedure(target: GLenum; level, xoffset, yoffset: GLint; width, height: GLsizei; format, atype: GLenum; const pixels: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  glVertexPointer: procedure(size: GLint; atype: GLenum; stride: GLsizei; const pointer: Pointer); {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  {$IFDEF WINDOWS}
  ChoosePixelFormat: function(DC: HDC; p2: PPixelFormatDescriptor): int; {$IFDEF WINDOWS}stdcall;{$ELSE}cdecl;{$ENDIF}
  {$ENDIF}
  **/



    /*
GLenum ULuaGl_StringToEnum(Str: String);
  GLenum GetEnum(const Str: String);
  var
    i : int;
  {
    for i := 0 to high(ULuaGl_Enum) do
    {
      if 0 = AnsiCompareText(Str, ULuaGl_Enum[i].Text) then
      {
        Result := ULuaGl_Enum[i].Value;
        Exit;
      }
    }
    Result := ULuaGl_EnumERROR;
  }
  var
    i : int;
    j : int;
    temp : GLenum;
{
  Result := 0;
  j := 1;
  for i := 1 to Length(Str) do
  {
    if Str[i] = "," then
    {
      temp := GetEnum(Copy(Str,j,i-j));
      if temp <> ULuaGl_EnumERROR then
        Result := Result or temp;
      j := i + 1;
    }
  }

  temp := GetEnum(Copy(Str,j,MaxInt));
  if (temp = ULuaGl_EnumERROR) then
  {
    if Result = 0 then
      Result := ULuaGl_EnumERROR;
    exit;
  }
  Result := Result or temp;
}
}
    */