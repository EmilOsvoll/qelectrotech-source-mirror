#!/usr/bin/env python3
"""
Simple and direct conversion script for language files
"""

import xml.etree.ElementTree as ET
import sys
import os


def convert_language_file_direct(
    input_file: str, output_file: str, english_reference: str
) -> bool:
    """
    Convert a language file using a direct approach.
    """

    # Parse the original English file to get French->English mapping
    print(f"Parsing original English file: qet_en.ts")
    tree = ET.parse("qet_en.ts")
    root = tree.getroot()
    french_to_english = {}

    for message in root.findall(".//message"):
        source_elem = message.find("source")
        translation_elem = message.find("translation")

        if source_elem is not None and translation_elem is not None:
            french_source = source_elem.text or ""
            english_translation = translation_elem.text or ""

            if french_source.strip() and english_translation.strip():
                french_to_english[french_source] = english_translation

    print(f"Found {len(french_to_english)} French->English mappings")

    # Parse the input file
    print(f"Parsing input file: {input_file}")
    tree = ET.parse(input_file)
    root = tree.getroot()

    converted_count = 0

    # Convert each message
    for message in root.findall(".//message"):
        source_elem = message.find("source")
        translation_elem = message.find("translation")

        if source_elem is not None and translation_elem is not None:
            french_source = source_elem.text or ""

            if french_source in french_to_english:
                # Replace French source with English source
                english_source = french_to_english[french_source]
                source_elem.text = english_source
                converted_count += 1

    # Write the converted file
    try:
        os.makedirs(os.path.dirname(output_file), exist_ok=True)
        tree.write(output_file, encoding="utf-8", xml_declaration=True)

        print(f"Successfully converted {converted_count} translations")
        print(f"Output written to: {output_file}")
        return True

    except Exception as e:
        print(f"Error writing output file: {e}")
        return False


def main():
    if len(sys.argv) != 3:
        print("Usage: python convert_direct.py <input_file> <output_file>")
        print("Example: python convert_direct.py qet_de.ts todo/qet_de_converted.ts")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    success = convert_language_file_direct(input_file, output_file, "")
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
