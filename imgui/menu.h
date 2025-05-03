

#include "framework/ImGui/gui.h"
#include "framework/ImGui/imgui.h"
#include "cheat_configs.h"
#include <d3d11.h>
#include <d3dx11tex.h>
#include <d3dcompiler.h>
#include "framework/ImGui/imgui_impl_dx11.h"
#include "framework/ImGui/imgui_impl_win32.h"
#include <filesystem>
#include "Keybinds.h"
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);



void menu_class() {
	ui::add_page(0, []() {
		ImGui::BeginGroup();
		{
			ui::begin_child("AIMBOT");
			{
				static bool bools[10];
				static int ints[10];

				ImGui::Checkbox("Enabled", &settings::aimbot::enable);
				ImGui::Checkbox("Triggerbot", &settings::aimbot::trigger_bot);
				ImGui::SliderFloat("Smoothness", &settings::aimbot::smoothness, 0, 100, "%.1f%%");

				ImGui::Checkbox("Render FOV", &settings::aimbot::show_fov);
				ImGui::Checkbox("Dynamic FOV", &settings::aimbot::dynamic_fov);
				ImGui::Checkbox("Crosshair", &settings::Features::crosshair);
				ImGui::SliderFloat("FOV Size", &settings::aimbot::fov, 0, 450, "%.1f%%");
				ImGui::Text("Aimbot Key");

				HotkeyButton(settings::aimbot::current_key, ChangeKey, keystatus);

				ImGui::Text("Trigger Key");
				HotkeyButton(settings::triggerbot::trigger_key, ChangeKeytrigger, keystatus2);

			}
			ui::end_child();

			
		}
		ImGui::EndGroup();

		ImGui::SameLine();

		
		});


	// [[ Visuals Tab ]]


	ui::add_page(1, []() {
		ImGui::BeginGroup();
		{
			ui::begin_child("Visuals");
			{
				static bool bools[10];
				static int ints[10];
				const char* box_types[] = { ("Regular"), ("Cornered")};
				ImGui::Checkbox("Enable Visual", &settings::visuals::enable);
				ImGui::Checkbox("Box Esp", &settings::visuals::Box);
				ImGui::Combo(("Box Type"), &settings::visuals::BoxType, box_types, IM_ARRAYSIZE(box_types), 2);


				ImGui::Checkbox("Skeleton [CPU]", &settings::visuals::skeleton);
		//		ImGui::Checkbox("Cornered BOX", &settings::visuals::CorneredBox);
				ImGui::Checkbox("Filled Box", &settings::visuals::fill_box);
				ImGui::Checkbox("Username", &settings::visuals::username);
				ImGui::Checkbox("Platform", &settings::visuals::platform);
				ImGui::Checkbox("Distance", &settings::visuals::distance);
				ImGui::Checkbox("Weapon", &settings::visuals::weapon);


			}
			ui::end_child();
			ImGui::SameLine();
			ui::begin_child("Radar");
			{
				
				ImGui::Checkbox("Radar Enable", &settings::radar::radar);
				ImGui::Checkbox("Cross", &settings::radar::cross);
				ImGui::Checkbox("Local Player", &settings::radar::local_player);
				ImGui::SliderFloat("Range", &settings::radar::range, 25, 450, "%.1f%%");


			}
			ui::end_child();
		}
		ImGui::EndGroup();

		ImGui::SameLine();


		});



	// [[ Weapon Tab ]] 

	ui::add_page(2, []() {
		ImGui::BeginGroup();
		{
			ui::begin_child("Weapon Configs");
			{
		
				const char* weapon_cnfgs[] = { ("Shotgun"), ("Smg/Pistol"), ("Rifle"), "Sniper"};
				ImGui::Checkbox("Enable Weapon Configs", &settings::aimbot::weapon_cnfg);
				ImGui::Combo(("Weapon Configs"), &settings::aimbot::weapon_type, weapon_cnfgs, IM_ARRAYSIZE(weapon_cnfgs), 4);
				if (settings::aimbot::weapon_cnfg) {
					if (settings::aimbot::weapon_type == 0) {
						ImGui::Checkbox(("Shotgun Aimbot"), &settings::aimbot::shotgun::aimbot);
						ImGui::Checkbox(("Shotgun Render Fov"), &settings::aimbot::shotgun::render_fov);
						ImGui::SliderInt(("Shotgun Fov"), &settings::aimbot::shotgun::aim_fov, 0, 1000, ("%d%"));
						ImGui::SliderInt(("Shotgun Smoothing"), &settings::aimbot::shotgun::smooth, 3, 100, ("%d%"));
					}
					if (settings::aimbot::weapon_type == 1) {
						ImGui::Checkbox(("Smg Aimbot"), &settings::aimbot::smg::aimbot);
						ImGui::Checkbox(("Smg Render Fov"), &settings::aimbot::smg::render_fov);
						ImGui::SliderInt(("Smg Fov"), &settings::aimbot::smg::aim_fov, 0, 1000, ("%d%"));
						ImGui::SliderInt(("Smg Smoothing"), &settings::aimbot::smg::smooth, 3, 100, ("%d%"));
					}
					if (settings::aimbot::weapon_type == 2) {
						ImGui::Checkbox(("Rifle Aimbot"), &settings::aimbot::rifle::aimbot);
						ImGui::Checkbox(("Rifle Render Fov"), &settings::aimbot::rifle::render_fov);
						ImGui::SliderInt(("Rifle Fov"), &settings::aimbot::rifle::aim_fov, 0, 1000, ("%d%"));
						ImGui::SliderInt(("Rifle Smoothing"), &settings::aimbot::rifle::smooth, 3, 100, ("%d%"));
					}
					if (settings::aimbot::weapon_type == 3) {
						ImGui::Checkbox(("Sniper Aimbot"), &settings::aimbot::sniper::aimbot);
						ImGui::Checkbox(("Sniper Render Fov"), &settings::aimbot::sniper::render_fov);
						ImGui::SliderInt(("Sniper Fov"), &settings::aimbot::sniper::aim_fov, 0, 1000, ("%d%"));
						ImGui::SliderInt(("Sniper Smoothing"), &settings::aimbot::sniper::smooth, 3, 100, ("%d%"));
					}
				}
		




			}
			ui::end_child();





		}
		ImGui::EndGroup();

		ImGui::SameLine();
		ui::begin_child("Exploits");
		{

	///		ImGui::Checkbox("Speed Hack", &settings::Exploits::sped_hack);
			ImGui::Checkbox("SpinBot", &settings::Exploits::spinbot);


		}
		ui::end_child();

		});


	

	// [[ Settings Tab ]] 

	ui::add_page(3, []() {
		ImGui::BeginGroup();
		{
			ui::begin_child("Save Configs");
			{

				if (ImGui::Button("Save Configs", ImVec2(125, 25))) {
					config::save_config();
				}
				if (ImGui::Button("Load Configs", ImVec2(125, 25))) {

					config::load_config();
				}
			}
			ui::end_child();


		}
		ImGui::EndGroup();

		ImGui::SameLine();


		});




}

