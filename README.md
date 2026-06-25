# papyrus

# ----Work in Progress----

## Simple text editor "from scratch" in C++
### Currently tested on Linux (Ubuntu). Other operating systems are not officially supported yet.

The goal of this project is to keep dependencies intentionally low and implement as much editor behavior as possible directly in C++. This is both a learning project and a practical exploration of editor internals (text handling, rendering, input, search, and command execution) with minimal framework overhead.

# Dependencies
- SDL3
- SDL3_ttf
- SDL3_image

The core application runtime is built around these SDL libraries. Test code additionally uses GoogleTest.

# Usage

```bash
papyrus [filename] [options]
```


## Options

| Short | Long | Description |
|---|---|---|
| `-h` | `--help` | Print usage information and exit. |

## Examples

```bash
# Launch editor
papyrus

# Launch and open a file
papyrus ./notes/todo.txt

# Show help
papyrus --help
```

# Current features
- Multi-pane editor workflow:
	- Editor screen and file browser screen with quick switching (`F3` / `F4`)
	- Integrated terminal panel toggle (`Ctrl+T`)
- Core text editing:
	- Insert/delete text, multiline editing, and cursor navigation
	- Selection support (including Shift-based selection and select-all)
	- Undo/redo (`Ctrl+Z` / `Ctrl+Y`)
	- Clipboard operations (`Ctrl+C`, `Ctrl+X`, `Ctrl+V`)
- File operations:
	- Open file from CLI (`papyrus [filename]`)
	- Open files and navigate directories in the built-in file browser
	- Save current file from editor shortcut (`Ctrl+S`) or terminal command
- Search:
	- In-editor find (`Ctrl+F`) with highlighted matches
	- Match navigation and match count tracking
- Syntax highlighting:
	- C++ lexer/tokenization with themed token rendering
	- Unsupported languages are currently treated as not-highlighted
- Integrated terminal commands:
	- Built-in commands including `open`, `save`, `quit`, `build`, and `cl` (change language)
	- Shell passthrough via `!command`
	- Command history navigation
- UI and rendering:
	- Line numbers, scrolling viewports, themed colors, and cursor blink
	- Adjustable font size (`Ctrl++`, `Ctrl+-`)

### Status note
The editor is actively evolving. Core workflows are functional, while several advanced features are still in progress.

# Planned features/requirements:
- Search and replace workflow (replace is not implemented yet)
- Broader language support and lexer coverage beyond current C++ focus
- Autocomplete and code-aware suggestions
- More configurable editor behavior (themes, keybinds, preferences)
- Command system improvements toward a command-palette-like experience
- Continued terminal UX improvements
- Cross-platform hardening (currently Linux-first)
- Optional plugin/extension model exploration
- Rendering experiments and performance improvements
- CI and release automation maturation
  