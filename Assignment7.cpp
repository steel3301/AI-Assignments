#include <bits/stdc++.h>
using namespace std;

/*
 * Forward Chaining over propositional Horn rules.
 * Input (interactive):
 *   fact X
 *   rule A & B -> C
 *   run
 *   ask X
 *   show
 *   exit
 * Example:
 *   fact mammal
 *   fact has_hair
 *   rule mammal & lays_eggs -> monotreme
 *   rule has_hair -> mammal
 *   run
 *   ask monotreme
 */

struct Rule {
    vector<string> antecedents;
    string consequent;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    unordered_set<string> facts; // known true
    vector<Rule> rules;

    auto trim = [](string s){
        auto issp = [](char c){ return isspace((unsigned char)c); };
        while(!s.empty() && issp(s.front())) s.erase(s.begin());
        while(!s.empty() && issp(s.back())) s.pop_back();
        return s;
    };

    cout << "Forward Chaining. Type 'help' for commands.\n";
    string line;
    while (true) {
        cout << "> ";
        if (!getline(cin, line)) break;
        line = trim(line);
        if (line=="") continue;
        if (line=="exit") break;
        if (line=="help") {
            cout << "Commands:\n"
                 << "  fact <symbol>\n"
                 << "  rule <A [& B [& ...]] -> C>\n"
                 << "  run        # infer to fixpoint\n"
                 << "  ask <symbol>\n"
                 << "  show\n"
                 << "  exit\n";
            continue;
        }
        if (line.rfind("fact ",0)==0) {
            string f = trim(line.substr(5));
            if (!f.empty()) { facts.insert(f); cout << "ok\n"; }
            continue;
        }
        if (line.rfind("rule ",0)==0) {
            string body = trim(line.substr(5));
            auto pos = body.find("->");
            if (pos==string::npos){ cout<<"bad rule\n"; continue; }
            string lhs = trim(body.substr(0,pos));
            string rhs = trim(body.substr(pos+2));
            vector<string> ants;
            {
                string token; stringstream ss(lhs);
                while (getline(ss, token, '&')) ants.push_back(trim(token));
            }
            rules.push_back({ants, rhs});
            cout << "ok\n"; continue;
        }
        if (line=="run") {
            bool changed=true; size_t iterations=0;
            while (changed) {
                changed=false; iterations++;
                for (auto &r: rules) {
                    bool all=true;
                    for (auto &a: r.antecedents) if (!facts.count(a)) { all=false; break; }
                    if (all && !facts.count(r.consequent)) {
                        facts.insert(r.consequent);
                        changed=true;
                        cout << "Derived: " << r.consequent << "\n";
                    }
                }
                if (iterations>10000) { cout<<"stopping (loop guard)\n"; break; }
            }
            cout << "Done. Total facts: " << facts.size() << "\n";
            continue;
        }
        if (line.rfind("ask ",0)==0) {
            string q = trim(line.substr(4));
            cout << (facts.count(q) ? "YES\n" : "NO\n");
            continue;
        }
        if (line=="show") {
            cout << "Facts:\n"; for (auto &f: facts) cout << "  " << f << "\n";
            cout << "Rules:\n";
            for (auto &r: rules) {
                for (size_t i=0;i<r.antecedents.size();++i) {
                    if (i) cout << " & ";
                    cout << r.antecedents[i];
                }
                cout << " -> " << r.consequent << "\n";
            }
            continue;
        }
        cout << "unknown command\n";
    }
    return 0;
}
