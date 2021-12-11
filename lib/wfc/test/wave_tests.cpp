/**
 * @copyright 2021 Tyl
 * @author Brian Cairl
 */

// C++ Standard Library
#include <type_traits>

// GTest
#include <gtest/gtest.h>

// Tyl
#include <tyl/common/bitops.hpp>
#include <tyl/wfc/wave.hpp>

using namespace tyl::wfc;

static const float TEST_WAVE_ELEMENT_PROBABILITIES[4] = {0.1f, 0.2f, 0.3f, 0.4f};

static constexpr std::size_t TEST_WAVE_ELEMENT_COUNT = std::extent_v<decltype(TEST_WAVE_ELEMENT_PROBABILITIES)>;

static const WaveParameters TEST_WAVE_PARAMETERS{
  .layer_count = 2,
  .row_count = 10,
  .col_count = 10,
  .element_probabilities = tyl::make_array_view(TEST_WAVE_ELEMENT_PROBABILITIES),
};

TEST(Wave, InitialState)
{
  Wave wave{TEST_WAVE_PARAMETERS};

  // Element count fits into chunk storage (uint8_t), so it should match size
  ASSERT_EQ(wave.chunks(), wave.size());

  ASSERT_EQ(wave.rows(), TEST_WAVE_PARAMETERS.row_count);

  ASSERT_EQ(wave.cols(), TEST_WAVE_PARAMETERS.col_count);

  ASSERT_EQ(wave.layers(), TEST_WAVE_PARAMETERS.layer_count);

  ASSERT_EQ(
    TEST_WAVE_PARAMETERS.layer_count * TEST_WAVE_PARAMETERS.row_count * TEST_WAVE_PARAMETERS.col_count, wave.size());

  for (const auto& state : wave)
  {
    ASSERT_EQ(state.count(), TEST_WAVE_ELEMENT_COUNT);
  }
}

TEST(Wave, CollapsedState)
{
  Wave wave{TEST_WAVE_PARAMETERS};

  const Loc location{0, 1, 2};

  const ElementID id = 3;

  ASSERT_GT(wave.entropy(location), 0.f);
  ASSERT_FALSE(wave.is_collapsed(location));
  ASSERT_EQ(wave.possible_states(location), TEST_WAVE_ELEMENT_COUNT);

  wave.collapse(location, id);

  // Entropy after collapsation should be 0 (lower)
  ASSERT_NEAR(wave.entropy(location), 0.f, 1e-9f);
  ASSERT_TRUE(wave.is_collapsed(location));
  ASSERT_EQ(wave.possible_states(location), 1UL);
}

TEST(Wave, EliminatingState)
{
  Wave wave{TEST_WAVE_PARAMETERS};

  const Loc location{0, 1, 2};

  ASSERT_FALSE(wave.is_collapsed(location));

  float prev_entropy = wave.entropy(location);
  for (ElementID i = 1; i < TEST_WAVE_ELEMENT_COUNT; ++i)
  {
    wave.eliminate(location, i);

    const float curr_entropy = wave.entropy(location);

    ASSERT_LT(curr_entropy, prev_entropy);

    prev_entropy = curr_entropy;
  }

  ASSERT_TRUE(wave.is_collapsed(location));
}