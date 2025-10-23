#!/usr/bin/env python3
"""
Detailed debug script to understand the mapping issue
"""

import xml.etree.ElementTree as ET

def debug_mapping():
    print("=== Debugging Mapping Logic ===")
    
    # Parse original English file
    tree = ET.parse('qet_en.ts')
    root = tree.getroot()
    original_english_map = {}
    
    for message in root.findall('.//message'):
        source_elem = message.find('source')
        translation_elem = message.find('translation')
        
        if source_elem is not None and translation_elem is not None:
            source_text = source_elem.text or ""
            translation_text = translation_elem.text or ""
            original_english_map[source_text] = translation_text
    
    # Parse converted English file
    tree = ET.parse('todo/qet_en_converted.ts')
    root = tree.getroot()
    converted_english_map = {}
    
    for message in root.findall('.//message'):
        source_elem = message.find('source')
        translation_elem = message.find('translation')
        
        if source_elem is not None and translation_elem is not None:
            source_text = source_elem.text or ""
            translation_text = translation_elem.text or ""
            converted_english_map[source_text] = translation_text
    
    # Parse German file
    tree = ET.parse('qet_de.ts')
    root = tree.getroot()
    german_map = {}
    
    for message in root.findall('.//message'):
        source_elem = message.find('source')
        translation_elem = message.find('translation')
        
        if source_elem is not None and translation_elem is not None:
            source_text = source_elem.text or ""
            translation_text = translation_elem.text or ""
            german_map[source_text] = translation_text
    
    print(f"Original English file entries: {len(original_english_map)}")
    print(f"Converted English file entries: {len(converted_english_map)}")
    print(f"German file entries: {len(german_map)}")
    
    # Test mapping logic
    print("\n=== Testing Mapping Logic ===")
    test_french_source = "À propos de QElectrotech"
    
    if test_french_source in original_english_map:
        english_translation = original_english_map[test_french_source]
        print(f"French source: '{test_french_source}'")
        print(f"English translation: '{english_translation}'")
        
        if english_translation in converted_english_map:
            print(f"Found in converted English file: '{english_translation}'")
        else:
            print(f"NOT found in converted English file: '{english_translation}'")
            
        # Check if this French source exists in German file
        if test_french_source in german_map:
            german_translation = german_map[test_french_source]
            print(f"German translation: '{german_translation}'")
        else:
            print(f"French source not found in German file")
    else:
        print(f"French source not found in original English file")

if __name__ == '__main__':
    debug_mapping()
