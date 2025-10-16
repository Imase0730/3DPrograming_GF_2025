//-------------------------------------------------------------------------------------
// File: DirectXTK_ImGui.h
//
// ImGui�̏������܂Ƃ߂��N���X
//
// Date: 2025.10.14
// Author: Hideyasu Imase
// 
// �g�ݍ��ݕ��F
//	�P�Epch.h�Ɂy DirectXTK_ImGui.h �z���C���N���[�h����B
//	�Q�EMain.cpp��WndProc�֐��̐擪�Ɂy DXTK_ImGui::ProcessMessage(hWnd, message, wParam, lParam); �z��ǉ�����B
//	�R�EMain.cpp��wWinMain�֐���g_game.reset()�̒��O�Ɂy DXTK_ImGui::Reset(); �z��ǉ�����B
//	�S�EGame.cpp��CreateDeviceDependentResources�֐��Ɂy DXTK_ImGui::Initialize(m_deviceResources->GetWindow(), m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext()); �z��ǉ�����B
//	�T�EGame.cpp��Update�֐��Ɂy DXTK_ImGui::Update(); �z��ǉ�����B
//	�U�EGame.cpp��Render�֐���m_deviceResources->PIXEndEvent();�̒��O�Ɂy DXTK_ImGui::Render(); �z��ǉ�����B
//
// �g�p���@�̓f���E�C���h�E���Q�l�ɂ��Ă��������B
//
//	<<< �L�q�� >>
//	ImGui::Begin("TestWindow");
// 
//	// float�^�R����͂��鍀�ڂ�ǉ� 
//	static float v[3] = { 0.1f, 0.2f, 0.3f };
//	ImGui::DragFloat3("input float3", v, 0.1f);
// 
//	// ImGui�̃E�C���h�E�Ƀt�H�[�J�X�������Ă��鎞�̓J�����̑���𐧌�����j
//	m_debugCamera->Update(!ImGui::IsWindowFocused());
// 
//	ImGui::End();
//
//--------------------------------------------------------------------------------------
#pragma once

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"

namespace Imase
{
	class DXTK_ImGui
	{
	private:

		// �C���X�^���X�ւ̃|�C���^
		static DXTK_ImGui* m_instance;

		// �R���X�g���N�^
		DXTK_ImGui(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context);

		// �R�s�[�������֎~
		DXTK_ImGui(const DXTK_ImGui&) = delete;
		DXTK_ImGui& operator=(const DXTK_ImGui&) = delete;

	public:

		// �f�X�g���N�^
		virtual ~DXTK_ImGui();

		// �������֐�
		static void Initialize(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* context);

		// ���Z�b�g�֐�
		static void Reset();

		// �X�V����
		static void Update();

		// �`�揈��
		static void Render();

		// ���b�Z�[�W����
		static bool ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}