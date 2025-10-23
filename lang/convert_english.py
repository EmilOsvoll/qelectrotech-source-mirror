#!/usr/bin/env python3
"""
QElectroTech English Translation Converter

This script converts the English translation file (qet_en.ts) to use English as the source language.
It swaps the source and translation tags, making English the source and clearing the translations.
"""

import xml.etree.ElementTree as ET
import sys
import os


def convert_english_file(input_file: str, output_file: str) -> bool:
    """
    Convert the English translation file to use English as source language.

    Args:
        input_file: Path to the input qet_en.ts file
        output_file: Path to the output file

    Returns:
        True if conversion was successful, False otherwise
    """

    try:
        # Parse the input file
        print(f"Parsing input file: {input_file}")
        tree = ET.parse(input_file)
        root = tree.getroot()

        converted_count = 0

        # Process each message
        for message in root.findall(".//message"):
            source_elem = message.find("source")
            translation_elem = message.find("translation")

            if source_elem is not None and translation_elem is not None:
                # Get the current values
                french_source = source_elem.text or ""
                english_translation = translation_elem.text or ""

                # Only skip if both source and translation are empty
                if not french_source.strip() and not english_translation.strip():
                    continue

                # Swap: English translation becomes source, French source becomes empty translation
                if english_translation.strip():
                    source_elem.text = english_translation
                    translation_elem.text = ""  # Empty since English is now the source
                else:
                    # If no English translation, keep the French source but clear translation
                    translation_elem.text = ""

                converted_count += 1

        # Write the converted file
        os.makedirs(os.path.dirname(output_file), exist_ok=True)

        # Write with proper formatting
        tree.write(output_file, encoding="utf-8", xml_declaration=True)

        print(f"Successfully converted {converted_count} translations")
        print(f"Output written to: {output_file}")
        return True

    except ET.ParseError as e:
        print(f"Error parsing {input_file}: {e}")
        return False
    except FileNotFoundError:
        print(f"File not found: {input_file}")
        return False
    except Exception as e:
        print(f"Error during conversion: {e}")
        return False


def main():
    if len(sys.argv) != 3:
        print("Usage: python convert_english.py <input_file> <output_file>")
        print("Example: python convert_english.py qet_en.ts todo/qet_en_converted.ts")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    success = convert_english_file(input_file, output_file)
    sys.exit(0 if success else 1)


if __name__ == "__main__":
    main()
