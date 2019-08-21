#ifndef __GLEXT_1_2_H
#define __GLEXT_1_2_H

#include<windows.h>

#ifdef _MSC_VER
#pragma comment(lib, "opengl32.lib")
#endif

// Section 2.4
// Basic GL Operation
typedef void GLvoid;
typedef unsigned __int8 GLboolean;
typedef __int8 GLbyte;
typedef unsigned __int8 GLubyte;
typedef __int16 GLshort;
typedef unsigned __int16 GLushort;
typedef __int32 GLint;
typedef unsigned __int32 GLuint;
typedef unsigned __int32 GLsizei;
typedef __int32 GLenum;
typedef unsigned __int32 GLbitfield;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;

// Section 2.5
// GL Errors
GLenum WINAPI glGetError(GLvoid);

// Section 2.6.1
// Begin and End Objects
GLvoid WINAPI glBegin(GLenum mode);
GLvoid WINAPI glEnd(GLvoid);

// Section 2.6.2
// Polygon Edges
GLvoid WINAPI glEdgeFlag(GLboolean flag);
GLvoid WINAPI glEdgeFlagv(GLboolean* flag);

// Section 2.7
// Vertex Specification
GLvoid WINAPI glVertex2s(GLshort x, GLshort y);
GLvoid WINAPI glVertex3s(GLshort x, GLshort y, GLshort z);
GLvoid WINAPI glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w);
GLvoid WINAPI glVertex2i(GLint x, GLint y);
GLvoid WINAPI glVertex3i(GLint x, GLint y, GLint z);
GLvoid WINAPI glVertex4i(GLint x, GLint y, GLint z, GLint w);
GLvoid WINAPI glVertex2f(GLfloat x, GLfloat y);
GLvoid WINAPI glVertex3f(GLfloat x, GLfloat y, GLfloat z);
GLvoid WINAPI glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLvoid WINAPI glVertex2d(GLdouble x, GLdouble y);
GLvoid WINAPI glVertex3d(GLdouble x, GLdouble y, GLdouble z);
GLvoid WINAPI glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLvoid WINAPI glVertex2sv(const GLshort* coords);
GLvoid WINAPI glVertex2iv(const GLint* coords);
GLvoid WINAPI glVertex2fv(const GLfloat* coords);
GLvoid WINAPI glVertex2dv(const GLdouble* coords);
GLvoid WINAPI glVertex3sv(const GLshort* coords);
GLvoid WINAPI glVertex3iv(const GLint* coords);
GLvoid WINAPI glVertex3fv(const GLfloat* coords);
GLvoid WINAPI glVertex3dv(const GLdouble* coords);
GLvoid WINAPI glVertex4sv(const GLshort* coords);
GLvoid WINAPI glVertex4iv(const GLint* coords);
GLvoid WINAPI glVertex4fv(const GLfloat* coords);
GLvoid WINAPI glVertex4dv(const GLdouble* coords);
GLvoid WINAPI glTexCoord1s(GLshort s);
GLvoid WINAPI glTexCoord1i(GLint s);
GLvoid WINAPI glTexCoord1f(GLfloat s);
GLvoid WINAPI glTexCoord1d(GLdouble s);
GLvoid WINAPI glTexCoord2s(GLshort s, GLshort t);
GLvoid WINAPI glTexCoord2i(GLint s, GLint t);
GLvoid WINAPI glTexCoord2f(GLfloat s, GLfloat t);
GLvoid WINAPI glTexCoord2d(GLdouble s, GLdouble t);
GLvoid WINAPI glTexCoord3s(GLshort s, GLshort t, GLshort r);
GLvoid WINAPI glTexCoord3i(GLint s, GLint t, GLint r);
GLvoid WINAPI glTexCoord3f(GLfloat s, GLfloat t, GLfloat r);
GLvoid WINAPI glTexCoord3d(GLdouble s, GLdouble t, GLdouble r);
GLvoid WINAPI glTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q);
GLvoid WINAPI glTexCoord4i(GLint s, GLint t, GLint r, GLint q);
GLvoid WINAPI glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
GLvoid WINAPI glTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q);
GLvoid WINAPI glTexCoord1sv(const GLshort* coords);
GLvoid WINAPI glTexCoord1iv(const GLint* coords);
GLvoid WINAPI glTexCoord1fv(const GLfloat* coords);
GLvoid WINAPI glTexCoord1dv(const GLdouble* coords);
GLvoid WINAPI glTexCoord2sv(const GLshort* coords);
GLvoid WINAPI glTexCoord2iv(const GLint* coords);
GLvoid WINAPI glTexCoord2fv(const GLfloat* coords);
GLvoid WINAPI glTexCoord2dv(const GLdouble* coords);
GLvoid WINAPI glTexCoord3sv(const GLshort* coords);
GLvoid WINAPI glTexCoord3iv(const GLint* coords);
GLvoid WINAPI glTexCoord3fv(const GLfloat* coords);
GLvoid WINAPI glTexCoord3dv(const GLdouble* coords);
GLvoid WINAPI glTexCoord4sv(const GLshort* coords);
GLvoid WINAPI glTexCoord4iv(const GLint* coords);
GLvoid WINAPI glTexCoord4fv(const GLfloat* coords);
GLvoid WINAPI glTexCoord4dv(const GLdouble* coords);
GLvoid WINAPI glMultiTexCoord1s(GLenum texture, GLshort s);
GLvoid WINAPI glMultiTexCoord1i(GLenum texture, GLint s);
GLvoid WINAPI glMultiTexCoord1f(GLenum texture, GLfloat s);
GLvoid WINAPI glMultiTexCoord1d(GLenum texture, GLdouble s);
GLvoid WINAPI glMultiTexCoord2s(GLenum texture, GLshort s, GLshort t);
GLvoid WINAPI glMultiTexCoord2i(GLenum texture, GLint s, GLint t);
GLvoid WINAPI glMultiTexCoord2f(GLenum texture, GLfloat s, GLfloat t);
GLvoid WINAPI glMultiTexCoord2d(GLenum texture, GLdouble s, GLdouble t);
GLvoid WINAPI glMultiTexCoord3s(GLenum texture, GLshort s, GLshort t, GLshort r);
GLvoid WINAPI glMultiTexCoord3i(GLenum texture, GLint s, GLint t, GLint r);
GLvoid WINAPI glMultiTexCoord3f(GLenum texture, GLfloat s, GLfloat t, GLfloat r);
GLvoid WINAPI glMultiTexCoord3d(GLenum texture, GLdouble s, GLdouble t, GLdouble r);
GLvoid WINAPI glMultiTexCoord4s(GLenum texture, GLshort s, GLshort t, GLshort r, GLshort q);
GLvoid WINAPI glMultiTexCoord4i(GLenum texture, GLint s, GLint t, GLint r, GLint q);
GLvoid WINAPI glMultiTexCoord4f(GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
GLvoid WINAPI glMultiTexCoord4d(GLenum texture, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
GLvoid WINAPI glMultiTexCoord1sv(GLenum texture, const GLshort* coords);
GLvoid WINAPI glMultiTexCoord1iv(GLenum texture, const GLint* coords);
GLvoid WINAPI glMultiTexCoord1fv(GLenum texture, const GLfloat* coords);
GLvoid WINAPI glMultiTexCoord1dv(GLenum texture, const GLdouble* coords);
GLvoid WINAPI glMultiTexCoord2sv(GLenum texture, const GLshort* coords);
GLvoid WINAPI glMultiTexCoord2iv(GLenum texture, const GLint* coords);
GLvoid WINAPI glMultiTexCoord2fv(GLenum texture, const GLfloat* coords);
GLvoid WINAPI glMultiTexCoord2dv(GLenum texture, const GLdouble* coords);
GLvoid WINAPI glMultiTexCoord3sv(GLenum texture, const GLshort* coords);
GLvoid WINAPI glMultiTexCoord3iv(GLenum texture, const GLint* coords);
GLvoid WINAPI glMultiTexCoord3fv(GLenum texture, const GLfloat* coords);
GLvoid WINAPI glMultiTexCoord3dv(GLenum texture, const GLdouble* coords);
GLvoid WINAPI glMultiTexCoord4sv(GLenum texture, const GLshort* coords);
GLvoid WINAPI glMultiTexCoord4iv(GLenum texture, const GLint* coords);
GLvoid WINAPI glMultiTexCoord4fv(GLenum texture, const GLfloat* coords);
GLvoid WINAPI glMultiTexCoord4dv(GLenum texture, const GLdouble* coords);
GLvoid WINAPI glNormal3b(GLbyte x, GLbyte y, GLbyte z);
GLvoid WINAPI glNormal3s(GLshort x, GLshort y, GLshort z);
GLvoid WINAPI glNormal3i(GLint x, GLint y, GLint z);
GLvoid WINAPI glNormal3f(GLfloat x, GLfloat y, GLfloat z);
GLvoid WINAPI glNormal3d(GLdouble x, GLdouble y, GLdouble z);
GLvoid WINAPI glNormal3bv(const GLbyte* coords);
GLvoid WINAPI glNormal3sv(const GLshort* coords);
GLvoid WINAPI glNormal3iv(const GLint* coords);
GLvoid WINAPI glNormal3fv(const GLfloat* coords);
GLvoid WINAPI glNormal3dv(const GLdouble* coords);
GLvoid WINAPI glColor3b(GLbyte r, GLbyte g, GLbyte b);
GLvoid WINAPI glColor3s(GLshort r, GLshort g, GLshort b);
GLvoid WINAPI glColor3i(GLint r, GLint g, GLint b);
GLvoid WINAPI glColor3f(GLfloat r, GLfloat g, GLfloat b);
GLvoid WINAPI glColor3d(GLdouble r, GLdouble g, GLdouble b);
GLvoid WINAPI glColor3ub(GLubyte r, GLubyte g, GLubyte b);
GLvoid WINAPI glColor3us(GLushort r, GLushort g, GLushort b);
GLvoid WINAPI glColor3ui(GLuint r, GLuint g, GLuint b);
GLvoid WINAPI glColor4b(GLbyte r, GLbyte g, GLbyte b, GLbyte a);
GLvoid WINAPI glColor4s(GLshort r, GLshort g, GLshort b, GLshort a);
GLvoid WINAPI glColor4i(GLint r, GLint g, GLint b, GLint a);
GLvoid WINAPI glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
GLvoid WINAPI glColor4d(GLdouble r, GLdouble g, GLdouble b, GLdouble a);
GLvoid WINAPI glColor4ub(GLubyte r, GLubyte g, GLubyte b, GLubyte a);
GLvoid WINAPI glColor4us(GLushort r, GLushort g, GLushort b, GLushort a);
GLvoid WINAPI glColor4ui(GLuint r, GLuint g, GLuint b, GLuint a);
GLvoid WINAPI glColor3bv(const GLbyte* components);
GLvoid WINAPI glColor3sv(const GLshort* components);
GLvoid WINAPI glColor3iv(const GLint* components);
GLvoid WINAPI glColor3fv(const GLfloat* components);
GLvoid WINAPI glColor3dv(const GLdouble* components);
GLvoid WINAPI glColor3ubv(const GLubyte* components);
GLvoid WINAPI glColor3usv(const GLushort* components);
GLvoid WINAPI glColor3uiv(const GLuint* components);
GLvoid WINAPI glColor4bv(const GLbyte* components);
GLvoid WINAPI glColor4sv(const GLshort* components);
GLvoid WINAPI glColor4iv(const GLint* components);
GLvoid WINAPI glColor4fv(const GLfloat* components);
GLvoid WINAPI glColor4dv(const GLdouble* components);
GLvoid WINAPI glColor4ubv(const GLubyte* components);
GLvoid WINAPI glColor4usv(const GLushort* components);
GLvoid WINAPI glColor4uiv(const GLuint* components);
GLvoid WINAPI glIndexs(GLshort index);
GLvoid WINAPI glIndexi(GLint index);
GLvoid WINAPI glIndexf(GLfloat index);
GLvoid WINAPI glIndexd(GLdouble index);
GLvoid WINAPI glIndexub(GLubyte index);
GLvoid WINAPI glIndexsv(const GLshort* index);
GLvoid WINAPI glIndexiv(const GLint* index);
GLvoid WINAPI glIndexfv(const GLfloat* index);
GLvoid WINAPI glIndexdv(const GLdouble* index);
GLvoid WINAPI glIndexubv(const GLubyte* index);

// Section 2.8
// Vertex Arrays
GLvoid WINAPI glClientActiveTexture(GLenum texture);
GLvoid WINAPI glEdgeFlagPointer(GLsizei stride, const GLvoid* pointer);
GLvoid WINAPI glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
GLvoid WINAPI glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
GLvoid WINAPI glIndexPointer(GLenum type, GLsizei stride, const GLvoid* pointer);
GLvoid WINAPI glNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer);
GLvoid WINAPI glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer);
GLvoid WINAPI glEnableClientState(GLenum array);
GLvoid WINAPI glDisableClientState(GLenum array);
GLvoid WINAPI glArrayElement(GLint i);
GLvoid WINAPI glDrawArrays(GLenum mode, GLint first, GLsizei count);
GLvoid WINAPI glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices);
GLvoid WINAPI glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices);
GLvoid WINAPI glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid* pointer);

