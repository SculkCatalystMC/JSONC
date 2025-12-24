add_rules("mode.debug", "mode.release")

if is_plat("windows") and not has_config("vs_runtime") then
    set_runtimes("MD")
end

target("test")
    set_kind("binary")
    set_languages("c++23")
    add_packages(
        "binarystream",
        "zlib"
    )
    add_includedirs(
        "include",
        "test"
    )
    add_files("test/**.cpp")
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
        cprint("${bright green}[Execuatble]: ${reset}Execuatble file already generated to " .. output_dir)
    end)