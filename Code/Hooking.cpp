#include "Hooking.hpp"
#include "Game.hpp"
#include "ScriptManager.hpp"
#include "CustomText.hpp"
#include "Util.hpp"
#include "D3DRenderer.hpp"
#include "UI/UIManager.hpp"
#include <MinHook/MinHook.h>
#include "Natives.hpp"
#include "MainScript.hpp"

#ifdef BIGBASE_DEBUG
#  pragma comment(lib, "MinHook-Debug.lib")
#else
#  pragma comment(lib, "MinHook-Release.lib")
#endif

namespace Big
{
	namespace
	{
		std::uint32_t g_HookFrameCount{};
	}

	bool Hooks::IsDlcPresent(std::uint32_t hash)
	{
		if (g_Running && g_HookFrameCount != *g_GameVariables->m_FrameCount)
		{
			g_HookFrameCount = *g_GameVariables->m_FrameCount;
			g_ScriptManager->OnGameTick();
		}

		return static_cast<decltype(&IsDlcPresent)>(g_Hooking->m_OriginalIsDlcPresent)(hash);
	}

	const char* Hooks::GetLabelText(void* unk, const char* label)
	{
		if (g_Running)
			if (auto text = g_CustomText->GetText(Joaat(label)))
				return text;

		return static_cast<decltype(&GetLabelText)>(g_Hooking->m_OriginalGetLabelText)(unk, label);
	}

	bool Hooks::GetEventData(std::int32_t eventGroup, std::int32_t eventIndex, std::int64_t* args, std::uint32_t argCount)
	{
		if (1352706024 == args[0]) //kick protection
		{
			return true;
		}

		auto result = static_cast<decltype(&GetEventData)>(g_Hooking->m_OriginalGetEventData)(eventGroup, eventIndex, args, argCount);
		
		if (result && g_LogScriptEvents)
		{
			g_Logger->Info("Script event group: %i", eventGroup);
			g_Logger->Info("Script event index: %i", eventIndex);
			g_Logger->Info("Script event argcount: %i", argCount);
			for (std::uint32_t i = 0; i < argCount; ++i)
				g_Logger->Info("Script event args[%u] : %" PRIi64, i, args[i]);
		}

		return result;
	}

