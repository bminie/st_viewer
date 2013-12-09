/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef GLELEMENTDATA_H
#define GLELEMENTDATA_H

#include <QVector>

#include "GLCommon.h"
#include "GLColor.h"
#include "GLQt.h"

namespace GL
{

// GLElementData is a container class used to store and manipulate
// rendering data. It is intended to be used as the data model for
// rendering primitives stored in array structures (ie. via
// glDrawElements).
class GLElementData
{
public:

    enum ClearFlags {
        PointArray = 0x001u,
        ColorArray = 0x002u,
        IndexArray = 0x004u,
        TextureArray = 0x008u,
        OptionArray = 0x010u,
        RenderMode = 0x100u,
        // composite flags
        Arrays = (PointArray | ColorArray | IndexArray | TextureArray | OptionArray),
        Modes = (RenderMode),
        All = (Arrays | Modes)
    };

    static const GLflag DEFAULT_CLEAR_FLAGS = GLElementData::All;

    //empty constructor
    inline GLElementData();

    //clear arrays
    inline void clear(GLflag flags = DEFAULT_CLEAR_FLAGS);

    // data builders
    //points
    inline GLElementData &addPoint(GLfloat x, GLfloat y, GLindex *index = 0);
    inline GLElementData &addPoint(const GLpoint &point, GLindex *index = 0);
    template <int N>
    inline GLElementData &addShape(const GLpointdata<N> &points, GLindex *index = 0);
    //shapes
    inline GLElementData &addColor(const GLcolor &color, GLindex *index = 0);
    template <int N>
    inline GLElementData &addColor(const GLcolordata<N> &colors, GLindex *index = 0);
    //textures
    inline GLElementData &addTexture(const GLpoint &point, GLindex *index = 0);
    template <int N>
    inline GLElementData &addTexture(const GLpointdata<N> &points, GLindex *index = 0);
    //options (selected)
    inline GLElementData &addOption(const GLoption &option, GLindex *index = 0);
    template <int N>
    inline GLElementData &addOption(const GLoptiondata<N> &options, GLindex *index = 0);

    //connect indexes to data
    template <int N>
    inline GLElementData &connect(GLindex *index = 0);
    template <int N>
    inline GLElementData &connect(const GLindexdata<N> &indicies, GLindex *index = 0);
    template <int N>
    inline void deconnect(GLindex index);

    // data modifiers
    template <int N>
    inline void setShape(const GLindex index, const GLpointdata<N> &shape);
    template <int N>
    inline void setColor(const GLindex index, const GLcolordata<N> &colors);
    template <int N>
    inline void setTexture(const GLindex index, const GLpointdata<N> &texture);
    template <int N>
    inline void setOption(const GLindex index, const GLoptiondata<N> &options);

    // data access
    template <int N>
    inline const GLcolordata<N> getColor(const GLindex index);
    template <int N>
    inline const GLoptiondata<N> getOption(const GLindex index);

    // state modifiers
    inline GLElementData &setMode(GLenum mode);

    //array getters
    inline const GLarray<GLpoint> vertices() const;
    inline const GLarray<GLcolor> colors() const;
    inline const GLarray<GLindex> indices() const;
    inline const GLarray<GLpoint> textures() const;
    inline const GLarray<GLoption> options() const;

    //enum mode getter
    inline GLenum mode() const;

    //check whether the arrays are empty or not
    inline bool isEmpty() const
    {
        return m_colors.empty() && m_indices.empty() && m_textures.empty()
                && m_points.empty() && m_options.empty();
    }

protected:
    //types
    typedef std::vector<GLpoint> GLPoints;
    typedef std::vector<GLcolor> GLColors;
    typedef std::vector<GLindex> GLIndicies;
    typedef std::vector<GLpoint> GLTextures;
    typedef std::vector<GLoption> GLOptions;

    //data members
    GLPoints m_points;
    GLColors m_colors;
    GLIndicies m_indices;
    GLTextures m_textures;
    GLOptions m_options;
    GLenum m_mode;
};

} // namespace GL //

/****************************************** DEFINITION ******************************************/

namespace GL
{

GLElementData::GLElementData() : m_points(), m_indices(), m_mode(GL_TRIANGLES)
{

}

void GLElementData::clear(GLflag flags)
{
    if (flags & GLElementData::PointArray) {
        m_points.resize(0);
    }
    if (flags & GLElementData::ColorArray) {
        m_colors.resize(0);
    }
    if (flags & GLElementData::IndexArray) {
        m_indices.resize(0);
    }
    if (flags & GLElementData::TextureArray) {
        m_textures.resize(0);
    }
    if (flags & GLElementData::OptionArray) {
        m_options.resize(0);
    }
    if (flags & GLElementData::RenderMode) {
        m_mode = GL_TRIANGLES;
    }
}

GLElementData &GLElementData::addPoint(GLfloat x, GLfloat y, GLindex *index)
{
    return addPoint(GLpoint(x, y), index);
}

GLElementData &GLElementData::addPoint(const GLpoint &point, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = static_cast<GLindex>(m_points.size());
    }
    m_points.push_back(point);
    return (*this);
}

template <int N>
GLElementData &GLElementData::addShape(const GLpointdata<N> &points, GLindex *index)
{
    for (int i = 0; i < N; ++i) {
        addPoint(points.p[i], ((i == 0) ? index : 0));
    }
    return (*this);
}

