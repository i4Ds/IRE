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

#include <vector>
#include <cassert>
#include <memory>

namespace gl {

template<GLenum BufferType>
class Buffer {
  GLuint buffer;

public:

  Buffer() :
      buffer(0) {
    glGenBuffers(1, &buffer);
    assert(buffer != 0);
  }

  Buffer(Buffer && other) :
      buffer(0) {
    std::swap(other.buffer, buffer);
  }

  template<typename T>
  Buffer(const std::vector<T> & data) :
      buffer(0) {
    glGenBuffers(1, &buffer);

    bind();
    load(data);
    unbind();
  }

  template<typename T, size_t SIZE>
  Buffer(T (&array)[SIZE]) :
      buffer(0) {
    glGenBuffers(1, &buffer);
    bind();
    load(SIZE * sizeof(T), array);
    unbind();
  }

  virtual ~Buffer() {
    glDeleteBuffers(1, &buffer);
  }

  template<typename T>
  void load(const std::vector<T> & data, GLenum usage = GL_STATIC_DRAW) {
    load(data.size() * sizeof(T), &data[0], usage);
  }

  void load(size_t size, const void * data, GLenum usage = GL_STATIC_DRAW) {
    glBufferData(BufferType, size, data, usage);
  }

  void bind() const {
    glBindBuffer(BufferType, buffer);
  }

  static void unbind() {
    glBindBuffer(BufferType, 0);
  }

  void loadSub(GLintptr offset, GLsizeiptr size, const void *data) {
    glBufferSubData(BufferType, offset, size, data);
  }

  static void storage(GLsizeiptr size, GLbitfield flags) {
    glBufferStorage(BufferType, size, nullptr, flags);
  }
};

typedef Buffer<GL_ELEMENT_ARRAY_BUFFER> IndexBuffer;
typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;

typedef Buffer<GL_ARRAY_BUFFER> VertexBuffer;
typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;

class BufferLoader {
public:
  virtual ~BufferLoader() {
  }
  virtual const GLvoid* getData() const  = 0;
  virtual GLsizeiptr getSize() const = 0;
};

template<class T> class VectorLoader: public BufferLoader {
public:
  typedef std::vector<T> V;
  const V & data;
  VectorLoader(const V & data) :
      data(data) {
  }

  const GLvoid* getData() const {
    const T * ptr = &(this->data[0]);
    return ptr;
  }

  GLsizeiptr getSize() const {
    return sizeof(T) * data.size();
  }

};

template<typename T> VectorLoader<T> makeVectorLoader(
    const std::vector<T> & vector) {
  return VectorLoader<T>(vector);
}

template<typename T, size_t SIZE> class ArrayLoader: public BufferLoader {
public:
  const T * data;

  ArrayLoader(const T * data) :
      data(data) {
  }

  const GLvoid* getData() const {
    return (const GLvoid*)data;
  }

  GLsizeiptr getSize() const {
    return sizeof(T) * SIZE;
  }
};

template<typename T, size_t SIZE> ArrayLoader<T, SIZE> makeArrayLoader(
    T * pointer) {
  return ArrayLoader<T, SIZE>(pointer);
}

template<typename T, size_t SIZE> ArrayLoader<T, SIZE> makeArrayLoader(
  T(&array)[SIZE]) {
  return ArrayLoader<T, SIZE>(array);
}



template <GLenum BufferType>
Buffer<BufferType> & operator <<(Buffer<BufferType> & buffer, const BufferLoader & loader) {
  buffer.bind();
  buffer.load(loader.getSize(), loader.getData());
  Buffer<BufferType>::unbind();
  GL_CHECK_ERROR;
  return buffer;
}

template <GLenum BufferType>
Buffer<BufferType> & operator <<(Buffer<BufferType> & buffer, BufferLoader && loader) {
    GL_CHECK_ERROR;
  buffer.bind();
    GL_CHECK_ERROR;
  buffer.load(loader.getSize(), loader.getData());
    GL_CHECK_ERROR;
  Buffer<BufferType>::unbind();
  GL_CHECK_ERROR;
  return buffer;
}


} // gl

