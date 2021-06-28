#pragma once
#include "Memory.hpp"


namespace rage
{
	struct scrNativeCallContext;
	struct scrNativeRegistration;
}

namespace Big
{
	class GameVariables
	{
	public:
		explicit GameVariables();
		~GameVariables() noexcept = default;
		GameVariables(GameVariables const&) = delete;
		GameVariables(GameVariables&&) = delete;
		GameVariables& operator=(GameVariables const&) = delete;
		GameVariables& operator=(GameVariables&&) = delete;

		void PostInit();

		std::uint32_t* m_GameState;
		std::uint32_t* m_FrameCount;
		uint64_t m_WorldPtr;
		PVOID m_ModelSpawnBypass;
		PVOID m_NativeReturn;
		PVOID m_IncrementStatEvent;
		

		HWND m_GameWindow;
		IDXGISwapChain* m_Swapchain;
		rage::scrNativeRegistration** m_NativeRegistrations;
		std::uint64_t** m_GlobalBase;
		char* m_GameBuild;
	};

	class GameFunctions
	{
	public:
		explicit GameFunctions();
		~GameFunctions() noexcept = default;
		GameFunctions(GameFunctions const&) = delete;
		GameFunctions(GameFunctions&&) = delete;
		GameFunctions& operator=(GameFunctions const&) = delete;
		GameFunctions& operator=(GameFunctions&&) = delete;

		using IsDlcPresent = bool(std::uint32_t hash);
		IsDlcPresent* m_IsDlcPresent;

		using WndProc = LRESULT(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		WndProc* m_WndProc;

		using GetEventData = bool(std::int32_t eventGroup, std::int32_t eventIndex, std::int64_t* args, std::uint32_t argCount);
		GetEventData* m_GetEventData;

		using GetLabelText = const char*(void* unk, const char* label);
		GetLabelText* m_GetLabelText;

		using fpFileRegister = uint32_t(*)(int* p1, const char* p2, bool p3, const char* p4, bool p5);
		fpFileRegister m_FileRegister;

		decltype(auto) Load_Ytd(int* unk0, const char* unk1, bool unk2, const char* unk3, bool unk4) const { return m_FileRegister(unk0, unk1, unk2, unk3, unk4); }

		using SessionWeather = void(int a1, int weather_id, int a3, __int64 a4);
		SessionWeather* m_SessionWeather;
	//Usage Example g_GameFunctions->m_SessionWeather(1, WEATHER_ID, 76, 0);

		using fix_vectors_t = void(*)(rage::scrNativeCallContext*);
		fix_vectors_t m_FixVectors{};

		using TriggerScriptEvent = void(int eventGroup, std::uint64_t* argstruct, int argCount, int bitflag);
		TriggerScriptEvent* m_TriggerScriptEvent;

		using NetworkIncreasementStatEvent = BOOL(__fastcall*)(uint64_t net_event_struct, int64_t sender, int64_t a3);
		NetworkIncreasementStatEvent m_NetworkIncresementStatEvent{};

		using ClearPedTasksEvent = void(__fastcall*)(int32_t ped);
		ClearPedTasksEvent m_NetworkClearTasksEvent{};

		using NetworkSessionKickPlayerEvent = void(__fastcall*)(int32_t ped);
		NetworkSessionKickPlayerEvent m_NetworkKickPlayerEvent{};

		using BlockSigScan = void(__fastcall*)();
		BlockSigScan m_BlockSigScan{};
	
	};

	inline std::unique_ptr<GameVariables> g_GameVariables;
	inline std::unique_ptr<GameFunctions> g_GameFunctions;	
}
