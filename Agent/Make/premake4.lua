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

  includedirs {
     "../../Extern/include/",
     "../../Extern/include/glib-2.0/",
     "../../Extern/lib/glib-2.0/include/"
  }
  
  libdirs {
	 "../../Extern/lib/"
	}
  
   -- debug configuration
  configuration { "Debug" }
	defines { "DEBUG" }
	flags { "Symbols" }
	
  links {
    "glib-2.0",
	"gio-2.0",
	"gobject-2.0"
	}
	
  -- store the makefiles the Make/ directory
  location "."
  
  
  -- add projects
  dofile "RobuLAB-10-Server.lua"
