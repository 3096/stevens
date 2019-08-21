#include "glext.h"

static HMODULE opengl32 = 0;
static BOOL loaded = FALSE;
static char errbuff[1024];

// Section 2.5
// GL Errors

typedef GLenum (WINAPI *LPGLGETERROR)(GLvoid);
static LPGLGETERROR lpglGetError;
GLenum WINAPI glGetError(GLvoid)
{
 if(lpglGetError) return (*lpglGetError)();
 return (GLenum)0;
}

// Section 2.6.1
// Begin and End Objects

typedef GLvoid (WINAPI *LPGLBEGIN)(GLenum);
static LPGLBEGIN lpglBegin;
GLvoid WINAPI glBegin(GLenum mode)
{
 if(lpglBegin) (*lpglBegin)(mode);
}

typedef GLvoid (WINAPI *LPGLEND)(GLvoid);
static LPGLEND lpglEnd;
GLvoid WINAPI glEnd(GLvoid)
{
 if(lpglEnd) (*lpglEnd)();
}

// Section 2.6.2
// Polygon Edges

typedef GLvoid (WINAPI *LPGLEDGEFLAG)(GLboolean);
static LPGLEDGEFLAG lpglEdgeFlag;
GLvoid WINAPI glEdgeFlag(GLboolean flag)
{
 if(lpglEdgeFlag) (*lpglEdgeFlag)(flag);
}

typedef GLvoid (WINAPI *LPGLEDGEFLAGV)(GLboolean*);
static LPGLEDGEFLAGV lpglEdgeFlagv;
GLvoid WINAPI glEdgeFlagv(GLboolean* flag)
{
 if(lpglEdgeFlagv) (*lpglEdgeFlagv)(flag);
}

// Section 2.7
// Vertex Specification

typedef GLvoid (WINAPI *LPGLVERTEX2S)(GLshort, GLshort);
static LPGLVERTEX2S lpglVertex2s;
GLvoid WINAPI glVertex2s(GLshort x, GLshort y)
{
 if(lpglVertex2s) (*lpglVertex2s)(x, y);
}

typedef GLvoid (WINAPI *LPGLVERTEX3S)(GLshort, GLshort, GLshort);
static LPGLVERTEX3S lpglVertex3s;
GLvoid WINAPI glVertex3s(GLshort x, GLshort y, GLshort z)
{
 if(lpglVertex3s) (*lpglVertex3s)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLVERTEX4S)(GLshort, GLshort, GLshort, GLshort);
static LPGLVERTEX4S lpglVertex4s;
GLvoid WINAPI glVertex4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
 if(lpglVertex4s) (*lpglVertex4s)(x, y, z, w);
}

typedef GLvoid (WINAPI *LPGLVERTEX2I)(GLint, GLint);
static LPGLVERTEX2I lpglVertex2i;
GLvoid WINAPI glVertex2i(GLint x, GLint y)
{
 if(lpglVertex2i) (*lpglVertex2i)(x, y);
}

typedef GLvoid (WINAPI *LPGLVERTEX3I)(GLint, GLint, GLint);
static LPGLVERTEX3I lpglVertex3i;
GLvoid WINAPI glVertex3i(GLint x, GLint y, GLint z)
{
 if(lpglVertex3i) (*lpglVertex3i)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLVERTEX4I)(GLint, GLint, GLint, GLint);
static LPGLVERTEX4I lpglVertex4i;
GLvoid WINAPI glVertex4i(GLint x, GLint y, GLint z, GLint w)
{
 if(lpglVertex4i) (*lpglVertex4i)(x, y, z, w);
}

typedef GLvoid (WINAPI *LPGLVERTEX2F)(GLfloat, GLfloat);
static LPGLVERTEX2F lpglVertex2f;
GLvoid WINAPI glVertex2f(GLfloat x, GLfloat y)
{
 if(lpglVertex2f) (*lpglVertex2f)(x, y);
}

typedef GLvoid (WINAPI *LPGLVERTEX3F)(GLfloat, GLfloat, GLfloat);
static LPGLVERTEX3F lpglVertex3f;
GLvoid WINAPI glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
 if(lpglVertex3f) (*lpglVertex3f)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLVERTEX4F)(GLfloat, GLfloat, GLfloat, GLfloat);
static LPGLVERTEX4F lpglVertex4f;
GLvoid WINAPI glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
 if(lpglVertex4f) (*lpglVertex4f)(x, y, z, w);
}

typedef GLvoid (WINAPI *LPGLVERTEX2D)(GLdouble, GLdouble);
static LPGLVERTEX2D lpglVertex2d;
GLvoid WINAPI glVertex2d(GLdouble x, GLdouble y)
{
 if(lpglVertex2d) (*lpglVertex2d)(x, y);
}

typedef GLvoid (WINAPI *LPGLVERTEX3D)(GLdouble, GLdouble, GLdouble);
static LPGLVERTEX3D lpglVertex3d;
GLvoid WINAPI glVertex3d(GLdouble x, GLdouble y, GLdouble z)
{
 if(lpglVertex3d) (*lpglVertex3d)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLVERTEX4D)(GLdouble, GLdouble, GLdouble, GLdouble);
static LPGLVERTEX4D lpglVertex4d;
GLvoid WINAPI glVertex4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
 if(lpglVertex4d) (*lpglVertex4d)(x, y, z, w);
}

typedef GLvoid (WINAPI *LPGLVERTEX2SV)(const GLshort*);
static LPGLVERTEX2SV lpglVertex2sv;
GLvoid WINAPI glVertex2sv(const GLshort* coords)
{
 if(lpglVertex2sv) (*lpglVertex2sv)(coords);
}

typedef GLvoid (WINAPI *LPGLVERTEX2IV)(const GLint*);
static LPGLVERTEX2IV lpglVertex2iv;
GLvoid WINAPI glVertex2iv(const GLint* coords)
{
 if(lpglVertex2iv) (*lpglVertex2iv)(coords);
}

typedef GLvoid (WINAPI *LPGLVERTEX2FV)(const GLfloat*);
static LPGLVERTEX2FV lpglVertex2fv;
GLvoid WINAPI glVertex2fv(const GLfloat* coords)
{
 if(lpglVertex2fv) (*lpglVertex2fv)(coords);
}

typedef GLvoid (WINAPI *LPGLVERTEX2DV)(const GLdouble*);
static LPGLVERTEX2DV lpglVertex2dv;
GLvoid WINAPI glVertex2dv(const GLdouble* coords)
{
 if(lpglVertex2dv) (*lpglVertex2dv)(coords);
}

typedef GLvoid (WINAPI *LPGLVERTEX3SV)(const GLshort*);
static LPGLVERTEX3SV lpglVertex3sv;
GLvoid WINAPI glVertex3sv(const GLshort* coords)
{
 if(lpglVertex3sv) (*lpglVertex3sv)(coords);
}

typedef GLvoid (WINAPI *LPGLVERTEX3IV)(const GLint*);
static LPGLVERTEX3IV lpglVertex3iv;
GLvoid WINAPI glVertex3iv(const GLint* coords)
{
 if(lpglVertex3iv) (*lpglVertex3iv)(coords);
}

typedef GLvoid (WINAPI *LPGLVERTEX3FV)(const GLfloat*);
static LPGLVERTEX3FV lpglVertex3fv;
GLvoid WINAPI glVertex3fv(const GLfloat* coords)
{
 if(lpglVertex3fv) (*lpglVertex3fv)(coords);
}

typedef GLvoid (WINAPI *LPGLVERTEX3DV)(const GLdouble*);
static LPGLVERTEX3DV lpglVertex3dv;
GLvoid WINAPI glVertex3dv(const GLdouble* coords)
{
 if(lpglVertex3dv) (*lpglVertex3dv)(coords);
}

typedef GLvoid (WINAPI *LPGLVERTEX4SV)(const GLshort*);
static LPGLVERTEX4SV lpglVertex4sv;
GLvoid WINAPI glVertex4sv(const GLshort* coords)
{
 if(lpglVertex4sv) (*lpglVertex4sv)(coords);
}

typedef GLvoid (WINAPI *LPGLVERTEX4IV)(const GLint*);
static LPGLVERTEX4IV lpglVertex4iv;
GLvoid WINAPI glVertex4iv(const GLint* coords)
{
 if(lpglVertex4iv) (*lpglVertex4iv)(coords);
}

typedef GLvoid (WINAPI *LPGLVERTEX4FV)(const GLfloat*);
static LPGLVERTEX4FV lpglVertex4fv;
GLvoid WINAPI glVertex4fv(const GLfloat* coords)
{
 if(lpglVertex4fv) (*lpglVertex4fv)(coords);
}