GLElementData &GLElementData::addColor(const GLcolor &color, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = static_cast<GLindex>(m_colors.size());
    }
    m_colors.push_back(color);
    return (*this);
}

template <int N>
GLElementData &GLElementData::addColor(const GLcolordata<N> &colors, GLindex *index)
{
    for (int i = 0; i < N; ++i) {
        addColor(colors.c[i], ((i == 0) ? index : 0));
    }
    return (*this);
}

inline GLElementData &GLElementData::addTexture(const GLpoint &point, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = static_cast<GLindex>(m_textures.size());
    }
    m_textures.push_back(point);
    return (*this);
}

template <int N>
inline GLElementData &GLElementData::addTexture(const GLpointdata<N> &points, GLindex *index)
{
    for (int i = 0; i < N; ++i) {
        addTexture(points.p[i], ((i == 0) ? index : 0));
    }
    return (*this);
}

inline GLElementData &GLElementData::addOption(const GLoption &option, GLindex *index)
{
    // return new index if pointer provided
    if (index != 0) {
        (*index) = static_cast<GLindex>(m_options.size());
    }
    m_options.push_back(option);
    return (*this);
}

template <int N>
inline GLElementData &GLElementData::addOption(const GLoptiondata<N> &options, GLindex *index)
{
    for (int i = 0; i < N; ++i) {
        addOption(options.p[i], ((i == 0) ? index : 0));
    }
    return (*this);
}

template <int N>
GLElementData &GLElementData::connect(GLindex *index)
{
    const GLPoints::size_type size = (m_points.size() - 1);
    GLindex tail = static_cast<GLindex>(size);
    // return new index if pointer provided
    if (index != 0) {
        (*index) = static_cast<GLindex>(m_indices.size());
    }
    for (int i = 0; i < N; ++i) {
        m_indices.push_back(tail--);
    }
    return (*this);
}

template <int N>
inline GLElementData &GLElementData::connect(const GLindexdata<N> &indicies, GLindex *index)
{
    const GLPoints::size_type size = (m_points.size() - 1);
    GLindex tail = static_cast<GLindex>(size);
    // return new index if pointer provided
    if (index != 0) {
        (*index) = static_cast<GLindex>(m_indices.size());
    }
    for (int i = 0; i < N; ++i) {
        m_indices.push_back((indicies.i[i] != GL::INVALID_INDEX) ? indicies.i[i] : tail--);
    }
    return (*this);
}

template <int N>
void GLElementData::deconnect(GLindex index)
{
    // get tail
    GLindex tail = static_cast<GLindex>(m_indices.size() - N);
    if (index != tail) {
        for (int i = 0; i < N; ++i) {
            qSwap(m_indices[index + i], m_indices[tail + i]);
        }
    }
    m_indices.resize(tail);
}

// data modifiers
template <int N>
inline void GLElementData::setShape(const GLindex index, const GLpointdata<N> &shape)
{
    GLpointdata<N> *data = reinterpret_cast<GLpointdata<N> *>(&m_points[index]);
    (*data) = shape;
}

template <int N>
inline void GLElementData::setColor(const GLindex index, const GLcolordata<N> &colors)
{
    GLcolordata<N> *data = reinterpret_cast<GLcolordata<N> *>(&m_colors[index]);
    (*data) = colors;
}

template <int N>
inline void GLElementData::setTexture(const GLindex index, const GLpointdata<N> &texture)
{
    GLpointdata<N> *data = reinterpret_cast<GLpointdata<N> *>(&m_textures[index]);
    (*data) = texture;
}

template <int N>
inline void GLElementData::setOption(const GLindex index, const GLoptiondata<N> &options)
{
    GLoptiondata<N> *data = reinterpret_cast<GLoptiondata<N> *>(&m_options[index]);
    (*data) = options;
}

// data access
template <int N>
inline const GLcolordata<N> GLElementData::getColor(const GLindex index)
{
    return *(reinterpret_cast<GLcolordata<N> *>(&m_colors[index]));
}

template <int N>
inline const GLoptiondata<N> GLElementData::getOption(const GLindex index)
{
    return *(reinterpret_cast<GLoptiondata<N> *>(&m_options[index]));
}

// state modifiers
GLElementData &GLElementData::setMode(GLenum mode)
{
    m_mode = mode;
    return (*this);
}

const GLarray<GLpoint> GLElementData::vertices() const
{
    return GLarray<GLpoint>(static_cast<GLindex>(m_points.size()),
                            static_cast<const GLpoint *>(m_points.data()));
}

const GLarray<GLcolor> GLElementData::colors() const
{
    return GLarray<GLcolor>(static_cast<GLindex>(m_colors.size()),
                            static_cast<const GLcolor *>(m_colors.data()));
}

const GLarray<GLindex> GLElementData::indices() const
{
    return GLarray<GLindex>(static_cast<GLindex>(m_indices.size()),
                            static_cast<const GLindex *>(m_indices.data()));
}

const GLarray<GLpoint> GLElementData::textures() const
{
    return GLarray<GLpoint>(static_cast<GLindex>(m_textures.size()),
                            static_cast<const GLpoint *>(m_textures.data()));
}

const GLarray<GLoption> GLElementData::options() const
{
    return GLarray<GLoption>(static_cast<GLindex>(m_options.size()),
                             static_cast<const GLoption *>(m_options.data()));
}

GLenum GLElementData::mode() const
{
    return m_mode;
}

} // namespace GL //


#endif // GLELEMENTDATA_H //