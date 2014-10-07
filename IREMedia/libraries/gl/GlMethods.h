/************************************************************************************

 Authors     :   Bradley Austin Davis <bdavis@saintandreas.org>
 Copyright   :   Copyright Brad Davis. All Rights reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 ************************************************************************************/

#pragma once
#include <vector>

namespace gl {
namespace Error {
enum {
  NONE = GL_NO_ERROR,
  INVALID_ENUM = GL_INVALID_ENUM,
  INVALID_VALUE = GL_INVALID_VALUE,
  INVALID_OPERATION = GL_INVALID_OPERATION,
//  STACK_OVERFLOW = GL_STACK_OVERFLOW,
//  STACK_UNDERFLOW = GL_STACK_UNDERFLOW,
  OUT_OF_MEMORY = GL_OUT_OF_MEMORY,
};
}

namespace DrawMode {
enum {
  POINTS = GL_POINTS,
  LINE_STRIP = GL_LINE_STRIP,
  LINE_LOOP = GL_LINE_LOOP,
  LINES = GL_LINES,
  LINE_STRIP_ADJACENCY = GL_LINE_STRIP_ADJACENCY,
  LINES_ADJACENCY = GL_LINES_ADJACENCY,
  TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
  TRIANGLE_FAN = GL_TRIANGLE_FAN,
  TRIANGLES = GL_TRIANGLES,
  TRIANGLE_STRIP_ADJACENCY = GL_TRIANGLE_STRIP_ADJACENCY,
  TRIANGLES_ADJACENCY = GL_TRIANGLES_ADJACENCY,
  PATCHES = GL_PATCHES,
};
}

inline void uniform(GLint location, GLfloat a) {
  glUniform1f(location, a);
}

inline void uniform(GLint location, GLint a) {
  glUniform1i(location, a);
}

inline void uniform(GLint location, const glm::vec2 & a) {
  glUniform2f(location, a.x, a.y);
}

inline void uniform(GLint location, const glm::vec4 & a) {
  glUniform4f(location, a.x, a.y, a.z, a.w);
}

inline void uniform(GLint location, const std::vector<glm::vec4> & a) {
  glUniform4fv(location, (GLsizei) a.size(), &(a[0][0]));
}

inline void uniform(GLint location, const glm::vec3 & a) {
  glUniform3f(location, a.x, a.y, a.z);
}

inline void uniform(GLint location, const glm::mat4 & a) {
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(a));
}

inline void lineWidth(GLfloat width) {
  glLineWidth(width);
}

inline void clearColor(const glm::vec4 & c) {
  glClearColor(c.r, c.g, c.b, c.a);
}

inline void clearColor(const glm::vec3 & c) {
  glClearColor(c.r, c.g, c.b, 1.0);
}

inline void scissor(const glm::ivec2 & v, const glm::uvec2 & size) {
  glScissor(v.x, v.y, size.x, size.y);
}

inline void viewport( const glm::uvec2 & pos, const glm::uvec2 & size ) {
  glViewport(pos.x, pos.y, size.x, size.y);
}

inline void viewport( const glm::uvec2 & size ) {
  viewport(glm::uvec2(), size);
}

}

namespace glm {
  template <typename T>
  inline float aspect(T const & size) {
    return (float) size.x / (float)size.y;
  }
}

