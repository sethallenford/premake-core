/**
 * \file   premake_main.c
 * \brief  Program entry point.
 * \author Copyright (c) 2002-2013 Jason Perkins and the Premake project
 */

#include "premake.h"
#include "luaprofiler/luaprofiler.h"
#include <string.h>
#include <stdlib.h>

void setup_profiler(int argc, const char** argv)
{
	int i;
	for (i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "/profile") == 0 || strcmp(argv[i], "--profile") == 0)
		{
			init_profiler();
			return;
		}
	}
}

static void* profile_alloc(void* ud, void* ptr, size_t osize, size_t nsize)
{
	(void)ud;
	(void)osize;

	void* result;

	// free.
	if (nsize == 0)
	{
		if (ptr != NULL)
		{
			free_callback((uintptr_t)ptr);
			free(ptr);
		}
		return NULL;
	}

	// malloc.
	if (ptr == NULL)
	{
		result = malloc(nsize);
		malloc_callback((uintptr_t)result, nsize);
		return result;
	}

	// realloc.
	result = realloc(ptr, nsize);
	realloc_callback((uintptr_t)result, (uintptr_t)ptr, nsize);
	return result;
}


namespace fs = std::filesystem;

void load_lua_scripts(lua_State* L, const std::filesystem::path& dir_path, const std::string& addon_name = "") {
	if (!std::filesystem::exists(dir_path) || !std::filesystem::is_directory(dir_path)) {
		std::cerr << "Error: Provided path does not exist or is not a directory: " << dir_path << std::endl;
		return;
	}

	for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
		if (entry.is_regular_file() && entry.path().extension() == ".lua") {
			std::string script_path = entry.path().string();
			std::cout << "Loading Lua script: " << script_path << std::endl;
			luaL_dofile(L, script_path.c_str());
		}
		else if (entry.is_directory() && !addon_name.empty() && entry.path().filename() == addon_name) {
			load_lua_scripts(L, entry.path());
		}
	}
}


int main(int argc, const char** argv)
{
	lua_State* L;
	int z;

	setup_profiler(argc, argv);

	L = lua_newstate(profile_alloc, NULL);
	luaL_openlibs(L);

	// Load all Lua files in the specified directory
	fs::path dir_path = "C:\\Users\\Administrator\\OneDrive\\Documents\\WindowsProject1\\WindowsProject1\\BlizzardInterfaceCode";
	load_lua_scripts(L, dir_path);

	// Add custom search path for Lua modules
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "path");
	std::string current_path = lua_tostring(L, -1);
	current_path.append(";");
	current_path.append("C:\\Users\\Administrator\\OneDrive\\Documents\\WindowsProject1\\WindowsProject1\\BlizzardInterfaceCode\\Interface\\AddOns\\?\\?.lua");
	lua_pop(L, 1);
	lua_pushstring(L, current_path.c_str());
	lua_setfield(L, -2, "path");
	lua_pop(L, 1);

	// Load the Blizzard_DebugTools addon
	load_lua_scripts(L, dir_path, "Blizzard_DebugTools");

	// Execute a Lua script
	const char* script = "print('Hello, World!')";
	int status = luaL_dostring(L, script);

	if (status != LUA_OK) {
		std::cerr << "Error executing 'Hello, World!' script: " << lua_tostring(L, -1) << std::endl;
	}

	// Load and run Blizzard_DebugTools.lua
	luaL_dofile(L, "C:\\Users\\Administrator\\OneDrive\\Documents\\WindowsProject1\\WindowsProject1\\BlizzardInterfaceCode\\Interface\\AddOns\\Blizzard_DebugTools\\Blizzard_DebugTools.lua");

	// Load and run main.lua
	luaL_dofile(L, "C:\\Users\\Administrator\\OneDrive\\Documents\\WindowsProject1\\WindowsProject1\\main.lua");


	if (status != LUA_OK) {
		std::cerr << "Error executing main.lua: " << lua_tostring(L, -1) << std::endl;
	}


	hook_profiler(L);

	z = premake_init(L);

	if (z == OKAY) {
		z = premake_execute(L, argc, argv, "src/_premake_main.lua");
	}

	shutdown_profiler();

	return z;
}
