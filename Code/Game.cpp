#include "Game.hpp"
#include "Invoker.hpp"
#include "Logger.hpp"



namespace Big
{
	GameVariables::GameVariables():
		m_GameState(Signature("48 85 C9 74 4B 83 3D").Scan().Add(7).Rip().As<decltype(m_GameState)>()),
		m_FrameCount(Signature("F3 0F 10 0D ? ? ? ? 44 89 6B 08").Scan().Add(4).Rip().Sub(8).As<decltype(m_GameState)>()),
		m_GameWindow(FindWindowA("grcWindow", "Grand Theft Auto V")),
		m_Swapchain(Signature("48 8B 0D ? ? ? ? 48 8D 55 A0 48 8B 01").Scan().Add(3).Rip().As<decltype(m_Swapchain)&>()),
		m_NativeRegistrations(Signature("48 83 EC 20 48 8D 0D ? ? ? ? E8 ? ? ? ? 0F B7 15 ? ? ? ? 33 FF").Scan().Add(7).Rip().As<decltype(m_NativeRegistrations)>()),
		m_GlobalBase(Signature("4C 8D 4D 08 48 8D 15 ? ? ? ? 4C 8B C0").Scan().Add(7).Rip().As<decltype(m_GlobalBase)>()),
		m_ModelSpawnBypass(Signature("48 8B C8 FF 52 30 84 C0 74 05 48").Scan().Add(8).As<decltype(m_ModelSpawnBypass)>()),
		m_WorldPtr(Signature("48 8B 05 ? ? ? ? 45 ? ? ? ? 48 8B 48 08 48 85 C9 74 07").Scan().Add(3).Rip().As<decltype(m_WorldPtr)>()),
		m_NativeReturn(Signature("FF E3").Scan().Add(0).As<decltype(m_NativeReturn)>()),
		m_IncrementStatEvent(Signature("48 89 5C 24 ? 48 89 74 24 ? 55 57 41 55 41 56 41 57 48 8B EC 48 83 EC 60 8B 79 30").Scan().As<decltype(m_IncrementStatEvent)>())
	{
		auto sig = Signature("48 83 EC 60 48 8D 0D ? ? ? ? E8").Scan().Sub(17);
		m_GameBuild = sig.Add(265 + 3).Rip().As<decltype(m_GameBuild)>();

		/*const int EVENT_COUNT = 78;
		static std::vector<void*> EventPtr;
		static char EventRestore[EVENT_COUNT] = {};
		auto p_eventHook = Signature("48 83 EC 28 E8 ? ? ? ? 48 8B 0D ? ? ? ? 4C 8D 0D ? ? ? ? 4C 8D 05 ? ? ? ? BA 03");*/
		

			
		






		




	}

	void GameVariables::PostInit()
	{
	}

	GameFunctions::GameFunctions():
		m_IsDlcPresent(Signature("84 C0 74 0D 8D 53 0C").Scan().Sub(63).As<decltype(m_IsDlcPresent)>()),
		m_WndProc(Signature("48 8B 0D ? ? ? ? 48 8D 55 EF FF 15").Scan().Sub(853).As<decltype(m_WndProc)>()),
		m_GetEventData(Signature("48 85 C0 74 14 4C 8B 10").Scan().Sub(28).As<decltype(m_GetEventData)>()),
		m_GetLabelText(Signature("75 ? E8 ? ? ? ? 8B 0D ? ? ? ? 65 48 8B 04 25 ? ? ? ? BA ? ? ? ? 48 8B 04 C8 8B 0C 02 D1 E9").Scan().Sub(19).As<decltype(m_GetLabelText)>()),
		m_FileRegister(Signature("48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 54 41 56 41 57 48 83 EC 50 48 8B EA 4C 8B FA 48 8B D9 4D 85 C9").Scan().As<decltype(m_FileRegister)>()),
		m_SessionWeather(Signature("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 48 83 EC 30 40 8A E9").Scan().As<decltype(m_SessionWeather)>()),
		m_FixVectors(Signature("83 79 18 00 48 8B D1 74 4A FF 4A 18 48 63 4A 18 48 8D 41 04 48 8B 4C CA").Scan().As<decltype(m_FixVectors)>()),
		m_TriggerScriptEvent(Signature("48 8B C4 48 89 58 08 48 89 68 10 48 89 70 18 48 89 78 20 41 56 48 81 EC ? ? ? ? 45 8B F0 41 8B F9").Scan().As<decltype(m_TriggerScriptEvent)>()),
		m_NetworkIncresementStatEvent(Signature(encryptDecrypt("59 98 4D 35 ? 59 98 65 35 ? 44 46 50 44 50 47 50 46 59 9A FD 59 92 FD 71 9A 68 21").c_str()).Scan().As<decltype(m_NetworkIncresementStatEvent)>()),
		m_NetworkClearTasksEvent(Signature("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 30 0F B7 51 30").Scan().As<decltype(m_NetworkClearTasksEvent)>()),
		m_NetworkKickPlayerEvent(Signature("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8D 71 30").Scan().As<decltype(m_NetworkKickPlayerEvent)>()),
		m_BlockSigScan(Signature("80 3D ? ? ? ? ? 0F 85 ? ? ? ? 48 8B 05 ? ? ? ? 48 8B 08").Scan().As<decltype(m_BlockSigScan)>())
	{//m_TriggerScriptEvent(Signature("85 C7 75 05").Scan().Sub(86).As<decltype(m_TriggerScriptEvent)>())
	}

}
