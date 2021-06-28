#include "Common.hpp"
#include "Functions.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "D3DRenderer.hpp"
#include "MainScript.hpp"
#include "Offsets.h"
#include "Game.hpp"
#include "Memory.hpp"
#include "ScriptCallback.hpp"
#include "Lists.hpp"
#include "Timer.hpp"
#include "Translation.hpp"
#include "CustomText.hpp"
#include "Natives.hpp"
#include "UI/UIManager.hpp"
#include "Timer.hpp"
#include <timeapi.h>

namespace Big::Functions
{
	namespace Recovery
	{
		bool cstrcmp(const char* s1, const char* s2)
		{
			while (*s1 && (*s1 == *s2))
				s1++, s2++;
			if (*(const unsigned char*)s1 - *(const unsigned char*)s2 == 0)
				return true;
			else
				return false;
		}

		int NumberKeyboard()
		{
			MISC::DISPLAY_ONSCREEN_KEYBOARD(1, "", "", "", "", "", "", 10);
			g_CallbackScript->AddCallback<KeyBoardCallBack>("", 10, [] {
				while (MISC::UPDATE_ONSCREEN_KEYBOARD() == 0) std::this_thread::yield();
				if (!MISC::GET_ONSCREEN_KEYBOARD_RESULT()) return;
				{
					num = atof(MISC::GET_ONSCREEN_KEYBOARD_RESULT());
				}
				});
			return num;
		}

		void changeClothes(const char* family, int model, int texture)
		{
			if (cstrcmp(family, "HATS") || cstrcmp(family, "GLASSES") || cstrcmp(family, "EARS"))
			{
				if (cstrcmp(family, "HATS"))
					sup = 0;
				else if (cstrcmp(family, "GLASSES"))
					sup = 1;
				else if (cstrcmp(family, "EARS"))
					sup = 2;
				PED::SET_PED_PROP_INDEX(PLAYER::PLAYER_PED_ID(), sup, model - 1, texture, 1);
			}
			else
			{
				if (cstrcmp(family, "FACE"))
					sup = 0;
				else if (cstrcmp(family, "MASK"))
					sup = 1;
				else if (cstrcmp(family, "HAIR"))
					sup = 2;
				else if (cstrcmp(family, "TORSO"))
					sup = 3;
				else if (cstrcmp(family, "LEGS"))
					sup = 4;
				else if (cstrcmp(family, "HANDS"))
					sup = 5;
				else if (cstrcmp(family, "SHOES"))
					sup = 6;
				else if (cstrcmp(family, "SPECIAL1"))
					sup = 7;
				else if (cstrcmp(family, "SPECIAL2"))
					sup = 8;
				else if (cstrcmp(family, "SPECIAL3"))
					sup = 9;
				else if (cstrcmp(family, "TEXTURE"))
					sup = 10;
				else if (cstrcmp(family, "TORSO2"))
					sup = 11;
				PED::SET_PED_COMPONENT_VARIATION(PLAYER::PLAYER_PED_ID(), sup, model, texture, 0);
			}
		}
	}

	bool is_ped_shooting(Ped ped) {
		NativeVector3 coords = ENTITY::GET_ENTITY_COORDS(ped, 1);
		return PED::IS_PED_SHOOTING_IN_AREA(ped, coords.x, coords.y, coords.z, coords.x, coords.y, coords.z, true, true);
	}

	float degToRad(float degs)
	{
		return degs * 3.141592653589793f / 180.f;
	}

	double DegreeToRadian(double n) {
		return n * 0.017453292519943295;
	}

	NativeVector3 RotationToDirection(NativeVector3 rot)
	{
		double num = DegreeToRadian(rot.z);
		double num2 = DegreeToRadian(rot.x);
		double val = cos(num2);
		double num3 = abs(val);
		rot.x = (float)(-(float)sin(num) * num3);
		rot.y = (float)(cos(num) * num3);
		rot.z = (float)sin(num2);
		return rot;

	}

	NativeVector3 addVectors(NativeVector3 vector, NativeVector3 vector2)
	{
		vector.x += vector2.x;
		vector.y += vector2.y;
		vector.z += vector2.z;
		/*vector.paddingX += vector2.paddingX;
		vector.paddingY += vector2.paddingY;
		vector.paddingZ += vector2.paddingZ;*/
		return vector;
	}

	NativeVector3 multiplyVector(NativeVector3 vector, float inc) {
		vector.x *= inc;
		vector.y *= inc;
		vector.z *= inc;
		/*vector._paddingx *= inc;
		vector._paddingy *= inc;
		vector._paddingz *= inc;*/
		return vector;
	}

	namespace Memory
	{
		uintptr_t get_multilayer_pointer(uintptr_t base_address, std::vector<DWORD> offsets)
		{
			uintptr_t ptr = *(uintptr_t*)(base_address);
			if (!ptr) {

				return NULL;
			}
			auto level = offsets.size();

			for (auto i = 0; i < level; i++)
			{
				if (i == level - 1)
				{
					ptr += offsets[i];
					if (!ptr) return NULL;
					return ptr;
				}
				else
				{
					ptr = *(uint64_t*)(ptr + offsets[i]);
					if (!ptr) return NULL;
				}
			}

			return ptr;
		}

