#include "SDK.hpp"
#include <windows.h>
#include <wininet.h>
#include <algorithm>
#include <string>

#pragma comment(lib, "wininet.lib")

namespace capware {
    struct sdk::impl {
        std::string base = "https://capware.fun";

        uint64_t uworld_level_ = 0;
        uint64_t uworld_instance_ = 0;
        uint64_t uworld_state_ = 0;
        uint64_t actor_root_ = 0;
        uint64_t player_team_ = 0;
        uint64_t player_name_ = 0;

        bool http_get(const std::string& url, std::string& response) const {
            response.clear();

            HINTERNET internet = InternetOpenA("CapWare SDK/1.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
            if (!internet) return false;

            HINTERNET connect = InternetOpenUrlA(internet, url.c_str(), NULL, 0,
                INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0);

            if (!connect) {
                InternetCloseHandle(internet);
                return false;
            }

            DWORD timeout_ms = 10000;
            InternetSetOptionA(connect, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout_ms, sizeof(timeout_ms));

            char buffer[8192];
            DWORD bytes_read = 0;

            while (InternetReadFile(connect, buffer, sizeof(buffer), &bytes_read) && bytes_read > 0) {
                response.append(buffer, bytes_read);
            }

            InternetCloseHandle(connect);
            InternetCloseHandle(internet);
            return !response.empty();
        }

        static uint64_t parse_hex(std::string s) {
            if (s.empty()) return 0;
            if (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) s = s.substr(2);
            s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());
            try { return std::stoull(s, nullptr, 16); }
            catch (...) { return 0; }
        }

        static std::string extract(const std::string& j, const std::string& k) {
            std::string sk = "\"" + k + "\":";
            size_t p = j.find(sk);
            if (p == std::string::npos) return "";
            p += sk.length();

            while (p < j.length() && isspace(static_cast<unsigned char>(j[p]))) p++;
            if (p >= j.length()) return "";

            if (j[p] == '\"') {
                size_t e = j.find('\"', p + 1);
                return (e != std::string::npos) ? j.substr(p + 1, e - p - 1) : "";
            }

            size_t e = p;
            while (e < j.length() && (isxdigit(static_cast<unsigned char>(j[e])) || j[e] == 'x' || j[e] == 'X')) e++;
            return j.substr(p, e - p);
        }

        bool health() const {
            std::string res;
            return http_get(base + "/api/health", res) &&
                res.find("\"status\":\"ok\"") != std::string::npos;
        }

        uint64_t get_offset(const std::string& c, const std::string& m) const {
            std::string res;
            if (!http_get(base + "/api/offset/" + c + "/" + m, res))
                return 0;
            return parse_hex(extract(res, "offset"));
        }
    };

    sdk::sdk() : p_(std::make_unique<impl>()) {}
    sdk::~sdk() = default;

    bool sdk::initialize() {
        if (!p_->health())
            return false;

        p_->uworld_level_ = p_->get_offset("UWorld", "PersistentLevel");
        p_->uworld_instance_ = p_->get_offset("UWorld", "OwningGameInstance");
        p_->uworld_state_ = p_->get_offset("UWorld", "GameState");
        p_->actor_root_ = p_->get_offset("Actor", "RootComponent");
        p_->player_team_ = p_->get_offset("FortPlayerStateAthena", "TeamIndex");
        p_->player_name_ = p_->get_offset("APlayerState", "PlayerNamePrivate");

        return true;
    }

    bool sdk::health() const { return p_->health(); }
    uint64_t sdk::get_offset(const std::string& c, const std::string& m) { return p_->get_offset(c, m); }

    uint64_t sdk::uworld_level() const { return p_->uworld_level_; }
    uint64_t sdk::uworld_instance() const { return p_->uworld_instance_; }
    uint64_t sdk::uworld_state() const { return p_->uworld_state_; }
    uint64_t sdk::actor_root() const { return p_->actor_root_; }
    uint64_t sdk::player_team() const { return p_->player_team_; }
    uint64_t sdk::player_name() const { return p_->player_name_; }
}