dofile ("../../Utils/Lua/qtcreator.lua")
dofile ("../../Utils/Lua/qtcreator_2.7+.lua")


solution "RobuLABServer"	  
  platforms {"Native"}
  configurations {"Debug","Release"}
  
  if os.is("windows") then
	defines {"WIN32", "NOMINMAX"}
    buildoptions {"/wd4351", -- disable warning: "...new behavior: elements of array..."
				  "/wd4996", -- disable warning: "...deprecated..."
				  "/wd4290"} -- exception specification ignored (typed stecifications are ignored)
	links {"ws2_32"}
  end

  buildoptions {
    "$(shell pkg-config --cflags glib-2.0)"
  }

  
  linkoptions {
    "$(shell pkg-config --libs glib-2.0)"
  }

  includedirs {
     "/usr/include/",
     "/usr/include/glib-2.0/",
     "/usr/lib/glib-2.0/include/"
  }
  
  libdirs {
	"/usr/lib/"
	}
  
   -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }
	
  links {
    "glib-2.0",
	"gio-2.0",
	"gobject-2.0"}
	
  -- store the makefiles the Make/ directory
  location "../build"
  
  
  -- add projects
  dofile "RobuLAB-10-Server.lua"
