#!/usr/bin/awk -f

# libmaple's own Evil Mangler
#
# Input filter hack to trick Doxygen into thinking that a series
# header is in a separate namespace. This is necessary because Doxygen
# doesn't know how to cope with two data structures with the same name
# in different places in the project. (We do that all the time,
# e.g. for foo_reg_map structs.)
#
# E.g., an STM32F1 header gets transformed into:
#
# namespace stm32f1 {
# <contents of header>
# }

BEGIN {
    # For extracting series component from header FILENAME.
    series_regex = "/stm32[flw][0-9]*/";
    # Holds header FILENAME. Cargo-culted; not sure why it's necessary.
    f = "";
    # Holds series component.
    series = "";
}
{
    if (f != FILENAME) {
        f = FILENAME;
        match(f, series_regex);
        series = substr(f, RSTART + 1, RLENGTH - 2);
        printf("namespace %s {\n", series);
    }
    print;
}
END {
    if (series != "") {
        print "}"
    }
}
