/**
 * @copyright 2022-present Brian Cairl
 *
 * @file weather.cpp
 */

// C++ Standard Library
#include <cstdio>

// Tyl
#include <tyl/debug/assert.hpp>
#include <tyl/graphics/device/debug.hpp>
#include <tyl/graphics/device/shader.hpp>
#include <tyl/graphics/device/texture.hpp>
#include <tyl/graphics/device/vertex_buffer.hpp>
#include <tyl/graphics/host/image.hpp>
#include <tyl/grid/kernel_op.hpp>
#include <tyl/math/vec.hpp>
#include <tyl/window/window.hpp>


using namespace tyl::graphics::device;
using namespace tyl::graphics::host;

void diffuse(
  Eigen::MatrixXf& dst,
  const Eigen::MatrixXf& src,
  const Eigen::MatrixXf& heightmap,
  const float flat_height = 0.1)
{
  TYL_ASSERT_EQ(dst.size(), src.size());
  TYL_ASSERT_EQ(dst.size(), heightmap.size());

  tyl::kernel_op(
    dst.rows(), dst.cols(), [&](const int s_row, const int s_col, const int n_row, const int n_col, const float m) {
      const float s_height = heightmap(s_row, s_col);
      const float s_value = src(s_row, s_col);
      if (s_height > flat_height && s_value > 0.f)
      {
        const float n_height = heightmap(n_row, n_col);
        const float delta = std::min(s_value / 8.f, m * s_value * std::max(0.f, s_height - n_height));
        dst(n_row, n_col) += delta;
        dst(s_row, s_col) -= delta;
      }
    });
  dst += src;
}

void diffuse(Eigen::MatrixXf& dst, const Eigen::MatrixXf& src)
{
  TYL_ASSERT_EQ(dst.size(), src.size());

  tyl::kernel_op(
    dst.rows(), dst.cols(), [&](const int s_row, const int s_col, const int n_row, const int n_col, const float m) {
      const float s_value = src(s_row, s_col);
      const float delta = s_value / 8.f;
      dst(n_row, n_col) += delta;
      dst(s_row, s_col) -= delta;
    });
  dst += src;
}

void evaporate(
  Eigen::MatrixXf& to,
  Eigen::MatrixXf& from,
  const Eigen::MatrixXf& heightmap,
  const Eigen::MatrixXf& temperature,
  const float rate = 0.01f,
  const float min_height = 0.1f)
{
  TYL_ASSERT_EQ(to.size(), from.size());
  TYL_ASSERT_EQ(to.size(), heightmap.size());
  for (int row = 0; row < from.rows(); ++row)
  {
    for (int col = 0; col < from.cols(); ++col)
    {
      if (heightmap(row, col) > min_height)
      {
        float& from_value = from(row, col);
        float& to_value = to(row, col);
        const float delta = rate * temperature(row, col);
        from_value -= delta;
        to_value += delta;
      }
    }
  }
}

void precipitate(
  Eigen::MatrixXf& to,
  Eigen::MatrixXf& from,
  const Eigen::MatrixXf& heightmap,
  const Eigen::MatrixXf& temperature,
  const float rate = 0.01f)
{
  TYL_ASSERT_EQ(to.size(), from.size());
  TYL_ASSERT_EQ(to.size(), heightmap.size());
  for (int row = 0; row < from.rows(); ++row)
  {
    for (int col = 0; col < from.cols(); ++col)
    {
      float& from_value = from(row, col);
      float& to_value = to(row, col);
      const float thresh = (1.f - heightmap(row, col));
      if (from_value > thresh)
      {
        const float delta = rate * from_value;
        from_value -= delta;
        to_value += delta;
      }
    }
  }
}

template <typename FunctionT> void fill_gradient_x(Eigen::MatrixXf& m, const float ts, const float te, FunctionT fn)
{
  const float t_step = (te - ts) / m.cols();
  for (int i = 0; i < m.rows(); ++i)
  {
    m(i, 0) = fn(ts + i * t_step);
  }

  for (int i = 1; i < m.rows(); ++i)
  {
    m.col(i) = m.col(0);
  }
}


