//--------------------------------------------------------------------------------------
// File: DirectXTK_ImGui.cpp
//
// ImGui�̏������܂Ƃ߂��N���X
//
// Date: 2025.10.14
// Author: Hideyasu Imase
//--------------------------------------------------------------------------------------
#include "pch.h"

bool Imase::DXTK_ImGui::m_isInitialized = false;

// �������֐�
void Imase::DXTK_ImGui::Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context)
{
    // ���Z�b�g����
    Imase::DXTK_ImGui::Reset();

    //  �o�[�W�����̊m�F
    IMGUI_CHECKVERSION();

    //  �R���e�L�X�g�̍쐬
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // �L�[�{�[�h�ɂ��i�r�Q�[�V�����̗L����
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // �R���g���[���[�ɂ��i�r�Q�[�V�����̗L����

    //  Win32�p�̏�����
    ImGui_ImplWin32_Init(hWnd);

    //  DirectX11�p�̏�����
    ImGui_ImplDX11_Init(device, context);
}

// ���Z�b�g�֐�
void Imase::DXTK_ImGui::Reset()
{
    if (m_isInitialized)
    {
        m_isInitialized = false;

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
}

// �X�V����
void Imase::DXTK_ImGui::Update()
{
    //  �V�t���[���̊J�n
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    //  �f���E�B���h�E�̕`��
    ImGui::ShowDemoWindow();
}

// �`�揈��
void Imase::DXTK_ImGui::Render()
{
    //  ImGui�̕`�揈��
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

// ���b�Z�[�W����
bool Imase::DXTK_ImGui::ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    //  ImGui�̃��b�Z�[�W����
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) return true;

    return false;
}
