/*
 Copyright (c) 2010-2012, Paul Houx - All rights reserved.
 This code is intended for use with the Cinder C++ library: http://libcinder.org

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#include "Common.h"
#include "IO.h"
namespace Text {

// 1 point = 1pt = 1/72in (cala) = 0.3528 mm
const float Font::DTP_TO_METERS = 0.003528f;
const float Font::METERS_TO_DTP = 1.0f / Font::DTP_TO_METERS;

Font::Font(void)
    : mFamily("Unknown"), mFontSize(12.0f), mLeading(0.0f), mAscent(0.0f), mDescent(
        0.0f), mSpaceWidth(0.0f) {
}

Font::~Font(void) {
}

struct TextureVertex {
  glm::vec4 pos;
  glm::vec4 tex;
  TextureVertex() {
  }
  TextureVertex(const glm::vec2 & pos, const glm::vec2 & tex)
      : pos(pos, 0, 0), tex(tex, 0, 0) {
  }
};

struct QuadBuilder {
  TextureVertex vertices[4];
  QuadBuilder(const rectf & r, const rectf & tr) {
    vertices[0] = TextureVertex(r.getLowerLeft(), tr.getUpperLeft());
    vertices[1] = TextureVertex(r.getLowerRight(), tr.getUpperRight());
    vertices[2] = TextureVertex(r.getUpperRight(), tr.getLowerRight());
    vertices[3] = TextureVertex(r.getUpperLeft(), tr.getLowerLeft());
  }
};

void readPngToTexture(const char * data, size_t size,
    gl::TexturePtr & texture,
    glm::vec2 & textureSize) {
  glm::uvec2 imageSize;

  std::vector<unsigned char> pngData;
  pngData.resize(size);
  memcpy(&pngData[0], data, size);

  std::vector<unsigned char> imageData;
  GlUtils::getImageData(pngData, imageSize, imageData, false);

  textureSize = glm::vec2(imageSize);
  texture = gl::TexturePtr(new gl::Texture2d());
  texture->bind();
  texture->parameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  texture->parameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifdef HAVE_OPENCV
  texture->image2d(imageSize, &imageData[0], 0, GL_RGB);
#else
  texture->image2d(imageSize, &imageData[0], 0, GL_RED);
#endif

  gl::Texture2d::unbind();
}

void Font::read(const void * data, size_t size) {
  std::istringstream in(std::string(static_cast<const char*>(data), size));
//  SignedDistanceFontFile sdff;
//  sdff.read(in);

  uint8_t header[4];
  readStream(in, header);
  if (memcmp(header, "SDFF", 4)) {
    FAIL("Bad font file");
  }

  uint16_t version;
  readStream(in, version);

  // read font name
  if (version > 0x0001) {
    char c;
    readStream(in, c);
    while (c) {
      mFamily += c;
      readStream(in, c);
    }
  }

  // read font data
  readStream(in, mLeading);
  readStream(in, mAscent);
  readStream(in, mDescent);
  readStream(in, mSpaceWidth);
  mFontSize = mAscent + mDescent;

  // read metrics data
  mMetrics.clear();

  uint16_t count;
  readStream(in, count);

  for (int i = 0; i < count; ++i) {
    uint16_t charcode;
    readStream(in, charcode);
    Metrics & m = mMetrics[charcode];
    readStream(in, m.ul.x);
    readStream(in, m.ul.y);
    readStream(in, m.size.x);
    readStream(in, m.size.y);
    readStream(in, m.offset.x);
    readStream(in, m.offset.y);
    readStream(in, m.d);
    m.lr = m.ul + m.size;
  }

  // read image data
  readPngToTexture((const char *) data + in.tellg(), size - in.tellg(),
      mTexture, mTextureSize);

  std::vector<TextureVertex> vertexData;
  std::vector<GLuint> indexData;
  float texH = 0, texW = 0, texA = 0;
  int characters = 0;
  std::for_each(mMetrics.begin(), mMetrics.end(),
      [&] ( MetricsData::reference & md ) {
        uint16_t id = md.first;
        Font::Metrics & m = md.second;
        ++characters;
        GLuint index = (GLuint)vertexData.size();
        rectf bounds = getBounds(m, mFontSize);
        rectf texBounds = getTexCoords(m);

        QuadBuilder qb(bounds, texBounds);
        for (int i = 0; i < 4; ++i) {
          vertexData.push_back(qb.vertices[i]);
        }

        m.indexOffset = indexData.size();
        indexData.push_back(index + 0);
        indexData.push_back(index + 1);
        indexData.push_back(index + 2);
        indexData.push_back(index + 0);
        indexData.push_back(index + 2);
        indexData.push_back(index + 3);
      });

  gl::VertexBufferPtr vertexBuffer(new gl::VertexBuffer(vertexData));
  gl::IndexBufferPtr indexBuffer(new gl::IndexBuffer(indexData));
  mGeometry = gl::GeometryPtr(
      new gl::Geometry(vertexBuffer, indexBuffer, characters * 2,
          gl::Geometry::Flag::HAS_TEXTURE));
  mGeometry->buildVertexArray();
}

Font::Metrics Font::getMetrics(uint16_t charcode) const {
  MetricsData::const_iterator itr = mMetrics.find(charcode);
  if (itr == mMetrics.end())
    return Metrics();

  return itr->second;
}

rectf Font::getBounds(uint16_t charcode, float fontSize) const {
  MetricsData::const_iterator itr = mMetrics.find(charcode);
  if (itr != mMetrics.end())
    return getBounds(itr->second, fontSize);
  else
    return rectf();
}

rectf Font::getBounds(const Metrics &m, float fontSize) const {
  float scale = (fontSize / mFontSize);
  return rectf(m.offset * scale, m.offset + m.size * scale);
}

rectf Font::getTexCoords(const Metrics &m) const {
  return rectf(m.ul / mTextureSize, m.lr / mTextureSize);
}

float Font::getAdvance(uint16_t charcode, float fontSize) const {
  MetricsData::const_iterator itr = mMetrics.find(charcode);
  if (itr != mMetrics.end())
    return getAdvance(itr->second, fontSize);

  return 0.0f;
}

float Font::getAdvance(const Metrics &metrics, float fontSize) const {
  return metrics.d * fontSize / mFontSize;
}

rectf Font::getDimensions(const std::wstring & str, float fontSize) {
  return rectf();
}

std::wstring toUtf16(const std::string & text) {
//    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring wide(text.begin(), text.end()); //= converter.from_bytes(narrow.c_str());
  return wide;
}

void Font::renderString(
    const std::string & str,
    glm::vec2 & cursor,
    float fontSize,
    float maxWidth) {
  renderString(toUtf16(str), cursor, fontSize, maxWidth);
}

typedef std::pair<size_t, size_t> Token;
typedef std::vector<Token> TokenList;

bool tokenStop(uint16_t c) {
  return c == ' ' || c == '\n';
}

std::vector<std::wstring> inline Tokenize(const std::wstring &source) {
  static std::wstring tokens = toUtf16(" ");
  std::vector<std::wstring> results;

  size_t prev = 0;
  size_t next = 0;
  while ((next = source.find_first_of(tokens, prev)) != std::wstring::npos) {
    if (next - prev != 0) {
      results.push_back(source.substr(prev, next - prev));
    }
    prev = next + 1;
  }

  if (prev < source.size()) {
    results.push_back(source.substr(prev));
  }

  return results;
}

void Font::renderString(
    const std::wstring & str,
    glm::vec2 & cursor,
    float fontSize,
    float maxWidth) {
  float scale = Text::Font::DTP_TO_METERS * fontSize / mFontSize;
  bool wrap = (maxWidth == maxWidth);
  if (wrap) {
    maxWidth /= scale;
  }

  gl::MatrixStack & mv = gl::Stacks::modelview();
  size_t mvDepth = mv.size();

  glm::vec4 aspectTest = gl::Stacks::projection().top() * glm::vec4(1, 1, 0, 1);
  float aspect = std::abs(aspectTest.x / aspectTest.y);
  mv.push().translate(cursor).translate(glm::vec2(0, scale * -mAscent));

  // scale the modelview from into font units
  mv.scale(scale);
  gl::ProgramPtr program = GlUtils::getProgram(
      Resource::SHADERS_TEXT_VS,
      Resource::SHADERS_TEXT_FS);

  program->use();
  program->setUniform("Color", glm::vec4(1));
  program->setUniform("Font", 0);
  program->setUniform4x4f("Projection",
      gl::Stacks::projection().top());

  mTexture->bind();
  mGeometry->bindVertexArray();

  std::vector<std::wstring> tokens = Tokenize(str);

  // Stores how far we've moved from the start of the string, in DTP units
  glm::vec2 advance;
  static std::wstring SPACE = toUtf16(" ");
  for_each(tokens.begin(), tokens.end(), [&](const std::wstring & token) {
    float tokenWidth = measureWidth(token, fontSize) ;
    if (wrap && 0 != advance.x && (advance.x + tokenWidth) > maxWidth) {
      advance.x = 0;
      advance.y -= (mAscent + mDescent);
    }

    for_each(token.begin(), token.end(), [&](::uint16_t id) {
      if ('\n' == id) {
        advance.x = 0;
        advance.y -= (mAscent + mDescent);
        return;
      }

      if (!contains(id)) {
        id = '?';
      }

      // get metrics for this character to speed up measurements
      const Font::Metrics & m = getMetrics(id);

      if (wrap && ((advance.x + m.d) > maxWidth)) {
        advance.x = 0;
        advance.y -= (mAscent + mDescent);
      }

      // We create an offset vec2 to hold the local offset of this character
      // This includes compensating for the inverted Y axis of the font
      // coordinates
      glm::vec2 offset(advance);
      offset.y -= m.size.y;
      // Bind the new position
      mv.with_push([&]{
          mv.translate(offset);
          mv.apply(program);
          // Render the item
          glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(m.indexOffset * sizeof(GLuint)));
      });
      advance.x += m.d;//+ m.offset.x;// font->getAdvance(m, mFontSize);
    });
    advance.x += getMetrics(' ').d;
  });

  gl::VertexArray::unbind();
  gl::Texture2d::unbind();
  gl::Program::clear();
  mv.pop();
  //cursor.x += advance * scale;
}

//rectf Font::measure(const std::wstring &text, float fontSize) const {
//  float offset = 0.0f;
//  rectf result(0.0f, 0.0f, 0.0f, 0.0f);
//
//  std::wstring::const_iterator citr;
//  for (citr = text.begin(); citr != text.end(); ++citr) {
//    uint16_t charcode = (uint16_t) * citr;
//
//    // TODO: handle special chars like /t
//    MetricsData::const_iterator itr = mMetrics.find(charcode);
//    if (itr != mMetrics.end()) {
//      result.include(
//          rectf(offset + itr->second.dx, -itr->second.dy,
//              offset + itr->second.dx + itr->second.w,
//              itr->second.h - itr->second.dy));
//      offset += itr->second.d;
//    }
//  }
//
//  // return
//  result.scale(fontSize / mFontSize);
//  return result;
//}

float Font::measureWidth(const std::wstring &text,
    float fontSize,
    bool precise) const {
  return measureWidth(text, 0, text.length(), fontSize, precise);
}

float Font::measureWidth(
    const std::wstring &text,
    size_t start, size_t end,
    float fontSize,
    bool precise) const {
  float offset = 0.0f;
  float adjust = 0.0f;

  for (size_t i = start; i < end; ++i) {
    uint16_t charcode = text.at(i);
    // TODO: handle special chars like /t
    MetricsData::const_iterator itr = mMetrics.find(charcode);
    if (itr != mMetrics.end()) {
      offset += itr->second.d;

      // precise measurement takes into account that the last character
      // contributes to the total width only by its own width, not its advance
      if (precise)
        adjust = itr->second.offset.x + itr->second.size.x - itr->second.d;
    }
  }

  return (offset + adjust);
}

}