// Section 2.9
// Rectangles
GLvoid WINAPI glRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2);
GLvoid WINAPI glRecti(GLint x1, GLint y1, GLint x2, GLint y2);
GLvoid WINAPI glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
GLvoid WINAPI glRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
GLvoid WINAPI glRectsv(GLshort v1[2], GLshort v2[2]);
GLvoid WINAPI glRectiv(GLint v1[2], GLint v2[2]);
GLvoid WINAPI glRectfv(GLfloat v1[2], GLfloat v2[2]);
GLvoid WINAPI glRectdv(GLdouble v1[2], GLdouble v2[2]);

// Section 2.10.1
// Controlling the Viewport
GLvoid WINAPI glDepthRange(GLclampd n, GLclampd f);
GLvoid WINAPI glViewport(GLint x, GLint y, GLsizei w, GLsizei h);

// Section 2.10.2
// Matrices
GLvoid WINAPI glMatrixMode(GLenum mode);
GLvoid WINAPI glLoadMatrixf(GLfloat m[16]);
GLvoid WINAPI glLoadMatrixd(GLdouble m[16]);
GLvoid WINAPI glMultMatrixf(GLfloat m[16]);
GLvoid WINAPI glMultMatrixd(GLdouble m[16]);
GLvoid WINAPI glLoadIdentity(GLvoid);
GLvoid WINAPI glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
GLvoid WINAPI glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
GLvoid WINAPI glTranslatef(GLfloat x, GLfloat y, GLfloat z);
GLvoid WINAPI glTranslated(GLdouble x, GLdouble y, GLdouble z);
GLvoid WINAPI glScalef(GLfloat x, GLfloat y, GLfloat z);
GLvoid WINAPI glScaled(GLdouble x, GLdouble y, GLdouble z);
GLvoid WINAPI glFrustum(GLdouble l, GLdouble r, GLdouble b, GLdouble t, GLdouble n, GLdouble f);
GLvoid WINAPI glOrtho(GLdouble l, GLdouble r, GLdouble b, GLdouble t, GLdouble n, GLdouble f);
GLvoid WINAPI glPushMatrix(GLvoid);
GLvoid WINAPI glActiveTexture(GLenum texture);
GLvoid WINAPI glPopMatrix(GLvoid);

// Section 2.10.3
// Normal Transformation
GLvoid WINAPI glEnable(GLenum target);
GLvoid WINAPI glDisable(GLenum target);

