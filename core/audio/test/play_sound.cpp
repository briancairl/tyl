/**
 * @copyright 2023-present Brian Cairl
 *
 * @file play_sound.cpp
 */

// C++ Standard Library
#include <chrono>
#include <cmath>
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

  if (!audio_device.enable())
  {
    std::fprintf(stderr, "[ERROR] %s\n", "Failed to enable device");
    return 1;
  }

  device::Listener audio_listener{audio_device};

  auto sound_data_or_error = host::SoundData::load(argv[1]);
  if (!sound_data_or_error.has_value())
  {
    std::fprintf(stderr, "[ERROR] %s: %d\n", "Failed to load sound", static_cast<int>(sound_data_or_error.error()));
    return 1;
  }

  auto sound = sound_data_or_error->sound();

  device::Source audio_source;
  audio_source.set_pitch_scaling(2.5);
  audio_source.set_volume(2.0);

  auto playback = audio_source.play(sound);

  while (playback.is_playing())
  {
    const float p = playback.progress();
    std::fprintf(stderr, "progress: %f\n", p);
    audio_listener.set_position(std::cos(2 * p), std::sin(2 * p), 0);
    std::this_thread::sleep_for(std::chrono::milliseconds{50});

    if (p > 0.5)
    {
      playback.stop();
      break;
    }
  }

  return 0;
}
