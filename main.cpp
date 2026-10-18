#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "TemplatePopup.hpp"
using namespace geode::prelude;

class $modify(AutobuildUI, EditorUI) {
    bool init(LevelEditorLayer* lel) {
        if (!EditorUI::init(lel)) return false;

        if (auto menu = this->getChildByID("undo-menu")) {
            auto spr = ButtonSprite::create("Tpl", "bigFont.fnt", "GJ_button_04.png", .6f);
            spr->setScale(.8f);
            auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(AutobuildUI::onTemplates));
            btn->setID("templates-button"_spr);
            menu->addChild(btn);
            menu->updateLayout();
        }
        return true;
    }

    void onTemplates(CCObject*) {
        TemplatePopup::create(this)->show();
    }
};
