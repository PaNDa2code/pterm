const std = @import("std");

pub fn build(b: *std.Build) !void {
    const optimize = b.standardOptimizeOption(.{
        .preferred_optimize_mode = .ReleaseFast,
    });

    const target = b.standardTargetOptions(.{
        .default_target = .{
            .os_tag = .windows,
            .abi = .msvc,
        },
    });

    const src_path = b.path("src");
    const include_path = b.path("include");
    const main_path = try src_path.join(b.allocator, "main.c");

    const pterm_lib = b.addStaticLibrary(.{
        .name = "pterm",
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });

    // Include directory
    pterm_lib.addIncludePath(include_path);
    // C source files
    pterm_lib.addCSourceFiles(.{
        .files = &.{
            "create_process.c",
            "ring_buffer.c",
        },
        .root = src_path,
    });
    // Link libraries
    pterm_lib.linkSystemLibrary("kernel32");
    pterm_lib.linkSystemLibrary("user32");
    pterm_lib.linkSystemLibrary("onecore");

    // Executable
    const exe = b.addExecutable(.{
        .name = "pterm",
        .target = target,
        .optimize = optimize,
    });

    // Add the main c file
    exe.addCSourceFile(.{ .file = main_path });
    // Add include directory
    exe.addIncludePath(include_path);
    // Link aginst the static library
    exe.linkLibrary(pterm_lib);
    // Set as default build
    b.installArtifact(exe);

    const exe_run = b.addRunArtifact(exe);

    const run_step = b.step("run", "run pterm executable");

    run_step.dependOn(&exe_run.step);

    const unit_test = b.addExecutable(.{
        .name = "unit test",
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });

    unit_test.addCSourceFiles(.{
        .root = b.path("test"),
        .files = &.{ "test_main.c", "munit.c" },
    });

    unit_test.addIncludePath(include_path);

    if (target.result.abi == .msvc) {
        // MSVC POSIX runtime library for windows
        unit_test.linkSystemLibrary("msvcrt");
        unit_test.linkSystemLibrary("oldnames");
    }
    unit_test.linkLibrary(pterm_lib);
    const test_run = b.addRunArtifact(unit_test);
    const test_step = b.step("test", "run unit tests");
    test_step.dependOn(&test_run.step);

    const DebugTargetEnum = enum { exe, tests };
    const debug_target = b.option(DebugTargetEnum, "debug", "debuging target, only used with dbg step") orelse DebugTargetEnum.exe;

    const dbg_step = b.step("dbg", "run windbg on target exe or unit tests");

    const windbg_command = b.addSystemCommand(&.{"windbgx"});

    switch (debug_target) {
        .exe => {
            windbg_command.addFileArg(exe.getEmittedBin());
            windbg_command.step.dependOn(&exe.step);
        },
        .tests => {
            windbg_command.addFileArg(unit_test.getEmittedBin());
            windbg_command.step.dependOn(&unit_test.step);
        },
    }

    dbg_step.dependOn(&windbg_command.step);
}
