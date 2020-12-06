// Trie implementation which uses nodes storing a single character and a flag indicating if they are a completed word
// Tries offer efficient textual lookups by prefix
// This implementation uses a fixed-size vector and assumes lowercase characters.
// For larger character sets, this should be updated to use an associative container to keep unused memory to a minimum

constexpr int ALPHA_SIZE = 26;

struct TrieNode
{
    TrieNode() = default;
    TrieNode(char c)
        : data(c)
    {
        createChildren();
    }

    char data = '\0';
    bool isWord = false;
    std::vector<TrieNode> children;

    void createChildren()
    {
        children.resize(ALPHA_SIZE);
    }

    bool hasChildren() const
    {
        // iterating through a small vector (26) is sufficiently fast, however
        // this should be updated to an associative container to support larger character
        // sets such as unicode
        return std::any_of(children.cbegin(), children.cend(), [] (const TrieNode &node)
            {
                return node.data != '\0';
            });
    }
};

class Trie
{
public:
    Trie()
    {
        m_root = std::make_unique<TrieNode>('\0');
    }

    auto& insert(const std::string &s)
    {
        assert(isLower(s));
        TrieNode *current = m_root.get();
        for (std::size_t i = 0; i < s.size(); ++i)
        {
            // index into child using smallest permitted character 'a' as offset index
            TrieNode &trieNode = current->children[s[i] - 'a'];
            if (trieNode.children.empty())
                trieNode.createChildren();

            trieNode.data = s[i];

            // set completed word flag if at end of input string
            const bool atEnd = i == s.size() - 1;
            if (atEnd)
                trieNode.isWord = true;

            current = &trieNode;
        }

        // allow chaining
        return *this;
    }

    bool isWord(const std::string &s)
    {
        assert(isLower(s));
        TrieNode *current = m_root.get();
        bool isWord = false;

        for (const char c : s)
        {
            if (current->children.empty())
                break;

            current = &current->children[c - 'a'];

            isWord = current->isWord;
        }

        return isWord;
    }

    std::vector<std::string> getWords()
    {
        std::vector<std::string> words;
        std::string substr;
        getWordsImpl(*m_root, substr, words);

        return words;
    }

    static bool isLower(const std::string &s)
    {
        return std::all_of(s.cbegin(), s.cend(), islower);
    }

private:
    std::unique_ptr<TrieNode> m_root;

    // return words via depth first traversal of tree
    void getWordsImpl(const TrieNode &node, std::string &substr, std::vector<std::string> &words)
    {
        for (const TrieNode &child : node.children)
        {
            if (child.data == '\0') // only look at set nodes
                continue;
            
            // build entire word by appending to substr with reach recursive call
            substr += child.data;
            if (child.isWord)
                words.push_back(substr); // found a completed word

            if (!child.hasChildren())
                substr.clear(); // leaf -- recursion stack done

            getWordsImpl(child, substr, words);
        }
    }
};

void testTrie()
{
    Trie t;
    assert(!t.isWord("many"));
    t.insert("many").insert("man").insert("quick").insert("quickly");

    assert(t.isWord("many"));
    assert(t.isWord("man"));
    assert(!t.isWord("ma"));
    assert(!t.isWord("m"));
    assert(t.isWord("quick"));
    assert(t.isWord("quickly"));
    assert(!t.isWord("mmismatch"));
    assert(!t.isWord("qmismatch"));
    assert(!t.isWord("z"));

    auto words = t.getWords();
    assert(words == std::vector<std::string>({"man", "many", "quick", "quickly"}));

    t.insert("zzz");
    words = t.getWords();
    assert(words == std::vector<std::string>({ "man", "many", "quick", "quickly", "zzz" }));

    Trie t2;
    assert(t2.getWords().empty());
}
