<!-- Claude Code integration for AWS Graviton Hotspots Optimization (Beginner).

     Setup:
     1. Copy arm-hotspots-optimization.md to .claude/commands/arm-hotspots-optimization.md in your workspace
     2. Add the MCP server config below to your project's .mcp.json or ~/.claude.json

     MCP Configuration - add to .mcp.json in your project root:
     {
       "mcpServers": {
         "arm-mcp": {
           "command": "sh",
           "args": ["-c", "docker run --rm -i -v \"$(pwd)\":/workspace armlimited/arm-mcp:latest"],
           "timeout": 180000
         }
       }
     }

     Invoke using /arm-hotspots-optimization in the Claude Code chat.
-->
---
description: "Guide a beginner through Arm cloud performance tuning using ATP code_hotspots baseline, targeted code changes, and delta validation"
allowed-tools: mcp__arm-mcp__apx_recipe_run, mcp__arm-mcp__knowledge_base_search, mcp__arm-mcp__mca, mcp__arm-mcp__check_image, mcp__arm-mcp__skopeo, mcp__arm-mcp__migrate_ease_scan, mcp__arm-mcp__sysreport_instructions
---

# Arm Hotspots Performance Optimization (Beginner)

## Overview

Your goal is to help a cloud developer with zero optimization experience improve code performance on an Arm-based cloud machine using an iterative, measurable workflow.

## Primary Workflow

* Keep the process beginner-friendly. Explain what you are doing in plain language and avoid unexplained jargon.
* Use `code_hotspots` as the default recipe unless the user explicitly asks for another recipe.
* Follow this loop exactly: baseline profile -> one focused code change -> re-profile -> compare delta.

## Steps

* Confirm runtime context before profiling:
  * Try to infer the workload command, target host/IP, SSH username, and whether the target is localhost or remote. Confirm before running. Use absolute paths. Examples:
    * For C++ code: `/home/user/arm-migration-example/benchmark`
    * For Python code: `python /home/user/workspace/train.py`
    * For Java: `java -cp "absolute/path/to/class" some.package.Main`
  * If localhost is requested, use `localhost` as the remote_ip.
  * If APX setup is missing, surface the exact tool error and ask the user to fix MCP APX mount/config first.
  * Identify the build or compile command if the code language requires it. Then build or compile the binary or executable before running APX.
* Run first profile with `mcp__arm-mcp__apx_recipe_run` using recipe `code_hotspots` and the user workload command. This workload command should be simply the path to the executable. Do not combine other commands such as `cd` or `cat` or any other bash commands.
* Parse and summarize hotspots:
  * Identify top hot functions/regions and estimate where most CPU time is spent.
  * Give a short "why this is hot" hypothesis for each top hotspot.
  * Choose one optimization candidate with the highest likely payoff and lowest implementation risk.
* Make one targeted code change per profile run.
  * Typical first-pass improvements include:
    * Avoid repeated work in tight loops.
    * Reduce allocations/copies in hot paths.
    * Replace inefficient data access patterns.
    * Use compiler/runtime flags that are safe for Arm cloud targets.
* Re-build or re-compile if required and re-run `mcp__arm-mcp__apx_recipe_run` with the same command and recipe (`code_hotspots`).
* Compare baseline vs new run:
  * Report hotspot movement and runtime delta in clear, concrete numbers when available.
  * State whether the change improved, regressed, or had no meaningful effect.
  * If improved, propose the next single change and repeat only if user wants to continue.
  * If regressed or neutral, revert or adjust strategy and run one more validation profile.

## Tool Usage Guidance

* The command for `mcp__arm-mcp__apx_recipe_run` needs to be a single executable. The profiler should be profiling only the application executable or binary.
* Use `Grep` and `Glob` to find hotspot symbols, call sites, and related code quickly.
* Use `mcp__arm-mcp__knowledge_base_search` for Arm-specific optimization guidance, intrinsics, compiler flags, and microarchitecture advice.
* Use `mcp__arm-mcp__mca` when assembly-level bottlenecks are suspected and an assembly/object file is available.
* Use `mcp__arm-mcp__check_image` or `mcp__arm-mcp__skopeo` when Docker base images or deployment images may affect Arm performance or compatibility.
* Use `mcp__arm-mcp__migrate_ease_scan` when architecture migration issues are likely mixed with performance issues.
* Use `mcp__arm-mcp__sysreport_instructions` when system-level CPU/memory/platform facts are missing.

## Pitfalls to Avoid

* Do not apply many optimizations at once; this breaks attribution.
* Do not include multiple commands in the cmd param for `apx_recipe_run` tool. Build first, then pass only the executable or runtime command to the tool.
* Do not compare runs with different workload commands, inputs, or environments.
* Do not present unmeasured claims as performance wins.
* Do not jump to architecture-specific intrinsics before simpler algorithmic/data fixes unless profiling strongly indicates it.
* Do not assume advanced PMU counter access; `code_hotspots` should work in more restricted environments.

## Output Format

* Baseline summary: top hotspots and key metrics.
* Change made: exact files/functions touched and rationale.
* Delta summary: what improved/regressed and by how much.
* Next step: one recommended follow-up optimization.
