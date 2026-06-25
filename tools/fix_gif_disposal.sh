#!/usr/bin/env bash
# fix_gif_disposal.sh — Re-encode a GIF with disposal=2 (restore-background)
# so transparent frames don't bleed previous content through.
#
# Usage:
#   ./tools/fix_gif_disposal.sh assets/some.gif
#   ./tools/fix_gif_disposal.sh assets/some.gif assets/output.gif   # custom output path
#
# The original is backed up as <input>.bak unless --no-backup is passed.
#
# Options:
#   --no-backup   Skip creating a .bak of the original
#   --dry-run     Show what would happen without writing any files

set -euo pipefail

BACKUP=1
DRY_RUN=0
INPUT=""
OUTPUT=""

for arg in "$@"; do
    case "$arg" in
        --no-backup) BACKUP=0 ;;
        --dry-run)   DRY_RUN=1 ;;
        *)
            if [[ -z "$INPUT" ]]; then
                INPUT="$arg"
            elif [[ -z "$OUTPUT" ]]; then
                OUTPUT="$arg"
            fi
            ;;
    esac
done

if [[ -z "$INPUT" ]]; then
    echo "Usage: $0 <input.gif> [output.gif] [--no-backup] [--dry-run]"
    exit 1
fi

if [[ ! -f "$INPUT" ]]; then
    echo "Error: file not found: $INPUT"
    exit 1
fi

if ! command -v ffmpeg &>/dev/null; then
    echo "Error: ffmpeg is required but not installed."
    exit 1
fi

if ! command -v python3 &>/dev/null; then
    echo "Error: python3 is required but not installed."
    exit 1
fi

# Default output: overwrite input in-place (after backup)
if [[ -z "$OUTPUT" ]]; then
    OUTPUT="$INPUT"
fi

INPLACE=0
if [[ "$OUTPUT" == "$INPUT" ]]; then
    INPLACE=1
fi

TMPFILE=$(mktemp --suffix=.gif)
trap 'rm -f "$TMPFILE"' EXIT

# Diagnose current disposal methods
DISPOSALS=$(python3 - "$INPUT" <<'EOF'
import sys

def read_gif_disposals(path):
    with open(path, 'rb') as f:
        data = f.read()
    disposals = []
    i = 0
    while i < len(data) - 4:
        if data[i] == 0x21 and data[i+1] == 0xF9 and data[i+2] == 0x04:
            packed = data[i+3]
            disposal = (packed >> 2) & 0x07
            disposals.append(disposal)
            i += 8
        else:
            i += 1
    return disposals

# disposal values: 0=unspecified, 1=do-not-dispose, 2=restore-bg, 3=restore-previous
disposals = read_gif_disposals(sys.argv[1])
names = {0: "unspecified", 1: "do-not-dispose", 2: "restore-background", 3: "restore-previous"}
unique = sorted(set(disposals))
print(f"{len(disposals)} frames, disposal methods: {[names.get(d, str(d)) for d in unique]}")
EOF
)

echo "Input:    $INPUT"
echo "Current:  $DISPOSALS"

if [[ "$DRY_RUN" -eq 1 ]]; then
    echo "[dry-run] Would re-encode to disposal=2 (restore-background) → $OUTPUT"
    exit 0
fi

# Two-pass palette + full-frame re-encode with disposal=2
ffmpeg -y -i "$INPUT" \
    -vf "split[s0][s1];[s0]palettegen=reserve_transparent=1:transparency_color=000000[p];[s1][p]paletteuse=alpha_threshold=128:dither=bayer" \
    -gifflags -offsetting \
    -loop 0 \
    "$TMPFILE" \
    -loglevel warning

# Verify
AFTER=$(python3 - "$TMPFILE" <<'EOF'
import sys

def read_gif_disposals(path):
    with open(path, 'rb') as f:
        data = f.read()
    disposals = []
    i = 0
    while i < len(data) - 4:
        if data[i] == 0x21 and data[i+1] == 0xF9 and data[i+2] == 0x04:
            packed = data[i+3]
            disposal = (packed >> 2) & 0x07
            disposals.append(disposal)
            i += 8
        else:
            i += 1
    return disposals

disposals = read_gif_disposals(sys.argv[1])
names = {0: "unspecified", 1: "do-not-dispose", 2: "restore-background", 3: "restore-previous"}
unique = sorted(set(disposals))
print(f"{len(disposals)} frames, disposal methods: {[names.get(d, str(d)) for d in unique]}")
EOF
)

echo "Fixed:    $AFTER"

if [[ "$INPLACE" -eq 1 && "$BACKUP" -eq 1 ]]; then
    cp "$INPUT" "${INPUT%.gif}_original.gif"
    echo "Backup:   ${INPUT%.gif}_original.gif"
fi

mv "$TMPFILE" "$OUTPUT"
echo "Output:   $OUTPUT"
