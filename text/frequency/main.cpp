#include <functional>
#include <iostream>
#include <map>
#include <string>

const std::string alph = "אבגדהוזחטיךכלםמןנסעףפץצקרשתûü‎‏ÿ";
const char freq[] = {'מ', 'ו', 'א', 'ט', 'ם', 'ע', 'ס', 'כ', 'ג', 'נ', 'ך', 'ה', 'ל', 'ף', 'ן', 'ÿ', 'ü', 'ד', 'û', 'ח', 'ב', 'ק', 'י', 'ז', 'ר', 'ץ', '‏', 'צ', 'ש', '‎', 'פ', 'ת'};
const std::string text = "חלהןמה‏הטפךהחלהןלז‏וןאפערüמאץןמעפךסךןמעÿעצ‎לץכךשאגהמלהןשופעלרלמךדאמעינך‏עדלÿהזפךץכךרודךמחשךץמךץחלהןכךשאגעהדמךדומעחשעכךרודךמעסהפענומלÿדךםעהדחהץזדשלהחלהןעחשךץמךץסהפענומלÿדךםעהדחהץזדשלהחשךץמךûךחלהןעזרךשךחלהןלזכנהםחהךסךןמעÿערךכךרודךמדנהדüדךמעלרלÿהדשהנדüדךמעשמךשהץגהץ‏וןאפהחלהןךסךןמעÿעהדנעןרלÿלה‏הםחוינך‏עדלÿהזפל‏לחלעדךמלÿהזפל‏כךרודךמע‏למעכנל‏הנ‏הםחוחךחךחלהןלחךנהסה‏ךרü‏הםחובמûענ‏ךמלÿהזפל‏לןשופע‏ל";

std::map<int, int> real_repl{
    {'ÿ', 'ÿ'},
    {'ח', 'ס'},
    //{'ל', 'נ'},
    {'ה', 'ו'},
    {'ן', 'ה'},
    {'מ', 'ם'},
    {'ÿ', 'ÿ'},
    {'ÿ', 'ÿ'},
    {'ÿ', 'ÿ'},
    {'ÿ', 'ÿ'},
    {'ÿ', 'ÿ'},
    {'ÿ', 'ÿ'},
    {'ÿ', 'ÿ'},
    {'ÿ', 'ÿ'},
    {'ÿ', 'ÿ'},
    {'ÿ', 'ÿ'},
    {'ÿ', 'ÿ'},
    {'ÿ', 'ÿ'},

    {'י', 'ÿ'},
    {'נ', 'ה'},
    {'ך', 'מ'},
    {'‏', 'ג'},
    {'ע', 'ט'},
    {'ד', 'ע'},
    {'ל', 'א'},
    {'ÿ', 'ÿ'},
    {'ÿ', 'ÿ'},
};

int main()
{
    for (int i = 1; i <= alph.size(); i++)
    {
        auto t = text;
        for (auto &c : t)
            c = alph[(c - alph[0] + i) % alph.size()];
        std::cout << t << "\n";
    }

    std::map<int, int> f;
    for (auto &c : text)
        f[c]++;

    std::multimap<int, int, std::greater<>> f2;
    for (auto &[k,v] : f)
        f2.insert({v,k});

    std::map<int, int> repl;
    int i = 0;
    for (auto &[k,v] : f2)
        repl[v] = freq[i++];

    auto t = text;
    for (auto &c : t)
        c = repl[c];
    std::cout << t << "\n";

    t = text;
    for (auto &c : t)
        c = real_repl.find(c) == real_repl.end() ? 'x' : real_repl[c];
    std::cout << t << "\n";

    for (auto &[k,v] : real_repl)
        repl[k] = v;

    t = text;
    for (auto &c : t)
        c = repl[c];
    std::cout << t << "\n";

    return 0;
}
