#include <facter/facts/linux/networking_resolver.hpp>
#include <facter/facts/fact_map.hpp>
#include <facter/facts/scalar_value.hpp>
#include <facter/util/posix/scoped_descriptor.hpp>
#include <facter/util/file.hpp>
#include <facter/util/string.hpp>
#include <facter/logging/logging.hpp>
#include <cstring>
#include <netpacket/packet.h>
#include <net/if.h>
#include <sys/ioctl.h>

using namespace std;
using namespace facter::util;
using namespace facter::util::posix;

LOG_DECLARE_NAMESPACE("facts.linux.networking");

namespace facter { namespace facts { namespace linux {

    bool networking_resolver::is_link_address(sockaddr const* addr) const
    {
        return addr && addr->sa_family == AF_PACKET;
    }

    uint8_t const* networking_resolver::get_link_address_bytes(sockaddr const* addr) const
    {
        if (!is_link_address(addr)) {
            return nullptr;
        }
        sockaddr_ll const* link_addr = reinterpret_cast<sockaddr_ll const*>(addr);
        if (link_addr->sll_halen != 6) {
            return nullptr;
        }
        return reinterpret_cast<uint8_t const*>(link_addr->sll_addr);
    }

    int networking_resolver::get_link_mtu(string const& interface, void* data) const
    {
        // Unfortunately in Linux, the data points at interface statistics
        // Nothing useful for us, so we need to use ioctl to query the MTU
        ifreq req;
        memset(&req, 0, sizeof(req));
        strncpy(req.ifr_name, interface.c_str(), sizeof(req.ifr_name));

        scoped_descriptor sock(socket(AF_INET, SOCK_DGRAM, 0));
        if (static_cast<int>(sock) < 0) {
            LOG_WARNING("socket failed: %1% (%2%): interface MTU fact is unavailable for interface %3%.", strerror(errno), errno, interface);
            return -1;
        }

        if (ioctl(sock, SIOCGIFMTU, &req) != 0) {
            LOG_WARNING("ioctl failed: %1% (%2%): interface MTU fact is unavailable for interface %3%.", strerror(errno), errno, interface);
            return -1;
        }
        return req.ifr_mtu;
    }

    string networking_resolver::get_primary_interface()
    {
        // Read /proc/net/route to determine the primary interface
        // We consider the primary interface to be the one that has 0.0.0.0 as the
        // routing destination.
        string interface;
        file::each_line("/proc/net/route", [&interface](string& line) {
            auto parts = tokenize(line);
            if (parts.size() > 2 && parts[1] == "00000000") {
                interface = move(parts[0]);
                return false;
            }
            return true;
        });
        return interface;
    }

}}}  // namespace facter::facts::linux
