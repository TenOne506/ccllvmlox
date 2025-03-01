add_rules("mode.debug", "mode.release")
set_toolchains("clang",{debugger = "lldb"})
set_toolset("cc", "clang")
set_toolset("cxx", "clang++")
-- add_requires("llvm")
-- Enable clang-tidy checks
before_build(function (target)
            -- 获取 LLVM 编译和链接标志
            local llvm_cflags = os.iorun("llvm-config --cxxflags --ldflags --system-libs --libs core mcjit native")  -- 获取 C++ 编译标志
            local llvm_ldflags = os.iorun("llvm-config --ldflags --system-libs --libs core")  -- 获取链接标志
            local llvm_libs = os.iorun("llvm-config --libs all")  -- 获取所有必需的库
            -- Clean up library names by removing duplicate -l prefixes
            local cleaned_libs = {}
            for lib in string.gmatch(llvm_libs, "-l(%S+)") do
                table.insert(cleaned_libs, lib)
            end
            -- print("llvm_cflags: " .. llvm_cflags)
            -- print("llvm_ldflags: " .. llvm_ldflags)
            --target:add("cxflags", llvm_cflags)  -- 添加 LLVM 编译标志
            --target:add("ldflags", llvm_ldflags)  -- 添加 LLVM 链接标志
            target:add("links", cleaned_libs)  -- 添加清理后的 LLVM 库
            -- 手动指定核心库，尝试先链接核心库
            -- target:add("links", "LLVMCore", "LLVMSupport", "LLVMBitstreamReader", "LLVMRemarks")  
            -- 添加需要链接的核心库
        end)

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
set_languages("c++20")
-- Configure clang-tidy options
add_cxxflags("-fexceptions")
--add_cxxflags(" -w  -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING=1")
set_policy("build.sanitizer.address", false)
target("lox")
    set_kind("binary")
    add_includedirs("include")
    --add_cxflags("-Wall -Wextra -Werror",{force=true})
    if is_mode("debug") then
        -- 在debug模式下添加-g标志以包含调试信息
        ---gdwarf-4 是为了兼容低版本的varlgrind
        --add_cxflags("-g -gdwarf-4")
        -- add_cxflags("-fsanitize=address")  -- 启用 ASAN
        -- add_ldflags("-fsanitize=address",{force = true})  -- 链接 ASAN
        add_cxflags("-g")
        -- 可选：禁用优化
        add_cxflags("-O0")
        -- 可选：定义DEBUG宏
        -- add_defines("DEBUG")
    end
    add_files("src/main.cpp")
    -- add_files("src/Lox/*.cpp")
    -- add_files("src/frontend/*.cpp")

    add_files("src/**/*.cpp") -- 递归添加src目录及其所有子目录下的.cpp文件
    set_languages("c++20")
    -- 在编译前运行 clang-tidy 检查
    -- before_build(function (target)
    --     print("Running clang-tidy...")
    --     os.exec("xmake check clang.tidy --configfile=.clang-tidy ")
    -- end)
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


-- cmake -S llvm -B build -G Ninja  \
--   -DLLVM_ENABLE_PROJECTS='clang' \
--   -DLLVM_TARGETS_TO_BUILD="Native;NVPTX" \
--   -DLLVM_ENABLE_RTTI=ON \
--   -DCMAKE_BUILD_TYPE=Debug \
--   -DLLVM_ENABLE_RUNTIMES="compiler-rt"