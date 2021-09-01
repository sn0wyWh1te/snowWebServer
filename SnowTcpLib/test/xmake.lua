add_rules("mode.debug")
add_includedirs("../")

target("timer_test")
    set_kind("binary")
    add_files("../*.cc","./*.cc")
    set_targetdir("./")
    add_links("pthread")
