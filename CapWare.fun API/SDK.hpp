#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <memory>

namespace capware {
    class sdk {
    public:
        sdk();
        ~sdk();

        bool initialize();
        bool health() const;

        uint64_t get_offset(const std::string& class_name, const std::string& member_name);
        bool search(const std::string& query, std::vector<std::string>& results) const;

        uint64_t uworld_level() const;
        uint64_t uworld_instance() const;
        uint64_t uworld_state() const;
        uint64_t actor_root() const;
        uint64_t player_team() const;
        uint64_t player_name() const;

    private:
        struct impl;
        std::unique_ptr<impl> p_;
    };
}
