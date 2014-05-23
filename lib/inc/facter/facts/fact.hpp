#ifndef FACTER_FACTS_FACT_HPP_
#define FACTER_FACTS_FACT_HPP_

namespace facter { namespace facts {

    /**
     * Stores the constant fact names.
     */
    struct fact
    {
        // kernel facts
        constexpr static char const* kernel = "kernel";
        constexpr static char const* kernel_version = "kernelversion";
        constexpr static char const* kernel_release = "kernelrelease";
        constexpr static char const* kernel_major_version = "kernelmajversion";

        // operating system facts
        constexpr static char const* operating_system = "operatingsystem";
        constexpr static char const* os_family = "osfamily";
        constexpr static char const* operating_system_release = "operatingsystemrelease";
        constexpr static char const* operating_system_major_release = "operatingsystemmajrelease";
        constexpr static char const* hostname = "hostname";
        constexpr static char const* lsb_dist_id = "lsbdistid";
        constexpr static char const* lsb_dist_release = "lsbdistrelease";
        constexpr static char const* lsb_dist_codename = "lsbdistcodename";
        constexpr static char const* lsb_dist_description = "lsbdistdescription";
        constexpr static char const* lsb_dist_major_release = "lsbmajdistrelease";
        constexpr static char const* lsb_dist_minor_release = "lsbminordistrelease";
        constexpr static char const* lsb_release = "lsbrelease";

        // network facts
        constexpr static char const* ipaddress = "ipaddress";
        constexpr static char const* ipaddress6 = "ipaddress6";
        constexpr static char const* mtu = "mtu";
        constexpr static char const* netmask = "netmask";
        constexpr static char const* netmask6 = "netmask6";
        constexpr static char const* network = "network";
        constexpr static char const* network6 = "network6";
        constexpr static char const* macaddress = "macaddress";
        constexpr static char const* interfaces = "interfaces";
        constexpr static char const* domain = "domain";
        constexpr static char const* fqdn = "fqdn";

        // block device facts
        constexpr static char const* block_device = "blockdevice";
        constexpr static char const* block_devices = "blockdevices";

        // processor facts
        constexpr static char const* processor = "processor";
        constexpr static char const* processor_count = "processorcount";
        constexpr static char const* physical_processor_count = "physicalprocessorcount";

        // dmidecode facts
        constexpr static char const* hardware_isa = "hardwareisa";
        constexpr static char const* hardware_model = "hardwaremodel";
        constexpr static char const* architecture = "architecture";
        constexpr static char const* bios_vendor = "bios_vendor";
        constexpr static char const* bios_version = "bios_version";
        constexpr static char const* bios_release_date = "bios_release_date";
        constexpr static char const* board_manufacturer = "boardmanufacturer";
        constexpr static char const* board_product_name = "boardproductname";
        constexpr static char const* board_serial_number = "boardserialnumber";
        constexpr static char const* manufacturer = "manufacturer";
        constexpr static char const* product_name = "productname";
        constexpr static char const* serial_number = "serialnumber";
        constexpr static char const* product_uuid = "productuuid";
        constexpr static char const* chassis_type = "chassistype";

        // uptime facts
        constexpr static char const* uptime = "uptime";
        constexpr static char const* uptime_days = "uptime_days";
        constexpr static char const* uptime_hours = "uptime_hours";
        constexpr static char const* uptime_seconds = "uptime_seconds";

        // selinux facts
        constexpr static char const* selinux = "selinux";
        constexpr static char const* selinux_enforced = "selinux_enforced";
        constexpr static char const* selinux_policyversion = "selinux_policyversion";
        constexpr static char const* selinux_current_mode = "selinux_current_mode";
        constexpr static char const* selinux_config_mode = "selinux_config_mode";
        constexpr static char const* selinux_config_policy = "selinux_config_policy";

        // ssh facts
        constexpr static char const* ssh_dsa_key = "sshdsakey";
        constexpr static char const* ssh_rsa_key = "sshrsakey";
        constexpr static char const* ssh_ecdsa_key = "sshecdsakey";
        constexpr static char const* ssh_ed25519_key = "sshed25519key";
        constexpr static char const* sshfp_dsa = "sshfp_dsa";
        constexpr static char const* sshfp_rsa = "sshfp_rsa";
        constexpr static char const* sshfp_ecdsa = "sshfp_ecdsa";
        constexpr static char const* sshfp_ed25519 = "sshfp_ed25519";
    };

}}  // namespace facter::facts

#endif  // FACTER_FACTS_FACT_HPP_