// Section 2.10.4
// Generating Texture Coordinates
GLvoid WINAPI glTexGeni(GLenum coord, GLenum pname, GLint param);
GLvoid WINAPI glTexGenf(GLenum coord, GLenum pname, GLfloat param);
GLvoid WINAPI glTexGend(GLenum coord, GLenum pname, GLdouble param);
GLvoid WINAPI glTexGeniv(GLenum coord, GLenum pname, const GLint* params);
GLvoid WINAPI glTexGenfv(GLenum coord, GLenum pname, const GLfloat* params);
GLvoid WINAPI glTexGendv(GLenum coord, GLenum pname, const GLdouble* params);

// Section 2.11
// Clipping
GLvoid WINAPI glClipPlane(GLenum p, GLdouble eqn[4]);

// Section 2.12
// Current Raster Position
GLvoid WINAPI glRasterPos2s(GLshort x, GLshort y);
GLvoid WINAPI glRasterPos2i(GLint x, GLint y);
GLvoid WINAPI glRasterPos2f(GLfloat x, GLfloat y);
GLvoid WINAPI glRasterPos2d(GLdouble x, GLdouble y);
GLvoid WINAPI glRasterPos3s(GLshort x, GLshort y, GLshort z);
GLvoid WINAPI glRasterPos3i(GLint x, GLint y, GLint z);
GLvoid WINAPI glRasterPos3f(GLfloat x, GLfloat y, GLfloat z);
GLvoid WINAPI glRasterPos3d(GLdouble x, GLdouble y, GLdouble z);
GLvoid WINAPI glRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w);
GLvoid WINAPI glRasterPos4i(GLint x, GLint y, GLint z, GLint w);
GLvoid WINAPI glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
GLvoid WINAPI glRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
GLvoid WINAPI glRasterPos2sv(const GLshort* coords);
GLvoid WINAPI glRasterPos2iv(const GLint* coords);
GLvoid WINAPI glRasterPos2fv(const GLfloat* coords);
GLvoid WINAPI glRasterPos2dv(const GLdouble* coords);
GLvoid WINAPI glRasterPos3sv(const GLshort* coords);
GLvoid WINAPI glRasterPos3iv(const GLint* coords);
GLvoid WINAPI glRasterPos3fv(const GLfloat* coords);
GLvoid WINAPI glRasterPos3dv(const GLdouble* coords);
GLvoid WINAPI glRasterPos4sv(const GLshort* coords);
GLvoid WINAPI glRasterPos4iv(const GLint* coords);
GLvoid WINAPI glRasterPos4fv(const GLfloat* coords);
GLvoid WINAPI glRasterPos4dv(const GLdouble* coords);

// Section 2.13.1
// Lighting
GLvoid WINAPI glFrontFace(GLenum dir);

// Section 2.13.2
// Lighting Parameter Specification
GLvoid WINAPI glMateriali(GLenum face, GLenum pname, GLint param);
GLvoid WINAPI glMaterialf(GLenum face, GLenum pname, GLfloat param);
GLvoid WINAPI glMaterialiv(GLenum face, GLenum pname, const GLint* params);
GLvoid WINAPI glMaterialfv(GLenum face, GLenum pname, const GLfloat* params);
GLvoid WINAPI glLighti(GLenum light, GLenum pname, GLint param);
GLvoid WINAPI glLightf(GLenum light, GLenum pname, GLfloat param);
GLvoid WINAPI glLightiv(GLenum light, GLenum pname, const GLint* params);
GLvoid WINAPI glLightfv(GLenum light, GLenum pname, const GLfloat* params);
GLvoid WINAPI glLightModeli(GLenum pname, GLint param);
GLvoid WINAPI glLightModelf(GLenum pname, GLfloat param);
GLvoid WINAPI glLightModeliv(GLenum pname, const GLint* params);
GLvoid WINAPI glLightModelfv(GLenum pname, const GLfloat* params);

// Section 2.13.3
// Color Material
GLvoid WINAPI glColorMaterial(GLenum face, GLenum mode);

// Section 2.13.7
// Flatshading
GLvoid WINAPI glShadeModel(GLenum mode);

// Section 3.3
// Points
GLvoid WINAPI glPointSize(GLfloat size);

// Section 3.4
// Line Segments
GLvoid WINAPI glLineWidth(GLfloat width);
GLvoid WINAPI glLineStipple(GLint factor, GLushort pattern);

// Section 3.5.1
// Basic Polygon Rasterization
GLvoid WINAPI glCullFace(GLenum mode);

// Section 3.5.2
// Stippling
GLvoid WINAPI glPolygonStipple(const GLubyte* pattern);

// Section 3.5.4
// Options Controlling Polygon Rasterization
GLvoid WINAPI glPolygonMode(GLenum face, GLenum mode);

// Section 3.5.5
// Depth Offset
GLvoid WINAPI glPolygonOffset(GLfloat factor, GLfloat units);

// Section 3.6.1
// Pixel Storage Modes
GLvoid WINAPI glPixelStorei(GLenum pname, GLint param);
GLvoid WINAPI glPixelStoref(GLenum pname, GLfloat param);

// Section 3.6.3
// Pixel Transfer Modes
GLvoid WINAPI glPixelTransferi(GLenum param, GLint value);
GLvoid WINAPI glPixelTransferf(GLenum param, GLfloat value);
GLvoid WINAPI glPixelMapuiv(GLenum map, GLsizei size, const GLuint* values);
GLvoid WINAPI glPixelMapusv(GLenum map, GLsizei size, const GLushort* values);
GLvoid WINAPI glPixelMapfv(GLenum map, GLsizei size, const GLfloat* values);
GLvoid WINAPI glColorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid* data);
GLvoid WINAPI glColorTableParameteriv(GLenum target, GLenum pname, const GLint* params);
GLvoid WINAPI glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat* params);
GLvoid WINAPI glCopyColorTable(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
GLvoid WINAPI glColorSubTable(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid* data);
GLvoid WINAPI glCopyColorSubTable(GLenum target, GLsizei start, GLint x, GLint y, GLsizei count);
GLvoid WINAPI glConvolutionFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* data);
GLvoid WINAPI glConvolutionParameteriv(GLenum target, GLenum pname, const GLint* params);
GLvoid WINAPI glConvolutionParameterfv(GLenum target, GLenum pname, const GLfloat* params);
GLvoid WINAPI glConvolutionFilter1D(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid* data);
GLvoid WINAPI glSeparableFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* row, const GLvoid* column);
GLvoid WINAPI glCopyConvolutionFilter2D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height);
GLvoid WINAPI glCopyConvolutionFilter1D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width);
GLvoid WINAPI glHistogram(GLenum target, GLsizei width, GLenum internalformat, GLboolean sink);
GLvoid WINAPI glMinmax(GLenum target, GLenum internalformat, GLboolean sink);

// Section 3.6.4
// Rasterization of Pixel Rectangles
GLvoid WINAPI glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* data);
GLvoid WINAPI glPixelZoom(GLfloat zx, GLfloat zy);

// Section 3.6.5
// Pixel Transfer Operations
GLvoid WINAPI glConvolutionParameteri(GLenum target, GLenum pname, GLint param);
GLvoid WINAPI glConvolutionParameterf(GLenum target, GLenum pname, GLfloat param);

// Section 3.7
// Bitmaps
GLvoid WINAPI glBitmap(GLsizei w, GLsizei h, GLfloat xbo, GLfloat ybo, GLfloat xbi, GLfloat ybi, const GLubyte* data);

// Section 3.8.1
// Texture Image Specification
GLvoid WINAPI glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* data);
GLvoid WINAPI glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data);
GLvoid WINAPI glTexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* data);

// Section 3.8.2
// Alternate Texture Image Specification Commands
GLvoid WINAPI glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
GLvoid WINAPI glCopyTexImage1D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
GLvoid WINAPI glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLvoid* data);
GLvoid WINAPI glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* data);
GLvoid WINAPI glTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid* data);
GLvoid WINAPI glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GLvoid WINAPI glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GLvoid WINAPI glCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);

