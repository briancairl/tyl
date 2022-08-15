#ifdef TYL_AUDIO_AL_INL
#error("Detected double include of al.inl")
#else
#define TYL_AUDIO_AL_INL
#endif  // TYL_AUDIO_AL_INL

// clang-format off

// OpenAL
#include <AL/al.h>
#include <AL/alc.h>

// clang-format on

// C++ Standard Library
#include <type_traits>

// Tyl
#include <tyl/common/assert.hpp>
#include <tyl/audio/device/typedef.hpp>

namespace tyl::audio::device
{

static_assert(std::is_same<ALuint, source_handle_t>());

static_assert(std::is_same<ALuint, buffer_handle_t>());

static inline const char* al_error_to_str(const ALenum error)
{
    switch (error)
    {
        case AL_NO_ERROR: return "AL_NO_ERROR";
        case AL_INVALID_NAME: return "AL_INVALID_NAME";
        case AL_INVALID_ENUM: return "AL_INVALID_ENUM";
        case AL_INVALID_VALUE: return "AL_INVALID_VALUE";
        case AL_INVALID_OPERATION: return "AL_INVALID_OPERATION";
        case AL_OUT_OF_MEMORY: return "AL_OUT_OF_MEMORY";
    }
    return "<<INVALID ERROR CODE>>";
}

}  // tyl::audio::device

#ifndef NDEBUG
    #define TYL_AL_CHECK_LAST_ERROR() \
    {\
        const ALenum __last_openal_err = alGetError();\
        TYL_ASSERT(__last_openal_err == AL_NO_ERROR, al_error_to_str(__last_openal_err));\
    }
#else
    #define TYL_AL_CHECK_LAST_ERROR()
#endif // NDEBUG

#define TYL_AL_TEST_ERROR(statement) (statement); TYL_AL_CHECK_LAST_ERROR();
