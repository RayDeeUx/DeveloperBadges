#include "DeveloperBadges.hpp"
#include <Geode/binding/FLAlertLayer.hpp>
#include <Geode/loader/GameEvent.hpp>
#include <Geode/loader/Mod.hpp>
#include <hiimjasmine00.optional_settings/include/OptionalColor3BSetting.hpp>
#include <jasmine/setting.hpp>
#include <jasmine/web.hpp>

#include <alphalaneous.badgify/include/Badgify.hpp>

using namespace geode::prelude;
using namespace optional_settings;

std::map<int, DeveloperBadge> DeveloperBadges::developerBadges;

constexpr std::array settings = {
    std::make_pair("mod-developer-color", "mod-developer-color-toggle"),
    std::make_pair("verified-developer-color", "verified-developer-color-toggle"),
    std::make_pair("index-staff-color", "index-staff-color-toggle"),
    std::make_pair("lead-developer-color", "lead-developer-color-toggle")
};

$on_mod(Loaded) {
    auto mod = Mod::get();
    auto& data = mod->getSavedSettingsData();
    if (!mod->setSavedValue("migrated-colors", true)) {
        for (auto [key, toggle] : settings) {
            if (auto setting = jasmine::setting::get<std::optional<ccColor3B>>(key)) {
                auto oldColorValue = data.get<ccColor3B>(key);
                auto oldColorEnabled = data.get<bool>(toggle);
                if (oldColorValue.isOk() && oldColorEnabled.isOk()) {
                    setting->setStoredValue(oldColorValue.unwrap());
                    setting->setEnabled(oldColorEnabled.unwrap());
                }
            }
        }
    }
}

$on_game(Loaded) {
    spawn(
        web::WebRequest().get("https://badges.hiimjasmine00.com/developer"),
        [](web::WebResponse res) {
            if (!res.ok()) return;

            for (auto& value : jasmine::web::getArray(res)) {
                auto id = value.get<int>("id");
                if (!id.isOkAnd([](int id) { return id > 0; })) continue;

                auto type = value.get<int>("badge");
                if (!type.isOkAnd([](int type) { return type > 0 && type < 5; })) continue;

                auto name = value.get<std::string>("name");
                if (!name.isOk()) continue;

                DeveloperBadge badge(id.unwrap(), type.unwrap(), std::move(name).unwrap());
                DeveloperBadges::developerBadges.emplace(badge.id, std::move(badge));
            }
        }
    );
}

DeveloperBadge* DeveloperBadges::badgeForUser(int id) {
    auto badge = developerBadges.find(id);
    return badge != developerBadges.end() ? &badge->second : nullptr;
}

/*
constexpr std::array names = {
    "No Badge",
    "Mod Developer",
    "Verified Developer",
    "Index Staff",
    "Lead Developer"
};

constexpr std::array descriptions = {
    "does not have a badge",
    "is a <ca>mod developer</c> for <cy>Geode</c>.\n"
    "They have created mods that are available on the <cy>Geode mod index</c>.\n"
    "They will have to have new mods and mod updates approved by the <cd>index staff</c>",
    "is a <cp>verified mod developer</c> for <cy>Geode</c>.\n"
    "They can update their mods on the <cy>Geode mod index</c> without the need for approval.\n"
    "They will still have to have new mods approved by the <cd>index staff</c>",
    "is a member of the <cd>index staff</c> for <cy>Geode</c>.\n"
    "They can approve or reject mods uploaded to the <cy>Geode mod index</c>",
    "is a <co>lead developer</c> of <cy>Geode</c>.\n"
    "They are part of the main development team and have significant contributions to the <cy>Geode ecosystem</c>"
};

void DeveloperBadges::showBadgeInfo(std::string_view username, int type) {
    FLAlertLayer::create(
        names[type < names.size() ? type : 0],
        fmt::format("<cg>{}</c> {}.", username, descriptions[type < descriptions.size() ? type : 0]),
        "OK"
    )->show();
}
*/

