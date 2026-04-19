<!-- Claude Code integration for AWS Graviton Migration Power.

     Setup:
     1. Copy arm-migration.md to .claude/commands/arm-migration.md in your workspace
     2. Copy steering/ directory to .claude/commands/steering/ (optional, for Karpenter guidance)
     3. Add the MCP server config below to your project's .mcp.json or ~/.claude.json

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

     Invoke using /arm-migration in the Claude Code chat.
-->
---
description: "Analyzes source code to identify compatibilities with Graviton processors (Arm64 architecture). Generates reports with incompatibilities and provides suggestions for minimal required and recommended versions for language runtimes and dependency libraries."
allowed-tools: mcp__arm-mcp__skopeo, mcp__arm-mcp__check_image, mcp__arm-mcp__knowledge_base_search, mcp__arm-mcp__migrate_ease_scan, mcp__arm-mcp__mca, mcp__arm-mcp__sysreport_instructions
---

# Graviton Migration Power

## Overview

The Graviton Migration Power helps developers migrate workloads to AWS Graviton processors (Arm64 architecture). It analyzes source code for known code patterns and dependency libraries to identify compatibilities with Graviton processors, generates reports highlighting detected compatibility issues (manual review recommended), and provides actionable suggestions for minimal required and recommended versions for both language runtimes and dependency libraries.

## Prerequisites

Before starting, verify that:

1. **Docker** is installed and running (`docker --version` and `docker ps`)
2. The **arm-mcp** MCP server is configured. If you don't have access to the arm-mcp tools (skopeo, check_image, knowledge_base_search, migrate_ease_scan, mca, sysreport_instructions), add the following to your `.mcp.json`:

```json
{
  "mcpServers": {
    "arm-mcp": {
      "command": "sh",
      "args": ["-c", "docker run --rm -i -v \"$(pwd)\":/workspace armlimited/arm-mcp:latest"],
      "timeout": 180000
    }
  }
}
```

**CRITICAL**: If Docker is not installed or not running, DO NOT proceed with migration assessment.

## What This Power Does

The goal is to migrate a codebase from x86 to Arm. Use the MCP server tools to help you with this. Check for x86-specific dependencies (build flags, intrinsics, libraries, etc) and change them to Arm architecture equivalents, help identify compatibility issues and suggests optimizations for Arm architecture. Look at Dockerfiles, version files, and other dependencies, compatibility, and optimize performance.

## Steps to Follow

1. Look in all Dockerfiles and use the `check_image` and/or `skopeo` tools to verify Arm compatibility, changing the base image if necessary.
2. Look at the packages installed by the Dockerfile and send each package to the `knowledge_base_search` tool to check each package for Arm compatibility. If a package is not compatible, change it to a compatible version. When invoking the tool, explicitly ask "Is [package] compatible with Arm architecture?" where [package] is the name of the package.
3. Look at the contents of any requirements.txt files line-by-line and send each line to the `knowledge_base_search` tool to check each package for Arm compatibility. If a package is not compatible, change it to a compatible version.
4. Look at the codebase that you have access to, and determine what the language used is.
5. Run the `migrate_ease_scan` tool on the codebase, using the appropriate language scanner based on what language the codebase uses.
6. Provide an analysis report with complete dependency analysis, migration recommendations and optimizations for AWS Graviton processor.
7. Get a confirmation with user before proceeding with the code changes.

## Karpenter Migration

If the project uses Karpenter for Kubernetes node provisioning, follow the Karpenter migration strategy:

### Detection

Look for:
- YAML files containing `apiVersion: karpenter.sh/v1` or `karpenter.sh/v1beta1`
- Resources of `kind: NodePool` and `kind: EC2NodeClass`
- Existing `kubernetes.io/arch` requirements set to `amd64` only
- Instance family requirements using x86-only families (e.g., `m5`, `c5`, `r5`)

### Gradual Rollout Strategy

1. **Create a dedicated Graviton NodePool** — separate from existing x86 NodePool with `kubernetes.io/arch: arm64` requirement and a `graviton-migration` taint.
2. **Add tolerations to workloads** being migrated for the `graviton-migration` taint.
3. **Force scheduling on Graviton** — after validation, add `nodeSelector: kubernetes.io/arch: arm64`.
4. **Post-migration cleanup** — remove taints, tolerations, nodeSelectors, and delete the old x86-only NodePool.

### Common Instance Family Mappings

| x86 Family | Graviton Equivalent | Notes |
|------------|-------------------|-------|
| m5, m6i    | m6g, m7g          | General purpose |
| c5, c6i    | c6g, c7g          | Compute optimized |
| r5, r6i    | r6g, r7g          | Memory optimized |
| t3          | t4g               | Burstable |

### Key Checks for Karpenter

- Verify all container images support `linux/arm64` (multi-arch or ARM64-specific)
- Check sidecar containers (service mesh proxies, logging agents) for ARM64 support
- Check DaemonSets for ARM64 compatibility
- Validate any init containers also have ARM64 images

## Available Tools

- **migrate_ease_scan**: Scans codebases for Arm compatibility issues (C++, Python, Go, JS, Java)
- **skopeo**: Inspects container images remotely for architecture support
- **knowledge_base_search**: Searches Arm documentation for migration guidance
- **check_image**: Quick Docker image architecture verification
- **mca** (Machine Code Analyzer): Analyzes assembly code performance predictions

## Pitfalls to Avoid

* Don't confuse a software version with a language wrapper package version. For example, when checking the Python Redis client, check the Python package name "redis" rather than the Redis server version.
* NEON lane indices must be compile-time constants, not variables.
* If you're unsure about Arm equivalents, use `knowledge_base_search` to find documentation.
* Be sure to find out from the user or system what the target machine is, and use the appropriate intrinsics. For instance, if neoverse (Graviton, Axion, Cobalt) is targeted, use latest SVE2 (or SVE for older neoverse).

## Additional Resources

- [AWS Graviton Technical Guide](https://github.com/aws/aws-graviton-getting-started)
- [Migrating from x86 to Graviton on EKS using Karpenter](https://aws.amazon.com/blogs/containers/migrating-from-x86-to-aws-graviton-on-amazon-eks-using-karpenter/)
- [Karpenter NodePool docs](https://karpenter.sh/docs/concepts/nodepools/)
