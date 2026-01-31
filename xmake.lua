add_rules("mode.debug", "mode.release")

if is_plat("windows") then
    set_toolchains("msvc")
    if not has_config("vs_runtime") then
        set_runtimes("MD")
    end
else
    set_toolchains("clang")
end

option("kind")
    set_default("static")
    set_values("static", "shared")
    set_showmenu(true)
option_end()

option("enable_test")
    set_default(false)
    set_values(true, false)
    set_showmenu(true)
option_end()

target("jsonc")
    set_kind("$(kind)")
    set_languages("c++23")
    if is_config("kind", "shared") then 
        set_prefixname("lib")
        add_defines("_JSONC_EXPORT")
    end
    add_includedirs("include")
    add_files("src/**.cpp")

    if is_mode("debug") then
        set_symbols("debug")
    else
        set_optimize("aggressive")
        set_strip("all")
    end
    
    if is_plat("windows") then
        add_defines(
            "NOMINMAX",
            "UNICODE"
        )
        add_cxflags(
            "/EHsc",
            "/utf-8",
            "/W4",
            "/EHs-c-",
            "/GR-"
        )
        set_exceptions("none")
        if is_mode("release") then
            add_cxflags(
                "/O2",
                "/Ob3"
            )
        end
        if is_config("kind", "shared") then
            add_cxflags("/GL")
            add_shflags("/LGCT")
        end
    else
        add_cxflags(
            "-Wall",
            "-Wextra",
            "-Wconversion",
            "-pedantic",
            "-fno-exceptions",
            "-fPIC",
            "-stdlib=libc++",
            "-fno-rtti"
        )
        if is_mode("release") then
            add_cxflags(
                "-O3"
            )
        end
        if is_config("kind", "shared") then
            set_policy("build.optimization.lto", true)
            add_cxflags(
                "-fvisibility=hidden",
                "-fvisibility-inlines-hidden"
            )
            if is_plat("linux") then 
                add_shflags(
                    "-static-libstdc++",
                    "-static-libgcc",
                    "-Wl,--no-undefined",
                    "-Wl,--exclude-libs,ALL"
                )
                add_syslinks("libc++.a", "pthread")
            end
            if is_plat("macosx") then
                add_shflags("-dynamiclib")
            end
        end
    end
    if is_config("kind", "shared") then
        after_build(function (target)
            local plat = target:plat()
            local arch = target:arch()
            local name = target:name()
            if arch == "x64" then
                arch = "x86_64" -- Fix arch name on Windows
            end
            local target_file = target:targetfile()
            local filename = path.filename(target_file)
            local output_dir = path.join(os.projectdir(), "bin/" .. name .. "-" .. plat .. "-" .. arch)
            os.mkdir(output_dir)
            local artifact_dir = path.join(os.projectdir(), "artifacts")
            os.mkdir(artifact_dir)
            os.cp(target_file, output_dir)
            if plat == "macosx" then -- Fix rpath on MacOS
                os.run("install_name_tool -id @rpath/" .. filename .. " " .. path.join(output_dir, filename))
            end
            local zip_file = path.join(os.projectdir(), "bin/" .. name .. "-" .. plat .. "-" .. arch .. ".zip")
            os.rm(zip_file)
            if plat == "windows" then
                local win_src = output_dir:gsub("/", "\\")
                local win_dest = zip_file:gsub("/", "\\")
                local command = string.format(
                    'powershell -Command "Compress-Archive -Path \'%s\\*\' -DestinationPath \'%s\'"',
                    win_src,
                    win_dest
                )
                os.exec(command)
            else
                os.exec("zip -rj -q '%s' '%s'", zip_file, output_dir)
            end
            os.mv(zip_file, artifact_dir)
            cprint("${bright green}[Shared Library]: ${reset}".. filename .. " already generated to " .. output_dir)
        end)
    end

if is_config("enable_test", true) then 
target("test")
    set_kind("binary")
    set_languages("c++23")
    add_includedirs(
        "include",
        "test"
    )
    add_files("test/**.cpp")
    -- add_defines("JSONC_USE_EXPECTED")
    if is_plat("windows") then 
        add_defines(
            "NOMINMAX",
            "UNICODE"
        )
        add_cxflags("/utf-8", "/W4")
    else 
        add_cxflags("-Wall", "-stdlib=libc++")
        add_syslinks("c++")
    end

    after_build(function (target)
        local file = target:targetfile()
        local output_dir = path.join(os.projectdir(), "bin")
        os.mkdir(output_dir)
        os.cp(file, output_dir)
        cprint("${bright green}[Shared Library]: ${reset}Shared Library file already generated to " .. output_dir)
    end)
end