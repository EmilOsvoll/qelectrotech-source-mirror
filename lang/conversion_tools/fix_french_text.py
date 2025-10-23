#!/usr/bin/env python3
"""
Script to identify and fix French text in source files that wasn't properly translated.
This script will find French text patterns and suggest English replacements.
"""

import os
import re
from typing import Dict, List, Tuple, Set


def find_french_patterns_in_file(file_path: str) -> List[Tuple[int, str]]:
    """
    Find French text patterns in a source file.
    Returns list of (line_number, french_text)
    """
    french_patterns = []

    try:
        with open(file_path, "r", encoding="utf-8") as f:
            lines = f.readlines()

        for i, line in enumerate(lines, 1):
            # Look for French text in tr() calls
            tr_matches = re.findall(
                r'tr\(["\']([^"\']*[àâäéèêëïîôöùûüÿçÀÂÄÉÈÊËÏÎÔÖÙÛÜŸÇ][^"\']*)["\']',
                line,
            )
            for match in tr_matches:
                french_patterns.append((i, match))

            # Look for French text in QStringLiteral calls
            qstring_matches = re.findall(
                r'QStringLiteral\(["\']([^"\']*[àâäéèêëïîôöùûüÿçÀÂÄÉÈÊËÏÎÔÖÙÛÜŸÇ][^"\']*)["\']',
                line,
            )
            for match in qstring_matches:
                french_patterns.append((i, match))

            # Look for French text in setText calls
            settext_matches = re.findall(
                r'setText\([^)]*tr\(["\']([^"\']*[àâäéèêëïîôöùûüÿçÀÂÄÉÈÊËÏÎÔÖÙÛÜŸÇ][^"\']*)["\']',
                line,
            )
            for match in settext_matches:
                french_patterns.append((i, match))

    except Exception as e:
        print(f"Error reading {file_path}: {e}")

    return french_patterns


def suggest_english_translation(french_text: str) -> str:
    """
    Suggest English translation for French text based on common patterns.
    """
    # Common French to English mappings
    translations = {
        "Add text d'élément": "Add element text",
        "Add a group of texts d'élément": "Add a group of element texts",
        "Grouper des texts d'élément": "Group element texts",
        "Delete un groupe de texts d'élément": "Delete a group of element texts",
        "Insérer un text d'élément dans un groupe de texts": "Insert element text into a text group",
        "Enlever un text d'élément d'un groupe de texts": "Remove an element text from a group of texts",
        "Modifier l'alinement d'un groupe de texts": "Modify the alignment of a group of texts",
        "Edit les référence croisé": "Edit the cross reference",
        "Rotate la selection": "Rotate the selection",
        "Choose texts orientation sélectionnés": "Choose orientation for selected texts",
        "modify the title block": "modify the title block",  # Already in English
    }

    # Direct mapping
    if french_text in translations:
        return translations[french_text]

    # Pattern-based translations
    if "d'élément" in french_text:
        return french_text.replace("d'élément", "element")
    if "texts d'élément" in french_text:
        return french_text.replace("texts d'élément", "element texts")
    if "text d'élément" in french_text:
        return french_text.replace("text d'élément", "element text")
    if "référence croisé" in french_text:
        return french_text.replace("référence croisé", "cross reference")
    if "la selection" in french_text:
        return french_text.replace("la selection", "the selection")

    # If no pattern matches, return the original text
    return french_text


def fix_french_text_in_file(file_path: str) -> bool:
    """
    Fix French text in a source file by replacing it with English equivalents.
    """
    if not os.path.exists(file_path):
        print(f"File not found: {file_path}")
        return False

    try:
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()

        original_content = content
        updates_made = 0

        # Find and replace French text patterns
        french_patterns = find_french_patterns_in_file(file_path)

        for line_num, french_text in french_patterns:
            english_text = suggest_english_translation(french_text)
            if english_text != french_text:
                print(f"  Line {line_num}: '{french_text}' -> '{english_text}'")
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
    print("=== QElectroTech French Text Fixer ===\n")

    # Test with the specific file that has issues
    test_file = "../sources/undocommand/addelementtextcommand.cpp"

    if os.path.exists(test_file):
        print(f"Analyzing file: {test_file}")
        french_patterns = find_french_patterns_in_file(test_file)

        if french_patterns:
            print(f"Found {len(french_patterns)} French text patterns:")
            for line_num, french_text in french_patterns:
                english_text = suggest_english_translation(french_text)
                print(f"  Line {line_num}: '{french_text}' -> '{english_text}'")

            print(f"\nFixing file: {test_file}")
            success = fix_french_text_in_file(test_file)
            if success:
                print("Fix completed successfully!")
            else:
                print("Fix failed!")
        else:
            print("No French text patterns found.")
    else:
        print(f"Test file not found: {test_file}")


if __name__ == "__main__":
    main()