// Section 3.8.3
// Texture Parameters
GLvoid WINAPI glTexParameteri(GLenum target, GLenum pname, GLint param);
GLvoid WINAPI glTexParameterf(GLenum target, GLenum pname, GLfloat param);
GLvoid WINAPI glTexParameteriv(GLenum target, GLenum pname, const GLint* params);
GLvoid WINAPI glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params);

// Section 3.8.8
// Texture Objects
GLvoid WINAPI glBindTexture(GLenum target, GLuint texture);
GLvoid WINAPI glDeleteTextures(GLsizei n, const GLuint* textures);
GLvoid WINAPI glGenTextures(GLsizei n, GLuint* textures);
GLboolean WINAPI glAreTexturesResident(GLsizei n, const GLuint* textures, GLboolean* residences);
GLvoid WINAPI glPrioritizeTextures(GLsizei n, const GLuint* textures, const GLclampf* priorities);

// Section 3.8.9
// Texture Environments and Texture Functions
GLvoid WINAPI glTexEnvi(GLenum target, GLenum pname, GLint param);
GLvoid WINAPI glTexEnvf(GLenum target, GLenum pname, GLfloat param);
GLvoid WINAPI glTexEnviv(GLenum target, GLenum pname, const GLint* params);
GLvoid WINAPI glTexEnvfv(GLenum target, GLenum pname, const GLfloat* params);

// Section 3.10
// Fog
GLvoid WINAPI glFogi(GLenum pname, GLint param);
GLvoid WINAPI glFogf(GLenum pname, GLfloat param);
GLvoid WINAPI glFogiv(GLenum pname, const GLint* params);
GLvoid WINAPI glFogfv(GLenum pname, const GLfloat* params);

// Section 4.1.2
// Scissor Test
GLvoid WINAPI glScissor(GLint left, GLint bottom, GLsizei width, GLsizei height);

// Section 4.1.3
// Alpha Test
GLvoid WINAPI glAlphaFunc(GLenum func, GLclampf ref);

// Section 4.1.4
// Stencil Test
GLvoid WINAPI glStencilFunc(GLenum func, GLint ref, GLuint mask);
GLvoid WINAPI glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass);

// Section 4.1.5
// Depth Buffer Test
GLvoid WINAPI glDepthFunc(GLenum func);

// Section 4.1.6
// Blending
GLvoid WINAPI glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
GLvoid WINAPI glBlendEquation(GLenum mode);
GLvoid WINAPI glBlendFunc(GLenum src, GLenum dst);

// Section 4.1.8
// Logical Operation
GLvoid WINAPI glLogicOp(GLenum op);

// Section 4.2.1
// Selecting a Buffer for Writing
GLvoid WINAPI glDrawBuffer(GLenum buf);

// Section 4.2.2
// Fine Control of Buffer Updates
GLvoid WINAPI glIndexMask(GLuint mask);
GLvoid WINAPI glColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a);
GLvoid WINAPI glDepthMask(GLboolean mask);
GLvoid WINAPI glStencilMask(GLuint mask);

// Section 4.2.3
// Clearing the Buffers
GLvoid WINAPI glClear(GLbitfield buf);
GLvoid WINAPI glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
GLvoid WINAPI glClearIndex(GLfloat index);
GLvoid WINAPI glClearDepth(GLclampd d);
GLvoid WINAPI glClearStencil(GLint s);
GLvoid WINAPI glClearAccum(GLfloat r, GLfloat g, GLfloat b, GLfloat a);

// Section 4.2.4
// The Accumulation Buffer
GLvoid WINAPI glAccum(GLenum op, GLfloat value);

// Section 4.3.2
// Reading Pixels
GLvoid WINAPI glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* data);
GLvoid WINAPI glReadBuffer(GLenum src);

// Section 4.3.3
// Copying Pixels
GLvoid WINAPI glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);

// Section 5.1
// Evaluators
GLvoid WINAPI glMap1f(GLenum type, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points);
GLvoid WINAPI glMap1d(GLenum type, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble* points);
GLvoid WINAPI glMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points);
GLvoid WINAPI glMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble* points);
GLvoid WINAPI glEvalCoord1f(GLfloat u);
GLvoid WINAPI glEvalCoord1d(GLdouble u);
GLvoid WINAPI glEvalCoord2f(GLfloat u, GLfloat v);
GLvoid WINAPI glEvalCoord2d(GLdouble u, GLdouble v);
GLvoid WINAPI glEvalCoord1fv(const GLfloat* arg);
GLvoid WINAPI glEvalCoord1dv(const GLdouble* arg);
GLvoid WINAPI glEvalCoord2fv(const GLfloat* arg);
GLvoid WINAPI glEvalCoord2dv(const GLdouble* arg);
GLvoid WINAPI glMapGrid1f(GLint n, GLfloat u1, GLfloat u2);
GLvoid WINAPI glMapGrid1d(GLint n, GLdouble u1, GLdouble u2);
GLvoid WINAPI glMapGrid2f(GLint nu, GLfloat u1, GLfloat u2, GLint nv, GLfloat v1, GLfloat v2);
GLvoid WINAPI glMapGrid2d(GLint nu, GLdouble u1, GLdouble u2, GLint nv, GLdouble v1, GLdouble v2);
GLvoid WINAPI glEvalMesh1(GLenum mode, GLint p1, GLint p2);
GLvoid WINAPI glEvalMesh2(GLenum mode, GLint p1, GLint p2, GLint q1, GLint q2);
GLvoid WINAPI glEvalPoint1(GLint p);
GLvoid WINAPI glEvalPoint2(GLint p, GLint q);

// Section 5.2
// Selection
GLvoid WINAPI glInitNames(GLvoid);
GLvoid WINAPI glPopName(GLvoid);
GLvoid WINAPI glPushName(GLuint name);
GLvoid WINAPI glLoadName(GLuint name);
GLint WINAPI glRenderMode(GLenum mode);
GLvoid WINAPI glSelectBuffer(GLsizei n, GLuint* buffer);

// Section 5.3
// Feedback
GLvoid WINAPI glFeedbackBuffer(GLsizei n, GLenum type, GLfloat* buffer);
GLvoid WINAPI glPassThrough(GLfloat token);

// Section 5.4
// Display Lists
GLvoid WINAPI glNewList(GLuint n, GLenum mode);
GLvoid WINAPI glEndList(GLvoid);
GLvoid WINAPI glCallList(GLuint n);
GLvoid WINAPI glCallLists(GLsizei n, GLenum type, const GLvoid* lists);
GLvoid WINAPI glListBase(GLuint base);
GLuint WINAPI glGenLists(GLsizei s);
GLboolean WINAPI glIsList(GLuint list);
GLvoid WINAPI glDeleteLists(GLuint list, GLsizei range);

// Section 5.5
// Flush and Finish
GLvoid WINAPI glFlush(GLvoid);
GLvoid WINAPI glFinish(GLvoid);

// Section 5.6
// Hints
GLvoid WINAPI glHint(GLenum target, GLenum hint);

// Section 6.1.1
// Simple Queries
GLvoid WINAPI glGetBooleanv(GLenum value, GLboolean* data);
GLvoid WINAPI glGetIntegerv(GLenum value, GLint* data);
GLvoid WINAPI glGetFloatv(GLenum value, GLfloat* data);
GLvoid WINAPI glGetDoublev(GLenum value, GLdouble* data);
GLboolean WINAPI glIsEnabled(GLenum value);