		template <typename T>
		T get_value(std::vector<DWORD> offsets) {

			uintptr_t Addr = get_multilayer_pointer((uintptr_t)g_GameVariables->m_WorldPtr, offsets);
			if (Addr == NULL) {
				return NULL;
			}

			return *((T*)Addr);
		}

		template <typename T>
		void set_value(std::vector<DWORD> offsets, T value) {
			uintptr_t Addr = get_multilayer_pointer((uintptr_t)g_GameVariables->m_WorldPtr, offsets);
			if (Addr == NULL) {
				return;
			}

			*reinterpret_cast<T*>(Addr) = value;
		}
	}

	enum eFrameFlags : std::uint32_t
	{
		eFrameFlagExplosiveAmmo = 1 << 11,
		eFrameFlagFireAmmo = 1 << 12,
		eFrameFlagExplosiveMelee = 1 << 13,
		eFrameFlagSuperJump = 1 << 14,
	};

	void writeFlameFlag(uint32_t Flag)
	{
		uint32_t CurrentValue = Memory::get_value<uint32_t>({ 0x8, 0x10C8, 0x1F8 });
		Memory::set_value<uint32_t>({ 0x8, 0x10C8, 0x1F8 }, CurrentValue |= Flag);
	}

	void removeFlameFlag(uint32_t Flag)
	{
		uint32_t CurrentValue = Memory::get_value<uint32_t>({ 0x8, 0x10C8, 0x1F8 });
		Memory::set_value<uint32_t>({ 0x8, 0x10C8, 0x1F8 }, CurrentValue &= ~(1 << Flag));
	}

	int TimePD15;
	int DropDelay = 50;
	/////////////////////////////////////////////////////////////////////////////////////////////
	//Self
	/////////////////////////////////////////////////////////////////////////////////////////////
	void GodMode(bool toggle)
	{
		if (toggle)
		{
			Memory::set_value<bool>({ OFFSET_PLAYER_PED, OFFSET_ENTITY_GOD }, true);
		}
		else
		{
			Memory::set_value<bool>({ OFFSET_PLAYER_PED, OFFSET_ENTITY_GOD }, false);
		}
	}

	void NeverWanted(bool toggle)
	{
		if (toggle)
		{
			Memory::set_value<int32_t>({ OFFSET_PLAYER_PED, OFFSET_PLAYER_INFO, OFFSET_PLAYER_INFO_WANTED }, 0);
		}
	}

