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

#ifndef GL_ZERO
#error "You must include the gl headers before including this file"
#endif

#include "GlBuffers.h"
#include "GlVertexArrays.h"

namespace gl {

class Geometry;

typedef std::shared_ptr<Geometry> GeometryPtr;

class Geometry {
public:
  static const int VERTEX_ATTRIBUTE_SIZE = 4;
  static const int BYTES_PER_ATTRIBUTE = (sizeof(float)
      * VERTEX_ATTRIBUTE_SIZE);
  enum Flag {
    HAS_NORMAL = 0x01,
    HAS_COLOR = 0x02,
    HAS_TEXTURE = 0x04,
  };

  VertexBufferPtr vertices;
  IndexBufferPtr indices;
  VertexArrayPtr vertexArray;

  unsigned int elements;
  GLenum elementType;
  unsigned int verticesPerElement;
  unsigned int flags;

  void bindVertexArray() {
    vertexArray->bind();
  }

  static void unbindVertexArray() {
    gl::VertexArray::unbind();
  }

  static unsigned int getStride(unsigned int flags) {
    int result = VERTEX_ATTRIBUTE_SIZE;
    if (flags & Geometry::Flag::HAS_NORMAL) {
      result += VERTEX_ATTRIBUTE_SIZE;
    }
    if (flags & Geometry::Flag::HAS_COLOR) {
      result += VERTEX_ATTRIBUTE_SIZE;
    }
    if (flags & Geometry::Flag::HAS_TEXTURE) {
      result += VERTEX_ATTRIBUTE_SIZE;
    }
    return result * sizeof(float);
  }

  unsigned int getStride() {
    return getStride(flags);
  }

  void draw() {
    glDrawElements(elementType, elements * verticesPerElement,
        GL_UNSIGNED_INT, (void*) 0);
  }

  Geometry(
      const std::vector<glm::vec4> & in_vertices,
      const std::vector<GLuint> & in_indices,
      unsigned int elements = 0,
      unsigned int flags = 0,
      GLenum elementType = GL_TRIANGLES,
      unsigned int verticesPerElement = 3)
      : elements(elements),
          elementType(elementType),
          flags(flags),
          vertexArray(new gl::VertexArray()),
          verticesPerElement(verticesPerElement) {
    vertices = VertexBufferPtr(new VertexBuffer());
    *vertices << VectorLoader<glm::vec4>(in_vertices);
    indices = IndexBufferPtr(new IndexBuffer());
    *indices << VectorLoader<GLuint>(in_indices);
    buildVertexArray();
  }

  Geometry(
      const gl::VertexBufferPtr in_vertices,
      const gl::IndexBufferPtr in_indices,
      unsigned int elements = 0,
      unsigned int flags = 0,
      GLenum elementType = GL_TRIANGLES,
      unsigned int verticesPerElement = 3)
      : vertices(in_vertices),
          indices(in_indices),
          elements(elements),
          flags(flags),
          vertexArray(new gl::VertexArray()),
          elementType(elementType),
          verticesPerElement(verticesPerElement) {
    buildVertexArray();
  }

  void updateVertices(const std::vector<glm::vec3> & in_vertices, GLenum usage = GL_DYNAMIC_DRAW) {
    std::vector<glm::vec4> v4;
    v4.reserve(in_vertices.size());
    std::for_each(in_vertices.begin(), in_vertices.end(), [&](const glm::vec3 & element){
      v4.push_back(glm::vec4(element, 1));
    });
    updateVertices(v4, usage);
  }

  void updateVertices(const std::vector<glm::vec4> & in_vertices, GLenum usage = GL_DYNAMIC_DRAW) {
    vertices->bind();
    vertices->load(in_vertices);
    vertices->unbind();
    buildVertexArray();
  }

  void buildVertexArray() {
    vertexArray->bind();
    {
      indices->bind();
      vertices->bind();
      GL_CHECK_ERROR;
      unsigned int stride = getStride(flags);
      GLfloat * offset = 0;
      glEnableVertexAttribArray(gl::Attribute::Position);
      glVertexAttribPointer(gl::Attribute::Position, 3, GL_FLOAT, GL_FALSE,
          stride, offset);
      GL_CHECK_ERROR;

      if (flags & HAS_NORMAL) {
        offset += VERTEX_ATTRIBUTE_SIZE;
        glEnableVertexAttribArray(gl::Attribute::Normal);
        glVertexAttribPointer(gl::Attribute::Normal, 3, GL_FLOAT, GL_FALSE,
            stride, offset);
        GL_CHECK_ERROR;
      }

      if (flags & HAS_COLOR) {
        offset += VERTEX_ATTRIBUTE_SIZE;
        glEnableVertexAttribArray(gl::Attribute::Color);
        glVertexAttribPointer(gl::Attribute::Color, 3, GL_FLOAT, GL_FALSE,
            stride, offset);
        GL_CHECK_ERROR;
      }

      if (flags & HAS_TEXTURE) {
        offset += VERTEX_ATTRIBUTE_SIZE;
        glEnableVertexAttribArray(gl::Attribute::TexCoord0);
        glVertexAttribPointer(gl::Attribute::TexCoord0, 2, GL_FLOAT,
            GL_FALSE, stride, offset);
        GL_CHECK_ERROR;
      }
    }
    VertexArray::unbind();
    IndexBuffer::unbind();
    VertexBuffer::unbind();
    GL_CHECK_ERROR;
  }

};

} // gl

