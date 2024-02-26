#include <iostream>
#include <fstream>
#include <string_view>

#include <zlib.h>

class DomainTrie {
    static constexpr auto length = 37;

protected:
    class TrieNode final {
    public:
        TrieNode* children[length] = {nullptr};
        bool terminated = false;

        ~TrieNode() {
            for (auto& child : children)
                if (child) {
                    delete child;
                }
        }
    };

private:
    TrieNode* root = new TrieNode();
    std::ofstream outfile;

    static constexpr unsigned char char_to_index_table[256] = {
            ['0'] = 0,  ['1'] = 1,  ['2'] = 2,  ['3'] = 3,
            ['4'] = 4,  ['5'] = 5,  ['6'] = 6,  ['7'] = 7,
            ['8'] = 8,  ['9'] = 9,  ['a'] = 10, ['b'] = 11,
            ['c'] = 12, ['d'] = 13, ['e'] = 14, ['f'] = 15,
            ['g'] = 16, ['h'] = 17, ['i'] = 18, ['j'] = 19,
            ['k'] = 20, ['l'] = 21, ['m'] = 22, ['n'] = 23,
            ['o'] = 24, ['p'] = 25, ['q'] = 26, ['r'] = 27,
            ['s'] = 28, ['t'] = 29, ['u'] = 30, ['v'] = 31,
            ['w'] = 32, ['x'] = 33, ['y'] = 34, ['z'] = 35,
            ['-'] = 36
    };

    static constexpr char index_to_char_table[37] = {
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
            'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
            'u', 'v', 'w', 'x', 'y', 'z', '-'
    };

public:
    ~DomainTrie() {
        delete root;
    }

    void push(const std::string_view domain) {
        auto node = root;
        for(unsigned char c : domain) {
            c = char_to_index_table[c];
            if (node->children[c] == nullptr)
                node->children[c] = new TrieNode();
            node = node->children[c];
        }

        // New or existing word pushed: increase the counter
        node->terminated = true;
    }

    void find_all(int level) {
        outfile.open("/Users/denis.miller/Desktop/local_domain_chacker/output_" + std::to_string(level) + ".txt");
        find_all(root, "", level);
        outfile.close();
    }

    void find_all(TrieNode* node, std::string sofar, int level) {
        if (level == 0) {
            if (!node->terminated && (sofar.back() != '-')) {
                outfile << sofar << ".com" << std::endl;
            }
            return;
        }
        for (unsigned char c = 0; c < length; ++c) {
            const auto chr = index_to_char_table[c];
            if (node->children[c] == nullptr) {
                if ((!node->terminated) && (chr != '-')) {
                    outfile << sofar << chr << ".com" << std::endl;
                }
            }
            else {
                if ((chr == '-') && (sofar.back() == '-')) {
                    // disallow double hyphen
                    continue;
                }
                find_all(node->children[c], sofar + chr, level - 1);
            }
        }
    }
};

int main() {
    static constexpr int maxDomainSize = 3;
    std::cout << "Reading..." << std::endl;

    gzFile file = gzopen("/Users/denis.miller/Desktop/local_domain_chacker/com.txt.gz", "r");
    if (file == nullptr) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    auto trie = DomainTrie();
    int line = 0;

    std::string_view buffer;
    char temp[1024];
    while (gzgets(file, temp, 1024) != nullptr) {
        if (line == 0) {
            line++;
            continue;
        }

        buffer = temp;
        std::size_t found = buffer.find(".com.\t");
        if (found != std::string_view::npos) {
            buffer.remove_suffix(buffer.size() - found);
        }

        #if 0
        std::cout << buffer << std::endl;
        #endif
        found = buffer.find(".");
        if ((found == std::string_view::npos) && (buffer.size() <= maxDomainSize)) {
            trie.push(buffer);
        }
        line++;
        #if 0
        if (line == 200)
            break;
        #endif
    }

    // Close the file
    gzclose(file);
    std::cout << "DONE" << std::endl;

    trie.find_all(maxDomainSize);

    return 0;
}
