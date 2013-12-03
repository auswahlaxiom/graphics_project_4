
#include <vector>
#include "buffer.h"

#pragma once


/* ------------------------------------------- */

namespace gl_CSCI441 {

  class VertexArray {

  private:
    GLuint handle;

  public:

    VertexArray();
    ~VertexArray();

    VertexArray ( const VertexArray & );
    VertexArray & operator= ( const VertexArray & );

    void attachAttribute ( const GLuint aix, const Buffer * b, GLuint divisor = 0 );

    void on();
    void off();

    // N is for instanced rendering (=number of instances)
    void sendToPipeline ( GLenum ptype, GLint first, GLsizei num, GLsizei N = -1 );
    void sendToPipelineIndexed ( GLenum ptype, IndexBuffer *b, GLint first, GLsizei num, GLsizei N = -1 );

    void setDivisor ( GLuint aix, GLuint divisor );
    void resetDivisor ( GLuint aix );
  };
}
  

/* ------------------------------------------- */
