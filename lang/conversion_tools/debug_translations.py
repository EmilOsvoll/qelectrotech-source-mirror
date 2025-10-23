#!/usr/bin/env python3
"""
Debug script to understand the translation file structure
"""

import xml.etree.ElementTree as ET

def debug_file(filepath):
    print(f"\n=== Debugging {filepath} ===")
    tree = ET.parse(filepath)
    root = tree.getroot()
    
    count = 0
    for message in root.findall('.//message'):
        source_elem = message.find('source')
        translation_elem = message.find('translation')
        
        if source_elem is not None and translation_elem is not None:
            source_text = source_elem.text or ""
            translation_text = translation_elem.text or ""
            
            if count < 5:  # Show first 5 examples
                print(f"Source: '{source_text}' -> Translation: '{translation_text}'")
            
            count += 1
    
    print(f"Total messages: {count}")

if __name__ == '__main__':
    debug_file('qet_en.ts')
    debug_file('qet_de.ts')
    debug_file('todo/qet_en_converted.ts')
