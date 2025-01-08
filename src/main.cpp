
#include <Geode/Geode.hpp>
#include <string>
#include <vector>
#include <map>
#include <Geode/modify/ProfilePage.hpp>

using namespace geode::prelude;

namespace CGlobal {
    std::map<std::string, std::vector<std::string>> members = {
        {"members", {"AstroTeamgd", "Lum1nGMD", "AstroNovaGD", "GMDPLUTONIC"}},
		{"tech", {"GMDPLUTONIC"}},
        {"members", {"Lum1nGMD", "AstroNovaGD", "GMDPLUTONIC", "AdutzHD", "Joevin Boger", "Martinosu", "NerdLabs", "QuickDash", "Bekeglockski", "Eternal Richiey", "Ethan Demon"}}
    };

    std::vector<std::string> staff = {
        "lum1ngmd", "astronovagd", "gmdplutonic"
    };

    template <typename T>
    bool objectExists(const T& target, const std::vector<T>& vec) {
        return std::find(vec.begin(), vec.end(), target) != vec.end();
    }

    std::string lowercaseString(std::string& _str) {
        std::string str = _str;

        for (int i = 0; i < str.length(); i++) {
            str[i] = std::tolower(str[i]);
        }

        return str;
    }

    class BadgeObject {
    public:
        CCSprite* m_sprite = nullptr;
        std::string m_role = "";
        std::string m_owner = "";

        void postSetup() {
            if (m_sprite == nullptr) return;
                
            m_sprite->setAnchorPoint({ 0.5, 0.6 });
        }
    };

    BadgeObject* setupSpriteFromUsername(std::string _username) {
        std::string username = lowercaseString(_username);
        
        bool isFound = false;
        std::string role_name;

        for (auto& [k, v] : members) {
            for (std::string& _u : v) {
                std::string u = lowercaseString(_u);

                if (u == username) {
                    isFound = true;
                    role_name = k;
                    
                    break;
                }
            }
            
            if (isFound) {
                break;
            }
        }

        if (!isFound) return nullptr;

        static std::map<std::string, std::string> spriteMap = {
            {"unknown", "Astro_Badge.png"_spr}
        };

        if (spriteMap.count(role_name) == 0) {
            log::error("Astro Team role cannot be found for user {}: unknown role \"{}\"", _username, role_name);

            return nullptr;
        }

        std::string sprite = spriteMap[role_name];

        CCSprite* sprObj = CCSprite::create(sprite.c_str());
        if (sprObj == nullptr) {
            log::error("badge \"{}\" (linked with role \"{}\") cannot be found", sprite, role_name);

            return nullptr;
        }

        BadgeObject* bobj = new BadgeObject();
        bobj->m_owner = _username;
        bobj->m_role = role_name;
        bobj->m_sprite = sprObj;

        return bobj;
    }

    std::string currentUsername;
    BadgeObject currentBadge;
}

class $modify(XProfilePage, ProfilePage) {
    std::string getPopupTitle() {
        return "Badge Info";
    }

    void onastroMember(CCObject *sender) {
        std::string popupTitle = getPopupTitle();

        if (CGlobal::currentUsername == "astroteamgd") {
            FLAlertLayer::create(popupTitle.c_str(), "This Account Uploads All Of The Official Collabs Made By Astro Team", "Close")->show();

            return;
        }
        if (CGlobal::objectExists<std::string>(CGlobal::lowercaseString(CGlobal::currentUsername), CGlobal::staff)) {
            std::string text = fmt::format("{} is <cp>Astro Team Staff</c>. {} can suggest people <cy>to join the astro team</c>.", CGlobal::currentUsername, CGlobal::currentUsername);

            FLAlertLayer::create(popupTitle.c_str(), text, "Close")->show();

            return;
        }

        std::string str = fmt::format("{} is a <cy>member</c> of <cb>Astro Team<c>.", CGlobal::currentUsername);

        std::string role = CGlobal::currentBadge.m_role;

        if (role == "leader") {
            return onastroLeader(sender);
        }

        std::map<std::string, std::string> roleMap1 = {
            {"leaders", "Leaders Of <cp>Astro Team</c>"},
            {"tech", "People Who Made Tech Stuff For <cp>Astro Team</c>"},
            {"members", "Members Of <cp>Astro Team</c>"}
        };
        static std::map<std::string, std::string> roleMap2 = {
            {"leaders", "<cb>These People</c> Are The <cr>Leaders</c> Of <cp>Astro Team</c>"},
			{"tech", "These Are The People Who Helped Out with tech related stuff for <cp>Astro Team</c>"},
            {"members", "These Are The <cb>Members</c> Of <cp>Astro Team</c>."}
        };

        if (roleMap1.count(role)) {
            str += fmt::format(" He is <cy>{}</c>.", roleMap1[role]);
        }

        if (roleMap2.count(role)) {
            str += fmt::format(" This player {}.", roleMap2[role]);
        }

        FLAlertLayer::create(popupTitle.c_str(), str, "Close")->show();
    }
    void onastroLeader(CCObject* sender) {
        std::string title = getPopupTitle();
        std::string str = fmt::format("{} Is the <cr>Leader</c> of <cp>Astro Team</c>.", CGlobal::currentUsername);

        FLAlertLayer::create(title.c_str(), str, "Close")->show();
    }

    void loadPageFromUserInfo(GJUserScore * a2) {
        ProfilePage::loadPageFromUserInfo(a2);
        auto layer = m_mainLayer;

        CCMenu* username_menu = typeinfo_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));

        bool isMain = false;
        bool isLeader = false;

        std::string username = a2->m_userName;
        CGlobal::currentUsername = username;
        
        auto badge = CGlobal::setupSpriteFromUsername(username);
        if (!badge) {
            return;
        }

        CCMenuItemSpriteExtra* badgeBtn = CCMenuItemSpriteExtra::create(badge->m_sprite, this, menu_selector(XProfilePage::onastroMember));
        badgeBtn->setID("astrobadge-badge");

        username_menu->addChild(badgeBtn);

        username_menu->updateLayout();
        updateLayout();

        badge->postSetup();

        CGlobal::currentBadge = *badge;

        delete badge;
    }
};
