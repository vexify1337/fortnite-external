#pragma once
#include "imgui/framework/ImGui/imgui.h"
#include "imgui/framework/ImGui/imgui_impl_dx11.h"
#include "imgui/framework/ImGui/imgui_impl_win32.h"
#include <dwmapi.h>
#include "iostream"
#include "includes.hpp"
#include <vector>
#pragma comment(lib, "dwmapi.lib")
#include <dxgi.h>
#include <d3d11.h> 
#include <windows.h>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <cstdint>
#include "imgui/framework/ImGui/imgui.h"
#include "imgui/framework/ImGui/imgui_impl_dx11.h"
#include "imgui/framework/ImGui/imgui_impl_win32.h"
#include <dwmapi.h>
#include <dxgi.h>
#include <d3d11.h>
#include <windows.h>
#include <vector>
#include <string>
#include <thread>
#include <regex>
#include "imgui/menu.h"
IDXGIFactory* p_factory = NULL; 
ID3D11Device* p_device = NULL; 
ID3D11DeviceContext* p_device_context = NULL; 
DXGI_SWAP_CHAIN_DESC p_params = { 0 }; 
MSG messager = { 0 }; 
HWND my_wnd = NULL; 
HWND game_wnd = NULL; 
DWORD processID;
 IDXGISwapChain* p_swap_chain;
 ID3D11RenderTargetView* p_render_target_view;
 ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
#include "../imgui/framework//ImGui/gui.h"
#include "filesystem"
 void gRenderMenu() {
	 if (settings::show_menu) {
		 ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
		 ImGui::Begin("UI", 0, ImGuiWindowFlags_NoDecoration);
		 {
			 ImGui::PopStyleVar();
			 ImGui::SetWindowSize(ui::size);

			 ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, GImGui->Style.WindowRounding);
			 ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
			 if (ImGui::BeginChild("header", ImVec2(-1, 60), true)) {
				 if (!ui::tabs.empty()) {
					 for (size_t i = 0; i < ui::tabs.size(); ++i) {
						 ui::tab(i);
						 if (i < ui::tabs.size() - 1) { // Avoid SameLine after last tab
							 ImGui::SameLine(0, 0);
						 }
					 }
				 }
				 else {
					 ImGui::Text("No tabs available");
				 }
				 ImGui::EndChild();
			 }
			 ImGui::PopStyleVar(2);

			 ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ui::content_anim);
			 ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 20, 20 });
			 ImGui::BeginChild("content", { -1, -1 }, 0, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoBackground);
			 {
				 ImGui::PopStyleVar();

				 ui::render_page();
			 }
			 ImGui::EndChild();

			 ImGui::PopStyleVar();
		 }
		 ImGui::End();

		 ui::handle_alpha_anim();

		 ImGui::Render();
	 }
 }
#include "fonts.h"
 void Init() {
	 std::cout << "[Init] Starting initialization\n";

	 DXGI_SWAP_CHAIN_DESC sd;
	 ZeroMemory(&sd, sizeof(sd));
	 sd.BufferCount = 2;
	 sd.BufferDesc.Width = 0;
	 sd.BufferDesc.Height = 0;
	 sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	 sd.BufferDesc.RefreshRate.Numerator = 60;
	 sd.BufferDesc.RefreshRate.Denominator = 1;
	 sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	 sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	 sd.OutputWindow = my_wnd;
	 sd.SampleDesc.Count = 1;
	 sd.SampleDesc.Quality = 0;
	 sd.Windowed = TRUE;
	 sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	 UINT createDeviceFlags = 0;
	 D3D_FEATURE_LEVEL featureLevel;
	 constexpr D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

	 HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &p_swap_chain, &p_device, &featureLevel, &p_device_context);
	 if (res == DXGI_ERROR_UNSUPPORTED) {
		 res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &p_swap_chain, &p_device, &featureLevel, &p_device_context);
	 }
	 if (FAILED(res)) {
		 std::cout << "[Init] Failed to create D3D11 device and swap chain: 0x" << std::hex << res << "\n";
		 return;
	 }
	 std::cout << "[Init] Device and swap chain created\n";

	 ID3D11Texture2D* pBackBuffer = nullptr;
	 if (FAILED(p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) {
		 std::cout << "[Init] Failed to get back buffer\n";
		 return;
	 }

	 if (FAILED(p_device->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView))) {
		 std::cout << "[Init] Failed to create render target view\n";
		 pBackBuffer->Release();
		 return;
	 }
	 pBackBuffer->Release();
	 std::cout << "[Init] Render target view created\n";

	 IMGUI_CHECKVERSION();
	 ImGui::CreateContext();
	 ImGuiIO& io = ImGui::GetIO();
	 io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad;

	 ui::colors();
	 ui::styles();


	 auto config = ImFontConfig();
	 config.FontDataOwnedByAtlas = false;
	 ImFont* font1 = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(&fontb, sizeof fontb, 18, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());/*ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\fortnite.otf", 15.f);*//*io.Fonts->AddFontFromFileTTF("C:\\Users\\vsn13\\Downloads\\free menu0302\\DirectX 9\\GUI\\resources\\fontb.ttf", 15.f);*/
	 ImFont* font2 = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(&Glyphter, sizeof Glyphter, 18, NULL, ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());/*ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\fortnite.otf", 15.f);*//*io.Fonts->AddFontFromFileTTF("C:\\Users\\vsn13\\Downloads\\free menu0302\\DirectX 9\\GUI\\resources\\fontb.ttf", 15.f);*/

	 io.Fonts->Build();

	 if (!ImGui_ImplWin32_Init(my_wnd)) {
		 std::cout << "[Init] Failed to initialize ImGui Win32\n";
		 return;
	 }
	 if (!ImGui_ImplDX11_Init(p_device, p_device_context)) {
		 std::cout << "[Init] Failed to initialize ImGui DX11\n";
		 return;
	 }
	 std::cout << "[Init] ImGui initialized\n";

	 // Define the UI page
	 std::cout << "[Init] Defining UI page\n";
	 menu_class();
 }

void create_overlay()
{
	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		0,
		LoadIcon(0, IDI_APPLICATION),
		LoadCursor(0, IDC_ARROW),
		0,
		0,
		"D11_Base",
		LoadIcon(0, IDI_APPLICATION)
	};
	ATOM rce = RegisterClassExA(&wcex);
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	my_wnd = gui::create_window_in_band(0, rce, L"D11_Base", WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, 0, 0, wcex.hInstance, 0, gui::ZBID_UIACCESS);
	SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(my_wnd, &margin);
	ShowWindow(my_wnd, SW_SHOW);
	UpdateWindow(my_wnd);
}



