// ============================================================
// AI Prompt Engineering Assistant (Rule-Based Analyzer System)
// A beginner-friendly C++ OOP project for 2nd semester students
// ============================================================

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <cstdlib>
#include <cstdint>
using namespace std;

// ============================================================
// UTILITY FUNCTIONS
// Simple helper functions that make the code easier to read
// ============================================================

// Gets a valid integer between minV and maxV
int getValidInt(string msg, int minV, int maxV) {
    string in;
    int val;

    while (true) {
        cout << msg;
        getline(cin, in);

        if (in.empty()) {
            cout << "[!] Please enter a number between " << minV << " and " << maxV << ".\n";
            continue;
        }

        bool ok = true;
        for (size_t i = 0; i < in.length(); ++i) {
            if (i == 0 && in[i] == '-') continue;
            if (!isdigit(in[i])) {
                ok = false;
                break;
            }
        }

        if (!ok) {
            cout << "[!] Please enter a valid number between " << minV << " and " << maxV << ".\n";
            continue;
        }

        stringstream(in) >> val;

        if (val < minV || val > maxV) {
            cout << "[!] Number must be between " << minV << " and " << maxV << ".\n";
            continue;
        }

        return val;
    }
}

// Gets a non-empty string from user
string getValidString(string msg) {
    string res;

    while (true) {
        cout << msg;
        getline(cin, res);

        size_t start = res.find_first_not_of(' ');

        if (start == string::npos) {
            cout << "[!] Input cannot be empty.\n";
            continue;
        }

        size_t end = res.find_last_not_of(' ');
        return res.substr(start, end - start + 1);
    }
}

// Converts a string to lowercase for case-insensitive comparison
string toLower(string s) {
    for (size_t i = 0; i < s.length(); ++i) {
        s[i] = tolower(s[i]);
    }
    return s;
}

// Counts the number of words in a string
int countWords(string t) {
    int c = 0;
    stringstream ss(t);
    string w;

    while (ss >> w) {
        c++;
    }

    return c;
}

// Checks if any keyword exists in the text
bool hasKeyword(string text, vector<string> kw) {
    for (size_t i = 0; i < kw.size(); ++i) {
        if (text.find(kw[i]) != string::npos) {
            return true;
        }
    }
    return false;
}

// Returns a score label based on score value
string scoreLabel(int s) {
    if (s >= 5) return "EXCELLENT";
    if (s >= 4) return "GOOD";
    if (s >= 3) return "AVERAGE";
    if (s >= 2) return "WEAK";
    return "MISSING";
}

// Returns a rating label based on score value
string ratingLabel(float s) {
    if (s >= 4.5f) return "EXCELLENT";
    if (s >= 3.5f) return "GOOD";
    if (s >= 2.5f) return "AVERAGE";
    if (s >= 1.5f) return "WEAK";
    return "POOR";
}

// Converts word count to score (1-5)
int wordScore(int c) {
    if (c < 6) return 1;
    if (c <= 15) return 3;
    return 5;
}

