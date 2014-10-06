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

namespace gl {
template <
  typename GenFunction,
  typename DelFunction,
  typename BindFunction>
class Object {
  GLuint id;
  const GenFunction gen;
  const DelFunction del;
  const BindFunction bind;
public:
  Object(GenFunction g, DelFunction del, BindFunction bind) : gen(g), del(del), bind(bind) {
  }
};

template <GLenum QueryType = GL_TIME_ELAPSED> class Query {
  GLuint query;
public:
  Query() {
    glGenQueries(1, &query);
  }

  virtual ~Query() {
    glDeleteQueries(1, &query);
    query = 0;
  }

  void begin() {
    glBeginQuery(QueryType, query);
  }

  static void end() {
    glEndQuery(QueryType);
  }

  bool available() {
    GLint result;
    glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &result);
    return result == GL_TRUE;
  }

  GLint getResult() {
    GLint result = -1;
    glGetQueryObjectiv(query, GL_QUERY_RESULT, &result);
    return result;
  }

  typedef std::shared_ptr<gl::Query<QueryType> > Ptr;
};

typedef Query<GL_TIME_ELAPSED> TimeQuery;
typedef TimeQuery::Ptr TimeQueryPtr;

typedef Query<GL_SAMPLES_PASSED> SampleQuery;
typedef SampleQuery::Ptr SampleQueryPtr;

typedef Query<GL_PRIMITIVES_GENERATED> PrimitiveQuery;
typedef PrimitiveQuery::Ptr PrimitiveQueryPtr;

}
