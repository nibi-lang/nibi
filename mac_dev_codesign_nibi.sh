#!/usr/bin/env bash
set -euo pipefail

# Ad-hoc code-sign a nibi binary with the get-task-allow entitlement so macOS
# `leaks` (and lldb) can read full process memory, dump leaked-object contents,
# and symbolicate allocation backtraces. Without this, leaks reports:
#   "Process is not debuggable. Due to security restrictions..."
#
# Usage:
#   ./mac_dev_codesign_nibi.sh [path-to-nibi-binary]
#
# Defaults to the local build output. Then run, for example:
#   MallocStackLogging=1 leaks --atExit --list -- ./nibi/build/nibi tests/leak_detector.nibi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TARGET="${1:-$SCRIPT_DIR/nibi/build/nibi}"

if [[ ! -f "$TARGET" ]]; then
  echo "error: binary not found: $TARGET" >&2
  echo "build it first, or pass an explicit path: ./mac_dev_codesign_nibi.sh /path/to/nibi" >&2
  exit 1
fi

if [[ ! -w "$TARGET" ]]; then
  echo "error: $TARGET is not writable (owned by another user?)." >&2
  echo "sign a user-owned build instead of the system-installed binary." >&2
  exit 1
fi

ENTITLEMENTS="$(mktemp -t nibi_entitlements).plist"
trap 'rm -f "$ENTITLEMENTS"' EXIT

cat > "$ENTITLEMENTS" <<'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.security.get-task-allow</key>
    <true/>
</dict>
</plist>
PLIST

echo "signing: $TARGET"
codesign -s - -f --entitlements "$ENTITLEMENTS" "$TARGET"

echo "verifying entitlements:"
codesign -d --entitlements - "$TARGET" 2>/dev/null || true

echo "done."
