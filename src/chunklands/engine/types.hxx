#ifndef __CHUNKLANDS_ENGINE_ENGINETYPES_HXX__
#define __CHUNKLANDS_ENGINE_ENGINETYPES_HXX__

#include <boost/signals2.hpp>
#include <chunklands/engine/engine_exception.hxx>
#include <chunklands/libcxx/boost_thread.hxx>
#include <chunklands/libcxx/glfw.hxx>
#include <string>
#include <vector>

namespace chunklands::engine {

template <class T>
struct OkResultX {
    T value;
};

struct OkEmptyResultX {
};

template <class E>
struct ErrResultX {
    E value;
};

template <class E, class T>
class ResultX {
public:
    ResultX(OkResultX<T>&& ok)
        : result_(std::move(ok.value))
    {
    }

    ResultX(OkEmptyResultX&&)
        : result_(T())
    {
    }

    ResultX(ErrResultX<E>&& err)
        : result_(std::move(err.value))
    {
    }

    bool IsError() const
    {
        return std::holds_alternative<E>(result_);
    }

    bool IsOk() const
    {
        return std::holds_alternative<T>(result_);
    }

    const T& Value() const
    {
        return std::get<T>(result_);
    }

    T& Value()
    {
        return std::get<T>(result_);
    }

    const E& Error() const
    {
        return std::get<E>(result_);
    }

    E& Error()
    {
        return std::get<E>(result_);
    }

private:
    std::variant<E, T> result_;
};

template <class E>
static inline ErrResultX<E> Err(E e)
{
    return ErrResultX<E> { std::move(e) };
}

template <class T>
static inline OkResultX<T> Ok(T&& value)
{
    return OkResultX<T> { std::forward<T>(value) };
};

template <class T>
static inline OkResultX<T> Ok(T& value)
{
    return OkResultX<T> { value };
};

static inline OkEmptyResultX Ok()
{
    return OkEmptyResultX();
};

template <class E, class T>
class AsyncResultX {
public:
    // using AsyncResult2<E, T>;
    // using AsyncResult2<E, T>::AsyncResult2;

    // AsyncResultX(boost::future<ResultX<E, T>> future) {
    //   (*this) = std::move(future);
    // }

    AsyncResultX(ErrResultX<E>&& value)
    {
        boost::promise<ResultX<E, T>> p;
        p.set_value(std::move(value));
        (*this) = p.get_future();
    }

    AsyncResultX(OkResultX<T>&& value)
    {
        boost::promise<ResultX<E, T>> p;
        p.set_value(std::move(value));
        (*this) = p.get_future();
    }

    AsyncResultX(boost::future<ResultX<E, T>> future)
        : future_(std::move(future))
    {
    }

    boost::future<ResultX<E, T>>& Future()
    {
        return future_;
    }

    const boost::future<ResultX<E, T>>& Future() const
    {
        return future_;
    }

    // AsyncResult3(const AsyncResult3<E, T>&) = default;
    // AsyncResult3(AsyncResult3<E, T>&&) = default;
    // AsyncResult3<E, T>& operator=(const AsyncResult3<E, T>&) = default;
    // AsyncResult3<E, T>& operator=(AsyncResult3<E, T>&&) = default;
private:
    boost::future<ResultX<E, T>> future_;
};

template <class T>
using EngineResultX = ResultX<engine_exception, T>;

template <class T>
using AsyncEngineResult = AsyncResultX<engine_exception, T>;

template <class T>
using Result = std::variant<engine_exception, T>;

template <class T>
using AsyncResult = boost::future<Result<T>>;

using CENone = std::monostate;

using EventConnection = boost::signals2::scoped_connection;

struct CEWindowHandle;
struct CEChunkHandle;
struct CEGBufferMeshHandle;
struct CEBlockRegistrar;
struct CEBlockHandle;
struct CESpriteHandle;
struct CETextHandle;
struct CEFontHandle;

constexpr int CE_CHUNK_SIZE = 32;
constexpr int CE_CHUNK_BLOCK_COUNT = CE_CHUNK_SIZE * CE_CHUNK_SIZE * CE_CHUNK_SIZE;

enum class FaceType {
    Unknown,
    Left,
    Right,
    Bottom,
    Top,
    Front,
    Back,
};

struct __attribute__((packed)) CEVaoElementChunkBlock {
    GLfloat position[3];
    GLfloat normal[3];
    GLfloat uv[2];
};

struct __attribute__((packed)) CEVaoElementSprite {
    GLfloat position[3];
    GLfloat normal[3];
    GLfloat uv[2];
};

struct __attribute__((packed)) CEVaoElementText {
    GLfloat position[2];
    GLfloat uv[2];
};

struct __attribute__((packed)) CEVaoElementBlockSelectBlock {
    GLfloat position[3];
};

struct CEBlockFace {
    FaceType type;
    std::vector<CEVaoElementChunkBlock> data;
};

struct CEBlockCreateInit {
    std::string id;
    bool opaque = false;
    std::vector<CEBlockFace> faces;
    std::vector<unsigned char> texture;
};

struct CESpriteCreateInit {
    std::string id;
    std::vector<CEVaoElementSprite> data;
    std::vector<unsigned char> texture;
};

struct CEPassInit {
    std::string vertex_shader;
    std::string fragment_shader;
};

struct CERenderPipelineInit {
    CEPassInit gbuffer;
    CEPassInit lighting;
    CEPassInit select_block;
    CEPassInit sprite;
    CEPassInit text;
};

struct CEEvent {
    CEEvent(std::string type)
        : type(std::move(type))
    {
    }
    std::string type;
};

struct CEWindowEvent : public CEEvent {
    using CEEvent::CEEvent;

    union {
        struct {
            int button;
            int action;
            int mods;
        } click;

        struct {
            int key;
            int scancode;
            int action;
            int mods;
        } key;
    };
};

struct CECameraPosition {
    float x, y, z;
};

struct CECameraEvent : public CEEvent {
    using CEEvent::CEEvent;
    union {
        CECameraPosition positionchange;
    };
};

struct CEVector2f {
    float x, y;
};

struct CETextUpdate {
    std::optional<CEVector2f> pos;
    std::optional<std::string> text;
};

struct CEFontInitCharacter {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    int originX = 0;
    int originY = 0;
    int advance = 0;
};

struct CEFontInit {
    std::string name;
    int size;
    int width;
    int height;
    std::array<CEFontInitCharacter, 256> characters;
    std::vector<unsigned char> texture;
};

} // namespace chunklands::engine

#endif