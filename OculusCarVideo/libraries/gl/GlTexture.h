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

#include <glm/glm.hpp>
#include <memory>

namespace gl {

namespace TextureTarget2D {
enum Enum {
  TEXTURE_2D = GL_TEXTURE_2D,
  PROXY_TEXTURE_2D = GL_PROXY_TEXTURE_2D,
  TEXTURE_1D_ARRAY = GL_TEXTURE_1D_ARRAY,
  PROXY_TEXTURE_1D_ARRAY = GL_PROXY_TEXTURE_1D_ARRAY,
  TEXTURE_RECTANGLE = GL_TEXTURE_RECTANGLE,
  PROXY_TEXTURE_RECTANGLE = GL_PROXY_TEXTURE_RECTANGLE,
  TEXTURE_CUBE_MAP_POSITIVE_X = GL_TEXTURE_CUBE_MAP_POSITIVE_X,
  TEXTURE_CUBE_MAP_NEGATIVE_X = GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
  TEXTURE_CUBE_MAP_POSITIVE_Y = GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
  TEXTURE_CUBE_MAP_NEGATIVE_Y = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
  TEXTURE_CUBE_MAP_POSITIVE_Z = GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
  TEXTURE_CUBE_MAP_NEGATIVE_Z = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
  PROXY_TEXTURE_CUBE_MAP = GL_PROXY_TEXTURE_CUBE_MAP
};
}

template <
GLenum TextureType = GL_TEXTURE_2D,
GLenum TextureFormat = GL_RGBA8
>
class Texture {
public:
  GLuint texture;
  Texture()
      : texture(0) {
    glGenTextures(1, &texture);
  }
  ~Texture() {
    if (texture) {
      glDeleteTextures(1, &texture);
    }
  }

  void bind() {
    glBindTexture(TextureType, texture);
  }

  static void unbind() {
    glBindTexture(TextureType, 0);
  }

  void parameter(GLenum paramName, GLfloat f, GLenum target = TextureType ) {
    glTexParameterf(target, paramName, f);
  }

  void parameter(GLenum paramName, GLint i, GLenum target = TextureType ) {
    glTexParameteri(target, paramName, i);
  }

  void image2d(const glm::uvec2 & size, void * data,
      GLint level = 0,
      GLenum format = GL_RGB,
      GLenum type = GL_UNSIGNED_BYTE) {
    image2d(TextureType, size, data, level, format, type);
  }

  void image2d(GLenum target, const glm::uvec2 & size, void * data,
      GLint level = 0,
      GLenum format = GL_RGB,
      GLenum type = GL_UNSIGNED_BYTE) {
    GL_CHECK_ERROR;
    glTexImage2D(target, level, TextureFormat, size.x, size.y, 0, format, type, data);
    GL_CHECK_ERROR;
  }

  void storage2d(const glm::uvec2 & size, GLint levels = 1) {
    glTexStorage2D(TextureType, levels, TextureFormat, size.x, size.y);
  }

  void storage2dMultisample(const glm::uvec2 & size, GLsizei samples = 4, GLboolean fixedSampleLocations = GL_TRUE) {
    glTexStorage2DMultisample(TextureType, samples, TextureFormat, size.x, size.y, fixedSampleLocations);
  }

  void generateMipmap() {
    glGenerateMipmap(TextureType);
  }

  operator GLuint() {
    return texture;
  }

  typedef std::shared_ptr<Texture<TextureType, TextureFormat> > Ptr;
};

typedef Texture<GL_TEXTURE_2D> Texture2d;
typedef Texture<GL_TEXTURE_2D, GL_DEPTH_COMPONENT16> Texture2dDepth;
typedef Texture<GL_TEXTURE_2D_MULTISAMPLE> Texture2dMs;
typedef Texture<GL_TEXTURE_3D> Texture3d;
typedef Texture<GL_TEXTURE_CUBE_MAP> TextureCubeMap;
typedef Texture2d::Ptr Texture2dPtr;
typedef Texture2dDepth::Ptr Texture2dDepthPtr;
typedef TextureCubeMap::Ptr TextureCubeMapPtr;
typedef Texture2dMs::Ptr Texture2dMsPtr;
typedef Texture2d::Ptr TexturePtr;
} // gl
