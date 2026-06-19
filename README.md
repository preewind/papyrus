# papyrus

#  ----Work in Progess!---- 

## Simple text editor "from scratch" in C++
### currently only tested on Linux (Ubuntu) - no guarantees to work on other OS

The idea here is to use as few libraries as possible and write everything I need from scratch (for the most part) to learn about the specific techniques used in certain areas. I am using C++ to have a relatively low level language, but more conveniences compared to using bare C.

# Dependencies
- SDL3
- SDL-ttf

# Usage

## Synopsis

```bash
papyrus [filename] [options]
```

## Description

`papyrus` starts the editor UI and optionally opens a file at startup.

## Arguments

| Name | Type | Required | Description |
|---|---|---|---|
| `filename` | Positional | No | Path to a file to open on launch. |

## Options

| Short | Long | Description |
|---|---|---|
| `-h` | `--help` | Print usage information and exit. |

## Exit Codes

| Code | Meaning |
|---|---|
| `0` | Success (normal launch or `--help`). |
| `2` | Invalid command-line arguments. |

## Validation Rules

- Only one positional argument (`filename`) is supported.
- Unknown options are rejected.
- `-h` / `--help` do not accept values.

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
- Basic editor UI with simple SDL text rendering
- Scrolling
- Many common shortcuts
- Selection
- Copy/Paste
- File browser

### Warning: most of the features are not fully implemented yet

# Planned features/requirements:
- Editor itself (UI)
- Different text rendering approaches, OpenGL, Vulkan, SDF etc.
- Syntax highlighting
- Simple autocomplete recommendations
- Search & Replace
- Highly customizable
- CI to guarantee functionality
- VSCode like command palette
- Terminal integration
- Platform independence
- Terminal mode? (Launching in terminal like VIM)
- Plugins to extend functionality?
- ...
  