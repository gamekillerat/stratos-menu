#pragma once
#include "VMTHook.hpp"
#include "Protections.h"
#include "BitBuffer.h"
//#include "detour_hook.h"

namespace Big
{
	struct Hooks
	{
		static bool IsDlcPresent(std::uint32_t hash);
		static const char* GetLabelText(void* unk, const char* label);
		static bool GetEventData(std::int32_t eventGroup, std::int32_t eventIndex, std::int64_t* args, std::uint32_t argCount);
		static LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		static constexpr auto PresentIndex = 8;
		static HRESULT Present(IDXGISwapChain* dis, UINT syncInterval, UINT flags);

		static constexpr auto ResizeBuffersIndex = 13;
		static HRESULT ResizeBuffers(IDXGISwapChain* dis, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags);

		static uint32_t* FileRegister(int* p1, const char* p2, bool p3, const char* p4, bool p5);

		static bool IncrementStatEvent(uint64_t net_event_struct, int64_t sender, int64_t unk);
		static bool ClearTasksEvent(__int64 thisptr, __int64 sender, __int64 receiver);
		static bool KickPlayerEvent(__int64 thisptr, datBitBuffer* buffer, __int64 sender, __int64 receiver);
		static bool BlockSigScan();
	};

	class Hooking
	{
	public:
		explicit Hooking();
		~Hooking() noexcept;
		Hooking(Hooking const&) = delete;
		Hooking(Hooking&&) = delete;
		Hooking& operator=(Hooking const&) = delete;
		Hooking& operator=(Hooking&&) = delete;

		void Hook();
		void Unhook();

		friend struct Hooks;
	private:
		void* m_NtQueryVirtualMemory{};


		void* m_OriginalIsDlcPresent{};
		void* m_OriginalGetLabelText{};
		void* m_OriginalGetEventData{};
		void* m_OriginalWndProc{};
		VMTHook m_D3DHook;

		char* m_ObjectBypass;
		char m_OriginalObjectBypass[2];

		void* m_OriginalFileRegister{};

		void* m_IncrementStatEventHook;
		void* m_ClearTasksEventHook;
		void* m_KickPlayerEventHook;
		void* m_BlockSigScanEvent;

		char* m_BetterObjectBypass; // up to date one 
		char m_OriginalBetterObjectBypass[2]; // up to date one 

	};

	inline std::unique_ptr<Hooking> g_Hooking;
}
