//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
    : m_diceRotateY{ 0.0f }
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

    // �f�o�b�O�J�����̍쐬
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

    // �P�b�ԂłS�T�x��]����
    m_diceRotateY += 45.0f * elapsedTime;

    // �L�[�����擾
    auto kb = Keyboard::Get().GetState();

    // �E�L�[�������ꂽ
    if (kb.Right)
    {
        m_dicePosition.x += 1.0f * elapsedTime;
    }

    // ���L�[�������ꂽ
    if (kb.Left)
    {
        m_dicePosition.x -= 1.0f * elapsedTime;
    }

#ifdef _DEBUG
    // ImGui�̍X�V����
    Imase::DXTK_ImGui::Update();
#endif // _DEBUG

    // �f�o�b�O�J�����̍X�V
    m_debugCamera->Update();
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

    // �r���[�s����擾����
    SimpleMath::Matrix view = m_debugCamera->GetCameraMatrix();

    // �O���b�h�̏��̕`��
    m_gridFloor->Render(context, view, m_proj);

    // ���[���h�s��
    SimpleMath::Matrix world_1, world_2, world_3;

    // ���s�ړ��s����쐬�i�Q�C�O�C�O�Ɉړ�����֐��j
    SimpleMath::Matrix trans = SimpleMath::Matrix::CreateTranslation(2.0f, 0.0f, 0.0f);

    // ��]�s����쐬�i�P�b�Ԃ�Y���𒆐S�ɂS�T�x��]����s��j
    SimpleMath::Matrix rotateY = SimpleMath::Matrix::CreateRotationY(XMConvertToRadians(m_diceRotateY));

    // �g��k���s����쐬�i���_�𒆐S�ɑ傫���𔼕��ɂ���s��j
    SimpleMath::Matrix scale = SimpleMath::Matrix::CreateScale(0.5f);

    world_1 = rotateY;

    // �T�C�R���̕`��
    m_dice->Draw(context, *m_states.get(), world_1, view, m_proj);

    world_2 = scale * rotateY * trans * world_1;

    // �T�C�R���̕`��
    m_dice->Draw(context, *m_states.get(), world_2, view, m_proj);

    world_3 = scale * rotateY * trans * world_2;

    // �T�C�R���̕`��
    m_dice->Draw(context, *m_states.get(), world_3, view, m_proj);

    // �f�o�b�O�t�H���g�̕`��
    m_debugFont->Render(m_states.get());

#ifdef _DEBUG
    // ImGui�̕`�揈��
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
    // ImuGui�̏�����
    Imase::DXTK_ImGui::Initialize(m_deviceResources->GetWindow(), device, context);
#endif // _DEBUG

    // �R�����X�e�[�g�̍쐬
    m_states = std::make_unique<CommonStates>(device);

    // �f�o�b�O�t�H���g�̍쐬
    m_debugFont = std::make_unique<Imase::DebugFont>(device, context
        , L"Resources/Font/SegoeUI_18.spritefont");

    // �O���b�h�̏��̍쐬
    m_gridFloor = std::make_unique<Imase::GridFloor>(
        device, context, m_states.get());

    // �T�C�R���̍쐬
    EffectFactory fx(device);
    fx.SetDirectory(L"Resources/Models");
    m_dice = Model::CreateFromCMO(device, L"Resources/Models/Dice.cmo", fx);

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.

    // ��ʃT�C�Y���擾����
    int w, h;
    GetDefaultSize(w, h);

    // �ˉe�s��̍쐬
    m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
        // ��p�@�@�@�@�@�@�@�@�@�@�A�X�y�N�g��
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
