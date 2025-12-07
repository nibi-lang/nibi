#!/bin/bash

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

show_usage() {
    echo "Usage: $0 [--vim|--vscode|--cursor]"
    echo ""
    echo "Install Nibi syntax highlighting for various editors:"
    echo "  --vim     Install syntax file for Vim/Neovim"
    echo "  --vscode  Install extension for VSCode"
    echo "  --cursor  Install extension for Cursor"
    echo ""
    echo "You can specify multiple editors at once."
    echo "If no option is provided, usage information is displayed."
    exit 1
}

install_vim() {
    echo "Installing Vim syntax file..."
    
    VIM_SYNTAX_DIR="$HOME/.config/nvim/syntax"
    mkdir -p "$VIM_SYNTAX_DIR"
    
    cp "$SCRIPT_DIR/nibi.vim" "$VIM_SYNTAX_DIR/"
    
    echo "✓ Vim syntax file installed to $VIM_SYNTAX_DIR/nibi.vim"
    echo "  Add 'au BufRead,BufNewFile *.nibi set filetype=nibi' to your init.vim/init.lua"
}

install_vscode() {
    echo "Installing VSCode extension..."
    
    VSCODE_EXT_DIR="$HOME/.vscode/extensions/nibi-lang"
    mkdir -p "$VSCODE_EXT_DIR/syntaxes"
    mkdir -p "$VSCODE_EXT_DIR/icons"
    
    cp "$SCRIPT_DIR/vscode/package.json" "$VSCODE_EXT_DIR/"
    cp "$SCRIPT_DIR/vscode/language-configuration.json" "$VSCODE_EXT_DIR/"
    cp "$SCRIPT_DIR/vscode/syntaxes/nibi.tmLanguage.json" "$VSCODE_EXT_DIR/syntaxes/"
    cp "$SCRIPT_DIR/icons/sxs-icon-dark.svg" "$VSCODE_EXT_DIR/icons/"
    cp "$SCRIPT_DIR/icons/sxs-icon-light.svg" "$VSCODE_EXT_DIR/icons/"
    
    echo "✓ VSCode extension installed to $VSCODE_EXT_DIR"
    echo "  Restart VSCode to activate the extension"
}

install_cursor() {
    echo "Installing Cursor extension..."
    
    CURSOR_EXT_DIR="$HOME/.cursor/extensions/nibi-lang"
    mkdir -p "$CURSOR_EXT_DIR/syntaxes"
    mkdir -p "$CURSOR_EXT_DIR/icons"
    
    cp "$SCRIPT_DIR/vscode/package.json" "$CURSOR_EXT_DIR/"
    cp "$SCRIPT_DIR/vscode/language-configuration.json" "$CURSOR_EXT_DIR/"
    cp "$SCRIPT_DIR/vscode/syntaxes/nibi.tmLanguage.json" "$CURSOR_EXT_DIR/syntaxes/"
    cp "$SCRIPT_DIR/icons/sxs-icon-dark.svg" "$CURSOR_EXT_DIR/icons/"
    cp "$SCRIPT_DIR/icons/sxs-icon-light.svg" "$CURSOR_EXT_DIR/icons/"
    
    echo "✓ Cursor extension installed to $CURSOR_EXT_DIR"
    echo "  Restart Cursor to activate the extension"
}

if [ $# -eq 0 ]; then
    show_usage
fi

INSTALLED_ANY=false

while [ $# -gt 0 ]; do
    case "$1" in
        --vim)
            install_vim
            INSTALLED_ANY=true
            ;;
        --vscode)
            install_vscode
            INSTALLED_ANY=true
            ;;
        --cursor)
            install_cursor
            INSTALLED_ANY=true
            ;;
        --help|-h)
            show_usage
            ;;
        *)
            echo "Error: Unknown option '$1'"
            show_usage
            ;;
    esac
    shift
done

if [ "$INSTALLED_ANY" = true ]; then
    echo ""
    echo "Installation complete!"
fi
