/**
 * @file runtime_renderer.cpp
 * @brief Initializes bgfx on the SDL3 Win32 window and submits one static scene mesh.
 */
#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "runtime_renderer.h"

#include <SDL3/SDL.h>
#include <bgfx/bgfx.h>
#include <dxbc/fs_draw.sc.bin.h>
#include <dxbc/vs_draw.sc.bin.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

namespace Veyra::Runtime
{
namespace
{
constexpr bgfx::ViewId SceneView = 0;
constexpr std::uint32_t ClearColor = 0x20222AFFU;

struct GpuVertex
{
    float x;
    float y;
    float z;
    std::uint32_t color;
};

bgfx::VertexLayout makeVertexLayout()
{
    bgfx::VertexLayout layout;
    layout.begin()
        .add(bgfx::Attrib::Position, 3U, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4U, bgfx::AttribType::Uint8, true)
        .end();
    return layout;
}

bool setError(std::string* error, const std::string& value)
{
    if (error != nullptr)
        *error = value;
    return false;
}
} // namespace

struct RuntimeRenderer::Impl
{
    bgfx::ProgramHandle program = BGFX_INVALID_HANDLE;
    bgfx::VertexBufferHandle vertexBuffer = BGFX_INVALID_HANDLE;
    bgfx::IndexBufferHandle indexBuffer = BGFX_INVALID_HANDLE;
    bgfx::VertexLayout vertexLayout;
    bool initialized = false;
    int lastWidth = 0;
    int lastHeight = 0;
    std::uint32_t indexCount = 0;
};

RuntimeRenderer::RuntimeRenderer() : m_impl(std::make_unique<Impl>())
{
}

RuntimeRenderer::~RuntimeRenderer()
{
    shutdown();
}

bool RuntimeRenderer::initialize(SDL_Window* window, const SceneGeometry& geometry,
                                 std::string* error)
{
    if (m_impl->initialized)
        return setError(error, "The runtime renderer has already been initialized.");
    if (window == nullptr)
        return setError(error, "Cannot initialize the renderer without an SDL window.");

    const SDL_PropertiesID properties = SDL_GetWindowProperties(window);
    if (properties == 0)
        return setError(error, std::string("Could not read SDL window properties: ") + SDL_GetError());

    HWND nativeWindow = static_cast<HWND>(
        SDL_GetPointerProperty(properties, SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
    if (nativeWindow == nullptr)
        return setError(error, "SDL did not expose a Win32 HWND for the runtime window.");

    int width = 0;
    int height = 0;
    if (!SDL_GetWindowSizeInPixels(window, &width, &height) || width <= 0 || height <= 0)
        return setError(error, std::string("Could not read runtime window pixel size: ") + SDL_GetError());

    bgfx::Init init;
    init.type = bgfx::RendererType::Direct3D11;
    init.platformData.nwh = nativeWindow;
    init.resolution.width = static_cast<std::uint32_t>(width);
    init.resolution.height = static_cast<std::uint32_t>(height);
    init.resolution.reset = BGFX_RESET_VSYNC;
    if (!bgfx::init(init))
        return setError(error, "bgfx could not initialize the Direct3D 11 renderer.");

    m_impl->initialized = true;
    m_impl->lastWidth = width;
    m_impl->lastHeight = height;
    m_impl->vertexLayout = makeVertexLayout();

    const bgfx::Memory* vertexShaderMemory =
        bgfx::makeRef(vs_draw_dxbc, static_cast<std::uint32_t>(sizeof(vs_draw_dxbc)));
    const bgfx::Memory* fragmentShaderMemory =
        bgfx::makeRef(fs_draw_dxbc, static_cast<std::uint32_t>(sizeof(fs_draw_dxbc)));
    const bgfx::ShaderHandle vertexShader = bgfx::createShader(vertexShaderMemory);
    const bgfx::ShaderHandle fragmentShader = bgfx::createShader(fragmentShaderMemory);
    if (!bgfx::isValid(vertexShader) || !bgfx::isValid(fragmentShader))
    {
        if (bgfx::isValid(vertexShader)) bgfx::destroy(vertexShader);
        if (bgfx::isValid(fragmentShader)) bgfx::destroy(fragmentShader);
        shutdown();
        return setError(error, "bgfx could not create the embedded scene shaders.");
    }

    m_impl->program = bgfx::createProgram(vertexShader, fragmentShader, true);
    if (!bgfx::isValid(m_impl->program))
    {
        shutdown();
        return setError(error, "bgfx could not link the embedded scene shaders.");
    }

    if (geometry.vertices.size() > std::numeric_limits<std::uint32_t>::max() / sizeof(GpuVertex) ||
        geometry.indices.size() > std::numeric_limits<std::uint32_t>::max() / sizeof(std::uint32_t))
    {
        shutdown();
        return setError(error, "Scene geometry is too large for bgfx's buffer-size limits.");
    }

    if (!geometry.vertices.empty() && !geometry.indices.empty())
    {
        std::vector<GpuVertex> vertices;
        vertices.reserve(geometry.vertices.size());
        for (const GeometryVertex& vertex : geometry.vertices)
            vertices.push_back({vertex.x, vertex.y, 0.0F, vertex.color});

        const std::uint32_t vertexBytes =
            static_cast<std::uint32_t>(vertices.size() * sizeof(GpuVertex));
        const std::uint32_t indexBytes =
            static_cast<std::uint32_t>(geometry.indices.size() * sizeof(std::uint32_t));
        m_impl->vertexBuffer = bgfx::createVertexBuffer(
            bgfx::copy(vertices.data(), vertexBytes), m_impl->vertexLayout);
        m_impl->indexBuffer = bgfx::createIndexBuffer(
            bgfx::copy(geometry.indices.data(), indexBytes), BGFX_BUFFER_INDEX32);
        if (!bgfx::isValid(m_impl->vertexBuffer) || !bgfx::isValid(m_impl->indexBuffer))
        {
            shutdown();
            return setError(error, "bgfx could not upload the scene geometry buffers.");
        }
        m_impl->indexCount = static_cast<std::uint32_t>(geometry.indices.size());
    }

    bgfx::setViewName(SceneView, "Veyra 2D Scene");
    bgfx::setViewClear(SceneView, BGFX_CLEAR_COLOR, ClearColor, 1.0F, 0U);
    bgfx::setViewRect(SceneView, 0U, 0U,
                      static_cast<std::uint16_t>(std::min(width, 65535)),
                      static_cast<std::uint16_t>(std::min(height, 65535)));
    if (error != nullptr)
        error->clear();
    return true;
}

void RuntimeRenderer::render(int pixelWidth, int pixelHeight)
{
    if (!m_impl->initialized)
        return;

    if (pixelWidth <= 0 || pixelHeight <= 0)
    {
        bgfx::frame();
        return;
    }

    if (pixelWidth != m_impl->lastWidth || pixelHeight != m_impl->lastHeight)
    {
        m_impl->lastWidth = pixelWidth;
        m_impl->lastHeight = pixelHeight;
        bgfx::reset(static_cast<std::uint32_t>(pixelWidth),
                    static_cast<std::uint32_t>(pixelHeight), BGFX_RESET_VSYNC);
    }

    const std::uint16_t viewWidth =
        static_cast<std::uint16_t>(std::min(pixelWidth, 65535));
    const std::uint16_t viewHeight =
        static_cast<std::uint16_t>(std::min(pixelHeight, 65535));
    bgfx::setViewRect(SceneView, 0U, 0U, viewWidth, viewHeight);
    bgfx::setViewClear(SceneView, BGFX_CLEAR_COLOR, ClearColor, 1.0F, 0U);

    const float view[16] = {
        1.0F, 0.0F, 0.0F, 0.0F,
        0.0F, 1.0F, 0.0F, 0.0F,
        0.0F, 0.0F, 1.0F, 0.0F,
        0.0F, 0.0F, 0.0F, 1.0F
    };
    const float projection[16] = {
        2.0F / static_cast<float>(pixelWidth), 0.0F, 0.0F, 0.0F,
        0.0F, 2.0F / static_cast<float>(pixelHeight), 0.0F, 0.0F,
        0.0F, 0.0F, 1.0F, 0.0F,
        0.0F, 0.0F, 0.0F, 1.0F
    };
    bgfx::setViewTransform(SceneView, view, projection);

    if (m_impl->indexCount > 0U &&
        bgfx::isValid(m_impl->vertexBuffer) &&
        bgfx::isValid(m_impl->indexBuffer) &&
        bgfx::isValid(m_impl->program))
    {
        bgfx::setVertexBuffer(0U, m_impl->vertexBuffer);
        bgfx::setIndexBuffer(m_impl->indexBuffer, 0U, m_impl->indexCount);
        bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_MSAA);
        bgfx::submit(SceneView, m_impl->program);
    }
    else
    {
        bgfx::touch(SceneView);
    }
    bgfx::frame();
}

void RuntimeRenderer::shutdown() noexcept
{
    if (!m_impl || !m_impl->initialized)
        return;

    if (bgfx::isValid(m_impl->indexBuffer)) bgfx::destroy(m_impl->indexBuffer);
    if (bgfx::isValid(m_impl->vertexBuffer)) bgfx::destroy(m_impl->vertexBuffer);
    if (bgfx::isValid(m_impl->program)) bgfx::destroy(m_impl->program);
    m_impl->indexBuffer = BGFX_INVALID_HANDLE;
    m_impl->vertexBuffer = BGFX_INVALID_HANDLE;
    m_impl->program = BGFX_INVALID_HANDLE;
    m_impl->indexCount = 0U;
    bgfx::shutdown();
    m_impl->initialized = false;
}
} // namespace Veyra::Runtime
