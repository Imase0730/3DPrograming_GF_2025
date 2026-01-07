//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
    : m_ambientLightColor{}
    , m_lightDiffuseColor{}
    , m_lightSpecularColor{}
    , m_specularPower{}
    , m_diffuseColor{}
    , m_emissiveColor{}
    , m_ballModel{}
    , m_floorModel{}
    , m_shadowModel{}
    , m_treeModel{}
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

#ifdef _DEBUG
    // Fontの変更
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/ARIAL.ttf", 16.0f);
#endif

    // デバッグカメラの作成
    m_debugCamera = std::make_unique<Imase::DebugCamera>(width, height);

    // アンビエントライト色の初期設定
    m_ambientLightColor = { 0.3f, 0.3f, 0.3f };

    // ライトの向きの初期設定
    m_lightDirection = { 0.0f, -1.0f, 0.0f };

    // ライトのディフューズ色の初期設定
    m_lightDiffuseColor = Colors::White;

    // ライトのスペキュラー色の初期設定
    m_lightSpecularColor = Colors::White;

    // ディフューズ色の初期設定
    m_diffuseColor = Colors::White;

    // スペキュラー色の初期設定
    m_specularColor = Colors::White;

    // スペキュラーパワーの初期設定
    m_specularPower = 80.0f;

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
    Imase::DXTK_ImGui::Update(m_deviceResources->GetWindow());

    ImGui::Begin("Light & Material");

    // ----- ImGuiのウインドウに項目を追加する ----- //

    //std::vector<float> v;

    //ImGui::SeparatorText("LIGHT SETTING:");

    //// ライトの向き
    //v = { m_lightDirection.x, m_lightDirection.y, m_lightDirection.z };
    //ImGui::DragFloat3("LightDirection", v.data(), 0.01f);
    //m_lightDirection = { v[0], v[1], v[2] };
    //m_lightDirection.Normalize();

    //// アンビエントライト色
    //v = { m_ambientLightColor.x, m_ambientLightColor.y, m_ambientLightColor.z };
    //ImGui::ColorEdit3("AmbientLightColor", v.data());
    //m_ambientLightColor = { v[0], v[1], v[2] };

    //// ライトのディフューズ色
    //v = { m_lightDiffuseColor.x, m_lightDiffuseColor.y, m_lightDiffuseColor.z };
    //ImGui::ColorEdit3("LightDiffuseColor", v.data());
    //m_lightDiffuseColor = { v[0], v[1], v[2] };

    //// ライトのスペキュラー色
    //v = { m_lightSpecularColor.x, m_lightSpecularColor.y, m_lightSpecularColor.z };
    //ImGui::ColorEdit3("LightSpecularColor", v.data());
    //m_lightSpecularColor = { v[0], v[1], v[2] };

    //ImGui::SeparatorText("MATERIAL SETTING:");

    //// ディフューズ色
    //v = { m_diffuseColor.x, m_diffuseColor.y, m_diffuseColor.z };
    //ImGui::ColorEdit3("DiffuseColor", v.data());
    //m_diffuseColor = { v[0], v[1], v[2] };

    //// スペキュラー色
    //v = { m_specularColor.x, m_specularColor.y, m_specularColor.z };
    //ImGui::ColorEdit3("SpecularColor", v.data());
    //m_specularColor = { v[0], v[1], v[2] };

    //// スペキュラーパワー
    //ImGui::DragFloat("SpecuarPower", &m_specularPower, 1.0f, 10.0f, 100.0f);

    //// エミッシブ色
    //v = { m_emissiveColor.x, m_emissiveColor.y, m_emissiveColor.z };
    //ImGui::ColorEdit3("EmissiveColor", v.data());
    //m_emissiveColor = { v[0], v[1], v[2] };

    // --------------------------------------------- //

    // デバッグカメラの更新
    m_debugCamera->Update(!ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows));

    ImGui::End();