//    inline void ColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
//    inline void AlphaFunc( GLenum func, GLclampf ref );
//    inline void BlendFunc( GLenum sfactor, GLenum dfactor );
//    inline void LogicOp( GLenum opcode );
//    inline void CullFace( GLenum mode );
//    inline void FrontFace( GLenum mode );
//    inline void PointSize( GLfloat size );
//    inline void LineStipple( GLint factor, GLushort pattern );
//    inline void PolygonMode( GLenum face, GLenum mode );
//    inline void PolygonOffset( GLfloat factor, GLfloat units );
//    inline void PolygonStipple( const GLubyte *mask );
//    inline void GetPolygonStipple( GLubyte *mask );
//    inline void EdgeFlag( GLboolean flag );
//    inline void EdgeFlagv( const GLboolean *flag );
//    inline void ClipPlane( GLenum plane, const GLdouble *equation );
//    inline void GetClipPlane( GLenum plane, GLdouble *equation );
//    inline void DrawBuffer( GLenum mode );
//    inline void ReadBuffer( GLenum mode );
//    inline void Enable( GLenum cap );
//    inline void Disable( GLenum cap );
//    inline bool IsEnabled( GLenum cap );
//    inline void EnableClientState( GLenum cap );  /* 1.1 */
//    inline void DisableClientState( GLenum cap );  /* 1.1 */
//    inline void GetBooleanv( GLenum pname, GLboolean *params );
//    inline void GetDoublev( GLenum pname, GLdouble *params );
//    inline void GetFloatv( GLenum pname, GLfloat *params );
//    inline void GetIntegerv( GLenum pname, GLint *params );
//    inline void PushAttrib( GLbitfield mask );
//    inline void PopAttrib( void );
//    inline void PushClientAttrib( GLbitfield mask );  /* 1.1 */
//    inline void PopClientAttrib( void );  /* 1.1 */
//    inline int32_tRenderMode( GLenum mode );
//    GLAPI GLenum GLAPIENTRY glGetError( void );
//    GLAPI const GLubyte * GLAPIENTRY glGetString( GLenum name );
//    inline void Finish( void );
//    inline void Flush( void );
//    inline void Hint( GLenum target, GLenum mode );
//    inline void ClearDepth( GLclampd depth );
//    inline void DepthFunc( GLenum func );
//    inline void DepthMask( GLboolean flag );
//    inline void DepthRange( GLclampd near_val, GLclampd far_val );
//    inline void ClearAccum( const glm::vec4 & color );
//    inline void VertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr );
//    inline void NormalPointer( GLenum type, GLsizei stride, const GLvoid *ptr );
//    inline void ColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr );
//    inline void IndexPointer( GLenum type, GLsizei stride, const GLvoid *ptr );
//    inline void TexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *ptr );
//    inline void EdgeFlagPointer( GLsizei stride, const GLvoid *ptr );
//    inline void GetPointerv( GLenum pname, GLvoid **params );
//    inline void ArrayElement( GLint i );
//    inline void DrawArrays( GLenum mode, GLint first, GLsizei count );
//    inline void DrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices );
//    inline void InterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer );
//    inline void ShadeModel( GLenum mode );
//    inline void PixelZoom( GLfloat xfactor, GLfloat yfactor );
//    inline void PixelStoref( GLenum pname, GLfloat param );
//    inline void PixelStorei( GLenum pname, GLint param );
//    inline void PixelTransferf( GLenum pname, GLfloat param );
//    inline void PixelTransferi( GLenum pname, GLint param );
//    inline void PixelMapfv( GLenum map, GLsizei mapsize, const GLfloat *values );
//    inline void PixelMapuiv( GLenum map, GLsizei mapsize, const GLuint *values );
//    inline void PixelMapusv( GLenum map, GLsizei mapsize, const GLushort *values );
//    inline void GetPixelMapfv( GLenum map, GLfloat *values );
//    inline void GetPixelMapuiv( GLenum map, GLuint *values );
//    inline void GetPixelMapusv( GLenum map, GLushort *values );
//    inline void Bitmap( const glm::ivec2 & size, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap );
//    inline void ReadPixels( const glm::ivec2 & v, const glm::ivec2 & size, GLenum format, GLenum type, GLvoid *pixels );
//    inline void DrawPixels( const glm::ivec2 & size, GLenum format, GLenum type, const GLvoid *pixels );
//    inline void CopyPixels( const glm::ivec2 & v, const glm::ivec2 & size, GLenum type );
//    inline void StencilFunc( GLenum func, GLint ref, GLuint mask );
//    inline void StencilMask( GLuint mask );
//    inline void StencilOp( GLenum fail, GLenum zfail, GLenum zpass );
//    inline void ClearStencil( GLint s );
//    inline void TexGend( GLenum coord, GLenum pname, GLdouble param );
//    inline void TexGenf( GLenum coord, GLenum pname, GLfloat param );
//    inline void TexGeni( GLenum coord, GLenum pname, GLint param );
//    inline void TexGendv( GLenum coord, GLenum pname, const GLdouble *params );
//    inline void TexGenfv( GLenum coord, GLenum pname, const GLfloat *params );
//    inline void TexGeniv( GLenum coord, GLenum pname, const GLint *params );
//    inline void GetTexGendv( GLenum coord, GLenum pname, GLdouble *params );
//    inline void GetTexGenfv( GLenum coord, GLenum pname, GLfloat *params );
//    inline void GetTexGeniv( GLenum coord, GLenum pname, GLint *params );
//    inline void TexEnvf( GLenum target, GLenum pname, GLfloat param );
//    inline void TexEnvi( GLenum target, GLenum pname, GLint param );
//    inline void TexEnvfv( GLenum target, GLenum pname, const GLfloat *params );
//    inline void TexEnviv( GLenum target, GLenum pname, const GLint *params );
//    inline void GetTexEnvfv( GLenum target, GLenum pname, GLfloat *params );
//    inline void GetTexEnviv( GLenum target, GLenum pname, GLint *params );
//    inline void TexParameterf( GLenum target, GLenum pname, GLfloat param );
//    inline void TexParameteri( GLenum target, GLenum pname, GLint param );
//    inline void TexParameterfv( GLenum target, GLenum pname, const GLfloat *params );
//    inline void TexParameteriv( GLenum target, GLenum pname, const GLint *params );
//    inline void GetTexParameterfv( GLenum target, GLenum pname, GLfloat *params);
//    inline void GetTexParameteriv( GLenum target, GLenum pname, GLint *params );
//    inline void GetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params );
//    inline void GetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params );
//    inline void TexImage1D( GLenum target, GLint level, GLint internalFormat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
//    inline void TexImage2D( GLenum target, GLint level, GLint internalFormat, const glm::ivec2 & size, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
//    inline void GetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels );
//    inline void GenTextures( GLsizei n, GLuint *textures );
//    inline void DeleteTextures( GLsizei n, const GLuint *textures);
//    inline void BindTexture( GLenum target, GLuint texture );
//    inline void PrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities );
//    inline bool AreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences );
//    inline bool IsTexture( GLuint texture );
//    inline void TexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels );
//    inline void TexSubImage2D( GLenum target, GLint level, const glm::ivec2 & offset, const glm::ivec2 & size, GLenum format, GLenum type, const GLvoid *pixels );
//    inline void CopyTexImage1D( GLenum target, GLint level, GLenum internalformat, const glm::ivec2 & v, GLsizei width, GLint border );
//    inline void CopyTexImage2D( GLenum target, GLint level, GLenum internalformat, const glm::ivec2 & v, const glm::ivec2 & size, GLint border );
//    inline void CopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, const glm::ivec2 & v, GLsizei width );
//    inline void CopyTexSubImage2D( GLenum target, GLint level, const glm::ivec2 & offset, const glm::ivec2 & v, const glm::ivec2 & size );
//    inline void Map1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points );
//    inline void Map1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points );
//    inline void Map2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points );
//    inline void Map2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points );
//    inline void GetMapdv( GLenum target, GLenum query, GLdouble *v );
//    inline void GetMapfv( GLenum target, GLenum query, GLfloat *v );
//    inline void GetMapiv( GLenum target, GLenum query, GLint *v );
//    inline void EvalCoord1d( GLdouble u );
//    inline void EvalCoord1f( GLfloat u );
//    inline void EvalCoord2d( GLdouble u, GLdouble v );
//    inline void EvalCoord2f( GLfloat u, GLfloat v );
//    inline void MapGrid1d( GLint un, GLdouble u1, GLdouble u2 );
//    inline void MapGrid1f( GLint un, GLfloat u1, GLfloat u2 );
//    inline void MapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 );
//    inline void MapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 );
//    inline void EvalPoint1( GLint i );
//    inline void EvalPoint2( GLint i, GLint j );
//    inline void EvalMesh1( GLenum mode, GLint i1, GLint i2 );
//    inline void EvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );
//    inline void Fogf( GLenum pname, GLfloat param );
//    inline void Fogi( GLenum pname, GLint param );
//    inline void Fogfv( GLenum pname, const GLfloat *params );
//    inline void Fogiv( GLenum pname, const GLint *params );
//    inline void FeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer );
//    inline void PassThrough( GLfloat token );
//    inline void SelectBuffer( GLsizei size, GLuint *buffer );
//    inline void InitNames( void );
//    inline void LoadName( GLuint name );
//    inline void PushName( GLuint name );
//    inline void PopName( void );
//    inline void DrawRangeElements( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices );
//    inline void TexImage3D( GLenum target, GLint level, GLint internalFormat, const glm::ivec3 & size, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
//    inline void TexSubImage3D( GLenum target, GLint level, const glm::ivec3 & offset, const glm::ivec3 & size, GLenum format, GLenum type, const GLvoid *pixels);
//    inline void CopyTexSubImage3D( GLenum target, GLint level, const glm::ivec3 & offset, GLint x, GLint y, const glm::ivec2 & size );
//    inline void ColorTable( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table );
//    inline void ColorSubTable( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data );
//    inline void ColorTableParameteriv(GLenum target, GLenum pname, const GLint *params);
//    inline void ColorTableParameterfv(GLenum target, GLenum pname, const GLfloat *params);
//    inline void CopyColorSubTable( GLenum target, GLsizei start, const glm::ivec2 & v, GLsizei width );
//    inline void CopyColorTable( GLenum target, GLenum internalformat, const glm::ivec2 & v, GLsizei width );
//    inline void GetColorTable( GLenum target, GLenum format, GLenum type, GLvoid *table );
//    inline void GetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params );
//    inline void GetColorTableParameteriv( GLenum target, GLenum pname, GLint *params );
//    inline void BlendEquation( GLenum mode );
//    inline void BlendColor( const glm::vec4 & c );
//    inline void Histogram( GLenum target, GLsizei width, GLenum internalformat, GLboolean sink );
//    inline void ResetHistogram( GLenum target );
//    inline void GetHistogram( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values );
//    inline void GetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params );
//    inline void GetHistogramParameteriv( GLenum target, GLenum pname, GLint *params );
//    inline void Minmax( GLenum target, GLenum internalformat, GLboolean sink );
//    inline void ResetMinmax( GLenum target );
//    inline void GetMinmax( GLenum target, GLboolean reset, GLenum format, GLenum types, GLvoid *values );
//    inline void GetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params );
//    inline void GetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params );
//    inline void ConvolutionFilter1D( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image );
//    inline void ConvolutionFilter2D( GLenum target, GLenum internalformat, const glm::ivec2 & size, GLenum format, GLenum type, const GLvoid *image );
//    inline void ConvolutionParameterf( GLenum target, GLenum pname, GLfloat params );
//    inline void ConvolutionParameterfv( GLenum target, GLenum pname, const GLfloat *params );
//    inline void ConvolutionParameteri( GLenum target, GLenum pname, GLint params );
//    inline void ConvolutionParameteriv( GLenum target, GLenum pname, const GLint *params );
//    inline void CopyConvolutionFilter1D( GLenum target, GLenum internalformat, const glm::ivec2 & v, GLsizei width );
//    inline void CopyConvolutionFilter2D( GLenum target, GLenum internalformat, const glm::ivec2 & v, const glm::ivec2 & size);
//    inline void GetConvolutionFilter( GLenum target, GLenum format, GLenum type, GLvoid *image );
//    inline void GetConvolutionParameterfv( GLenum target, GLenum pname, GLfloat *params );
//    inline void GetConvolutionParameteriv( GLenum target, GLenum pname, GLint *params );
//    inline void SeparableFilter2D( GLenum target, GLenum internalformat, const glm::ivec2 & size, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column );
//    inline void GetSeparableFilter( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span );
//    inline void ActiveTexture( GLenum texture );
//    inline void ClientActiveTexture( GLenum texture );
//    inline void CompressedTexImage1D( GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *data );
//    inline void CompressedTexImage2D( GLenum target, GLint level, GLenum internalformat, const glm::ivec2 & size, GLint border, GLsizei imageSize, const GLvoid *data );
//    inline void CompressedTexImage3D( GLenum target, GLint level, GLenum internalformat, const glm::ivec3 & size, GLint border, GLsizei imageSize, const GLvoid *data );
//    inline void CompressedTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data );
//    inline void CompressedTexSubImage2D( GLenum target, GLint level, const glm::ivec2 & offset, const glm::ivec2 & size, GLenum format, GLsizei imageSize, const GLvoid *data );
//    inline void CompressedTexSubImage3D( GLenum target, GLint level, const glm::ivec3 & offset, const glm::ivec3 & size, GLenum format, GLsizei imageSize, const GLvoid *data );
//    inline void GetCompressedTexImage( GLenum target, GLint lod, GLvoid *img );
//    inline void LoadTransposeMatrixd( const GLdouble m[16] );
//    inline void LoadTransposeMatrixf( const GLfloat m[16] );
//    inline void MultTransposeMatrixd( const GLdouble m[16] );
//    inline void MultTransposeMatrixf( const GLfloat m[16] );
//    inline void SampleCoverage( GLclampf value, GLboolean invert );
