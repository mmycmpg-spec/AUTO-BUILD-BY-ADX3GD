#pragma once
#include <Geode/Geode.hpp>
#include <Geode/ui/ScrollLayer.hpp>
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/Notification.hpp>
#include <limits>
#include "Templates.hpp"
using namespace geode::prelude;

class TemplatePopup : public Popup {
protected:
    EditorUI* m_ui = nullptr;
    TextInput* m_nameInput = nullptr;
    ScrollLayer* m_scroll = nullptr;

    bool init(EditorUI* ui) {
        if (!Popup::init(320.f, 260.f)) return false;
        m_ui = ui;
        this->setTitle("Autobuild Templates");

        auto size = m_mainLayer->getContentSize();

        m_nameInput = TextInput::create(170.f, "Template name", "chatFont.fnt");
        m_nameInput->setPosition({size.width / 2 - 45.f, size.height - 50.f});
        m_mainLayer->addChild(m_nameInput);

        auto saveBtn = CCMenuItemExt::createSpriteExtra(
            ButtonSprite::create("Save", "goldFont.fnt", "GJ_button_01.png", .7f),
            [this](CCObject*) { this->saveSelection(); }
        );
        saveBtn->setPosition({size.width / 2 + 100.f, size.height - 50.f});
        m_buttonMenu->addChild(saveBtn);

        m_scroll = ScrollLayer::create({280.f, 150.f});
        m_scroll->setPosition({20.f, 22.f});
        m_mainLayer->addChild(m_scroll);

        this->refresh();
        return true;
    }

    void refresh() {
        auto content = m_scroll->m_contentLayer;
        content->removeAllChildren();
        content->setLayout(
            ColumnLayout::create()
                ->setAxisReverse(true)
                ->setAxisAlignment(AxisAlignment::End)
                ->setAutoGrowAxis(150.f)
                ->setGap(5.f)
        );

        for (auto const& [name, data] : loadTemplates()) {
            auto row = CCMenu::create();
            row->setContentSize({280.f, 30.f});

            auto applyBtn = CCMenuItemExt::createSpriteExtra(
                ButtonSprite::create(name.c_str(), 200, true, "bigFont.fnt", "GJ_button_04.png", 30.f, .5f),
                [this, name](CCObject*) { this->applyTemplate(name); }
            );
            applyBtn->setPosition({115.f, 15.f});
            row->addChild(applyBtn);

            auto delSpr = CCSprite::createWithSpriteFrameName("GJ_trashBtn_001.png");
            delSpr->setScale(.7f);
            auto delBtn = CCMenuItemExt::createSpriteExtra(delSpr, [this, name](CCObject*) {
                auto t = loadTemplates();
                t.erase(name);
                storeTemplates(t);
                this->refresh();
            });
            delBtn->setPosition({260.f, 15.f});
            row->addChild(delBtn);

            content->addChild(row);
        }
        content->updateLayout();
        m_scroll->scrollToTop();
    }

    void saveSelection() {
        auto sel = m_ui->getSelectedObjects();
        if (!sel || sel->count() == 0) {
            Notification::create("Select some objects first", NotificationIcon::Error)->show();
            return;
        }

        std::string data;
        for (auto obj : CCArrayExt<GameObject*>(sel)) {
            data += std::string(obj->getSaveString(m_ui->m_editorLayer));
        }

        auto templates = loadTemplates();
        std::string name = m_nameInput->getString();
        if (name.empty()) name = "Template " + std::to_string(templates.size() + 1);
        templates[name] = data;
        storeTemplates(templates);

        m_nameInput->setString("");
        Notification::create("Saved \"" + name + "\"", NotificationIcon::Success)->show();
        this->refresh();
    }

    void applyTemplate(std::string const& name) {
        auto templates = loadTemplates();
        auto it = templates.find(name);
        if (it == templates.end()) return;

        auto layer = m_ui->m_editorLayer;
        auto objs = layer->createObjectsFromString(it->second, true, true);
        if (!objs || objs->count() == 0) {
            Notification::create("Template is empty", NotificationIcon::Error)->show();
            return;
        }

        // centre of the new objects
        float minX = std::numeric_limits<float>::max(), minY = minX;
        float maxX = std::numeric_limits<float>::lowest(), maxY = maxX;
        for (auto obj : CCArrayExt<GameObject*>(objs)) {
            auto p = obj->getPosition();
            minX = std::min(minX, p.x); maxX = std::max(maxX, p.x);
            minY = std::min(minY, p.y); maxY = std::max(maxY, p.y);
        }
        CCPoint center = {(minX + maxX) / 2, (minY + maxY) / 2};

        // centre of the current editor view, in object-layer space
        auto winSize = CCDirector::get()->getWinSize();
        CCPoint target = layer->m_objectLayer->convertToNodeSpace(winSize / 2);
        CCPoint delta = target - center;

        for (auto obj : CCArrayExt<GameObject*>(objs)) {
            m_ui->moveObject(obj, delta);
        }
        m_ui->selectObjects(objs, false);

        this->onClose(nullptr);
    }

public:
    static TemplatePopup* create(EditorUI* ui) {
        auto ret = new TemplatePopup();
        if (ret->init(ui)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};
