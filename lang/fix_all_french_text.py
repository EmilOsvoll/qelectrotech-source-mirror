#!/usr/bin/env python3
"""
Comprehensive script to fix all French text in QElectroTech source files.
"""

import os
import re
from typing import List, Tuple


def find_all_source_files() -> List[str]:
    """
    Find all C++ source files in the sources directory.
    """
    source_files = []
    for root, dirs, files in os.walk("../sources"):
        for file in files:
            if file.endswith((".cpp", ".h")):
                source_files.append(os.path.join(root, file))
    return source_files


def fix_french_text_in_file(file_path: str) -> Tuple[int, List[str]]:
    """
    Fix French text in a source file.
    Returns (number_of_changes, list_of_changes_made)
    """
    if not os.path.exists(file_path):
        return 0, []

    try:
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()

        changes_made = []
        updates_made = 0

        # Comprehensive French to English replacements
        replacements = [
            # Element text related
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
            # Cross reference related
            ('"Edit les référence croisé"', '"Edit the cross reference"'),
            ('"edite the cross reference"', '"edit the cross reference"'),
            # Selection related
            ('"Rotate la selection"', '"Rotate the selection"'),
            (
                '"Choose texts orientation sélectionnés"',
                '"Choose orientation for selected texts"',
            ),
            # Move related
            ('"move %1% {1?}"', '"move %1"'),
            # Delete related
            ('"delete %1% {1?}"', '"delete %1"'),
            # Groups related
            ('" %1% {1?} groupes de texts"', '" %1 groups of texts"'),
            ('" %1% {1?} texts"', '" %1 texts"'),
            # Title block related
            (
                '"modify the title block"',
                '"modify the title block"',
            ),  # Already in English
            # Other common patterns
            ('"Affiche&r en contacts"', '"&View contacts"'),
            ('"Display en croix"', '"View cross"'),
            (
                '"Display les contacts de puissance dans la croix"',
                '"Show the power contacts in the cross"',
            ),
            ('"XRef Greenical Offset:"', '"XRef Vertical Offset:"'),
            (
                '"Set Greenical Offset for the Cross References. 10px corresponds to 1 tile displacement."',
                '"Set Vertical Offset for the Cross References. 10px corresponds to 1 tile displacement."',
            ),
            ('"Tofault - Fit to XRef height"', '"Default - Fit to XRef height"'),
        ]

        for french_text, english_text in replacements:
            if french_text in content:
                changes_made.append(f"  {french_text} -> {english_text}")
                content = content.replace(french_text, english_text)
                updates_made += 1

        if updates_made > 0:
            with open(file_path, "w", encoding="utf-8") as f:
                f.write(content)

        return updates_made, changes_made

    except Exception as e:
        print(f"Error updating {file_path}: {e}")
        return 0, []


def main():
    print("=== QElectroTech Comprehensive French Text Fixer ===\n")

    # Find all source files
    source_files = find_all_source_files()
    print(f"Found {len(source_files)} source files to check")

    total_changes = 0
    files_updated = 0

    for file_path in source_files:
        # Convert to relative path for display
        rel_path = file_path.replace("\\", "/")

        updates_made, changes_made = fix_french_text_in_file(file_path)

        if updates_made > 0:
            print(f"\n--- Updated {rel_path} ({updates_made} changes) ---")
            for change in changes_made:
                print(change)
            files_updated += 1
            total_changes += updates_made

    print(f"\n=== Summary ===")
    print(f"Files checked: {len(source_files)}")
    print(f"Files updated: {files_updated}")
    print(f"Total changes: {total_changes}")


if __name__ == "__main__":
    main()