// Section 6.1.3
// Enumerated Queries
GLvoid WINAPI glGetClipPlane(GLenum plane, GLdouble eqn[4]);
GLvoid WINAPI glGetLightiv(GLenum light, GLenum value, GLint* data);
GLvoid WINAPI glGetLightfv(GLenum light, GLenum value, GLfloat* data);
GLvoid WINAPI glGetMaterialiv(GLenum face, GLenum value, GLint* data);
GLvoid WINAPI glGetMaterialfv(GLenum face, GLenum value, GLfloat* data);
GLvoid WINAPI glGetTexEnviv(GLenum env, GLenum value, GLint* data);
GLvoid WINAPI glGetTexEnvfv(GLenum env, GLenum value, GLfloat* data);
GLvoid WINAPI glGetTexGeniv(GLenum coord, GLenum value, GLint* data);
GLvoid WINAPI glGetTexGenfv(GLenum coord, GLenum value, GLfloat* data);
GLvoid WINAPI glGetTexParameteriv(GLenum target, GLenum value, GLint* data);
GLvoid WINAPI glGetTexParameterfv(GLenum target, GLenum value, GLfloat* data);
GLvoid WINAPI glGetTexLevelParameteriv(GLenum target, GLint lod, GLenum value, GLint* data);
GLvoid WINAPI glGetTexLevelParameterfv(GLenum target, GLint lod, GLenum value, GLfloat* data);
GLvoid WINAPI glGetPixelMapuiv(GLenum map, GLuint* data);
GLvoid WINAPI glGetPixelMapusv(GLenum map, GLushort* data);
GLvoid WINAPI glGetPixelMapfv(GLenum map, GLfloat* data);
GLvoid WINAPI glGetMapiv(GLenum map, GLenum value, GLint* data);
GLvoid WINAPI glGetMapfv(GLenum map, GLenum value, GLfloat* data);
GLvoid WINAPI glGetMapdv(GLenum map, GLenum value, GLdouble* data);

// Section 6.1.4
// Texture Queries
GLvoid WINAPI glGetTexImage(GLenum tex, GLint lod, GLenum format, GLenum type, GLvoid* img);
GLboolean WINAPI glIsTexture(GLuint texture);

// Section 6.1.5
// Stipple Query
GLvoid WINAPI glGetPolygonStipple(GLvoid* pattern);

// Section 6.1.7
// Color Table Query
GLvoid WINAPI glGetColorTable(GLenum target, GLenum format, GLenum type, GLvoid* table);
GLvoid WINAPI glGetColorTableParameteriv(GLenum target, GLenum pname, GLint* params);
GLvoid WINAPI glGetColorTableParameterfv(GLenum target, GLenum pname, GLfloat* params);

// Section 6.1.8
// Convolution Query
GLvoid WINAPI glGetConvolutionFilter(GLenum target, GLenum format, GLenum type, GLvoid* image);
GLvoid WINAPI glGetSeparableFilter(GLenum target, GLenum format, GLenum type, GLvoid* row, GLvoid* column, GLvoid* span);
GLvoid WINAPI glGetConvolutionParameteriv(GLenum target, GLenum pname, GLint* params);
GLvoid WINAPI glGetConvolutionParameterfv(GLenum target, GLenum pname, GLfloat* params);

// Section 6.1.9
// Histogram Query
GLvoid WINAPI glGetHistogram(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid* values);
GLvoid WINAPI glResetHistogram(GLenum target);
GLvoid WINAPI glGetHistogramParameteriv(GLenum target, GLenum pname, GLint* params);
GLvoid WINAPI glGetHistogramParameterfv(GLenum target, GLenum pname, GLfloat* params);

// Section 6.1.10
// Minmax Query
GLvoid WINAPI glGetMinmax(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid* values);
GLvoid WINAPI glResetMinmax(GLenum target);
GLvoid WINAPI glGetMinmaxParameteriv(GLenum target, GLenum pname, GLint* params);
GLvoid WINAPI glGetMinmaxParameterfv(GLenum target, GLenum pname, GLfloat* params);

// Section 6.1.11
// Pointer and String Queries
GLvoid WINAPI glGetPointerv(GLenum pname, GLvoid** params);
const GLubyte* WINAPI glGetString(GLenum name);

// Section 6.1.12
// Saving and Restoring State
GLvoid WINAPI glPushAttrib(GLbitfield mask);
GLvoid WINAPI glPushClientAttrib(GLbitfield mask);
GLvoid WINAPI glPopAttrib(GLvoid);
GLvoid WINAPI glPopClientAttrib(GLvoid);

// Section WGL
// Extra Extensions
HGLRC WINAPI wglCreateContextEx(HDC device);
BOOL WINAPI wglLoadExtensions(BOOL (*onfail)(LPCSTR));

