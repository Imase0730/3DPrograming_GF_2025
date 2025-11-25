//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    // TODO: Provide parameters for swapchain format, depth/stencil format, and backbuffer count.
    //   Add DX::DeviceResources::c_AllowTearing to opt-in to variable rate displays.
    //   Add DX::DeviceResources::c_EnableHDR for HDR10 display.
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */

    // Fontの変更
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/ARIAL.ttf", 24.0f);

    // デバッグカメラの作成
    m_debugCamera = std::make_unique<Imase::DebugCamera>(width, height);

}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.

    // キー情報を取得
    auto kb = Keyboard::Get().GetState();

#ifdef _DEBUG
    // Debug

    // ImGuiの更新処理
    Imase::DXTK_ImGui::Update();

    ImGui::Begin("Title");

    // ----- ImGuiのウインドウに項目を追加する ----- //

    // --------------------------------------------- //

    // デバッグカメラの更新
    m_debugCamera->Update(!ImGui::IsWindowFocused());

    ImGui::End();

#else
    // Release

    // デバッグカメラの更新
    m_debugCamera->Update();

#endif // _DEBUG

}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
    context;

    // ビュー行列を取得する
    SimpleMath::Matrix view = m_debugCamera->GetCameraMatrix();

    // グリッドの床の描画
    m_gridFloor->Render(context, view, m_proj);

    // ----- 四角形の描画 ----- //

    // 頂点データ
    VertexPositionTexture v[4] = 
    {
        { SimpleMath::Vector3(-1.0f,  1.0f, 0.0f), SimpleMath::Vector2( 0.0f, 0.0f) }, // 0
        { SimpleMath::Vector3( 1.0f,  1.0f, 0.0f), SimpleMath::Vector2( 2.0f, 0.0f) }, // 1
        { SimpleMath::Vector3( 1.0f, -1.0f, 0.0f), SimpleMath::Vector2( 2.0f, 2.0f) }, // 2
        { SimpleMath::Vector3(-1.0f, -1.0f, 0.0f), SimpleMath::Vector2( 0.0f, 2.0f) }, // 3
    };

    // インデックスデータ
    uint16_t indexes[6] = { 0, 1, 2, 0, 2, 3 };

    // ラスタライザーステートの設定（反時計周りをカリングする）
    context->RSSetState(m_states->CullCounterClockwise());

    // ブレンドステートの設定（不透明）
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xffffffff);

    // デプスステンシルステートの設定（通常の設定）
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);

    // テクスチャサンプラーの設定
    ID3D11SamplerState* samples[] = { m_states->LinearWrap() };
    context->PSSetSamplers(0, 1, samples);

    SimpleMath::Matrix world;

    // ワールド行列を設定
    m_basicEffect->SetWorld(world);
    // ビュー行列を設定
    m_basicEffect->SetView(view);
    // 射影行列を設定
    m_basicEffect->SetProjection(m_proj);

    // テクスチャを設定する
    m_basicEffect->SetTexture(m_diceTexture.Get());

    // エフェクトを適応する
    m_basicEffect->Apply(context);

    // 入力レイアウトを設定する
    context->IASetInputLayout(m_inputLayout.Get());

    // プリミティブバッチで描画する
    m_primitiveBatch->Begin();

    // 三角形を描画する
    m_primitiveBatch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, indexes, 6, v, 4);

    m_primitiveBatch->End();

    // デバッグフォントの描画
    m_debugFont->Render(m_states.get());

#ifdef _DEBUG
    // ImGuiの描画処理
    Imase::DXTK_ImGui::Render();
#endif // _DEBUG

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    const auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    const auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange()
{
    m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 1280;
    height = 720;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Initialize device dependent objects here (independent of window size).
    device;

#ifdef _DEBUG
    // ImuGuiの初期化
    Imase::DXTK_ImGui::Initialize(m_deviceResources->GetWindow(), device, context);
#endif // _DEBUG

    // コモンステートの作成
    m_states = std::make_unique<CommonStates>(device);

    // デバッグフォントの作成
    m_debugFont = std::make_unique<Imase::DebugFont>(device, context
        , L"Resources/Font/SegoeUI_18.spritefont");

    // グリッドの床の作成
    m_gridFloor = std::make_unique<Imase::GridFloor>(
        device, context, m_states.get());

    // プリミティブバッチの作成
    m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionTexture>>(context);

    // ベーシックエフェクトの作成
    m_basicEffect = std::make_unique<BasicEffect>(device);

    // 頂点カラーを使用しない
    m_basicEffect->SetVertexColorEnabled(false);

    // テクスチャを使用する
    m_basicEffect->SetTextureEnabled(true);

    // ライトを使用しない
    m_basicEffect->SetLightingEnabled(false);

    // 入力レイアウトの作成
    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexPositionTexture>(
            device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf())
    );

    // テクスチャの読み込み（dice.dds）
    DX::ThrowIfFailed(
        CreateDDSTextureFromFile( device, L"Resources/Textures/dice.dds"
                                , nullptr, m_diceTexture.ReleaseAndGetAddressOf())
    );

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.

    // 画面サイズを取得する
    int w, h;
    GetDefaultSize(w, h);

    // 射影行列の作成
    m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
        // 画角　　　　　　　　　　アスペクト比
        XMConvertToRadians(45.0f), static_cast<float>(w) / static_cast<float>(h),
        // Near Far
        0.1f, 100.0f);

}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
