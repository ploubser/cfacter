#include <facter/util/windows/wmi.hpp>
#include <facter/util/windows/string_conv.hpp>
#include <facter/util/scoped_resource.hpp>
#include <facter/logging/logging.hpp>
#include <facter/execution/execution.hpp>
#include <boost/algorithm/string/join.hpp>

#define _WIN32_DCOM
#include <comdef.h>
#include <wbemidl.h>

using namespace std;
using namespace facter::execution;

#undef LOG_NAMESPACE
#define LOG_NAMESPACE "facter.util.windows.wmi"

namespace facter { namespace util { namespace windows { namespace wmi {

    // GUID taken from a Windows installation and unaccepted change to MinGW-w64. The MinGW-w64 library
    // doesn't define it, but obscures the Windows Platform SDK version of wbemuuid.lib.
    static CLSID MyCLSID_WbemLocator = {0x4590f811, 0x1d3a, 0x11d0, 0x89, 0x1f, 0x00, 0xaa, 0x00, 0x4b, 0x2e, 0x24};

    class WMIQuery {
        WMIQuery()
        {
            auto hres = CoInitializeEx(0, COINIT_MULTITHREADED);
            if (FAILED(hres)) {
                throw runtime_error("failed to initialize COM library");
            }
            _coInit = scoped_resource<bool>(true, [](bool b) { CoUninitialize(); });

            hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT,
                RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
            if (FAILED(hres)) {
                throw runtime_error("failed to initialize security");
            }

            IWbemLocator *pLoc;
            hres = CoCreateInstance(MyCLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator,
                reinterpret_cast<LPVOID *>(&pLoc));
            if (FAILED(hres)) {
                throw runtime_error("failed to create IWbemLocator object");
            }
            _pLoc = scoped_resource<IWbemLocator *>(move(pLoc),
                [](IWbemLocator *loc) { if (loc) loc->Release(); });

            IWbemServices *pSvc;
            hres = (*_pLoc).ConnectServer(_bstr_t(L"ROOT\\CIMV2"), nullptr, nullptr, nullptr, 0, nullptr, nullptr, &pSvc);
            if (FAILED(hres)) {
                throw runtime_error("could not connect to WMI server");
            }
            _pSvc = scoped_resource<IWbemServices *>(move(pSvc),
                [](IWbemServices *svc) { if (svc) svc->Release(); });

            hres = CoSetProxyBlanket(_pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
            if (FAILED(hres)) {
                throw runtime_error("could not set proxy blanket");
            }
        }

        scoped_resource<bool> _coInit;
        scoped_resource<IWbemLocator *> _pLoc;
        scoped_resource<IWbemServices *> _pSvc;

     public:
        imap query(string const& group, vector<string> const& keys) const
        {
            IEnumWbemClassObject *_pEnum = NULL;
            string qry = "SELECT " + boost::join(keys, ",") + " FROM " + group;

            auto hres = (*_pSvc).ExecQuery(_bstr_t(L"WQL"), _bstr_t(to_utf16(qry).c_str()),
                WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &_pEnum);
            if (FAILED(hres)) {
                LOG_DEBUG("query %1% failed", qry);
                return {};
            }
            scoped_resource<IEnumWbemClassObject *> pEnum(move(_pEnum),
                [](IEnumWbemClassObject *rsc) { if (rsc) rsc->Release(); });

            imap vals;

            IWbemClassObject *pclsObj;
            ULONG uReturn = 0;
            while (pEnum) {
                auto hr = (*pEnum).Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
                if (FAILED(hr) || 0 == uReturn) {
                    break;
                }

                for (auto &s : keys) {
                    VARIANT vtProp;
                    hr = pclsObj->Get(_bstr_t(to_utf16(s).c_str()), 0, &vtProp, 0, 0);
                    if (FAILED(hr)) {
                        break;
                    }
                    vals.emplace(s, to_utf8(vtProp.bstrVal));
                    VariantClear(&vtProp);
                }

                pclsObj->Release();
            }

            return vals;
        }

        static WMIQuery const& get()
        {
            static WMIQuery _instance;
            return _instance;
        }
    };

    imap query(string const& group, vector<string> const& keys)
    {
        try {
            return WMIQuery::get().query(group, keys);
        } catch (runtime_error &e) {
            LOG_DEBUG(e.what());
            return {};
        }
    }

}}}}  // namespace facter::util::windows::wmi