int main(int argc, char const* argv[])
{
  tyl::Window window{{.title = "weather-poc", .size = {.height = 500, .width = 500}}};

  auto heightmap_texture = tyl::graphics::host::load("systems/game/weather/heightmap_ds.png");

  Eigen::MatrixXf heightmap{heightmap_texture.height(), heightmap_texture.width()};

  using Pixel = std::array<std::uint8_t, 3>;
  std::transform(
    heightmap_texture.begin<Pixel>(), heightmap_texture.end<Pixel>(), heightmap.data(), [](const Pixel& v) -> float {
      constexpr float kScaling = 1.0 / 256.0;
      return v[0] * kScaling;
    });

  Eigen::MatrixXf prev_gnd_moisture = Eigen::MatrixXf::Zero(heightmap_texture.height(), heightmap_texture.width());
  Eigen::MatrixXf curr_gnd_moisture = Eigen::MatrixXf::Zero(heightmap_texture.height(), heightmap_texture.width());

  std::transform(
    heightmap.data(), heightmap.data() + heightmap.size(), prev_gnd_moisture.data(), [](const float h) -> float {
      return h < 0.4 ? 1.0 : 0.0;
    });

  Eigen::MatrixXf prev_atm_moisture = Eigen::MatrixXf::Zero(heightmap_texture.height(), heightmap_texture.width());
  Eigen::MatrixXf curr_atm_moisture = Eigen::MatrixXf::Zero(heightmap_texture.height(), heightmap_texture.width());

  Eigen::MatrixXf temperature = Eigen::MatrixXf::Zero(heightmap_texture.height(), heightmap_texture.width());

  enable_debug_logs();
  enable_error_logs();

  // Upload texture data
  const Texture texture{
    static_cast<int>(heightmap.rows()), static_cast<int>(heightmap.cols()), heightmap.data(), TextureChannels::R};

  // Download texture data
  // TextureOptions texture_options;
  // auto texture_on_host = texture.download(texture_options);
  // texture_on_host.element<float>(1, 1)[1] = 0.5f;

  texture.bind(1);

  static constexpr std::size_t VERTEX_COUNT = 4;

  auto [vb, elements, positions, texcoords] = VertexElementBuffer::create(
    VertexBuffer::BufferMode::Static,
    6UL,
    VertexAttribute<float, 2>{VERTEX_COUNT},
    VertexAttribute<float, 2>{VERTEX_COUNT});

  {
    auto mapped = vb.get_mapped_element_buffer();

    {
      auto* const p = mapped(elements);

      p[0] = 0;
      p[1] = 1;
      p[2] = 2;

      p[3] = 1;
      p[4] = 2;
      p[5] = 3;
    }
  }

  {
    auto mapped = vb.get_mapped_vertex_buffer();

    {
      auto* const p = mapped(positions);

      p[0] = -1.0f;
      p[1] = -1.0f;

      p[2] = +1.0f;
      p[3] = -1.0f;

      p[4] = -1.0f;
      p[5] = +1.0f;

      p[6] = +1.0f;
      p[7] = +1.0f;
    }

    {
      auto* const p = mapped(texcoords);

      p[0] = +0.0f;
      p[1] = +0.0f;

      p[2] = +1.0f;
      p[3] = +0.0f;

      p[4] = +0.0f;
      p[5] = +1.0f;

      p[6] = +1.0f;
      p[7] = +1.0f;
    }
  }

  Shader shader{
    ShaderSource::vertex(
      R"VertexShader(

      layout (location = 0) in vec2 vPos;
      layout (location = 1) in vec2 vTexCoord;

      out vec2 fTexCoord;

      void main()
      {
        gl_Position = vec4(vPos, 0, 1);
        fTexCoord = vTexCoord;
      }

      )VertexShader"),
    ShaderSource::fragment(
      R"FragmentShader(

      out vec4 FragColor;

      in vec2 fTexCoord;

      uniform sampler2D fTextureID;

      void main()
      {
        FragColor = texture(fTextureID, fTexCoord);
      }

      )FragmentShader")};

  shader.bind();
  shader.setInt("fTextureID", 1);

  std::size_t iteration = 19;

  std::vector<float> merged;
  merged.resize(heightmap.size() * 3);

  window([&](const auto& s) {
    if (++iteration % 3 == 0)
    {
      const float ts = static_cast<float>(iteration) * 0.1;
      const float te = ts + 1.f;

      // cv::swap(curr_atm_moisture, prev_atm_moisture);
      fill_gradient_x(temperature, ts, te, [](const float t) { return 0.5f * (std::sin(t * 0.025f) + 1.f); });
      temperature -= curr_atm_moisture;

      diffuse(curr_gnd_moisture, prev_gnd_moisture, heightmap, 0.1);
      diffuse(curr_atm_moisture, prev_atm_moisture);
      evaporate(curr_atm_moisture, curr_gnd_moisture, heightmap, temperature, 1e-3f, 0.175f);
      precipitate(curr_gnd_moisture, curr_atm_moisture, heightmap, temperature, 5e-1f);

      for (int i = 0; i < heightmap.size(); ++i)
      {
        merged[3 * i + 2] = curr_gnd_moisture(i);
        merged[3 * i + 1] = heightmap(i) * temperature(i);
        merged[3 * i + 0] = curr_atm_moisture(i);
      }

      texture.upload(TextureView{
        merged.data(),
        static_cast<int>(curr_gnd_moisture.rows()),
        static_cast<int>(curr_gnd_moisture.cols()),
        TextureChannels::RGB});

      prev_gnd_moisture.swap(curr_gnd_moisture);
      curr_gnd_moisture.setZero();

      prev_atm_moisture.swap(curr_atm_moisture);
      curr_atm_moisture.setZero();
    }
    texture.bind(1);
    vb.draw(elements, VertexBuffer::DrawMode::Triangles);
  });

  return 0;
}