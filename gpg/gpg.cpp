/*
c++: 17
dependencies:
    - pvt.cppan.demo.gnu.gpg.gpgme.gpgmecpp: 1
    - pvt.egorpugin.primitives.sw.main: master
*/

#include <gpgme.h>
#include <primitives/sw/main.h>

int main(int argc, char **argv)
{
    gpgme_set_global_flag("w32-inst-dir", ".");

    gpgme_check_version("1.11.1");

    //gpgme_ctx_t ctx;
    //auto err = gpgme_new(&ctx);
    //err = gpgme_op_keylist_start(ctx, "*", 0);


    gpgme_ctx_t ctx;
    gpgme_key_t key;
    gpgme_error_t err = gpgme_new(&ctx);

    if (!err)
    {
        err = gpgme_op_keylist_start(ctx, "", 0);
        while (!err)
        {
            err = gpgme_op_keylist_next(ctx, &key);
            if (err)
                break;
            printf("%s:", key->subkeys->keyid);
            if (key->uids && key->uids->name)
                printf(" %s", key->uids->name);
            if (key->uids && key->uids->email)
                printf(" <%s>", key->uids->email);
            putchar('\n');
            gpgme_key_release(key);
        }
        gpgme_release(ctx);
    }
    if (gpg_err_code(err) != GPG_ERR_EOF)
    {
        fprintf(stderr, "can not list keys: %s\n", gpgme_strerror(err));
    }

    return 0;
}