void radar_range(float* x, float* y, float range)
{
	if (fabs((*x)) > range || fabs((*y)) > range)
	{
		if ((*y) > (*x))
		{
			if ((*y) > -(*x))
			{
				(*x) = range * (*x) / (*y);
				(*y) = range;
			}
			else
			{
				(*y) = -range * (*y) / (*x);
				(*x) = -range;
			}
		}
		else
		{
			if ((*y) > -(*x))
			{
				(*y) = range * (*y) / (*x);
				(*x) = range;
			}
			else
			{
				(*x) = -range * (*x) / (*y);
				(*y) = -range;
			}
		}
	}
}

void project_world_to_radar(Vector3 vOrigin, int& screenx, int& screeny, ImVec2 pos)
{
	Vector3 vAngle = ViewPoint::Rotation;
	auto fYaw = vAngle.y * M_PI / 180.0f;
	float dx = vOrigin.x - ViewPoint::Location.x;
	float dy = vOrigin.y - ViewPoint::Location.y;
	float fsin_yaw = sinf(fYaw);
	float fminus_cos_yaw = -cosf(fYaw);
	float x = dy * fminus_cos_yaw + dx * fsin_yaw;
	x = -x;
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;
	float range = (float)(settings::radar::range / 5) * 1000.f;

	radar_range(&x, &y, range);

	ImVec2 DrawPos = ImVec2(pos.x, pos.y);
	ImVec2 DrawSize = ImVec2(200.0f, 200.0f);
	float radarRadius = 200.0f / 2;
	float distance = sqrt(pow(screenx - DrawPos.x, 2) + pow(screeny - DrawPos.y, 2));

	if (distance > radarRadius)
	{
		float angle = atan2(screeny - DrawPos.y, screenx - DrawPos.x);
		screenx = static_cast<int>(DrawPos.x + radarRadius * cos(angle));
		screeny = static_cast<int>(DrawPos.y + radarRadius * sin(angle));
	}

	int rad_x = (int)DrawPos.x;
	int rad_y = (int)DrawPos.y;
	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;
	int x_max = (int)r_siz_x + rad_x - 5;
	int y_max = (int)r_siz_y + rad_y - 5;

	screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
	screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));

	if (screenx > x_max)
		screenx = x_max;

	if (screenx < rad_x)
		screenx = rad_x;

	if (screeny > y_max)
		screeny = y_max;

	if (screeny < rad_y)
		screeny = rad_y;
}
namespace prediction {
	static float atan2_approximation2(float y, float x)
	{
		if (x == 0.0f)
		{
			if (y > 0.0f) return 1.5707963f;
			if (y == 0.0f) return 0.0f;
			return -1.5707963f;
		}
		float atan;
		float z = y / x;
		if (fabs(z) < 1.0f)
		{
			atan = z / (1.0f + 0.28f * z * z);
			if (x < 0.0f)
			{
				if (y < 0.0f) return atan - 3.14159265f;
				return atan + 3.14159265f;
			}
		}
		else
		{
			atan = 1.5707963f - z / (z * z + 0.28f);
			if (y < 0.0f) return atan - 3.14159265f;
		}
		return atan;
	}
	static float powf_(float _X, float _Y) {
		return (_mm_cvtss_f32(_mm_pow_ps(_mm_set_ss(_X), _mm_set_ss(_Y))));
	}
	static float sqrtf_(float _X) {
		return (_mm_cvtss_f32(_mm_sqrt_ps(_mm_set_ss(_X))));
	}
	static double GetCrossDistance(double x1, double y1, double x2, double y2) {
		return sqrtf(powf((x2 - x1), 2) + powf_((y2 - y1), 2));
	}
	static float GetBulletDrop(float height, float DepthPlayerTarget, float BulletVelocity, float GravityAcceleration)
	{
		float pitch = (atan2_approximation2(height, DepthPlayerTarget));
		float BulletVelocityXY = BulletVelocity * cos(pitch);
		float Time = DepthPlayerTarget / BulletVelocityXY;
		float TotalVerticalDrop = (0.5f * GravityAcceleration * Time * Time);
		return TotalVerticalDrop;
	}
	static float GetBulletDropCorrectionScale(Vector3 CameraLocation, Vector3 TargetPosition, float ProjectileSpeed, float ProjectileGravity) {
		float Height = TargetPosition.z - CameraLocation.z;
		float DepthPlayerTarget = sqrt(pow(TargetPosition.y - CameraLocation.y, 2) + pow(TargetPosition.x - CameraLocation.x, 2));
		float BulletDrop = GetBulletDrop(Height, DepthPlayerTarget, ProjectileSpeed, ProjectileGravity);
		return BulletDrop;
	}
	Vector3 PredictLocation(Vector3 CurrentLocation, Vector3 TargetVelocity, float ProjectileSpeed, float ProjectileGravityScale, float distance)
	{
		Vector3 CalculatedPosition = CurrentLocation;
		float TimeToTarget = distance / ProjectileSpeed;
		float CalculatedDrop = 0.0f; // Initialize to avoid uninitialized variable
		if (ProjectileGravityScale > 0.0f) {
			CalculatedDrop = GetBulletDropCorrectionScale(ViewPoint::Location, CurrentLocation, ProjectileSpeed, ProjectileGravityScale);
		}

		// Manual component-wise multiplication and addition
		CalculatedPosition.x += TargetVelocity.x * TimeToTarget;
		CalculatedPosition.y += TargetVelocity.y * TimeToTarget;
		CalculatedPosition.z += TargetVelocity.z * TimeToTarget;

		CalculatedPosition.z += CalculatedDrop;

		return CalculatedPosition;
	}
}



static ImVec2 radarSize = { 200, 200 };
static ImVec2 radarPos = { 10, 35 };
static int RedDotSize = 3;


void Fpswtr() {

	constexpr float fontSize = 24.0f;
	constexpr ImVec2 watermarkPos = ImVec2(10.0f, 5.0f);

	static float lastTime = 0.0f;
	static int fps = 0;

	float currentTime = ImGui::GetTime();
	if (currentTime - lastTime >= 1.0f) {
		fps = static_cast<int>(ImGui::GetIO().Framerate); 
		lastTime = currentTime;
	}

	char fpsText[32];
	snprintf(fpsText, sizeof(fpsText), "FPS: %.2f", ImGui::GetIO().Framerate);
	ImVec2 fpsSize = ImGui::CalcTextSize(fpsText);
	ImVec2 fpsPos = ImVec2(watermarkPos.x + 3.0f, watermarkPos.y + 3.0f);

}





#include <chrono>

using namespace std::chrono;