// Appendix
// Constants
#define GL_2_BYTES 0x1407
#define GL_3D 0x0601
#define GL_3D_COLOR 0x0602
#define GL_3D_COLOR_TEXTURE 0x0603
#define GL_3_BYTES 0x1408
#define GL_4D_COLOR_TEXTURE 0x0604
#define GL_4_BYTES 0x1409
#define GL_ACCUM 0x0100
#define GL_ACCUM_BUFFER_BIT 0x00000200
#define GL_ACTIVE_TEXTURE 0x84E0
#define GL_ADD 0x0104
#define GL_ALL_ATTRIB_BITS 0x000FFFFF
#define GL_ALL_CLIENT_ATTRIB_BITS
#define GL_ALPHA 0x1906
#define GL_ALPHA12 0x803D
#define GL_ALPHA16 0x803E
#define GL_ALPHA4 0x803B
#define GL_ALPHA8 0x803C
#define GL_ALPHA_BIAS 0x0D1D
#define GL_ALPHA_SCALE 0x0D1C
#define GL_ALPHA_TEST 0x0BC0
#define GL_ALWAYS 0x0207
#define GL_AMBIENT 0x1200
#define GL_AMBIENT_AND_DIFFUSE 0x1602
#define GL_AND 0x1501
#define GL_AND_INVERTED 0x1504
#define GL_AND_REVERSE 0x1502
#define GL_AUTO_NORMAL 0x0D80
#define GL_AUX0 0x0409
#define GL_BACK 0x0405
#define GL_BACK_LEFT 0x0402
#define GL_BACK_RIGHT 0x0403
#define GL_BGR 0x80E0
#define GL_BGRA 0x80E1
#define GL_BITMAP 0x1A00
#define GL_BITMAP_TOKEN 0x0704
#define GL_BLEND 0x0BE2
#define GL_BLUE 0x1905
#define GL_BLUE_BIAS 0x0D1B
#define GL_BLUE_SCALE 0x0D1A
#define GL_BYTE 0x1400
#define GL_C3F_V3F 0x2A24
#define GL_C4F_N3F_V3F 0x2A26
#define GL_C4UB_V2F 0x2A22
#define GL_C4UB_V3F 0x2A23
#define GL_CCW 0x0901
#define GL_CLAMP 0x2900
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_CLEAR 0x1500
#define GL_CLIENT_ACTIVE_TEXTURE 0x84E1
#define GL_CLIENT_PIXEL_STORE_BIT 0x00000001
#define GL_CLIENT_VERTEX_ARRAY_BIT 0x00000002
#define GL_CLIP_PLANE0 0x3000
#define GL_COEFF 0x0A00
#define GL_COLOR 0x1800
#define GL_COLOR_ARRAY 0x8076
#define GL_COLOR_ARRAY_POINTER 0x8090
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_COLOR_INDEX 0x1900
#define GL_COLOR_INDEXES 0x1603
#define GL_COLOR_LOGIC_OP 0x0BF2
#define GL_COLOR_MATERIAL 0x0B57
#define GL_COLOR_MATRIX 0x80B1
#define GL_COLOR_MATRIX_STACK_DEPTH 0x80B2
#define GL_COLOR_TABLE 0x80D0
#define GL_COLOR_TABLE_ALPHA_SIZE 0x80DD
#define GL_COLOR_TABLE_BIAS 0x80D7
#define GL_COLOR_TABLE_BLUE_SIZE 0x80DC
#define GL_COLOR_TABLE_FORMAT 0x80D8
#define GL_COLOR_TABLE_GREEN_SIZE 0x80DB
#define GL_COLOR_TABLE_INTENSITY_SIZE 0x80DF
#define GL_COLOR_TABLE_LUMINANCE_SIZE 0x80DE
#define GL_COLOR_TABLE_RED_SIZE 0x80DA
#define GL_COLOR_TABLE_SCALE 0x80D6
#define GL_COLOR_TABLE_WIDTH 0x80D9
#define GL_COMPILE 0x1300
#define GL_COMPILE_AND_EXECUTE 0x1301
#define GL_CONSTANT_ALPHA 0x8003
#define GL_CONSTANT_ATTENUATION 0x1207
#define GL_CONSTANT_BORDER 0x8151
#define GL_CONSTANT_COLOR 0x8001
#define GL_CONVOLUTION_1D 0x8010
#define GL_CONVOLUTION_2D 0x8011
#define GL_CONVOLUTION_BORDER_COLOR 0x8154
#define GL_CONVOLUTION_BORDER_MODE 0x8013
#define GL_CONVOLUTION_FILTER_BIAS 0x8015
#define GL_CONVOLUTION_FILTER_SCALE 0x8014
#define GL_CONVOLUTION_FORMAT 0x8017
#define GL_CONVOLUTION_HEIGHT 0x8019
#define GL_CONVOLUTION_WIDTH 0x8018
#define GL_COPY 0x1503
#define GL_COPY_INVERTED 0x150C
#define GL_COPY_PIXEL_TOKEN 0x0706
#define GL_CULL_FACE 0x0B44
#define GL_CURRENT_BIT 0x00000001
#define GL_CURRENT_RASTER_TEXTURE_COORDS 0x0B06
#define GL_CURRENT_TEXTURE_COORDS 0x0B03
#define GL_CW 0x0900
#define GL_DECAL 0x2101
#define GL_DECR 0x1E03
#define GL_DEPTH 0x1801
#define GL_DEPTH_BIAS 0x0D1F
#define GL_DEPTH_BUFFER_BIT 0x00000100
#define GL_DEPTH_COMPONENT 0x1902
#define GL_DEPTH_SCALE 0x0D1E
#define GL_DEPTH_TEST 0x0B71
#define GL_DIFFUSE 0x1201
#define GL_DITHER 0x0BD0
#define GL_DOMAIN 0x0A02
#define GL_DONT_CARE 0x1100
#define GL_DOUBLE 0x140A
#define GL_DRAW_PIXEL_TOKEN 0x0705
#define GL_DST_ALPHA 0x0304
#define GL_DST_COLOR 0x0306
#define GL_EDGE_FLAG_ARRAY 0x8079
#define GL_EDGE_FLAG_ARRAY_POINTER 0x8093
#define GL_EMISSION 0x1600
#define GL_ENABLE_BIT 0x00002000
#define GL_EQUAL 0x0202
#define GL_EQUIV 0x1509
#define GL_EVAL_BIT 0x00010000
#define GL_EXP 0x0800
#define GL_EXP2 0x0801
#define GL_EXTENSIONS 0x1F03
#define GL_EYE_LINEAR 0x2400
#define GL_EYE_PLANE 0x2502
#define GL_FALSE 0
#define GL_FASTEST 0x1101
#define GL_FEEDBACK 0x1C01
#define GL_FEEDBACK_BUFFER_POINTER 0x0DF0
#define GL_FILL 0x1B02
#define GL_FLAT 0x1D00
#define GL_FLOAT 0x1406
#define GL_FOG 0x0B60
#define GL_FOG_BIT 0x00000080
#define GL_FOG_COLOR 0x0B66
#define GL_FOG_DENSITY 0x0B62
#define GL_FOG_END 0x0B64
#define GL_FOG_HINT 0x0C54
#define GL_FOG_INDEX 0x0B61
#define GL_FOG_MODE 0x0B65
#define GL_FOG_START 0x0B63
#define GL_FRONT 0x0404
#define GL_FRONT_AND_BACK 0x0408
#define GL_FRONT_LEFT 0x0400
#define GL_FRONT_RIGHT 0x0401
#define GL_FUNC_ADD 0x8006
#define GL_FUNC_REVERSE_SUBTRACT 0x800B
#define GL_FUNC_SUBTRACT 0x800A
#define GL_GEQUAL 0x0206
#define GL_GREATER 0x0204
#define GL_GREEN 0x1904
#define GL_GREEN_BIAS 0x0D19
#define GL_GREEN_SCALE 0x0D18
#define GL_HINT_BIT 0x00008000
#define GL_HISTOGRAM 0x8024
#define GL_HISTOGRAM_ALPHA_SIZE 0x802B
#define GL_HISTOGRAM_BLUE_SIZE 0x802A
#define GL_HISTOGRAM_FORMAT 0x8027
#define GL_HISTOGRAM_GREEN_SIZE 0x8029
#define GL_HISTOGRAM_LUMINANCE_SIZE 0x802C
#define GL_HISTOGRAM_RED_SIZE 0x8028
#define GL_HISTOGRAM_SINK 0x802D
#define GL_HISTOGRAM_WIDTH 0x8026
#define GL_INCR 0x1E02
#define GL_INDEX_ARRAY 0x8077
#define GL_INDEX_ARRAY_POINTER 0x8091
#define GL_INDEX_LOGIC_OP 0x0BF1
#define GL_INDEX_OFFSET 0x0D13
#define GL_INDEX_SHIFT 0x0D12
#define GL_INT 0x1404
#define GL_INTENSITY 0x8049
#define GL_INTENSITY12 0x804C
#define GL_INTENSITY16 0x804D
#define GL_INTENSITY4 0x804A
#define GL_INTENSITY8 0x804B
#define GL_INVALID_ENUM 0x0500
#define GL_INVALID_OPERATION 0x0502
#define GL_INVALID_VALUE 0x0501
#define GL_INVERT 0x150A
#define GL_KEEP 0x1E00
#define GL_LEFT 0x0406
#define GL_LEQUAL 0x0203
#define GL_LESS 0x0201
#define GL_LIGHT0 0x4000
#define GL_LIGHT_MODEL_AMBIENT 0x0B53
#define GL_LIGHT_MODEL_COLOR_CONTROL
#define GL_LIGHT_MODEL_LOCAL_VIEWER 0x0B51
#define GL_LIGHT_MODEL_TWO_SIDE 0x0B52
#define GL_LIGHTING 0x0B50
#define GL_LIGHTING_BIT 0x00000040
#define GL_LINE 0x1B01
#define GL_LINE_BIT 0x00000004
#define GL_LINE_LOOP 0x0002
#define GL_LINE_RESET_TOKEN 0x0707
#define GL_LINE_SMOOTH 0x0B20
#define GL_LINE_SMOOTH_HINT 0x0C52
#define GL_LINE_STIPPLE 0x0B24
#define GL_LINE_STRIP 0x0003
#define GL_LINE_TOKEN 0x0702
#define GL_LINEAR 0x2601
#define GL_LINEAR_ATTENUATION 0x1208
#define GL_LINEAR_MIPMAP_LINEAR 0x2703
#define GL_LINEAR_MIPMAP_NEAREST 0x2701
#define GL_LINES 0x0001
#define GL_LIST_BIT 0x00020000
#define GL_LOAD 0x0101
#define GL_LOGIC_OP 0x0BF1
#define GL_LUMINANCE 0x1909
#define GL_LUMINANCE12 0x8041
#define GL_LUMINANCE12_ALPHA12 0x8047
#define GL_LUMINANCE12_ALPHA4 0x8046
#define GL_LUMINANCE16 0x8042
#define GL_LUMINANCE16_ALPHA16 0x8048
#define GL_LUMINANCE4 0x803F
#define GL_LUMINANCE4_ALPHA4 0x8043
#define GL_LUMINANCE6_ALPHA2 0x8044
#define GL_LUMINANCE8 0x8040
#define GL_LUMINANCE8_ALPHA8 0x8045
#define GL_LUMINANCE_ALPHA 0x190A
#define GL_MAP1_COLOR_4 0x0D90
#define GL_MAP1_INDEX 0x0D91
#define GL_MAP1_NORMAL 0x0D92
#define GL_MAP1_TEXTURE_COORD_1 0x0D93
#define GL_MAP1_TEXTURE_COORD_2 0x0D94
#define GL_MAP1_TEXTURE_COORD_3 0x0D95
#define GL_MAP1_TEXTURE_COORD_4 0x0D96
#define GL_MAP1_VERTEX_3 0x0D97
#define GL_MAP1_VERTEX_4 0x0D98
#define GL_MAP2_VERTEX_3 0x0DB7
#define GL_MAP2_VERTEX_4 0x0DB8
#define GL_MAP_COLOR 0x0D10
#define GL_MAP_STENCIL 0x0D11
#define GL_MAX 0x8008
#define GL_MAX_3D_TEXTURE_SIZE 0x8073
#define GL_MAX_ATTRIB_STACK_DEPTH 0x0D35
#define GL_MAX_CLIENT_ATTRIB_STACK_DEPTH 0x0D3B
#define GL_MAX_COLOR_MATRIX_STACK_DEPTH 0x80B3
#define GL_MAX_CONVOLUTION_HEIGHT 0x801B
#define GL_MAX_CONVOLUTION_WIDTH 0x801A
#define GL_MAX_ELEMENTS_INDICES 0x80E9
#define GL_MAX_ELEMENTS_VERTICES 0x80E8
#define GL_MAX_EVAL_ORDER 0x0D30
#define GL_MAX_PIXEL_MAP_TABLE 0x0D34
#define GL_MAX_TEXTURE_SIZE 0x0D33
#define GL_MAX_TEXTURE_UNITS 0x84E2
#define GL_MIN 0x8007
#define GL_MINMAX 0x802E
#define GL_MINMAX_FORMAT 0x802F
#define GL_MINMAX_SINK 0x8030
#define GL_MODELVIEW 0x1700
#define GL_MODULATE 0x2100
#define GL_MULT 0x0103
#define GL_N3F_V3F 0x2A25
#define GL_NAND 0x150E
#define GL_NEAREST 0x2600
#define GL_NEAREST_MIPMAP_LINEAR 0x2702
#define GL_NEAREST_MIPMAP_NEAREST 0x2700
#define GL_NEVER 0x0200
#define GL_NICEST 0x1102
#define GL_NO_ERROR 0x0000
#define GL_NONE 0x0000
#define GL_NOOP 0x1505
#define GL_NOR 0x1508
#define GL_NORMAL_ARRAY 0x8075
#define GL_NORMAL_ARRAY_POINTER 0x808F
#define GL_NORMALIZE 0x0BA1
#define GL_NOTEQUAL 0x0205
#define GL_OBJECT_LINEAR 0x2401
#define GL_OBJECT_PLANE 0x2501
#define GL_ONE 1
#define GL_ONE_MINUS_CONSTANT_ALPHA 0x8004
#define GL_ONE_MINUS_CONSTANT_COLOR 0x8002
#define GL_ONE_MINUS_DST_ALPHA 0x0305
#define GL_ONE_MINUS_DST_COLOR 0x0307
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_ONE_MINUS_SRC_COLOR 0x0301
#define GL_OR 0x1507
#define GL_OR_INVERTED 0x150D
#define GL_OR_REVERSE 0x150B
#define GL_ORDER 0x0A01
#define GL_OUT_OF_MEMORY 0x0505
#define GL_PACK_ALIGNMENT 0x0D05
#define GL_PACK_IMAGE_HEIGHT 0x806C
#define GL_PACK_LSB_FIRST 0x0D01
#define GL_PACK_ROW_LENGTH 0x0D02
#define GL_PACK_SKIP_IMAGES 0x806B
#define GL_PACK_SKIP_PIXELS 0x0D04
#define GL_PACK_SKIP_ROWS 0x0D03
#define GL_PACK_SWAP_BYTES 0x0D00
#define GL_PASS_THROUGH_TOKEN 0x0700
#define GL_PERSPECTIVE_CORRECTION_HINT 0x0C50
#define GL_PIXEL_MAP_A_TO_A 0x0C79
#define GL_PIXEL_MAP_B_TO_B 0x0C78
#define GL_PIXEL_MAP_G_TO_G 0x0C77
#define GL_PIXEL_MAP_I_TO_A 0x0C75
#define GL_PIXEL_MAP_I_TO_B 0x0C74
#define GL_PIXEL_MAP_I_TO_G 0x0C73
#define GL_PIXEL_MAP_I_TO_I 0x0C70
#define GL_PIXEL_MAP_I_TO_R 0x0C72
#define GL_PIXEL_MAP_R_TO_R 0x0C76
#define GL_PIXEL_MAP_S_TO_S 0x0C71
#define GL_PIXEL_MODE_BIT 0x00000020
#define GL_POINT 0x1B00
#define GL_POINT_BIT 0x00000002
#define GL_POINT_SMOOTH 0x0B10
#define GL_POINT_SMOOTH_HINT 0x0C51
#define GL_POINT_TOKEN 0x0701
#define GL_POINTS 0x0000
#define GL_POLYGON 0x0009
#define GL_POLYGON_BIT 0x00000008
#define GL_POLYGON_OFFSET_FILL 0x8037
#define GL_POLYGON_OFFSET_LINE 0x2A02
#define GL_POLYGON_OFFSET_POINT 0x2A01
#define GL_POLYGON_SMOOTH 0x0B41
#define GL_POLYGON_SMOOTH_HINT 0x0C53
#define GL_POLYGON_STIPPLE 0x0B42
#define GL_POLYGON_STIPPLE_BIT 0x00000010
#define GL_POLYGON_TOKEN 0x0703
#define GL_POSITION 0x1203
#define GL_POST_COLOR_MATRIX_ALPHA_BIAS 0x80BB
#define GL_POST_COLOR_MATRIX_ALPHA_SCALE 0x80B7
#define GL_POST_COLOR_MATRIX_BLUE_BIAS 0x80BA
#define GL_POST_COLOR_MATRIX_BLUE_SCALE 0x80B6
#define GL_POST_COLOR_MATRIX_COLOR_TABLE 0x80D2
#define GL_POST_COLOR_MATRIX_GREEN_BIAS 0x80B9
#define GL_POST_COLOR_MATRIX_GREEN_SCALE 0x80B5
#define GL_POST_COLOR_MATRIX_RED_BIAS 0x80B8
#define GL_POST_COLOR_MATRIX_RED_SCALE 0x80B4
#define GL_POST_CONVOLUTION_ALPHA_BIAS 0x8023
#define GL_POST_CONVOLUTION_ALPHA_SCALE 0x801F
#define GL_POST_CONVOLUTION_BLUE_BIAS 0x8022
#define GL_POST_CONVOLUTION_BLUE_SCALE 0x801E
#define GL_POST_CONVOLUTION_COLOR_TABLE 0x80D1
#define GL_POST_CONVOLUTION_GREEN_BIAS 0x8021
#define GL_POST_CONVOLUTION_GREEN_SCALE 0x801D
#define GL_POST_CONVOLUTION_RED_BIAS 0x8020
#define GL_POST_CONVOLUTION_RED_SCALE 0x801C
#define GL_PROJECTION 0x1701
#define GL_PROXY_COLOR_TABLE 0x80D3
#define GL_PROXY_HISTOGRAM 0x8025
#define GL_PROXY_POST_COLOR_MATRIX_COLOR_TABLE 0x80D5
#define GL_PROXY_POST_CONVOLUTION_COLOR_TABLE 0x80D4
#define GL_PROXY_TEXTURE_1D 0x8063
#define GL_PROXY_TEXTURE_2D 0x8064
#define GL_PROXY_TEXTURE_3D 0x8070
#define GL_Q 0x2003
#define GL_QUAD_STRIP 0x0008
#define GL_QUADRATIC_ATTENUATION 0x1209
#define GL_QUADS 0x0007
#define GL_R 0x2002
#define GL_R3_G3_B2 0x2A10
#define GL_RED 0x1903
#define GL_RED_BIAS 0x0D15
#define GL_RED_SCALE 0x0D14
#define GL_REDUCE 0x8016
#define GL_RENDER 0x1C00
#define GL_RENDERER 0x1F01
#define GL_REPEAT 0x2901
#define GL_REPLACE 0x1E01
#define GL_REPLICATE_BORDER 0x8153
#define GL_RESCALE_NORMAL 0x803A
#define GL_RETURN 0x0102
#define GL_RGB 0x1907
#define GL_RGB10 0x8052
#define GL_RGB10_A2 0x8059
#define GL_RGB12 0x8053
#define GL_RGB16 0x8054
#define GL_RGB4 0x804F
#define GL_RGB5 0x8050
#define GL_RGB5_A1 0x8057
#define GL_RGB8 0x8051
#define GL_RGBA 0x1908
#define GL_RGBA12 0x805A
#define GL_RGBA16 0x805B
#define GL_RGBA2 0x8055
#define GL_RGBA4 0x8056
#define GL_RGBA8 0x8058
#define GL_RIGHT 0x0407
#define GL_S 0x2000
#define GL_SCISSOR_BIT 0x00080000
#define GL_SCISSOR_TEST 0x0C11
#define GL_SELECT 0x1C02
#define GL_SELECTION_BUFFER_POINTER 0x0DF3
#define GL_SEPARABLE_2D 0x8012
#define GL_SEPARATE_SPECULAR_COLOR
#define GL_SET 0x150F
#define GL_SHININESS 0x1601
#define GL_SHORT 0x1402
#define GL_SINGLE_COLOR
#define GL_SMOOTH 0x1D01
#define GL_SPECULAR 0x1202
#define GL_SPHERE_MAP 0x2402
#define GL_SPOT_CUTOFF 0x1206
#define GL_SPOT_DIRECTION 0x1204
#define GL_SPOT_EXPONENT 0x1205
#define GL_SRC_ALPHA 0x0302
#define GL_SRC_ALPHA_SATURATE 0x0308
#define GL_SRC_COLOR 0x0300
#define GL_STACK_OVERFLOW 0x0503
#define GL_STACK_UNDERFLOW 0x0504
#define GL_STENCIL 0x1802
#define GL_STENCIL_BUFFER_BIT 0x00000400
#define GL_STENCIL_INDEX 0x1901
#define GL_STENCIL_TEST 0x0B90
#define GL_T 0x2001
#define GL_T2F_C3F_V3F 0x2A2A
#define GL_T2F_C4F_N3F_V3F 0x2A2C
#define GL_T2F_C4UB_V3F 0x2A29
#define GL_T2F_N3F_V3F 0x2A2B
#define GL_T2F_V3F 0x2A27
#define GL_T4F_C4F_N3F_V4F 0x2A2D
#define GL_T4F_V4F 0x2A28
#define GL_TABLE_TOO_LARGE 0x8031
#define GL_TEXTURE 0x1702
#define GL_TEXTURE0 0x84C0
#define GL_TEXTURE1 0x84C1
#define GL_TEXTURE2 0x84C2
#define GL_TEXTURE3 0x84C3
#define GL_TEXTURE4 0x84C4
#define GL_TEXTURE5 0x84C5
#define GL_TEXTURE6 0x84C6
#define GL_TEXTURE7 0x84C7
#define GL_TEXTURE8 0x84C8
#define GL_TEXTURE9 0x84C9
#define GL_TEXTURE10 0x84CA
#define GL_TEXTURE11 0x84CB
#define GL_TEXTURE12 0x84CC
#define GL_TEXTURE13 0x84CD
#define GL_TEXTURE14 0x84CE
#define GL_TEXTURE15 0x84CF
#define GL_TEXTURE16 0x84D0
#define GL_TEXTURE17 0x84D1
#define GL_TEXTURE18 0x84D2
#define GL_TEXTURE19 0x84D3
#define GL_TEXTURE20 0x84D4
#define GL_TEXTURE21 0x84D5
#define GL_TEXTURE22 0x84D6
#define GL_TEXTURE23 0x84D7
#define GL_TEXTURE24 0x84D8
#define GL_TEXTURE25 0x84D9
#define GL_TEXTURE26 0x84DA
#define GL_TEXTURE27 0x84DB
#define GL_TEXTURE28 0x84DC
#define GL_TEXTURE29 0x84DD
#define GL_TEXTURE30 0x84DE
#define GL_TEXTURE31 0x84DF
#define GL_TEXTURE_1D 0x0DE0
#define GL_TEXTURE_2D 0x0DE1
#define GL_TEXTURE_3D 0x806F
#define GL_TEXTURE_ALPHA_SIZE 0x805F
#define GL_TEXTURE_BASE_LEVEL 0x813C
#define GL_TEXTURE_BIT 0x00040000
#define GL_TEXTURE_BLUE_SIZE 0x805E
#define GL_TEXTURE_BORDER 0x1005
#define GL_TEXTURE_BORDER_COLOR 0x1004
#define GL_TEXTURE_COMPONENTS 0x1003
#define GL_TEXTURE_COORD_ARRAY 0x8078
#define GL_TEXTURE_COORD_ARRAY_POINTER 0x8092
#define GL_TEXTURE_DEPTH 0x8071
#define GL_TEXTURE_ENV 0x2300
#define GL_TEXTURE_ENV_COLOR 0x2201
#define GL_TEXTURE_ENV_MODE 0x2200
#define GL_TEXTURE_GEN_MODE 0x2500
#define GL_TEXTURE_GEN_Q 0x0C63
#define GL_TEXTURE_GEN_R 0x0C62
#define GL_TEXTURE_GEN_S 0x0C60
#define GL_TEXTURE_GEN_T 0x0C61
#define GL_TEXTURE_GREEN_SIZE 0x805D
#define GL_TEXTURE_HEIGHT 0x1001
#define GL_TEXTURE_INTENSITY_SIZE 0x8061
#define GL_TEXTURE_INTERNAL_FORMAT 0x1003
#define GL_TEXTURE_LUMINANCE_SIZE 0x8060
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MAX_LEVEL 0x813D
#define GL_TEXTURE_MAX_LOD 0x813B
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MIN_LOD 0x813A
#define GL_TEXTURE_PRIORITY 0x8066
#define GL_TEXTURE_RED_SIZE 0x805C
#define GL_TEXTURE_RESIDENT 0x8067
#define GL_TEXTURE_WIDTH 0x1000
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_TEXTURE_WRAP_S 0x2802
#define GL_TEXTURE_WRAP_T 0x2803
#define GL_TRANSFORM_BIT 0x00001000
#define GL_TRIANGLE_FAN 0x0006
#define GL_TRIANGLE_STRIP 0x0005
#define GL_TRIANGLES 0x0004
#define GL_TRUE 1
#define GL_UNPACK_ALIGNMENT 0x0CF5
#define GL_UNPACK_IMAGE_HEIGHT 0x806E
#define GL_UNPACK_LSB_FIRST 0x0CF1
#define GL_UNPACK_ROW_LENGTH 0x0CF2
#define GL_UNPACK_SKIP_IMAGES 0x806D
#define GL_UNPACK_SKIP_PIXELS 0x0CF4
#define GL_UNPACK_SKIP_ROWS 0x0CF3
#define GL_UNPACK_SWAP_BYTES 0x0CF0
#define GL_UNSIGNED_BYTE 0x1401
#define GL_UNSIGNED_BYTE_2_3_3_REV 0x8362
#define GL_UNSIGNED_BYTE_3_3_2 0x8032
#define GL_UNSIGNED_INT 0x1405
#define GL_UNSIGNED_INT_10_10_10_2 0x8036
#define GL_UNSIGNED_INT_2_10_10_10_REV 0x8368
#define GL_UNSIGNED_INT_8_8_8_8 0x8035
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#define GL_UNSIGNED_SHORT 0x1403
#define GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8366
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#define GL_UNSIGNED_SHORT_4_4_4_4_REV 0x8365
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV 0x8364
#define GL_VENDOR 0x1F00
#define GL_VERSION 0x1F02
#define GL_VERTEX_ARRAY 0x8074
#define GL_VERTEX_ARRAY_POINTER 0x808E
#define GL_VIEWPORT_BIT 0x00000800
#define GL_XOR 0x1506
#define GL_ZERO 0

#endif
