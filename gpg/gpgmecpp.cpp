/*
c++: 17
dependencies:
    - org.sw.demo.gnu.gpg.gpgme.gpgmecpp: 1
    - pub.egorpugin.primitives.sw.main: master

    # cppan, old
    #- pvt.cppan.demo.gnu.gpg.gpgme.gpgmecpp: 1
    #- pvt.egorpugin.primitives.sw.main: master
*/

#include <gpgme.h>
#include <primitives/sw/main.h>

#define GPGMEPP_DEPRECATED
#include <context.h>
#include <data.h>
#include <key.h>
#include <keylistresult.h>
#include <verificationresult.h>

#include <iostream>

int main(int argc, char **argv)
{
    // set it to gpg path on the first run
    // if gpg was not run on the system before
    gpgme_set_global_flag("w32-inst-dir", "d:\\arh\\apps\\GnuPG\\bin");

    gpgme_check_version("1.11.1");

    auto c = GpgME::Context::create(GpgME::OpenPGP);
    if (!c)
        return 1;
    auto e = c->startKeyListing("");
    if (e)
    {
        std::cerr << e << "\n";
        return 1;
    }
    while (1)
    {
        auto k = c->nextKey(e);
        if (e)
        {
            std::cerr << e << "\n";
            break;
        }
        for (auto &sk : k.subkeys())
        {
            std::cout << sk.keyID();
            break;
        }
        std::cout << ": ";
        for (auto &u : k.userIDs())
        {
            std::cout << u.name() << " <" << u.email() << ">\n";
            break;
        }
    }
    c->endKeyListing();

    auto ftxt = read_file("d:/dev/examples/gpg/1.txt");
    auto fsig = read_file("d:/dev/examples/gpg/1.txt.sig");

    GpgME::Data txt(ftxt.data(), ftxt.size());
    GpgME::Data sig(fsig.data(), fsig.size());
    auto r = c->verifyDetachedSignature(sig, txt);
    if (r.error())
    {
        std::cerr << r.error() << "\n";
        return 1;
    }
    for (auto &s : r.signatures())
    {
        std::cout << s.fingerprint() << "\n";
        std::cout << s.hashAlgorithmAsString() << "\n";
        std::cout << s.publicKeyAlgorithmAsString() << "\n";
        std::cout << s.status() << "\n";
        std::cout << s.summary() << "\n";
        std::cout << s.validity() << "\n";
        std::cout << s.validityAsString() << "\n";
    }

    return 0;
}
