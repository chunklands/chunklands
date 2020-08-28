#ifndef __CHUNKLANDS_ENGINE_GL_VAO_HXX__
#define __CHUNKLANDS_ENGINE_GL_VAO_HXX__

#include <boost/move/core.hpp>
#include <chunklands/engine/gl/gl_check.hxx>
#include <chunklands/engine/types.hxx>
#include <chunklands/libcxx/glfw.hxx>

namespace chunklands::engine::gl {

// was just an idea, but it doesn't work well due to my metaprogramming skills
// template<class T = GLvoid>
// struct GLType {
//   static constexpr GLenum value = GL_NONE;
// };

// template<>
// struct GLType<GLfloat> {
//   static constexpr GLenum value = GL_FLOAT;
// };

// template<class T>
// struct Size {
//   static constexpr size_t value = sizeof(T);
// };

// template<>
// struct Size<void> {
//   static constexpr size_t value = 0;
// };

// template<GLint N, class Type>
// struct Vec {
//   static constexpr GLint size = N * Size<Type>::value;
//   static constexpr GLenum type = GLType<Type>::value;
// };

// template<class ...Args> struct Stride;

// template<class Type>
// struct Stride {
//   static constexpr GLsizei value = Type::size;
// };

// template<class Type, class ...Args>
// struct Stride {
//   static constexpr GLsizei value = Type::size + Stride<Args...>::value;
// };

// template<class Type, class ...Args>
// constexpr GLsizei stride2();

// template<class Type, class ...Args>
// constexpr GLsizei stride2() { return Type::size + stride2<Args...>(); }

// template<class Type = Vec<0, void>>
// constexpr GLsizei stride2() {
//   return Type::size;
// }

// template<class ...Args>
// constexpr GLsizei stride() {
//   return stride2<Args...>();
// }

// template<GLsizei Stride, GLuint Index, GLsizei Pointer, class Type, class ...Args>
// void a() {
//   a<Stride, Index, Pointer, Type>();
//   a<Stride, Index + 1, Pointer + Stride<Type>::value, Args...>();
// }

// template<GLsizei Stride, GLuint Index, GLsizei Pointer, class Type>
// void a() {
//   glVertexAttribPointer(Index, Type::N, Type::type, Stride, reinterpret_cast<void*>(Pointer));
//   glEnableVertexAttribArray(Index);
// }

// template<class ...Args>
// void a0() {
//   a<Stride<Args...>::value, 0, 0, Args...>();
// }

// template<int N>
// struct Float {};

// template<class ...Args>
// struct Attrib {};

// template<>
// struct Attrib<Float<3>, Float<3>, Float<2>> {
//   void attrib() {
//     glVertexAttribPointer(Index, N, gl_type<Type>, Stride, reinterpret_cast<void*>(Pointer));
//     glEnableVertexAttribArray(Index);
//   }
// }

struct __attribute__((packed)) VaoElementRenderQuad {
    GLfloat position[3];
    GLfloat uv[2];
};

template <class T>
struct X {
};

template <>
struct X<CEVaoElementChunkBlock> {
    static void DefineAttribs()
    {
        GL_CHECK_DEBUG();

        constexpr GLsizei stride = sizeof(CEVaoElementChunkBlock);
        static_assert(stride == 32, "packed check");

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);

        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        // uv attribute
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        GL_CHECK_DEBUG();
    }
};

template <>
struct X<VaoElementRenderQuad> {
    static void DefineAttribs()
    {
        GL_CHECK_DEBUG();

        constexpr GLsizei stride = sizeof(VaoElementRenderQuad);
        static_assert(stride == 20, "packed check");

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);

        // uv attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        GL_CHECK_DEBUG();
    }
};

template <>
struct X<CEVaoElementBlockSelectBlock> {
    static void DefineAttribs()
    {
        GL_CHECK_DEBUG();

        constexpr GLsizei stride = sizeof(CEVaoElementBlockSelectBlock);
        static_assert(stride == 12, "packed check");

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)(0 * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        GL_CHECK_DEBUG();
    }
};

template <GLenum Mode, class T>
class Vao {
public:
    Vao() {}
    ~Vao()
    {
        if (vbo_ != 0) {
            glDeleteBuffers(1, &vbo_);
            vbo_ = 0;
        }

        if (vao_ != 0) {
            glDeleteVertexArrays(1, &vao_);
            vao_ = 0;
        }
    }

    void Initialize(const T* data, int count)
    {
        GL_CHECK_DEBUG();

        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * count, data, GL_STATIC_DRAW);

        glBindVertexArray(vao_);
        X<T>::DefineAttribs();
        glBindVertexArray(0);

        count_ = count;

        GL_CHECK_DEBUG();
    }

    void Render() const
    {
        if (count_ > 0 && vao_ != 0) {
            GL_CHECK_DEBUG();
            glBindVertexArray(vao_);
            glDrawArrays(Mode, 0, count_);
            glBindVertexArray(0);
            GL_CHECK_DEBUG();
        }
    }

private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLsizei count_ = 0;

    BOOST_MOVABLE_BUT_NOT_COPYABLE(Vao)
};

} // namespace chunklands::engine::gl

#endif