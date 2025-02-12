const std = @import("std");

pub fn build(b: *std.Build) void {
    const optimize = b.standardOptimizeOption(.{
        .preferred_optimize_mode = .ReleaseFast,
    });

    const target = b.standardTargetOptions(.{
        .default_target = .{
            .os_tag = .windows,
            .abi = .msvc,
        },
    });

    const exe = b.addExecutable(.{
        .name = "pterm",
        .target = target,
        .optimize = optimize,
        .link_libc = true,
        .linkage = .static,
    });

    // Include directory
    exe.addIncludePath(b.path("include/"));

    exe.addCSourceFiles(.{
        .files = &.{
            "main.c",
            "create_process.c",
            "ring_buffer.c",
        },
        .root = b.path("src/"),
    });

    // Link libraries
    exe.linkSystemLibrary("kernel32");
    exe.linkSystemLibrary("user32");
    exe.linkSystemLibrary("onecore");

    b.installArtifact(exe);

    const exe_run = b.addRunArtifact(exe);

    const run_step = b.step("run", "run pterm executable");

    run_step.dependOn(&exe_run.step);
}
