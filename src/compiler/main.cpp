// (C) hikarideai
// Quick and dirty CHIP8 compiler
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <cctype>

enum ModeType {
    CODE_MODE, TEXT_MODE, RAW_MODE, SPRITE_MODE, SKIP_MODE, ModeTypeSize
};

enum LineType {
    INSTR, LABEL, RBITS
};

std::string raw_trans(const std::string &);
std::string sprite_trans(const std::string &);
std::string instr_trans(const std::string &, std::map<std::string, size_t> &);
std::vector<std::string> split(const std::string &, int (*)(int));
int hexchar2dec(char, bool);

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input file> <output name>" 
                  << std::endl;
        return 0;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Couldn't open " << argv[1] << " for reading.\n";
        return 0;
    }

    std::ofstream out(argv[2]);
    if (!out.is_open()) {
        std::cerr << "Couldn't open " << argv[2] << " for writting.\n";
        return 0;
    }

    //|Since several instructions make use of labels, we have to transform each
    // label to its corresponding address before transforming instructions
    // themselves. We will process the data that doesn't require labels first,
    // then labels, then instructions, then... that's it!
    //|RBITS corresponds to the complete chunks of the binary file. We get them
    // from the non-.code sections
    //|LABEL and INSTR are cleaned up versions of the assembler lines.
    //|We know sized of all LineTypes, and so we can easily generate jump
    // table.
    
    // A great way to transform non-code sections is to generate an array of
    // function pointers and call the corresponding one depending on the
    // current mode. The input is the assembler line (as is) and the output is
    // the binary string.
    
    std::vector<std::function<std::string(const std::string &)>> 
                                                    section_trans(ModeTypeSize);
    section_trans[TEXT_MODE] = [](const std::string & line) -> std::string {
        return line;
    };
    section_trans[SKIP_MODE] = [](const std::string & line) -> std::string {
        return "";
    };
    section_trans[RAW_MODE] = raw_trans;
    section_trans[SPRITE_MODE] = sprite_trans;
    
    std::vector<std::pair<LineType, std::string>> catl;
    ModeType mode = SKIP_MODE;
    std::string line;
    int line_id = 0;
    while (!file.eof()) {
        line_id++;
        std::getline(file, line);
        // Comment support
        if (line.empty() || line[0] == '#')
            continue;
        auto sharp_pos = line.find("#");
        line = line.substr(0, sharp_pos);
        // section .*** changes the way the lines of the input are being
        // percieved.
        // HACK: dot is only encountered in this statement, so I exploit it.
        auto dot_pos = line.find(".");
        if (dot_pos != std::string::npos) {
            auto mode_id = line.substr(dot_pos + 1);
            if (mode_id == "code") {
                mode = CODE_MODE;
            } else if (mode_id == "text") {
                mode = TEXT_MODE;
            } else if (mode_id == "raw") {
                mode = RAW_MODE;
            } else if (mode_id == "sprite") {
                mode = SPRITE_MODE;
            } else if (mode_id == "skip") {
                mode = SKIP_MODE;
            } else {
                std::cerr << argv[1] << ":" << line_id << ":0: Unknown section "
                          "(" << mode_id << ") encountered";
            }
            continue;
        }
        // Detect labels, since they can be anywhere in the code.
        // HACK: Only labels contain ':'
        auto colon_pos = line.find(":");
        if (colon_pos != std::string::npos) {
            catl.emplace_back(LABEL, line.substr(0, colon_pos));
            continue;
        }
        // Remember, instruction processing is postponed.
        if (mode == CODE_MODE) {
            catl.emplace_back(INSTR, line);
        } else {
            catl.emplace_back(RBITS, section_trans[mode](line));
        }
    }

    // At this point, we don't need the input file.
    file.close();

    // We'll traverse the pseude-rom and generate jump table
    std::map<std::string, size_t> jtab;
    const size_t START_ADDR = 0x200;
    size_t addr = START_ADDR;
    for (auto & i : catl) {
        if (i.first == INSTR)
            addr += 2;
        else if (i.first == LABEL)
            jtab[i.second] = addr;
        else if (i.first == RBITS)
            addr += i.second.size();
    }

    // Finally, generate the CHIP8 ROM.
    for (auto & i : catl) {
        if (i.first == INSTR)
            out << instr_trans(i.second, jtab);
        else if (i.first == LABEL)
            continue;
        else if (i.first == RBITS)
            out << i.second;
    }

    // ...And close it of course
    out.close();

    return 0;
}

