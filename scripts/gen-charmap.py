#!/usr/bin/python2

import os
import string

def convert_to_bin(binstring):
    binstring = binstring.replace(" ", "")
    return "0b%s" % binstring

def gen_charmap():
    charmap = {}

    for i in range(0, 128, 1):
        charmap[i] = [
            "0000 0000",
            "0000 0000",
            "0000 0000"
        ]

    charmap[ord('0')] = [
        "0111 1110",
        "1000 0001",
        "0111 1110"]

    charmap[ord('1')] = [
        "1000 0010",
        "1111 1111",
        "1000 0000"]

    charmap[ord('2')] = [
        "1110 0010",
        "1001 0001",
        "1000 1110"]

    charmap[ord('3')] = [
        "1001 0001",
        "1001 1001",
        "0110 0111"]

    charmap[ord('4')] = [
        "0000 1111",
        "0000 1000",
        "1111 1111"]

    charmap[ord('5')] = [
        "1000 1111",
        "1000 1001",
        "0111 0001"]

    charmap[ord('6')] = [
        "0111 1110",
        "1000 1001",
        "0111 0001"]

    charmap[ord('7')] = [
        "0000 0001",
        "1111 0001",
        "0000 1111"]

    charmap[ord('8')] = [
        "0111 1110",
        "1001 1001",
        "0111 1110"]

    charmap[ord('9')] = [
        "1000 1110",
        "1001 0001",
        "0111 1110"]

    charmap[ord('A')] = [
        "1111 1110",
        "0001 1001",
        "1111 1110"]

    charmap[ord('B')] = [
        "1111 1111",
        "1001 1001",
        "0110 0110"]

    charmap[ord('C')] = [
        "0111 1110",
        "1000 0001",
        "1000 0001"]

    charmap[ord('D')] = [
        "1111 1111",
        "1000 0001",
        "0111 1110"]

    charmap[ord('E')] = [
        "1111 1111",
        "1001 1001",
        "1000 0001"]

    charmap[ord('F')] = [
        "1111 1111",
        "0000 1001",
        "0000 0001"]

    charmap[ord('G')] = [
        "0110 1110",
        "1001 0001",
        "1111 0010"]

    charmap[ord('H')] = [
        "1111 1111",
        "0001 1000",
        "1111 1111"]

    charmap[ord('I')] = [
        "1000 0001",
        "1111 1111",
        "1000 0001"]

    charmap[ord('J')] = [
        "0100 0001",
        "1000 0001",
        "0111 1111"]

    charmap[ord('K')] = [
        "1111 1111",
        "0001 1000",
        "1110 0111"]

    charmap[ord('L')] = [
        "1111 1111",
        "1000 0000",
        "1000 0000"]

    charmap[ord('M')] = [
        "1111 1111",
        "0000 1100",
        "1111 1111"]

    charmap[ord('N')] = [
        "1111 1111",
        "0011 1000",
        "1111 1111"]

    charmap[ord('O')] = [
        "1111 1111",
        "1000 0001",
        "1111 1111"]

    charmap[ord('P')] = [
        "1111 1111",
        "0001 0001",
        "0000 1110"]

    charmap[ord('Q')] = [
        "0111 1110",
        "1010 0001",
        "1111 1110"]

    charmap[ord('R')] = [
        "1111 1111",
        "0001 0001",
        "1110 1110"]

    charmap[ord('S')] = [
        "1000 1110",
        "1001 1001",
        "0111 0001"]

    charmap[ord('T')] = [
        "0000 0001",
        "1111 1111",
        "0000 0001"]

    charmap[ord('U')] = [
        "1111 1111",
        "1000 0000",
        "1111 1111"]

    charmap[ord('V')] = [
        "0111 1111",
        "1100 0000",
        "0111 1111"]

    charmap[ord('W')] = [
        "1111 1111",
        "0011 0000",
        "1111 1111"]

    charmap[ord('X')] = [
        "1110 0111",
        "0001 1000",
        "1110 0111"]

    charmap[ord('Y')] = [
        "0000 0111",
        "1111 1000",
        "0000 0111"]

    charmap[ord('Z')] = [
        "1110 0001",
        "1001 1001",
        "1000 0111"]

    charmap[ord('!')] = [
        "0000 0000",
        "1011 1111",
        "0000 0000"]

    charmap[ord('?')] = [
        "0000 0010",
        "1011 0001",
        "0000 1110"]

    charmap[ord('.')] = [
        "0000 0000",
        "1000 0000",
        "0000 0000"]

    charmap[ord(',')] = [
        "1000 0000",
        "0100 0000",
        "0000 0000"]

    charmap[ord(':')] = [
        "0000 0000",
        "1000 1000",
        "0000 0000"]

    charmap[ord(';')] = [
        "1000 0000",
        "0100 1000",
        "0000 0000"]

    charmap[ord('_')] = [
        "1000 0000",
        "1000 0000",
        "1000 0000"]

    charmap[ord('-')] = [
        "0000 1000",
        "0000 1000",
        "0000 1000"]

    charmap[ord('=')] = [
        "0001 1000",
        "0001 1000",
        "0001 1000"]

    charmap[ord('\\')] = [
        "0000 0011",
        "0011 1100",
        "1100 0000"]

    charmap[ord('|')] = [
        "0000 0000",
        "1111 1111",
        "0000 0000"]

    charmap[ord('/')] = [
        "1100 0000",
        "0011 1100",
        "0000 0011"]

    charmap[ord('+')] = [
        "0000 1000",
        "0011 1110",
        "0000 1000"]

    charmap[ord('*')] = [
        "0011 1011",
        "0000 1100",
        "0011 1011"]

    for i in range(ord('A'), ord('Z') + 1, 1):
        offset = i - ord('A')
        charmap[ord('a') + offset] = charmap[i]

    return charmap


def gen_source(charmap):
    source = []

    source.append("#ifndef CHARMAP_H")
    source.append("#define CHARMAP_H")
    source.append("")
    source.append("#define CHAR_WIDTH 3")
    source.append("")
    source.append("static const char charmap[128][CHAR_WIDTH] = {")
    for i in range(0, 128, 1):
        c = chr(i)
        symbol = "control"
        if c.isspace():
            symbol = "whitespace"
        elif c in string.printable:
            symbol = c

        source.append("")
        source.append("    /* [%d] %s */" % (i, symbol))
        source.append("    { %s," % convert_to_bin(charmap[i][0]))
        source.append("      %s," % convert_to_bin(charmap[i][1]))
        source.append("      %s }," % convert_to_bin(charmap[i][2]))

    source.append("")
    source.append("}; /* static const charmap[128][CHAR_WIDTH] */")
    source.append("")
    source.append("#endif /* CHARMAP_H */")
    source.append("")
    return source


def main():
    charmap = gen_charmap()
    source = gen_source(charmap)

    #print("\n".join(source))

    output_dir_path = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src"))
    output_file_path = os.path.join(output_dir_path, "charmap.h")

    output_file = open(output_file_path, "wb")
    output_file.write("\n".join(source))
    output_file.close()

    print("Header \"%s\" has been generated." % output_file_path)


if __name__ == "__main__":
    main()