void DrawCrosshair(ImVec2 center, ImVec4 color, float size) {
	ImGui::GetForegroundDrawList()->AddLine(
		ImVec2(center.x - size, center.y), ImVec2(center.x + size, center.y),
		ImGui::ColorConvertFloat4ToU32(color), 2.0f
	);
	ImGui::GetForegroundDrawList()->AddLine(
		ImVec2(center.x, center.y - size), ImVec2(center.x, center.y + size),
		ImGui::ColorConvertFloat4ToU32(color), 2.0f
	);
}
#include <thread>
int HeldWeaponType;

enum EFortWeaponType : int
{
	Rifle = 0,
	Shotgun = 1,
	Smg = 2,
	Sniper = 3
};
// [[ Not using this for anything for now ]]
struct WeaponInformation
{
	int32_t ammo_count;
	int32_t max_ammo;

	BYTE tier;
	std::string weapon_name;
	std::string buildplan;

};

struct PlayerCacheEntry {
	uintptr_t player_state;
	int team_id;
	std::string username;
	std::string platform;
	std::string weapon;
	Vector3 head3d;
	Vector3 bottom3d;
	Vector2 head2d;
	Vector2 bottom2d;
	bool is_despawning;
	std::chrono::steady_clock::time_point last_updated;
	uintptr_t mesh;
};

struct WeaponCacheEntry {
	int weapon_type;
	float projectile_speed;
	float projectile_gravity;
	std::string weapon_name;
	std::chrono::steady_clock::time_point last_updated;
};

class GameCache {
private:
	std::unordered_map<uintptr_t, PlayerCacheEntry> player_cache;
	WeaponCacheEntry weapon_cache;
	std::mutex cache_mutex;
	const std::chrono::milliseconds cache_timeout = std::chrono::milliseconds(1000); // 1 second timeout

public:

	bool GetPlayerCache(uintptr_t pawn_private, PlayerCacheEntry& entry) {
		std::lock_guard<std::mutex> lock(cache_mutex);
		auto it = player_cache.find(pawn_private);
		if (it != player_cache.end()) {
			auto now = std::chrono::steady_clock::now();
			if (std::chrono::duration_cast<std::chrono::milliseconds>(now - it->second.last_updated) < cache_timeout) {
				entry = it->second;
				return true;
			}
			player_cache.erase(it);
		}
		return false;
	}

	void UpdatePlayerCache(const PlayerCacheEntry& entry) {
		std::lock_guard<std::mutex> lock(cache_mutex);
		player_cache[entry.player_state] = entry;
	}


	void InvalidatePlayerCache(uintptr_t pawn_private) {
		std::lock_guard<std::mutex> lock(cache_mutex);
		player_cache.erase(pawn_private);
	}


	void ClearPlayerCache() {
		std::lock_guard<std::mutex> lock(cache_mutex);
		player_cache.clear();
	}

	bool GetWeaponCache(WeaponCacheEntry& entry) {
		std::lock_guard<std::mutex> lock(cache_mutex);
		auto now = std::chrono::steady_clock::now();
		if (weapon_cache.last_updated.time_since_epoch().count() == 0 ||
			std::chrono::duration_cast<std::chrono::milliseconds>(now - weapon_cache.last_updated) < cache_timeout) {
			entry = weapon_cache;
			return true;
		}
		return false;
	}
	void UpdateWeaponCache(const WeaponCacheEntry& entry) {
		std::lock_guard<std::mutex> lock(cache_mutex);
		weapon_cache = entry;
	}
};
GameCache g_cache;
std::string LocalPlayerWeapon;