int hexchar2dec(char d, bool test = true) {
    if ('0' <= d && d <= '9')
        return d - '0';
    if ('A' <= d && d <= 'F')
        return d - 'A' + 10;
    if ('a' <= d && d <= 'f')
        return d - 'a' + 10;

    if (test)
        std::cerr << "Bad hex -- \'" << d << "\'(" << (int)d << ").\n";
    return -1;
}

std::vector<std::string> split(const std::string & str, int (*isDel)(int)) {
    std::vector<std::string> out;
    std::string acc;
    bool word = false;
    for (int i = 0; i < str.size(); ++i) {
        word = !isDel(str[i]);
        if (word) {
            acc.push_back(str[i]);
        } else if (!acc.empty()) {
            out.push_back(acc);
            acc.clear();
        }
    }

    if (!acc.empty())
        out.push_back(acc);

    return out;
}

std::string raw_trans(const std::string & line) {
    std::string bin;
    for (int i = 0; i < line.size(); i++) {
        // Correctly skip 'wrong' characters.
        if (hexchar2dec(line[i], false) == -1)
            continue;

        unsigned char ac = hexchar2dec(line[i],false) << 4;
        while (i < line.size() && hexchar2dec(line[i], false) == -1)
            ++i;
        if (i < line.size()) {
            ac |= hexchar2dec(line[i], false);
        }
        bin.push_back(ac);
    }

    return bin;
}

std::string sprite_trans(const std::string & line) {
    std::string bin;
    unsigned char acc = 0;
    int pos = 7;
    for (int i = 0; i < line.size(); ++i) {
        if (line[i] == '*') {
            acc |= 1 << pos;
            pos--;
        } else if (line[i] == '_') {
            pos--;
        }

        if (pos == -1) {
            bin.push_back(acc);
            acc = 0;
            pos = 7;
        }
    }

    if (pos != 7)
        bin.push_back(acc);

    return bin;
}