// A simple SHA-256 implementation
string sha256(const string str) {
    unsigned int h0 = 0x6a09e667;
    unsigned int h1 = 0xbb67ae85;
    unsigned int h2 = 0x3c6ef372;
    unsigned int h3 = 0xa54ff53a;
    unsigned int h4 = 0x510e527f;
    unsigned int h5 = 0x9b05688c;
    unsigned int h6 = 0x1f83d9ab;
    unsigned int h7 = 0x5be0cd19;

    const unsigned int k[] = {
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
    };

    vector<unsigned char> payload(str.begin(), str.end());
    unsigned long long bitLen = payload.size() * 8;
    payload.push_back(0x80);
    while ((payload.size() + 8) % 64 != 0) {
        payload.push_back(0x00);
    }
    for (int i = 7; i >= 0; i--) {
        payload.push_back((bitLen >> (i * 8)) & 0xff);
    }

    for (size_t chunk = 0; chunk < payload.size(); chunk += 64) {
        unsigned int w[64];
        for (int i = 0; i < 16; i++) {
            w[i] = (payload[chunk + i * 4] << 24) |
                   (payload[chunk + i * 4 + 1] << 16) |
                   (payload[chunk + i * 4 + 2] << 8) |
                   (payload[chunk + i * 4 + 3]);
        }
        for (int i = 16; i < 64; i++) {
            unsigned int s0 = ((w[i - 15] >> 7) | (w[i - 15] << 25)) ^
                              ((w[i - 15] >> 18) | (w[i - 15] << 14)) ^
                              (w[i - 15] >> 3);
            unsigned int s1 = ((w[i - 2] >> 17) | (w[i - 2] << 15)) ^
                              ((w[i - 2] >> 19) | (w[i - 2] << 13)) ^
                              (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }

        unsigned int a = h0, b = h1, c = h2, d = h3, e = h4, f = h5, g = h6, h = h7;

        for (int i = 0; i < 64; i++) {
            unsigned int S1 = ((e >> 6) | (e << 26)) ^ ((e >> 11) | (e << 21)) ^ ((e >> 25) | (e << 7));
            unsigned int ch_val = (e & f) ^ (~e & g);
            unsigned int temp1 = h + S1 + ch_val + k[i] + w[i];
            unsigned int S0 = ((a >> 2) | (a << 30)) ^ ((a >> 13) | (a << 19)) ^ ((a >> 22) | (a << 10));
            unsigned int maj = (a & b) ^ (a & c) ^ (b & c);
            unsigned int temp2 = S0 + maj;

            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
        h5 += f;
        h6 += g;
        h7 += h;
    }

    stringstream ss;
    ss << setfill('0') << hex
       << setw(8) << h0
       << setw(8) << h1
       << setw(8) << h2
       << setw(8) << h3
       << setw(8) << h4
       << setw(8) << h5
       << setw(8) << h6
       << setw(8) << h7;
    return ss.str();
}

// ============================================================
// LOGIN SYSTEM
// Handles user registration and login
// ============================================================

class LoginSystem {
private:
    string filename;

public:
    LoginSystem() : filename("users.txt") {}

    void showMenu() {
        while (true) {
            cout << "\n===== LOGIN MENU =====\n";
            cout << "1. Login\n";
            cout << "2. Register\n";
            cout << "0. Exit\n";

            int ch = getValidInt("Choice: ", 0, 2);

            if (ch == 0) {
                cout << "Goodbye!\n";
                exit(0);
            }
            if (ch == 1 && login()) {
                return;
            }
            if (ch == 2) {
                registerUser();
            }
        }
    }

private:
    bool login() {
        string user = getValidString("Username: ");
        string pass = getValidString("Password: ");

        ifstream in(filename.c_str());

        if (!in) {
            ofstream out(filename.c_str());
            out << "admin|" << sha256("admin123") << "\n";
            out.close();
            in.open(filename.c_str());
        }

        string line;
        bool found = false;

        while (getline(in, line)) {
            stringstream ss(line);
            string u, p;

            getline(ss, u, '|');
            getline(ss, p);

            if (u == user && p == sha256(pass)) {
                found = true;
                break;
            }
        }

        in.close();

        if (found) {
            cout << "[+] Welcome, " << user << "!\n";
            return true;
        } else {
            cout << "[-] Invalid username or password.\n";
            return false;
        }
    }

    void registerUser() {
        string user = getValidString("New Username: ");
        string pass = getValidString("New Password: ");

        ifstream in(filename.c_str());
        if (in) {
            string line;
            while (getline(in, line)) {
                stringstream ss(line);
                string u;
                getline(ss, u, '|');

                if (u == user) {
                    in.close();
                    cout << "[-] Username already exists.\n";
                    return;
                }
            }
            in.close();
        }

        ofstream out(filename.c_str(), ios::app);
        out << user << "|" << sha256(pass) << "\n";
        out.close();

        cout << "[+] Registered! You can now login.\n";
    }
};

// ============================================================
// PROMPT CLASS
// Base class for all prompts
// Demonstrates: ENCAPSULATION
// ============================================================

class Prompt {
private:
    int id;
    string title;
    string content;
    string category;
    bool isFavorite;
    int editCount;
    float overallScore;
    vector<string> versionHistory;
    int scores[5];
    static int nextID;

public:
    Prompt(string t, string c, string cat)
        : id(nextID++), title(t), content(c), category(cat),
          isFavorite(false), editCount(0), overallScore(0.0f) {
        for (int i = 0; i < 5; i++) {
            scores[i] = 0;
        }
    }

    Prompt(int id, string t, string c, string cat, bool fav,
           int edits, float score, vector<string> hist)
        : id(id), title(t), content(c), category(cat),
          isFavorite(fav), editCount(edits), overallScore(score),
          versionHistory(hist) {
        for (int i = 0; i < 5; i++) {
            scores[i] = 0;
        }
    }

    virtual ~Prompt() {}

    int getId() const { return id; }
    string getTitle() const { return title; }
    string getContent() const { return content; }
    string getCategory() const { return category; }
    bool getIsFavorite() const { return isFavorite; }
    int getEditCount() const { return editCount; }
    float getOverallScore() const { return overallScore; }
    vector<string> getVersionHistory() const { return versionHistory; }

    int getScore(int i) const {
        if (i >= 0 && i < 5) {
            return scores[i];
        }
        return 0;
    }

    void setTitle(string t) {
        if (!t.empty()) {
            title = t;
        }
    }

    void setContent(string c) {
        if (!c.empty()) {
            content = c;
        }
    }

    void setIsFavorite(bool f) { isFavorite = f; }
    void setOverallScore(float s) {
        if (s >= 0.0f && s <= 5.0f) {
            overallScore = s;
        }
    }

    void setScore(int i, int v) {
        if (i >= 0 && i < 5) {
            scores[i] = v;
        }
    }

    void incrementEditCount() { editCount++; }

    void saveVersion() {
        versionHistory.push_back(content);
    }

    static void setNextID(int id) { nextID = (id < 1) ? 1 : id; }

    virtual void displayDetail() {
        cout << "[ID:" << id << "] " << left << setw(20)
             << (title.length() > 20 ? title.substr(0, 17) + "..." : title)
             << " | Cat: " << setw(10) << category
             << " | Score: " << fixed << setprecision(1) << overallScore << "/5"
             << " | Fav: " << (isFavorite ? "Yes" : "No") << "\n";
    }

    void showHistory() {
        if (versionHistory.empty()) {
            cout << "No history available.\n";
            return;
        }

        for (size_t i = 0; i < versionHistory.size(); ++i) {
            cout << "  Version " << (i + 1) << ": " << versionHistory[i] << "\n";
        }
    }
};

int Prompt::nextID = 1;

// ============================================================
// CODING PROMPT CLASS
// Derived from Prompt class
// Demonstrates: INHERITANCE and POLYMORPHISM
// ============================================================

class CodingPrompt : public Prompt {
private:
    string language;
    string expectedOutput;

public:
    CodingPrompt(string t, string c)
        : Prompt(t, c, "Coding"), language(""), expectedOutput("") {}

    CodingPrompt(int id, string t, string c, bool fav, int edits,
                 float score, vector<string> hist, string lang, string exp)
        : Prompt(id, t, c, "Coding", fav, edits, score, hist),
          language(lang), expectedOutput(exp) {}

    string getLanguage() const { return language; }
    string getExpectedOutput() const { return expectedOutput; }

    void setLanguage(string l) { language = l; }
    void setExpectedOutput(string e) { expectedOutput = e; }

    void displayDetail() override {
        Prompt::displayDetail();
        cout << "  -> Language: " << (language.empty() ? "N/A" : language)
             << " | Output: " << (expectedOutput.empty() ? "N/A" : expectedOutput) << "\n";
    }
};

// ============================================================
// ANALYZER CLASS
// Analyzes prompts and provides feedback
// Demonstrates: ABSTRACTION
// ============================================================

class Analyzer {
public:
    void analyze(Prompt* p) {
        string low = toLower(p->getContent());
        string cat = p->getCategory();

        p->setScore(0, wordScore(countWords(low)));

        if (cat == "Coding") {
            scoreCoding(p, low);
        } else if (cat == "Writing") {
            scoreWriting(p, low);
        } else if (cat == "Research") {
            scoreResearch(p, low);
        } else if (cat == "Image") {
            scoreImage(p, low);
        }

        float ov = 0;
        for (int i = 0; i < 5; i++) {
            ov += p->getScore(i);
        }
        ov /= 5.0f;
        p->setOverallScore(ov);

        printReport(p, cat, ov);
    }

    void scoreOnly(Prompt* p) {
        string low = toLower(p->getContent());
        string cat = p->getCategory();

        p->setScore(0, wordScore(countWords(low)));

        if (cat == "Coding") {
            scoreCoding(p, low);
        } else if (cat == "Writing") {
            scoreWriting(p, low);
        } else if (cat == "Research") {
            scoreResearch(p, low);
        } else if (cat == "Image") {
            scoreImage(p, low);
        }

        float ov = 0;
        for (int i = 0; i < 5; i++) {
            ov += p->getScore(i);
        }
        ov /= 5.0f;
        p->setOverallScore(ov);
    }

    void generateTips(Prompt* p) {
        if (p->getOverallScore() == 5.0f) {
            cout << "Tip: Your prompt is already excellent!\n";
            return;
        }

        cout << "\n--- IMPROVEMENT TIPS ---\n";
        int n = 1;
        string cat = p->getCategory();

        if (p->getScore(0) < 5) {
            cout << n++ << ". Add more detail to help the AI understand.\n";
        }

        if (cat == "Coding") {
            codingTips(p, n);
        } else if (cat == "Writing") {
            writingTips(p, n);
        } else if (cat == "Research") {
            researchTips(p, n);
        } else if (cat == "Image") {
            imageTips(p, n);
        }

        cout << "------------------------\n";
    }

private:
    void scoreCoding(Prompt* p, string low) {
        p->setScore(1, hasKeyword(low, {"python","c++","java","javascript","c#"}) ? 5 : 1);
        p->setScore(2, hasKeyword(low, {"output","result","return","print","display"}) ? 5 : 1);
        p->setScore(3, hasKeyword(low, {"sort","search","loop","recursion","stack","queue","tree"}) ? 5 : 1);
        p->setScore(4, hasKeyword(low, {"empty","null","invalid","error","handle","boundary"}) ? 5 : 1);

        CodingPrompt* cp = dynamic_cast<CodingPrompt*>(p);
        if (cp) {
            if (p->getScore(1) == 5) cp->setLanguage("Detected");
            else cp->setLanguage("");
            
            if (p->getScore(2) == 5) cp->setExpectedOutput("Detected");
            else cp->setExpectedOutput("");
        }
    }

    void scoreWriting(Prompt* p, string low) {
        p->setScore(1, hasKeyword(low, {"formal","casual","professional","friendly","persuasive"}) ? 5 : 1);
        p->setScore(2, hasKeyword(low, {"students","adults","beginners","experts","readers"}) ? 5 : 1);
        p->setScore(3, hasKeyword(low, {"words","paragraphs","short","long","500","1000","brief"}) ? 5 : 1);
        p->setScore(4, hasKeyword(low, {"essay","article","blog","report","letter","story","email"}) ? 5 : 1);
    }

    void scoreResearch(Prompt* p, string low) {
        p->setScore(1, hasKeyword(low, {"focus","limit","specifically","about","regarding"}) ? 5 : 1);
        p->setScore(2, hasKeyword(low, {"brief","detailed","in-depth","academic","overview"}) ? 5 : 1);
        p->setScore(3, hasKeyword(low, {"compare","impact","cause","history","pros","cons","effect"}) ? 5 : 1);
        p->setScore(4, hasKeyword(low, {"sources","references","cite","journal","books","articles"}) ? 5 : 1);
    }

    void scoreImage(Prompt* p, string low) {
        p->setScore(1, hasKeyword(low, {"realistic","cartoon","anime","painting","sketch","3d","digital"}) ? 5 : 1);
        p->setScore(2, hasKeyword(low, {"dark","bright","moody","happy","peaceful","dramatic","warm"}) ? 5 : 1);
        p->setScore(3, hasKeyword(low, {"red","blue","green","black","white","golden","colorful","vibrant"}) ? 5 : 1);
        p->setScore(4, hasKeyword(low, {"lighting","shadow","sunlight","close-up","portrait","wide"}) ? 5 : 1);
    }

    void printReport(Prompt* p, string cat, float ov) {
        string missing[4][5] = {
            {"Needs more words.",      "Language not specified.",  "Output not described.",  "Algorithm missing.",     "Edge cases not handled."},
            {"Needs more words.",      "Tone not defined.",        "Audience missing.",      "Length not specified.",  "Format missing."},
            {"Needs more words.",      "Scope missing.",           "Depth level missing.",   "Angle missing.",         "Sources not requested."},
            {"Needs more words.",      "Art style missing.",       "Mood missing.",          "Color scheme missing.",  "Lighting missing."}
        };

        int ci = 0;
        if (cat == "Coding") {
            ci = 0;
        } else if (cat == "Writing") {
            ci = 1;
        } else if (cat == "Research") {
            ci = 2;
        } else if (cat == "Image") {
            ci = 3;
        }

        cout << "\n===== ANALYSIS REPORT =====\n";
        cout << "ID       : " << p->getId() << "\n";
        cout << "Title    : " << p->getTitle() << "\n";
        cout << "Category : " << cat << "\n";
        cout << "---------------------------\n";
        cout << "Overall  : " << fixed << setprecision(1) << ov << " / 5.0\n";
        cout << "Rating   : " << ratingLabel(ov) << "\n";
        cout << "---------------------------\n";
        cout << "Missing:\n";

        bool any = false;
        for (int i = 0; i < 5; i++) {
            if (p->getScore(i) == 1) {
                cout << "  - " << missing[ci][i] << "\n";
                any = true;
            }
        }

        if (!any) {
            cout << "  - Nothing! Excellent prompt.\n";
        }

        cout << "===========================\n";
    }

    void codingTips(Prompt* p, int& n) {
        if (p->getScore(1) == 1) cout << n++ << ". Name the language (e.g., 'C++17 function').\n";
        if (p->getScore(2) == 1) cout << n++ << ". Describe expected output (e.g., 'return JSON').\n";
        if (p->getScore(3) == 1) cout << n++ << ". Mention an algorithm (e.g., 'use Hash Map').\n";
        if (p->getScore(4) == 1) cout << n++ << ". Ask to handle errors and edge cases.\n";
    }

    void writingTips(Prompt* p, int& n) {
        if (p->getScore(1) == 1) cout << n++ << ". Define the tone (e.g., 'professional').\n";
        if (p->getScore(2) == 1) cout << n++ << ". Specify the audience (e.g., 'beginners').\n";
        if (p->getScore(3) == 1) cout << n++ << ". Set a length (e.g., '3 paragraphs').\n";
        if (p->getScore(4) == 1) cout << n++ << ". State the format (e.g., 'blog post').\n";
    }

    void researchTips(Prompt* p, int& n) {
        if (p->getScore(1) == 1) cout << n++ << ". Narrow the scope (e.g., 'focusing on Asia').\n";
        if (p->getScore(2) == 1) cout << n++ << ". Set depth (e.g., 'detailed analysis').\n";
        if (p->getScore(3) == 1) cout << n++ << ". Define an angle (e.g., 'ethical view').\n";
        if (p->getScore(4) == 1) cout << n++ << ". Request citations from journals.\n";
    }

    void imageTips(Prompt* p, int& n) {
        if (p->getScore(1) == 1) cout << n++ << ". Specify technique (e.g., 'realistic render').\n";
        if (p->getScore(2) == 1) cout << n++ << ". Set mood (e.g., 'ethereal mist').\n";
        if (p->getScore(3) == 1) cout << n++ << ". Suggest colors (e.g., 'blue tones').\n";
        if (p->getScore(4) == 1) cout << n++ << ". Direct the shot (e.g., 'golden hour').\n";
    }
};

// ============================================================
// FILE MANAGER CLASS
// Handles saving and loading prompts from files
// Demonstrates: ABSTRACTION
// ============================================================

class FileManager {
public:
    static void saveAll(const vector<Prompt*>& prompts, const string& filename) {
        ofstream out(filename.c_str());
        if (!out) {
            return;
        }

        for (size_t i = 0; i < prompts.size(); ++i) {
            Prompt* p = prompts[i];

            out << p->getId() << "|"
                << p->getCategory() << "|"
                << p->getTitle() << "|"
                << p->getContent() << "|"
                << p->getIsFavorite() << "|"
                << p->getEditCount() << "|"
                << p->getOverallScore() << "|";

            vector<string> hist = p->getVersionHistory();
            for (size_t j = 0; j < hist.size(); ++j) {
                out << hist[j];
                if (j < hist.size() - 1) {
                    out << ";";
                }
            }

            if (p->getCategory() == "Coding") {
                CodingPrompt* cp = dynamic_cast<CodingPrompt*>(p);
                if (cp) {
                    out << "|" << cp->getLanguage() << "|" << cp->getExpectedOutput();
                }
            }

            out << "\n";
        }

        out.close();
    }

    static vector<Prompt*> loadAll(const string& filename) {
        vector<Prompt*> prompts;
        ifstream in(filename.c_str());

        if (!in) {
            // "Starting fresh" - silent for API wrapper
            return prompts;
        }

        string line;
        int maxID = 0;

        while (getline(in, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string idS, cat, title, content, favS, editS, scoreS, histS;

            getline(ss, idS, '|');
            getline(ss, cat, '|');
            getline(ss, title, '|');
            getline(ss, content, '|');
            getline(ss, favS, '|');
            getline(ss, editS, '|');
            getline(ss, scoreS, '|');
            getline(ss, histS, '|');

            int id = 0;
            stringstream(idS) >> id;

            // If ID is 0 (corrupted/old data), assign a new valid ID
            if (id == 0) {
                id = maxID + 1;
            }

            if (id > maxID) {
                maxID = id;
            }

            bool fav = (favS == "1" || favS == "true");
            int edits = 0;
            stringstream(editS) >> edits;
            float score = 0;
            stringstream(scoreS) >> score;

            vector<string> hist;
            if (!histS.empty()) {
                stringstream hss(histS);
                string h;
                while (getline(hss, h, ';')) {
                    hist.push_back(h);
                }
            }

            if (cat == "Coding") {
                string lang, exp;
                getline(ss, lang, '|');
                getline(ss, exp, '|');
                prompts.push_back(
                    new CodingPrompt(id, title, content, fav, edits, score, hist, lang, exp));
            } else {
                prompts.push_back(
                    new Prompt(id, title, content, cat, fav, edits, score, hist));
            }
        }

        in.close();
        Prompt::setNextID(maxID + 1);

        return prompts;
    }
};

// ============================================================
// STATISTICS CLASS
// Provides statistical information about prompts
// Demonstrates: ABSTRACTION
// ============================================================

class Statistics {
public:
    static void showDashboard(const vector<Prompt*>& prompts) {
        if (prompts.empty()) {
            cout << "No prompts available.\n";
            return;
        }

        float totalScore = 0;
        int counts[4] = {0, 0, 0, 0};
        Prompt* best = nullptr;

        for (size_t i = 0; i < prompts.size(); ++i) {
            Prompt* p = prompts[i];
            totalScore += p->getOverallScore();

            string cat = p->getCategory();
            if (cat == "Coding") {
                counts[0]++;
            } else if (cat == "Writing") {
                counts[1]++;
            } else if (cat == "Research") {
                counts[2]++;
            } else if (cat == "Image") {
                counts[3]++;
            }

            if (!best || p->getOverallScore() > best->getOverallScore()) {
                best = p;
            }
        }

        cout << "\n===== STATISTICS DASHBOARD =====\n";
        cout << "Total Prompts : " << prompts.size() << "\n";
        cout << "Average Score : " << fixed << setprecision(1)
             << (totalScore / prompts.size()) << " / 5.0\n";
        cout << "Coding   : " << counts[0] << "\n";
        cout << "Writing  : " << counts[1] << "\n";
        cout << "Research : " << counts[2] << "\n";
        cout << "Image    : " << counts[3] << "\n";

        if (best) {
            cout << "Best     : [ID:" << best->getId() << "] " << best->getTitle()
                 << " (" << best->getOverallScore() << "/5.0)\n";
        }

        cout << "================================\n";
    }

    static void showEffectiveness(const vector<Prompt*>& prompts) {
        cout << "\n===== EFFECTIVENESS REPORT =====\n";

        bool found = false;
        for (size_t i = 0; i < prompts.size(); ++i) {
            float s = prompts[i]->getOverallScore();
            if (s > 0.0f && s < 2.5f) {
                cout << "[ID:" << prompts[i]->getId() << "] "
                     << prompts[i]->getTitle() << " - Score: "
                     << fixed << setprecision(1) << s << " => Needs Improvement\n";
                found = true;
            }
        }

        if (!found) {
            cout << "No weak prompts found. Great job!\n";
        }

        cout << "================================\n";
    }
};

// ============================================================
// PROMPT MANAGER CLASS
// Manages all prompt operations (create, edit, search, etc.)
// ============================================================

class PromptManager {
private:
    vector<Prompt*> prompts;
    Analyzer analyzer;

    Prompt* findById(int id) {
        for (size_t i = 0; i < prompts.size(); ++i) {
            if (prompts[i]->getId() == id) {
                return prompts[i];
            }
        }
        return nullptr;
    }

public:
    PromptManager() {
        prompts = FileManager::loadAll("prompts.txt");
    }

    ~PromptManager() {
        for (size_t i = 0; i < prompts.size(); ++i) {
            delete prompts[i];
        }
    }

    void save() {
        FileManager::saveAll(prompts, "prompts.txt");
    }

    void addPrompt() {
        cout << "\n--- Create New Prompt ---\n";

        string title = getValidString("Enter Title: ");
        string content = getValidString("Enter Content: ");

        cout << "Select Category:\n";
        cout << "1. Coding\n";
        cout << "2. Writing\n";
        cout << "3. Research\n";
        cout << "4. Image\n";

        int ch = getValidInt("Your choice: ", 1, 4);

        Prompt* p = nullptr;

        if (ch == 1) {
            p = new CodingPrompt(title, content);
        } else if (ch == 2) {
            p = new Prompt(title, content, "Writing");
        } else if (ch == 3) {
            p = new Prompt(title, content, "Research");
        } else if (ch == 4) {
            p = new Prompt(title, content, "Image");
        }

        prompts.push_back(p);
        cout << "Prompt created with ID: " << p->getId() << "\n";
        cout << "\nAuto-analyzing...\n";

        analyzer.analyze(p);
        analyzer.generateTips(p);
    }

    void displayAll() {
        cout << "\n--- MASTER DASHBOARD ---\n";

        if (prompts.empty()) {
            cout << "No prompts found.\n";
            return;
        }

        Statistics::showDashboard(prompts);
        Statistics::showEffectiveness(prompts);

        cout << "\n--- Favorite Prompts ---\n";
        bool found = false;
        for (size_t i = 0; i < prompts.size(); ++i) {
            if (prompts[i]->getIsFavorite()) {
                prompts[i]->displayDetail();
                found = true;
            }
        }
        if (!found) {
            cout << "No favorites.\n";
        }

        cout << "\n--- All Prompts ---\n";
        for (size_t i = 0; i < prompts.size(); ++i) {
            prompts[i]->displayDetail();
        }

        int id = getValidInt("\nEnter ID to Analyze (0 to return): ", 0, 99999);
        if (id != 0) {
            Prompt* p = findById(id);
            if (p) {
                analyzer.analyze(p);
                analyzer.generateTips(p);
            } else {
                cout << "[!] Not found.\n";
            }
        }
    }

    void editPrompt() {
        if (prompts.empty()) {
            cout << "No prompts.\n";
            return;
        }

        Prompt* p = findById(getValidInt("Enter Prompt ID: ", 1, 99999));
        if (p) {
            cout << "Current: " << p->getContent() << "\n";
            p->saveVersion();
            p->incrementEditCount();
            p->setContent(getValidString("New content: "));
            p->setOverallScore(0.0f);
            cout << "Updated. Use Dashboard to re-analyze.\n";
        } else {
            cout << "[!] Not found.\n";
        }
    }

    void searchPrompt() {
        string kw = toLower(getValidString("Enter keyword: "));
        bool found = false;

        for (size_t i = 0; i < prompts.size(); ++i) {
            string t = toLower(prompts[i]->getTitle());
            string c = toLower(prompts[i]->getContent());

            if (t.find(kw) != string::npos || c.find(kw) != string::npos) {
                prompts[i]->displayDetail();
                found = true;
            }
        }

        if (!found) {
            cout << "No matches.\n";
        }
    }

    void toggleFavorite() {
        if (prompts.empty()) {
            cout << "No prompts.\n";
            return;
        }

        Prompt* p = findById(getValidInt("Enter Prompt ID: ", 1, 99999));
        if (p) {
            p->setIsFavorite(!p->getIsFavorite());
            cout << "Favorite updated.\n";
        } else {
            cout << "[!] Not found.\n";
        }
    }

    void viewHistory() {
        if (prompts.empty()) {
            cout << "No prompts.\n";
            return;
        }

        Prompt* p = findById(getValidInt("Enter Prompt ID: ", 1, 99999));
        if (p) {
            cout << "\n--- History for ID " << p->getId() << " ---\n";
            p->showHistory();
        } else {
            cout << "[!] Not found.\n";
        }
    }

    void compareTwo() {
        if (prompts.size() < 2) {
            cout << "Need at least 2 prompts.\n";
            return;
        }

        Prompt* p1 = findById(getValidInt("Enter first ID: ", 1, 99999));
        if (!p1) {
            cout << "[!] Not found.\n";
            return;
        }

        Prompt* p2 = findById(getValidInt("Enter second ID: ", 1, 99999));
        if (!p2) {
            cout << "[!] Not found.\n";
            return;
        }

        cout << "\n=== COMPARISON ===\n";
        cout << "PROMPT 1:\n";
        p1->displayDetail();
        cout << "Content: " << p1->getContent() << "\n\n";

        cout << "PROMPT 2:\n";
        p2->displayDetail();
        cout << "Content: " << p2->getContent() << "\n";
    }

    void deletePrompt() {
        if (prompts.empty()) {
            cout << "No prompts.\n";
            return;
        }

        int id = getValidInt("Enter Prompt ID: ", 1, 99999);

        for (size_t i = 0; i < prompts.size(); ++i) {
            if (prompts[i]->getId() == id) {
                delete prompts[i];
                prompts.erase(prompts.begin() + i);
                cout << "Deleted.\n";
                return;
            }
        }

        cout << "[!] Not found.\n";
    }
};

// ============================================================
// JSON API BRIDGING FUNCTIONS
// These map C++ OOP structures to standard JSON formats
// ============================================================

string escapeJSON(const string& s) {
    string out;
    for (size_t i = 0; i < s.length(); ++i) {
        char c = s[i];
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else if ((unsigned char)c < 32) {
            // ignore
        } else {
            out += c;
        }
    }
    return out;
}

string promptToJSON(Prompt* p) {
    stringstream ss;
    ss << "{";
    ss << "\"id\":" << p->getId() << ",";
    ss << "\"title\":\"" << escapeJSON(p->getTitle()) << "\",";
    ss << "\"content\":\"" << escapeJSON(p->getContent()) << "\",";
    ss << "\"category\":\"" << escapeJSON(p->getCategory()) << "\",";
    ss << "\"isFavorite\":" << (p->getIsFavorite() ? "true" : "false") << ",";
    ss << "\"editCount\":" << p->getEditCount() << ",";
    ss << "\"overallScore\":" << fixed << setprecision(2) << p->getOverallScore() << ",";
    
    // scores
    ss << "\"scores\":[";
    for (int i = 0; i < 5; ++i) {
        ss << p->getScore(i);
        if (i < 4) ss << ",";
    }
    ss << "],";

    // versionHistory
    ss << "\"versionHistory\":[";
    vector<string> hist = p->getVersionHistory();
    for (size_t i = 0; i < hist.size(); ++i) {
        ss << "\"" << escapeJSON(hist[i]) << "\"";
        if (i < hist.size() - 1) ss << ",";
    }
    ss << "]";

    // coding prompt details
    if (p->getCategory() == "Coding") {
        CodingPrompt* cp = dynamic_cast<CodingPrompt*>(p);
        if (cp) {
            ss << ",\"language\":\"" << escapeJSON(cp->getLanguage()) << "\",";
            ss << "\"expectedOutput\":\"" << escapeJSON(cp->getExpectedOutput()) << "\"";
        }
    }
    
    ss << "}";
    return ss.str();
}

vector<string> getTipsList(Prompt* p) {
    vector<string> tips;
    if (p->getOverallScore() == 5.0f) {
        tips.push_back("Your prompt is already excellent!");
        return tips;
    }

    string cat = p->getCategory();
    if (p->getScore(0) < 5) {
        tips.push_back("Add more detail to help the AI understand.");
    }

    if (cat == "Coding") {
        if (p->getScore(1) == 1) tips.push_back("Name the language (e.g., 'C++17 function').");
        if (p->getScore(2) == 1) tips.push_back("Describe expected output (e.g., 'return JSON').");
        if (p->getScore(3) == 1) tips.push_back("Mention an algorithm (e.g., 'use Hash Map').");
        if (p->getScore(4) == 1) tips.push_back("Ask to handle errors and edge cases.");
    } else if (cat == "Writing") {
        if (p->getScore(1) == 1) tips.push_back("Define the tone (e.g., 'professional').");
        if (p->getScore(2) == 1) tips.push_back("Specify the audience (e.g., 'beginners').");
        if (p->getScore(3) == 1) tips.push_back("Set a length (e.g., '3 paragraphs').");
        if (p->getScore(4) == 1) tips.push_back("State the format (e.g., 'blog post').");
    } else if (cat == "Research") {
        if (p->getScore(1) == 1) tips.push_back("Narrow the scope (e.g., 'focusing on Asia').");
        if (p->getScore(2) == 1) tips.push_back("Set depth (e.g., 'detailed analysis').");
        if (p->getScore(3) == 1) tips.push_back("Define an angle (e.g., 'ethical view').");
        if (p->getScore(4) == 1) tips.push_back("Request citations from journals.");
    } else if (cat == "Image") {
        if (p->getScore(1) == 1) tips.push_back("Specify technique (e.g., 'realistic render').");
        if (p->getScore(2) == 1) tips.push_back("Set mood (e.g., 'ethereal mist').");
        if (p->getScore(3) == 1) tips.push_back("Suggest colors (e.g., 'blue tones').");
        if (p->getScore(4) == 1) tips.push_back("Direct the shot (e.g., 'golden hour').");
    }
    return tips;
}

vector<string> getMissingList(Prompt* p) {
    vector<string> missingList;
    string missing[4][5] = {
        {"Needs more words.",      "Language not specified.",  "Output not described.",  "Algorithm missing.",     "Edge cases not handled."},
        {"Needs more words.",      "Tone not defined.",        "Audience missing.",      "Length not specified.",  "Format missing."},
        {"Needs more words.",      "Scope missing.",           "Depth level missing.",   "Angle missing.",         "Sources not requested."},
        {"Needs more words.",      "Art style missing.",       "Mood missing.",          "Color scheme missing.",  "Lighting missing."}
    };

    string cat = p->getCategory();
    int ci = 0;
    if (cat == "Coding") ci = 0;
    else if (cat == "Writing") ci = 1;
    else if (cat == "Research") ci = 2;
    else if (cat == "Image") ci = 3;

    for (int i = 0; i < 5; i++) {
        if (p->getScore(i) == 1) {
            missingList.push_back(missing[ci][i]);
        }
    }
    return missingList;
}

string analysisToJSON(Prompt* p) {
    Analyzer analyzer;
    analyzer.scoreOnly(p);

    stringstream ss;
    ss << "{";
    ss << "\"prompt\":" << promptToJSON(p) << ",";
    ss << "\"rating\":\"" << ratingLabel(p->getOverallScore()) << "\",";
    
    // tips
    ss << "\"tips\":[";
    vector<string> tips = getTipsList(p);
    for (size_t i = 0; i < tips.size(); ++i) {
        ss << "\"" << escapeJSON(tips[i]) << "\"";
        if (i < tips.size() - 1) ss << ",";
    }
    ss << "],";

    // missing
    ss << "\"missing\":[";
    vector<string> missing = getMissingList(p);
    for (size_t i = 0; i < missing.size(); ++i) {
        ss << "\"" << escapeJSON(missing[i]) << "\"";
        if (i < missing.size() - 1) ss << ",";
    }
    ss << "]";
    ss << "}";
    return ss.str();
}

// API command executions
void runApiLogin(string user, string pass) {
    ifstream in("users.txt");
    if (!in) {
        ofstream out("users.txt");
        out << "admin|admin123\n";
        out.close();
        in.open("users.txt");
    }

    string line;
    bool found = false;
    while (getline(in, line)) {
        stringstream ss(line);
        string u, p;
        getline(ss, u, '|');
        getline(ss, p);
        if (u == user && p == pass) {
            found = true;
            break;
        }
    }
    in.close();

    if (found) {
        cout << "{\"success\":true,\"username\":\"" << escapeJSON(user) << "\"}" << endl;
    } else {
        cout << "{\"success\":false,\"error\":\"Invalid username or password.\"}" << endl;
    }
}

void runApiRegister(string user, string pass) {
    if (user.empty() || pass.empty() || user.find('|') != string::npos || pass.find('|') != string::npos) {
        cout << "{\"success\":false,\"error\":\"Invalid characters in username or password.\"}" << endl;
        return;
    }

    ifstream in("users.txt");
    if (in) {
        string line;
        while (getline(in, line)) {
            stringstream ss(line);
            string u;
            getline(ss, u, '|');
            if (u == user) {
                in.close();
                cout << "{\"success\":false,\"error\":\"Username already exists.\"}" << endl;
                return;
            }
        }
        in.close();
    }

    ofstream out("users.txt", ios::app);
    out << user << "|" << pass << "\n";
    out.close();

    cout << "{\"success\":true,\"username\":\"" << escapeJSON(user) << "\"}" << endl;
}

void runApiAnalyzeStdin() {
    string cat, title, content, line;
    if (getline(cin, cat) && getline(cin, title)) {
        while (getline(cin, line)) {
            content += line + "\n";
        }
        if (!content.empty() && content.back() == '\n') {
            content.pop_back();
        }
        
        Prompt* p = nullptr;
        if (cat == "Coding") {
            p = new CodingPrompt(title, content);
        } else {
            p = new Prompt(title, content, cat);
        }

        cout << analysisToJSON(p) << endl;
        delete p;
    } else {
        cout << "{\"error\":\"Failed to read input parameters from stdin.\"}" << endl;
    }
}

void runApiList() {
    vector<Prompt*> prompts = FileManager::loadAll("prompts.txt");
    cout << "[";
    for (size_t i = 0; i < prompts.size(); ++i) {
        cout << promptToJSON(prompts[i]);
        if (i < prompts.size() - 1) cout << ",";
        delete prompts[i];
    }
    cout << "]" << endl;
}

void runApiSaveStdin() {
    string idS, cat, title, favS, content, line;
    if (getline(cin, idS) && getline(cin, cat) && getline(cin, title) && getline(cin, favS)) {
        while (getline(cin, line)) {
            content += line + "\n";
        }
        if (!content.empty() && content.back() == '\n') {
            content.pop_back();
        }

        int id = atoi(idS.c_str());
        bool isFavorite = (favS == "1" || favS == "true");

        vector<Prompt*> prompts = FileManager::loadAll("prompts.txt");
        Prompt* found = nullptr;
        for (size_t i = 0; i < prompts.size(); ++i) {
            if (prompts[i]->getId() == id) {
                found = prompts[i];
                break;
            }
        }

        Analyzer analyzer;
        if (found) {
            found->saveVersion();
            found->incrementEditCount();
            found->setTitle(title);
            found->setContent(content);
            found->setIsFavorite(isFavorite);
            analyzer.scoreOnly(found);
        } else {
            if (cat == "Coding") {
                found = new CodingPrompt(title, content);
            } else {
                found = new Prompt(title, content, cat);
            }
            found->setIsFavorite(isFavorite);
            analyzer.scoreOnly(found);
            prompts.push_back(found);
        }

        FileManager::saveAll(prompts, "prompts.txt");
        cout << promptToJSON(found) << endl;

        for (size_t i = 0; i < prompts.size(); ++i) {
            delete prompts[i];
        }
    } else {
        cout << "{\"error\":\"Failed to read parameters from stdin.\"}" << endl;
    }
}

void runApiDelete(int id) {
    vector<Prompt*> prompts = FileManager::loadAll("prompts.txt");
    bool deleted = false;
    for (size_t i = 0; i < prompts.size(); ++i) {
        if (prompts[i]->getId() == id) {
            delete prompts[i];
            prompts.erase(prompts.begin() + i);
            deleted = true;
            break;
        }
    }

    if (deleted) {
        FileManager::saveAll(prompts, "prompts.txt");
        cout << "{\"success\":true}" << endl;
    } else {
        cout << "{\"success\":false,\"error\":\"Prompt not found.\"}" << endl;
    }

    for (size_t i = 0; i < prompts.size(); ++i) {
        delete prompts[i];
    }
}

void runApiStats() {
    vector<Prompt*> prompts = FileManager::loadAll("prompts.txt");
    if (prompts.empty()) {
        cout << "{\"total\":0,\"avgScore\":0.0,\"counts\":[0,0,0,0],\"best\":null}" << endl;
        return;
    }

    float totalScore = 0;
    int counts[4] = {0, 0, 0, 0}; // Coding, Writing, Research, Image
    Prompt* best = nullptr;

    for (size_t i = 0; i < prompts.size(); ++i) {
        Prompt* p = prompts[i];
        totalScore += p->getOverallScore();

        string cat = p->getCategory();
        if (cat == "Coding") counts[0]++;
        else if (cat == "Writing") counts[1]++;
        else if (cat == "Research") counts[2]++;
        else if (cat == "Image") counts[3]++;

        if (!best || p->getOverallScore() > best->getOverallScore()) {
            best = p;
        }
    }

    stringstream ss;
    ss << "{";
    ss << "\"total\":" << prompts.size() << ",";
    ss << "\"avgScore\":" << fixed << setprecision(2) << (totalScore / prompts.size()) << ",";
    ss << "\"counts\":[" << counts[0] << "," << counts[1] << "," << counts[2] << "," << counts[3] << "],";
    if (best) {
        ss << "\"best\":" << promptToJSON(best);
    } else {
        ss << "\"best\":null";
    }
    ss << "}";

    cout << ss.str() << endl;

    for (size_t i = 0; i < prompts.size(); ++i) {
        delete prompts[i];
    }
}

// ============================================================
// MAIN FUNCTION
// Entry point of the program
// Supports standard terminal menu and JSON API wrappers
// ============================================================

int main(int argc, char* argv[]) {
    // API argument checking
    if (argc > 1) {
        string cmd = argv[1];
        if (cmd == "--api-login" && argc >= 4) {
            runApiLogin(argv[2], argv[3]);
            return 0;
        }
        if (cmd == "--api-register" && argc >= 4) {
            runApiRegister(argv[2], argv[3]);
            return 0;
        }
        if (cmd == "--api-analyze") {
            runApiAnalyzeStdin();
            return 0;
        }
        if (cmd == "--api-list") {
            runApiList();
            return 0;
        }
        if (cmd == "--api-save") {
            runApiSaveStdin();
            return 0;
        }
        if (cmd == "--api-delete" && argc >= 3) {
            runApiDelete(atoi(argv[2]));
            return 0;
        }
        if (cmd == "--api-stats") {
            runApiStats();
            return 0;
        }
        
        cout << "{\"error\":\"Invalid API command or arguments count.\"}" << endl;
        return 1;
    }

    // Classic Console Mode (defaults here if run directly with no arguments)
    LoginSystem login;
    login.showMenu();

    PromptManager mgr;
    int ch = -1;

    do {
        cout << "\n===== AI Prompt Engineering Studio =====\n";
        cout << " 1. Create New Prompt\n";
        cout << " 2. View All (Dashboard)\n";
        cout << " 3. Edit a Prompt\n";
        cout << " 4. Search Prompts\n";
        cout << " 5. Mark / Unmark Favorite\n";
        cout << " 6. View Version History\n";
        cout << " 7. Compare Two Prompts\n";
        cout << " 8. Delete a Prompt\n";
        cout << " 0. Save and Exit\n";
        cout << "=========================================\n";

        ch = getValidInt("Your choice: ", 0, 8);

        switch (ch) {
            case 1: mgr.addPrompt();      break;
            case 2: mgr.displayAll();     break;
            case 3: mgr.editPrompt();     break;
            case 4: mgr.searchPrompt();   break;
            case 5: mgr.toggleFavorite(); break;
            case 6: mgr.viewHistory();    break;
            case 7: mgr.compareTwo();     break;
            case 8: mgr.deletePrompt();   break;
            case 0:
                cout << "Saving...\n";
                mgr.save();
                cout << "Goodbye!\n";
                break;
        }
    } while (ch != 0);

    return 0;
}
