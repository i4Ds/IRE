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

#include "GlTexture.h"
#include "GlDebug.h"

namespace gl {

struct FrameBuffer {
  GLuint frameBuffer;

  FrameBuffer() : frameBuffer(0) {
    glGenFramebuffers(1, &frameBuffer);
  }

  virtual ~FrameBuffer() {
    if (frameBuffer) {
      glDeleteFramebuffers(1, &frameBuffer);
      GL_CHECK_ERROR;
    }
  }

  void bind(GLenum target = GL_FRAMEBUFFER) const {
    glBindFramebuffer(target, frameBuffer);
  }

  static void unbind(GLenum target = GL_FRAMEBUFFER) {
    glBindFramebuffer(target, 0);
  }

  static bool checkStatus(GLenum target = GL_FRAMEBUFFER) {
      GLuint status = glCheckFramebufferStatus(target);
      switch(status) {
      case GL_FRAMEBUFFER_COMPLETE:
          return true;
          break;

      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
          std::cerr << "framebuffer incomplete attachment" << std::endl;
          break;

      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
          std::cerr << "framebuffer missing attachment" << std::endl;
          break;

      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
          std::cerr << "framebuffer incomplete draw buffer" << std::endl;
          break;

      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
          std::cerr << "framebuffer incomplete read buffer" << std::endl;
          break;

      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
          std::cerr << "framebuffer incomplete multisample" << std::endl;
          break;

      case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS :
          std::cerr << "framebuffer incomplete layer targets" << std::endl;
          break;

      case GL_FRAMEBUFFER_UNSUPPORTED:
          std::cerr << "framebuffer unsupported internal format or image" << std::endl;
          break;

      default:
          std::cerr << "other framebuffer error" << std::endl;
          break;
      }

      return false;
  }

  template <GLenum TextureType, GLenum TextureFormat>
  void attach(int attachPoint, std::shared_ptr<Texture<TextureType, TextureFormat> > & texture) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachPoint, TextureType, *texture, 0);
  }
};

typedef std::shared_ptr<FrameBuffer> FrameBufferPtr;

template <
  GLenum TextureType = GL_TEXTURE_2D,
  GLenum ColorFormat = GL_RGBA8,
  GLenum DepthFormat = GL_DEPTH_COMPONENT16
>
struct TFrameBufferWrapper {
  typedef Texture<TextureType, ColorFormat> ColorTexture;
  typedef Texture<TextureType, DepthFormat> DepthTexture;
  typedef typename ColorTexture::Ptr ColorTexturePtr;
  typedef typename DepthTexture::Ptr DepthTexturePtr;

  FrameBufferPtr frameBuffer;
  ColorTexturePtr color;
  DepthTexturePtr depth;
  glm::uvec2 size;

  void init(const glm::uvec2 & size) {
    this->size = size;
    GL_CHECK_ERROR;
    if (!frameBuffer) {
      frameBuffer = FrameBufferPtr(new FrameBuffer());
    }
    frameBuffer->bind();
    GL_CHECK_ERROR;

    if (!color) {
      color = ColorTexturePtr(new ColorTexture());
      color->bind();
      if (TextureType == GL_TEXTURE_2D) {
        color->parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        color->parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        color->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        color->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        color->storage2d(size);
      } else if (TextureType == GL_TEXTURE_2D_MULTISAMPLE) {
        color->storage2dMultisample(size);
      }
      ColorTexture::unbind();
    }
    frameBuffer->attach(GL_COLOR_ATTACHMENT0, color);
    GL_CHECK_ERROR;

    if (!depth) {
      depth = DepthTexturePtr(new DepthTexture());
      depth->bind();
      if (TextureType == GL_TEXTURE_2D) {
        depth->storage2d(size);
      } else if (TextureType == GL_TEXTURE_2D_MULTISAMPLE) {
        depth->storage2dMultisample(size);
      }
      DepthTexture::unbind();
    }
    frameBuffer->attach(GL_DEPTH_ATTACHMENT, depth);
    GL_CHECK_ERROR;

    if (!frameBuffer->checkStatus()) {
      throw std::runtime_error("Bad framebuffer creation");
    }
    GL_CHECK_ERROR;
    FrameBuffer::unbind();
    GL_CHECK_ERROR;
  }

  void initMultisample(const glm::uvec2 & size, uint8_t samples) {
    this->size = size;
    GL_CHECK_ERROR;
    if (!frameBuffer) {
      frameBuffer = FrameBufferPtr(new FrameBuffer());
    }
    frameBuffer->bind();
    GL_CHECK_ERROR;

    if (!color) {
      color = typename ColorTexture::Ptr(new ColorTexture());
      color->bind();
      color->parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      color->parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      color->parameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
      color->parameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
      color->storage2d(size);
      Texture2d::unbind();
    }
    frameBuffer->attach(GL_COLOR_ATTACHMENT0, color);
    GL_CHECK_ERROR;

    if (!depth) {
      depth = Texture2dDepth::Ptr(new Texture2dDepth());
      depth->bind();
      depth->storage2d(size);
      Texture2dDepth::unbind();
    }
    frameBuffer->attach(GL_DEPTH_ATTACHMENT, depth);
    GL_CHECK_ERROR;


    if (!frameBuffer->checkStatus()) {
      throw std::runtime_error("Bad framebuffer creation");
    }
    GL_CHECK_ERROR;
    FrameBuffer::unbind();
    GL_CHECK_ERROR;
  }

  void activate() const {
    frameBuffer->bind();
    viewport(size);
  }

  static void deactivate() {
    FrameBuffer::unbind();
  }

  template <typename Function>
  void withFramebufferActive(Function f) {
    activate();
    f();
    deactivate();
  }
};

typedef TFrameBufferWrapper<> FrameBufferWrapper;
typedef TFrameBufferWrapper<GL_TEXTURE_2D_MULTISAMPLE> MultisampleFrameBufferWrapper;

} // gl


