
#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <cassert>
#include <iostream>
#include <cstdlib>

#include "vertexarray.h"

using namespace std;

/* ------------------------------------------- */

namespace gl_CSCI441 {

  /* ------------------------------------------- */

  VertexArray::VertexArray ( const VertexArray & rhs )
  {
    cerr << "Attempting to copy VertexArray object" << endl;
    exit(1);     
  }

  /* ------------------------------------------- */

  VertexArray & VertexArray::operator= ( const VertexArray & rhs )
  {
    cerr << "Attempting assignment of VertexArray object" << endl;
    exit(1);
  }

  /* ------------------------------------------- */

  /*
  int VertexArray::find ( int aix ) const 
  {
    for ( int i=0; i<ix.size(); i++ )
      if (ix[i]==aix)
	return i;
    return -1;
  }
  */

  /* ------------------------------------------- */

  VertexArray::VertexArray()
    {
      glGenVertexArrays(1,&handle);
    }

  /* ------------------------------------------- */

  VertexArray::~VertexArray()
    {
      assert(glIsVertexArray(handle));
      glDeleteVertexArrays(1,&handle);
    }

  /* ------------------------------------------- */

  void VertexArray::attachAttribute ( const GLuint aix, const Buffer * b, GLuint divisor )
    {
      glBindVertexArray(handle);
      b->setIndex(aix);
      glEnableVertexAttribArray(aix);
      glVertexAttribDivisor(aix,divisor);
      glBindVertexArray(0);
    }

  /* ------------------------------------------- */

  void VertexArray::on()
  {
    glBindVertexArray(handle);
  }

  /* ------------------------------------------- */

  void VertexArray::off()
  {
    glBindVertexArray(0);
  }

  /* ------------------------------------------- */

  void VertexArray::sendToPipeline ( GLenum ptype, GLint first, GLsizei num, GLsizei N )
  {
    on();
    if (N<0)
      glDrawArrays(ptype,first,num);
    else
      glDrawArraysInstanced(ptype,first,num,N);
    off();
  }

  /* ------------------------------------------- */

  void VertexArray::setDivisor ( GLuint aix, GLuint divisor )
  {
    on();
    glVertexAttribDivisor(aix,divisor);
    off();
  }

  /* ------------------------------------------- */

  void VertexArray::resetDivisor ( GLuint aix )
  {
    setDivisor(aix,0);
  }

  /* ------------------------------------------- */

  void VertexArray::sendToPipelineIndexed ( GLenum ptype, IndexBuffer *b, GLint first, GLsizei num, GLsizei N )
  {
    on();
    b->on();
    if (N<0)
      glDrawElements(ptype,num,b->getType(),(GLvoid*)(long)first);
    else
      glDrawElementsInstanced(ptype,num,b->getType(),(GLvoid*)(long)first,N);
    b->off();
    off();
  }

  /* ------------------------------------------- */
}
