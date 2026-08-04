# Publishing EdgeTX32

Push this tree to the public GitHub repository:
https://github.com/aliemreko/edgetx32

## Script push

1. Create a [Personal Access Token](https://github.com/settings/tokens) (classic) with the `repo` scope.
2. From this tree (Git Bash / WSL / Linux):

```bash
export GH_TOKEN=ghp_your_token_here
OWNER=aliemreko REPO=edgetx32 ./tools/publish_edgetx32_repo.sh
```

## Manual git push

```bash
git init -b main
git add -A
git commit -m "Initial public release: EdgeTX32 (EdgeTX ESP32-S3 fork)"
git remote add origin https://github.com/aliemreko/edgetx32.git
git push -u origin main
```

## What is included

- Full ESP32-S3 EdgeTX fork sources
- Docs: architecture, build, pinout, what-changed, statistics
- Comparison PDF + charts under `reports/`
- GPLv2 license + NOTICE
