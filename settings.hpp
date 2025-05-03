#pragma once
#include <chrono>
namespace settings
{
	inline int width = GetSystemMetrics(SM_CXSCREEN);
	inline int height = GetSystemMetrics(SM_CYSCREEN);

	inline int screen_center_x = width / 2;
	inline int screen_center_y = height / 2;
	inline bool show_menu = true;
	inline int tab = 0;
	namespace aimbot
	{
		namespace rifle
		{
			inline bool aimbot = 1;
			inline bool render_fov = 0;
			inline int aim_fov = 15;
			inline int smooth = 3;
		}

		namespace shotgun
		{
			inline bool aimbot = 1;
			inline bool render_fov = 0;
			inline int aim_fov = 15;
			inline int smooth = 3;
		}

		namespace smg
		{
			inline bool aimbot = 1;
			inline bool render_fov = 0;
			inline int aim_fov = 15;
			inline int smooth = 3;
		}

		namespace sniper
		{
			inline bool aimbot = 1;
			inline bool render_fov = 0;
			inline int aim_fov = 15;
			inline int smooth = 3;
		}

		inline bool enable = false;
		inline bool weapon_cnfg = false;
		inline int  weapon_type = 0;
		inline bool trigger_bot = false;
		inline bool show_fov = false;
		inline bool dynamic_fov = false;
		inline bool projectile_predict = false;
		inline float fov = 160;
		inline float smoothness = 6;
		inline bool visible_check = false;
		inline static int current_key = VK_RBUTTON;

	}
	namespace triggerbot {
		std::chrono::steady_clock::time_point tb_begin;
		std::chrono::steady_clock::time_point tb_end;
		bool has_clicked;
		int tb_time_since;
		inline static int trigger_key = VK_RBUTTON;
		inline bool shotgun_only = false;
		bool NoKey = 1;
		int Delay = 0;
		int TriggerDistance = 20;
		bool triggerDistanceCheck = false;

	}

	namespace visuals
	{
		inline bool enable = true;
		inline bool CorneredBox = false;
		inline bool fill_box = false;
		inline bool fill_wcbox = false;
		inline bool line = false;
		inline bool distance = false;
		inline bool weapon = false;
		inline bool username = false;
		inline bool platform = false;
		inline bool Box = true;
		inline bool skeleton = false;
		inline int BoxType = 0;
	}
	namespace Features
	{
		inline bool Watermark = true;
		inline bool info = false;
		inline bool crosshair = false;
		inline bool fpswater = true;
	}
	namespace radar
	{
		inline bool radar = false;
		inline bool cross = true;
		inline bool local_player = true;
		inline float range = 50.0f;
	}
	namespace Exploits
	{
		inline bool spinbot = false;
		inline bool sped_hack = false;
		inline float fovchanger_value = false;
		inline bool fovchanger = false;
		inline bool playerbackwards = false;
	}
	namespace trigger
	{
		inline static int current_key = VK_LBUTTON;
		inline bool Triggerbot = false;
		inline float TriggerDelay = 70;
	}
}