//    ImGui::ShowDemoWindow();
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
    //m_gridFloor->Render(context, view, m_proj);

    SimpleMath::Matrix world;

    // モデルを描画する（床）
    world = SimpleMath::Matrix::Identity;
    m_floorModel->Draw(context, *m_states, world, view, m_proj, false, [&]()
        {
            // テクスチャサンプラーの設定
            ID3D11SamplerState* samplers[] = { m_states->PointWrap() };
            context->PSSetSamplers(0, 1, samplers);
        }
    );
    
    // 木を描画する関数
    DrawTree(SimpleMath::Vector3(0.0f, 0.0f, 0.0f));

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
    int w, h;
    GetDefaultSize(w, h);
    // ImuGuiの初期化
    Imase::DXTK_ImGui::Initialize(m_deviceResources->GetWindow(), device, context, w, h);
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
    m_primitiveBatch = std::make_unique<PrimitiveBatch<VertexPositionNormal>>(context);

    // ベーシックエフェクトの作成
    m_basicEffect = std::make_unique<BasicEffect>(device);

    // 頂点カラーを使用しない
    m_basicEffect->SetVertexColorEnabled(false);

    // テクスチャを使用しない
    m_basicEffect->SetTextureEnabled(false);

    // ライトを使用する
    m_basicEffect->SetLightingEnabled(true);

    // ディフォルトライトを設定する
    m_basicEffect->EnableDefaultLighting();

    // ピクセルシェーダーでライトの計算を行う
    m_basicEffect->SetPerPixelLighting(true);

    // 入力レイアウトの作成
    DX::ThrowIfFailed(
        CreateInputLayoutFromEffect<VertexPositionNormalTexture>(
            device, m_basicEffect.get(), m_inputLayout.ReleaseAndGetAddressOf())
    );

    EffectFactory fx(device);
    fx.SetDirectory(L"Resources/Models");

    // モデルの読み込み（床）
    m_floorModel = Model::CreateFromCMO(device, L"Resources/Models/Floor.cmo", fx);

    // モデルの読み込み（影）
    m_shadowModel = Model::CreateFromCMO(device, L"Resources/Models/Shadow.cmo", fx);

    // モデルの読み込み（木）
    m_treeModel = Model::CreateFromCMO(device, L"Resources/Models/Tree.cmo", fx);

    // ビルボードのライトの影響をなくす
    m_treeModel->UpdateEffects([&](IEffect* effect)
        {
            BasicEffect* basicEffect = dynamic_cast<BasicEffect*>(effect);
            if (basicEffect)
            {
                // ライトを切る
                basicEffect->SetLightEnabled(0, false);
                basicEffect->SetLightEnabled(1, false);
                basicEffect->SetLightEnabled(2, false);
                // 自己発光する
                basicEffect->SetEmissiveColor(Colors::White);
            }
        }
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

// 木を描画する関数
void Game::DrawTree(DirectX::SimpleMath::Vector3 position)
{
    auto context = m_deviceResources->GetD3DDeviceContext();

    SimpleMath::Matrix world;

    // ビュー行列を取得する
    SimpleMath::Matrix view = m_debugCamera->GetCameraMatrix();

    // ----- 丸影の描画 ----- //

    // ワールド行列の作成（Y座標はZファイティング対策のため0.01上に上げている）
    world = SimpleMath::Matrix::CreateTranslation(position.x, position.y + 0.01f, position.z);

    // モデルを描画する（影）
    m_shadowModel->Draw(context, *m_states, world, view, m_proj, false, [&]()
        {
            // 半透明の設定（ストレートアルファ）
            context->OMSetBlendState(m_states->NonPremultiplied(), nullptr, 0xffffffff);
            // 深度ステンシルステートの設定
            context->OMSetDepthStencilState(m_states->DepthRead(), 0);
        }
    );

    // ----- ビルボードの木を描画する ----- //

    // ビルボード用の行列を作成する
    world = SimpleMath::Matrix::CreateConstrainedBillboard(position
        , m_debugCamera->GetEyePosition(), SimpleMath::Vector3::Up);

    m_treeModel->Draw(context, *m_states, world, view, m_proj, false, [&]()
        {
            // 半透明の設定（ストレートアルファ）
            context->OMSetBlendState(m_states->NonPremultiplied(), nullptr, 0xffffffff);
            // 深度ステンシルステートの設定
            context->OMSetDepthStencilState(m_states->DepthRead(), 0);
        }
    );

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
