/*
dependencies:
    pvt.cppan.demo.arvidn.libtorrent: 1
*/

/*
local_settings:
    use_cache: false
    #storage_dir: s
options:
    any:
        definitions:
            public:
                - TORRENT_DEBUG
*/

//#include <libtorrent/torrent.hpp>
#include <libtorrent/create_torrent.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/alert_types.hpp>
#include <libtorrent/magnet_uri.hpp>
#include <libtorrent/extensions/ut_pex.hpp>

#include <fstream>

using namespace libtorrent;

void ct()
{
    file_storage fs;

    // recursively adds files in directories
    add_files(fs, "dir");

    create_torrent t(fs);
    t.add_tracker("udp://public.popcorn-tracker.org:6969/announce");
    t.set_creator("libtorrent example");

    // reads the files and calculates the hashes
    set_piece_hashes(t, ".");

    std::ofstream out("my_torrent.torrent", std::ios_base::binary);
    bencode(std::ostream_iterator<char>(out), t.generate());
}

void pub()
{
    settings_pack p;

    //p.set_bool(settings_pack::enable_dht, true);
    //p.set_bool(settings_pack::enable_lsd, true);

    p.set_str(settings_pack::dht_bootstrap_nodes,
        "dht.libtorrent.org:25401,router.bittorrent.com:6881,router.utorrent.com:6881,dht.transmissionbt.com:6881,dht.aelitis.com:6881");

    p.set_int(lt::settings_pack::alert_mask, lt::alert::status_notification | lt::alert::error_notification
        //| lt::alert::all_categories
    );

    session s(p);
    //s.add_extension(&create_ut_pex_plugin);

    add_torrent_params params;
    error_code ec;
    //parse_magnet_uri("magnet:?xt=urn:btih:aa1834038d772240929cd18a75187684863255f7", params, ec);
    //parse_magnet_uri("magnet:?xt=urn:btih:aa1834038d772240929cd18a75187684863255f7&tr=udp%3a%2f%2ftracker.coppersurfer.tk%3a6969%2fannounce&tr=udp%3a%2f%2ftracker.open-internet.nl%3a6969%2fannounce&tr=udp%3a%2f%2ftracker.skyts.net%3a6969%2fannounce&tr=udp%3a%2f%2ftracker.piratepublic.com%3a1337%2fannounce&tr=udp%3a%2f%2ftracker.opentrackr.org%3a1337%2fannounce&tr=udp%3a%2f%2ftracker.internetwarriors.net%3a1337%2fannounce&tr=udp%3a%2f%2f9.rarbg.to%3a2710%2fannounce&tr=udp%3a%2f%2fpublic.popcorn-tracker.org%3a6969%2fannounce&tr=udp%3a%2f%2fwambo.club%3a1337%2fannounce&tr=udp%3a%2f%2ftracker4.itzmx.com%3a2710%2fannounce&tr=udp%3a%2f%2ftracker1.wasabii.com.tw%3a6969%2fannounce&tr=udp%3a%2f%2ftracker.zer0day.to%3a1337%2fannounce&tr=udp%3a%2f%2ftracker.xku.tv%3a6969%2fannounce&tr=udp%3a%2f%2ftracker.vanitycore.co%3a6969%2fannounce&tr=udp%3a%2f%2fopen.facedatabg.net%3a6969%2fannounce&tr=udp%3a%2f%2fmgtracker.org%3a6969%2fannounce&tr=udp%3a%2f%2fipv4.tracker.harry.lu%3a80%2fannounce&tr=udp%3a%2f%2finferno.demonoid.pw%3a3418%2fannounce&tr=udp%3a%2f%2ftracker.tiny-vps.com%3a6969%2fannounce&tr=udp%3a%2f%2fp4p.arenabg.com%3a1337%2fannounce", params, ec);
    //parse_magnet_uri("magnet:?xt=urn:btih:615e18333efdafac0458f9bad0c72ccc222045f1&dn=sw.tar.gz&tr=udp%3a%2f%2fpublic.popcorn-tracker.org%3a6969%2fannounce", params, ec);
    //parse_magnet_uri("magnet:?xt=urn:btih:615e18333efdafac0458f9bad0c72ccc222045f1&dn=sw.tar.gz&tr=udp%3a%2f%2ftracker.vanitycore.co%3a6969%2fannounce", params, ec);

    //parse_magnet_uri("magnet:?xt=urn:btih:615e18333efdafac0458f9bad0c72ccc222045f1&dn=sw.tar.gz", params, ec);

    //parse_magnet_uri("magnet:?xt=urn:btih:621a36acc1f6df0d62f8e980c033bd710b4d5900", params, ec);
    //parse_magnet_uri("magnet:?xt=urn:btih:621a36acc1f6df0d62f8e980c033bd710b4d5900&dn=package_path.inl&tr=http%3a%2f%2fs3-tracker.eu-west-1.amazonaws.com%3a6969%2fannounce", params, ec);
    params.url = "https://s3-eu-west-1.amazonaws.com/software-network/package_path.inl?torrent";
    params.save_path = "out";
    s.add_torrent(params);

    for (;;)
    {
        std::vector<lt::alert *> alerts;
        s.pop_alerts(&alerts);

        for (lt::alert const *a : alerts)
        {
            std::cout << a->message() << std::endl;
            // if we receive the finished alert or an error, we're done
            if (lt::alert_cast<lt::torrent_finished_alert>(a))
            {
                goto done;
            }
            if (lt::alert_cast<lt::torrent_error_alert>(a))
            {
                goto done;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
done:
    std::cout << "done, shutting down" << std::endl;
}

int main()
{
    //ct();
    pub();

    return 0;
}