namespace SophieRain {
	class Exploits_Funcs {
		public
			:
				void run_spinbot( ) {
					auto Mesh = ReadMemory<uintptr_t>(pointers.local_pawn + MESH);

					static auto Cached = ReadMemory<uintptr_t>(Mesh + 0x150);

					if (GetAsyncKeyState(VK_SPACE)) {

						write(Mesh + 0x150, Vector3(1, rand() % 361, 1));

					}
				}
	}; Exploits_Funcs g_funcs;
	bool debuggame = 0; // [[ set true, to debug  the programmy gammy ! ]]]
	bool debug_app = 1;
	void g_ExploitsThread() {
		for (;;) {
			 
			if (settings::Exploits::spinbot) {
				g_funcs.run_spinbot();
			}


			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}
	void g_PointersThread() {
		int last_array_count = 0;
		for (;;) {
			pointers.uworld = ReadMemory<uintptr_t>(virtualaddy + UWORLD);
			if (debuggame) { std::cout << "UWorld Pointers -> " << std::hex << pointers.uworld << std::endl; }
			if (!pointers.uworld) continue;

			pointers.game_instance = ReadMemory<uintptr_t>(pointers.uworld + GAME_INSTANCE);
			if (debuggame) { std::cout << "Game Instance Pointers -> " << std::hex << pointers.game_instance << std::endl; }
			if (!pointers.game_instance) continue;

			pointers.local_players = ReadMemory<uintptr_t>(ReadMemory<uintptr_t>(pointers.game_instance + LOCAL_PLAYERS));
			pointers.player_controller = ReadMemory<uintptr_t>(pointers.local_players + PLAYER_CONTROLLER);
			pointers.local_pawn = ReadMemory<uintptr_t>(pointers.player_controller + LOCAL_PAWN);

			if (pointers.local_pawn != 0) {
				pointers.root_component = ReadMemory<uintptr_t>(pointers.local_pawn + ROOT_COMPONENT);
				pointers.player_state = ReadMemory<uintptr_t>(pointers.local_pawn + PLAYER_STATE);
				pointers.pawn_private = ReadMemory<uintptr_t>(pointers.player_state + PAWN_PRIVATE);
				pointers.HimPlayerState = ReadMemory<uintptr_t>(pointers.pawn_private + PLAYER_STATE);
				pointers.my_team_id = ReadMemory<int>(pointers.player_state + TEAM_INDEX);
			}

			pointers.game_state = ReadMemory<uintptr_t>(pointers.uworld + GAME_STATE);
			if (debuggame) { std::cout << "Game State Pointers -> " << std::hex << pointers.game_state << std::endl; }
			if (!pointers.game_state) continue;

			pointers.player_array = ReadMemory<uintptr_t>(pointers.game_state + PLAYER_ARRAY);
			pointers.array_count = ReadMemory<int>(pointers.game_state + (PLAYER_ARRAY + sizeof(uintptr_t)));

			
			if (pointers.array_count != last_array_count) {
				g_cache.ClearPlayerCache();
				last_array_count = pointers.array_count;
			}

			pointers.closest_distance = FLT_MAX;
			pointers.closest_mesh = NULL;
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}

	namespace g_functions {
		void g_DistanceRender(Vector2 location, float distance, const ImColor color)
		{
			char dist[64];
			sprintf_s(dist, "%.fm", distance);
			ImVec2 text_size = ImGui::CalcTextSize(dist);
			ImGui::GetForegroundDrawList()->AddText(ImVec2(location.x - text_size.x / 2, location.y - text_size.y / 2), color, dist);
		}
		std::string sanitizeActorName(const std::string& actorName) {
			std::regex invalidCharRegex("[^A-Za-z0-9_]");
			return std::regex_replace(actorName, invalidCharRegex, " ");
		}

		void g_CorneredBoxRender(int x, int y, int w, int h, const ImColor color, int thickness)
		{
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, thickness);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, thickness);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, thickness);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, thickness);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, thickness);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, thickness);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, thickness);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, thickness);
		}



		void g_BoxRender(int X, int Y, int W, int H, const ImColor color, int thickness)
		{
			ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)Y }, ImVec2{ (float)(X + W), (float)Y }, color, thickness);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)(X + W), (float)Y }, ImVec2{ (float)(X + W), (float)(Y + H) }, color, thickness);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)(Y + H) }, ImVec2{ (float)(X + W), (float)(Y + H) }, color, thickness);
			ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)Y }, ImVec2{ (float)X, (float)(Y + H) }, color, thickness);
		}

		void g_FilledRectRender(int x, int y, int w, int h, const ImColor color)
		{
			ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
		}

		void g_DrawLineRender(Vector2 target, const ImColor color)
		{
			ImGui::GetForegroundDrawList()->AddLine(ImVec2(settings::screen_center_x, settings::height), ImVec2(target.x, target.y), color, 0.1f);
		}

		std::string g_DecryptUsername(uintptr_t player_state) {
			int pNameLength;
			uint16_t* pNameBufferPointer;
			int i;
			char v25;
			int v26;
			int v29;
			char16_t* pNameBuffer;

			uintptr_t pNameStructure = ReadMemory<uintptr_t>(player_state + 0xb08);
			pNameLength = ReadMemory<int>(pNameStructure + 0x10);
			if (pNameLength <= 0)
				return "[RESCINDED]";

			pNameBuffer = new char16_t[pNameLength];
			uintptr_t pNameEncryptedBuffer = ReadMemory <uintptr_t>(pNameStructure + 0x8);
			PagefaultAPI::read_physical((PVOID)pNameEncryptedBuffer, pNameBuffer, pNameLength * sizeof(wchar_t));

			v25 = pNameLength - 1;
			v26 = 0;
			pNameBufferPointer = (uint16_t*)pNameBuffer;

			for (i = (v25) & 3;; *pNameBufferPointer++ += i & 7)
			{
				v29 = pNameLength - 1;
				if (!(uint32_t)pNameLength)
					v29 = 0;

				if (v26 >= v29)
					break;

				i += 3;
				++v26;
			}

			std::u16string temp_wstring(pNameBuffer);
			delete[] pNameBuffer;

			return std::string(temp_wstring.begin(), temp_wstring.end());
		}
		std::string g_DecryptPlatform(uintptr_t player_state)
		{

			DWORD_PTR test_platform = ReadMemory<DWORD_PTR>(player_state + 0x430); //0x438 FortniteGame.FortPlayerState.Platform -> 0x408 https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UFortHudContextTutorialTipsBase&member=PositionPerPlatform  UFortHudContextTutorialTipsBase.PositionPerPlatform -> 0x430 (Size: 0x10)
			if (!test_platform) return std::string("AI");
			wchar_t platform[64];
			PagefaultAPI::read_physical((PVOID)test_platform, (size_t*)platform, sizeof(platform));
			std::wstring platform_wstr(platform);
			std::string platform_str(platform_wstr.begin(), platform_wstr.end());
			return platform_str;
		}


		std::string g_DecryptWeapon(uintptr_t Player) {
			std::string weapon_name;

			uint64_t current_weapon = ReadMemory<uint64_t>(Player + CURRENT_WEAPON);
			uint64_t weapon_data = ReadMemory<uint64_t>(current_weapon + WEAPON_DATA);
			uint64_t item_name = ReadMemory<uint64_t>(weapon_data + 0x40);

			uint64_t FData = ReadMemory<uint64_t>(item_name + 0x18);
			int FLength = ReadMemory<int>(item_name + 0x20);

			if (FLength > 0 && FLength < 50)
			{
				wchar_t* WeaponBuffer = new wchar_t[FLength];
				PagefaultAPI::read_physical((void*)FData, (PVOID)WeaponBuffer, FLength * sizeof(wchar_t));
				std::wstring wstr_buf(WeaponBuffer);
				weapon_name.append(std::string(wstr_buf.begin(), wstr_buf.end()));

				delete[] WeaponBuffer;
			}

			return weapon_name;
		}


		void g_Skeleton(uintptr_t mesh, float distance) {
			if (!mesh) return;
			if (distance > 25) return;
			if (!EntityMesh->IsMeshVisibile(mesh)) {
				return;
			}

			const std::vector<int> boneIDs = {
				66, 9, 10, 11, 38, 39, 40, 2, 71, 72, 78, 79, 75, 82, 67
			};

			std::vector<Vector2> screenPositions(boneIDs.size());

			for (size_t i = 0; i < boneIDs.size(); ++i) {
				Vector3 bone = EntityMesh->GetBoneMatrixByID(mesh, boneIDs[i]);
				screenPositions[i] = ViewPoint::K_Project(bone);
			}

			const struct BoneConnection {
				size_t from;
				size_t to;
			} connections[] = {
				{1, 4}, {1, 2}, {2, 3}, {4, 5}, {5, 6}, {0, 7},
				{7, 8}, {8, 9}, {9, 12}, {7, 10}, {10, 11}, {11, 13}
			};

			ImColor lineColor = EntityMesh->IsMeshVisibile(mesh) ?
				ImColor(36, 207, 2, 250) :
				ImColor(250, 0, 0, 250);

			auto* drawList = ImGui::GetForegroundDrawList();
			for (const auto& conn : connections) {
				drawList->AddLine(
					ImVec2(screenPositions[conn.from].x, screenPositions[conn.from].y),
					ImVec2(screenPositions[conn.to].x, screenPositions[conn.to].y),
					lineColor,
					2.f
				);
			}
		}

		void g_String(const Vector2& position, const char* text, ImColor color)
		{
			ImGui::GetForegroundDrawList()->AddText(ImVec2(position.x, position.y), color, text);
		}


	}

	void g_CombatThread() {
		for (;;) {
		
			if (!pointers.local_pawn) {
				std::this_thread::sleep_for(std::chrono::milliseconds(16));
				continue;
			}

			WeaponCacheEntry weapon_entry;
			bool weapon_cached = g_cache.GetWeaponCache(weapon_entry);

			if (!weapon_cached) {
				
				uint64_t player_weapon = ReadMemory<uint64_t>(pointers.local_pawn + CURRENT_WEAPON);
				if (!player_weapon) {
					std::this_thread::sleep_for(std::chrono::milliseconds(16));
					continue;
				}

			
				uint64_t weapon_data = ReadMemory<uint64_t>(player_weapon + WEAPON_DATA);
				if (!weapon_data) {
					std::this_thread::sleep_for(std::chrono::milliseconds(16));
					continue;
				}

				uint64_t ftext_ptr = ReadMemory<uint64_t>(weapon_data + 0x40);
				if (!ftext_ptr) {
					std::this_thread::sleep_for(std::chrono::milliseconds(16));
					continue;
				}

				uint64_t ftext_data = ReadMemory<uint64_t>(ftext_ptr + 0x20);
				int ftext_length = ReadMemory<int>(ftext_ptr + 0x28);

				std::string weapon_name;
				if (ftext_length > 0 && ftext_length < 50) {
					wchar_t* ftext_buf = new wchar_t[ftext_length];
					PagefaultAPI::read_physical(reinterpret_cast<void*>(ftext_data), reinterpret_cast<uint8_t*>(ftext_buf), ftext_length * sizeof(wchar_t));
					std::this_thread::sleep_for(std::chrono::milliseconds(16));

					std::wstring wstr_buf(ftext_buf);
					weapon_name = std::string(wstr_buf.begin(), wstr_buf.end());
					LocalPlayerWeapon = weapon_name;
					delete[] ftext_buf;
				}
				else {
					std::this_thread::sleep_for(std::chrono::milliseconds(16));
					continue;
				}

				weapon_entry.weapon_type = EFortWeaponType::Rifle; // Default
				weapon_entry.projectile_speed = ReadMemory<float>(player_weapon + 0x220c);
				weapon_entry.projectile_gravity = ReadMemory<float>(player_weapon + 0x2210);
				weapon_entry.weapon_name = weapon_name;
				weapon_entry.last_updated = std::chrono::steady_clock::now();
				if (!weapon_name.empty()) {
					std::wstring w_weapon_name(weapon_name.begin(), weapon_name.end());
					const wchar_t* w_name = w_weapon_name.c_str();

					if (wcsstr(w_name, L"Assault Rifle") ||
						wcsstr(w_name, L"Havoc Suppressed Assault Rifle") ||
						wcsstr(w_name, L"Red-Eye Assault Rifle") ||
						wcsstr(w_name, L"Suppressed Assault Rifle") ||
						wcsstr(w_name, L"Striker Burst Rifle") ||
						wcsstr(w_name, L"Burst Assault Rifle") ||
						wcsstr(w_name, L"Ranger Assault Rifle") ||
						wcsstr(w_name, L"Flapjack Rifle") ||
						wcsstr(w_name, L"Heavy Assault Rifle") ||
						wcsstr(w_name, L"MK-Seven Assault Rifle") ||
						wcsstr(w_name, L"MK-Alpha Assault Rifle") ||
						wcsstr(w_name, L"Combat Assault Rifle") ||
						wcsstr(w_name, L"Nemesis AR") ||
						wcsstr(w_name, L"Ambush Striker AR") ||
						wcsstr(w_name, L"Striker AR") ||
						wcsstr(w_name, L"Tactical Assault Rifle") ||
						wcsstr(w_name, L"Hammer Assault Rifle") ||
						wcsstr(w_name, L"Sideways Rifle") ||
						wcsstr(w_name, L"Makeshift Rifle") ||
						wcsstr(w_name, L"Drum Gun")) {
						weapon_entry.weapon_type = EFortWeaponType::Rifle;
					}
					else if (wcsstr(w_name, L"Shotgun") ||
						wcsstr(w_name, L"Frenzy Auto Shotgun") ||
						wcsstr(w_name, L"Iron Warrior Hammer Pump Shotgun") ||
						wcsstr(w_name, L"Peter Griffin's Pump Shotgun") ||
						wcsstr(w_name, L"PETER GRIFFIN'S PUMP SHOTGUN")) {
						weapon_entry.weapon_type = EFortWeaponType::Shotgun;
					}
					else if (wcsstr(w_name, L"smg") ||
						wcsstr(w_name, L"Ambush Hyper SMG") ||
						wcsstr(w_name, L"Submachine Gun") ||
						wcsstr(w_name, L"Combat Smg") ||
						wcsstr(w_name, L"Pistol") ||
						wcsstr(w_name, L"Machine Smg") ||
						wcsstr(w_name, L"Scoped Burst SMG") ||
						wcsstr(w_name, L"Hyper SMG") ||
						wcsstr(w_name, L"THUNGER BURST SMG")) {
						weapon_entry.weapon_type = EFortWeaponType::Smg;
					}
					else if (wcsstr(w_name, L"Hunting Rifle") ||
						wcsstr(w_name, L"Heavy Sniper Rifle") ||
						wcsstr(w_name, L"Suppressed Sniper Rifle") ||
						wcsstr(w_name, L"Storm Scout") ||
						wcsstr(w_name, L"Bolt-Action Sniper Rifle") ||
						wcsstr(w_name, L"Automatic Sniper Rifle") ||
						wcsstr(w_name, L"DMR") ||
						wcsstr(w_name, L"Thermal DMR") ||
						wcsstr(w_name, L"Hunter Bolt-Action Sniper") ||
						wcsstr(w_name, L"Reaper Sniper Rifle") ||
						wcsstr(w_name, L"Semi-Auto Sniper Rifle") ||
						wcsstr(w_name, L"Crossbow") ||
						wcsstr(w_name, L"Mechanical Bow") ||
						wcsstr(w_name, L"Hand Cannon")) {
						weapon_entry.weapon_type = EFortWeaponType::Sniper;
					}
				}

				
				g_cache.UpdateWeaponCache(weapon_entry);
				HeldWeaponType = weapon_entry.weapon_type;
			}
			else {
				
				HeldWeaponType = weapon_entry.weapon_type;
				LocalPlayerWeapon = weapon_entry.weapon_name;
			}

			if (settings::aimbot::weapon_cnfg && pointers.local_pawn) {
				switch (HeldWeaponType) {
				case EFortWeaponType::Rifle:
					settings::aimbot::enable = settings::aimbot::rifle::aimbot;
					settings::aimbot::show_fov = settings::aimbot::rifle::render_fov;
					settings::aimbot::fov = settings::aimbot::rifle::aim_fov;
					settings::aimbot::smoothness = settings::aimbot::rifle::smooth;
					break;
				case EFortWeaponType::Shotgun:
					settings::aimbot::enable = settings::aimbot::shotgun::aimbot;
					settings::aimbot::show_fov = settings::aimbot::shotgun::render_fov;
					settings::aimbot::fov = settings::aimbot::shotgun::aim_fov;
					settings::aimbot::smoothness = settings::aimbot::shotgun::smooth;
					break;
				case EFortWeaponType::Smg:
					settings::aimbot::enable = settings::aimbot::smg::aimbot;
					settings::aimbot::show_fov = settings::aimbot::smg::render_fov;
					settings::aimbot::fov = settings::aimbot::smg::aim_fov;
					settings::aimbot::smoothness = settings::aimbot::smg::smooth;
					break;
				case EFortWeaponType::Sniper:
					settings::aimbot::enable = settings::aimbot::sniper::aimbot;
					settings::aimbot::show_fov = settings::aimbot::sniper::render_fov;
					settings::aimbot::fov = settings::aimbot::sniper::aim_fov;
					settings::aimbot::smoothness = settings::aimbot::sniper::smooth;
					break;
				default:
					// Disable aimbot if weapon type is unknown
					settings::aimbot::enable = false;
					break;
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}


}


#include <regex>


#include <chrono>
#include <cstdint>

void gLoop() {
	for (int i = 0; i < pointers.array_count; i++) {
		uintptr_t player_state = ReadMemory<uintptr_t>(pointers.player_array + (i * sizeof(uintptr_t)));
		if (!player_state) continue;

		PlayerCacheEntry cache_entry;
		bool is_cached = false;
		uintptr_t pawn_private = ReadMemory<uintptr_t>(player_state + PAWN_PRIVATE);
		if (!pawn_private || pawn_private == pointers.local_pawn) continue;

		if (g_cache.GetPlayerCache(pawn_private, cache_entry)) {
			is_cached = true;
		}
		else {
			cache_entry.player_state = player_state;
			cache_entry.team_id = ReadMemory<int>(player_state + TEAM_INDEX);
			if (cache_entry.team_id == pointers.my_team_id) continue;

			cache_entry.mesh = ReadMemory<uintptr_t>(pawn_private + MESH);
			if (!cache_entry.mesh) continue;

			cache_entry.is_despawning = (ReadMemory<char>(pawn_private + BIsDying) >> 5);
			cache_entry.head3d = EntityMesh->GetBoneMatrixByID(cache_entry.mesh, 110);
			cache_entry.bottom3d = EntityMesh->GetBoneMatrixByID(cache_entry.mesh, 0);
			cache_entry.head2d = ViewPoint::K_Project(Vector3(cache_entry.head3d.x, cache_entry.head3d.y, cache_entry.head3d.z + 20));
			cache_entry.bottom2d = ViewPoint::K_Project(cache_entry.bottom3d);
			cache_entry.username = SophieRain::g_functions::g_DecryptUsername(player_state);
			cache_entry.platform = SophieRain::g_functions::g_DecryptPlatform(player_state);
			cache_entry.weapon = SophieRain::g_functions::g_DecryptWeapon(pawn_private);
			cache_entry.last_updated = std::chrono::steady_clock::now();
		}

		if (cache_entry.is_despawning) {
			g_cache.InvalidatePlayerCache(pawn_private);
			continue;
		}

		float box_height = abs(cache_entry.head2d.y - cache_entry.bottom2d.y);
		float box_width = box_height * 0.50f;
		float distance = ViewPoint::Location.distance(cache_entry.bottom3d) / 100;

		if (!is_cached) {
			g_cache.UpdatePlayerCache(cache_entry);
		}

		if (settings::visuals::enable) {
			if (settings::visuals::weapon) {
				std::string weapon = cache_entry.weapon.empty() ? "[Materials/Unknown]" : cache_entry.weapon;
				ImVec2 text_size = ImGui::CalcTextSize(weapon.c_str());
				SophieRain::g_functions::g_String(cache_entry.head2d, weapon.c_str(), ImColor(255, 255, 255));
			}
			if (settings::visuals::line) {
				ImColor line_color = EntityMesh->IsMeshVisibile(cache_entry.mesh) ? ImColor(36, 207, 2, 250) : ImColor(250, 0, 0, 250);
				SophieRain::g_functions::g_DrawLineRender(cache_entry.bottom2d, line_color);
			}
			if (settings::visuals::Box) {
				ImColor box_color = EntityMesh->IsMeshVisibile(cache_entry.mesh) ? ImColor(36, 207, 2, 250) : ImColor(255, 0, 0, 250);
				if (settings::visuals::BoxType == 0) {
					SophieRain::g_functions::g_BoxRender(cache_entry.head2d.x - (box_width / 2), cache_entry.head2d.y, box_width, box_height, box_color, 1);
					if (settings::visuals::fill_box) SophieRain::g_functions::g_FilledRectRender(cache_entry.head2d.x - (box_width / 2), cache_entry.head2d.y, box_width, box_height, ImColor(0, 0, 0, 50));
				}
				else if (settings::visuals::BoxType == 1) {
					SophieRain::g_functions::g_CorneredBoxRender(cache_entry.head2d.x - (box_width / 2), cache_entry.head2d.y, box_width, box_height, box_color, 1);
					if (settings::visuals::fill_box) SophieRain::g_functions::g_FilledRectRender(cache_entry.head2d.x - (box_width / 2), cache_entry.head2d.y, box_width, box_height, ImColor(0, 0, 0, 50));
				}
			}
			if (settings::visuals::fill_box) {
				ImColor fill_color = EntityMesh->IsMeshVisibile(cache_entry.mesh) ? ImColor(36, 207, 2, 100) : ImColor(250, 0, 0, 100);
				SophieRain::g_functions::g_FilledRectRender(cache_entry.head2d.x - (box_width / 2), cache_entry.head2d.y, box_width, box_height, fill_color);
			}
			if (settings::visuals::skeleton) {
				SophieRain::g_functions::g_Skeleton(cache_entry.mesh, distance);
			}
			if (settings::visuals::username) {
				ImColor text_color = EntityMesh->IsMeshVisibile(cache_entry.mesh) ? ImColor(255, 255, 255) : ImColor(250, 0, 0, 255);
				Vector2 pos = cache_entry.head2d;
				pos.y -= 8.0f;
				SophieRain::g_functions::g_String(pos, cache_entry.username.c_str(), text_color);
			}
			if (settings::visuals::platform) {
				ImColor text_color = EntityMesh->IsMeshVisibile(cache_entry.mesh) ? ImColor(255, 255, 255) : ImColor(250, 0, 0, 250);
				SophieRain::g_functions::g_String(cache_entry.head2d, cache_entry.platform.c_str(), text_color);
			}
			if (settings::visuals::distance) {
				ImColor dist_color = EntityMesh->IsMeshVisibile(cache_entry.mesh) ? ImColor(255, 255, 255, 250) : ImColor(255, 0, 0, 250);
				SophieRain::g_functions::g_DistanceRender(cache_entry.bottom2d, distance, dist_color);
			}
		}
		// [[ Definetly cood look better, but it works (its probably best to lower it in height a little bit  lmfao ]]
		if (settings::radar::radar) {
			ImGui::SetNextWindowPos(ImVec2(10, 5));
			ImGui::SetNextWindowSize(ImVec2(200, 200));
			if (ImGui::Begin("morva999", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground)) {
				ImVec2 p = ImVec2(ImGui::GetWindowPos().x + ImGui::GetStyle().WindowPadding.x, ImGui::GetWindowPos().y + ImGui::GetStyle().WindowPadding.y);
				ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(p.x, p.y), ImVec2(p.x + 200, p.y + 200), ImColor(13, 11, 13, 155));
				ImVec2 winsize = ImGui::GetWindowSize();
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(10 + winsize.x * 0.5f, 10), ImVec2(10 + winsize.x * 0.5f, 10 + winsize.y), ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(10, 10 + winsize.y * 0.5f), ImVec2(10 + winsize.x, 10 + winsize.y * 0.5f), ImGui::GetColorU32(ImGuiCol_Border), 1.0f);
				ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(10 + winsize.x * 0.5f, 10 + winsize.y * 0.5f), 2.0f, ImColor(255, 255, 255, 255), 64);

				int ScreenX, ScreenY = 0;
				project_world_to_radar(cache_entry.bottom3d, ScreenX, ScreenY, ImVec2(10, 10));
				ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(ScreenX, ScreenY), 2.0f, ImColor(255, 255, 255), 64);
			}
			ImGui::End();
		}

		if (settings::aimbot::trigger_bot) {
			if (GetAsyncKeyState(settings::triggerbot::trigger_key) & 0x8000) {
				uintptr_t* targeted_pawn = ReadMemory<uintptr_t*>(pointers.player_controller + TargetedFortPawn);
				if (!targeted_pawn) continue;

				auto now = std::chrono::steady_clock::now();
				if (settings::triggerbot::has_clicked) {
					settings::triggerbot::tb_begin = now;
					settings::triggerbot::has_clicked = false;
				}

				settings::triggerbot::tb_end = now;
				settings::triggerbot::tb_time_since = std::chrono::duration_cast<std::chrono::milliseconds>(
					settings::triggerbot::tb_end - settings::triggerbot::tb_begin).count();

				if (settings::triggerbot::tb_time_since >= settings::triggerbot::Delay) {
					WeaponCacheEntry weapon_entry;
					bool weapon_cached = g_cache.GetWeaponCache(weapon_entry);
					int current_weapon_type = weapon_cached ? weapon_entry.weapon_type : HeldWeaponType;

					if (settings::triggerbot::shotgun_only) {
						if (current_weapon_type == EFortWeaponType::Shotgun) {
							mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
							mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
						}
					}
					else {
						mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
						mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
					}
					settings::triggerbot::has_clicked = true;
				}
			}
		}

		double dx = cache_entry.head2d.x - settings::screen_center_x;
		double dy = cache_entry.head2d.y - settings::screen_center_y;
		float dist = sqrtf(dx * dx + dy * dy);
		if (dist <= settings::aimbot::fov && dist < pointers.closest_distance) {
			pointers.closest_distance = dist;
			pointers.closest_mesh = cache_entry.mesh;
		}
	}

	if (settings::aimbot::enable) {
		if (GetAsyncKeyState(settings::aimbot::current_key)) {
			uintptr_t target_mesh = pointers.closest_mesh;
			if (!target_mesh || !EntityMesh->IsMeshVisibile(target_mesh)) return;

			Vector3 head3d = EntityMesh->GetBoneMatrixByID(target_mesh, 110);
			Vector2 head2d = ViewPoint::K_Project(head3d);
			Vector3 Velocity = ReadMemory<Vector3>(pointers.root_component + VELOCITY);
			Vector3 targets{};

			if (head2d.x != 0) {
				if (head2d.x > settings::screen_center_x) {
					targets.x = -(settings::screen_center_x - head2d.x);
					targets.x /= settings::aimbot::smoothness;
					if (targets.x + settings::screen_center_x > settings::screen_center_x * 2) targets.x = 0;
				}
				if (head2d.x < settings::screen_center_x) {
					targets.x = head2d.x - settings::screen_center_x;
					targets.x /= settings::aimbot::smoothness;
					if (targets.x + settings::screen_center_x < 0) targets.x = 0;
				}
			}
			if (head2d.y != 0) {
				if (head2d.y > settings::screen_center_y) {
					targets.y = -(settings::screen_center_y - head2d.y);
					targets.y /= settings::aimbot::smoothness;
					if (targets.y + settings::screen_center_y > settings::screen_center_y * 2) targets.y = 0;
				}
				if (head2d.y < settings::screen_center_y) {
					targets.y = head2d.y - settings::screen_center_y;
					targets.y /= settings::aimbot::smoothness;
					if (targets.y + settings::screen_center_y < 0) targets.y = 0;
				}
			}

			WeaponCacheEntry weapon_entry;
			bool weapon_cached = g_cache.GetWeaponCache(weapon_entry);
			float speed = weapon_cached ? weapon_entry.projectile_speed : ReadMemory<float>(ReadMemory<uintptr_t>(pointers.local_pawn + CURRENT_WEAPON) + 0x220c);
			float gravity = weapon_cached ? weapon_entry.projectile_gravity : ReadMemory<float>(ReadMemory<uintptr_t>(pointers.local_pawn + CURRENT_WEAPON) + 0x2210);

			if (settings::aimbot::projectile_predict && speed > 1.f && !Velocity.IsNearlyZero()) {
				Vector3 bottom3d = EntityMesh->GetBoneMatrixByID(target_mesh, 0);
				float distance = ViewPoint::Location.distance(bottom3d) / 100;
				targets = prediction::PredictLocation(head3d, Velocity, speed, gravity, distance);
			}
			input::move_mouse(targets.x, targets.y);
		}
	}

	if (settings::Features::crosshair) {
		ImVec2 center = ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
		ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		float size = 10.0f;
		DrawCrosshair(center, color, size);
	}
}





