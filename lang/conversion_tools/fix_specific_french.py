#!/usr/bin/env python3
"""
Simple script to fix specific French text patterns in source files.
"""

import os
import re


def fix_specific_french_text(file_path: str) -> bool:
    """
    Fix specific French text patterns in a source file.
    """
    if not os.path.exists(file_path):
        print(f"File not found: {file_path}")
        return False

    try:
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()

        original_content = content
        updates_made = 0

        # Specific French to English replacements
        replacements = [
            ('"Add text d\'élément"', '"Add element text"'),
            ('"Add a group of texts d\'élément"', '"Add a group of element texts"'),
            ('"Grouper des texts d\'élément"', '"Group element texts"'),
            (
                '"Delete un groupe de texts d\'élément"',
                '"Delete a group of element texts"',
            ),
            (
                '"Insérer un text d\'élément dans un groupe de texts"',
                '"Insert element text into a text group"',
            ),
            (
                "\"Enlever un text d'élément d'un groupe de texts\"",
                '"Remove an element text from a group of texts"',
            ),
            (
                "\"Modifier l'alinement d'un groupe de texts\"",
                '"Modify the alignment of a group of texts"',
            ),
            ('"Edit les référence croisé"', '"Edit the cross reference"'),
            ('"Rotate la selection"', '"Rotate the selection"'),
            (
                '"Choose texts orientation sélectionnés"',
                '"Choose orientation for selected texts"',
            ),
        ]

        for french_text, english_text in replacements:
            if french_text in content:
                print(f"  Replacing: {french_text} -> {english_text}")
                content = content.replace(french_text, english_text)
                updates_made += 1

        if updates_made > 0:
            with open(file_path, "w", encoding="utf-8") as f:
                f.write(content)
            print(f"  Updated {file_path} ({updates_made} changes)")
            return True
        else:
            print(f"  No French text found in {file_path}")
            return True

    except Exception as e:
        print(f"Error updating {file_path}: {e}")
        return False


def main():
    print("=== QElectroTech French Text Fixer (Simple) ===\n")

    # Test with the specific file that has issues
    test_file = "../sources/undocommand/addelementtextcommand.cpp"

    if os.path.exists(test_file):
        print(f"Fixing file: {test_file}")
        success = fix_specific_french_text(test_file)
        if success:
            print("Fix completed successfully!")
        else:
            print("Fix failed!")
    else:
        print(f"Test file not found: {test_file}")


if __name__ == "__main__":
    main()
