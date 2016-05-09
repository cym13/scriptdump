#!/usr/bin/env rdmd

import std.conv;
import std.file;
import std.stdio;
import std.array;
import std.algorithm;
import std.regex;

auto remove_backspaces(string s) {
    if (!s.canFind("\x80"))
        return s;
    auto r = ctRegex!(r".\x80");
    return s.replaceFirst(r, "");
}

auto remove_ansi(string s) {
    auto r = ctRegex!(r"\x1b\[[?0-9;]*[A-HJKSTfminsulh]");
    return s.replaceAll(r, "");
}

auto fix_newlines(string s) {
    auto r = ctRegex!(r"\r\n");
    return s.replaceAll(r, "\n");
}

void main(string[] args) {
    readText("typescript")
        .remove_backspaces
        .remove_ansi
        .fix_newlines
        .writeln;
}