HWND get_process_wnd(uint32_t pid)
{
	std::pair<HWND, uint32_t> params = { 0, pid };
	BOOL bresult = EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL
	{
		auto pparams = (std::pair<HWND, uint32_t>*)(lparam);
		uint32_t processid = 0;
		if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processid)) && processid == pparams->second)
		{
			SetLastError((uint32_t)-1);
			pparams->first = hwnd;
			return FALSE;
		}
		return TRUE;
	}, (LPARAM)&params);
	if (!bresult && GetLastError() == -1 && params.first) return params.first;
	return 0;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_SIZE:
		if (p_device != nullptr && wParam != SIZE_MINIMIZED) {
			if (g_mainRenderTargetView) {
				g_mainRenderTargetView->Release();
				g_mainRenderTargetView = nullptr;
			}
			p_swap_chain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			ID3D11Texture2D* pBackBuffer = nullptr;
			if (SUCCEEDED(p_swap_chain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)))) {
				p_device->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
				pBackBuffer->Release();
			}
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

void watemr() {
	// Grab screen size, we goin' full Ohio screen mode 🖥️
#ifdef _WIN32
	int screen_width = GetSystemMetrics(SM_CXSCREEN);  // Screen width, straight W
	int screen_height = GetSystemMetrics(SM_CYSCREEN); // Screen height, no Ohio Ls
#else
	// Fallback for non-Windows, u on Linux or smth? GYATT energy
	int screen_width = 1920;
	int screen_height = 1080;
#endif

	// Calculate FPS, we countin' frames like a TikTok trend 📈
	static float last_time = ImGui::GetTime();
	static int frame_count = 0;
	static float fps = 0.0f;
	frame_count++;
	float current_time = ImGui::GetTime();
	if (current_time - last_time >= 1.0f) {
		fps = frame_count / (current_time - last_time);
		frame_count = 0;
		last_time = current_time;
	}

	// Set position to top-left, no Ohio fumbles
	ImGui::SetNextWindowPos(ImVec2(10, 10)); // Top-left corner, Skibidi toilet vibes
	ImGui::SetNextWindowBgAlpha(0.0f); // No background, we stayin' clean fr

	// Window flags to keep it drippy, no GYATT clutter
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav;

	// Draw the text, makin' it THICC with shadows, Gen Alpha energy 💪
	ImGui::Begin("##watermark", nullptr, window_flags);

	// Multi-layer shadow for that extra thiccness, no Ohio Ls
	ImGui::SetCursorPos(ImVec2(11, 11)); // Shadow 1
	ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 0.9f), "Memorium Slotted [ FPS : %.1f ]", fps);
	ImGui::SetCursorPos(ImVec2(12, 12)); // Shadow 2
	ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 0.9f), "Memorium Slotted [ FPS : %.1f ]", fps);
	ImGui::SetCursorPos(ImVec2(10, 12)); // Shadow 3
	ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 0.9f), "Memorium Slotted [ FPS : %.1f ]", fps);
	ImGui::SetCursorPos(ImVec2(12, 10)); // Shadow 4
	ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 0.9f), "Memorium Slotted [ FPS : %.1f ]", fps);

	// Main text, lighter purple for readability, we eatin' good fr 💜
	ImGui::SetCursorPos(ImVec2(10, 10)); // Reset position
	ImGui::TextColored(ImVec4(0.69f, 0.35f, 1.0f, 1.0f), "Memorium Slotted [ FPS : %.1f ]", fps); // Brighter purple, maximum Rizz

	ImGui::End();
}
void Loop() {
	std::cout << "[Loop] Starting main loop\n";
	static bool show_menu = true;

	while (messager.message != WM_QUIT) {
		if (!UpdateWindow(my_wnd) || !ShowWindow(my_wnd, SW_SHOW)) {
			std::cout << "[Loop] Failed to update/show window\n";
			break;
		}

		if (PeekMessage(&messager, my_wnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&messager);
			DispatchMessage(&messager);
		}

		ImGuiIO& io = ImGui::GetIO();
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		viewport->PlatformHandleRaw = (void*)my_wnd;

		io.DeltaTime = 1.0f / 60.0f;

		CURSORINFO cursorInfo;
		cursorInfo.cbSize = sizeof(CURSORINFO);
		GetCursorInfo(&cursorInfo);
		io.MousePos.x = cursorInfo.ptScreenPos.x;
		io.MousePos.y = cursorInfo.ptScreenPos.y;

		if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].y = io.MousePos.y;
		}
		else {
			io.MouseDown[0] = false;
		}

		if (GetAsyncKeyState(VK_F8) & 1) {
			settings::show_menu = !settings::show_menu;
		//	std::cout << "[Loop] Show menu: " << show_menu << "\n";
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		gRenderMenu();
		gLoop();
		if (settings::aimbot::show_fov)
		{
			if (settings::aimbot::dynamic_fov) {

				ImDrawList* draw_list = ImGui::GetForegroundDrawList();
				float ViewPoint_SCale = ViewPoint::FieldOfView;
				float halfScreenWidth = settings::width;
				float fovScale = tanf(10.0f * 0.5f * M_PI / 90.0f) / tanf(ViewPoint_SCale * 0.5f * M_PI / 200.0f);
				settings::aimbot::fov = halfScreenWidth * fovScale;

				draw_list->AddCircle(ImVec2(settings::screen_center_x, settings::screen_center_y), settings::aimbot::fov, ImColor(255, 255, 255), 50, 1);

			}
			else {
				ImGui::GetForegroundDrawList()->AddCircle(ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2), settings::aimbot::fov, ImColor(250, 250, 250, 250), 100, 1.0f);
			}
		}
		watemr();

		ImGui::Render();
		if (!g_mainRenderTargetView || !p_device_context || !p_swap_chain) {
			std::cout << "[Loop] Invalid render target view, device context, or swap chain\n";
			break;
		}
		p_device_context->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // Try { 1.0f, 1.0f, 1.0f, 1.0f } for testing
		p_device_context->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		HRESULT present_result = p_swap_chain->Present(1, 0);
		if (FAILED(present_result)) {
			std::cout << "[Loop] Present failed: 0x" << std::hex << present_result << "\n";
			break;
		}
	}

	std::cout << "[Loop] Cleaning up\n";
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (g_mainRenderTargetView) {
		g_mainRenderTargetView->Release();
		g_mainRenderTargetView = nullptr;
	}
	if (p_swap_chain) {
		p_swap_chain->Release();
		p_swap_chain = nullptr;
	}
	if (p_device_context) {
		p_device_context->Release();
		p_device_context = nullptr;
	}
	if (p_device) {
		p_device->Release();
		p_device = nullptr;
	}
	if (my_wnd) {
		DestroyWindow(my_wnd);
		my_wnd = nullptr;
	}
	std::cout << "[Loop] Exited main loop\n";
}