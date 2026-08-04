#!/usr/bin/env bash
# Publish this tree to a NEW public GitHub repository named EdgeTX32.
#
# Prefer creating an empty public repo on GitHub first, then:
#
#   export GH_TOKEN=ghp_...   # classic PAT with `repo` scope
#   ./tools/publish_edgetx32_repo.sh
#
# Optional:
#   OWNER=aliemreko REPO=edgetx32 ./tools/publish_edgetx32_repo.sh
#
set -euo pipefail

OWNER="${OWNER:-aliemreko}"
REPO="${REPO:-edgetx32}"
ROOT="$(cd "$(dirname "$0")/.." && pwd)"

if [[ -z "${GH_TOKEN:-${GITHUB_TOKEN:-}}" ]]; then
  echo "Set GH_TOKEN to a classic PAT with 'repo' scope (and delete_repo if recreating)."
  exit 1
fi
TOKEN="${GH_TOKEN:-$GITHUB_TOKEN}"

api() {
  local method=$1; shift
  local url=$1; shift
  curl -sS -X "$method" \
    -H "Authorization: token ${TOKEN}" \
    -H "Accept: application/vnd.github+json" \
    -H "X-GitHub-Api-Version: 2022-11-28" \
    "$url" "$@"
}

echo "==> Checking if ${OWNER}/${REPO} exists"
CODE=$(curl -sS -o /tmp/edgetx32_repo.json -w "%{http_code}" \
  -H "Authorization: token ${TOKEN}" \
  -H "Accept: application/vnd.github+json" \
  "https://api.github.com/repos/${OWNER}/${REPO}")

if [[ "$CODE" == "404" ]]; then
  echo "==> Creating public repository ${OWNER}/${REPO}"
  RESP=$(api POST "https://api.github.com/user/repos" \
    -d "{\"name\":\"${REPO}\",\"description\":\"EdgeTX32 — open-source EdgeTX fork for ESP32-S3 (ESP-IDF)\",\"homepage\":\"https://github.com/EdgeTX/edgetx\",\"private\":false,\"has_issues\":true,\"has_wiki\":true,\"auto_init\":false}")
  echo "$RESP" | python3 -c "import sys,json; d=json.load(sys.stdin); print(d.get('html_url', d))"
elif [[ "$CODE" == "200" ]]; then
  echo "==> Repository already exists"
else
  echo "Unexpected status $CODE"; cat /tmp/edgetx32_repo.json; exit 1
fi

TMP=$(mktemp -d)
echo "==> Preparing clean git history in $TMP"
rsync -a --delete --exclude='.git' "$ROOT/" "$TMP/" 2>/dev/null || {
  mkdir -p "$TMP"
  cp -a "$ROOT"/. "$TMP/"
  rm -rf "$TMP/.git"
}
cd "$TMP"
git init -b main
git config user.email "edgetx32@users.noreply.github.com"
git config user.name "EdgeTX32 Publish"
git add -A
git commit -m "Initial public release: EdgeTX32 (EdgeTX ESP32-S3 fork)"

git remote add origin "https://x-access-token:${TOKEN}@github.com/${OWNER}/${REPO}.git"
echo "==> Pushing main (force if re-publish)"
git push -u origin main --force

echo
echo "Done: https://github.com/${OWNER}/${REPO}"
echo "Set repo visibility to Public in GitHub settings if needed."
