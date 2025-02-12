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

    pterm_lib.linkSystemLibrary("kernel32");
    pterm_lib.linkSystemLibrary("user32");
    pterm_lib.linkSystemLibrary("onecore");
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
}
