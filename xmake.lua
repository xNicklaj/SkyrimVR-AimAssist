add_rules("mode.debug", "mode.release")
add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})

set_project("AimAssistVR")
set_version("1.0.0")
set_languages("cxx23")

includes("extern/CommonLibVR")

add_requires("imgui v1.89", {configs = {dx11 = true, win32 = true}})

target("AimAssistVR")
    add_rules("commonlibsse-ng.plugin", {
        name = "AimAssistVR",
        author = "Author",
        description = "Dynamic aim assist for projectiles in Skyrim VR"
    })
    
    set_kind("shared")
    set_symbols("debug")
    set_targetdir("build")
    
    add_files("src/*.cpp")
    add_includedirs("src")
    add_includedirs("extern/SKSEMenuFramework/include")
    
    add_packages("imgui")
