#!/usr/bin/env python3
"""
Script to fix source files that still contain French text.
This script will specifically target the mismatches found by the validation script.
"""

import xml.etree.ElementTree as ET
import os
import re
from typing import Dict, List, Tuple


def get_french_to_english_mapping() -> Dict[str, str]:
    """
    Get the mapping from French text to English text from the original English file.
    """
    french_to_english = {}

    try:
        tree = ET.parse("qet_en.ts")
        root = tree.getroot()

        for message in root.findall(".//message"):
            source_elem = message.find("source")
            translation_elem = message.find("translation")

            if source_elem is not None and translation_elem is not None:
                french_source = source_elem.text or ""
                english_translation = translation_elem.text or ""

                if french_source.strip() and english_translation.strip():
                    french_to_english[french_source] = english_translation

    except Exception as e:
        print(f"Error parsing qet_en.ts: {e}")

    return french_to_english


def fix_source_file(file_path: str, french_to_english: Dict[str, str]) -> bool:
    """
    Fix a source file by replacing French text with English text.
    """
    if not os.path.exists(file_path):
        print(f"File not found: {file_path}")
        return False

    try:
        # Read the file
        with open(file_path, "r", encoding="utf-8") as f:
            content = f.read()

        original_content = content

        # Find French text that needs to be replaced
        updates_made = 0
        for french_text, english_text in french_to_english.items():
            if french_text in content:
                print(f"  Replacing: '{french_text}' -> '{english_text}'")
                content = content.replace(french_text, english_text)
                updates_made += 1

        if updates_made > 0:
            # Write the updated content
            with open(file_path, "w", encoding="utf-8") as f:
                f.write(content)
            print(f"  Updated {file_path} ({updates_made} changes)")
            return True
        else:
            print(f"  No updates needed for {file_path}")
            return True

    except Exception as e:
        print(f"Error updating {file_path}: {e}")
        return False


def main():
    print("=== QElectroTech Source File French Text Fixer ===\n")

    # Get French to English mapping
    french_to_english = get_french_to_english_mapping()
    print(f"Found {len(french_to_english)} French->English mappings")

    # Test with a specific file that we know has issues
    test_file = "../sources/undocommand/addelementtextcommand.cpp"

    if os.path.exists(test_file):
        print(f"\nTesting with file: {test_file}")
        success = fix_source_file(test_file, french_to_english)
        if success:
            print("Test successful!")
        else:
            print("Test failed!")
    else:
        print(f"Test file not found: {test_file}")


if __name__ == "__main__":
    main()
