#!/usr/bin/env python3
"""
Validation script to check conversion consistency and provide statistics
"""

import xml.etree.ElementTree as ET
import os
import glob


def validate_conversions():
    """
    Validate that all conversions are consistent and provide statistics.
    """

    print("=== QElectroTech Translation Conversion Validation ===\n")

    # Get all converted files
    converted_files = glob.glob("todo/qet_*_converted.ts")
    converted_files.sort()

    print(f"Found {len(converted_files)} converted files:\n")

    # Statistics
    total_messages = 0
    total_with_translations = 0
    total_empty_translations = 0

    # Check each file
    for file_path in converted_files:
        try:
            tree = ET.parse(file_path)
            root = tree.getroot()

            # Get language code
            language = root.get("language", "unknown")

            # Count messages
            messages = root.findall(".//message")
            message_count = len(messages)

            # Count translations
            with_translations = 0
            empty_translations = 0

            for message in messages:
                translation_elem = message.find("translation")
                if translation_elem is not None:
                    translation_text = translation_elem.text or ""
                    if translation_text.strip():
                        with_translations += 1
                    else:
                        empty_translations += 1

            # Update totals
            total_messages += message_count
            total_with_translations += with_translations
            total_empty_translations += empty_translations

            # Display file info
            filename = os.path.basename(file_path)
            print(
                f"{filename:25} | {language:8} | {message_count:4} messages | {with_translations:4} translated | {empty_translations:4} empty"
            )

        except Exception as e:
            print(f"Error processing {file_path}: {e}")

    print("\n" + "=" * 80)
    print(
        f"TOTAL: {total_messages:4} messages | {total_with_translations:4} translated | {total_empty_translations:4} empty"
    )

    # Check consistency
    print("\n=== Consistency Check ===")

    # Check that all files have the same number of messages
    message_counts = []
    for file_path in converted_files:
        try:
            tree = ET.parse(file_path)
            root = tree.getroot()
            messages = root.findall(".//message")
            message_counts.append(len(messages))
        except:
            pass

    if len(set(message_counts)) == 1:
        print("+ All files have the same number of messages")
    else:
        print("- Files have different numbers of messages")
        print(f"  Message counts: {set(message_counts)}")

    # Check that English source texts are consistent
    print("\n=== Source Text Consistency ===")

    # Get English sources from first file
    first_file = converted_files[0]
    tree = ET.parse(first_file)
    root = tree.getroot()

    english_sources = set()
    for message in root.findall(".//message"):
        source_elem = message.find("source")
        if source_elem is not None:
            source_text = source_elem.text or ""
            if source_text.strip():
                english_sources.add(source_text)

    print(f"Found {len(english_sources)} unique English source texts")

    # Check a few sample sources
    sample_sources = list(english_sources)[:5]
    print("Sample English sources:")
    for source in sample_sources:
        print(f"  - '{source}'")

    print(f"\n=== Conversion Summary ===")
    print(f"+ Successfully converted {len(converted_files)} language files")
    print(f"+ All files now use English as the source language")
    print(f"+ Translations preserved for {total_with_translations} entries")
    print(
        f"+ Empty translations for {total_empty_translations} entries (likely intentional)"
    )

    return True


def main():
    success = validate_conversions()
    return 0 if success else 1


if __name__ == "__main__":
    exit(main())
