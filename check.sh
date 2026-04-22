#!/usr/bin/env bash
set -euo pipefail

BOLD='\033[1m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m'

FIX=0
for arg in "$@"; do
    [[ "$arg" == "--fix" ]] && FIX=1
done

FAILED=()

# ── clang-format ─────────────────────────────────────────────────────────────

echo -e "${BOLD}clang-format${NC}"
FILES=$(find src game/src \( -name '*.cpp' -o -name '*.hpp' \))

if [[ $FIX -eq 1 ]]; then
    echo "$FILES" | xargs clang-format -i
    echo -e "  ${GREEN}formatted${NC}"
elif echo "$FILES" | xargs clang-format --dry-run --Werror 2>&1; then
    echo -e "  ${GREEN}pass${NC}"
else
    echo -e "  ${RED}fail${NC} — run ${YELLOW}./check.sh --fix${NC} to auto-format"
    FAILED+=("clang-format")
fi

# ── clang-tidy ───────────────────────────────────────────────────────────────

echo -e "${BOLD}clang-tidy${NC}"
if [[ ! -f build/compile_commands.json ]]; then
    echo -e "  ${YELLOW}configuring build to generate compile_commands.json...${NC}"
    cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON > /dev/null
fi

if find src game/src -name '*.cpp' | xargs clang-tidy -p build; then
    echo -e "  ${GREEN}pass${NC}"
else
    FAILED+=("clang-tidy")
fi

# ── summary ──────────────────────────────────────────────────────────────────

echo ""
if [[ ${#FAILED[@]} -eq 0 ]]; then
    echo -e "${GREEN}${BOLD}All checks passed.${NC}"
else
    echo -e "${RED}${BOLD}Failed: ${FAILED[*]}${NC}"
    exit 1
fi
