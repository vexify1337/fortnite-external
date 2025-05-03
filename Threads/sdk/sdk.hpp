#pragma once
#include <d3d9.h>
#include <vector>
#include <numbers>
#include "mathmatics.h"
#include <thread>
class CachePointers {
public:
	uintptr_t uworld;
	uintptr_t game_instance;
	uintptr_t local_players;
	uintptr_t player_controller;
	uintptr_t local_pawn;
	uintptr_t root_component;
	uintptr_t player_state;
	uintptr_t pawn_private;
	uintptr_t HimPlayerState;
	int my_team_id;
	uintptr_t game_state;
	uintptr_t player_array;
	int array_count;
	float closest_distance;
	uintptr_t closest_mesh;

}; CachePointers pointers;




namespace ViewPoint {
	Vector3 Location;
	Vector3 Rotation;
	float FieldOfView;
	void g_CameraThread()
	{
		for (;;) {
			uintptr_t location_pointer = ReadMemory<uintptr_t>(pointers.uworld + Camera_Location); // camera location offset
			uintptr_t rotation_pointer = ReadMemory<uintptr_t>(pointers.uworld + Camera_Rotation);// camera rotation offset
			FNRot fnrot{};
			fnrot.a = ReadMemory<double>(rotation_pointer);
			fnrot.b = ReadMemory<double>(rotation_pointer + 0x20);
			fnrot.c = ReadMemory<double>(rotation_pointer + 0x1D0);
			Location = ReadMemory<Vector3>(location_pointer);
			Rotation.x = asin(fnrot.c) * (180.0 / M_PI);
			Rotation.y = ((atan2(fnrot.a * -1, fnrot.b) * (180.0 / M_PI)) * -1) * -1;
			FieldOfView = ReadMemory<float>(pointers.player_controller + 0x3AC) * 90.0f;
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		
	}


	Vector2 K_Project(Vector3 world_location)
	{
		// we would usually call view point here, but instead imam just thread it at the start/.
		D3DMATRIX temp_matrix = to_matrix(ViewPoint::Rotation);
		Vector3 vaxisx = Vector3(temp_matrix.m[0][0], temp_matrix.m[0][1], temp_matrix.m[0][2]);
		Vector3 vaxisy = Vector3(temp_matrix.m[1][0], temp_matrix.m[1][1], temp_matrix.m[1][2]);
		Vector3 vaxisz = Vector3(temp_matrix.m[2][0], temp_matrix.m[2][1], temp_matrix.m[2][2]);
		Vector3 vdelta = world_location - ViewPoint::Location;
		Vector3 vtransformed = Vector3(vdelta.dot(vaxisy), vdelta.dot(vaxisz), vdelta.dot(vaxisx));
		if (vtransformed.z < 1) vtransformed.z = 1;
		return Vector2(settings::screen_center_x + vtransformed.x * ((settings::screen_center_x / tanf(ViewPoint::FieldOfView * M_PI / 360))) / vtransformed.z, settings::screen_center_y - vtransformed.y * ((settings::screen_center_x / tanf(ViewPoint::FieldOfView * M_PI / 360))) / vtransformed.z);
	}
}

struct EntityMeshC {
public:
	Vector3 GetBoneMatrixByID(uintptr_t mesh, int bone_id)
	{
		uintptr_t bone_array = ReadMemory<uintptr_t>(mesh + BONE_ARRAY);
		if (bone_array == 0) bone_array = ReadMemory<uintptr_t>(mesh + BONE_ARRAY_CACHE);
		FTransform bone = ReadMemory<FTransform>(bone_array + (bone_id * 0x60));
		FTransform component_to_world = ReadMemory<FTransform>(mesh + COMPONENT_TO_WORLD);
		D3DMATRIX matrix = matrix_multiplication(bone.to_matrix_with_scale(), component_to_world.to_matrix_with_scale());
		return Vector3(matrix._41, matrix._42, matrix._43);
	}
	bool IsMeshVisibile(uintptr_t mesh)
	{
		auto Seconds = ReadMemory<double>(pointers.uworld+ WorldSeconds);
		auto LastRenderTime = ReadMemory<float>(mesh + 0x32C);
		return Seconds - LastRenderTime <= 0.06f;
	}


} ;static EntityMeshC* EntityMesh = new EntityMeshC();
