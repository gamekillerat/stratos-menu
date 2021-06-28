#pragma once
extern "C" {
# include <lua.h>
# include <lauxlib.h>
# include <lualib.h>
}
#include <LuaBridge/LuaBridge.h>
#ifdef NDEBUG
#pragma comment(lib, "Lua52.lib")
#else
#pragma comment(lib, "Lua52d.lib")
#endif

using namespace luabridge;

class Lua
{
public:
    void init(lua_State* L)
    {
        L = luaL_newstate();
        luaL_openlibs(L);
    }
    LuaRef Get_Global(lua_State* L, std::string name)
    {
        return getGlobal(L, name.c_str());
    }
    bool Do_File(lua_State* L, std::string path)
    {
        return luaL_loadfilex(L, path.c_str(), NULL) || lua_pcall(L, 0, LUA_MULTRET, 0);
    }
    int Pcall(lua_State* L, int args = 0, int results = 0, int errfunc = 0)
    {
        return lua_pcallk(L, args, results, errfunc, 0, NULL);
    }
    Namespace Get_Global_Namespace(lua_State* L)
    {
        return getGlobalNamespace(L);
    }
    lua_State* Get_State(lua_State* L)
    {
        return L;
    }
} inline g_lua;

class LuaScript
{
public:
    void State_Init()
    {
        L = luaL_newstate();
        luaL_openlibs(L);
    }
    void init() {
        LuaRef Init = getGlobal(L, ("init"));
        if (Init.isNil())
        {
            // TO LOG(("No init function"));
            return;
        }
        Init();
    }
    void uninit() {
        LuaRef Uninit = getGlobal(L, ("uninit"));
        if (Uninit.isNil())
        {
            // TO LOG(("No uninit function"));
            return;
        }
        Uninit();
    }
    LuaRef Get_Global(char const* name)
    {
        return getGlobal(L, name);
    }
    void Set_Path(std::string new_path)
    {
        path = new_path;
    }
    std::string Get_Path()
    {
        return path;
    }
    void Set_Name(std::string new_path)
    {
        name = new_path;
    }
    std::string Get_Name()
    {
        return name;
    }
    int Pcall(int args = 0, int results = 0, int errfunc = 0)
    {
        return lua_pcallk(L, args, results, errfunc, 0, NULL);
    }
    Namespace Get_Global_Namespace()
    {
        return getGlobalNamespace(L);
    }
    lua_State* Get_State()
    {
        return L;
    }
    bool Do_File()
    {
        return luaL_loadfilex(L, path.c_str(), NULL) || lua_pcall(L, 0, LUA_MULTRET, 0);
    }
    void Set_Init_Status(bool togl)
    {
        inited = togl;
    }
    bool Get_Init_Status()
    {
        return inited;
    }
private:
    std::string path;
    std::string name;
    bool inited = false;
    lua_State* L = nullptr;
};
inline std::map<std::string, LuaScript> Luas;