#pragma once
#include "Common.hpp"
#include "MainScript.hpp"
#include "Natives.hpp"

namespace Big::Functions
{
	namespace Memory
	{
		uintptr_t get_multilayer_pointer(uintptr_t base_address, std::vector<DWORD> offsets);

		template <typename T>
		T get_value(std::vector<DWORD> offsets);

		template <typename T>
		void set_value(std::vector<DWORD> offsets, T value);
	}

	namespace Recovery
	{
		//Int's
		inline static int Face = 0;
		inline static int Glasses = 0;
		inline static int Ears = 0;
		inline static int Torso = 0;
		inline static int Torso2 = 0;
		inline static int Legs = 0;
		inline static int Hands = 0;
		inline static int Watches = 0;
		inline static int Special1 = 0;
		inline static int Special2 = 0;
		inline static int Special3 = 0;
		inline static int Texture = 0;

		inline static int FaceTex = 0;
		inline static int GlassesTex = 0;
		inline static int EarsTex = 0;
		inline static int TorsoTex = 0;
		inline static int Torso2Tex = 0;
		inline static int LegsTex = 0;
		inline static int HandsTex = 0;
		inline static int WatchesTex = 0;
		inline static int Special1Tex = 0;
		inline static int Special2Tex = 0;
		inline static int Special3Tex = 0;
		inline static int sup;
		inline static int num;

		bool cstrcmp(const char* s1, const char* s2);
		int NumberKeyboard();
		void changeClothes(const char* family, int model, int texture);
	}

	void writeFlameFlag(uint32_t Flag);
	void removeFlameFlag(uint32_t Flag);
	//Self
	void GodMode(bool toggle);
	void NeverWanted(bool toggle);
	void Invisible(bool toggle);
	void TinyPlayer(bool toggle);
	void NoRagdoll(bool toggle);
	void FastSwim(bool toggle);
	void SuperJump(bool toggle);
	void FlamingFists(bool toggle);
	void SlowMode(bool toggle);
	void DragonBreath(bool toggle);
	void NoClip(bool toggle);
	void NPCIgnore(bool toggle);
	void Freeze(bool toggle);
	void NoCollision(bool toggle);
	void Ragdoll(bool toggle);
	//Weapon Options
	void FlamingAmmo(bool toggle);
	void ExplosiveAmmo(bool toggle);
	void UnlimitedAmmo(bool toggle);
	void RapidFire(bool toggle);
	void TeleportGun(bool toggle);
	void OneShot(bool toggle);
	void DeleteGun(bool toggle);
	void AirStrikeGun(bool toggle);
	void MolotovGun(bool toggle);
	void FlareGun(bool toggle);
	void SmokeGun(bool toggle);
	//void CartoonGun(bool toggle);
//Vehicle Options
	void VehicleGodMode(bool toggle);
	void HornBoost(bool toggle);
	void VehicleJump(bool toggle);
	void AutoFix(bool toggle);
	void DriftMode(bool toggle);
	void SeatBelt(bool toggle);
	void ZeroGravity(bool toggle);
	void BoostAndJump(bool toggle);
	void VehParachute(bool toggle);
	void HeadlightInten(bool toggle);
	//World
	void OffRadar(bool toggle);
	void Snowing(bool toggle);
	void AntiAFK(bool toggle);
	void OrbitalCooldown(bool toggle);
	void RevealPlayers(bool toggle);
	//Misc
	void DisplayFPS(bool toggle);
	void MobileRadio(bool toggle);
	void FlashRun(bool toggle);
	void ForceField(bool toggle);
	void ThermalVision(bool toggle);
	void NightVision(bool toggle);
	//Network
	void TeleportTo(int player);
	void FreezePlayer(Player player, bool enable);
	void RPDrop(uint32_t Play, bool toggle);
	void RPGun(bool toggle);
	void Spectate(uint32_t Play, bool toggle);
	//Protection
	void RAdmin(bool toggle);
	//Lua
	void Lua();
}