	void Invisible(bool toggle)
	{
		if (toggle)
		{
			ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), false, 0);
		}
		else
		{
			ENTITY::SET_ENTITY_VISIBLE(PLAYER::PLAYER_PED_ID(), true, 0);
		}
	}

	void TinyPlayer(bool toggle)
	{
		if (toggle)
		{
			PED::SET_PED_CONFIG_FLAG(PLAYER::PLAYER_PED_ID(), 223, true);
		}
		else
		{
			PED::SET_PED_CONFIG_FLAG(PLAYER::PLAYER_PED_ID(), 223, false);
		}
	}

	void NoRagdoll(bool toggle)
	{
		if (toggle)
		{
			PED::SET_PED_RAGDOLL_ON_COLLISION(PLAYER::PLAYER_PED_ID(), false);
			PED::SET_PED_CAN_RAGDOLL_FROM_PLAYER_IMPACT(PLAYER::PLAYER_PED_ID(), false);
			PED::SET_PED_CAN_RAGDOLL(PLAYER::PLAYER_PED_ID(), false);
		}
		else
		{
			NULL;
		}
	}

	void FastSwim(bool toggle)
	{
		if (toggle)
		{
			Memory::set_value<float>({ OFFSET_PLAYER_PED, OFFSET_PLAYER_INFO, OFFSET_PLAYER_INFO_SWIM_SPD }, 2.5f);
		}
		else
		{
			Memory::set_value<float>({ OFFSET_PLAYER_PED, OFFSET_PLAYER_INFO, OFFSET_PLAYER_INFO_SWIM_SPD }, 1.f);
		}
	}

	void SuperJump(bool toggle)
	{
		if (toggle)
		{
			writeFlameFlag(eFrameFlagSuperJump);
		}
		else
		{
			removeFlameFlag(eFrameFlagSuperJump);
		}
	}

	void FlamingFists(bool toggle)
	{
		if (toggle)
		{
			writeFlameFlag(eFrameFlagExplosiveMelee);
		}
		else
		{
			removeFlameFlag(eFrameFlagExplosiveMelee);
		}
	}

	void SlowMode(bool toggle)
	{
		if (toggle)
		{
			MISC::SET_TIME_SCALE(0.2f);
		}
		else
		{
			MISC::SET_TIME_SCALE(1.00f);
		}
	}

	void DragonBreath(bool toggle)
	{
		if (toggle)
		{
			float XPos = 0.02, YPos = 0.2, ZPos = 0.0, XOff = 90.0, YOff = -100.0, ZOff = 90.0;
			STREAMING::REQUEST_NAMED_PTFX_ASSET("core");
			GRAPHICS::_USE_PARTICLE_FX_ASSET_NEXT_CALL("core");
			int ptfx = GRAPHICS::_START_PARTICLE_FX_NON_LOOPED_ON_PED_BONE_2("ent_sht_flame", PLAYER::PLAYER_PED_ID(), XPos, YPos, ZPos, XOff, YOff, ZOff, 0x796e, 1, 1, 1, 1);
			STREAMING::REMOVE_PTFX_ASSET();
		}
	}

	void NoClip(bool toggle)
	{
		if (toggle)
		{
			Ped playerPed = PLAYER::PLAYER_PED_ID();
			NativeVector3 pos = ENTITY::GET_ENTITY_COORDS(playerPed, false);
			ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, pos.x, pos.y, pos.z, false, false, false);
			if (GetAsyncKeyState(0x57) || PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 268)) {
				float fivef = .5f;
				float heading = ENTITY::GET_ENTITY_HEADING(playerPed);
				float xVec = fivef * sin(degToRad(heading)) * -1.0f;
				float yVec = fivef * cos(degToRad(heading));
				ENTITY::SET_ENTITY_HEADING(playerPed, heading);

				pos.x -= xVec, pos.y -= yVec;
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, pos.x, pos.y, pos.z, false, false, false);
			}
			if (GetAsyncKeyState(0x53) || PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 269)) {
				float fivef = .5f;
				float heading = ENTITY::GET_ENTITY_HEADING(playerPed);
				float xVec = fivef * sin(degToRad(heading)) * -1.0f;
				float yVec = fivef * cos(degToRad(heading));
				ENTITY::SET_ENTITY_HEADING(playerPed, heading);

				pos.x += xVec, pos.y += yVec;
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, pos.x, pos.y, pos.z, false, false, false);
			}
			if (GetAsyncKeyState(0x41) || PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 266)) {
				float fivef = .5f;
				float heading = ENTITY::GET_ENTITY_HEADING(playerPed);
				ENTITY::SET_ENTITY_HEADING(playerPed, heading + 1.f);
			}
			if (GetAsyncKeyState(0x44) || PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 271)) {
				float fivef = .5f;
				float heading = ENTITY::GET_ENTITY_HEADING(playerPed);
				ENTITY::SET_ENTITY_HEADING(playerPed, heading - 1.f);
			}
			if (GetAsyncKeyState(VK_SHIFT) || PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 206)) {
				float heading = ENTITY::GET_ENTITY_HEADING(playerPed);
				ENTITY::SET_ENTITY_HEADING(playerPed, heading);

				pos.z -= 0.1;
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, pos.x, pos.y, pos.z, false, false, false);
			}
			if (GetAsyncKeyState(VK_SPACE) || PAD::IS_DISABLED_CONTROL_JUST_PRESSED(2, 205)) {
				float heading = ENTITY::GET_ENTITY_HEADING(playerPed);
				ENTITY::SET_ENTITY_HEADING(playerPed, heading);

				pos.z += 0.1;
				ENTITY::SET_ENTITY_COORDS_NO_OFFSET(playerPed, pos.x, pos.y, pos.z, false, false, false);
			}
		}
	}

	void NPCIgnore(bool toggle)
	{
		if (toggle)
		{
			Memory::set_value<DWORD>({ OFFSET_PLAYER_PED, OFFSET_PLAYER_INFO, OFFSET_PLAYER_INFO_NPC_IGNORE }, 0x450000);
			PED::REMOVE_PED_DEFENSIVE_AREA(PLAYER::PLAYER_PED_ID(), true);
		}
		else
		{
			PED::REMOVE_PED_DEFENSIVE_AREA(PLAYER::PLAYER_PED_ID(), false);
		}
	}

	void Freeze(bool toggle)
	{
		if (toggle)
		{
			BRAIN::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(PLAYER::PLAYER_ID()));
		}
		else
		{
			NULL;
		}
	}

	void NoCollision(bool toggle)
	{
		if (toggle)
		{
			ENTITY::_0x9EBC85ED0FFFE51C(PLAYER::PLAYER_PED_ID(), false, 1);
		}
		else
		{
			ENTITY::_0x9EBC85ED0FFFE51C(PLAYER::PLAYER_PED_ID(), true, 1);
		}
	}

	void Ragdoll(bool toggle)
	{
		if (toggle)
		{
			NativeVector3 get;
			get = ENTITY::GET_ENTITY_FORWARD_VECTOR(PLAYER::PLAYER_PED_ID());
			PED::SET_PED_TO_RAGDOLL_WITH_FALL(PLAYER::PLAYER_PED_ID(), 1500, 2000, 1, get.x, get.y, get.z, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
		}
		else
		{
			NULL;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	//World
	/////////////////////////////////////////////////////////////////////////////////////////
	void OffRadar(bool toggle)
	{
		if (toggle)
		{
			*ScriptGlobal::ScriptGlobal(2425869).Add(PLAYER::PLAYER_ID(), 443).Add(204).As<bool*>() = 1;
			*ScriptGlobal::ScriptGlobal(2440049).Add(70).As<int32_t*>() = NETWORK::GET_NETWORK_TIME();
		}
		else
		{
			*ScriptGlobal::ScriptGlobal(2425869).Add(PLAYER::PLAYER_ID(), 443).Add(204).As<bool*>() = 0;
			*ScriptGlobal::ScriptGlobal(2440049).Add(70).As<int32_t*>() = NETWORK::GET_NETWORK_TIME();
		}
	}

	void Snowing(bool toggle)
	{
		if (toggle)
		{
			*ScriptGlobal::ScriptGlobal(262145).Add(4723).As<bool*>() = 1;
		}
		else
		{
			*ScriptGlobal::ScriptGlobal(262145).Add(4723).As<bool*>() = 0;
		}
	}

	void AntiAFK(bool toggle)
	{
		if (toggle)
		{
			*ScriptGlobal::ScriptGlobal(262145).Add(87).As<int*>() = 99999999;
			*ScriptGlobal::ScriptGlobal(262145).Add(88).As<int*>() = 99999999;
			*ScriptGlobal::ScriptGlobal(262145).Add(89).As<int*>() = 99999999;
			*ScriptGlobal::ScriptGlobal(262145).Add(90).As<int*>() = 99999999;
		}
		else
		{
			*ScriptGlobal::ScriptGlobal(262145).Add(87).As<int*>() = 120000;
			*ScriptGlobal::ScriptGlobal(262145).Add(88).As<int*>() = 300000;
			*ScriptGlobal::ScriptGlobal(262145).Add(89).As<int*>() = 600000;
			*ScriptGlobal::ScriptGlobal(262145).Add(90).As<int*>() = 900000;
		}
	}

	void OrbitalCooldown(bool toggle)
	{
		if (toggle)
		{
			*ScriptGlobal::ScriptGlobal(262145).Add(22589).As<int*>() = 0;
		}
		else
		{
			*ScriptGlobal::ScriptGlobal(262145).Add(22589).As<int*>() = 2880000;
		}
	}

	void RevealPlayers(bool toggle)
	{
		if (toggle)
		{
			*ScriptGlobal::ScriptGlobal(2425869 + 1 + PLAYER::PLAYER_ID() * 443 + 207).As<int*>() = 1;
			*ScriptGlobal::ScriptGlobal(2440049).Add(71).As<int*>() = NETWORK::GET_NETWORK_TIME() + 99999999;
		}
		else
		{
			*ScriptGlobal::ScriptGlobal(2425869 + 1 + PLAYER::PLAYER_ID() * 443 + 207).As<int*>() = 0;
			*ScriptGlobal::ScriptGlobal(2440049).Add(71).As<int*>() = 0;
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////////
	//Misc
	/////////////////////////////////////////////////////////////////////////////////////////
	void DisplayFPS(bool toggle)
	{
		if (toggle)
		{
			float LastFrameTime = MISC::GET_FRAME_TIME();
			int getFPS = (1.0f / LastFrameTime);
			char FPStext[60];
			snprintf(FPStext, sizeof(FPStext), "FPS: %d", getFPS);
			g_UiManager->DrawLeftText(FPStext, 0.020f, 0.010f, 0.4f, UserInterface::Font::HouseScript, { 255, 255, 255, 255 }, true, false);
		}
	}

	void MobileRadio(bool toggle)
	{
		int station = AUDIO::GET_PLAYER_RADIO_STATION_INDEX();
		if (toggle)
		{
			AUDIO::SET_MOBILE_RADIO_ENABLED_DURING_GAMEPLAY(true);
			AUDIO::SET_MOBILE_PHONE_RADIO_STATE(1);
			AUDIO::SET_RADIO_TO_STATION_INDEX(station);
		}
		else
		{
			AUDIO::SET_MOBILE_RADIO_ENABLED_DURING_GAMEPLAY(false);
			AUDIO::SET_MOBILE_PHONE_RADIO_STATE(0);
		}
	}

	void FlashRun(bool toggle)
	{
		if (toggle)
		{
			float XOff = 90.0, YOff = -100.0, ZOff = 90.0;
			NativeVector3 pos = ENTITY::GET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), true);

			if (BRAIN::IS_PED_RUNNING(PLAYER::PLAYER_PED_ID()) || BRAIN::IS_PED_SPRINTING(PLAYER::PLAYER_PED_ID()))
			{
				AUDIO::PLAY_SOUND_FRONTEND(-1, "collect_part", "DLC_sum20_BB_Captured_Sounds", false);
				ENTITY::APPLY_FORCE_TO_ENTITY(PLAYER::PLAYER_PED_ID(), 1, 0.f, 1.5f, 0.f, 0.f, 0.f, 0.f, 1, true, true, true, false, true);
				GRAPHICS::_START_SCREEN_EFFECT("RaceTurbo", 0, 0);
				STREAMING::REQUEST_NAMED_PTFX_ASSET("core");
				GRAPHICS::_USE_PARTICLE_FX_ASSET_NEXT_CALL("core");
				GRAPHICS::START_PARTICLE_FX_LOOPED_ON_ENTITY("ped_parachute_canopy_trail", PLAYER::PLAYER_PED_ID(), pos.x, pos.y, pos.z, XOff, YOff, ZOff, 1, 1, 1, 1);
			}
			STREAMING::REMOVE_PTFX_ASSET();
		}
	}

	void ForceField(bool toggle)
	{
		if (toggle)
		{
			NativeVector3 coords = ENTITY::GET_ENTITY_COORDS(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(PLAYER::PLAYER_ID()), true);
			FIRE::ADD_EXPLOSION(coords.x, coords.y, coords.z, 0x08, 640.00, 0, 1, 0, 1);
			ENTITY::SET_ENTITY_INVINCIBLE(PLAYER::PLAYER_PED_ID(), true);
		}
	}

	void ThermalVision(bool toggle)
	{
		if (toggle)
		{
			GRAPHICS::SET_SEETHROUGH(true);
		}
		else
		{
			GRAPHICS::SET_SEETHROUGH(false);
		}
	}

	void NightVision(bool toggle)
	{
		if (toggle)
		{
			GRAPHICS::SET_NIGHTVISION(true);
		}
		else
		{
			GRAPHICS::SET_NIGHTVISION(false);
		}
	}

	/// //////////////////////////////////////////////////////////////////
	/// Weapon
	/// //////////////////////////////////////////////////////////////////
	void FlamingAmmo(bool toggle)
	{
		if (toggle)
		{
			writeFlameFlag(eFrameFlagFireAmmo);
		}
		else
		{
			removeFlameFlag(eFrameFlagFireAmmo);
		}
	}

	void ExplosiveAmmo(bool toggle)
	{
		if (toggle)
		{
			writeFlameFlag(eFrameFlagExplosiveAmmo);
		}
		else
		{
			removeFlameFlag(eFrameFlagExplosiveAmmo);
		}
	}

	void UnlimitedAmmo(bool toggle)
	{
		if (toggle)
		{
			WEAPON::SET_PED_INFINITE_AMMO_CLIP(PLAYER::PLAYER_PED_ID(), true);
		}
		else
		{
			WEAPON::SET_PED_INFINITE_AMMO_CLIP(PLAYER::PLAYER_PED_ID(), false);
		}
	}

	void RapidFire(bool toggle)
	{
		if (toggle)
		{
			Player playerPed = PLAYER::PLAYER_PED_ID();
			if (!PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 1)) {
				//PLAYER::DISABLE_PLAYER_FIRING(PLAYER::PLAYER_PED_ID(), 1);
				NativeVector3 gameplayCam = CAM::_GET_GAMEPLAY_CAM_COORDS();
				NativeVector3 gameplayCamRot = CAM::GET_GAMEPLAY_CAM_ROT(0);
				NativeVector3 gameplayCamDirection = RotationToDirection(gameplayCamRot);
				NativeVector3 startCoords = addVectors(gameplayCam, (multiplyVector(gameplayCamDirection, 1.0f)));
				NativeVector3 endCoords = addVectors(startCoords, multiplyVector(gameplayCamDirection, 500.0f));
				Hash weaponhash;
				WEAPON::GET_CURRENT_PED_WEAPON(playerPed, &weaponhash, 1);
				if (PAD::IS_CONTROL_PRESSED(2, 208) || (GetKeyState(VK_LBUTTON) & 0x8000)) {
					MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(startCoords.x, startCoords.y, startCoords.z, endCoords.x, endCoords.y, endCoords.z, 50, 1, weaponhash, playerPed, 1, 1, 0xbf800000);
				}
			}
		}
	}

	void TeleportGun(bool toggle)
	{
		if (toggle)
		{
			if (ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID()) && is_ped_shooting)
			{
				NativeVector3 teleportcoords;
				if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &teleportcoords))
				{
					ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), teleportcoords.x, teleportcoords.y, teleportcoords.z + 1, 0, 0, 0, 0);
				}
			}
		}
	}

	void OneShot(bool toggle)
	{
		if (toggle)
		{
			Memory::set_value<float>({ OFFSET_PLAYER_PED, OFFSET_WEAPON_MANAGER, OFFSET_WEAPON_CURRENT, OFFSET_WEAPON_BULLET_DMG }, 10000.f);
		}
		else
		{
			Memory::set_value<float>({ OFFSET_PLAYER_PED, OFFSET_WEAPON_MANAGER, OFFSET_WEAPON_CURRENT, OFFSET_WEAPON_BULLET_DMG }, 1.f);
		}
	}

	void DeleteGun(bool toggle)
	{
		if (toggle)
		{
			if (ENTITY::DOES_ENTITY_EXIST(PLAYER::PLAYER_PED_ID()) && is_ped_shooting(PLAYER::PLAYER_PED_ID()))
			{
				NativeVector3 deltetecoords;
				if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &deltetecoords))
				{
					MISC::CLEAR_AREA(deltetecoords.x, deltetecoords.y, deltetecoords.z, 2, 0, 0, 0, 0);
				}
			}
		}
	}

	void AirStrikeGun(bool toggle)
	{
		NativeVector3 ShootCoord;
		Player MyPlayer = PLAYER::PLAYER_PED_ID();
		if (toggle)
		{
			if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(MyPlayer, &ShootCoord))
			{
				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(ShootCoord.x, ShootCoord.y, ShootCoord.z + 5, ShootCoord.x, ShootCoord.y, ShootCoord.z, 100, 1, MISC::GET_HASH_KEY("WEAPON_AIRSTRIKE_ROCKET"), MyPlayer, 1, 1, 100);
				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(ShootCoord.x, ShootCoord.y + 5, ShootCoord.z + 5, ShootCoord.x, ShootCoord.y, ShootCoord.z, 100, 1, MISC::GET_HASH_KEY("WEAPON_AIRSTRIKE_ROCKET"), MyPlayer, 1, 1, 130);
				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(ShootCoord.x + 3, ShootCoord.y, ShootCoord.z + 5, ShootCoord.x, ShootCoord.y, ShootCoord.z, 100, 1, MISC::GET_HASH_KEY("WEAPON_AIRSTRIKE_ROCKET"), MyPlayer, 1, 1, 80);
				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(ShootCoord.x - 5, ShootCoord.y, ShootCoord.z + 5, ShootCoord.x, ShootCoord.y, ShootCoord.z, 100, 1, MISC::GET_HASH_KEY("WEAPON_AIRSTRIKE_ROCKET"), MyPlayer, 1, 1, 110);
				MISC::SHOOT_SINGLE_BULLET_BETWEEN_COORDS(ShootCoord.x, ShootCoord.y - 2, ShootCoord.z + 5, ShootCoord.x, ShootCoord.y, ShootCoord.z, 100, 1, MISC::GET_HASH_KEY("WEAPON_AIRSTRIKE_ROCKET"), MyPlayer, 1, 1, 150);
			}
		}
	}

	void MolotovGun(bool toggle)
	{
		NativeVector3 explosivecoords;
		if (toggle)
		{
			if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &explosivecoords))
			{
				FIRE::ADD_EXPLOSION(explosivecoords.x, explosivecoords.y, explosivecoords.z, 3, 3, true, false, 0, 0);
			}
		}
	}

	void FlareGun(bool toggle)
	{
		NativeVector3 explosivecoords;
		if (toggle)
		{
			if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &explosivecoords))
			{
				FIRE::ADD_EXPLOSION(explosivecoords.x, explosivecoords.y, explosivecoords.z, 22, 3, true, false, 0, 0);
			}
		}
	}

	void SmokeGun(bool toggle)
	{
		NativeVector3 explosivecoords;
		if (toggle)
		{
			if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &explosivecoords))
			{
				FIRE::ADD_EXPLOSION(explosivecoords.x, explosivecoords.y, explosivecoords.z, 20, 3, true, false, 0, 0);
			}
		}
	}

	/*void CartoonGun(bool toggle)
	{
		if (toggle)
		{
			if (is_ped_shooting(PLAYER::PLAYER_PED_ID())) {
				auto weapon = WEAPON::GET_CURRENT_PED_WEAPON_ENTITY_INDEX(PLAYER::PLAYER_PED_ID(), 0);
				auto weapon2 = WEAPON::GET_SELECTED_PED_WEAPON(PLAYER::PLAYER_PED_ID());
				NativeVector3 d1;
				NativeVector3 d2;
				MISC::GET_MODEL_DIMENSIONS(weapon2, &d1, &d2);
				auto x = std::sqrt((d1.x - d2.x));
				STREAMING::REQUEST_NAMED_PTFX_ASSET("scr_rcbarry2");
				while (!STREAMING::HAS_NAMED_PTFX_ASSET_LOADED("scr_rcbarry2"))
					std::this_thread::sleep_for(0ms);
				GRAPHICS::_USE_PARTICLE_FX_ASSET_NEXT_CALL("scr_rcbarry2");
				GRAPHICS::_START_PARTICLE_FX_NON_LOOPED_ON_ENTITY_2("muz_clown", weapon, ((x * x) / 1.9f + 0.7f), 0.f, 0.f, 0.f, 180.f, 0.f, 1.f, 1, 1, 1);
			}
		}
	}*/

	void VehicleGodMode(bool toggle)
	{
		if (toggle)
		{
			Memory::set_value<bool>({ OFFSET_PLAYER_PED, 0xD30, OFFSET_ENTITY_GOD }, true);
		}
		else
		{
			Memory::set_value<bool>({ OFFSET_PLAYER_PED, 0xD30, OFFSET_ENTITY_GOD }, false);
		}
	}

	void HornBoost(bool toggle)
	{
		if (toggle)
		{
			if (PLAYER::IS_PLAYER_PRESSING_HORN(PLAYER::PLAYER_ID()))
			{
				Vehicle Veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(PLAYER::PLAYER_ID()), false);
				NETWORK::NETWORK_REQUEST_CONTROL_OF_ENTITY(Veh);
				if (NETWORK::NETWORK_HAS_CONTROL_OF_ENTITY(Veh))
				{
					VEHICLE::SET_VEHICLE_FORWARD_SPEED(Veh, 40);
					GRAPHICS::_START_SCREEN_EFFECT("RaceTurbo", 0, 0);
				}
			}
		}
	}

	void VehicleJump(bool toggle)
	{
		if (toggle)
		{
			if (GetAsyncKeyState(VK_SPACE) && PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), 1)) {
				int veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), 0);
				ENTITY::APPLY_FORCE_TO_ENTITY(veh, 1, 0 + ENTITY::GET_ENTITY_FORWARD_X(veh), 0 + ENTITY::GET_ENTITY_FORWARD_Y(veh), 7, 0, 0, 0, 1, 0, 1, 1, 1, 1);
			}
		}
	}

	void AutoFix(bool toggle)
	{
		if (toggle)
		{
			DWORD AutoVeh = PED::GET_VEHICLE_PED_IS_USING(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(PLAYER::PLAYER_ID()));

			VEHICLE::SET_VEHICLE_FIXED(AutoVeh);
			VEHICLE::SET_VEHICLE_DEFORMATION_FIXED(AutoVeh);
			VEHICLE::SET_VEHICLE_DIRT_LEVEL(AutoVeh, 0);
		}
		ENTITY::SET_ENTITY_INVINCIBLE(PED::GET_VEHICLE_PED_IS_USING(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(PLAYER::PLAYER_ID())), g_MainScript->m_VehicleGodMode);
	}

	void DriftMode(bool toggle)
	{
		if (toggle)
		{
			VEHICLE::SET_VEHICLE_REDUCE_GRIP(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false), true);
		}
		else
		{
			VEHICLE::SET_VEHICLE_REDUCE_GRIP(PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false), false);
		}
	}

	void SeatBelt(bool toggle)
	{
		if (toggle)
		{
			Memory::set_value<byte>({ OFFSET_PLAYER_PED, 0x140C }, 56);
		}
		else
		{
			Memory::set_value<byte>({ OFFSET_PLAYER_PED, 0x140C }, 55);
		}
	}

	void HeadlightInten(bool toggle)
	{
		if (toggle)
		{
			Memory::set_value<float>({ 0x8, 0xD30, 0xA14 }, 10);
		}
		else
		{
			Memory::set_value<float>({ 0x8, 0xD30, 0xA14 }, 1);
		}
	}

	void ZeroGravity(bool toggle)
	{
		if (toggle)
		{
			Memory::set_value<float>({ 0x8, 0xD30, 0xC5C }, 0);
		}
		else
		{
			Memory::set_value<float>({ 0x8, 0xD30, 0xC5C }, 9.f);
		}
	}

	void BoostAndJump(bool toggle)
	{
		if (toggle)
		{
			Memory::set_value<byte>({ 0x8, 0xD30, 0x20, 0x58B }, 96);
		}
		else
		{
			Memory::set_value<byte>({ 0x8, 0xD30, 0x20, 0x58B }, 0);
		}
	}

	void VehParachute(bool toggle)
	{
		if (toggle)
		{
			Memory::set_value<byte>({ 0x8, 0xD30, 0x20, 0x58C }, 1);
		}
		else
		{
			Memory::set_value<byte>({ 0x8, 0xD30, 0x20, 0x58C }, 0);
		}
	}
	/////////////////////
		//Network
	/////////////////////
	void TeleportTo(int player)
	{
		Ped player_ped = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player);
		if (ENTITY::DOES_ENTITY_EXIST(player_ped))
		{
			auto pos = ENTITY::GET_ENTITY_COORDS(player_ped, true);
			if (PED::IS_PED_IN_ANY_VEHICLE(player_ped, true))
			{
				auto ped_veh = PED::GET_VEHICLE_PED_IS_IN(player_ped, false);
				PED::SET_PED_INTO_VEHICLE(PLAYER::PLAYER_PED_ID(), ped_veh, -2);
			}
			else if (PED::IS_PED_IN_ANY_VEHICLE(PLAYER::PLAYER_PED_ID(), false))
			{
				auto my_veh = PED::GET_VEHICLE_PED_IS_IN(PLAYER::PLAYER_PED_ID(), false);
				ENTITY::SET_ENTITY_COORDS(my_veh, pos.x, pos.y, pos.z, 0, 0, 0, 0);
			}
			else
			{
				ENTITY::SET_ENTITY_COORDS(PLAYER::PLAYER_PED_ID(), pos.x, pos.y, pos.z, 0, 0, 0, 0);
			}
		}
	}

	void FreezePlayer(Player player, bool enable)
	{
		if (enable)
		{
			BRAIN::CLEAR_PED_TASKS_IMMEDIATELY(PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(player));
		}
	}

	void RPDrop(uint32_t Play, bool toggle)
	{
		if (toggle)
		{
			if ((timeGetTime() - TimePD15) > DropDelay)
			{
				Ped iPed = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(Play);
				STREAMING::REQUEST_MODEL(MISC::GET_HASH_KEY("vw_prop_vw_colle_alien"));
				if (!STREAMING::HAS_MODEL_LOADED(MISC::GET_HASH_KEY("vw_prop_vw_colle_alien"))) {
					std::this_thread::sleep_for(0ms);
				}
				else {
					NativeVector3 playerPosition = ENTITY::GET_ENTITY_COORDS(iPed, FALSE);
					OBJECT::CREATE_AMBIENT_PICKUP(0x2C014CA6, playerPosition.x, playerPosition.y, playerPosition.z + 0.5f / 10, 0, 2000, MISC::GET_HASH_KEY("vw_prop_vw_colle_alien"), FALSE, TRUE);
					STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(MISC::GET_HASH_KEY("vw_prop_vw_colle_alien"));
				}
				TimePD15 = timeGetTime();
			}
		}
	}

	void RPGun(bool toggle)
	{
		NativeVector3 explosivecoords;
		if (toggle)
		{
			if (WEAPON::GET_PED_LAST_WEAPON_IMPACT_COORD(PLAYER::PLAYER_PED_ID(), &explosivecoords))
			{
				STREAMING::REQUEST_MODEL(MISC::GET_HASH_KEY("vw_prop_vw_colle_alien"));
				if (STREAMING::HAS_MODEL_LOADED(MISC::GET_HASH_KEY("vw_prop_vw_colle_alien")))
				{
					OBJECT::CREATE_AMBIENT_PICKUP(0x2C014CA6, explosivecoords.x, explosivecoords.y, explosivecoords.z + 0, 0, 2500, MISC::GET_HASH_KEY("vw_prop_vw_colle_alien"), true, false);
					STREAMING::SET_MODEL_AS_NO_LONGER_NEEDED(MISC::GET_HASH_KEY("vw_prop_vw_colle_alien"));
				}
			}
		}
	}

	void Spectate(uint32_t Play, bool toggle)
	{
		if (toggle)
		{
			NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(true, PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(Play));
		}

		else
		{
			NETWORK::NETWORK_SET_IN_SPECTATOR_MODE(false, PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(Play));
		}
	}
	/////////////////////////////////////////////////////////////////////////////////////
	//Protection
	/////////////////////////////////////////////////////////////////////////////////////
	void RAdmin(bool toggle)
	{
		const char* AdminNames[] = {
	"DigitalKitsune", "DigitalFox9", "FoxesAreCool69", "huginn6767", "xCuteBunny", "random_123",
		"flyingcobra16", "CriticalRegret", "bipolarcarp", "HammerDaddy69", "Kakorot02", "ScentedPotter",
		"random123", "Huginn5", "BrexitToTheKnees", "SweetPlumbus", "RossThetic", "StrongBelwas1", "PayneInUrAbs",
		"shibuz_gamer123", "M1thras", "Morr1gan", "Titan261", "johnet123", "jakw0lf", "PainOfSex", "Coffee_Collie",
		"kingmario11", "Z3ro_Chill", "Sonknuck-", "Mavrick", "NYC_TBM", "BananaGod951", "thewho146" };
		if (toggle)
		{
			for (int i = 0; i < 32; i++)
			{
				Player player = PLAYER::GET_PLAYER_PED_SCRIPT_INDEX(i);

				if (!player)
					continue;

				for (int j = 0; j < 33; j++)
				{
					const char* AdminName = AdminNames[j];

					if (PLAYER::GET_PLAYER_NAME(i) == AdminName)
					{
						g_Logger->Info("Detected admin %s, finding new session...", AdminName);
						*ScriptGlobal::ScriptGlobal(1312854).As<int*>() = 0;
						MISC::SET_BIT(&*ScriptGlobal::ScriptGlobal(1312854).As<int*>(), 1);
						std::this_thread::sleep_for(200ms);
						MISC::SET_BIT(&*ScriptGlobal::ScriptGlobal(1312854).As<int*>(), 0);
						g_Logger->Info("Joining new session!");
					}
				}
			}
		}
	}

	void Lua()
	{
		//sike nigger
	}
}