$on_mod(Loaded) {
    alpha::badgify::registerBadge(
        "mod-developer"_spr,
        "Mod Developer",
        "This user is a <ca>mod developer</c> for <cy>Geode</c>.\n\n"
        "They have created mods that are available on the <cy>Geode mod index</c>.\n\n"
        "They will have to have new mods and mod updates approved by the <cd>index staff</c>",
        [] (const alpha::badgify::Badge& badgeFromAlpha) {
            DeveloperBadges* badge = DeveloperBadges::badgeForUser(badge.user.data()->m_accountID);
            if (!badge || badge->type != 1) return;

            CCSprite* badgeButton = CCSprite::createWithSpriteFrameName("badge01.png"_spr);
            badgeButton->setTag(badgeType);

            // this is fine because it'll be reset on a new account ID anyway
            alpha::badgify::setDescription("mod-developer"_spr,
                fmt::format(
                    "<cg>{}</c> is a <ca>mod developer</c> for <cy>Geode</c>.\n\n"
                    "They have created mods that are available on the <cy>Geode mod index</c>.\n\n"
                    "They will have to have new mods and mod updates approved by the <cd>index staff</c>.",
                    static_cast<std::string>(badgeFromAlpha.user.data()->m_userName)
                )
            );

            alpha::badgify::showBadge(badgeFromAlpha, badgeButton);
        }
    );
    alpha::badgify::registerBadge(
        "verified-developer"_spr,
        "Verified Developer",
        "This user is a <cp>verified mod developer</c> for <cy>Geode</c>.\n\n"
        "They can update their mods on the <cy>Geode mod index</c> without the need for approval.\n\n"
        "They will still have to have new mods approved by the <cd>index staff</c>",
        [] (const alpha::badgify::Badge& badgeFromAlpha) {
            DeveloperBadges* badge = DeveloperBadges::badgeForUser(badge.user.data()->m_accountID);
            if (!badge || badge->type != 2) return;

            CCSprite* badgeButton = CCSprite::createWithSpriteFrameName("badge02.png"_spr);
            badgeButton->setTag(badgeType);

            // this is fine because it'll be reset on a new account ID anyway
            alpha::badgify::setDescription("verified-developer"_spr,
                fmt::format(
                    "<cg>{}</c> is a <cp>verified mod developer</c> for <cy>Geode</c>.\n\n"
                    "They can update their mods on the <cy>Geode mod index</c> without the need for approval.\n\n"
                    "They will still have to have new mods approved by the <cd>index staff</c>.",
                    static_cast<std::string>(badgeFromAlpha.user.data()->m_userName)
                )
            );

            alpha::badgify::showBadge(badgeFromAlpha, badgeButton);
        }
    );
    alpha::badgify::registerBadge(
        "index-staff"_spr,
        "Index Staff",
        "This user is a member of the <cd>index staff</c> for <cy>Geode</c>.\n\n"
        "They can approve or reject mods uploaded to the <cy>Geode mod index</c>.",
        [] (const alpha::badgify::Badge& badgeFromAlpha) {
            DeveloperBadges* badge = DeveloperBadges::badgeForUser(badge.user.data()->m_accountID);
            if (!badge || badge->type != 3) return;

            CCSprite* badgeButton = CCSprite::createWithSpriteFrameName("badge03.png"_spr);
            badgeButton->setTag(badgeType);

            // this is fine because it'll be reset on a new account ID anyway
            alpha::badgify::setDescription("index-staff"_spr,
                fmt::format(
                    "<cg>{}</c> is a member of the <cd>index staff</c> for <cy>Geode</c>.\n\n"
                    "They can approve or reject mods uploaded to the <cy>Geode mod index</c>.",
                    static_cast<std::string>(badgeFromAlpha.user.data()->m_userName)
                )
            );

            alpha::badgify::showBadge(badgeFromAlpha, badgeButton);
        }
    );
    alpha::badgify::registerBadge(
        "lead-developer"_spr,
        "Lead Developer",
        "This user is a <co>lead developer</c> of <cy>Geode</c>.\n\n"
        "They are part of the main development team and have significant contributions to the <cy>Geode ecosystem</c>.",
        [] (const alpha::badgify::Badge& badgeFromAlpha) {
            DeveloperBadges* badge = DeveloperBadges::badgeForUser(badge.user.data()->m_accountID);
            if (!badge || badge->type != 4) return;

            CCSprite* badgeButton = CCSprite::createWithSpriteFrameName("badge04.png"_spr);
            badgeButton->setTag(badgeType);

            // this is fine because it'll be reset on a new account ID anyway
            alpha::badgify::setDescription("lead-developer"_spr,
                fmt::format(
                    "<cg>{}</c> is a <co>lead developer</c> of <cy>Geode</c>.\n\n"
                    "They are part of the main development team and have significant contributions to the <cy>Geode ecosystem</c>.",
                    static_cast<std::string>(badgeFromAlpha.user.data()->m_userName)
                )
            );

            alpha::badgify::showBadge(badgeFromAlpha, badgeButton);
        }
    );
}