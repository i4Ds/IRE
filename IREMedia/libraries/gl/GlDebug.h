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

#ifdef WIN32
#include <Windows.h>
#endif

#include <stdexcept>
#include <iostream>
#include <ostream>
#include <cstdarg>
#include <string>
#include <cstring>

#ifdef WIN32
#pragma warning(disable: 4996)
#define snprintf _snprintf
#endif

namespace gl {

class error : public std::runtime_error {
public:
  GLenum errorCode;

  error(GLenum errorCode)
      : errorCode(errorCode), runtime_error("OpenGL Error") {
  }

  static inline void check() {
#ifdef GL_DEBUG
    GLenum errorCode = glGetError();
    if (errorCode != 0) {
      throw error(errorCode);
    }
#endif
  }
};

class shader_error : public std::runtime_error {
public:
  GLuint shader;
  shader_error(GLuint shader, const std::string & log)
      : shader(shader), runtime_error("OpenGL Shader Error: " + log) {
  }
};

}

#define GL_CHECK_ERROR  gl::error::check()
