#!/usr/bin/env python3
"""
Test script to update a single source file
"""

import xml.etree.ElementTree as ET
import os


def test_single_file_update():
    """
    Test updating a single file to verify the approach works.
    """

    # Get French to English mapping
    french_to_english = {}
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

    print(f"Found {len(french_to_english)} French->English mappings")

    # Test with aboutqetdialog.ui
    test_file = "../sources/ui/aboutqetdialog.ui"

    if not os.path.exists(test_file):
        print(f"File not found: {test_file}")
        return

    print(f"Testing with file: {test_file}")

    # Read the file
    with open(test_file, "r", encoding="utf-8") as f:
        content = f.read()

    # Find French text that needs to be replaced
    updates_made = 0
    for french_text, english_text in french_to_english.items():
        if french_text in content:
            print(f"Found French text: '{french_text}' -> '{english_text}'")
            content = content.replace(french_text, english_text)
            updates_made += 1

    print(f"Made {updates_made} updates")

    if updates_made > 0:
        # Write the updated content
        with open(test_file, "w", encoding="utf-8") as f:
            f.write(content)
        print(f"Updated {test_file}")
    else:
        print("No updates needed")


if __name__ == "__main__":
    test_single_file_update()
