<!-- Claude Code integration for Arm vs x86 Performance Comparison.

     Setup:
     1. Copy arm-vs-x86-performance-comparison.md to .claude/commands/arm-vs-x86-performance-comparison.md in your workspace
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

     Invoke using /arm-vs-x86-performance-comparison in the Claude Code chat.
-->
---
description: "Guide a user through APX code_hotspots profiling on x86 and Arm hosts, then report architecture-driven performance delta and price-performance signals"
allowed-tools: mcp__arm-mcp__apx_recipe_run, mcp__arm-mcp__knowledge_base_search, mcp__arm-mcp__mca, mcp__arm-mcp__check_image, mcp__arm-mcp__skopeo, mcp__arm-mcp__migrate_ease_scan, mcp__arm-mcp__sysreport_instructions
---

# Arm vs x86 Performance Comparison

## Overview

Your goal is to help a user compare application performance between an existing x86 cloud instance and an existing Arm cloud instance using a repeatable APX workflow.

Use this prompt when the user says they are migrating from x86 to Arm and wants measurable proof of performance change (including price-performance-per-watt direction) but does not know where to start.

## Primary Workflow

* Keep the process beginner-friendly and concrete. Explain each step in plain language.
* Use `code_hotspots` as the default APX recipe.
* Run the same workload command on both machines. Do not change inputs between runs.
* Treat x86 as baseline and Arm as candidate.
* Compare results only after both runs are complete and valid.

## Steps

* Confirm required runtime context first:
  * x86 host/IP and SSH username.
  * Arm host/IP and SSH username.
  * Workload command to run on both hosts. Use absolute paths. Examples:
    * For C++ code: `/home/user/arm-migration-example/benchmark`
    * For Python code: `python /home/user/workspace/train.py`
    * For Java: `java -cp "absolute/path/to/class" some.package.Main`
  * If code requires build or compile steps, run those first on each host before APX profiling.
  * Whether either target is local-machine-hosted (if localhost is requested, remember APX tooling runs in a container and host IP is usually `172.17.0.1`).
  * Any required env vars, dataset paths, and run duration/arguments.
  * If ATP/APX setup is missing or misconfigured, surface the exact tool error and ask the user to fix MCP ATP mount/config, then continue.
* Run baseline profile on x86 using `mcp__arm-mcp__apx_recipe_run` with recipe `code_hotspots`.
* Validate baseline run quality:
  * Confirm the command actually executed.
  * Confirm report includes useful hotspot data (not empty/failed/truncated).
* Run candidate profile on Arm using `mcp__arm-mcp__apx_recipe_run` with the same recipe and command.
* Validate candidate run quality with the same checks as baseline.
* Compare Arm vs x86:
  * Wall time/runtime delta (absolute and percent).
  * Hotspot shifts (which functions got better/worse).
  * Any obvious architecture-related behavior changes.
* Translate results into user-facing outcome:
  * State clearly whether Arm improved, regressed, or was neutral.
  * Summarize likely impact on price-performance using measured runtime delta.
  * If power or cost inputs are missing, state assumptions explicitly and provide a method for final price-performance-per-watt confirmation.
* Optional follow-up:
  * If Arm underperforms or has a clear hotspot issue, suggest one targeted optimization and run one extra Arm validation profile if the user wants.

## Tool Usage Guidance

* Use `mcp__arm-mcp__apx_recipe_run` for both x86 and Arm runs.
* Use `mcp__arm-mcp__sysreport_instructions` when machine metadata (CPU model, core count, frequency behavior, memory) is missing and needed for interpretation.
* Use `mcp__arm-mcp__knowledge_base_search` for architecture-specific guidance when explaining hotspot differences or next optimizations.
* Use `Grep`, `Glob`, and `Read` for code-level analysis only if user asks for optimization after the comparison.

## Pitfalls to Avoid

* Do not include multiple commands in the cmd param for `apx_recipe_run` tool. Build first, then pass only the executable or runtime command to the tool.
* Do not compare runs with different workload commands, arguments, input data, or machine scale.
* Do not claim perf/watt wins without either measured power/cost inputs or explicit assumptions.
* Do not mix x86 and Arm reports from different app versions/builds unless clearly labeled as non-comparable.
* Do not skip run validation; failed or partial runs must be rerun before comparison.
* Do not apply broad code changes before establishing the architecture-only baseline delta.

## Output Format

* Setup summary: x86 host, Arm host, workload command, and comparability checks.
* x86 baseline summary: key runtime and top hotspots.
* Arm run summary: key runtime and top hotspots.
* Delta summary: Arm vs x86 runtime and hotspot movement in concrete numbers.
* Price-performance-per-watt interpretation: measured conclusion or assumption-based estimate with caveats.
* Next step: one recommended follow-up action.