std::string instr_trans(const std::string & line, 
                        std::map<std::string, size_t> & jtab) {
    auto cmd = split(line, isspace);

    for (auto & i : cmd)
        for (auto & c : i)
            c = tolower(c);
    
    std::string bin;

    // wow, I'm not kidding!
    // TODO: Check for segfaults
    int16_t op = 0;
    if (cmd[0] == "clear") {
        op = 0x00E0;
    } else if (cmd[0] == "ret") {
        op = 0x00EE;
    } else if (cmd[0] == "goto") {
        op = 0x1000 | jtab[cmd[1]];
    } else if (cmd[0] == "call") {
        op = 0x2000 | jtab[cmd[1]];
    } else if (cmd[0] == "eq") {
        if (cmd[2][0] == 'v') {
            int reg1 = hexchar2dec(cmd[1][1]);
            int reg2 = hexchar2dec(cmd[2][1]);
            op = 0x5000 | (reg1 << 2 * 4) | (reg2 << 4);
        } else {
            int reg = hexchar2dec(cmd[1][1]);
            int val = stoi(cmd[2], nullptr, 0);
            op = 0x3000 | (reg << 2 * 4) | val;
        }
    } else if (cmd[0] == "neq") {
        if (cmd[2][0] == 'v') {
            int reg1 = hexchar2dec(cmd[1][1]);
            int reg2 = hexchar2dec(cmd[2][1]);
            op = 0x9000 | (reg1 << 2 * 4) | (reg2 << 4);
        } else {
            int reg = hexchar2dec(cmd[1][1]);
            int val = stoi(cmd[2], nullptr, 0);
            op = 0x4000 | (reg << 2 * 4) | val;
        }
    } else if (cmd[0] == "set") {
        if (cmd[2][0] == 'v') {
            int reg1 = hexchar2dec(cmd[1][1]);
            int reg2 = hexchar2dec(cmd[2][1]);
            op = 0x8000 | (reg1 << 2 * 4) | (reg2 << 4);
        } else {
            int reg = hexchar2dec(cmd[1][1]);
            int val = stoi(cmd[2], nullptr, 0);
            op = 0x6000 | (reg << 2 * 4) | val;
        }
    } else if (cmd[0] == "add") {
        if (cmd[2][0] == 'v') {
            int reg1 = hexchar2dec(cmd[1][1]);
            int reg2 = hexchar2dec(cmd[2][1]);
            op = 0x8004 | (reg1 << 2 * 4) | (reg2 << 4);
        } else {
            int reg = hexchar2dec(cmd[1][1]);
            int val = stoi(cmd[2], nullptr, 0);
            op = 0x7000 | (reg << 2 * 4) | val;
        }
    } else if (cmd[0] == "or") {
        int reg1 = hexchar2dec(cmd[1][1]);
        int reg2 = hexchar2dec(cmd[2][1]);
        op = 0x8001 | (reg1 << 2 * 4) | (reg2 << 4);
    } else if (cmd[0] == "and") {
        int reg1 = hexchar2dec(cmd[1][1]);
        int reg2 = hexchar2dec(cmd[2][1]);
        op = 0x8002 | (reg1 << 2 * 4) | (reg2 << 4);
    } else if (cmd[0] == "xor") {
        int reg1 = hexchar2dec(cmd[1][1]);
        int reg2 = hexchar2dec(cmd[2][1]);
        op = 0x8003 | (reg1 << 2 * 4) | (reg2 << 4);
    } else if (cmd[0] == "sub") {
        int reg1 = hexchar2dec(cmd[1][1]);
        int reg2 = hexchar2dec(cmd[2][1]);
        op = 0x8005 | (reg1 << 2 * 4) | (reg2 << 4);
    } else if (cmd[0] == "shr") {
        int reg1 = hexchar2dec(cmd[1][1]);
        op = 0x8006 | (reg1 << 2 * 4);
    } else if (cmd[0] == "isub") {
        int reg1 = hexchar2dec(cmd[1][1]);
        int reg2 = hexchar2dec(cmd[2][1]);
        op = 0x8007 | (reg1 << 2 * 4) | (reg2 << 4);
    } else if (cmd[0] == "shl") {
        int reg1 = hexchar2dec(cmd[1][1]);
        op = 0x800E | (reg1 << 2 * 4);
    } else if (cmd[0] == "ld") {
        op = 0xA000 | jtab[cmd[1]];
    } else if (cmd[0] == "offto") {
        op = 0xB000 | jtab[cmd[1]];
    } else if (cmd[0] == "rand") {
        int reg = hexchar2dec(cmd[1][1]);
        int val = stoi(cmd[2], nullptr, 0);
        op = 0xC000 | (reg << 2 * 4) | val;
    } else if (cmd[0] == "draw") {
        int reg1 = hexchar2dec(cmd[1][1]);
        int reg2 = hexchar2dec(cmd[2][1]);
        int val = stoi(cmd[3]);
        op = 0xD000 | (reg1 << 2 * 4) | (reg2 << 4) | val;
    } else if (cmd[0] == "keypressed") {
        int reg = hexchar2dec(cmd[1][1]);
        op = 0xE09E | (reg << 2 * 4);
    } else if (cmd[0] == "keyreleased") {
        int reg = hexchar2dec(cmd[1][1]);
        op = 0xE0A1 | (reg << 2 * 4);
    } else if (cmd[0] == "getdt") {
        int reg = hexchar2dec(cmd[1][1]);
        op = 0xF007 | (reg << 2 * 4);
    } else if (cmd[0] == "waitkey") {
        int reg = hexchar2dec(cmd[1][1]);
        op = 0xF00A | (reg << 2 * 4);
    } else if (cmd[0] == "setdt") {
        int reg = hexchar2dec(cmd[1][1]);
        op = 0xF015 | (reg << 2 * 4);
    } else if (cmd[0] == "beep") {
        int reg = hexchar2dec(cmd[1][1]);
        op = 0xF018 | (reg << 2 * 4);
    } else if (cmd[0] == "adv") {
        int reg = hexchar2dec(cmd[1][1]);
        op = 0xF01E | (reg << 2 * 4);
    } else if (cmd[0] == "glyph") {
        int reg = hexchar2dec(cmd[1][1]);
        op = 0xF029 | (reg << 2 * 4);
    } else if (cmd[0] == "bcd") {
        int reg = hexchar2dec(cmd[1][1]);
        op = 0xF033 | (reg << 2 * 4);
    } else if (cmd[0] == "regdump") {
        int val = stoi(cmd[1], nullptr, 0);
        op = 0xF055 | (val << 2 * 4);
    } else if (cmd[0] == "regload") {
        int val = stoi(cmd[1], nullptr, 0);
        op = 0xF065 | (val << 2 * 4);
    }

    bin.push_back(op >> 8);
    bin.push_back(op & 0xFF);

    return bin;
}
