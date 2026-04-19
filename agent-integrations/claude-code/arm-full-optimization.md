<!-- Claude Code integration for AWS Graviton Full Performance Optimization.

     Setup:
     1. Copy arm-full-optimization.md to .claude/commands/arm-full-optimization.md in your workspace
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

     Invoke using /arm-full-optimization in the Claude Code chat.
-->
---
description: "Drive expert-level iterative Arm performance tuning across code_hotspots, instruction_mix, cpu_microarchitecture, and memory recipes with measured deltas after each code change"
allowed-tools: mcp__arm-mcp__apx_recipe_run, mcp__arm-mcp__knowledge_base_search, mcp__arm-mcp__mca, mcp__arm-mcp__check_image, mcp__arm-mcp__skopeo, mcp__arm-mcp__migrate_ease_scan, mcp__arm-mcp__sysreport_instructions
---

# Advanced Arm Performance Optimization

## Overview

Your goal is to help an experienced performance engineer maximize application speed on Arm-based cloud systems, with a measured and repeatable optimization workflow.

Use this prompt when the user wants aggressive performance tuning (often C/C++/C#) and expects deep technical guidance, but is less familiar with Arm cloud optimization.

## Primary Workflow

* Keep changes measurable: one focused optimization at a time, then re-profile.
* Use a fixed workload command, dataset, and runtime environment for all comparisons.
* Follow this sequence exactly:
  * Baseline `code_hotspots` -> code change -> re-profile.
  * Baseline `instruction_mix` -> code change -> re-profile.
  * Baseline `cpu_microarchitecture` -> code change -> re-profile.
  * Baseline `memory_access` -> code change -> re-profile.
* After recipe-specific loops, compare final run against original baseline for total gain.
* Prioritize high-impact, low-risk edits first, then move to micro-optimizations.

## Steps

* Try to infer as much runtime context as possible from the user query, but confirm all critical details before starting:
  * Workload command and build configuration. Use absolute paths. Examples:
    * For C++ code: `/home/user/arm-migration-example/benchmark`
    * For Python code: `python /home/user/workspace/train.py`
    * For Java: `java -cp "absolute/path/to/class" some.package.Main`
* Confirm runtime context before first profile:
  * Target host/IP, SSH username, workload command, input dataset, build flags, and repetition count.
  * Compiler/toolchain versions and current optimization flags.
  * CPU/platform details (Neoverse generation if available).
  * If ATP/APX setup is missing, surface exact error and request MCP ATP config fix.
* Establish initial baseline with `mcp__arm-mcp__apx_recipe_run` and recipe `code_hotspots`.
* For each recipe phase (`code_hotspots`, `instruction_mix`, `cpu_microarchitecture`, `memory_access`):
  * Run baseline for that recipe.
  * Identify the strongest actionable bottleneck.
  * Make one targeted code/build change.
  * Re-run the same recipe with identical workload.
  * Report delta and keep/revert based on measured outcome.
* During analysis:
  * Use `instruction_mix` to target expensive instruction patterns and vectorization opportunities.
  * Use `cpu_microarchitecture` to classify front-end, bad speculation, back-end, and retiring bottlenecks.
  * Use `memory_access` to target cache misses, bandwidth pressure, and access locality.
* End with total optimization summary:
  * Compare final run vs original run in absolute and percentage terms.
  * Summarize cumulative effect by hotspot class (compute, pipeline, memory).

## Arm-Specific Guidance

* Use `mcp__arm-mcp__knowledge_base_search` for Arm microarchitecture advice, compiler flags, and intrinsic guidance.
* Validate target-specific strategies (for example NEON vs SVE/SVE2) against actual target CPU.
* Prefer algorithm and data-layout wins before architecture-specific intrinsics unless profiling strongly justifies low-level tuning.

## Pitfalls to Avoid

* Do not apply multiple unrelated edits in one measurement step.
* Do not include multiple commands in the cmd param for `apx_recipe_run` tool. Build first, then pass only the executable or runtime command to the tool.
* Do not change workload inputs between baseline and re-profile runs.
* Do not present unmeasured gains as improvements.
* Do not force intrinsics where compiler auto-vectorization with proper flags is sufficient.
* Do not ignore statistical noise; repeat runs when deltas are small or unstable.

## Output Format

* Environment summary: host, CPU details, compiler flags, and workload command.
* Per-recipe findings:
  * Bottleneck identified.
  * Change made (file/function/build flag).
  * Re-profile delta and keep/revert decision.
* Final cumulative delta: final vs original baseline with concrete numbers.
* Recommended next optimization: one highest-value follow-up.
