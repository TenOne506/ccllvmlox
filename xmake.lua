add_rules("mode.debug", "mode.release")
set_toolchains("clang")
set_toolset("cc", "clang")
set_toolset("cxx", "clang++")
-- add_requires("llvm")
-- Enable clang-tidy checks

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
-- Configure clang-tidy options
set_languages("c++17")
target("test")
    set_kind("binary")
    add_files("src/*.cpp")
    -- 在编译前运行 clang-tidy 检查
    before_build(function (target)
        print("Running clang-tidy...")
        os.exec("xmake check clang.tidy --configfile=.clang-tidy ")
    end)
    -- Generate compile_commands.json
    -- set_policy("build.merge_archive", false)
    -- set_policy("build.warning", true)
    -- add_rules("utils.install.cmake_importfiles")
    -- add_rules("utils.install.pkgconfig_importfiles")
    
    -- -- Add clang-tidy check before build
    -- on_build(function (target)
    --     local clang_tidy = "clang-tidy"
    --     local source_files = target:sourcefiles()
    --     local compile_commands = path.join(target:targetdir(), "compile_commands.json")
        
    --     -- Generate compile commands if not exists
    --     if not os.isfile(compile_commands) then
    --         os.run("xmake project -k compile_commands")
    --     end
        
    --     for _, file in ipairs(source_files) do
    --         os.runv(clang_tidy, {
    --             file,
    --             "--checks=bugprone-*,clang-analyzer-*,modernize-*,readability-*,cppcoreguidelines-*,performance-*",
    --             "--header-filter=.*",
    --             "--quiet",
    --             "-p", target:targetdir()
    --         })
    --     end 
    -- end)
