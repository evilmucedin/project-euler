# Pi agent instructions

This file is for agents running in Pi. Also read `CLAUDE.md`; it contains the shared repository workflow, setup, and build commands.

## Working style

- Keep changes small and task-focused.
- Use `rg`, `find`, and file reads to understand existing patterns before editing.
- Prefer precise edits over rewriting large generated or third-party files.
- Treat untracked local scratch files as user-owned unless the task explicitly asks to clean them up.

## Build workflow

- Primary build system: Buck2.
- Generated smoke-build system: Ninja via `scripts/generate_ninja.py`.
- After changing any `BUCK` or `BUILD` file, run:

```sh
python3 scripts/generate_ninja.py
```

- For quick validation, run:

```sh
ninja advent/2020/1/1
```

- If `buck2` is installed, also validate:

```sh
buck2 build //advent/2020/1/...
```

## PR checklist

Before creating a PR from Pi:

1. Check `git status --short --branch`.
2. Run relevant local checks and mention them in the PR body.
3. Exclude unrelated untracked files from commits.
4. Create a branch with a descriptive name.
5. Push and open the PR with `gh pr create`.
