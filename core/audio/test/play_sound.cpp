/**
 * @copyright 2023-present Brian Cairl
 *
 * @file play_sound.cpp
 */

//
// A simple test program which draws VBO colored with a texture
// Also downloads/re-uploads texture data
//

// C++ Standard Library
#include <chrono>
#include <cstdio>
#include <thread>

// Tyl
#include <tyl/audio/device/device.hpp>
#include <tyl/audio/device/listener.hpp>
#include <tyl/audio/device/source.hpp>
#include <tyl/audio/host/sound_data.hpp>

using namespace tyl::audio;

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    std::fprintf(stderr, "%s <sound file>\n", argv[0]);
    return 1;
  }

  device::Device audio_device;
  device::Listener audio_listener{audio_device};

  auto sound_or_error = host::SoundData::load(argv[1]);

  if (!sound_or_error.has_value())
  {
    std::fprintf(stderr, "[ERROR] Failed to load sound: %d\n", static_cast<int>(sound_or_error.error()));
    return 1;
  }

  device::Source audio_source;
  auto playback = audio_source.play(sound_or_error->sound());

  while (playback.is_playing())
  {
    std::this_thread::sleep_for(std::chrono::seconds{1});
  }

  return 0;
}