typedef GLvoid (WINAPI *LPGLVERTEX4DV)(const GLdouble*);
static LPGLVERTEX4DV lpglVertex4dv;
GLvoid WINAPI glVertex4dv(const GLdouble* coords)
{
 if(lpglVertex4dv) (*lpglVertex4dv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD1S)(GLshort);
static LPGLTEXCOORD1S lpglTexCoord1s;
GLvoid WINAPI glTexCoord1s(GLshort s)
{
 if(lpglTexCoord1s) (*lpglTexCoord1s)(s);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD1I)(GLint);
static LPGLTEXCOORD1I lpglTexCoord1i;
GLvoid WINAPI glTexCoord1i(GLint s)
{
 if(lpglTexCoord1i) (*lpglTexCoord1i)(s);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD1F)(GLfloat);
static LPGLTEXCOORD1F lpglTexCoord1f;
GLvoid WINAPI glTexCoord1f(GLfloat s)
{
 if(lpglTexCoord1f) (*lpglTexCoord1f)(s);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD1D)(GLdouble);
static LPGLTEXCOORD1D lpglTexCoord1d;
GLvoid WINAPI glTexCoord1d(GLdouble s)
{
 if(lpglTexCoord1d) (*lpglTexCoord1d)(s);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD2S)(GLshort, GLshort);
static LPGLTEXCOORD2S lpglTexCoord2s;
GLvoid WINAPI glTexCoord2s(GLshort s, GLshort t)
{
 if(lpglTexCoord2s) (*lpglTexCoord2s)(s, t);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD2I)(GLint, GLint);
static LPGLTEXCOORD2I lpglTexCoord2i;
GLvoid WINAPI glTexCoord2i(GLint s, GLint t)
{
 if(lpglTexCoord2i) (*lpglTexCoord2i)(s, t);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD2F)(GLfloat, GLfloat);
static LPGLTEXCOORD2F lpglTexCoord2f;
GLvoid WINAPI glTexCoord2f(GLfloat s, GLfloat t)
{
 if(lpglTexCoord2f) (*lpglTexCoord2f)(s, t);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD2D)(GLdouble, GLdouble);
static LPGLTEXCOORD2D lpglTexCoord2d;
GLvoid WINAPI glTexCoord2d(GLdouble s, GLdouble t)
{
 if(lpglTexCoord2d) (*lpglTexCoord2d)(s, t);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD3S)(GLshort, GLshort, GLshort);
static LPGLTEXCOORD3S lpglTexCoord3s;
GLvoid WINAPI glTexCoord3s(GLshort s, GLshort t, GLshort r)
{
 if(lpglTexCoord3s) (*lpglTexCoord3s)(s, t, r);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD3I)(GLint, GLint, GLint);
static LPGLTEXCOORD3I lpglTexCoord3i;
GLvoid WINAPI glTexCoord3i(GLint s, GLint t, GLint r)
{
 if(lpglTexCoord3i) (*lpglTexCoord3i)(s, t, r);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD3F)(GLfloat, GLfloat, GLfloat);
static LPGLTEXCOORD3F lpglTexCoord3f;
GLvoid WINAPI glTexCoord3f(GLfloat s, GLfloat t, GLfloat r)
{
 if(lpglTexCoord3f) (*lpglTexCoord3f)(s, t, r);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD3D)(GLdouble, GLdouble, GLdouble);
static LPGLTEXCOORD3D lpglTexCoord3d;
GLvoid WINAPI glTexCoord3d(GLdouble s, GLdouble t, GLdouble r)
{
 if(lpglTexCoord3d) (*lpglTexCoord3d)(s, t, r);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD4S)(GLshort, GLshort, GLshort, GLshort);
static LPGLTEXCOORD4S lpglTexCoord4s;
GLvoid WINAPI glTexCoord4s(GLshort s, GLshort t, GLshort r, GLshort q)
{
 if(lpglTexCoord4s) (*lpglTexCoord4s)(s, t, r, q);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD4I)(GLint, GLint, GLint, GLint);
static LPGLTEXCOORD4I lpglTexCoord4i;
GLvoid WINAPI glTexCoord4i(GLint s, GLint t, GLint r, GLint q)
{
 if(lpglTexCoord4i) (*lpglTexCoord4i)(s, t, r, q);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD4F)(GLfloat, GLfloat, GLfloat, GLfloat);
static LPGLTEXCOORD4F lpglTexCoord4f;
GLvoid WINAPI glTexCoord4f(GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
 if(lpglTexCoord4f) (*lpglTexCoord4f)(s, t, r, q);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD4D)(GLdouble, GLdouble, GLdouble, GLdouble);
static LPGLTEXCOORD4D lpglTexCoord4d;
GLvoid WINAPI glTexCoord4d(GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
 if(lpglTexCoord4d) (*lpglTexCoord4d)(s, t, r, q);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD1SV)(const GLshort*);
static LPGLTEXCOORD1SV lpglTexCoord1sv;
GLvoid WINAPI glTexCoord1sv(const GLshort* coords)
{
 if(lpglTexCoord1sv) (*lpglTexCoord1sv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD1IV)(const GLint*);
static LPGLTEXCOORD1IV lpglTexCoord1iv;
GLvoid WINAPI glTexCoord1iv(const GLint* coords)
{
 if(lpglTexCoord1iv) (*lpglTexCoord1iv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD1FV)(const GLfloat*);
static LPGLTEXCOORD1FV lpglTexCoord1fv;
GLvoid WINAPI glTexCoord1fv(const GLfloat* coords)
{
 if(lpglTexCoord1fv) (*lpglTexCoord1fv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD1DV)(const GLdouble*);
static LPGLTEXCOORD1DV lpglTexCoord1dv;
GLvoid WINAPI glTexCoord1dv(const GLdouble* coords)
{
 if(lpglTexCoord1dv) (*lpglTexCoord1dv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD2SV)(const GLshort*);
static LPGLTEXCOORD2SV lpglTexCoord2sv;
GLvoid WINAPI glTexCoord2sv(const GLshort* coords)
{
 if(lpglTexCoord2sv) (*lpglTexCoord2sv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD2IV)(const GLint*);
static LPGLTEXCOORD2IV lpglTexCoord2iv;
GLvoid WINAPI glTexCoord2iv(const GLint* coords)
{
 if(lpglTexCoord2iv) (*lpglTexCoord2iv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD2FV)(const GLfloat*);
static LPGLTEXCOORD2FV lpglTexCoord2fv;
GLvoid WINAPI glTexCoord2fv(const GLfloat* coords)
{
 if(lpglTexCoord2fv) (*lpglTexCoord2fv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD2DV)(const GLdouble*);
static LPGLTEXCOORD2DV lpglTexCoord2dv;
GLvoid WINAPI glTexCoord2dv(const GLdouble* coords)
{
 if(lpglTexCoord2dv) (*lpglTexCoord2dv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD3SV)(const GLshort*);
static LPGLTEXCOORD3SV lpglTexCoord3sv;
GLvoid WINAPI glTexCoord3sv(const GLshort* coords)
{
 if(lpglTexCoord3sv) (*lpglTexCoord3sv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD3IV)(const GLint*);
static LPGLTEXCOORD3IV lpglTexCoord3iv;
GLvoid WINAPI glTexCoord3iv(const GLint* coords)
{
 if(lpglTexCoord3iv) (*lpglTexCoord3iv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD3FV)(const GLfloat*);
static LPGLTEXCOORD3FV lpglTexCoord3fv;
GLvoid WINAPI glTexCoord3fv(const GLfloat* coords)
{
 if(lpglTexCoord3fv) (*lpglTexCoord3fv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD3DV)(const GLdouble*);
static LPGLTEXCOORD3DV lpglTexCoord3dv;
GLvoid WINAPI glTexCoord3dv(const GLdouble* coords)
{
 if(lpglTexCoord3dv) (*lpglTexCoord3dv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD4SV)(const GLshort*);
static LPGLTEXCOORD4SV lpglTexCoord4sv;
GLvoid WINAPI glTexCoord4sv(const GLshort* coords)
{
 if(lpglTexCoord4sv) (*lpglTexCoord4sv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD4IV)(const GLint*);
static LPGLTEXCOORD4IV lpglTexCoord4iv;
GLvoid WINAPI glTexCoord4iv(const GLint* coords)
{
 if(lpglTexCoord4iv) (*lpglTexCoord4iv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD4FV)(const GLfloat*);
static LPGLTEXCOORD4FV lpglTexCoord4fv;
GLvoid WINAPI glTexCoord4fv(const GLfloat* coords)
{
 if(lpglTexCoord4fv) (*lpglTexCoord4fv)(coords);
}

typedef GLvoid (WINAPI *LPGLTEXCOORD4DV)(const GLdouble*);
static LPGLTEXCOORD4DV lpglTexCoord4dv;
GLvoid WINAPI glTexCoord4dv(const GLdouble* coords)
{
 if(lpglTexCoord4dv) (*lpglTexCoord4dv)(coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD1S)(GLenum, GLshort);
static LPGLMULTITEXCOORD1S lpglMultiTexCoord1s;
GLvoid WINAPI glMultiTexCoord1s(GLenum texture, GLshort s)
{
 if(lpglMultiTexCoord1s) (*lpglMultiTexCoord1s)(texture, s);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD1I)(GLenum, GLint);
static LPGLMULTITEXCOORD1I lpglMultiTexCoord1i;
GLvoid WINAPI glMultiTexCoord1i(GLenum texture, GLint s)
{
 if(lpglMultiTexCoord1i) (*lpglMultiTexCoord1i)(texture, s);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD1F)(GLenum, GLfloat);
static LPGLMULTITEXCOORD1F lpglMultiTexCoord1f;
GLvoid WINAPI glMultiTexCoord1f(GLenum texture, GLfloat s)
{
 if(lpglMultiTexCoord1f) (*lpglMultiTexCoord1f)(texture, s);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD1D)(GLenum, GLdouble);
static LPGLMULTITEXCOORD1D lpglMultiTexCoord1d;
GLvoid WINAPI glMultiTexCoord1d(GLenum texture, GLdouble s)
{
 if(lpglMultiTexCoord1d) (*lpglMultiTexCoord1d)(texture, s);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD2S)(GLenum, GLshort, GLshort);
static LPGLMULTITEXCOORD2S lpglMultiTexCoord2s;
GLvoid WINAPI glMultiTexCoord2s(GLenum texture, GLshort s, GLshort t)
{
 if(lpglMultiTexCoord2s) (*lpglMultiTexCoord2s)(texture, s, t);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD2I)(GLenum, GLint, GLint);
static LPGLMULTITEXCOORD2I lpglMultiTexCoord2i;
GLvoid WINAPI glMultiTexCoord2i(GLenum texture, GLint s, GLint t)
{
 if(lpglMultiTexCoord2i) (*lpglMultiTexCoord2i)(texture, s, t);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD2F)(GLenum, GLfloat, GLfloat);
static LPGLMULTITEXCOORD2F lpglMultiTexCoord2f;
GLvoid WINAPI glMultiTexCoord2f(GLenum texture, GLfloat s, GLfloat t)
{
 if(lpglMultiTexCoord2f) (*lpglMultiTexCoord2f)(texture, s, t);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD2D)(GLenum, GLdouble, GLdouble);
static LPGLMULTITEXCOORD2D lpglMultiTexCoord2d;
GLvoid WINAPI glMultiTexCoord2d(GLenum texture, GLdouble s, GLdouble t)
{
 if(lpglMultiTexCoord2d) (*lpglMultiTexCoord2d)(texture, s, t);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD3S)(GLenum, GLshort, GLshort, GLshort);
static LPGLMULTITEXCOORD3S lpglMultiTexCoord3s;
GLvoid WINAPI glMultiTexCoord3s(GLenum texture, GLshort s, GLshort t, GLshort r)
{
 if(lpglMultiTexCoord3s) (*lpglMultiTexCoord3s)(texture, s, t, r);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD3I)(GLenum, GLint, GLint, GLint);
static LPGLMULTITEXCOORD3I lpglMultiTexCoord3i;
GLvoid WINAPI glMultiTexCoord3i(GLenum texture, GLint s, GLint t, GLint r)
{
 if(lpglMultiTexCoord3i) (*lpglMultiTexCoord3i)(texture, s, t, r);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD3F)(GLenum, GLfloat, GLfloat, GLfloat);
static LPGLMULTITEXCOORD3F lpglMultiTexCoord3f;
GLvoid WINAPI glMultiTexCoord3f(GLenum texture, GLfloat s, GLfloat t, GLfloat r)
{
 if(lpglMultiTexCoord3f) (*lpglMultiTexCoord3f)(texture, s, t, r);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD3D)(GLenum, GLdouble, GLdouble, GLdouble);
static LPGLMULTITEXCOORD3D lpglMultiTexCoord3d;
GLvoid WINAPI glMultiTexCoord3d(GLenum texture, GLdouble s, GLdouble t, GLdouble r)
{
 if(lpglMultiTexCoord3d) (*lpglMultiTexCoord3d)(texture, s, t, r);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD4S)(GLenum, GLshort, GLshort, GLshort, GLshort);
static LPGLMULTITEXCOORD4S lpglMultiTexCoord4s;
GLvoid WINAPI glMultiTexCoord4s(GLenum texture, GLshort s, GLshort t, GLshort r, GLshort q)
{
 if(lpglMultiTexCoord4s) (*lpglMultiTexCoord4s)(texture, s, t, r, q);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD4I)(GLenum, GLint, GLint, GLint, GLint);
static LPGLMULTITEXCOORD4I lpglMultiTexCoord4i;
GLvoid WINAPI glMultiTexCoord4i(GLenum texture, GLint s, GLint t, GLint r, GLint q)
{
 if(lpglMultiTexCoord4i) (*lpglMultiTexCoord4i)(texture, s, t, r, q);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD4F)(GLenum, GLfloat, GLfloat, GLfloat, GLfloat);
static LPGLMULTITEXCOORD4F lpglMultiTexCoord4f;
GLvoid WINAPI glMultiTexCoord4f(GLenum texture, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
 if(lpglMultiTexCoord4f) (*lpglMultiTexCoord4f)(texture, s, t, r, q);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD4D)(GLenum, GLdouble, GLdouble, GLdouble, GLdouble);
static LPGLMULTITEXCOORD4D lpglMultiTexCoord4d;
GLvoid WINAPI glMultiTexCoord4d(GLenum texture, GLdouble s, GLdouble t, GLdouble r, GLdouble q)
{
 if(lpglMultiTexCoord4d) (*lpglMultiTexCoord4d)(texture, s, t, r, q);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD1SV)(GLenum, const GLshort*);
static LPGLMULTITEXCOORD1SV lpglMultiTexCoord1sv;
GLvoid WINAPI glMultiTexCoord1sv(GLenum texture, const GLshort* coords)
{
 if(lpglMultiTexCoord1sv) (*lpglMultiTexCoord1sv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD1IV)(GLenum, const GLint*);
static LPGLMULTITEXCOORD1IV lpglMultiTexCoord1iv;
GLvoid WINAPI glMultiTexCoord1iv(GLenum texture, const GLint* coords)
{
 if(lpglMultiTexCoord1iv) (*lpglMultiTexCoord1iv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD1FV)(GLenum, const GLfloat*);
static LPGLMULTITEXCOORD1FV lpglMultiTexCoord1fv;
GLvoid WINAPI glMultiTexCoord1fv(GLenum texture, const GLfloat* coords)
{
 if(lpglMultiTexCoord1fv) (*lpglMultiTexCoord1fv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD1DV)(GLenum, const GLdouble*);
static LPGLMULTITEXCOORD1DV lpglMultiTexCoord1dv;
GLvoid WINAPI glMultiTexCoord1dv(GLenum texture, const GLdouble* coords)
{
 if(lpglMultiTexCoord1dv) (*lpglMultiTexCoord1dv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD2SV)(GLenum, const GLshort*);
static LPGLMULTITEXCOORD2SV lpglMultiTexCoord2sv;
GLvoid WINAPI glMultiTexCoord2sv(GLenum texture, const GLshort* coords)
{
 if(lpglMultiTexCoord2sv) (*lpglMultiTexCoord2sv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD2IV)(GLenum, const GLint*);
static LPGLMULTITEXCOORD2IV lpglMultiTexCoord2iv;
GLvoid WINAPI glMultiTexCoord2iv(GLenum texture, const GLint* coords)
{
 if(lpglMultiTexCoord2iv) (*lpglMultiTexCoord2iv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD2FV)(GLenum, const GLfloat*);
static LPGLMULTITEXCOORD2FV lpglMultiTexCoord2fv;
GLvoid WINAPI glMultiTexCoord2fv(GLenum texture, const GLfloat* coords)
{
 if(lpglMultiTexCoord2fv) (*lpglMultiTexCoord2fv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD2DV)(GLenum, const GLdouble*);
static LPGLMULTITEXCOORD2DV lpglMultiTexCoord2dv;
GLvoid WINAPI glMultiTexCoord2dv(GLenum texture, const GLdouble* coords)
{
 if(lpglMultiTexCoord2dv) (*lpglMultiTexCoord2dv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD3SV)(GLenum, const GLshort*);
static LPGLMULTITEXCOORD3SV lpglMultiTexCoord3sv;
GLvoid WINAPI glMultiTexCoord3sv(GLenum texture, const GLshort* coords)
{
 if(lpglMultiTexCoord3sv) (*lpglMultiTexCoord3sv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD3IV)(GLenum, const GLint*);
static LPGLMULTITEXCOORD3IV lpglMultiTexCoord3iv;
GLvoid WINAPI glMultiTexCoord3iv(GLenum texture, const GLint* coords)
{
 if(lpglMultiTexCoord3iv) (*lpglMultiTexCoord3iv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD3FV)(GLenum, const GLfloat*);
static LPGLMULTITEXCOORD3FV lpglMultiTexCoord3fv;
GLvoid WINAPI glMultiTexCoord3fv(GLenum texture, const GLfloat* coords)
{
 if(lpglMultiTexCoord3fv) (*lpglMultiTexCoord3fv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD3DV)(GLenum, const GLdouble*);
static LPGLMULTITEXCOORD3DV lpglMultiTexCoord3dv;
GLvoid WINAPI glMultiTexCoord3dv(GLenum texture, const GLdouble* coords)
{
 if(lpglMultiTexCoord3dv) (*lpglMultiTexCoord3dv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD4SV)(GLenum, const GLshort*);
static LPGLMULTITEXCOORD4SV lpglMultiTexCoord4sv;
GLvoid WINAPI glMultiTexCoord4sv(GLenum texture, const GLshort* coords)
{
 if(lpglMultiTexCoord4sv) (*lpglMultiTexCoord4sv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD4IV)(GLenum, const GLint*);
static LPGLMULTITEXCOORD4IV lpglMultiTexCoord4iv;
GLvoid WINAPI glMultiTexCoord4iv(GLenum texture, const GLint* coords)
{
 if(lpglMultiTexCoord4iv) (*lpglMultiTexCoord4iv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD4FV)(GLenum, const GLfloat*);
static LPGLMULTITEXCOORD4FV lpglMultiTexCoord4fv;
GLvoid WINAPI glMultiTexCoord4fv(GLenum texture, const GLfloat* coords)
{
 if(lpglMultiTexCoord4fv) (*lpglMultiTexCoord4fv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLMULTITEXCOORD4DV)(GLenum, const GLdouble*);
static LPGLMULTITEXCOORD4DV lpglMultiTexCoord4dv;
GLvoid WINAPI glMultiTexCoord4dv(GLenum texture, const GLdouble* coords)
{
 if(lpglMultiTexCoord4dv) (*lpglMultiTexCoord4dv)(texture, coords);
}

typedef GLvoid (WINAPI *LPGLNORMAL3B)(GLbyte, GLbyte, GLbyte);
static LPGLNORMAL3B lpglNormal3b;
GLvoid WINAPI glNormal3b(GLbyte x, GLbyte y, GLbyte z)
{
 if(lpglNormal3b) (*lpglNormal3b)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLNORMAL3S)(GLshort, GLshort, GLshort);
static LPGLNORMAL3S lpglNormal3s;
GLvoid WINAPI glNormal3s(GLshort x, GLshort y, GLshort z)
{
 if(lpglNormal3s) (*lpglNormal3s)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLNORMAL3I)(GLint, GLint, GLint);
static LPGLNORMAL3I lpglNormal3i;
GLvoid WINAPI glNormal3i(GLint x, GLint y, GLint z)
{
 if(lpglNormal3i) (*lpglNormal3i)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLNORMAL3F)(GLfloat, GLfloat, GLfloat);
static LPGLNORMAL3F lpglNormal3f;
GLvoid WINAPI glNormal3f(GLfloat x, GLfloat y, GLfloat z)
{
 if(lpglNormal3f) (*lpglNormal3f)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLNORMAL3D)(GLdouble, GLdouble, GLdouble);
static LPGLNORMAL3D lpglNormal3d;
GLvoid WINAPI glNormal3d(GLdouble x, GLdouble y, GLdouble z)
{
 if(lpglNormal3d) (*lpglNormal3d)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLNORMAL3BV)(const GLbyte*);
static LPGLNORMAL3BV lpglNormal3bv;
GLvoid WINAPI glNormal3bv(const GLbyte* coords)
{
 if(lpglNormal3bv) (*lpglNormal3bv)(coords);
}

typedef GLvoid (WINAPI *LPGLNORMAL3SV)(const GLshort*);
static LPGLNORMAL3SV lpglNormal3sv;
GLvoid WINAPI glNormal3sv(const GLshort* coords)
{
 if(lpglNormal3sv) (*lpglNormal3sv)(coords);
}

typedef GLvoid (WINAPI *LPGLNORMAL3IV)(const GLint*);
static LPGLNORMAL3IV lpglNormal3iv;
GLvoid WINAPI glNormal3iv(const GLint* coords)
{
 if(lpglNormal3iv) (*lpglNormal3iv)(coords);
}

typedef GLvoid (WINAPI *LPGLNORMAL3FV)(const GLfloat*);
static LPGLNORMAL3FV lpglNormal3fv;
GLvoid WINAPI glNormal3fv(const GLfloat* coords)
{
 if(lpglNormal3fv) (*lpglNormal3fv)(coords);
}

typedef GLvoid (WINAPI *LPGLNORMAL3DV)(const GLdouble*);
static LPGLNORMAL3DV lpglNormal3dv;
GLvoid WINAPI glNormal3dv(const GLdouble* coords)
{
 if(lpglNormal3dv) (*lpglNormal3dv)(coords);
}

typedef GLvoid (WINAPI *LPGLCOLOR3B)(GLbyte, GLbyte, GLbyte);
static LPGLCOLOR3B lpglColor3b;
GLvoid WINAPI glColor3b(GLbyte r, GLbyte g, GLbyte b)
{
 if(lpglColor3b) (*lpglColor3b)(r, g, b);
}

typedef GLvoid (WINAPI *LPGLCOLOR3S)(GLshort, GLshort, GLshort);
static LPGLCOLOR3S lpglColor3s;
GLvoid WINAPI glColor3s(GLshort r, GLshort g, GLshort b)
{
 if(lpglColor3s) (*lpglColor3s)(r, g, b);
}

typedef GLvoid (WINAPI *LPGLCOLOR3I)(GLint, GLint, GLint);
static LPGLCOLOR3I lpglColor3i;
GLvoid WINAPI glColor3i(GLint r, GLint g, GLint b)
{
 if(lpglColor3i) (*lpglColor3i)(r, g, b);
}

typedef GLvoid (WINAPI *LPGLCOLOR3F)(GLfloat, GLfloat, GLfloat);
static LPGLCOLOR3F lpglColor3f;
GLvoid WINAPI glColor3f(GLfloat r, GLfloat g, GLfloat b)
{
 if(lpglColor3f) (*lpglColor3f)(r, g, b);
}

typedef GLvoid (WINAPI *LPGLCOLOR3D)(GLdouble, GLdouble, GLdouble);
static LPGLCOLOR3D lpglColor3d;
GLvoid WINAPI glColor3d(GLdouble r, GLdouble g, GLdouble b)
{
 if(lpglColor3d) (*lpglColor3d)(r, g, b);
}

typedef GLvoid (WINAPI *LPGLCOLOR3UB)(GLubyte, GLubyte, GLubyte);
static LPGLCOLOR3UB lpglColor3ub;
GLvoid WINAPI glColor3ub(GLubyte r, GLubyte g, GLubyte b)
{
 if(lpglColor3ub) (*lpglColor3ub)(r, g, b);
}

typedef GLvoid (WINAPI *LPGLCOLOR3US)(GLushort, GLushort, GLushort);
static LPGLCOLOR3US lpglColor3us;
GLvoid WINAPI glColor3us(GLushort r, GLushort g, GLushort b)
{
 if(lpglColor3us) (*lpglColor3us)(r, g, b);
}

typedef GLvoid (WINAPI *LPGLCOLOR3UI)(GLuint, GLuint, GLuint);
static LPGLCOLOR3UI lpglColor3ui;
GLvoid WINAPI glColor3ui(GLuint r, GLuint g, GLuint b)
{
 if(lpglColor3ui) (*lpglColor3ui)(r, g, b);
}

typedef GLvoid (WINAPI *LPGLCOLOR4B)(GLbyte, GLbyte, GLbyte, GLbyte);
static LPGLCOLOR4B lpglColor4b;
GLvoid WINAPI glColor4b(GLbyte r, GLbyte g, GLbyte b, GLbyte a)
{
 if(lpglColor4b) (*lpglColor4b)(r, g, b, a);
}

typedef GLvoid (WINAPI *LPGLCOLOR4S)(GLshort, GLshort, GLshort, GLshort);
static LPGLCOLOR4S lpglColor4s;
GLvoid WINAPI glColor4s(GLshort r, GLshort g, GLshort b, GLshort a)
{
 if(lpglColor4s) (*lpglColor4s)(r, g, b, a);
}

typedef GLvoid (WINAPI *LPGLCOLOR4I)(GLint, GLint, GLint, GLint);
static LPGLCOLOR4I lpglColor4i;
GLvoid WINAPI glColor4i(GLint r, GLint g, GLint b, GLint a)
{
 if(lpglColor4i) (*lpglColor4i)(r, g, b, a);
}

typedef GLvoid (WINAPI *LPGLCOLOR4F)(GLfloat, GLfloat, GLfloat, GLfloat);
static LPGLCOLOR4F lpglColor4f;
GLvoid WINAPI glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
 if(lpglColor4f) (*lpglColor4f)(r, g, b, a);
}

typedef GLvoid (WINAPI *LPGLCOLOR4D)(GLdouble, GLdouble, GLdouble, GLdouble);
static LPGLCOLOR4D lpglColor4d;
GLvoid WINAPI glColor4d(GLdouble r, GLdouble g, GLdouble b, GLdouble a)
{
 if(lpglColor4d) (*lpglColor4d)(r, g, b, a);
}

typedef GLvoid (WINAPI *LPGLCOLOR4UB)(GLubyte, GLubyte, GLubyte, GLubyte);
static LPGLCOLOR4UB lpglColor4ub;
GLvoid WINAPI glColor4ub(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
{
 if(lpglColor4ub) (*lpglColor4ub)(r, g, b, a);
}

typedef GLvoid (WINAPI *LPGLCOLOR4US)(GLushort, GLushort, GLushort, GLushort);
static LPGLCOLOR4US lpglColor4us;
GLvoid WINAPI glColor4us(GLushort r, GLushort g, GLushort b, GLushort a)
{
 if(lpglColor4us) (*lpglColor4us)(r, g, b, a);
}

typedef GLvoid (WINAPI *LPGLCOLOR4UI)(GLuint, GLuint, GLuint, GLuint);
static LPGLCOLOR4UI lpglColor4ui;
GLvoid WINAPI glColor4ui(GLuint r, GLuint g, GLuint b, GLuint a)
{
 if(lpglColor4ui) (*lpglColor4ui)(r, g, b, a);
}

typedef GLvoid (WINAPI *LPGLCOLOR3BV)(const GLbyte*);
static LPGLCOLOR3BV lpglColor3bv;
GLvoid WINAPI glColor3bv(const GLbyte* components)
{
 if(lpglColor3bv) (*lpglColor3bv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR3SV)(const GLshort*);
static LPGLCOLOR3SV lpglColor3sv;
GLvoid WINAPI glColor3sv(const GLshort* components)
{
 if(lpglColor3sv) (*lpglColor3sv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR3IV)(const GLint*);
static LPGLCOLOR3IV lpglColor3iv;
GLvoid WINAPI glColor3iv(const GLint* components)
{
 if(lpglColor3iv) (*lpglColor3iv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR3FV)(const GLfloat*);
static LPGLCOLOR3FV lpglColor3fv;
GLvoid WINAPI glColor3fv(const GLfloat* components)
{
 if(lpglColor3fv) (*lpglColor3fv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR3DV)(const GLdouble*);
static LPGLCOLOR3DV lpglColor3dv;
GLvoid WINAPI glColor3dv(const GLdouble* components)
{
 if(lpglColor3dv) (*lpglColor3dv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR3UBV)(const GLubyte*);
static LPGLCOLOR3UBV lpglColor3ubv;
GLvoid WINAPI glColor3ubv(const GLubyte* components)
{
 if(lpglColor3ubv) (*lpglColor3ubv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR3USV)(const GLushort*);
static LPGLCOLOR3USV lpglColor3usv;
GLvoid WINAPI glColor3usv(const GLushort* components)
{
 if(lpglColor3usv) (*lpglColor3usv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR3UIV)(const GLuint*);
static LPGLCOLOR3UIV lpglColor3uiv;
GLvoid WINAPI glColor3uiv(const GLuint* components)
{
 if(lpglColor3uiv) (*lpglColor3uiv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR4BV)(const GLbyte*);
static LPGLCOLOR4BV lpglColor4bv;
GLvoid WINAPI glColor4bv(const GLbyte* components)
{
 if(lpglColor4bv) (*lpglColor4bv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR4SV)(const GLshort*);
static LPGLCOLOR4SV lpglColor4sv;
GLvoid WINAPI glColor4sv(const GLshort* components)
{
 if(lpglColor4sv) (*lpglColor4sv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR4IV)(const GLint*);
static LPGLCOLOR4IV lpglColor4iv;
GLvoid WINAPI glColor4iv(const GLint* components)
{
 if(lpglColor4iv) (*lpglColor4iv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR4FV)(const GLfloat*);
static LPGLCOLOR4FV lpglColor4fv;
GLvoid WINAPI glColor4fv(const GLfloat* components)
{
 if(lpglColor4fv) (*lpglColor4fv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR4DV)(const GLdouble*);
static LPGLCOLOR4DV lpglColor4dv;
GLvoid WINAPI glColor4dv(const GLdouble* components)
{
 if(lpglColor4dv) (*lpglColor4dv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR4UBV)(const GLubyte*);
static LPGLCOLOR4UBV lpglColor4ubv;
GLvoid WINAPI glColor4ubv(const GLubyte* components)
{
 if(lpglColor4ubv) (*lpglColor4ubv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR4USV)(const GLushort*);
static LPGLCOLOR4USV lpglColor4usv;
GLvoid WINAPI glColor4usv(const GLushort* components)
{
 if(lpglColor4usv) (*lpglColor4usv)(components);
}

typedef GLvoid (WINAPI *LPGLCOLOR4UIV)(const GLuint*);
static LPGLCOLOR4UIV lpglColor4uiv;
GLvoid WINAPI glColor4uiv(const GLuint* components)
{
 if(lpglColor4uiv) (*lpglColor4uiv)(components);
}

typedef GLvoid (WINAPI *LPGLINDEXS)(GLshort);
static LPGLINDEXS lpglIndexs;
GLvoid WINAPI glIndexs(GLshort index)
{
 if(lpglIndexs) (*lpglIndexs)(index);
}

typedef GLvoid (WINAPI *LPGLINDEXI)(GLint);
static LPGLINDEXI lpglIndexi;
GLvoid WINAPI glIndexi(GLint index)
{
 if(lpglIndexi) (*lpglIndexi)(index);
}

typedef GLvoid (WINAPI *LPGLINDEXF)(GLfloat);
static LPGLINDEXF lpglIndexf;
GLvoid WINAPI glIndexf(GLfloat index)
{
 if(lpglIndexf) (*lpglIndexf)(index);
}

typedef GLvoid (WINAPI *LPGLINDEXD)(GLdouble);
static LPGLINDEXD lpglIndexd;
GLvoid WINAPI glIndexd(GLdouble index)
{
 if(lpglIndexd) (*lpglIndexd)(index);
}

typedef GLvoid (WINAPI *LPGLINDEXUB)(GLubyte);
static LPGLINDEXUB lpglIndexub;
GLvoid WINAPI glIndexub(GLubyte index)
{
 if(lpglIndexub) (*lpglIndexub)(index);
}

typedef GLvoid (WINAPI *LPGLINDEXSV)(const GLshort*);
static LPGLINDEXSV lpglIndexsv;
GLvoid WINAPI glIndexsv(const GLshort* index)
{
 if(lpglIndexsv) (*lpglIndexsv)(index);
}

typedef GLvoid (WINAPI *LPGLINDEXIV)(const GLint*);
static LPGLINDEXIV lpglIndexiv;
GLvoid WINAPI glIndexiv(const GLint* index)
{
 if(lpglIndexiv) (*lpglIndexiv)(index);
}

typedef GLvoid (WINAPI *LPGLINDEXFV)(const GLfloat*);
static LPGLINDEXFV lpglIndexfv;
GLvoid WINAPI glIndexfv(const GLfloat* index)
{
 if(lpglIndexfv) (*lpglIndexfv)(index);
}

typedef GLvoid (WINAPI *LPGLINDEXDV)(const GLdouble*);
static LPGLINDEXDV lpglIndexdv;
GLvoid WINAPI glIndexdv(const GLdouble* index)
{
 if(lpglIndexdv) (*lpglIndexdv)(index);
}

typedef GLvoid (WINAPI *LPGLINDEXUBV)(const GLubyte*);
static LPGLINDEXUBV lpglIndexubv;
GLvoid WINAPI glIndexubv(const GLubyte* index)
{
 if(lpglIndexubv) (*lpglIndexubv)(index);
}

// Section 2.8
// Vertex Arrays

typedef GLvoid (WINAPI *LPGLCLIENTACTIVETEXTURE)(GLenum);
static LPGLCLIENTACTIVETEXTURE lpglClientActiveTexture;
GLvoid WINAPI glClientActiveTexture(GLenum texture)
{
 if(lpglClientActiveTexture) (*lpglClientActiveTexture)(texture);
}

typedef GLvoid (WINAPI *LPGLEDGEFLAGPOINTER)(GLsizei, const GLvoid*);
static LPGLEDGEFLAGPOINTER lpglEdgeFlagPointer;
GLvoid WINAPI glEdgeFlagPointer(GLsizei stride, const GLvoid* pointer)
{
 if(lpglEdgeFlagPointer) (*lpglEdgeFlagPointer)(stride, pointer);
}

typedef GLvoid (WINAPI *LPGLTEXCOORDPOINTER)(GLint, GLenum, GLsizei, const GLvoid*);
static LPGLTEXCOORDPOINTER lpglTexCoordPointer;
GLvoid WINAPI glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
 if(lpglTexCoordPointer) (*lpglTexCoordPointer)(size, type, stride, pointer);
}

typedef GLvoid (WINAPI *LPGLCOLORPOINTER)(GLint, GLenum, GLsizei, const GLvoid*);
static LPGLCOLORPOINTER lpglColorPointer;
GLvoid WINAPI glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
 if(lpglColorPointer) (*lpglColorPointer)(size, type, stride, pointer);
}

typedef GLvoid (WINAPI *LPGLINDEXPOINTER)(GLenum, GLsizei, const GLvoid*);
static LPGLINDEXPOINTER lpglIndexPointer;
GLvoid WINAPI glIndexPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
{
 if(lpglIndexPointer) (*lpglIndexPointer)(type, stride, pointer);
}

typedef GLvoid (WINAPI *LPGLNORMALPOINTER)(GLenum, GLsizei, const GLvoid*);
static LPGLNORMALPOINTER lpglNormalPointer;
GLvoid WINAPI glNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
{
 if(lpglNormalPointer) (*lpglNormalPointer)(type, stride, pointer);
}

typedef GLvoid (WINAPI *LPGLVERTEXPOINTER)(GLint, GLenum, GLsizei, const GLvoid*);
static LPGLVERTEXPOINTER lpglVertexPointer;
GLvoid WINAPI glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
 if(lpglVertexPointer) (*lpglVertexPointer)(size, type, stride, pointer);
}

typedef GLvoid (WINAPI *LPGLENABLECLIENTSTATE)(GLenum);
static LPGLENABLECLIENTSTATE lpglEnableClientState;
GLvoid WINAPI glEnableClientState(GLenum array)
{
 if(lpglEnableClientState) (*lpglEnableClientState)(array);
}

typedef GLvoid (WINAPI *LPGLDISABLECLIENTSTATE)(GLenum);
static LPGLDISABLECLIENTSTATE lpglDisableClientState;
GLvoid WINAPI glDisableClientState(GLenum array)
{
 if(lpglDisableClientState) (*lpglDisableClientState)(array);
}

typedef GLvoid (WINAPI *LPGLARRAYELEMENT)(GLint);
static LPGLARRAYELEMENT lpglArrayElement;
GLvoid WINAPI glArrayElement(GLint i)
{
 if(lpglArrayElement) (*lpglArrayElement)(i);
}

typedef GLvoid (WINAPI *LPGLDRAWARRAYS)(GLenum, GLint, GLsizei);
static LPGLDRAWARRAYS lpglDrawArrays;
GLvoid WINAPI glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
 if(lpglDrawArrays) (*lpglDrawArrays)(mode, first, count);
}

typedef GLvoid (WINAPI *LPGLDRAWELEMENTS)(GLenum, GLsizei, GLenum, const GLvoid*);
static LPGLDRAWELEMENTS lpglDrawElements;
GLvoid WINAPI glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
 if(lpglDrawElements) (*lpglDrawElements)(mode, count, type, indices);
}

typedef GLvoid (WINAPI *LPGLDRAWRANGEELEMENTS)(GLenum, GLuint, GLuint, GLsizei, GLenum, const GLvoid*);
static LPGLDRAWRANGEELEMENTS lpglDrawRangeElements;
GLvoid WINAPI glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid* indices)
{
 if(lpglDrawRangeElements) (*lpglDrawRangeElements)(mode, start, end, count, type, indices);
}

typedef GLvoid (WINAPI *LPGLINTERLEAVEDARRAYS)(GLenum, GLsizei, const GLvoid*);
static LPGLINTERLEAVEDARRAYS lpglInterleavedArrays;
GLvoid WINAPI glInterleavedArrays(GLenum format, GLsizei stride, const GLvoid* pointer)
{
 if(lpglInterleavedArrays) (*lpglInterleavedArrays)(format, stride, pointer);
}

// Section 2.9
// Rectangles

typedef GLvoid (WINAPI *LPGLRECTS)(GLshort, GLshort, GLshort, GLshort);
static LPGLRECTS lpglRects;
GLvoid WINAPI glRects(GLshort x1, GLshort y1, GLshort x2, GLshort y2)
{
 if(lpglRects) (*lpglRects)(x1, y1, x2, y2);
}

typedef GLvoid (WINAPI *LPGLRECTI)(GLint, GLint, GLint, GLint);
static LPGLRECTI lpglRecti;
GLvoid WINAPI glRecti(GLint x1, GLint y1, GLint x2, GLint y2)
{
 if(lpglRecti) (*lpglRecti)(x1, y1, x2, y2);
}

typedef GLvoid (WINAPI *LPGLRECTF)(GLfloat, GLfloat, GLfloat, GLfloat);
static LPGLRECTF lpglRectf;
GLvoid WINAPI glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
 if(lpglRectf) (*lpglRectf)(x1, y1, x2, y2);
}

typedef GLvoid (WINAPI *LPGLRECTD)(GLdouble, GLdouble, GLdouble, GLdouble);
static LPGLRECTD lpglRectd;
GLvoid WINAPI glRectd(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2)
{
 if(lpglRectd) (*lpglRectd)(x1, y1, x2, y2);
}

typedef GLvoid (WINAPI *LPGLRECTSV)(GLshort, GLshort);
static LPGLRECTSV lpglRectsv;
GLvoid WINAPI glRectsv(GLshort v1[2], GLshort v2[2])
{
 if(lpglRectsv) (*lpglRectsv)(v1[2], v2[2]);
}

typedef GLvoid (WINAPI *LPGLRECTIV)(GLint, GLint);
static LPGLRECTIV lpglRectiv;
GLvoid WINAPI glRectiv(GLint v1[2], GLint v2[2])
{
 if(lpglRectiv) (*lpglRectiv)(v1[2], v2[2]);
}

typedef GLvoid (WINAPI *LPGLRECTFV)(GLfloat, GLfloat);
static LPGLRECTFV lpglRectfv;
GLvoid WINAPI glRectfv(GLfloat v1[2], GLfloat v2[2])
{
 if(lpglRectfv) (*lpglRectfv)(v1[2], v2[2]);
}

typedef GLvoid (WINAPI *LPGLRECTDV)(GLdouble, GLdouble);
static LPGLRECTDV lpglRectdv;
GLvoid WINAPI glRectdv(GLdouble v1[2], GLdouble v2[2])
{
 if(lpglRectdv) (*lpglRectdv)(v1[2], v2[2]);
}

// Section 2.10.1
// Controlling the Viewport

typedef GLvoid (WINAPI *LPGLDEPTHRANGE)(GLclampd, GLclampd);
static LPGLDEPTHRANGE lpglDepthRange;
GLvoid WINAPI glDepthRange(GLclampd n, GLclampd f)
{
 if(lpglDepthRange) (*lpglDepthRange)(n, f);
}

typedef GLvoid (WINAPI *LPGLVIEWPORT)(GLint, GLint, GLsizei, GLsizei);
static LPGLVIEWPORT lpglViewport;
GLvoid WINAPI glViewport(GLint x, GLint y, GLsizei w, GLsizei h)
{
 if(lpglViewport) (*lpglViewport)(x, y, w, h);
}

// Section 2.10.2
// Matrices

typedef GLvoid (WINAPI *LPGLMATRIXMODE)(GLenum);
static LPGLMATRIXMODE lpglMatrixMode;
GLvoid WINAPI glMatrixMode(GLenum mode)
{
 if(lpglMatrixMode) (*lpglMatrixMode)(mode);
}

typedef GLvoid (WINAPI *LPGLLOADMATRIXF)(GLfloat);
static LPGLLOADMATRIXF lpglLoadMatrixf;
GLvoid WINAPI glLoadMatrixf(GLfloat m[16])
{
 if(lpglLoadMatrixf) (*lpglLoadMatrixf)(m[16]);
}

typedef GLvoid (WINAPI *LPGLLOADMATRIXD)(GLdouble);
static LPGLLOADMATRIXD lpglLoadMatrixd;
GLvoid WINAPI glLoadMatrixd(GLdouble m[16])
{
 if(lpglLoadMatrixd) (*lpglLoadMatrixd)(m[16]);
}

typedef GLvoid (WINAPI *LPGLMULTMATRIXF)(GLfloat);
static LPGLMULTMATRIXF lpglMultMatrixf;
GLvoid WINAPI glMultMatrixf(GLfloat m[16])
{
 if(lpglMultMatrixf) (*lpglMultMatrixf)(m[16]);
}

typedef GLvoid (WINAPI *LPGLMULTMATRIXD)(GLdouble);
static LPGLMULTMATRIXD lpglMultMatrixd;
GLvoid WINAPI glMultMatrixd(GLdouble m[16])
{
 if(lpglMultMatrixd) (*lpglMultMatrixd)(m[16]);
}

typedef GLvoid (WINAPI *LPGLLOADIDENTITY)(GLvoid);
static LPGLLOADIDENTITY lpglLoadIdentity;
GLvoid WINAPI glLoadIdentity(GLvoid)
{
 if(lpglLoadIdentity) (*lpglLoadIdentity)();
}

typedef GLvoid (WINAPI *LPGLROTATEF)(GLfloat, GLfloat, GLfloat, GLfloat);
static LPGLROTATEF lpglRotatef;
GLvoid WINAPI glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
 if(lpglRotatef) (*lpglRotatef)(angle, x, y, z);
}

typedef GLvoid (WINAPI *LPGLROTATED)(GLdouble, GLdouble, GLdouble, GLdouble);
static LPGLROTATED lpglRotated;
GLvoid WINAPI glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z)
{
 if(lpglRotated) (*lpglRotated)(angle, x, y, z);
}

typedef GLvoid (WINAPI *LPGLTRANSLATEF)(GLfloat, GLfloat, GLfloat);
static LPGLTRANSLATEF lpglTranslatef;
GLvoid WINAPI glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
 if(lpglTranslatef) (*lpglTranslatef)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLTRANSLATED)(GLdouble, GLdouble, GLdouble);
static LPGLTRANSLATED lpglTranslated;
GLvoid WINAPI glTranslated(GLdouble x, GLdouble y, GLdouble z)
{
 if(lpglTranslated) (*lpglTranslated)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLSCALEF)(GLfloat, GLfloat, GLfloat);
static LPGLSCALEF lpglScalef;
GLvoid WINAPI glScalef(GLfloat x, GLfloat y, GLfloat z)
{
 if(lpglScalef) (*lpglScalef)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLSCALED)(GLdouble, GLdouble, GLdouble);
static LPGLSCALED lpglScaled;
GLvoid WINAPI glScaled(GLdouble x, GLdouble y, GLdouble z)
{
 if(lpglScaled) (*lpglScaled)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLFRUSTUM)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
static LPGLFRUSTUM lpglFrustum;
GLvoid WINAPI glFrustum(GLdouble l, GLdouble r, GLdouble b, GLdouble t, GLdouble n, GLdouble f)
{
 if(lpglFrustum) (*lpglFrustum)(l, r, b, t, n, f);
}

typedef GLvoid (WINAPI *LPGLORTHO)(GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble);
static LPGLORTHO lpglOrtho;
GLvoid WINAPI glOrtho(GLdouble l, GLdouble r, GLdouble b, GLdouble t, GLdouble n, GLdouble f)
{
 if(lpglOrtho) (*lpglOrtho)(l, r, b, t, n, f);
}

typedef GLvoid (WINAPI *LPGLPUSHMATRIX)(GLvoid);
static LPGLPUSHMATRIX lpglPushMatrix;
GLvoid WINAPI glPushMatrix(GLvoid)
{
 if(lpglPushMatrix) (*lpglPushMatrix)();
}

typedef GLvoid (WINAPI *LPGLACTIVETEXTURE)(GLenum);
static LPGLACTIVETEXTURE lpglActiveTexture;
GLvoid WINAPI glActiveTexture(GLenum texture)
{
 if(lpglActiveTexture) (*lpglActiveTexture)(texture);
}

typedef GLvoid (WINAPI *LPGLPOPMATRIX)(GLvoid);
static LPGLPOPMATRIX lpglPopMatrix;
GLvoid WINAPI glPopMatrix(GLvoid)
{
 if(lpglPopMatrix) (*lpglPopMatrix)();
}

// Section 2.10.3
// Normal Transformation

typedef GLvoid (WINAPI *LPGLENABLE)(GLenum);
static LPGLENABLE lpglEnable;
GLvoid WINAPI glEnable(GLenum target)
{
 if(lpglEnable) (*lpglEnable)(target);
}

typedef GLvoid (WINAPI *LPGLDISABLE)(GLenum);
static LPGLDISABLE lpglDisable;
GLvoid WINAPI glDisable(GLenum target)
{
 if(lpglDisable) (*lpglDisable)(target);
}

// Section 2.10.4
// Generating Texture Coordinates

typedef GLvoid (WINAPI *LPGLTEXGENI)(GLenum, GLenum, GLint);
static LPGLTEXGENI lpglTexGeni;
GLvoid WINAPI glTexGeni(GLenum coord, GLenum pname, GLint param)
{
 if(lpglTexGeni) (*lpglTexGeni)(coord, pname, param);
}

typedef GLvoid (WINAPI *LPGLTEXGENF)(GLenum, GLenum, GLfloat);
static LPGLTEXGENF lpglTexGenf;
GLvoid WINAPI glTexGenf(GLenum coord, GLenum pname, GLfloat param)
{
 if(lpglTexGenf) (*lpglTexGenf)(coord, pname, param);
}

typedef GLvoid (WINAPI *LPGLTEXGEND)(GLenum, GLenum, GLdouble);
static LPGLTEXGEND lpglTexGend;
GLvoid WINAPI glTexGend(GLenum coord, GLenum pname, GLdouble param)
{
 if(lpglTexGend) (*lpglTexGend)(coord, pname, param);
}

typedef GLvoid (WINAPI *LPGLTEXGENIV)(GLenum, GLenum, const GLint*);
static LPGLTEXGENIV lpglTexGeniv;
GLvoid WINAPI glTexGeniv(GLenum coord, GLenum pname, const GLint* params)
{
 if(lpglTexGeniv) (*lpglTexGeniv)(coord, pname, params);
}

typedef GLvoid (WINAPI *LPGLTEXGENFV)(GLenum, GLenum, const GLfloat*);
static LPGLTEXGENFV lpglTexGenfv;
GLvoid WINAPI glTexGenfv(GLenum coord, GLenum pname, const GLfloat* params)
{
 if(lpglTexGenfv) (*lpglTexGenfv)(coord, pname, params);
}

typedef GLvoid (WINAPI *LPGLTEXGENDV)(GLenum, GLenum, const GLdouble*);
static LPGLTEXGENDV lpglTexGendv;
GLvoid WINAPI glTexGendv(GLenum coord, GLenum pname, const GLdouble* params)
{
 if(lpglTexGendv) (*lpglTexGendv)(coord, pname, params);
}

// Section 2.11
// Clipping

typedef GLvoid (WINAPI *LPGLCLIPPLANE)(GLenum, GLdouble);
static LPGLCLIPPLANE lpglClipPlane;
GLvoid WINAPI glClipPlane(GLenum p, GLdouble eqn[4])
{
 if(lpglClipPlane) (*lpglClipPlane)(p, eqn[4]);
}

// Section 2.12
// Current Raster Position

typedef GLvoid (WINAPI *LPGLRASTERPOS2S)(GLshort, GLshort);
static LPGLRASTERPOS2S lpglRasterPos2s;
GLvoid WINAPI glRasterPos2s(GLshort x, GLshort y)
{
 if(lpglRasterPos2s) (*lpglRasterPos2s)(x, y);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS2I)(GLint, GLint);
static LPGLRASTERPOS2I lpglRasterPos2i;
GLvoid WINAPI glRasterPos2i(GLint x, GLint y)
{
 if(lpglRasterPos2i) (*lpglRasterPos2i)(x, y);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS2F)(GLfloat, GLfloat);
static LPGLRASTERPOS2F lpglRasterPos2f;
GLvoid WINAPI glRasterPos2f(GLfloat x, GLfloat y)
{
 if(lpglRasterPos2f) (*lpglRasterPos2f)(x, y);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS2D)(GLdouble, GLdouble);
static LPGLRASTERPOS2D lpglRasterPos2d;
GLvoid WINAPI glRasterPos2d(GLdouble x, GLdouble y)
{
 if(lpglRasterPos2d) (*lpglRasterPos2d)(x, y);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS3S)(GLshort, GLshort, GLshort);
static LPGLRASTERPOS3S lpglRasterPos3s;
GLvoid WINAPI glRasterPos3s(GLshort x, GLshort y, GLshort z)
{
 if(lpglRasterPos3s) (*lpglRasterPos3s)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS3I)(GLint, GLint, GLint);
static LPGLRASTERPOS3I lpglRasterPos3i;
GLvoid WINAPI glRasterPos3i(GLint x, GLint y, GLint z)
{
 if(lpglRasterPos3i) (*lpglRasterPos3i)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS3F)(GLfloat, GLfloat, GLfloat);
static LPGLRASTERPOS3F lpglRasterPos3f;
GLvoid WINAPI glRasterPos3f(GLfloat x, GLfloat y, GLfloat z)
{
 if(lpglRasterPos3f) (*lpglRasterPos3f)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS3D)(GLdouble, GLdouble, GLdouble);
static LPGLRASTERPOS3D lpglRasterPos3d;
GLvoid WINAPI glRasterPos3d(GLdouble x, GLdouble y, GLdouble z)
{
 if(lpglRasterPos3d) (*lpglRasterPos3d)(x, y, z);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS4S)(GLshort, GLshort, GLshort, GLshort);
static LPGLRASTERPOS4S lpglRasterPos4s;
GLvoid WINAPI glRasterPos4s(GLshort x, GLshort y, GLshort z, GLshort w)
{
 if(lpglRasterPos4s) (*lpglRasterPos4s)(x, y, z, w);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS4I)(GLint, GLint, GLint, GLint);
static LPGLRASTERPOS4I lpglRasterPos4i;
GLvoid WINAPI glRasterPos4i(GLint x, GLint y, GLint z, GLint w)
{
 if(lpglRasterPos4i) (*lpglRasterPos4i)(x, y, z, w);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS4F)(GLfloat, GLfloat, GLfloat, GLfloat);
static LPGLRASTERPOS4F lpglRasterPos4f;
GLvoid WINAPI glRasterPos4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
 if(lpglRasterPos4f) (*lpglRasterPos4f)(x, y, z, w);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS4D)(GLdouble, GLdouble, GLdouble, GLdouble);
static LPGLRASTERPOS4D lpglRasterPos4d;
GLvoid WINAPI glRasterPos4d(GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
 if(lpglRasterPos4d) (*lpglRasterPos4d)(x, y, z, w);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS2SV)(const GLshort*);
static LPGLRASTERPOS2SV lpglRasterPos2sv;
GLvoid WINAPI glRasterPos2sv(const GLshort* coords)
{
 if(lpglRasterPos2sv) (*lpglRasterPos2sv)(coords);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS2IV)(const GLint*);
static LPGLRASTERPOS2IV lpglRasterPos2iv;
GLvoid WINAPI glRasterPos2iv(const GLint* coords)
{
 if(lpglRasterPos2iv) (*lpglRasterPos2iv)(coords);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS2FV)(const GLfloat*);
static LPGLRASTERPOS2FV lpglRasterPos2fv;
GLvoid WINAPI glRasterPos2fv(const GLfloat* coords)
{
 if(lpglRasterPos2fv) (*lpglRasterPos2fv)(coords);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS2DV)(const GLdouble*);
static LPGLRASTERPOS2DV lpglRasterPos2dv;
GLvoid WINAPI glRasterPos2dv(const GLdouble* coords)
{
 if(lpglRasterPos2dv) (*lpglRasterPos2dv)(coords);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS3SV)(const GLshort*);
static LPGLRASTERPOS3SV lpglRasterPos3sv;
GLvoid WINAPI glRasterPos3sv(const GLshort* coords)
{
 if(lpglRasterPos3sv) (*lpglRasterPos3sv)(coords);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS3IV)(const GLint*);
static LPGLRASTERPOS3IV lpglRasterPos3iv;
GLvoid WINAPI glRasterPos3iv(const GLint* coords)
{
 if(lpglRasterPos3iv) (*lpglRasterPos3iv)(coords);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS3FV)(const GLfloat*);
static LPGLRASTERPOS3FV lpglRasterPos3fv;
GLvoid WINAPI glRasterPos3fv(const GLfloat* coords)
{
 if(lpglRasterPos3fv) (*lpglRasterPos3fv)(coords);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS3DV)(const GLdouble*);
static LPGLRASTERPOS3DV lpglRasterPos3dv;
GLvoid WINAPI glRasterPos3dv(const GLdouble* coords)
{
 if(lpglRasterPos3dv) (*lpglRasterPos3dv)(coords);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS4SV)(const GLshort*);
static LPGLRASTERPOS4SV lpglRasterPos4sv;
GLvoid WINAPI glRasterPos4sv(const GLshort* coords)
{
 if(lpglRasterPos4sv) (*lpglRasterPos4sv)(coords);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS4IV)(const GLint*);
static LPGLRASTERPOS4IV lpglRasterPos4iv;
GLvoid WINAPI glRasterPos4iv(const GLint* coords)
{
 if(lpglRasterPos4iv) (*lpglRasterPos4iv)(coords);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS4FV)(const GLfloat*);
static LPGLRASTERPOS4FV lpglRasterPos4fv;
GLvoid WINAPI glRasterPos4fv(const GLfloat* coords)
{
 if(lpglRasterPos4fv) (*lpglRasterPos4fv)(coords);
}

typedef GLvoid (WINAPI *LPGLRASTERPOS4DV)(const GLdouble*);
static LPGLRASTERPOS4DV lpglRasterPos4dv;
GLvoid WINAPI glRasterPos4dv(const GLdouble* coords)
{
 if(lpglRasterPos4dv) (*lpglRasterPos4dv)(coords);
}

// Section 2.13.1
// Lighting

typedef GLvoid (WINAPI *LPGLFRONTFACE)(GLenum);
static LPGLFRONTFACE lpglFrontFace;
GLvoid WINAPI glFrontFace(GLenum dir)
{
 if(lpglFrontFace) (*lpglFrontFace)(dir);
}

// Section 2.13.2
// Lighting Parameter Specification

typedef GLvoid (WINAPI *LPGLMATERIALI)(GLenum, GLenum, GLint);
static LPGLMATERIALI lpglMateriali;
GLvoid WINAPI glMateriali(GLenum face, GLenum pname, GLint param)
{
 if(lpglMateriali) (*lpglMateriali)(face, pname, param);
}

typedef GLvoid (WINAPI *LPGLMATERIALF)(GLenum, GLenum, GLfloat);
static LPGLMATERIALF lpglMaterialf;
GLvoid WINAPI glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
 if(lpglMaterialf) (*lpglMaterialf)(face, pname, param);
}

typedef GLvoid (WINAPI *LPGLMATERIALIV)(GLenum, GLenum, const GLint*);
static LPGLMATERIALIV lpglMaterialiv;
GLvoid WINAPI glMaterialiv(GLenum face, GLenum pname, const GLint* params)
{
 if(lpglMaterialiv) (*lpglMaterialiv)(face, pname, params);
}

typedef GLvoid (WINAPI *LPGLMATERIALFV)(GLenum, GLenum, const GLfloat*);
static LPGLMATERIALFV lpglMaterialfv;
GLvoid WINAPI glMaterialfv(GLenum face, GLenum pname, const GLfloat* params)
{
 if(lpglMaterialfv) (*lpglMaterialfv)(face, pname, params);
}

typedef GLvoid (WINAPI *LPGLLIGHTI)(GLenum, GLenum, GLint);
static LPGLLIGHTI lpglLighti;
GLvoid WINAPI glLighti(GLenum light, GLenum pname, GLint param)
{
 if(lpglLighti) (*lpglLighti)(light, pname, param);
}

typedef GLvoid (WINAPI *LPGLLIGHTF)(GLenum, GLenum, GLfloat);
static LPGLLIGHTF lpglLightf;
GLvoid WINAPI glLightf(GLenum light, GLenum pname, GLfloat param)
{
 if(lpglLightf) (*lpglLightf)(light, pname, param);
}

typedef GLvoid (WINAPI *LPGLLIGHTIV)(GLenum, GLenum, const GLint*);
static LPGLLIGHTIV lpglLightiv;
GLvoid WINAPI glLightiv(GLenum light, GLenum pname, const GLint* params)
{
 if(lpglLightiv) (*lpglLightiv)(light, pname, params);
}

typedef GLvoid (WINAPI *LPGLLIGHTFV)(GLenum, GLenum, const GLfloat*);
static LPGLLIGHTFV lpglLightfv;
GLvoid WINAPI glLightfv(GLenum light, GLenum pname, const GLfloat* params)
{
 if(lpglLightfv) (*lpglLightfv)(light, pname, params);
}

typedef GLvoid (WINAPI *LPGLLIGHTMODELI)(GLenum, GLint);
static LPGLLIGHTMODELI lpglLightModeli;
GLvoid WINAPI glLightModeli(GLenum pname, GLint param)
{
 if(lpglLightModeli) (*lpglLightModeli)(pname, param);
}

typedef GLvoid (WINAPI *LPGLLIGHTMODELF)(GLenum, GLfloat);
static LPGLLIGHTMODELF lpglLightModelf;
GLvoid WINAPI glLightModelf(GLenum pname, GLfloat param)
{
 if(lpglLightModelf) (*lpglLightModelf)(pname, param);
}

typedef GLvoid (WINAPI *LPGLLIGHTMODELIV)(GLenum, const GLint*);
static LPGLLIGHTMODELIV lpglLightModeliv;
GLvoid WINAPI glLightModeliv(GLenum pname, const GLint* params)
{
 if(lpglLightModeliv) (*lpglLightModeliv)(pname, params);
}

typedef GLvoid (WINAPI *LPGLLIGHTMODELFV)(GLenum, const GLfloat*);
static LPGLLIGHTMODELFV lpglLightModelfv;
GLvoid WINAPI glLightModelfv(GLenum pname, const GLfloat* params)
{
 if(lpglLightModelfv) (*lpglLightModelfv)(pname, params);
}

// Section 2.13.3
// Color Material

typedef GLvoid (WINAPI *LPGLCOLORMATERIAL)(GLenum, GLenum);
static LPGLCOLORMATERIAL lpglColorMaterial;
GLvoid WINAPI glColorMaterial(GLenum face, GLenum mode)
{
 if(lpglColorMaterial) (*lpglColorMaterial)(face, mode);
}

// Section 2.13.7
// Flatshading

typedef GLvoid (WINAPI *LPGLSHADEMODEL)(GLenum);
static LPGLSHADEMODEL lpglShadeModel;
GLvoid WINAPI glShadeModel(GLenum mode)
{
 if(lpglShadeModel) (*lpglShadeModel)(mode);
}

// Section 3.3
// Points

typedef GLvoid (WINAPI *LPGLPOINTSIZE)(GLfloat);
static LPGLPOINTSIZE lpglPointSize;
GLvoid WINAPI glPointSize(GLfloat size)
{
 if(lpglPointSize) (*lpglPointSize)(size);
}

// Section 3.4
// Line Segments

typedef GLvoid (WINAPI *LPGLLINEWIDTH)(GLfloat);
static LPGLLINEWIDTH lpglLineWidth;
GLvoid WINAPI glLineWidth(GLfloat width)
{
 if(lpglLineWidth) (*lpglLineWidth)(width);
}

typedef GLvoid (WINAPI *LPGLLINESTIPPLE)(GLint, GLushort);
static LPGLLINESTIPPLE lpglLineStipple;
GLvoid WINAPI glLineStipple(GLint factor, GLushort pattern)
{
 if(lpglLineStipple) (*lpglLineStipple)(factor, pattern);
}

// Section 3.5.1
// Basic Polygon Rasterization

typedef GLvoid (WINAPI *LPGLCULLFACE)(GLenum);
static LPGLCULLFACE lpglCullFace;
GLvoid WINAPI glCullFace(GLenum mode)
{
 if(lpglCullFace) (*lpglCullFace)(mode);
}

// Section 3.5.2
// Stippling

typedef GLvoid (WINAPI *LPGLPOLYGONSTIPPLE)(const GLubyte*);
static LPGLPOLYGONSTIPPLE lpglPolygonStipple;
GLvoid WINAPI glPolygonStipple(const GLubyte* pattern)
{
 if(lpglPolygonStipple) (*lpglPolygonStipple)(pattern);
}

// Section 3.5.4
// Options Controlling Polygon Rasterization

typedef GLvoid (WINAPI *LPGLPOLYGONMODE)(GLenum, GLenum);
static LPGLPOLYGONMODE lpglPolygonMode;
GLvoid WINAPI glPolygonMode(GLenum face, GLenum mode)
{
 if(lpglPolygonMode) (*lpglPolygonMode)(face, mode);
}

// Section 3.5.5
// Depth Offset

typedef GLvoid (WINAPI *LPGLPOLYGONOFFSET)(GLfloat, GLfloat);
static LPGLPOLYGONOFFSET lpglPolygonOffset;
GLvoid WINAPI glPolygonOffset(GLfloat factor, GLfloat units)
{
 if(lpglPolygonOffset) (*lpglPolygonOffset)(factor, units);
}

// Section 3.6.1
// Pixel Storage Modes

typedef GLvoid (WINAPI *LPGLPIXELSTOREI)(GLenum, GLint);
static LPGLPIXELSTOREI lpglPixelStorei;
GLvoid WINAPI glPixelStorei(GLenum pname, GLint param)
{
 if(lpglPixelStorei) (*lpglPixelStorei)(pname, param);
}

typedef GLvoid (WINAPI *LPGLPIXELSTOREF)(GLenum, GLfloat);
static LPGLPIXELSTOREF lpglPixelStoref;
GLvoid WINAPI glPixelStoref(GLenum pname, GLfloat param)
{
 if(lpglPixelStoref) (*lpglPixelStoref)(pname, param);
}

// Section 3.6.3
// Pixel Transfer Modes

typedef GLvoid (WINAPI *LPGLPIXELTRANSFERI)(GLenum, GLint);
static LPGLPIXELTRANSFERI lpglPixelTransferi;
GLvoid WINAPI glPixelTransferi(GLenum param, GLint value)
{
 if(lpglPixelTransferi) (*lpglPixelTransferi)(param, value);
}

typedef GLvoid (WINAPI *LPGLPIXELTRANSFERF)(GLenum, GLfloat);
static LPGLPIXELTRANSFERF lpglPixelTransferf;
GLvoid WINAPI glPixelTransferf(GLenum param, GLfloat value)
{
 if(lpglPixelTransferf) (*lpglPixelTransferf)(param, value);
}

typedef GLvoid (WINAPI *LPGLPIXELMAPUIV)(GLenum, GLsizei, const GLuint*);
static LPGLPIXELMAPUIV lpglPixelMapuiv;
GLvoid WINAPI glPixelMapuiv(GLenum map, GLsizei size, const GLuint* values)
{
 if(lpglPixelMapuiv) (*lpglPixelMapuiv)(map, size, values);
}

typedef GLvoid (WINAPI *LPGLPIXELMAPUSV)(GLenum, GLsizei, const GLushort*);
static LPGLPIXELMAPUSV lpglPixelMapusv;
GLvoid WINAPI glPixelMapusv(GLenum map, GLsizei size, const GLushort* values)
{
 if(lpglPixelMapusv) (*lpglPixelMapusv)(map, size, values);
}

typedef GLvoid (WINAPI *LPGLPIXELMAPFV)(GLenum, GLsizei, const GLfloat*);
static LPGLPIXELMAPFV lpglPixelMapfv;
GLvoid WINAPI glPixelMapfv(GLenum map, GLsizei size, const GLfloat* values)
{
 if(lpglPixelMapfv) (*lpglPixelMapfv)(map, size, values);
}

typedef GLvoid (WINAPI *LPGLCOLORTABLE)(GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid*);
static LPGLCOLORTABLE lpglColorTable;
GLvoid WINAPI glColorTable(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid* data)
{
 if(lpglColorTable) (*lpglColorTable)(target, internalformat, width, format, type, data);
}

typedef GLvoid (WINAPI *LPGLCOLORTABLEPARAMETERIV)(GLenum, GLenum, const GLint*);
static LPGLCOLORTABLEPARAMETERIV lpglColorTableParameteriv;
GLvoid WINAPI glColorTableParameteriv(GLenum target, GLenum pname, const GLint* params)
{
 if(lpglColorTableParameteriv) (*lpglColorTableParameteriv)(target, pname, params);
}

typedef GLvoid (WINAPI *LPGLCOLORTABLEPARAMETERFV)(GLenum, GLenum, const GLfloat*);
static LPGLCOLORTABLEPARAMETERFV lpglColorTableParameterfv;
GLvoid WINAPI glColorTableParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
 if(lpglColorTableParameterfv) (*lpglColorTableParameterfv)(target, pname, params);
}

typedef GLvoid (WINAPI *LPGLCOPYCOLORTABLE)(GLenum, GLenum, GLint, GLint, GLsizei);
static LPGLCOPYCOLORTABLE lpglCopyColorTable;
GLvoid WINAPI glCopyColorTable(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
 if(lpglCopyColorTable) (*lpglCopyColorTable)(target, internalformat, x, y, width);
}

typedef GLvoid (WINAPI *LPGLCOLORSUBTABLE)(GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
static LPGLCOLORSUBTABLE lpglColorSubTable;
GLvoid WINAPI glColorSubTable(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid* data)
{
 if(lpglColorSubTable) (*lpglColorSubTable)(target, start, count, format, type, data);
}

typedef GLvoid (WINAPI *LPGLCOPYCOLORSUBTABLE)(GLenum, GLsizei, GLint, GLint, GLsizei);
static LPGLCOPYCOLORSUBTABLE lpglCopyColorSubTable;
GLvoid WINAPI glCopyColorSubTable(GLenum target, GLsizei start, GLint x, GLint y, GLsizei count)
{
 if(lpglCopyColorSubTable) (*lpglCopyColorSubTable)(target, start, x, y, count);
}

typedef GLvoid (WINAPI *LPGLCONVOLUTIONFILTER2D)(GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
static LPGLCONVOLUTIONFILTER2D lpglConvolutionFilter2D;
GLvoid WINAPI glConvolutionFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* data)
{
 if(lpglConvolutionFilter2D) (*lpglConvolutionFilter2D)(target, internalformat, width, height, format, type, data);
}

typedef GLvoid (WINAPI *LPGLCONVOLUTIONPARAMETERIV)(GLenum, GLenum, const GLint*);
static LPGLCONVOLUTIONPARAMETERIV lpglConvolutionParameteriv;
GLvoid WINAPI glConvolutionParameteriv(GLenum target, GLenum pname, const GLint* params)
{
 if(lpglConvolutionParameteriv) (*lpglConvolutionParameteriv)(target, pname, params);
}

typedef GLvoid (WINAPI *LPGLCONVOLUTIONPARAMETERFV)(GLenum, GLenum, const GLfloat*);
static LPGLCONVOLUTIONPARAMETERFV lpglConvolutionParameterfv;
GLvoid WINAPI glConvolutionParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
 if(lpglConvolutionParameterfv) (*lpglConvolutionParameterfv)(target, pname, params);
}

typedef GLvoid (WINAPI *LPGLCONVOLUTIONFILTER1D)(GLenum, GLenum, GLsizei, GLenum, GLenum, const GLvoid*);
static LPGLCONVOLUTIONFILTER1D lpglConvolutionFilter1D;
GLvoid WINAPI glConvolutionFilter1D(GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid* data)
{
 if(lpglConvolutionFilter1D) (*lpglConvolutionFilter1D)(target, internalformat, width, format, type, data);
}

typedef GLvoid (WINAPI *LPGLSEPARABLEFILTER2D)(GLenum, GLenum, GLsizei, GLsizei, GLenum, GLenum, const GLvoid*, const GLvoid*);
static LPGLSEPARABLEFILTER2D lpglSeparableFilter2D;
GLvoid WINAPI glSeparableFilter2D(GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* row, const GLvoid* column)
{
 if(lpglSeparableFilter2D) (*lpglSeparableFilter2D)(target, internalformat, width, height, format, type, row, column);
}

typedef GLvoid (WINAPI *LPGLCOPYCONVOLUTIONFILTER2D)(GLenum, GLenum, GLint, GLint, GLsizei, GLsizei);
static LPGLCOPYCONVOLUTIONFILTER2D lpglCopyConvolutionFilter2D;
GLvoid WINAPI glCopyConvolutionFilter2D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height)
{
 if(lpglCopyConvolutionFilter2D) (*lpglCopyConvolutionFilter2D)(target, internalformat, x, y, width, height);
}

typedef GLvoid (WINAPI *LPGLCOPYCONVOLUTIONFILTER1D)(GLenum, GLenum, GLint, GLint, GLsizei);
static LPGLCOPYCONVOLUTIONFILTER1D lpglCopyConvolutionFilter1D;
GLvoid WINAPI glCopyConvolutionFilter1D(GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width)
{
 if(lpglCopyConvolutionFilter1D) (*lpglCopyConvolutionFilter1D)(target, internalformat, x, y, width);
}

typedef GLvoid (WINAPI *LPGLHISTOGRAM)(GLenum, GLsizei, GLenum, GLboolean);
static LPGLHISTOGRAM lpglHistogram;
GLvoid WINAPI glHistogram(GLenum target, GLsizei width, GLenum internalformat, GLboolean sink)
{
 if(lpglHistogram) (*lpglHistogram)(target, width, internalformat, sink);
}

typedef GLvoid (WINAPI *LPGLMINMAX)(GLenum, GLenum, GLboolean);
static LPGLMINMAX lpglMinmax;
GLvoid WINAPI glMinmax(GLenum target, GLenum internalformat, GLboolean sink)
{
 if(lpglMinmax) (*lpglMinmax)(target, internalformat, sink);
}

// Section 3.6.4
// Rasterization of Pixel Rectangles

typedef GLvoid (WINAPI *LPGLDRAWPIXELS)(GLsizei, GLsizei, GLenum, GLenum, const GLvoid*);
static LPGLDRAWPIXELS lpglDrawPixels;
GLvoid WINAPI glDrawPixels(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid* data)
{
 if(lpglDrawPixels) (*lpglDrawPixels)(width, height, format, type, data);
}

typedef GLvoid (WINAPI *LPGLPIXELZOOM)(GLfloat, GLfloat);
static LPGLPIXELZOOM lpglPixelZoom;
GLvoid WINAPI glPixelZoom(GLfloat zx, GLfloat zy)
{
 if(lpglPixelZoom) (*lpglPixelZoom)(zx, zy);
}

// Section 3.6.5
// Pixel Transfer Operations

typedef GLvoid (WINAPI *LPGLCONVOLUTIONPARAMETERI)(GLenum, GLenum, GLint);
static LPGLCONVOLUTIONPARAMETERI lpglConvolutionParameteri;
GLvoid WINAPI glConvolutionParameteri(GLenum target, GLenum pname, GLint param)
{
 if(lpglConvolutionParameteri) (*lpglConvolutionParameteri)(target, pname, param);
}

typedef GLvoid (WINAPI *LPGLCONVOLUTIONPARAMETERF)(GLenum, GLenum, GLfloat);
static LPGLCONVOLUTIONPARAMETERF lpglConvolutionParameterf;
GLvoid WINAPI glConvolutionParameterf(GLenum target, GLenum pname, GLfloat param)
{
 if(lpglConvolutionParameterf) (*lpglConvolutionParameterf)(target, pname, param);
}

// Section 3.7
// Bitmaps

typedef GLvoid (WINAPI *LPGLBITMAP)(GLsizei, GLsizei, GLfloat, GLfloat, GLfloat, GLfloat, const GLubyte*);
static LPGLBITMAP lpglBitmap;
GLvoid WINAPI glBitmap(GLsizei w, GLsizei h, GLfloat xbo, GLfloat ybo, GLfloat xbi, GLfloat ybi, const GLubyte* data)
{
 if(lpglBitmap) (*lpglBitmap)(w, h, xbo, ybo, xbi, ybi, data);
}

// Section 3.8.1
// Texture Image Specification

typedef GLvoid (WINAPI *LPGLTEXIMAGE3D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
static LPGLTEXIMAGE3D lpglTexImage3D;
GLvoid WINAPI glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* data)
{
 if(lpglTexImage3D) (*lpglTexImage3D)(target, level, internalformat, width, height, depth, border, format, type, data);
}

typedef GLvoid (WINAPI *LPGLTEXIMAGE2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
static LPGLTEXIMAGE2D lpglTexImage2D;
GLvoid WINAPI glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data)
{
 if(lpglTexImage2D) (*lpglTexImage2D)(target, level, internalformat, width, height, border, format, type, data);
}

typedef GLvoid (WINAPI *LPGLTEXIMAGE1D)(GLenum, GLint, GLint, GLsizei, GLint, GLenum, GLenum, const GLvoid*);
static LPGLTEXIMAGE1D lpglTexImage1D;
GLvoid WINAPI glTexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid* data)
{
 if(lpglTexImage1D) (*lpglTexImage1D)(target, level, internalformat, width, border, format, type, data);
}

// Section 3.8.2
// Alternate Texture Image Specification Commands

typedef GLvoid (WINAPI *LPGLCOPYTEXIMAGE2D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLsizei, GLint);
static LPGLCOPYTEXIMAGE2D lpglCopyTexImage2D;
GLvoid WINAPI glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
 if(lpglCopyTexImage2D) (*lpglCopyTexImage2D)(target, level, internalformat, x, y, width, height, border);
}

typedef GLvoid (WINAPI *LPGLCOPYTEXIMAGE1D)(GLenum, GLint, GLenum, GLint, GLint, GLsizei, GLint);
static LPGLCOPYTEXIMAGE1D lpglCopyTexImage1D;
GLvoid WINAPI glCopyTexImage1D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)
{
 if(lpglCopyTexImage1D) (*lpglCopyTexImage1D)(target, level, internalformat, x, y, width, border);
}

typedef GLvoid (WINAPI *LPGLTEXSUBIMAGE3D)(GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei, GLenum, GLenum, GLvoid*);
static LPGLTEXSUBIMAGE3D lpglTexSubImage3D;
GLvoid WINAPI glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, GLvoid* data)
{
 if(lpglTexSubImage3D) (*lpglTexSubImage3D)(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data);
}

typedef GLvoid (WINAPI *LPGLTEXSUBIMAGE2D)(GLenum, GLint, GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*);
static LPGLTEXSUBIMAGE2D lpglTexSubImage2D;
GLvoid WINAPI glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* data)
{
 if(lpglTexSubImage2D) (*lpglTexSubImage2D)(target, level, xoffset, yoffset, width, height, format, type, data);
}

typedef GLvoid (WINAPI *LPGLTEXSUBIMAGE1D)(GLenum, GLint, GLint, GLsizei, GLenum, GLenum, const GLvoid*);
static LPGLTEXSUBIMAGE1D lpglTexSubImage1D;
GLvoid WINAPI glTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid* data)
{
 if(lpglTexSubImage1D) (*lpglTexSubImage1D)(target, level, xoffset, width, format, type, data);
}

typedef GLvoid (WINAPI *LPGLCOPYTEXSUBIMAGE3D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
static LPGLCOPYTEXSUBIMAGE3D lpglCopyTexSubImage3D;
GLvoid WINAPI glCopyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
 if(lpglCopyTexSubImage3D) (*lpglCopyTexSubImage3D)(target, level, xoffset, yoffset, zoffset, x, y, width, height);
}

typedef GLvoid (WINAPI *LPGLCOPYTEXSUBIMAGE2D)(GLenum, GLint, GLint, GLint, GLint, GLint, GLsizei, GLsizei);
static LPGLCOPYTEXSUBIMAGE2D lpglCopyTexSubImage2D;
GLvoid WINAPI glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
 if(lpglCopyTexSubImage2D) (*lpglCopyTexSubImage2D)(target, level, xoffset, yoffset, x, y, width, height);
}

typedef GLvoid (WINAPI *LPGLCOPYTEXSUBIMAGE1D)(GLenum, GLint, GLint, GLint, GLint, GLsizei);
static LPGLCOPYTEXSUBIMAGE1D lpglCopyTexSubImage1D;
GLvoid WINAPI glCopyTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
 if(lpglCopyTexSubImage1D) (*lpglCopyTexSubImage1D)(target, level, xoffset, x, y, width);
}

// Section 3.8.3
// Texture Parameters

typedef GLvoid (WINAPI *LPGLTEXPARAMETERI)(GLenum, GLenum, GLint);
static LPGLTEXPARAMETERI lpglTexParameteri;
GLvoid WINAPI glTexParameteri(GLenum target, GLenum pname, GLint param)
{
 if(lpglTexParameteri) (*lpglTexParameteri)(target, pname, param);
}

typedef GLvoid (WINAPI *LPGLTEXPARAMETERF)(GLenum, GLenum, GLfloat);
static LPGLTEXPARAMETERF lpglTexParameterf;
GLvoid WINAPI glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
 if(lpglTexParameterf) (*lpglTexParameterf)(target, pname, param);
}

typedef GLvoid (WINAPI *LPGLTEXPARAMETERIV)(GLenum, GLenum, const GLint*);
static LPGLTEXPARAMETERIV lpglTexParameteriv;
GLvoid WINAPI glTexParameteriv(GLenum target, GLenum pname, const GLint* params)
{
 if(lpglTexParameteriv) (*lpglTexParameteriv)(target, pname, params);
}

typedef GLvoid (WINAPI *LPGLTEXPARAMETERFV)(GLenum, GLenum, const GLfloat*);
static LPGLTEXPARAMETERFV lpglTexParameterfv;
GLvoid WINAPI glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
 if(lpglTexParameterfv) (*lpglTexParameterfv)(target, pname, params);
}

// Section 3.8.8
// Texture Objects

typedef GLvoid (WINAPI *LPGLBINDTEXTURE)(GLenum, GLuint);
static LPGLBINDTEXTURE lpglBindTexture;
GLvoid WINAPI glBindTexture(GLenum target, GLuint texture)
{
 if(lpglBindTexture) (*lpglBindTexture)(target, texture);
}

typedef GLvoid (WINAPI *LPGLDELETETEXTURES)(GLsizei, const GLuint*);
static LPGLDELETETEXTURES lpglDeleteTextures;
GLvoid WINAPI glDeleteTextures(GLsizei n, const GLuint* textures)
{
 if(lpglDeleteTextures) (*lpglDeleteTextures)(n, textures);
}

typedef GLvoid (WINAPI *LPGLGENTEXTURES)(GLsizei, GLuint*);
static LPGLGENTEXTURES lpglGenTextures;
GLvoid WINAPI glGenTextures(GLsizei n, GLuint* textures)
{
 if(lpglGenTextures) (*lpglGenTextures)(n, textures);
}

typedef GLboolean (WINAPI *LPGLARETEXTURESRESIDENT)(GLsizei, const GLuint*, GLboolean*);
static LPGLARETEXTURESRESIDENT lpglAreTexturesResident;
GLboolean WINAPI glAreTexturesResident(GLsizei n, const GLuint* textures, GLboolean* residences)
{
 if(lpglAreTexturesResident) return (*lpglAreTexturesResident)(n, textures, residences);
 return (GLboolean)0;
}

typedef GLvoid (WINAPI *LPGLPRIORITIZETEXTURES)(GLsizei, const GLuint*, const GLclampf*);
static LPGLPRIORITIZETEXTURES lpglPrioritizeTextures;
GLvoid WINAPI glPrioritizeTextures(GLsizei n, const GLuint* textures, const GLclampf* priorities)
{
 if(lpglPrioritizeTextures) (*lpglPrioritizeTextures)(n, textures, priorities);
}

// Section 3.8.9
// Texture Environments and Texture Functions

typedef GLvoid (WINAPI *LPGLTEXENVI)(GLenum, GLenum, GLint);
static LPGLTEXENVI lpglTexEnvi;
GLvoid WINAPI glTexEnvi(GLenum target, GLenum pname, GLint param)
{
 if(lpglTexEnvi) (*lpglTexEnvi)(target, pname, param);
}

typedef GLvoid (WINAPI *LPGLTEXENVF)(GLenum, GLenum, GLfloat);
static LPGLTEXENVF lpglTexEnvf;
GLvoid WINAPI glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
 if(lpglTexEnvf) (*lpglTexEnvf)(target, pname, param);
}

typedef GLvoid (WINAPI *LPGLTEXENVIV)(GLenum, GLenum, const GLint*);
static LPGLTEXENVIV lpglTexEnviv;
GLvoid WINAPI glTexEnviv(GLenum target, GLenum pname, const GLint* params)
{
 if(lpglTexEnviv) (*lpglTexEnviv)(target, pname, params);
}

typedef GLvoid (WINAPI *LPGLTEXENVFV)(GLenum, GLenum, const GLfloat*);
static LPGLTEXENVFV lpglTexEnvfv;
GLvoid WINAPI glTexEnvfv(GLenum target, GLenum pname, const GLfloat* params)
{
 if(lpglTexEnvfv) (*lpglTexEnvfv)(target, pname, params);
}

// Section 3.10
// Fog

typedef GLvoid (WINAPI *LPGLFOGI)(GLenum, GLint);
static LPGLFOGI lpglFogi;
GLvoid WINAPI glFogi(GLenum pname, GLint param)
{
 if(lpglFogi) (*lpglFogi)(pname, param);
}

typedef GLvoid (WINAPI *LPGLFOGF)(GLenum, GLfloat);
static LPGLFOGF lpglFogf;
GLvoid WINAPI glFogf(GLenum pname, GLfloat param)
{
 if(lpglFogf) (*lpglFogf)(pname, param);
}

typedef GLvoid (WINAPI *LPGLFOGIV)(GLenum, const GLint*);
static LPGLFOGIV lpglFogiv;
GLvoid WINAPI glFogiv(GLenum pname, const GLint* params)
{
 if(lpglFogiv) (*lpglFogiv)(pname, params);
}

typedef GLvoid (WINAPI *LPGLFOGFV)(GLenum, const GLfloat*);
static LPGLFOGFV lpglFogfv;
GLvoid WINAPI glFogfv(GLenum pname, const GLfloat* params)
{
 if(lpglFogfv) (*lpglFogfv)(pname, params);
}

// Section 4.1.2
// Scissor Test

typedef GLvoid (WINAPI *LPGLSCISSOR)(GLint, GLint, GLsizei, GLsizei);
static LPGLSCISSOR lpglScissor;
GLvoid WINAPI glScissor(GLint left, GLint bottom, GLsizei width, GLsizei height)
{
 if(lpglScissor) (*lpglScissor)(left, bottom, width, height);
}

// Section 4.1.3
// Alpha Test

typedef GLvoid (WINAPI *LPGLALPHAFUNC)(GLenum, GLclampf);
static LPGLALPHAFUNC lpglAlphaFunc;
GLvoid WINAPI glAlphaFunc(GLenum func, GLclampf ref)
{
 if(lpglAlphaFunc) (*lpglAlphaFunc)(func, ref);
}

// Section 4.1.4
// Stencil Test

typedef GLvoid (WINAPI *LPGLSTENCILFUNC)(GLenum, GLint, GLuint);
static LPGLSTENCILFUNC lpglStencilFunc;
GLvoid WINAPI glStencilFunc(GLenum func, GLint ref, GLuint mask)
{
 if(lpglStencilFunc) (*lpglStencilFunc)(func, ref, mask);
}

typedef GLvoid (WINAPI *LPGLSTENCILOP)(GLenum, GLenum, GLenum);
static LPGLSTENCILOP lpglStencilOp;
GLvoid WINAPI glStencilOp(GLenum sfail, GLenum dpfail, GLenum dppass)
{
 if(lpglStencilOp) (*lpglStencilOp)(sfail, dpfail, dppass);
}

// Section 4.1.5
// Depth Buffer Test

typedef GLvoid (WINAPI *LPGLDEPTHFUNC)(GLenum);
static LPGLDEPTHFUNC lpglDepthFunc;
GLvoid WINAPI glDepthFunc(GLenum func)
{
 if(lpglDepthFunc) (*lpglDepthFunc)(func);
}

// Section 4.1.6
// Blending

typedef GLvoid (WINAPI *LPGLBLENDCOLOR)(GLclampf, GLclampf, GLclampf, GLclampf);
static LPGLBLENDCOLOR lpglBlendColor;
GLvoid WINAPI glBlendColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
 if(lpglBlendColor) (*lpglBlendColor)(red, green, blue, alpha);
}

typedef GLvoid (WINAPI *LPGLBLENDEQUATION)(GLenum);
static LPGLBLENDEQUATION lpglBlendEquation;
GLvoid WINAPI glBlendEquation(GLenum mode)
{
 if(lpglBlendEquation) (*lpglBlendEquation)(mode);
}

typedef GLvoid (WINAPI *LPGLBLENDFUNC)(GLenum, GLenum);
static LPGLBLENDFUNC lpglBlendFunc;
GLvoid WINAPI glBlendFunc(GLenum src, GLenum dst)
{
 if(lpglBlendFunc) (*lpglBlendFunc)(src, dst);
}

// Section 4.1.8
// Logical Operation

typedef GLvoid (WINAPI *LPGLLOGICOP)(GLenum);
static LPGLLOGICOP lpglLogicOp;
GLvoid WINAPI glLogicOp(GLenum op)
{
 if(lpglLogicOp) (*lpglLogicOp)(op);
}

// Section 4.2.1
// Selecting a Buffer for Writing

typedef GLvoid (WINAPI *LPGLDRAWBUFFER)(GLenum);
static LPGLDRAWBUFFER lpglDrawBuffer;
GLvoid WINAPI glDrawBuffer(GLenum buf)
{
 if(lpglDrawBuffer) (*lpglDrawBuffer)(buf);
}

// Section 4.2.2
// Fine Control of Buffer Updates

typedef GLvoid (WINAPI *LPGLINDEXMASK)(GLuint);
static LPGLINDEXMASK lpglIndexMask;
GLvoid WINAPI glIndexMask(GLuint mask)
{
 if(lpglIndexMask) (*lpglIndexMask)(mask);
}

typedef GLvoid (WINAPI *LPGLCOLORMASK)(GLboolean, GLboolean, GLboolean, GLboolean);
static LPGLCOLORMASK lpglColorMask;
GLvoid WINAPI glColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a)
{
 if(lpglColorMask) (*lpglColorMask)(r, g, b, a);
}

typedef GLvoid (WINAPI *LPGLDEPTHMASK)(GLboolean);
static LPGLDEPTHMASK lpglDepthMask;
GLvoid WINAPI glDepthMask(GLboolean mask)
{
 if(lpglDepthMask) (*lpglDepthMask)(mask);
}

typedef GLvoid (WINAPI *LPGLSTENCILMASK)(GLuint);
static LPGLSTENCILMASK lpglStencilMask;
GLvoid WINAPI glStencilMask(GLuint mask)
{
 if(lpglStencilMask) (*lpglStencilMask)(mask);
}

// Section 4.2.3
// Clearing the Buffers

typedef GLvoid (WINAPI *LPGLCLEAR)(GLbitfield);
static LPGLCLEAR lpglClear;
GLvoid WINAPI glClear(GLbitfield buf)
{
 if(lpglClear) (*lpglClear)(buf);
}

typedef GLvoid (WINAPI *LPGLCLEARCOLOR)(GLclampf, GLclampf, GLclampf, GLclampf);
static LPGLCLEARCOLOR lpglClearColor;
GLvoid WINAPI glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a)
{
 if(lpglClearColor) (*lpglClearColor)(r, g, b, a);
}

typedef GLvoid (WINAPI *LPGLCLEARINDEX)(GLfloat);
static LPGLCLEARINDEX lpglClearIndex;
GLvoid WINAPI glClearIndex(GLfloat index)
{
 if(lpglClearIndex) (*lpglClearIndex)(index);
}

typedef GLvoid (WINAPI *LPGLCLEARDEPTH)(GLclampd);
static LPGLCLEARDEPTH lpglClearDepth;
GLvoid WINAPI glClearDepth(GLclampd d)
{
 if(lpglClearDepth) (*lpglClearDepth)(d);
}

typedef GLvoid (WINAPI *LPGLCLEARSTENCIL)(GLint);
static LPGLCLEARSTENCIL lpglClearStencil;
GLvoid WINAPI glClearStencil(GLint s)
{
 if(lpglClearStencil) (*lpglClearStencil)(s);
}

typedef GLvoid (WINAPI *LPGLCLEARACCUM)(GLfloat, GLfloat, GLfloat, GLfloat);
static LPGLCLEARACCUM lpglClearAccum;
GLvoid WINAPI glClearAccum(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
 if(lpglClearAccum) (*lpglClearAccum)(r, g, b, a);
}

// Section 4.2.4
// The Accumulation Buffer

typedef GLvoid (WINAPI *LPGLACCUM)(GLenum, GLfloat);
static LPGLACCUM lpglAccum;
GLvoid WINAPI glAccum(GLenum op, GLfloat value)
{
 if(lpglAccum) (*lpglAccum)(op, value);
}

// Section 4.3.2
// Reading Pixels

typedef GLvoid (WINAPI *LPGLREADPIXELS)(GLint, GLint, GLsizei, GLsizei, GLenum, GLenum, GLvoid*);
static LPGLREADPIXELS lpglReadPixels;
GLvoid WINAPI glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* data)
{
 if(lpglReadPixels) (*lpglReadPixels)(x, y, width, height, format, type, data);
}

typedef GLvoid (WINAPI *LPGLREADBUFFER)(GLenum);
static LPGLREADBUFFER lpglReadBuffer;
GLvoid WINAPI glReadBuffer(GLenum src)
{
 if(lpglReadBuffer) (*lpglReadBuffer)(src);
}

// Section 4.3.3
// Copying Pixels

typedef GLvoid (WINAPI *LPGLCOPYPIXELS)(GLint, GLint, GLsizei, GLsizei, GLenum);
static LPGLCOPYPIXELS lpglCopyPixels;
GLvoid WINAPI glCopyPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
 if(lpglCopyPixels) (*lpglCopyPixels)(x, y, width, height, type);
}

// Section 5.1
// Evaluators

typedef GLvoid (WINAPI *LPGLMAP1F)(GLenum, GLfloat, GLfloat, GLint, GLint, const GLfloat*);
static LPGLMAP1F lpglMap1f;
GLvoid WINAPI glMap1f(GLenum type, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat* points)
{
 if(lpglMap1f) (*lpglMap1f)(type, u1, u2, stride, order, points);
}

typedef GLvoid (WINAPI *LPGLMAP1D)(GLenum, GLdouble, GLdouble, GLint, GLint, const GLdouble*);
static LPGLMAP1D lpglMap1d;
GLvoid WINAPI glMap1d(GLenum type, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble* points)
{
 if(lpglMap1d) (*lpglMap1d)(type, u1, u2, stride, order, points);
}

typedef GLvoid (WINAPI *LPGLMAP2F)(GLenum, GLfloat, GLfloat, GLint, GLint, GLfloat, GLfloat, GLint, GLint, const GLfloat*);
static LPGLMAP2F lpglMap2f;
GLvoid WINAPI glMap2f(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat* points)
{
 if(lpglMap2f) (*lpglMap2f)(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
}

typedef GLvoid (WINAPI *LPGLMAP2D)(GLenum, GLdouble, GLdouble, GLint, GLint, GLdouble, GLdouble, GLint, GLint, const GLdouble*);
static LPGLMAP2D lpglMap2d;
GLvoid WINAPI glMap2d(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble* points)
{
 if(lpglMap2d) (*lpglMap2d)(target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points);
}

typedef GLvoid (WINAPI *LPGLEVALCOORD1F)(GLfloat);
static LPGLEVALCOORD1F lpglEvalCoord1f;
GLvoid WINAPI glEvalCoord1f(GLfloat u)
{
 if(lpglEvalCoord1f) (*lpglEvalCoord1f)(u);
}

typedef GLvoid (WINAPI *LPGLEVALCOORD1D)(GLdouble);
static LPGLEVALCOORD1D lpglEvalCoord1d;
GLvoid WINAPI glEvalCoord1d(GLdouble u)
{
 if(lpglEvalCoord1d) (*lpglEvalCoord1d)(u);
}

typedef GLvoid (WINAPI *LPGLEVALCOORD2F)(GLfloat, GLfloat);
static LPGLEVALCOORD2F lpglEvalCoord2f;
GLvoid WINAPI glEvalCoord2f(GLfloat u, GLfloat v)
{
 if(lpglEvalCoord2f) (*lpglEvalCoord2f)(u, v);
}

typedef GLvoid (WINAPI *LPGLEVALCOORD2D)(GLdouble, GLdouble);
static LPGLEVALCOORD2D lpglEvalCoord2d;
GLvoid WINAPI glEvalCoord2d(GLdouble u, GLdouble v)
{
 if(lpglEvalCoord2d) (*lpglEvalCoord2d)(u, v);
}

typedef GLvoid (WINAPI *LPGLEVALCOORD1FV)(const GLfloat*);
static LPGLEVALCOORD1FV lpglEvalCoord1fv;
GLvoid WINAPI glEvalCoord1fv(const GLfloat* arg)
{
 if(lpglEvalCoord1fv) (*lpglEvalCoord1fv)(arg);
}

typedef GLvoid (WINAPI *LPGLEVALCOORD1DV)(const GLdouble*);
static LPGLEVALCOORD1DV lpglEvalCoord1dv;
GLvoid WINAPI glEvalCoord1dv(const GLdouble* arg)
{
 if(lpglEvalCoord1dv) (*lpglEvalCoord1dv)(arg);
}

typedef GLvoid (WINAPI *LPGLEVALCOORD2FV)(const GLfloat*);
static LPGLEVALCOORD2FV lpglEvalCoord2fv;
GLvoid WINAPI glEvalCoord2fv(const GLfloat* arg)
{
 if(lpglEvalCoord2fv) (*lpglEvalCoord2fv)(arg);
}

typedef GLvoid (WINAPI *LPGLEVALCOORD2DV)(const GLdouble*);
static LPGLEVALCOORD2DV lpglEvalCoord2dv;
GLvoid WINAPI glEvalCoord2dv(const GLdouble* arg)
{
 if(lpglEvalCoord2dv) (*lpglEvalCoord2dv)(arg);
}

typedef GLvoid (WINAPI *LPGLMAPGRID1F)(GLint, GLfloat, GLfloat);
static LPGLMAPGRID1F lpglMapGrid1f;
GLvoid WINAPI glMapGrid1f(GLint n, GLfloat u1, GLfloat u2)
{
 if(lpglMapGrid1f) (*lpglMapGrid1f)(n, u1, u2);
}

typedef GLvoid (WINAPI *LPGLMAPGRID1D)(GLint, GLdouble, GLdouble);
static LPGLMAPGRID1D lpglMapGrid1d;
GLvoid WINAPI glMapGrid1d(GLint n, GLdouble u1, GLdouble u2)
{
 if(lpglMapGrid1d) (*lpglMapGrid1d)(n, u1, u2);
}

typedef GLvoid (WINAPI *LPGLMAPGRID2F)(GLint, GLfloat, GLfloat, GLint, GLfloat, GLfloat);
static LPGLMAPGRID2F lpglMapGrid2f;
GLvoid WINAPI glMapGrid2f(GLint nu, GLfloat u1, GLfloat u2, GLint nv, GLfloat v1, GLfloat v2)
{
 if(lpglMapGrid2f) (*lpglMapGrid2f)(nu, u1, u2, nv, v1, v2);
}

typedef GLvoid (WINAPI *LPGLMAPGRID2D)(GLint, GLdouble, GLdouble, GLint, GLdouble, GLdouble);
static LPGLMAPGRID2D lpglMapGrid2d;
GLvoid WINAPI glMapGrid2d(GLint nu, GLdouble u1, GLdouble u2, GLint nv, GLdouble v1, GLdouble v2)
{
 if(lpglMapGrid2d) (*lpglMapGrid2d)(nu, u1, u2, nv, v1, v2);
}

typedef GLvoid (WINAPI *LPGLEVALMESH1)(GLenum, GLint, GLint);
static LPGLEVALMESH1 lpglEvalMesh1;
GLvoid WINAPI glEvalMesh1(GLenum mode, GLint p1, GLint p2)
{
 if(lpglEvalMesh1) (*lpglEvalMesh1)(mode, p1, p2);
}

typedef GLvoid (WINAPI *LPGLEVALMESH2)(GLenum, GLint, GLint, GLint, GLint);
static LPGLEVALMESH2 lpglEvalMesh2;
GLvoid WINAPI glEvalMesh2(GLenum mode, GLint p1, GLint p2, GLint q1, GLint q2)
{
 if(lpglEvalMesh2) (*lpglEvalMesh2)(mode, p1, p2, q1, q2);
}

typedef GLvoid (WINAPI *LPGLEVALPOINT1)(GLint);
static LPGLEVALPOINT1 lpglEvalPoint1;
GLvoid WINAPI glEvalPoint1(GLint p)
{
 if(lpglEvalPoint1) (*lpglEvalPoint1)(p);
}

typedef GLvoid (WINAPI *LPGLEVALPOINT2)(GLint, GLint);
static LPGLEVALPOINT2 lpglEvalPoint2;
GLvoid WINAPI glEvalPoint2(GLint p, GLint q)
{
 if(lpglEvalPoint2) (*lpglEvalPoint2)(p, q);
}

// Section 5.2
// Selection

typedef GLvoid (WINAPI *LPGLINITNAMES)(GLvoid);
static LPGLINITNAMES lpglInitNames;
GLvoid WINAPI glInitNames(GLvoid)
{
 if(lpglInitNames) (*lpglInitNames)();
}

typedef GLvoid (WINAPI *LPGLPOPNAME)(GLvoid);
static LPGLPOPNAME lpglPopName;
GLvoid WINAPI glPopName(GLvoid)
{
 if(lpglPopName) (*lpglPopName)();
}

typedef GLvoid (WINAPI *LPGLPUSHNAME)(GLuint);
static LPGLPUSHNAME lpglPushName;
GLvoid WINAPI glPushName(GLuint name)
{
 if(lpglPushName) (*lpglPushName)(name);
}

typedef GLvoid (WINAPI *LPGLLOADNAME)(GLuint);
static LPGLLOADNAME lpglLoadName;
GLvoid WINAPI glLoadName(GLuint name)
{
 if(lpglLoadName) (*lpglLoadName)(name);
}

typedef GLint (WINAPI *LPGLRENDERMODE)(GLenum);
static LPGLRENDERMODE lpglRenderMode;
GLint WINAPI glRenderMode(GLenum mode)
{
 if(lpglRenderMode) return (*lpglRenderMode)(mode);
 return (GLint)0;
}

typedef GLvoid (WINAPI *LPGLSELECTBUFFER)(GLsizei, GLuint*);
static LPGLSELECTBUFFER lpglSelectBuffer;
GLvoid WINAPI glSelectBuffer(GLsizei n, GLuint* buffer)
{
 if(lpglSelectBuffer) (*lpglSelectBuffer)(n, buffer);
}

// Section 5.3
// Feedback

typedef GLvoid (WINAPI *LPGLFEEDBACKBUFFER)(GLsizei, GLenum, GLfloat*);
static LPGLFEEDBACKBUFFER lpglFeedbackBuffer;
GLvoid WINAPI glFeedbackBuffer(GLsizei n, GLenum type, GLfloat* buffer)
{
 if(lpglFeedbackBuffer) (*lpglFeedbackBuffer)(n, type, buffer);
}

typedef GLvoid (WINAPI *LPGLPASSTHROUGH)(GLfloat);
static LPGLPASSTHROUGH lpglPassThrough;
GLvoid WINAPI glPassThrough(GLfloat token)
{
 if(lpglPassThrough) (*lpglPassThrough)(token);
}

// Section 5.4
// Display Lists

typedef GLvoid (WINAPI *LPGLNEWLIST)(GLuint, GLenum);
static LPGLNEWLIST lpglNewList;
GLvoid WINAPI glNewList(GLuint n, GLenum mode)
{
 if(lpglNewList) (*lpglNewList)(n, mode);
}

typedef GLvoid (WINAPI *LPGLENDLIST)(GLvoid);
static LPGLENDLIST lpglEndList;
GLvoid WINAPI glEndList(GLvoid)
{
 if(lpglEndList) (*lpglEndList)();
}

typedef GLvoid (WINAPI *LPGLCALLLIST)(GLuint);
static LPGLCALLLIST lpglCallList;
GLvoid WINAPI glCallList(GLuint n)
{
 if(lpglCallList) (*lpglCallList)(n);
}

typedef GLvoid (WINAPI *LPGLCALLLISTS)(GLsizei, GLenum, const GLvoid*);
static LPGLCALLLISTS lpglCallLists;
GLvoid WINAPI glCallLists(GLsizei n, GLenum type, const GLvoid* lists)
{
 if(lpglCallLists) (*lpglCallLists)(n, type, lists);
}

typedef GLvoid (WINAPI *LPGLLISTBASE)(GLuint);
static LPGLLISTBASE lpglListBase;
GLvoid WINAPI glListBase(GLuint base)
{
 if(lpglListBase) (*lpglListBase)(base);
}

typedef GLuint (WINAPI *LPGLGENLISTS)(GLsizei);
static LPGLGENLISTS lpglGenLists;
GLuint WINAPI glGenLists(GLsizei s)
{
 if(lpglGenLists) return (*lpglGenLists)(s);
 return (GLuint)0;
}

typedef GLboolean (WINAPI *LPGLISLIST)(GLuint);
static LPGLISLIST lpglIsList;
GLboolean WINAPI glIsList(GLuint list)
{
 if(lpglIsList) return (*lpglIsList)(list);
 return (GLboolean)0;
}

typedef GLvoid (WINAPI *LPGLDELETELISTS)(GLuint, GLsizei);
static LPGLDELETELISTS lpglDeleteLists;
GLvoid WINAPI glDeleteLists(GLuint list, GLsizei range)
{
 if(lpglDeleteLists) (*lpglDeleteLists)(list, range);
}

// Section 5.5
// Flush and Finish

typedef GLvoid (WINAPI *LPGLFLUSH)(GLvoid);
static LPGLFLUSH lpglFlush;
GLvoid WINAPI glFlush(GLvoid)
{
 if(lpglFlush) (*lpglFlush)();
}

typedef GLvoid (WINAPI *LPGLFINISH)(GLvoid);
static LPGLFINISH lpglFinish;
GLvoid WINAPI glFinish(GLvoid)
{
 if(lpglFinish) (*lpglFinish)();
}

// Section 5.6
// Hints

typedef GLvoid (WINAPI *LPGLHINT)(GLenum, GLenum);
static LPGLHINT lpglHint;
GLvoid WINAPI glHint(GLenum target, GLenum hint)
{
 if(lpglHint) (*lpglHint)(target, hint);
}

// Section 6.1.1
// Simple Queries

typedef GLvoid (WINAPI *LPGLGETBOOLEANV)(GLenum, GLboolean*);
static LPGLGETBOOLEANV lpglGetBooleanv;
GLvoid WINAPI glGetBooleanv(GLenum value, GLboolean* data)
{
 if(lpglGetBooleanv) (*lpglGetBooleanv)(value, data);
}

typedef GLvoid (WINAPI *LPGLGETINTEGERV)(GLenum, GLint*);
static LPGLGETINTEGERV lpglGetIntegerv;
GLvoid WINAPI glGetIntegerv(GLenum value, GLint* data)
{
 if(lpglGetIntegerv) (*lpglGetIntegerv)(value, data);
}

typedef GLvoid (WINAPI *LPGLGETFLOATV)(GLenum, GLfloat*);
static LPGLGETFLOATV lpglGetFloatv;
GLvoid WINAPI glGetFloatv(GLenum value, GLfloat* data)
{
 if(lpglGetFloatv) (*lpglGetFloatv)(value, data);
}

typedef GLvoid (WINAPI *LPGLGETDOUBLEV)(GLenum, GLdouble*);
static LPGLGETDOUBLEV lpglGetDoublev;
GLvoid WINAPI glGetDoublev(GLenum value, GLdouble* data)
{
 if(lpglGetDoublev) (*lpglGetDoublev)(value, data);
}

typedef GLboolean (WINAPI *LPGLISENABLED)(GLenum);
static LPGLISENABLED lpglIsEnabled;
GLboolean WINAPI glIsEnabled(GLenum value)
{
 if(lpglIsEnabled) return (*lpglIsEnabled)(value);
 return (GLboolean)0;
}

// Section 6.1.3
// Enumerated Queries

typedef GLvoid (WINAPI *LPGLGETCLIPPLANE)(GLenum, GLdouble);
static LPGLGETCLIPPLANE lpglGetClipPlane;
GLvoid WINAPI glGetClipPlane(GLenum plane, GLdouble eqn[4])
{
 if(lpglGetClipPlane) (*lpglGetClipPlane)(plane, eqn[4]);
}

typedef GLvoid (WINAPI *LPGLGETLIGHTIV)(GLenum, GLenum, GLint*);
static LPGLGETLIGHTIV lpglGetLightiv;
GLvoid WINAPI glGetLightiv(GLenum light, GLenum value, GLint* data)
{
 if(lpglGetLightiv) (*lpglGetLightiv)(light, value, data);
}

typedef GLvoid (WINAPI *LPGLGETLIGHTFV)(GLenum, GLenum, GLfloat*);
static LPGLGETLIGHTFV lpglGetLightfv;
GLvoid WINAPI glGetLightfv(GLenum light, GLenum value, GLfloat* data)
{
 if(lpglGetLightfv) (*lpglGetLightfv)(light, value, data);
}

typedef GLvoid (WINAPI *LPGLGETMATERIALIV)(GLenum, GLenum, GLint*);
static LPGLGETMATERIALIV lpglGetMaterialiv;
GLvoid WINAPI glGetMaterialiv(GLenum face, GLenum value, GLint* data)
{
 if(lpglGetMaterialiv) (*lpglGetMaterialiv)(face, value, data);
}

typedef GLvoid (WINAPI *LPGLGETMATERIALFV)(GLenum, GLenum, GLfloat*);
static LPGLGETMATERIALFV lpglGetMaterialfv;
GLvoid WINAPI glGetMaterialfv(GLenum face, GLenum value, GLfloat* data)
{
 if(lpglGetMaterialfv) (*lpglGetMaterialfv)(face, value, data);
}

typedef GLvoid (WINAPI *LPGLGETTEXENVIV)(GLenum, GLenum, GLint*);
static LPGLGETTEXENVIV lpglGetTexEnviv;
GLvoid WINAPI glGetTexEnviv(GLenum env, GLenum value, GLint* data)
{
 if(lpglGetTexEnviv) (*lpglGetTexEnviv)(env, value, data);
}

typedef GLvoid (WINAPI *LPGLGETTEXENVFV)(GLenum, GLenum, GLfloat*);
static LPGLGETTEXENVFV lpglGetTexEnvfv;
GLvoid WINAPI glGetTexEnvfv(GLenum env, GLenum value, GLfloat* data)
{
 if(lpglGetTexEnvfv) (*lpglGetTexEnvfv)(env, value, data);
}

typedef GLvoid (WINAPI *LPGLGETTEXGENIV)(GLenum, GLenum, GLint*);
static LPGLGETTEXGENIV lpglGetTexGeniv;
GLvoid WINAPI glGetTexGeniv(GLenum coord, GLenum value, GLint* data)
{
 if(lpglGetTexGeniv) (*lpglGetTexGeniv)(coord, value, data);
}

typedef GLvoid (WINAPI *LPGLGETTEXGENFV)(GLenum, GLenum, GLfloat*);
static LPGLGETTEXGENFV lpglGetTexGenfv;
GLvoid WINAPI glGetTexGenfv(GLenum coord, GLenum value, GLfloat* data)
{
 if(lpglGetTexGenfv) (*lpglGetTexGenfv)(coord, value, data);
}

typedef GLvoid (WINAPI *LPGLGETTEXPARAMETERIV)(GLenum, GLenum, GLint*);
static LPGLGETTEXPARAMETERIV lpglGetTexParameteriv;
GLvoid WINAPI glGetTexParameteriv(GLenum target, GLenum value, GLint* data)
{
 if(lpglGetTexParameteriv) (*lpglGetTexParameteriv)(target, value, data);
}

typedef GLvoid (WINAPI *LPGLGETTEXPARAMETERFV)(GLenum, GLenum, GLfloat*);
static LPGLGETTEXPARAMETERFV lpglGetTexParameterfv;
GLvoid WINAPI glGetTexParameterfv(GLenum target, GLenum value, GLfloat* data)
{
 if(lpglGetTexParameterfv) (*lpglGetTexParameterfv)(target, value, data);
}

typedef GLvoid (WINAPI *LPGLGETTEXLEVELPARAMETERIV)(GLenum, GLint, GLenum, GLint*);
static LPGLGETTEXLEVELPARAMETERIV lpglGetTexLevelParameteriv;
GLvoid WINAPI glGetTexLevelParameteriv(GLenum target, GLint lod, GLenum value, GLint* data)
{
 if(lpglGetTexLevelParameteriv) (*lpglGetTexLevelParameteriv)(target, lod, value, data);
}

typedef GLvoid (WINAPI *LPGLGETTEXLEVELPARAMETERFV)(GLenum, GLint, GLenum, GLfloat*);
static LPGLGETTEXLEVELPARAMETERFV lpglGetTexLevelParameterfv;
GLvoid WINAPI glGetTexLevelParameterfv(GLenum target, GLint lod, GLenum value, GLfloat* data)
{
 if(lpglGetTexLevelParameterfv) (*lpglGetTexLevelParameterfv)(target, lod, value, data);
}

typedef GLvoid (WINAPI *LPGLGETPIXELMAPUIV)(GLenum, GLuint*);
static LPGLGETPIXELMAPUIV lpglGetPixelMapuiv;
GLvoid WINAPI glGetPixelMapuiv(GLenum map, GLuint* data)
{
 if(lpglGetPixelMapuiv) (*lpglGetPixelMapuiv)(map, data);
}

typedef GLvoid (WINAPI *LPGLGETPIXELMAPUSV)(GLenum, GLushort*);
static LPGLGETPIXELMAPUSV lpglGetPixelMapusv;
GLvoid WINAPI glGetPixelMapusv(GLenum map, GLushort* data)
{
 if(lpglGetPixelMapusv) (*lpglGetPixelMapusv)(map, data);
}

typedef GLvoid (WINAPI *LPGLGETPIXELMAPFV)(GLenum, GLfloat*);
static LPGLGETPIXELMAPFV lpglGetPixelMapfv;
GLvoid WINAPI glGetPixelMapfv(GLenum map, GLfloat* data)
{
 if(lpglGetPixelMapfv) (*lpglGetPixelMapfv)(map, data);
}

typedef GLvoid (WINAPI *LPGLGETMAPIV)(GLenum, GLenum, GLint*);
static LPGLGETMAPIV lpglGetMapiv;
GLvoid WINAPI glGetMapiv(GLenum map, GLenum value, GLint* data)
{
 if(lpglGetMapiv) (*lpglGetMapiv)(map, value, data);
}

typedef GLvoid (WINAPI *LPGLGETMAPFV)(GLenum, GLenum, GLfloat*);
static LPGLGETMAPFV lpglGetMapfv;
GLvoid WINAPI glGetMapfv(GLenum map, GLenum value, GLfloat* data)
{
 if(lpglGetMapfv) (*lpglGetMapfv)(map, value, data);
}

typedef GLvoid (WINAPI *LPGLGETMAPDV)(GLenum, GLenum, GLdouble*);
static LPGLGETMAPDV lpglGetMapdv;
GLvoid WINAPI glGetMapdv(GLenum map, GLenum value, GLdouble* data)
{
 if(lpglGetMapdv) (*lpglGetMapdv)(map, value, data);
}

// Section 6.1.4
// Texture Queries

typedef GLvoid (WINAPI *LPGLGETTEXIMAGE)(GLenum, GLint, GLenum, GLenum, GLvoid*);
static LPGLGETTEXIMAGE lpglGetTexImage;
GLvoid WINAPI glGetTexImage(GLenum tex, GLint lod, GLenum format, GLenum type, GLvoid* img)
{
 if(lpglGetTexImage) (*lpglGetTexImage)(tex, lod, format, type, img);
}

typedef GLboolean (WINAPI *LPGLISTEXTURE)(GLuint);
static LPGLISTEXTURE lpglIsTexture;
GLboolean WINAPI glIsTexture(GLuint texture)
{
 if(lpglIsTexture) return (*lpglIsTexture)(texture);
 return (GLboolean)0;
}

// Section 6.1.5
// Stipple Query

typedef GLvoid (WINAPI *LPGLGETPOLYGONSTIPPLE)(GLvoid*);
static LPGLGETPOLYGONSTIPPLE lpglGetPolygonStipple;
GLvoid WINAPI glGetPolygonStipple(GLvoid* pattern)
{
 if(lpglGetPolygonStipple) (*lpglGetPolygonStipple)(pattern);
}

// Section 6.1.7
// Color Table Query

typedef GLvoid (WINAPI *LPGLGETCOLORTABLE)(GLenum, GLenum, GLenum, GLvoid*);
static LPGLGETCOLORTABLE lpglGetColorTable;
GLvoid WINAPI glGetColorTable(GLenum target, GLenum format, GLenum type, GLvoid* table)
{
 if(lpglGetColorTable) (*lpglGetColorTable)(target, format, type, table);
}

typedef GLvoid (WINAPI *LPGLGETCOLORTABLEPARAMETERIV)(GLenum, GLenum, GLint*);
static LPGLGETCOLORTABLEPARAMETERIV lpglGetColorTableParameteriv;
GLvoid WINAPI glGetColorTableParameteriv(GLenum target, GLenum pname, GLint* params)
{
 if(lpglGetColorTableParameteriv) (*lpglGetColorTableParameteriv)(target, pname, params);
}

typedef GLvoid (WINAPI *LPGLGETCOLORTABLEPARAMETERFV)(GLenum, GLenum, GLfloat*);
static LPGLGETCOLORTABLEPARAMETERFV lpglGetColorTableParameterfv;
GLvoid WINAPI glGetColorTableParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
 if(lpglGetColorTableParameterfv) (*lpglGetColorTableParameterfv)(target, pname, params);
}

// Section 6.1.8
// Convolution Query

typedef GLvoid (WINAPI *LPGLGETCONVOLUTIONFILTER)(GLenum, GLenum, GLenum, GLvoid*);
static LPGLGETCONVOLUTIONFILTER lpglGetConvolutionFilter;
GLvoid WINAPI glGetConvolutionFilter(GLenum target, GLenum format, GLenum type, GLvoid* image)
{
 if(lpglGetConvolutionFilter) (*lpglGetConvolutionFilter)(target, format, type, image);
}

typedef GLvoid (WINAPI *LPGLGETSEPARABLEFILTER)(GLenum, GLenum, GLenum, GLvoid*, GLvoid*, GLvoid*);
static LPGLGETSEPARABLEFILTER lpglGetSeparableFilter;
GLvoid WINAPI glGetSeparableFilter(GLenum target, GLenum format, GLenum type, GLvoid* row, GLvoid* column, GLvoid* span)
{
 if(lpglGetSeparableFilter) (*lpglGetSeparableFilter)(target, format, type, row, column, span);
}

typedef GLvoid (WINAPI *LPGLGETCONVOLUTIONPARAMETERIV)(GLenum, GLenum, GLint*);
static LPGLGETCONVOLUTIONPARAMETERIV lpglGetConvolutionParameteriv;
GLvoid WINAPI glGetConvolutionParameteriv(GLenum target, GLenum pname, GLint* params)
{
 if(lpglGetConvolutionParameteriv) (*lpglGetConvolutionParameteriv)(target, pname, params);
}

typedef GLvoid (WINAPI *LPGLGETCONVOLUTIONPARAMETERFV)(GLenum, GLenum, GLfloat*);
static LPGLGETCONVOLUTIONPARAMETERFV lpglGetConvolutionParameterfv;
GLvoid WINAPI glGetConvolutionParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
 if(lpglGetConvolutionParameterfv) (*lpglGetConvolutionParameterfv)(target, pname, params);
}

// Section 6.1.9
// Histogram Query

typedef GLvoid (WINAPI *LPGLGETHISTOGRAM)(GLenum, GLboolean, GLenum, GLenum, GLvoid*);
static LPGLGETHISTOGRAM lpglGetHistogram;
GLvoid WINAPI glGetHistogram(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid* values)
{
 if(lpglGetHistogram) (*lpglGetHistogram)(target, reset, format, type, values);
}

typedef GLvoid (WINAPI *LPGLRESETHISTOGRAM)(GLenum);
static LPGLRESETHISTOGRAM lpglResetHistogram;
GLvoid WINAPI glResetHistogram(GLenum target)
{
 if(lpglResetHistogram) (*lpglResetHistogram)(target);
}

typedef GLvoid (WINAPI *LPGLGETHISTOGRAMPARAMETERIV)(GLenum, GLenum, GLint*);
static LPGLGETHISTOGRAMPARAMETERIV lpglGetHistogramParameteriv;
GLvoid WINAPI glGetHistogramParameteriv(GLenum target, GLenum pname, GLint* params)
{
 if(lpglGetHistogramParameteriv) (*lpglGetHistogramParameteriv)(target, pname, params);
}

typedef GLvoid (WINAPI *LPGLGETHISTOGRAMPARAMETERFV)(GLenum, GLenum, GLfloat*);
static LPGLGETHISTOGRAMPARAMETERFV lpglGetHistogramParameterfv;
GLvoid WINAPI glGetHistogramParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
 if(lpglGetHistogramParameterfv) (*lpglGetHistogramParameterfv)(target, pname, params);
}

// Section 6.1.10
// Minmax Query

typedef GLvoid (WINAPI *LPGLGETMINMAX)(GLenum, GLboolean, GLenum, GLenum, GLvoid*);
static LPGLGETMINMAX lpglGetMinmax;
GLvoid WINAPI glGetMinmax(GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid* values)
{
 if(lpglGetMinmax) (*lpglGetMinmax)(target, reset, format, type, values);
}

typedef GLvoid (WINAPI *LPGLRESETMINMAX)(GLenum);
static LPGLRESETMINMAX lpglResetMinmax;
GLvoid WINAPI glResetMinmax(GLenum target)
{
 if(lpglResetMinmax) (*lpglResetMinmax)(target);
}

typedef GLvoid (WINAPI *LPGLGETMINMAXPARAMETERIV)(GLenum, GLenum, GLint*);
static LPGLGETMINMAXPARAMETERIV lpglGetMinmaxParameteriv;
GLvoid WINAPI glGetMinmaxParameteriv(GLenum target, GLenum pname, GLint* params)
{
 if(lpglGetMinmaxParameteriv) (*lpglGetMinmaxParameteriv)(target, pname, params);
}

typedef GLvoid (WINAPI *LPGLGETMINMAXPARAMETERFV)(GLenum, GLenum, GLfloat*);
static LPGLGETMINMAXPARAMETERFV lpglGetMinmaxParameterfv;
GLvoid WINAPI glGetMinmaxParameterfv(GLenum target, GLenum pname, GLfloat* params)
{
 if(lpglGetMinmaxParameterfv) (*lpglGetMinmaxParameterfv)(target, pname, params);
}

// Section 6.1.11
// Pointer and String Queries

typedef GLvoid (WINAPI *LPGLGETPOINTERV)(GLenum, GLvoid**);
static LPGLGETPOINTERV lpglGetPointerv;
GLvoid WINAPI glGetPointerv(GLenum pname, GLvoid** params)
{
 if(lpglGetPointerv) (*lpglGetPointerv)(pname, params);
}

typedef const GLubyte* (WINAPI *LPGLGETSTRING)(GLenum);
static LPGLGETSTRING lpglGetString;
const GLubyte* WINAPI glGetString(GLenum name)
{
 if(lpglGetString) return (*lpglGetString)(name);
 return (const GLubyte*)0;
}

// Section 6.1.12
// Saving and Restoring State

typedef GLvoid (WINAPI *LPGLPUSHATTRIB)(GLbitfield);
static LPGLPUSHATTRIB lpglPushAttrib;
GLvoid WINAPI glPushAttrib(GLbitfield mask)
{
 if(lpglPushAttrib) (*lpglPushAttrib)(mask);
}

typedef GLvoid (WINAPI *LPGLPUSHCLIENTATTRIB)(GLbitfield);
static LPGLPUSHCLIENTATTRIB lpglPushClientAttrib;
GLvoid WINAPI glPushClientAttrib(GLbitfield mask)
{
 if(lpglPushClientAttrib) (*lpglPushClientAttrib)(mask);
}

typedef GLvoid (WINAPI *LPGLPOPATTRIB)(GLvoid);
static LPGLPOPATTRIB lpglPopAttrib;
GLvoid WINAPI glPopAttrib(GLvoid)
{
 if(lpglPopAttrib) (*lpglPopAttrib)();
}

typedef GLvoid (WINAPI *LPGLPOPCLIENTATTRIB)(GLvoid);
static LPGLPOPCLIENTATTRIB lpglPopClientAttrib;
GLvoid WINAPI glPopClientAttrib(GLvoid)
{
 if(lpglPopClientAttrib) (*lpglPopClientAttrib)();
}

static BOOL WINAPI LoadFailure(LPCSTR name, BOOL (*onfail)(LPCSTR))
{
 return (onfail ? (*onfail)(name) : FALSE);
}

static PROC WINAPI LoadType0(LPCSTR name)
{
 return GetProcAddress(opengl32, name);
}

static PROC WINAPI LoadType1(LPCSTR name)
{
 return wglGetProcAddress(name);
}

BOOL WINAPI wglLoadExtensions(BOOL (*onfail)(LPCSTR))
{
 // local variables
 typedef const GLubyte* (WINAPI *function)(GLenum);
 function func = 0;
 const char* extensions = 0;
 const char* version_str = 0;

 // check if already loaded
 if(loaded) return TRUE;

 // load OpenGL manually
 opengl32 = LoadLibrary(TEXT("opengl32.dll"));
 if(!opengl32) { if(onfail) (*onfail)("LoadLibrary failure."); return FALSE; }

 // get GetString address first
 func = (function)GetProcAddress(opengl32, "glGetString");
 if(!func) { if(onfail) (*onfail)("glGetString load failure."); return FALSE; }

 // get list of extensions
 extensions = (const char*)(*func)(GL_EXTENSIONS);
 if(!extensions || !strlen(extensions)) { if(onfail) (*onfail)("GL_EXTENSIONS failure."); return FALSE; }

 // get version string
 version_str = (const char*)(*func)(GL_VERSION);
 if(!version_str || !strlen(version_str)) { if(onfail) (*onfail)("GL_VERSION failure."); return FALSE; }

 // load functions and extensions
 lpglGetError = (LPGLGETERROR)LoadType0("glGetError"); if(!lpglGetError && !LoadFailure("glGetError", onfail)) return FALSE;
 lpglBegin = (LPGLBEGIN)LoadType0("glBegin"); if(!lpglBegin && !LoadFailure("glBegin", onfail)) return FALSE;
 lpglEnd = (LPGLEND)LoadType0("glEnd"); if(!lpglEnd && !LoadFailure("glEnd", onfail)) return FALSE;
 lpglEdgeFlag = (LPGLEDGEFLAG)LoadType0("glEdgeFlag"); if(!lpglEdgeFlag && !LoadFailure("glEdgeFlag", onfail)) return FALSE;
 lpglEdgeFlagv = (LPGLEDGEFLAGV)LoadType0("glEdgeFlagv"); if(!lpglEdgeFlagv && !LoadFailure("glEdgeFlagv", onfail)) return FALSE;
 lpglVertex2s = (LPGLVERTEX2S)LoadType0("glVertex2s"); if(!lpglVertex2s && !LoadFailure("glVertex2s", onfail)) return FALSE;
 lpglVertex3s = (LPGLVERTEX3S)LoadType0("glVertex3s"); if(!lpglVertex3s && !LoadFailure("glVertex3s", onfail)) return FALSE;
 lpglVertex4s = (LPGLVERTEX4S)LoadType0("glVertex4s"); if(!lpglVertex4s && !LoadFailure("glVertex4s", onfail)) return FALSE;
 lpglVertex2i = (LPGLVERTEX2I)LoadType0("glVertex2i"); if(!lpglVertex2i && !LoadFailure("glVertex2i", onfail)) return FALSE;
 lpglVertex3i = (LPGLVERTEX3I)LoadType0("glVertex3i"); if(!lpglVertex3i && !LoadFailure("glVertex3i", onfail)) return FALSE;
 lpglVertex4i = (LPGLVERTEX4I)LoadType0("glVertex4i"); if(!lpglVertex4i && !LoadFailure("glVertex4i", onfail)) return FALSE;
 lpglVertex2f = (LPGLVERTEX2F)LoadType0("glVertex2f"); if(!lpglVertex2f && !LoadFailure("glVertex2f", onfail)) return FALSE;
 lpglVertex3f = (LPGLVERTEX3F)LoadType0("glVertex3f"); if(!lpglVertex3f && !LoadFailure("glVertex3f", onfail)) return FALSE;
 lpglVertex4f = (LPGLVERTEX4F)LoadType0("glVertex4f"); if(!lpglVertex4f && !LoadFailure("glVertex4f", onfail)) return FALSE;
 lpglVertex2d = (LPGLVERTEX2D)LoadType0("glVertex2d"); if(!lpglVertex2d && !LoadFailure("glVertex2d", onfail)) return FALSE;
 lpglVertex3d = (LPGLVERTEX3D)LoadType0("glVertex3d"); if(!lpglVertex3d && !LoadFailure("glVertex3d", onfail)) return FALSE;
 lpglVertex4d = (LPGLVERTEX4D)LoadType0("glVertex4d"); if(!lpglVertex4d && !LoadFailure("glVertex4d", onfail)) return FALSE;
 lpglVertex2sv = (LPGLVERTEX2SV)LoadType0("glVertex2sv"); if(!lpglVertex2sv && !LoadFailure("glVertex2sv", onfail)) return FALSE;
 lpglVertex2iv = (LPGLVERTEX2IV)LoadType0("glVertex2iv"); if(!lpglVertex2iv && !LoadFailure("glVertex2iv", onfail)) return FALSE;
 lpglVertex2fv = (LPGLVERTEX2FV)LoadType0("glVertex2fv"); if(!lpglVertex2fv && !LoadFailure("glVertex2fv", onfail)) return FALSE;
 lpglVertex2dv = (LPGLVERTEX2DV)LoadType0("glVertex2dv"); if(!lpglVertex2dv && !LoadFailure("glVertex2dv", onfail)) return FALSE;
 lpglVertex3sv = (LPGLVERTEX3SV)LoadType0("glVertex3sv"); if(!lpglVertex3sv && !LoadFailure("glVertex3sv", onfail)) return FALSE;
 lpglVertex3iv = (LPGLVERTEX3IV)LoadType0("glVertex3iv"); if(!lpglVertex3iv && !LoadFailure("glVertex3iv", onfail)) return FALSE;
 lpglVertex3fv = (LPGLVERTEX3FV)LoadType0("glVertex3fv"); if(!lpglVertex3fv && !LoadFailure("glVertex3fv", onfail)) return FALSE;
 lpglVertex3dv = (LPGLVERTEX3DV)LoadType0("glVertex3dv"); if(!lpglVertex3dv && !LoadFailure("glVertex3dv", onfail)) return FALSE;
 lpglVertex4sv = (LPGLVERTEX4SV)LoadType0("glVertex4sv"); if(!lpglVertex4sv && !LoadFailure("glVertex4sv", onfail)) return FALSE;
 lpglVertex4iv = (LPGLVERTEX4IV)LoadType0("glVertex4iv"); if(!lpglVertex4iv && !LoadFailure("glVertex4iv", onfail)) return FALSE;
 lpglVertex4fv = (LPGLVERTEX4FV)LoadType0("glVertex4fv"); if(!lpglVertex4fv && !LoadFailure("glVertex4fv", onfail)) return FALSE;
 lpglVertex4dv = (LPGLVERTEX4DV)LoadType0("glVertex4dv"); if(!lpglVertex4dv && !LoadFailure("glVertex4dv", onfail)) return FALSE;
 lpglTexCoord1s = (LPGLTEXCOORD1S)LoadType0("glTexCoord1s"); if(!lpglTexCoord1s && !LoadFailure("glTexCoord1s", onfail)) return FALSE;
 lpglTexCoord1i = (LPGLTEXCOORD1I)LoadType0("glTexCoord1i"); if(!lpglTexCoord1i && !LoadFailure("glTexCoord1i", onfail)) return FALSE;
 lpglTexCoord1f = (LPGLTEXCOORD1F)LoadType0("glTexCoord1f"); if(!lpglTexCoord1f && !LoadFailure("glTexCoord1f", onfail)) return FALSE;
 lpglTexCoord1d = (LPGLTEXCOORD1D)LoadType0("glTexCoord1d"); if(!lpglTexCoord1d && !LoadFailure("glTexCoord1d", onfail)) return FALSE;
 lpglTexCoord2s = (LPGLTEXCOORD2S)LoadType0("glTexCoord2s"); if(!lpglTexCoord2s && !LoadFailure("glTexCoord2s", onfail)) return FALSE;
 lpglTexCoord2i = (LPGLTEXCOORD2I)LoadType0("glTexCoord2i"); if(!lpglTexCoord2i && !LoadFailure("glTexCoord2i", onfail)) return FALSE;
 lpglTexCoord2f = (LPGLTEXCOORD2F)LoadType0("glTexCoord2f"); if(!lpglTexCoord2f && !LoadFailure("glTexCoord2f", onfail)) return FALSE;
 lpglTexCoord2d = (LPGLTEXCOORD2D)LoadType0("glTexCoord2d"); if(!lpglTexCoord2d && !LoadFailure("glTexCoord2d", onfail)) return FALSE;
 lpglTexCoord3s = (LPGLTEXCOORD3S)LoadType0("glTexCoord3s"); if(!lpglTexCoord3s && !LoadFailure("glTexCoord3s", onfail)) return FALSE;
 lpglTexCoord3i = (LPGLTEXCOORD3I)LoadType0("glTexCoord3i"); if(!lpglTexCoord3i && !LoadFailure("glTexCoord3i", onfail)) return FALSE;
 lpglTexCoord3f = (LPGLTEXCOORD3F)LoadType0("glTexCoord3f"); if(!lpglTexCoord3f && !LoadFailure("glTexCoord3f", onfail)) return FALSE;
 lpglTexCoord3d = (LPGLTEXCOORD3D)LoadType0("glTexCoord3d"); if(!lpglTexCoord3d && !LoadFailure("glTexCoord3d", onfail)) return FALSE;
 lpglTexCoord4s = (LPGLTEXCOORD4S)LoadType0("glTexCoord4s"); if(!lpglTexCoord4s && !LoadFailure("glTexCoord4s", onfail)) return FALSE;
 lpglTexCoord4i = (LPGLTEXCOORD4I)LoadType0("glTexCoord4i"); if(!lpglTexCoord4i && !LoadFailure("glTexCoord4i", onfail)) return FALSE;
 lpglTexCoord4f = (LPGLTEXCOORD4F)LoadType0("glTexCoord4f"); if(!lpglTexCoord4f && !LoadFailure("glTexCoord4f", onfail)) return FALSE;
 lpglTexCoord4d = (LPGLTEXCOORD4D)LoadType0("glTexCoord4d"); if(!lpglTexCoord4d && !LoadFailure("glTexCoord4d", onfail)) return FALSE;
 lpglTexCoord1sv = (LPGLTEXCOORD1SV)LoadType0("glTexCoord1sv"); if(!lpglTexCoord1sv && !LoadFailure("glTexCoord1sv", onfail)) return FALSE;
 lpglTexCoord1iv = (LPGLTEXCOORD1IV)LoadType0("glTexCoord1iv"); if(!lpglTexCoord1iv && !LoadFailure("glTexCoord1iv", onfail)) return FALSE;
 lpglTexCoord1fv = (LPGLTEXCOORD1FV)LoadType0("glTexCoord1fv"); if(!lpglTexCoord1fv && !LoadFailure("glTexCoord1fv", onfail)) return FALSE;
 lpglTexCoord1dv = (LPGLTEXCOORD1DV)LoadType0("glTexCoord1dv"); if(!lpglTexCoord1dv && !LoadFailure("glTexCoord1dv", onfail)) return FALSE;
 lpglTexCoord2sv = (LPGLTEXCOORD2SV)LoadType0("glTexCoord2sv"); if(!lpglTexCoord2sv && !LoadFailure("glTexCoord2sv", onfail)) return FALSE;
 lpglTexCoord2iv = (LPGLTEXCOORD2IV)LoadType0("glTexCoord2iv"); if(!lpglTexCoord2iv && !LoadFailure("glTexCoord2iv", onfail)) return FALSE;
 lpglTexCoord2fv = (LPGLTEXCOORD2FV)LoadType0("glTexCoord2fv"); if(!lpglTexCoord2fv && !LoadFailure("glTexCoord2fv", onfail)) return FALSE;
 lpglTexCoord2dv = (LPGLTEXCOORD2DV)LoadType0("glTexCoord2dv"); if(!lpglTexCoord2dv && !LoadFailure("glTexCoord2dv", onfail)) return FALSE;
 lpglTexCoord3sv = (LPGLTEXCOORD3SV)LoadType0("glTexCoord3sv"); if(!lpglTexCoord3sv && !LoadFailure("glTexCoord3sv", onfail)) return FALSE;
 lpglTexCoord3iv = (LPGLTEXCOORD3IV)LoadType0("glTexCoord3iv"); if(!lpglTexCoord3iv && !LoadFailure("glTexCoord3iv", onfail)) return FALSE;
 lpglTexCoord3fv = (LPGLTEXCOORD3FV)LoadType0("glTexCoord3fv"); if(!lpglTexCoord3fv && !LoadFailure("glTexCoord3fv", onfail)) return FALSE;
 lpglTexCoord3dv = (LPGLTEXCOORD3DV)LoadType0("glTexCoord3dv"); if(!lpglTexCoord3dv && !LoadFailure("glTexCoord3dv", onfail)) return FALSE;
 lpglTexCoord4sv = (LPGLTEXCOORD4SV)LoadType0("glTexCoord4sv"); if(!lpglTexCoord4sv && !LoadFailure("glTexCoord4sv", onfail)) return FALSE;
 lpglTexCoord4iv = (LPGLTEXCOORD4IV)LoadType0("glTexCoord4iv"); if(!lpglTexCoord4iv && !LoadFailure("glTexCoord4iv", onfail)) return FALSE;
 lpglTexCoord4fv = (LPGLTEXCOORD4FV)LoadType0("glTexCoord4fv"); if(!lpglTexCoord4fv && !LoadFailure("glTexCoord4fv", onfail)) return FALSE;
 lpglTexCoord4dv = (LPGLTEXCOORD4DV)LoadType0("glTexCoord4dv"); if(!lpglTexCoord4dv && !LoadFailure("glTexCoord4dv", onfail)) return FALSE;
 lpglMultiTexCoord1s = (LPGLMULTITEXCOORD1S)LoadType1("glMultiTexCoord1s"); if(!lpglMultiTexCoord1s && !LoadFailure("glMultiTexCoord1s", onfail)) return FALSE;
 lpglMultiTexCoord1i = (LPGLMULTITEXCOORD1I)LoadType1("glMultiTexCoord1i"); if(!lpglMultiTexCoord1i && !LoadFailure("glMultiTexCoord1i", onfail)) return FALSE;
 lpglMultiTexCoord1f = (LPGLMULTITEXCOORD1F)LoadType1("glMultiTexCoord1f"); if(!lpglMultiTexCoord1f && !LoadFailure("glMultiTexCoord1f", onfail)) return FALSE;
 lpglMultiTexCoord1d = (LPGLMULTITEXCOORD1D)LoadType1("glMultiTexCoord1d"); if(!lpglMultiTexCoord1d && !LoadFailure("glMultiTexCoord1d", onfail)) return FALSE;
 lpglMultiTexCoord2s = (LPGLMULTITEXCOORD2S)LoadType1("glMultiTexCoord2s"); if(!lpglMultiTexCoord2s && !LoadFailure("glMultiTexCoord2s", onfail)) return FALSE;
 lpglMultiTexCoord2i = (LPGLMULTITEXCOORD2I)LoadType1("glMultiTexCoord2i"); if(!lpglMultiTexCoord2i && !LoadFailure("glMultiTexCoord2i", onfail)) return FALSE;
 lpglMultiTexCoord2f = (LPGLMULTITEXCOORD2F)LoadType1("glMultiTexCoord2f"); if(!lpglMultiTexCoord2f && !LoadFailure("glMultiTexCoord2f", onfail)) return FALSE;
 lpglMultiTexCoord2d = (LPGLMULTITEXCOORD2D)LoadType1("glMultiTexCoord2d"); if(!lpglMultiTexCoord2d && !LoadFailure("glMultiTexCoord2d", onfail)) return FALSE;
 lpglMultiTexCoord3s = (LPGLMULTITEXCOORD3S)LoadType1("glMultiTexCoord3s"); if(!lpglMultiTexCoord3s && !LoadFailure("glMultiTexCoord3s", onfail)) return FALSE;
 lpglMultiTexCoord3i = (LPGLMULTITEXCOORD3I)LoadType1("glMultiTexCoord3i"); if(!lpglMultiTexCoord3i && !LoadFailure("glMultiTexCoord3i", onfail)) return FALSE;
 lpglMultiTexCoord3f = (LPGLMULTITEXCOORD3F)LoadType1("glMultiTexCoord3f"); if(!lpglMultiTexCoord3f && !LoadFailure("glMultiTexCoord3f", onfail)) return FALSE;
 lpglMultiTexCoord3d = (LPGLMULTITEXCOORD3D)LoadType1("glMultiTexCoord3d"); if(!lpglMultiTexCoord3d && !LoadFailure("glMultiTexCoord3d", onfail)) return FALSE;
 lpglMultiTexCoord4s = (LPGLMULTITEXCOORD4S)LoadType1("glMultiTexCoord4s"); if(!lpglMultiTexCoord4s && !LoadFailure("glMultiTexCoord4s", onfail)) return FALSE;
 lpglMultiTexCoord4i = (LPGLMULTITEXCOORD4I)LoadType1("glMultiTexCoord4i"); if(!lpglMultiTexCoord4i && !LoadFailure("glMultiTexCoord4i", onfail)) return FALSE;
 lpglMultiTexCoord4f = (LPGLMULTITEXCOORD4F)LoadType1("glMultiTexCoord4f"); if(!lpglMultiTexCoord4f && !LoadFailure("glMultiTexCoord4f", onfail)) return FALSE;
 lpglMultiTexCoord4d = (LPGLMULTITEXCOORD4D)LoadType1("glMultiTexCoord4d"); if(!lpglMultiTexCoord4d && !LoadFailure("glMultiTexCoord4d", onfail)) return FALSE;
 lpglMultiTexCoord1sv = (LPGLMULTITEXCOORD1SV)LoadType1("glMultiTexCoord1sv"); if(!lpglMultiTexCoord1sv && !LoadFailure("glMultiTexCoord1sv", onfail)) return FALSE;
 lpglMultiTexCoord1iv = (LPGLMULTITEXCOORD1IV)LoadType1("glMultiTexCoord1iv"); if(!lpglMultiTexCoord1iv && !LoadFailure("glMultiTexCoord1iv", onfail)) return FALSE;
 lpglMultiTexCoord1fv = (LPGLMULTITEXCOORD1FV)LoadType1("glMultiTexCoord1fv"); if(!lpglMultiTexCoord1fv && !LoadFailure("glMultiTexCoord1fv", onfail)) return FALSE;
 lpglMultiTexCoord1dv = (LPGLMULTITEXCOORD1DV)LoadType1("glMultiTexCoord1dv"); if(!lpglMultiTexCoord1dv && !LoadFailure("glMultiTexCoord1dv", onfail)) return FALSE;
 lpglMultiTexCoord2sv = (LPGLMULTITEXCOORD2SV)LoadType1("glMultiTexCoord2sv"); if(!lpglMultiTexCoord2sv && !LoadFailure("glMultiTexCoord2sv", onfail)) return FALSE;
 lpglMultiTexCoord2iv = (LPGLMULTITEXCOORD2IV)LoadType1("glMultiTexCoord2iv"); if(!lpglMultiTexCoord2iv && !LoadFailure("glMultiTexCoord2iv", onfail)) return FALSE;
 lpglMultiTexCoord2fv = (LPGLMULTITEXCOORD2FV)LoadType1("glMultiTexCoord2fv"); if(!lpglMultiTexCoord2fv && !LoadFailure("glMultiTexCoord2fv", onfail)) return FALSE;
 lpglMultiTexCoord2dv = (LPGLMULTITEXCOORD2DV)LoadType1("glMultiTexCoord2dv"); if(!lpglMultiTexCoord2dv && !LoadFailure("glMultiTexCoord2dv", onfail)) return FALSE;
 lpglMultiTexCoord3sv = (LPGLMULTITEXCOORD3SV)LoadType1("glMultiTexCoord3sv"); if(!lpglMultiTexCoord3sv && !LoadFailure("glMultiTexCoord3sv", onfail)) return FALSE;
 lpglMultiTexCoord3iv = (LPGLMULTITEXCOORD3IV)LoadType1("glMultiTexCoord3iv"); if(!lpglMultiTexCoord3iv && !LoadFailure("glMultiTexCoord3iv", onfail)) return FALSE;
 lpglMultiTexCoord3fv = (LPGLMULTITEXCOORD3FV)LoadType1("glMultiTexCoord3fv"); if(!lpglMultiTexCoord3fv && !LoadFailure("glMultiTexCoord3fv", onfail)) return FALSE;
 lpglMultiTexCoord3dv = (LPGLMULTITEXCOORD3DV)LoadType1("glMultiTexCoord3dv"); if(!lpglMultiTexCoord3dv && !LoadFailure("glMultiTexCoord3dv", onfail)) return FALSE;
 lpglMultiTexCoord4sv = (LPGLMULTITEXCOORD4SV)LoadType1("glMultiTexCoord4sv"); if(!lpglMultiTexCoord4sv && !LoadFailure("glMultiTexCoord4sv", onfail)) return FALSE;
 lpglMultiTexCoord4iv = (LPGLMULTITEXCOORD4IV)LoadType1("glMultiTexCoord4iv"); if(!lpglMultiTexCoord4iv && !LoadFailure("glMultiTexCoord4iv", onfail)) return FALSE;
 lpglMultiTexCoord4fv = (LPGLMULTITEXCOORD4FV)LoadType1("glMultiTexCoord4fv"); if(!lpglMultiTexCoord4fv && !LoadFailure("glMultiTexCoord4fv", onfail)) return FALSE;
 lpglMultiTexCoord4dv = (LPGLMULTITEXCOORD4DV)LoadType1("glMultiTexCoord4dv"); if(!lpglMultiTexCoord4dv && !LoadFailure("glMultiTexCoord4dv", onfail)) return FALSE;
 lpglNormal3b = (LPGLNORMAL3B)LoadType0("glNormal3b"); if(!lpglNormal3b && !LoadFailure("glNormal3b", onfail)) return FALSE;
 lpglNormal3s = (LPGLNORMAL3S)LoadType0("glNormal3s"); if(!lpglNormal3s && !LoadFailure("glNormal3s", onfail)) return FALSE;
 lpglNormal3i = (LPGLNORMAL3I)LoadType0("glNormal3i"); if(!lpglNormal3i && !LoadFailure("glNormal3i", onfail)) return FALSE;
 lpglNormal3f = (LPGLNORMAL3F)LoadType0("glNormal3f"); if(!lpglNormal3f && !LoadFailure("glNormal3f", onfail)) return FALSE;
 lpglNormal3d = (LPGLNORMAL3D)LoadType0("glNormal3d"); if(!lpglNormal3d && !LoadFailure("glNormal3d", onfail)) return FALSE;
 lpglNormal3bv = (LPGLNORMAL3BV)LoadType0("glNormal3bv"); if(!lpglNormal3bv && !LoadFailure("glNormal3bv", onfail)) return FALSE;
 lpglNormal3sv = (LPGLNORMAL3SV)LoadType0("glNormal3sv"); if(!lpglNormal3sv && !LoadFailure("glNormal3sv", onfail)) return FALSE;
 lpglNormal3iv = (LPGLNORMAL3IV)LoadType0("glNormal3iv"); if(!lpglNormal3iv && !LoadFailure("glNormal3iv", onfail)) return FALSE;
 lpglNormal3fv = (LPGLNORMAL3FV)LoadType0("glNormal3fv"); if(!lpglNormal3fv && !LoadFailure("glNormal3fv", onfail)) return FALSE;
 lpglNormal3dv = (LPGLNORMAL3DV)LoadType0("glNormal3dv"); if(!lpglNormal3dv && !LoadFailure("glNormal3dv", onfail)) return FALSE;
 lpglColor3b = (LPGLCOLOR3B)LoadType0("glColor3b"); if(!lpglColor3b && !LoadFailure("glColor3b", onfail)) return FALSE;
 lpglColor3s = (LPGLCOLOR3S)LoadType0("glColor3s"); if(!lpglColor3s && !LoadFailure("glColor3s", onfail)) return FALSE;
 lpglColor3i = (LPGLCOLOR3I)LoadType0("glColor3i"); if(!lpglColor3i && !LoadFailure("glColor3i", onfail)) return FALSE;
 lpglColor3f = (LPGLCOLOR3F)LoadType0("glColor3f"); if(!lpglColor3f && !LoadFailure("glColor3f", onfail)) return FALSE;
 lpglColor3d = (LPGLCOLOR3D)LoadType0("glColor3d"); if(!lpglColor3d && !LoadFailure("glColor3d", onfail)) return FALSE;
 lpglColor3ub = (LPGLCOLOR3UB)LoadType0("glColor3ub"); if(!lpglColor3ub && !LoadFailure("glColor3ub", onfail)) return FALSE;
 lpglColor3us = (LPGLCOLOR3US)LoadType0("glColor3us"); if(!lpglColor3us && !LoadFailure("glColor3us", onfail)) return FALSE;
 lpglColor3ui = (LPGLCOLOR3UI)LoadType0("glColor3ui"); if(!lpglColor3ui && !LoadFailure("glColor3ui", onfail)) return FALSE;
 lpglColor4b = (LPGLCOLOR4B)LoadType0("glColor4b"); if(!lpglColor4b && !LoadFailure("glColor4b", onfail)) return FALSE;
 lpglColor4s = (LPGLCOLOR4S)LoadType0("glColor4s"); if(!lpglColor4s && !LoadFailure("glColor4s", onfail)) return FALSE;
 lpglColor4i = (LPGLCOLOR4I)LoadType0("glColor4i"); if(!lpglColor4i && !LoadFailure("glColor4i", onfail)) return FALSE;
 lpglColor4f = (LPGLCOLOR4F)LoadType0("glColor4f"); if(!lpglColor4f && !LoadFailure("glColor4f", onfail)) return FALSE;
 lpglColor4d = (LPGLCOLOR4D)LoadType0("glColor4d"); if(!lpglColor4d && !LoadFailure("glColor4d", onfail)) return FALSE;
 lpglColor4ub = (LPGLCOLOR4UB)LoadType0("glColor4ub"); if(!lpglColor4ub && !LoadFailure("glColor4ub", onfail)) return FALSE;
 lpglColor4us = (LPGLCOLOR4US)LoadType0("glColor4us"); if(!lpglColor4us && !LoadFailure("glColor4us", onfail)) return FALSE;
 lpglColor4ui = (LPGLCOLOR4UI)LoadType0("glColor4ui"); if(!lpglColor4ui && !LoadFailure("glColor4ui", onfail)) return FALSE;
 lpglColor3bv = (LPGLCOLOR3BV)LoadType0("glColor3bv"); if(!lpglColor3bv && !LoadFailure("glColor3bv", onfail)) return FALSE;
 lpglColor3sv = (LPGLCOLOR3SV)LoadType0("glColor3sv"); if(!lpglColor3sv && !LoadFailure("glColor3sv", onfail)) return FALSE;
 lpglColor3iv = (LPGLCOLOR3IV)LoadType0("glColor3iv"); if(!lpglColor3iv && !LoadFailure("glColor3iv", onfail)) return FALSE;
 lpglColor3fv = (LPGLCOLOR3FV)LoadType0("glColor3fv"); if(!lpglColor3fv && !LoadFailure("glColor3fv", onfail)) return FALSE;
 lpglColor3dv = (LPGLCOLOR3DV)LoadType0("glColor3dv"); if(!lpglColor3dv && !LoadFailure("glColor3dv", onfail)) return FALSE;
 lpglColor3ubv = (LPGLCOLOR3UBV)LoadType0("glColor3ubv"); if(!lpglColor3ubv && !LoadFailure("glColor3ubv", onfail)) return FALSE;
 lpglColor3usv = (LPGLCOLOR3USV)LoadType0("glColor3usv"); if(!lpglColor3usv && !LoadFailure("glColor3usv", onfail)) return FALSE;
 lpglColor3uiv = (LPGLCOLOR3UIV)LoadType0("glColor3uiv"); if(!lpglColor3uiv && !LoadFailure("glColor3uiv", onfail)) return FALSE;
 lpglColor4bv = (LPGLCOLOR4BV)LoadType0("glColor4bv"); if(!lpglColor4bv && !LoadFailure("glColor4bv", onfail)) return FALSE;
 lpglColor4sv = (LPGLCOLOR4SV)LoadType0("glColor4sv"); if(!lpglColor4sv && !LoadFailure("glColor4sv", onfail)) return FALSE;
 lpglColor4iv = (LPGLCOLOR4IV)LoadType0("glColor4iv"); if(!lpglColor4iv && !LoadFailure("glColor4iv", onfail)) return FALSE;
 lpglColor4fv = (LPGLCOLOR4FV)LoadType0("glColor4fv"); if(!lpglColor4fv && !LoadFailure("glColor4fv", onfail)) return FALSE;
 lpglColor4dv = (LPGLCOLOR4DV)LoadType0("glColor4dv"); if(!lpglColor4dv && !LoadFailure("glColor4dv", onfail)) return FALSE;
 lpglColor4ubv = (LPGLCOLOR4UBV)LoadType0("glColor4ubv"); if(!lpglColor4ubv && !LoadFailure("glColor4ubv", onfail)) return FALSE;
 lpglColor4usv = (LPGLCOLOR4USV)LoadType0("glColor4usv"); if(!lpglColor4usv && !LoadFailure("glColor4usv", onfail)) return FALSE;
 lpglColor4uiv = (LPGLCOLOR4UIV)LoadType0("glColor4uiv"); if(!lpglColor4uiv && !LoadFailure("glColor4uiv", onfail)) return FALSE;
 lpglIndexs = (LPGLINDEXS)LoadType0("glIndexs"); if(!lpglIndexs && !LoadFailure("glIndexs", onfail)) return FALSE;
 lpglIndexi = (LPGLINDEXI)LoadType0("glIndexi"); if(!lpglIndexi && !LoadFailure("glIndexi", onfail)) return FALSE;
 lpglIndexf = (LPGLINDEXF)LoadType0("glIndexf"); if(!lpglIndexf && !LoadFailure("glIndexf", onfail)) return FALSE;
 lpglIndexd = (LPGLINDEXD)LoadType0("glIndexd"); if(!lpglIndexd && !LoadFailure("glIndexd", onfail)) return FALSE;
 lpglIndexub = (LPGLINDEXUB)LoadType0("glIndexub"); if(!lpglIndexub && !LoadFailure("glIndexub", onfail)) return FALSE;
 lpglIndexsv = (LPGLINDEXSV)LoadType0("glIndexsv"); if(!lpglIndexsv && !LoadFailure("glIndexsv", onfail)) return FALSE;
 lpglIndexiv = (LPGLINDEXIV)LoadType0("glIndexiv"); if(!lpglIndexiv && !LoadFailure("glIndexiv", onfail)) return FALSE;
 lpglIndexfv = (LPGLINDEXFV)LoadType0("glIndexfv"); if(!lpglIndexfv && !LoadFailure("glIndexfv", onfail)) return FALSE;
 lpglIndexdv = (LPGLINDEXDV)LoadType0("glIndexdv"); if(!lpglIndexdv && !LoadFailure("glIndexdv", onfail)) return FALSE;
 lpglIndexubv = (LPGLINDEXUBV)LoadType0("glIndexubv"); if(!lpglIndexubv && !LoadFailure("glIndexubv", onfail)) return FALSE;
 lpglClientActiveTexture = (LPGLCLIENTACTIVETEXTURE)LoadType1("glClientActiveTexture"); if(!lpglClientActiveTexture && !LoadFailure("glClientActiveTexture", onfail)) return FALSE;
 lpglEdgeFlagPointer = (LPGLEDGEFLAGPOINTER)LoadType0("glEdgeFlagPointer"); if(!lpglEdgeFlagPointer && !LoadFailure("glEdgeFlagPointer", onfail)) return FALSE;
 lpglTexCoordPointer = (LPGLTEXCOORDPOINTER)LoadType0("glTexCoordPointer"); if(!lpglTexCoordPointer && !LoadFailure("glTexCoordPointer", onfail)) return FALSE;
 lpglColorPointer = (LPGLCOLORPOINTER)LoadType0("glColorPointer"); if(!lpglColorPointer && !LoadFailure("glColorPointer", onfail)) return FALSE;
 lpglIndexPointer = (LPGLINDEXPOINTER)LoadType0("glIndexPointer"); if(!lpglIndexPointer && !LoadFailure("glIndexPointer", onfail)) return FALSE;
 lpglNormalPointer = (LPGLNORMALPOINTER)LoadType0("glNormalPointer"); if(!lpglNormalPointer && !LoadFailure("glNormalPointer", onfail)) return FALSE;
 lpglVertexPointer = (LPGLVERTEXPOINTER)LoadType0("glVertexPointer"); if(!lpglVertexPointer && !LoadFailure("glVertexPointer", onfail)) return FALSE;
 lpglEnableClientState = (LPGLENABLECLIENTSTATE)LoadType0("glEnableClientState"); if(!lpglEnableClientState && !LoadFailure("glEnableClientState", onfail)) return FALSE;
 lpglDisableClientState = (LPGLDISABLECLIENTSTATE)LoadType0("glDisableClientState"); if(!lpglDisableClientState && !LoadFailure("glDisableClientState", onfail)) return FALSE;
 lpglArrayElement = (LPGLARRAYELEMENT)LoadType0("glArrayElement"); if(!lpglArrayElement && !LoadFailure("glArrayElement", onfail)) return FALSE;
 lpglDrawArrays = (LPGLDRAWARRAYS)LoadType0("glDrawArrays"); if(!lpglDrawArrays && !LoadFailure("glDrawArrays", onfail)) return FALSE;
 lpglDrawElements = (LPGLDRAWELEMENTS)LoadType0("glDrawElements"); if(!lpglDrawElements && !LoadFailure("glDrawElements", onfail)) return FALSE;
 lpglDrawRangeElements = (LPGLDRAWRANGEELEMENTS)LoadType1("glDrawRangeElements"); if(!lpglDrawRangeElements && !LoadFailure("glDrawRangeElements", onfail)) return FALSE;
 lpglInterleavedArrays = (LPGLINTERLEAVEDARRAYS)LoadType0("glInterleavedArrays"); if(!lpglInterleavedArrays && !LoadFailure("glInterleavedArrays", onfail)) return FALSE;
 lpglRects = (LPGLRECTS)LoadType0("glRects"); if(!lpglRects && !LoadFailure("glRects", onfail)) return FALSE;
 lpglRecti = (LPGLRECTI)LoadType0("glRecti"); if(!lpglRecti && !LoadFailure("glRecti", onfail)) return FALSE;
 lpglRectf = (LPGLRECTF)LoadType0("glRectf"); if(!lpglRectf && !LoadFailure("glRectf", onfail)) return FALSE;
 lpglRectd = (LPGLRECTD)LoadType0("glRectd"); if(!lpglRectd && !LoadFailure("glRectd", onfail)) return FALSE;
 lpglRectsv = (LPGLRECTSV)LoadType0("glRectsv"); if(!lpglRectsv && !LoadFailure("glRectsv", onfail)) return FALSE;
 lpglRectiv = (LPGLRECTIV)LoadType0("glRectiv"); if(!lpglRectiv && !LoadFailure("glRectiv", onfail)) return FALSE;
 lpglRectfv = (LPGLRECTFV)LoadType0("glRectfv"); if(!lpglRectfv && !LoadFailure("glRectfv", onfail)) return FALSE;
 lpglRectdv = (LPGLRECTDV)LoadType0("glRectdv"); if(!lpglRectdv && !LoadFailure("glRectdv", onfail)) return FALSE;
 lpglDepthRange = (LPGLDEPTHRANGE)LoadType0("glDepthRange"); if(!lpglDepthRange && !LoadFailure("glDepthRange", onfail)) return FALSE;
 lpglViewport = (LPGLVIEWPORT)LoadType0("glViewport"); if(!lpglViewport && !LoadFailure("glViewport", onfail)) return FALSE;
 lpglMatrixMode = (LPGLMATRIXMODE)LoadType0("glMatrixMode"); if(!lpglMatrixMode && !LoadFailure("glMatrixMode", onfail)) return FALSE;
 lpglLoadMatrixf = (LPGLLOADMATRIXF)LoadType0("glLoadMatrixf"); if(!lpglLoadMatrixf && !LoadFailure("glLoadMatrixf", onfail)) return FALSE;
 lpglLoadMatrixd = (LPGLLOADMATRIXD)LoadType0("glLoadMatrixd"); if(!lpglLoadMatrixd && !LoadFailure("glLoadMatrixd", onfail)) return FALSE;
 lpglMultMatrixf = (LPGLMULTMATRIXF)LoadType0("glMultMatrixf"); if(!lpglMultMatrixf && !LoadFailure("glMultMatrixf", onfail)) return FALSE;
 lpglMultMatrixd = (LPGLMULTMATRIXD)LoadType0("glMultMatrixd"); if(!lpglMultMatrixd && !LoadFailure("glMultMatrixd", onfail)) return FALSE;
 lpglLoadIdentity = (LPGLLOADIDENTITY)LoadType0("glLoadIdentity"); if(!lpglLoadIdentity && !LoadFailure("glLoadIdentity", onfail)) return FALSE;
 lpglRotatef = (LPGLROTATEF)LoadType0("glRotatef"); if(!lpglRotatef && !LoadFailure("glRotatef", onfail)) return FALSE;
 lpglRotated = (LPGLROTATED)LoadType0("glRotated"); if(!lpglRotated && !LoadFailure("glRotated", onfail)) return FALSE;
 lpglTranslatef = (LPGLTRANSLATEF)LoadType0("glTranslatef"); if(!lpglTranslatef && !LoadFailure("glTranslatef", onfail)) return FALSE;
 lpglTranslated = (LPGLTRANSLATED)LoadType0("glTranslated"); if(!lpglTranslated && !LoadFailure("glTranslated", onfail)) return FALSE;
 lpglScalef = (LPGLSCALEF)LoadType0("glScalef"); if(!lpglScalef && !LoadFailure("glScalef", onfail)) return FALSE;
 lpglScaled = (LPGLSCALED)LoadType0("glScaled"); if(!lpglScaled && !LoadFailure("glScaled", onfail)) return FALSE;
 lpglFrustum = (LPGLFRUSTUM)LoadType0("glFrustum"); if(!lpglFrustum && !LoadFailure("glFrustum", onfail)) return FALSE;
 lpglOrtho = (LPGLORTHO)LoadType0("glOrtho"); if(!lpglOrtho && !LoadFailure("glOrtho", onfail)) return FALSE;
 lpglPushMatrix = (LPGLPUSHMATRIX)LoadType0("glPushMatrix"); if(!lpglPushMatrix && !LoadFailure("glPushMatrix", onfail)) return FALSE;
 lpglActiveTexture = (LPGLACTIVETEXTURE)LoadType1("glActiveTexture"); if(!lpglActiveTexture && !LoadFailure("glActiveTexture", onfail)) return FALSE;
 lpglPopMatrix = (LPGLPOPMATRIX)LoadType0("glPopMatrix"); if(!lpglPopMatrix && !LoadFailure("glPopMatrix", onfail)) return FALSE;
 lpglEnable = (LPGLENABLE)LoadType0("glEnable"); if(!lpglEnable && !LoadFailure("glEnable", onfail)) return FALSE;
 lpglDisable = (LPGLDISABLE)LoadType0("glDisable"); if(!lpglDisable && !LoadFailure("glDisable", onfail)) return FALSE;
 lpglTexGeni = (LPGLTEXGENI)LoadType0("glTexGeni"); if(!lpglTexGeni && !LoadFailure("glTexGeni", onfail)) return FALSE;
 lpglTexGenf = (LPGLTEXGENF)LoadType0("glTexGenf"); if(!lpglTexGenf && !LoadFailure("glTexGenf", onfail)) return FALSE;
 lpglTexGend = (LPGLTEXGEND)LoadType0("glTexGend"); if(!lpglTexGend && !LoadFailure("glTexGend", onfail)) return FALSE;
 lpglTexGeniv = (LPGLTEXGENIV)LoadType0("glTexGeniv"); if(!lpglTexGeniv && !LoadFailure("glTexGeniv", onfail)) return FALSE;
 lpglTexGenfv = (LPGLTEXGENFV)LoadType0("glTexGenfv"); if(!lpglTexGenfv && !LoadFailure("glTexGenfv", onfail)) return FALSE;
 lpglTexGendv = (LPGLTEXGENDV)LoadType0("glTexGendv"); if(!lpglTexGendv && !LoadFailure("glTexGendv", onfail)) return FALSE;
 lpglClipPlane = (LPGLCLIPPLANE)LoadType0("glClipPlane"); if(!lpglClipPlane && !LoadFailure("glClipPlane", onfail)) return FALSE;
 lpglRasterPos2s = (LPGLRASTERPOS2S)LoadType0("glRasterPos2s"); if(!lpglRasterPos2s && !LoadFailure("glRasterPos2s", onfail)) return FALSE;
 lpglRasterPos2i = (LPGLRASTERPOS2I)LoadType0("glRasterPos2i"); if(!lpglRasterPos2i && !LoadFailure("glRasterPos2i", onfail)) return FALSE;
 lpglRasterPos2f = (LPGLRASTERPOS2F)LoadType0("glRasterPos2f"); if(!lpglRasterPos2f && !LoadFailure("glRasterPos2f", onfail)) return FALSE;
 lpglRasterPos2d = (LPGLRASTERPOS2D)LoadType0("glRasterPos2d"); if(!lpglRasterPos2d && !LoadFailure("glRasterPos2d", onfail)) return FALSE;
 lpglRasterPos3s = (LPGLRASTERPOS3S)LoadType0("glRasterPos3s"); if(!lpglRasterPos3s && !LoadFailure("glRasterPos3s", onfail)) return FALSE;
 lpglRasterPos3i = (LPGLRASTERPOS3I)LoadType0("glRasterPos3i"); if(!lpglRasterPos3i && !LoadFailure("glRasterPos3i", onfail)) return FALSE;
 lpglRasterPos3f = (LPGLRASTERPOS3F)LoadType0("glRasterPos3f"); if(!lpglRasterPos3f && !LoadFailure("glRasterPos3f", onfail)) return FALSE;
 lpglRasterPos3d = (LPGLRASTERPOS3D)LoadType0("glRasterPos3d"); if(!lpglRasterPos3d && !LoadFailure("glRasterPos3d", onfail)) return FALSE;
 lpglRasterPos4s = (LPGLRASTERPOS4S)LoadType0("glRasterPos4s"); if(!lpglRasterPos4s && !LoadFailure("glRasterPos4s", onfail)) return FALSE;
 lpglRasterPos4i = (LPGLRASTERPOS4I)LoadType0("glRasterPos4i"); if(!lpglRasterPos4i && !LoadFailure("glRasterPos4i", onfail)) return FALSE;
 lpglRasterPos4f = (LPGLRASTERPOS4F)LoadType0("glRasterPos4f"); if(!lpglRasterPos4f && !LoadFailure("glRasterPos4f", onfail)) return FALSE;
 lpglRasterPos4d = (LPGLRASTERPOS4D)LoadType0("glRasterPos4d"); if(!lpglRasterPos4d && !LoadFailure("glRasterPos4d", onfail)) return FALSE;
 lpglRasterPos2sv = (LPGLRASTERPOS2SV)LoadType0("glRasterPos2sv"); if(!lpglRasterPos2sv && !LoadFailure("glRasterPos2sv", onfail)) return FALSE;
 lpglRasterPos2iv = (LPGLRASTERPOS2IV)LoadType0("glRasterPos2iv"); if(!lpglRasterPos2iv && !LoadFailure("glRasterPos2iv", onfail)) return FALSE;
 lpglRasterPos2fv = (LPGLRASTERPOS2FV)LoadType0("glRasterPos2fv"); if(!lpglRasterPos2fv && !LoadFailure("glRasterPos2fv", onfail)) return FALSE;
 lpglRasterPos2dv = (LPGLRASTERPOS2DV)LoadType0("glRasterPos2dv"); if(!lpglRasterPos2dv && !LoadFailure("glRasterPos2dv", onfail)) return FALSE;
 lpglRasterPos3sv = (LPGLRASTERPOS3SV)LoadType0("glRasterPos3sv"); if(!lpglRasterPos3sv && !LoadFailure("glRasterPos3sv", onfail)) return FALSE;
 lpglRasterPos3iv = (LPGLRASTERPOS3IV)LoadType0("glRasterPos3iv"); if(!lpglRasterPos3iv && !LoadFailure("glRasterPos3iv", onfail)) return FALSE;
 lpglRasterPos3fv = (LPGLRASTERPOS3FV)LoadType0("glRasterPos3fv"); if(!lpglRasterPos3fv && !LoadFailure("glRasterPos3fv", onfail)) return FALSE;
 lpglRasterPos3dv = (LPGLRASTERPOS3DV)LoadType0("glRasterPos3dv"); if(!lpglRasterPos3dv && !LoadFailure("glRasterPos3dv", onfail)) return FALSE;
 lpglRasterPos4sv = (LPGLRASTERPOS4SV)LoadType0("glRasterPos4sv"); if(!lpglRasterPos4sv && !LoadFailure("glRasterPos4sv", onfail)) return FALSE;
 lpglRasterPos4iv = (LPGLRASTERPOS4IV)LoadType0("glRasterPos4iv"); if(!lpglRasterPos4iv && !LoadFailure("glRasterPos4iv", onfail)) return FALSE;
 lpglRasterPos4fv = (LPGLRASTERPOS4FV)LoadType0("glRasterPos4fv"); if(!lpglRasterPos4fv && !LoadFailure("glRasterPos4fv", onfail)) return FALSE;
 lpglRasterPos4dv = (LPGLRASTERPOS4DV)LoadType0("glRasterPos4dv"); if(!lpglRasterPos4dv && !LoadFailure("glRasterPos4dv", onfail)) return FALSE;
 lpglFrontFace = (LPGLFRONTFACE)LoadType0("glFrontFace"); if(!lpglFrontFace && !LoadFailure("glFrontFace", onfail)) return FALSE;
 lpglMateriali = (LPGLMATERIALI)LoadType0("glMateriali"); if(!lpglMateriali && !LoadFailure("glMateriali", onfail)) return FALSE;
 lpglMaterialf = (LPGLMATERIALF)LoadType0("glMaterialf"); if(!lpglMaterialf && !LoadFailure("glMaterialf", onfail)) return FALSE;
 lpglMaterialiv = (LPGLMATERIALIV)LoadType0("glMaterialiv"); if(!lpglMaterialiv && !LoadFailure("glMaterialiv", onfail)) return FALSE;
 lpglMaterialfv = (LPGLMATERIALFV)LoadType0("glMaterialfv"); if(!lpglMaterialfv && !LoadFailure("glMaterialfv", onfail)) return FALSE;
 lpglLighti = (LPGLLIGHTI)LoadType0("glLighti"); if(!lpglLighti && !LoadFailure("glLighti", onfail)) return FALSE;
 lpglLightf = (LPGLLIGHTF)LoadType0("glLightf"); if(!lpglLightf && !LoadFailure("glLightf", onfail)) return FALSE;
 lpglLightiv = (LPGLLIGHTIV)LoadType0("glLightiv"); if(!lpglLightiv && !LoadFailure("glLightiv", onfail)) return FALSE;
 lpglLightfv = (LPGLLIGHTFV)LoadType0("glLightfv"); if(!lpglLightfv && !LoadFailure("glLightfv", onfail)) return FALSE;
 lpglLightModeli = (LPGLLIGHTMODELI)LoadType0("glLightModeli"); if(!lpglLightModeli && !LoadFailure("glLightModeli", onfail)) return FALSE;
 lpglLightModelf = (LPGLLIGHTMODELF)LoadType0("glLightModelf"); if(!lpglLightModelf && !LoadFailure("glLightModelf", onfail)) return FALSE;
 lpglLightModeliv = (LPGLLIGHTMODELIV)LoadType0("glLightModeliv"); if(!lpglLightModeliv && !LoadFailure("glLightModeliv", onfail)) return FALSE;
 lpglLightModelfv = (LPGLLIGHTMODELFV)LoadType0("glLightModelfv"); if(!lpglLightModelfv && !LoadFailure("glLightModelfv", onfail)) return FALSE;
 lpglColorMaterial = (LPGLCOLORMATERIAL)LoadType0("glColorMaterial"); if(!lpglColorMaterial && !LoadFailure("glColorMaterial", onfail)) return FALSE;
 lpglShadeModel = (LPGLSHADEMODEL)LoadType0("glShadeModel"); if(!lpglShadeModel && !LoadFailure("glShadeModel", onfail)) return FALSE;
 lpglPointSize = (LPGLPOINTSIZE)LoadType0("glPointSize"); if(!lpglPointSize && !LoadFailure("glPointSize", onfail)) return FALSE;
 lpglLineWidth = (LPGLLINEWIDTH)LoadType0("glLineWidth"); if(!lpglLineWidth && !LoadFailure("glLineWidth", onfail)) return FALSE;
 lpglLineStipple = (LPGLLINESTIPPLE)LoadType0("glLineStipple"); if(!lpglLineStipple && !LoadFailure("glLineStipple", onfail)) return FALSE;
 lpglCullFace = (LPGLCULLFACE)LoadType0("glCullFace"); if(!lpglCullFace && !LoadFailure("glCullFace", onfail)) return FALSE;
 lpglPolygonStipple = (LPGLPOLYGONSTIPPLE)LoadType0("glPolygonStipple"); if(!lpglPolygonStipple && !LoadFailure("glPolygonStipple", onfail)) return FALSE;
 lpglPolygonMode = (LPGLPOLYGONMODE)LoadType0("glPolygonMode"); if(!lpglPolygonMode && !LoadFailure("glPolygonMode", onfail)) return FALSE;
 lpglPolygonOffset = (LPGLPOLYGONOFFSET)LoadType0("glPolygonOffset"); if(!lpglPolygonOffset && !LoadFailure("glPolygonOffset", onfail)) return FALSE;
 lpglPixelStorei = (LPGLPIXELSTOREI)LoadType0("glPixelStorei"); if(!lpglPixelStorei && !LoadFailure("glPixelStorei", onfail)) return FALSE;
 lpglPixelStoref = (LPGLPIXELSTOREF)LoadType0("glPixelStoref"); if(!lpglPixelStoref && !LoadFailure("glPixelStoref", onfail)) return FALSE;
 lpglPixelTransferi = (LPGLPIXELTRANSFERI)LoadType0("glPixelTransferi"); if(!lpglPixelTransferi && !LoadFailure("glPixelTransferi", onfail)) return FALSE;
 lpglPixelTransferf = (LPGLPIXELTRANSFERF)LoadType0("glPixelTransferf"); if(!lpglPixelTransferf && !LoadFailure("glPixelTransferf", onfail)) return FALSE;
 lpglPixelMapuiv = (LPGLPIXELMAPUIV)LoadType0("glPixelMapuiv"); if(!lpglPixelMapuiv && !LoadFailure("glPixelMapuiv", onfail)) return FALSE;
 lpglPixelMapusv = (LPGLPIXELMAPUSV)LoadType0("glPixelMapusv"); if(!lpglPixelMapusv && !LoadFailure("glPixelMapusv", onfail)) return FALSE;
 lpglPixelMapfv = (LPGLPIXELMAPFV)LoadType0("glPixelMapfv"); if(!lpglPixelMapfv && !LoadFailure("glPixelMapfv", onfail)) return FALSE;
 lpglColorTable = (LPGLCOLORTABLE)LoadType0("glColorTable"); if(!lpglColorTable && !LoadFailure("glColorTable", onfail)) return FALSE;
 lpglColorTableParameteriv = (LPGLCOLORTABLEPARAMETERIV)LoadType0("glColorTableParameteriv"); if(!lpglColorTableParameteriv && !LoadFailure("glColorTableParameteriv", onfail)) return FALSE;
 lpglColorTableParameterfv = (LPGLCOLORTABLEPARAMETERFV)LoadType0("glColorTableParameterfv"); if(!lpglColorTableParameterfv && !LoadFailure("glColorTableParameterfv", onfail)) return FALSE;
 lpglCopyColorTable = (LPGLCOPYCOLORTABLE)LoadType0("glCopyColorTable"); if(!lpglCopyColorTable && !LoadFailure("glCopyColorTable", onfail)) return FALSE;
 lpglColorSubTable = (LPGLCOLORSUBTABLE)LoadType0("glColorSubTable"); if(!lpglColorSubTable && !LoadFailure("glColorSubTable", onfail)) return FALSE;
 lpglCopyColorSubTable = (LPGLCOPYCOLORSUBTABLE)LoadType0("glCopyColorSubTable"); if(!lpglCopyColorSubTable && !LoadFailure("glCopyColorSubTable", onfail)) return FALSE;
 lpglConvolutionFilter2D = (LPGLCONVOLUTIONFILTER2D)LoadType0("glConvolutionFilter2D"); if(!lpglConvolutionFilter2D && !LoadFailure("glConvolutionFilter2D", onfail)) return FALSE;
 lpglConvolutionParameteriv = (LPGLCONVOLUTIONPARAMETERIV)LoadType0("glConvolutionParameteriv"); if(!lpglConvolutionParameteriv && !LoadFailure("glConvolutionParameteriv", onfail)) return FALSE;
 lpglConvolutionParameterfv = (LPGLCONVOLUTIONPARAMETERFV)LoadType0("glConvolutionParameterfv"); if(!lpglConvolutionParameterfv && !LoadFailure("glConvolutionParameterfv", onfail)) return FALSE;
 lpglConvolutionFilter1D = (LPGLCONVOLUTIONFILTER1D)LoadType0("glConvolutionFilter1D"); if(!lpglConvolutionFilter1D && !LoadFailure("glConvolutionFilter1D", onfail)) return FALSE;
 lpglSeparableFilter2D = (LPGLSEPARABLEFILTER2D)LoadType0("glSeparableFilter2D"); if(!lpglSeparableFilter2D && !LoadFailure("glSeparableFilter2D", onfail)) return FALSE;
 lpglCopyConvolutionFilter2D = (LPGLCOPYCONVOLUTIONFILTER2D)LoadType0("glCopyConvolutionFilter2D"); if(!lpglCopyConvolutionFilter2D && !LoadFailure("glCopyConvolutionFilter2D", onfail)) return FALSE;
 lpglCopyConvolutionFilter1D = (LPGLCOPYCONVOLUTIONFILTER1D)LoadType0("glCopyConvolutionFilter1D"); if(!lpglCopyConvolutionFilter1D && !LoadFailure("glCopyConvolutionFilter1D", onfail)) return FALSE;
 lpglHistogram = (LPGLHISTOGRAM)LoadType0("glHistogram"); if(!lpglHistogram && !LoadFailure("glHistogram", onfail)) return FALSE;
 lpglMinmax = (LPGLMINMAX)LoadType0("glMinmax"); if(!lpglMinmax && !LoadFailure("glMinmax", onfail)) return FALSE;
 lpglDrawPixels = (LPGLDRAWPIXELS)LoadType0("glDrawPixels"); if(!lpglDrawPixels && !LoadFailure("glDrawPixels", onfail)) return FALSE;
 lpglPixelZoom = (LPGLPIXELZOOM)LoadType0("glPixelZoom"); if(!lpglPixelZoom && !LoadFailure("glPixelZoom", onfail)) return FALSE;
 lpglConvolutionParameteri = (LPGLCONVOLUTIONPARAMETERI)LoadType0("glConvolutionParameteri"); if(!lpglConvolutionParameteri && !LoadFailure("glConvolutionParameteri", onfail)) return FALSE;
 lpglConvolutionParameterf = (LPGLCONVOLUTIONPARAMETERF)LoadType0("glConvolutionParameterf"); if(!lpglConvolutionParameterf && !LoadFailure("glConvolutionParameterf", onfail)) return FALSE;
 lpglBitmap = (LPGLBITMAP)LoadType0("glBitmap"); if(!lpglBitmap && !LoadFailure("glBitmap", onfail)) return FALSE;
 lpglTexImage3D = (LPGLTEXIMAGE3D)LoadType1("glTexImage3D"); if(!lpglTexImage3D && !LoadFailure("glTexImage3D", onfail)) return FALSE;
 lpglTexImage2D = (LPGLTEXIMAGE2D)LoadType0("glTexImage2D"); if(!lpglTexImage2D && !LoadFailure("glTexImage2D", onfail)) return FALSE;
 lpglTexImage1D = (LPGLTEXIMAGE1D)LoadType0("glTexImage1D"); if(!lpglTexImage1D && !LoadFailure("glTexImage1D", onfail)) return FALSE;
 lpglCopyTexImage2D = (LPGLCOPYTEXIMAGE2D)LoadType0("glCopyTexImage2D"); if(!lpglCopyTexImage2D && !LoadFailure("glCopyTexImage2D", onfail)) return FALSE;
 lpglCopyTexImage1D = (LPGLCOPYTEXIMAGE1D)LoadType0("glCopyTexImage1D"); if(!lpglCopyTexImage1D && !LoadFailure("glCopyTexImage1D", onfail)) return FALSE;
 lpglTexSubImage3D = (LPGLTEXSUBIMAGE3D)LoadType1("glTexSubImage3D"); if(!lpglTexSubImage3D && !LoadFailure("glTexSubImage3D", onfail)) return FALSE;
 lpglTexSubImage2D = (LPGLTEXSUBIMAGE2D)LoadType0("glTexSubImage2D"); if(!lpglTexSubImage2D && !LoadFailure("glTexSubImage2D", onfail)) return FALSE;
 lpglTexSubImage1D = (LPGLTEXSUBIMAGE1D)LoadType0("glTexSubImage1D"); if(!lpglTexSubImage1D && !LoadFailure("glTexSubImage1D", onfail)) return FALSE;
 lpglCopyTexSubImage3D = (LPGLCOPYTEXSUBIMAGE3D)LoadType1("glCopyTexSubImage3D"); if(!lpglCopyTexSubImage3D && !LoadFailure("glCopyTexSubImage3D", onfail)) return FALSE;
 lpglCopyTexSubImage2D = (LPGLCOPYTEXSUBIMAGE2D)LoadType0("glCopyTexSubImage2D"); if(!lpglCopyTexSubImage2D && !LoadFailure("glCopyTexSubImage2D", onfail)) return FALSE;
 lpglCopyTexSubImage1D = (LPGLCOPYTEXSUBIMAGE1D)LoadType0("glCopyTexSubImage1D"); if(!lpglCopyTexSubImage1D && !LoadFailure("glCopyTexSubImage1D", onfail)) return FALSE;
 lpglTexParameteri = (LPGLTEXPARAMETERI)LoadType0("glTexParameteri"); if(!lpglTexParameteri && !LoadFailure("glTexParameteri", onfail)) return FALSE;
 lpglTexParameterf = (LPGLTEXPARAMETERF)LoadType0("glTexParameterf"); if(!lpglTexParameterf && !LoadFailure("glTexParameterf", onfail)) return FALSE;
 lpglTexParameteriv = (LPGLTEXPARAMETERIV)LoadType0("glTexParameteriv"); if(!lpglTexParameteriv && !LoadFailure("glTexParameteriv", onfail)) return FALSE;
 lpglTexParameterfv = (LPGLTEXPARAMETERFV)LoadType0("glTexParameterfv"); if(!lpglTexParameterfv && !LoadFailure("glTexParameterfv", onfail)) return FALSE;
 lpglBindTexture = (LPGLBINDTEXTURE)LoadType0("glBindTexture"); if(!lpglBindTexture && !LoadFailure("glBindTexture", onfail)) return FALSE;
 lpglDeleteTextures = (LPGLDELETETEXTURES)LoadType0("glDeleteTextures"); if(!lpglDeleteTextures && !LoadFailure("glDeleteTextures", onfail)) return FALSE;
 lpglGenTextures = (LPGLGENTEXTURES)LoadType0("glGenTextures"); if(!lpglGenTextures && !LoadFailure("glGenTextures", onfail)) return FALSE;
 lpglAreTexturesResident = (LPGLARETEXTURESRESIDENT)LoadType0("glAreTexturesResident"); if(!lpglAreTexturesResident && !LoadFailure("glAreTexturesResident", onfail)) return FALSE;
 lpglPrioritizeTextures = (LPGLPRIORITIZETEXTURES)LoadType0("glPrioritizeTextures"); if(!lpglPrioritizeTextures && !LoadFailure("glPrioritizeTextures", onfail)) return FALSE;
 lpglTexEnvi = (LPGLTEXENVI)LoadType0("glTexEnvi"); if(!lpglTexEnvi && !LoadFailure("glTexEnvi", onfail)) return FALSE;
 lpglTexEnvf = (LPGLTEXENVF)LoadType0("glTexEnvf"); if(!lpglTexEnvf && !LoadFailure("glTexEnvf", onfail)) return FALSE;
 lpglTexEnviv = (LPGLTEXENVIV)LoadType0("glTexEnviv"); if(!lpglTexEnviv && !LoadFailure("glTexEnviv", onfail)) return FALSE;
 lpglTexEnvfv = (LPGLTEXENVFV)LoadType0("glTexEnvfv"); if(!lpglTexEnvfv && !LoadFailure("glTexEnvfv", onfail)) return FALSE;
 lpglFogi = (LPGLFOGI)LoadType0("glFogi"); if(!lpglFogi && !LoadFailure("glFogi", onfail)) return FALSE;
 lpglFogf = (LPGLFOGF)LoadType0("glFogf"); if(!lpglFogf && !LoadFailure("glFogf", onfail)) return FALSE;
 lpglFogiv = (LPGLFOGIV)LoadType0("glFogiv"); if(!lpglFogiv && !LoadFailure("glFogiv", onfail)) return FALSE;
 lpglFogfv = (LPGLFOGFV)LoadType0("glFogfv"); if(!lpglFogfv && !LoadFailure("glFogfv", onfail)) return FALSE;
 lpglScissor = (LPGLSCISSOR)LoadType0("glScissor"); if(!lpglScissor && !LoadFailure("glScissor", onfail)) return FALSE;
 lpglAlphaFunc = (LPGLALPHAFUNC)LoadType0("glAlphaFunc"); if(!lpglAlphaFunc && !LoadFailure("glAlphaFunc", onfail)) return FALSE;
 lpglStencilFunc = (LPGLSTENCILFUNC)LoadType0("glStencilFunc"); if(!lpglStencilFunc && !LoadFailure("glStencilFunc", onfail)) return FALSE;
 lpglStencilOp = (LPGLSTENCILOP)LoadType0("glStencilOp"); if(!lpglStencilOp && !LoadFailure("glStencilOp", onfail)) return FALSE;
 lpglDepthFunc = (LPGLDEPTHFUNC)LoadType0("glDepthFunc"); if(!lpglDepthFunc && !LoadFailure("glDepthFunc", onfail)) return FALSE;
 lpglBlendColor = (LPGLBLENDCOLOR)LoadType1("glBlendColor"); if(!lpglBlendColor && !LoadFailure("glBlendColor", onfail)) return FALSE;
 lpglBlendEquation = (LPGLBLENDEQUATION)LoadType1("glBlendEquation"); if(!lpglBlendEquation && !LoadFailure("glBlendEquation", onfail)) return FALSE;
 lpglBlendFunc = (LPGLBLENDFUNC)LoadType0("glBlendFunc"); if(!lpglBlendFunc && !LoadFailure("glBlendFunc", onfail)) return FALSE;
 lpglLogicOp = (LPGLLOGICOP)LoadType0("glLogicOp"); if(!lpglLogicOp && !LoadFailure("glLogicOp", onfail)) return FALSE;
 lpglDrawBuffer = (LPGLDRAWBUFFER)LoadType0("glDrawBuffer"); if(!lpglDrawBuffer && !LoadFailure("glDrawBuffer", onfail)) return FALSE;
 lpglIndexMask = (LPGLINDEXMASK)LoadType0("glIndexMask"); if(!lpglIndexMask && !LoadFailure("glIndexMask", onfail)) return FALSE;
 lpglColorMask = (LPGLCOLORMASK)LoadType0("glColorMask"); if(!lpglColorMask && !LoadFailure("glColorMask", onfail)) return FALSE;
 lpglDepthMask = (LPGLDEPTHMASK)LoadType0("glDepthMask"); if(!lpglDepthMask && !LoadFailure("glDepthMask", onfail)) return FALSE;
 lpglStencilMask = (LPGLSTENCILMASK)LoadType0("glStencilMask"); if(!lpglStencilMask && !LoadFailure("glStencilMask", onfail)) return FALSE;
 lpglClear = (LPGLCLEAR)LoadType0("glClear"); if(!lpglClear && !LoadFailure("glClear", onfail)) return FALSE;
 lpglClearColor = (LPGLCLEARCOLOR)LoadType0("glClearColor"); if(!lpglClearColor && !LoadFailure("glClearColor", onfail)) return FALSE;
 lpglClearIndex = (LPGLCLEARINDEX)LoadType0("glClearIndex"); if(!lpglClearIndex && !LoadFailure("glClearIndex", onfail)) return FALSE;
 lpglClearDepth = (LPGLCLEARDEPTH)LoadType0("glClearDepth"); if(!lpglClearDepth && !LoadFailure("glClearDepth", onfail)) return FALSE;
 lpglClearStencil = (LPGLCLEARSTENCIL)LoadType0("glClearStencil"); if(!lpglClearStencil && !LoadFailure("glClearStencil", onfail)) return FALSE;
 lpglClearAccum = (LPGLCLEARACCUM)LoadType0("glClearAccum"); if(!lpglClearAccum && !LoadFailure("glClearAccum", onfail)) return FALSE;
 lpglAccum = (LPGLACCUM)LoadType0("glAccum"); if(!lpglAccum && !LoadFailure("glAccum", onfail)) return FALSE;
 lpglReadPixels = (LPGLREADPIXELS)LoadType0("glReadPixels"); if(!lpglReadPixels && !LoadFailure("glReadPixels", onfail)) return FALSE;
 lpglReadBuffer = (LPGLREADBUFFER)LoadType0("glReadBuffer"); if(!lpglReadBuffer && !LoadFailure("glReadBuffer", onfail)) return FALSE;
 lpglCopyPixels = (LPGLCOPYPIXELS)LoadType0("glCopyPixels"); if(!lpglCopyPixels && !LoadFailure("glCopyPixels", onfail)) return FALSE;
 lpglMap1f = (LPGLMAP1F)LoadType0("glMap1f"); if(!lpglMap1f && !LoadFailure("glMap1f", onfail)) return FALSE;
 lpglMap1d = (LPGLMAP1D)LoadType0("glMap1d"); if(!lpglMap1d && !LoadFailure("glMap1d", onfail)) return FALSE;
 lpglMap2f = (LPGLMAP2F)LoadType0("glMap2f"); if(!lpglMap2f && !LoadFailure("glMap2f", onfail)) return FALSE;
 lpglMap2d = (LPGLMAP2D)LoadType0("glMap2d"); if(!lpglMap2d && !LoadFailure("glMap2d", onfail)) return FALSE;
 lpglEvalCoord1f = (LPGLEVALCOORD1F)LoadType0("glEvalCoord1f"); if(!lpglEvalCoord1f && !LoadFailure("glEvalCoord1f", onfail)) return FALSE;
 lpglEvalCoord1d = (LPGLEVALCOORD1D)LoadType0("glEvalCoord1d"); if(!lpglEvalCoord1d && !LoadFailure("glEvalCoord1d", onfail)) return FALSE;
 lpglEvalCoord2f = (LPGLEVALCOORD2F)LoadType0("glEvalCoord2f"); if(!lpglEvalCoord2f && !LoadFailure("glEvalCoord2f", onfail)) return FALSE;
 lpglEvalCoord2d = (LPGLEVALCOORD2D)LoadType0("glEvalCoord2d"); if(!lpglEvalCoord2d && !LoadFailure("glEvalCoord2d", onfail)) return FALSE;
 lpglEvalCoord1fv = (LPGLEVALCOORD1FV)LoadType0("glEvalCoord1fv"); if(!lpglEvalCoord1fv && !LoadFailure("glEvalCoord1fv", onfail)) return FALSE;
 lpglEvalCoord1dv = (LPGLEVALCOORD1DV)LoadType0("glEvalCoord1dv"); if(!lpglEvalCoord1dv && !LoadFailure("glEvalCoord1dv", onfail)) return FALSE;
 lpglEvalCoord2fv = (LPGLEVALCOORD2FV)LoadType0("glEvalCoord2fv"); if(!lpglEvalCoord2fv && !LoadFailure("glEvalCoord2fv", onfail)) return FALSE;
 lpglEvalCoord2dv = (LPGLEVALCOORD2DV)LoadType0("glEvalCoord2dv"); if(!lpglEvalCoord2dv && !LoadFailure("glEvalCoord2dv", onfail)) return FALSE;
 lpglMapGrid1f = (LPGLMAPGRID1F)LoadType0("glMapGrid1f"); if(!lpglMapGrid1f && !LoadFailure("glMapGrid1f", onfail)) return FALSE;
 lpglMapGrid1d = (LPGLMAPGRID1D)LoadType0("glMapGrid1d"); if(!lpglMapGrid1d && !LoadFailure("glMapGrid1d", onfail)) return FALSE;
 lpglMapGrid2f = (LPGLMAPGRID2F)LoadType0("glMapGrid2f"); if(!lpglMapGrid2f && !LoadFailure("glMapGrid2f", onfail)) return FALSE;
 lpglMapGrid2d = (LPGLMAPGRID2D)LoadType0("glMapGrid2d"); if(!lpglMapGrid2d && !LoadFailure("glMapGrid2d", onfail)) return FALSE;
 lpglEvalMesh1 = (LPGLEVALMESH1)LoadType0("glEvalMesh1"); if(!lpglEvalMesh1 && !LoadFailure("glEvalMesh1", onfail)) return FALSE;
 lpglEvalMesh2 = (LPGLEVALMESH2)LoadType0("glEvalMesh2"); if(!lpglEvalMesh2 && !LoadFailure("glEvalMesh2", onfail)) return FALSE;
 lpglEvalPoint1 = (LPGLEVALPOINT1)LoadType0("glEvalPoint1"); if(!lpglEvalPoint1 && !LoadFailure("glEvalPoint1", onfail)) return FALSE;
 lpglEvalPoint2 = (LPGLEVALPOINT2)LoadType0("glEvalPoint2"); if(!lpglEvalPoint2 && !LoadFailure("glEvalPoint2", onfail)) return FALSE;
 lpglInitNames = (LPGLINITNAMES)LoadType0("glInitNames"); if(!lpglInitNames && !LoadFailure("glInitNames", onfail)) return FALSE;
 lpglPopName = (LPGLPOPNAME)LoadType0("glPopName"); if(!lpglPopName && !LoadFailure("glPopName", onfail)) return FALSE;
 lpglPushName = (LPGLPUSHNAME)LoadType0("glPushName"); if(!lpglPushName && !LoadFailure("glPushName", onfail)) return FALSE;
 lpglLoadName = (LPGLLOADNAME)LoadType0("glLoadName"); if(!lpglLoadName && !LoadFailure("glLoadName", onfail)) return FALSE;
 lpglRenderMode = (LPGLRENDERMODE)LoadType0("glRenderMode"); if(!lpglRenderMode && !LoadFailure("glRenderMode", onfail)) return FALSE;
 lpglSelectBuffer = (LPGLSELECTBUFFER)LoadType0("glSelectBuffer"); if(!lpglSelectBuffer && !LoadFailure("glSelectBuffer", onfail)) return FALSE;
 lpglFeedbackBuffer = (LPGLFEEDBACKBUFFER)LoadType0("glFeedbackBuffer"); if(!lpglFeedbackBuffer && !LoadFailure("glFeedbackBuffer", onfail)) return FALSE;
 lpglPassThrough = (LPGLPASSTHROUGH)LoadType0("glPassThrough"); if(!lpglPassThrough && !LoadFailure("glPassThrough", onfail)) return FALSE;
 lpglNewList = (LPGLNEWLIST)LoadType0("glNewList"); if(!lpglNewList && !LoadFailure("glNewList", onfail)) return FALSE;
 lpglEndList = (LPGLENDLIST)LoadType0("glEndList"); if(!lpglEndList && !LoadFailure("glEndList", onfail)) return FALSE;
 lpglCallList = (LPGLCALLLIST)LoadType0("glCallList"); if(!lpglCallList && !LoadFailure("glCallList", onfail)) return FALSE;
 lpglCallLists = (LPGLCALLLISTS)LoadType0("glCallLists"); if(!lpglCallLists && !LoadFailure("glCallLists", onfail)) return FALSE;
 lpglListBase = (LPGLLISTBASE)LoadType0("glListBase"); if(!lpglListBase && !LoadFailure("glListBase", onfail)) return FALSE;
 lpglGenLists = (LPGLGENLISTS)LoadType0("glGenLists"); if(!lpglGenLists && !LoadFailure("glGenLists", onfail)) return FALSE;
 lpglIsList = (LPGLISLIST)LoadType0("glIsList"); if(!lpglIsList && !LoadFailure("glIsList", onfail)) return FALSE;
 lpglDeleteLists = (LPGLDELETELISTS)LoadType0("glDeleteLists"); if(!lpglDeleteLists && !LoadFailure("glDeleteLists", onfail)) return FALSE;
 lpglFlush = (LPGLFLUSH)LoadType0("glFlush"); if(!lpglFlush && !LoadFailure("glFlush", onfail)) return FALSE;
 lpglFinish = (LPGLFINISH)LoadType0("glFinish"); if(!lpglFinish && !LoadFailure("glFinish", onfail)) return FALSE;
 lpglHint = (LPGLHINT)LoadType0("glHint"); if(!lpglHint && !LoadFailure("glHint", onfail)) return FALSE;
 lpglGetBooleanv = (LPGLGETBOOLEANV)LoadType0("glGetBooleanv"); if(!lpglGetBooleanv && !LoadFailure("glGetBooleanv", onfail)) return FALSE;
 lpglGetIntegerv = (LPGLGETINTEGERV)LoadType0("glGetIntegerv"); if(!lpglGetIntegerv && !LoadFailure("glGetIntegerv", onfail)) return FALSE;
 lpglGetFloatv = (LPGLGETFLOATV)LoadType0("glGetFloatv"); if(!lpglGetFloatv && !LoadFailure("glGetFloatv", onfail)) return FALSE;
 lpglGetDoublev = (LPGLGETDOUBLEV)LoadType0("glGetDoublev"); if(!lpglGetDoublev && !LoadFailure("glGetDoublev", onfail)) return FALSE;
 lpglIsEnabled = (LPGLISENABLED)LoadType0("glIsEnabled"); if(!lpglIsEnabled && !LoadFailure("glIsEnabled", onfail)) return FALSE;
 lpglGetClipPlane = (LPGLGETCLIPPLANE)LoadType0("glGetClipPlane"); if(!lpglGetClipPlane && !LoadFailure("glGetClipPlane", onfail)) return FALSE;
 lpglGetLightiv = (LPGLGETLIGHTIV)LoadType0("glGetLightiv"); if(!lpglGetLightiv && !LoadFailure("glGetLightiv", onfail)) return FALSE;
 lpglGetLightfv = (LPGLGETLIGHTFV)LoadType0("glGetLightfv"); if(!lpglGetLightfv && !LoadFailure("glGetLightfv", onfail)) return FALSE;
 lpglGetMaterialiv = (LPGLGETMATERIALIV)LoadType0("glGetMaterialiv"); if(!lpglGetMaterialiv && !LoadFailure("glGetMaterialiv", onfail)) return FALSE;
 lpglGetMaterialfv = (LPGLGETMATERIALFV)LoadType0("glGetMaterialfv"); if(!lpglGetMaterialfv && !LoadFailure("glGetMaterialfv", onfail)) return FALSE;
 lpglGetTexEnviv = (LPGLGETTEXENVIV)LoadType0("glGetTexEnviv"); if(!lpglGetTexEnviv && !LoadFailure("glGetTexEnviv", onfail)) return FALSE;
 lpglGetTexEnvfv = (LPGLGETTEXENVFV)LoadType0("glGetTexEnvfv"); if(!lpglGetTexEnvfv && !LoadFailure("glGetTexEnvfv", onfail)) return FALSE;
 lpglGetTexGeniv = (LPGLGETTEXGENIV)LoadType0("glGetTexGeniv"); if(!lpglGetTexGeniv && !LoadFailure("glGetTexGeniv", onfail)) return FALSE;
 lpglGetTexGenfv = (LPGLGETTEXGENFV)LoadType0("glGetTexGenfv"); if(!lpglGetTexGenfv && !LoadFailure("glGetTexGenfv", onfail)) return FALSE;
 lpglGetTexParameteriv = (LPGLGETTEXPARAMETERIV)LoadType0("glGetTexParameteriv"); if(!lpglGetTexParameteriv && !LoadFailure("glGetTexParameteriv", onfail)) return FALSE;
 lpglGetTexParameterfv = (LPGLGETTEXPARAMETERFV)LoadType0("glGetTexParameterfv"); if(!lpglGetTexParameterfv && !LoadFailure("glGetTexParameterfv", onfail)) return FALSE;
 lpglGetTexLevelParameteriv = (LPGLGETTEXLEVELPARAMETERIV)LoadType0("glGetTexLevelParameteriv"); if(!lpglGetTexLevelParameteriv && !LoadFailure("glGetTexLevelParameteriv", onfail)) return FALSE;
 lpglGetTexLevelParameterfv = (LPGLGETTEXLEVELPARAMETERFV)LoadType0("glGetTexLevelParameterfv"); if(!lpglGetTexLevelParameterfv && !LoadFailure("glGetTexLevelParameterfv", onfail)) return FALSE;
 lpglGetPixelMapuiv = (LPGLGETPIXELMAPUIV)LoadType0("glGetPixelMapuiv"); if(!lpglGetPixelMapuiv && !LoadFailure("glGetPixelMapuiv", onfail)) return FALSE;
 lpglGetPixelMapusv = (LPGLGETPIXELMAPUSV)LoadType0("glGetPixelMapusv"); if(!lpglGetPixelMapusv && !LoadFailure("glGetPixelMapusv", onfail)) return FALSE;
 lpglGetPixelMapfv = (LPGLGETPIXELMAPFV)LoadType0("glGetPixelMapfv"); if(!lpglGetPixelMapfv && !LoadFailure("glGetPixelMapfv", onfail)) return FALSE;
 lpglGetMapiv = (LPGLGETMAPIV)LoadType0("glGetMapiv"); if(!lpglGetMapiv && !LoadFailure("glGetMapiv", onfail)) return FALSE;
 lpglGetMapfv = (LPGLGETMAPFV)LoadType0("glGetMapfv"); if(!lpglGetMapfv && !LoadFailure("glGetMapfv", onfail)) return FALSE;
 lpglGetMapdv = (LPGLGETMAPDV)LoadType0("glGetMapdv"); if(!lpglGetMapdv && !LoadFailure("glGetMapdv", onfail)) return FALSE;
 lpglGetTexImage = (LPGLGETTEXIMAGE)LoadType0("glGetTexImage"); if(!lpglGetTexImage && !LoadFailure("glGetTexImage", onfail)) return FALSE;
 lpglIsTexture = (LPGLISTEXTURE)LoadType0("glIsTexture"); if(!lpglIsTexture && !LoadFailure("glIsTexture", onfail)) return FALSE;
 lpglGetPolygonStipple = (LPGLGETPOLYGONSTIPPLE)LoadType0("glGetPolygonStipple"); if(!lpglGetPolygonStipple && !LoadFailure("glGetPolygonStipple", onfail)) return FALSE;
 lpglGetColorTable = (LPGLGETCOLORTABLE)LoadType0("glGetColorTable"); if(!lpglGetColorTable && !LoadFailure("glGetColorTable", onfail)) return FALSE;
 lpglGetColorTableParameteriv = (LPGLGETCOLORTABLEPARAMETERIV)LoadType0("glGetColorTableParameteriv"); if(!lpglGetColorTableParameteriv && !LoadFailure("glGetColorTableParameteriv", onfail)) return FALSE;
 lpglGetColorTableParameterfv = (LPGLGETCOLORTABLEPARAMETERFV)LoadType0("glGetColorTableParameterfv"); if(!lpglGetColorTableParameterfv && !LoadFailure("glGetColorTableParameterfv", onfail)) return FALSE;
 lpglGetConvolutionFilter = (LPGLGETCONVOLUTIONFILTER)LoadType0("glGetConvolutionFilter"); if(!lpglGetConvolutionFilter && !LoadFailure("glGetConvolutionFilter", onfail)) return FALSE;
 lpglGetSeparableFilter = (LPGLGETSEPARABLEFILTER)LoadType0("glGetSeparableFilter"); if(!lpglGetSeparableFilter && !LoadFailure("glGetSeparableFilter", onfail)) return FALSE;
 lpglGetConvolutionParameteriv = (LPGLGETCONVOLUTIONPARAMETERIV)LoadType0("glGetConvolutionParameteriv"); if(!lpglGetConvolutionParameteriv && !LoadFailure("glGetConvolutionParameteriv", onfail)) return FALSE;
 lpglGetConvolutionParameterfv = (LPGLGETCONVOLUTIONPARAMETERFV)LoadType0("glGetConvolutionParameterfv"); if(!lpglGetConvolutionParameterfv && !LoadFailure("glGetConvolutionParameterfv", onfail)) return FALSE;
 lpglGetHistogram = (LPGLGETHISTOGRAM)LoadType0("glGetHistogram"); if(!lpglGetHistogram && !LoadFailure("glGetHistogram", onfail)) return FALSE;
 lpglResetHistogram = (LPGLRESETHISTOGRAM)LoadType0("glResetHistogram"); if(!lpglResetHistogram && !LoadFailure("glResetHistogram", onfail)) return FALSE;
 lpglGetHistogramParameteriv = (LPGLGETHISTOGRAMPARAMETERIV)LoadType0("glGetHistogramParameteriv"); if(!lpglGetHistogramParameteriv && !LoadFailure("glGetHistogramParameteriv", onfail)) return FALSE;
 lpglGetHistogramParameterfv = (LPGLGETHISTOGRAMPARAMETERFV)LoadType0("glGetHistogramParameterfv"); if(!lpglGetHistogramParameterfv && !LoadFailure("glGetHistogramParameterfv", onfail)) return FALSE;
 lpglGetMinmax = (LPGLGETMINMAX)LoadType0("glGetMinmax"); if(!lpglGetMinmax && !LoadFailure("glGetMinmax", onfail)) return FALSE;
 lpglResetMinmax = (LPGLRESETMINMAX)LoadType0("glResetMinmax"); if(!lpglResetMinmax && !LoadFailure("glResetMinmax", onfail)) return FALSE;
 lpglGetMinmaxParameteriv = (LPGLGETMINMAXPARAMETERIV)LoadType0("glGetMinmaxParameteriv"); if(!lpglGetMinmaxParameteriv && !LoadFailure("glGetMinmaxParameteriv", onfail)) return FALSE;
 lpglGetMinmaxParameterfv = (LPGLGETMINMAXPARAMETERFV)LoadType0("glGetMinmaxParameterfv"); if(!lpglGetMinmaxParameterfv && !LoadFailure("glGetMinmaxParameterfv", onfail)) return FALSE;
 lpglGetPointerv = (LPGLGETPOINTERV)LoadType0("glGetPointerv"); if(!lpglGetPointerv && !LoadFailure("glGetPointerv", onfail)) return FALSE;
 lpglGetString = (LPGLGETSTRING)LoadType0("glGetString"); if(!lpglGetString && !LoadFailure("glGetString", onfail)) return FALSE;
 lpglPushAttrib = (LPGLPUSHATTRIB)LoadType0("glPushAttrib"); if(!lpglPushAttrib && !LoadFailure("glPushAttrib", onfail)) return FALSE;
 lpglPushClientAttrib = (LPGLPUSHCLIENTATTRIB)LoadType0("glPushClientAttrib"); if(!lpglPushClientAttrib && !LoadFailure("glPushClientAttrib", onfail)) return FALSE;
 lpglPopAttrib = (LPGLPOPATTRIB)LoadType0("glPopAttrib"); if(!lpglPopAttrib && !LoadFailure("glPopAttrib", onfail)) return FALSE;
 lpglPopClientAttrib = (LPGLPOPCLIENTATTRIB)LoadType0("glPopClientAttrib"); if(!lpglPopClientAttrib && !LoadFailure("glPopClientAttrib", onfail)) return FALSE;
 loaded = TRUE;
 return TRUE;
}

HGLRC WINAPI wglCreateContextEx(HDC device)
{
 return wglCreateContext(device);
}