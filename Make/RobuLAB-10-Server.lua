project "RobuLABServer"
  kind "ConsoleApp"
  language "C++"
  targetdir "../dist/"
  files{ "../Source/**.cpp", "../Source/**.h"}

  includedirs {
	"../Source/"
  }

  links {
    "glib-2.0",
	"gio-2.0",
	"gobject-2.0"}
