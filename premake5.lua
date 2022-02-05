---@diagnostic disable: undefined-global
 



newaction {
   trigger = "clean",
   description = "clean build files",
   execute = function ()
      print("Cleaing build files...")
      local dirs = os.matchdirs("**")
      for k, v in pairs(dirs) do
         if string.contains(v,"build") then
            os.rmdir(v)
         end
      end
      print("all build files have been cleaned")
   end
}

--copy folder and all its content from src to dst 
function CopyFolder(src,dst)
   local fm = os.matchfiles( src .. "/**")
   local hm = os.matchdirs(src .."/**")
   --os.mkdir("path")
   for i, v in pairs(hm) do os.mkdir(dst .. "/".. v )  end
   for i, v in pairs(fm) do os.copyfile(v, dst .. "/" .. v) end
end

BuidName = ""
Build_Actions = "gmake gmake2 vs2005 vs2008 vs2010 vs2012 vs2013 vs2015 vs2017 vs2019 vs2022 xcode4"

if _ACTION ~= nil then
   ActionIsOk = string.find(Build_Actions, _ACTION)
end

-- produce numbred builds in case of reproduction
if ActionIsOk then
   BuidName = _ACTION .. "-build"
   local hm = os.matchdirs(_ACTION.."**")
   if rawlen(hm) > 0 then
      BuidName = _ACTION .. "-build" .. tostring(rawlen(hm))
   end
   CopyFolder("src",BuidName)
   --CopyFolder("assets",BuidName)
end

workspace "HelloWorld"
   configurations {"Debug","Release"}
   --architecture "x64"
   system("windows")
   filename "GLFW Startup" --.sln
   location (BuidName)
   architecture "x64"

project "HelloWorld"
   kind "ConsoleApp"
   language "C++"
   cppdialect "C++11"
   targetdir ( BuidName .. "/bin/%{cfg.buildcfg}/%{prj.name}")
   objdir (BuidName .."/ints")
   
   --includedirs{"%{prj.location}/src/GLFW"}
   --includedirs { BuidName.."/src/assets" }

   files {BuidName .."/src/**.h", BuidName .."/src/**.cpp"}
   links{ "glfw3","opengl32" }
   
   filter "action:vs*"
      libdirs { BuidName .."/src/NMC/GLFW/lib"}

   filter "action:gmake*"
      links{ "comdlg32","ole32" }
      libdirs { BuidName .."/src/NMC/GLFW/dll"}

      -- use static runtime library on GCC builds
      buildoptions "-static-libstdc++"
      postbuildcommands {
         "{COPY} src/NMC/GLFW/dll/glfw3.dll %{cfg.targetdir}"
       }

   filter "configurations:Debug"
      runtime "Debug"
      defines {"Debug"}
      optimize "On"
      symbols "On"

   filter "configurations:Release"
      runtime "Release"
      defines {"NDEBUG"}
      optimize "On"
      symbols "Off"

   -- use static runtime library on visual studio builds
   filter { "action:vs*", "configurations:Debug" }
      buildoptions "/MTd"
   filter { "action:vs*", "configurations:Release" }
      buildoptions "/MT"

   filter "system:Windows"
      systemversion "latest" -- To use the latest version of the SDK available