	LRESULT Hooks::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		g_D3DRenderer->WndProc(hWnd, msg, wParam, lParam);
		return static_cast<decltype(&WndProc)>(g_Hooking->m_OriginalWndProc)(hWnd, msg, wParam, lParam);
	}

	HRESULT Hooks::Present(IDXGISwapChain* dis, UINT syncInterval, UINT flags)
	{
		if (g_Running)
		{
			g_D3DRenderer->BeginFrame();
			g_ScriptManager->OnD3DTick();
			g_D3DRenderer->EndFrame();
		}

		return g_Hooking->m_D3DHook.GetOriginal<decltype(&Present)>(PresentIndex)(dis, syncInterval, flags);
	}

	HRESULT Hooks::ResizeBuffers(IDXGISwapChain* dis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags)
	{
		if (g_Running)
		{
			g_D3DRenderer->PreResize();
			auto hr = g_Hooking->m_D3DHook.GetOriginal<decltype(&ResizeBuffers)>(ResizeBuffersIndex)(dis, bufferCount, width, height, newFormat, swapChainFlags);
			if (SUCCEEDED(hr))
			{
				g_D3DRenderer->PostResize();
			}

			return hr;
		}

		return g_Hooking->m_D3DHook.GetOriginal<decltype(&ResizeBuffers)>(ResizeBuffersIndex)(dis, bufferCount, width, height, newFormat, swapChainFlags);
	}

	uint32_t* Hooks::FileRegister(int* p1, const char* p2, bool p3, const char* p4, bool p5)
	{
		return static_cast<decltype(&FileRegister)>(g_Hooking->m_OriginalFileRegister)(p1, p2, p3, p4, p5);
	}

	bool Hooks::IncrementStatEvent(uint64_t net_event_struct, int64_t sender, int64_t unk)
	{
		const auto hash = *reinterpret_cast<DWORD*>(net_event_struct + 0x30);
		auto sender_id = *reinterpret_cast<std::int8_t*>(sender + 0x2D);
		auto sender_name = PLAYER::GET_PLAYER_NAME(sender_id);

		switch (hash)
		{
		case 0xCBFD04A4:
		case 0x0E7072CD:
		case 0x9F79BA0B:
		case 0x762F9994:
		case 0xB722D6C0:
			
			if (MainScript::m_ReportBlock)
			{
				g_Logger->Info("Blocked report from %", sender_name);

				return true;
			}
		default:
			break;
		}

		return static_cast<decltype(&IncrementStatEvent)>(g_Hooking->m_IncrementStatEventHook)(net_event_struct, sender, unk);
	}

	bool Hooks::ClearTasksEvent(__int64 thisptr, __int64 sender, __int64 reciever)
	{
		auto sender_id = *reinterpret_cast<std::int8_t*>(sender + 0x2D);
		auto sender_name = PLAYER::GET_PLAYER_NAME(sender_id);

		if (MainScript::m_FreezeEvent)
		{
			g_Logger->Info("Blocked freeze from %s", sender_name);
			return true;
		}
	}

	bool Hooks::KickPlayerEvent(__int64 thisptr, datBitBuffer* buffer, __int64 sender, __int64 receiver)
	{
		auto sender_id = *reinterpret_cast<std::int8_t*>(sender + 0x2D);
		auto sender_name = PLAYER::GET_PLAYER_NAME(sender_id);

		if (MainScript::m_KickEvent)
		{
			g_Logger->Info("Blocked host kick from %s", sender_name);
			return true;
		}
	}

	bool Hooks::BlockSigScan()
	{
		return false;
	}

	Hooking::Hooking():
		m_D3DHook(g_GameVariables->m_Swapchain, 18),
		//m_ObjectBypass(Signature("74 48 E8 ? ? ? ? 48 8B 48 48").Scan().Add(3).Rip().Add(30).As<char*>())
		m_BetterObjectBypass(Signature("48 8B C8 FF 52 30 84 C0 74 05 48").Scan().Add(8).As<decltype(m_BetterObjectBypass)>())  
	{
		std::copy_n(m_BetterObjectBypass, 2, m_OriginalBetterObjectBypass);

		MH_Initialize();
		g_Logger->Info("STRATOS || Starting Hooking");
		MH_CreateHook(g_GameFunctions->m_IsDlcPresent, &Hooks::IsDlcPresent, &m_OriginalIsDlcPresent);
		g_Logger->Info("STRATOS || Hooking IDP");
		g_Logger->Info("IDP Hooked");
		MH_CreateHook(g_GameFunctions->m_GetLabelText, &Hooks::GetLabelText, &m_OriginalGetLabelText);
		g_Logger->Info("STRATOS || Hooking GLT");
		g_Logger->Info("GLT Hooked");
		MH_CreateHook(g_GameFunctions->m_GetEventData, &Hooks::GetEventData, &m_OriginalGetEventData);
		g_Logger->Info("STRATOS || Hooking GED");
		g_Logger->Info("GED Hooked");
		MH_CreateHook(g_GameFunctions->m_WndProc, &Hooks::WndProc, &m_OriginalWndProc);
		g_Logger->Info("STRATOS || Hooking WPC");
		g_Logger->Info("WPC Hooked");
		MH_CreateHook(g_GameFunctions->m_FileRegister, &Hooks::FileRegister, &m_OriginalFileRegister);
		g_Logger->Info("STRATOS || Hooking OFR");
		g_Logger->Info("OFR Hooked");
		MH_CreateHook(g_GameFunctions->m_NetworkIncresementStatEvent, &Hooks::IncrementStatEvent, &m_IncrementStatEventHook);
		g_Logger->Info("STRATOS || Hooking NISE");
		g_Logger->Info("NISE Hooked");
		MH_CreateHook(g_GameFunctions->m_NetworkClearTasksEvent, &Hooks::ClearTasksEvent, &m_ClearTasksEventHook);
		g_Logger->Info("STRATOS || Hooking NCTE");
		g_Logger->Info("NCTE Hooked");
		MH_CreateHook(g_GameFunctions->m_NetworkKickPlayerEvent, &Hooks::KickPlayerEvent, &m_KickPlayerEventHook);
		g_Logger->Info("STRATOS || Hooking NKP");
		g_Logger->Info("NKP Hooked");
		MH_CreateHook(g_GameFunctions->m_BlockSigScan, &Hooks::BlockSigScan, &m_BlockSigScanEvent);
		g_Logger->Info("STRATOS || Hooking BSS");
		g_Logger->Info("BSS Hooked");

		m_D3DHook.Hook(&Hooks::Present, Hooks::PresentIndex);
		g_Logger->Info("STRATOS || Hooking PIX");
		g_Logger->Info("PIX Hooked");
		m_D3DHook.Hook(&Hooks::ResizeBuffers, Hooks::ResizeBuffersIndex);
		g_Logger->Info("STRATOS || Hooking RBI");
		g_Logger->Info("RBI Hooked");
	}

	Hooking::~Hooking() noexcept
	{
		MH_RemoveHook(g_GameFunctions->m_NetworkKickPlayerEvent);
		g_Logger->Info("NKP Unhooked");
		MH_RemoveHook(g_GameFunctions->m_NetworkClearTasksEvent);
		g_Logger->Info("NCTE Unhooked");
		MH_RemoveHook(g_GameFunctions->m_NetworkIncresementStatEvent);
		g_Logger->Info("NISE Unhooked");
		MH_RemoveHook(g_GameFunctions->m_WndProc);
		g_Logger->Info("WPC Unhooked");
		MH_RemoveHook(g_GameFunctions->m_GetEventData);
		g_Logger->Info("GED Unhooked");
		MH_RemoveHook(g_GameFunctions->m_GetLabelText);
		g_Logger->Info("GLT Unhooked");
		MH_RemoveHook(g_GameFunctions->m_IsDlcPresent);
		g_Logger->Info("IDP Unhooked");
		MH_RemoveHook(g_GameFunctions->m_FileRegister);
		g_Logger->Info("OFR Unhooked");
		MH_RemoveHook(g_GameFunctions->m_BlockSigScan);
		g_Logger->Info("BSS Unhooked");
		g_Logger->Info("STRATOS || Unhooked Successfully");
		MH_Uninitialize();
	}

	void Hooking::Hook()
	{
		std::fill_n(m_BetterObjectBypass, 2, '\x90');
		m_D3DHook.Enable();
		MH_EnableHook(MH_ALL_HOOKS);
	}

	void Hooking::Unhook()
	{
		std::copy_n(m_OriginalBetterObjectBypass, 2, m_BetterObjectBypass);
		m_D3DHook.Disable();
		MH_DisableHook(MH_ALL_HOOKS);
	}
}
