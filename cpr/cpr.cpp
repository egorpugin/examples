/*
c++: 17
dependencies:
    - pvt.cppan.demo.whoshuu.cpr: 1
    - pvt.egorpugin.primitives.string: master
    - pvt.egorpugin.primitives.executor: master
*/

#include <cpr/cpr.h>
#include <primitives/string.h>
#include <primitives/executor.h>

#include <regex>
#include <vector>

int run(const std::vector<int> &answers)
{
    auto r = cpr::Get(cpr::Url{ "http://" });
    auto cookies = r.cookies;

    for (int i = 2; i <= 31; i++)
    {
        r = cpr::Get(cpr::Url{ "http://" },
            cookies
        );
        for (auto &[k, v] : r.cookies.map_)
            cookies[k] = v;
    }

    cpr::Session session;
    session.SetUrl(cpr::Url{ "" });
    session.SetMultipart(cpr::Multipart{
        { "gender", "male" },
        });
    session.SetRedirect(true);
    session.SetCookies(cookies);
    r = session.Post();

    std::regex r("(\\d+)</title>");
    std::smatch m;
    if (std::regex_search(r.text, m, r))
    {
        return std::stoi(m[1].str());
    }

    return 0;
}

int main(int argc, char** argv)
{
    std::vector<int> answers{
    };
    std::vector<int> answers_correct{
    };
    auto answers2 = answers;

    auto max = run(answers_correct);
    auto base = run(answers);

    for (int i = 0; i < 30; i++)
    {
        for (int j = 1; j <= 6; j++)
        {
            getExecutor().push([=, &answers2]() mutable
            {
                answers[i] = j;
                auto r = run(answers);
                if (r - base > 3)
                    answers2[i] = j;
            });
        }
    }

    getExecutor().wait();

    return 0;
}
