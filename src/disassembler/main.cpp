
/* Should provide ability to flip rendering texture (since we don't know where 0,0 is
 * Color inversion (just for fun)
 * Load/Save States
 * Configurable keyboard map
 * Disassemler
 * Compiler
 * Ability to set color scheme
 */
#include <iostream>
#include <fstream>

inline void error(int buf_pos, int op) {
    std::cout << "Unknown opcode detected at " << buf_pos << "-th byte(" 
              << std::hex << buf_pos << "): " << op << std::dec << '\n';
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Disassemble CHIP8 binaries into the human readible assembly.\n";
        std::cout << "Usage: " << argv[0] << " [FILE]";
        return 0;
    }

    std::ifstream bin(argv[1]);
    std::ofstream out((std::string)argv[1] + ".out");
    out << std::hex;

    int loc = 0x200;
    int buf_pos = 1;
    while (!bin.eof()) {
        // Get the opcode that's 16 bits long byte-by-byte.
        int16_t op = bin.get();
        if (bin.eof()) {
            std::cout << "The input abrupts suddenly in the middle of describin"
                         "g the opcode.\n";
            break;
        }
        op = (op << 8) | bin.get();
        int a = (op & 0x0F00) >> 2 * 4;
        int b = (op & 0x00F0) >> 1 * 4;
        std::cout << std::hex << op << std::dec << ' ';

        // Here goes nothing.
        out << loc << "\t";
        switch ((op & 0xF000) >> 3 * 4) {
        case 0x0:
            switch (op & 0x00FF) {
            case 0xE0:
                out << "CLEAR" << '\n';
                break;
            case 0xEE:
                out << "RET" << '\n';
                break;
            default:
                out << op << '\n';
                error(buf_pos, op);
                std::cout << "Unknown opcode detected at " << buf_pos 
                          << "-th byte: " << std::hex << op << std::dec << '\n';
                //return 0;
            }
            break;
        case 0x1:
            out << "GOTO " << (op & 0x0FFF) << '\n';
            break;
        case 0x2:
            out << "CALL " << (op & 0x0FFF) << '\n';
            break;
        case 0x3:
            out << "EQ $" << a << std::dec << ' ' 
                << (op & 0x00FF) << std::hex << '\n';
            break;
        case 0x4:
            out << "NEQ $" << a << std::dec << ' ' 
                << (op & 0x00FF) << std::hex << '\n';
            break;
        case 0x5:
            if (op & 0x000F) {
                out << op << '\n';
                error(buf_pos, op);
                std::cout << "Unknown opcode detected at " << buf_pos 
                          << "-th byte: " << std::hex << op << std::dec << '\n';
                //return 0;
                break;
            }
            out << "REQ $" << a << " $" 
                << b << '\n';
            break;
        case 0x6:
            out << "SET $" << a << ' ' << std::dec
                << (op & 0x00FF) << std::hex << '\n';
            break;
        case 0x7:
            out << "ADD $" << a << ' ' << std::dec
                << (op & 0x00FF) << std::hex << '\n';
            break;
        case 0x8:
            switch (op & 0x000F) {
            case 0x0:
                out << "MOV $" << a << " $" << b << '\n';
                break;
            case 0x1:
                out << "OR $" << a << " $" << b << '\n';
                break;
            case 0x2:
                out << "AND $" << a << " $" << b << '\n';
                break;
            case 0x3:
                out << "XOR $" << a << " $" << b << '\n';
                break;
            case 0x4:
                out << "ADD $" << a << " $" << b << '\n';
                break;
            case 0x5:
                out << "SUB $" << a << " $" << b << '\n';
                break;
            case 0x6:
                out << "ROR $" << a << '\n';
                break;
            case 0x7:
                out << "BUS $" << a << " $" << b << '\n';
                break;
            case 0xE:
                out << "ROL $" << a << '\n';
                break;
            default:
                out << op << '\n';
                error(buf_pos, op);
                std::cout << "Unknown opcode detected at " << buf_pos 
                          << "-th byte: " << std::hex << op << std::dec << '\n';
                //return 0;
            }
            break;
        case 0x9:
            if (op & 0x000F) {
                out << op << '\n';
                error(buf_pos, op);
                std::cout << "Unknown opcode detected at " << buf_pos 
                          << "-th byte: " << std::hex << op << std::dec << '\n';
                //return 0;
                break;
            }
            out << "NREQ $" << a << " $" << b << '\n';
            break;
        case 0xA:
            out << "UP " << (op & 0x0FFF) << '\n';
            break;
        case 0xB:
            out << "GOTOO " << (op & 0x0FFF) << '\n';
            break;
        case 0xC:
            out << "RND $" << a << ' ' << std::dec
                << (op & 0x00FF) << std::hex << '\n';
            break;
        case 0xD:
            out << "DRAW $" << a << " $" << b << ' ' << std::dec
                << (op & 0x000F) << std::hex << '\n';
            break;
        case 0xE:
            switch (op & 0x00FF) {
            case 0x9E:
                out << "KP $" << a << '\n';
                break;
            case 0xA1:
                out << "KR $" << a << '\n';
                break;
            default:
                out << op << '\n';
                error(buf_pos, op);
                std::cout << "Unknown opcode detected at " << buf_pos 
                          << "-th byte: " << std::hex << op << std::dec << '\n';
                //return 0;
            }
            break;
        case 0xF:
            switch (op & 0x00FF) {
            case 0x07:
                out << "DTGET $" << a << '\n';
                break;
            case 0x0A:
                out << "WAITKEY $" << a << '\n';
                break;
            case 0x15:
                out << "DTSET $" << a << '\n';
                break;
            case 0x18:
                out << "BEEP $" << a << '\n';
                break;
            case 0x1E:
                out << "ADV $" << a << '\n';
                break;
            case 0x29:
                out << "GLYPH $" << a << '\n';
                break;
            case 0x33:
                out << "BCD $" << a << '\n';
                break;
            case 0x55:
                out << "DUMP $" << a << '\n';
                break;
            case 0x65:
                out << "UNDUMP $" << a << '\n';
                break;
            default:
                out << op << '\n';
                error(buf_pos, op);
                std::cout << "Unknown opcode detected at " << buf_pos 
                          << "-th byte: " << std::hex << op << std::dec << '\n';
                //return 0;
            }
            break;
        default:
            out << op << '\n';
            error(buf_pos, op);
            //return 0;
        }

        loc += 2;
        buf_pos += 2;
    }
    
    bin.close();
    out.close();
    return 0;
}
