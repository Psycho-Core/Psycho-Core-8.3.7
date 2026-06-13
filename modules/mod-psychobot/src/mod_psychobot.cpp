/*
 * ===========================================================================
 *  Copyright (c) 2026 Psycho-core. All rights reserved.
 *  Original work authored 100% from scratch for Psycho_Core.
 *  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
 *  NOT covered by the base GPL framework license. Development/evaluation only.
 *
 *  mod-psychobot - Stage 1 scripts: world tick, logout cleanup, and the
 *  ".psychobot" command. Bot AI lives in PsychobotMgr / PsychobotAI.
 * ===========================================================================
 */

#include "ScriptMgr.h"
#include "Chat.h"
#include "Player.h"
#include "WorldSession.h"
#include "Config.h"
#include "Log.h"
#include "RBAC.h"
#include "StringFormat.h"
#include <cstdlib>
#include "PsychobotMgr.h"
#include "PsychobotFactory.h"
#include "PsychobotPopulationMgr.h"
#include "PsychobotAhBot.h"
#include "dungeon/PsychobotDungeonMgr.h"

using namespace psychobot;

// ---------------------------------------------------------------------------
// WorldScript - drives the bot manager tick and reports config state.
// ---------------------------------------------------------------------------
class psychobot_WorldScript : public WorldScript
{
public:
    psychobot_WorldScript() : WorldScript("psychobot_WorldScript") { }

    void OnConfigLoad(bool reload) override
    {
        bool enabled = sConfigMgr->GetBoolDefault("Psychobot.Enable", false);
        // Stage 3/4: (re)load population/scaling + ahbot config too.
        sPsychobotPopulation->LoadConfig();
        sPsychobotAhBot->LoadConfig();
        // S25: seed the dungeon/raid encounter-script registry (idempotent).
        psychobot::DungeonMgr::InitEncounters();
        TC_LOG_INFO("module.psychobot", "mod-psychobot config %s (Psychobot.Enable = %u, RandomBots = %u).",
            reload ? "reloaded" : "loaded", enabled ? 1 : 0,
            sPsychobotPopulation->Config().enable ? 1 : 0);
    }

    void OnUpdate(uint32 diff) override
    {
        if (!sConfigMgr->GetBoolDefault("Psychobot.Enable", false))
            return;
        sPsychobotMgr->UpdateAI(diff);
        sPsychobotAhBot->Update(diff);
    }
};

// ---------------------------------------------------------------------------
// PlayerScript - cleanup when a master or bot logs out.
// ---------------------------------------------------------------------------
class psychobot_PlayerScript : public PlayerScript
{
public:
    psychobot_PlayerScript() : PlayerScript("psychobot_PlayerScript") { }

    void OnLogout(Player* player) override
    {
        sPsychobotMgr->OnPlayerLogout(player);
    }

    // S28: when a socketless bot finishes loading into the world, attach its AI.
    void OnLogin(Player* player, bool /*firstLogin*/) override
    {
        sPsychobotMgr->OnPlayerLogin(player);
    }
};

// ---------------------------------------------------------------------------
// CommandScript - ".psychobot add|remove|list"
// ---------------------------------------------------------------------------
class psychobot_CommandScript : public CommandScript
{
public:
    psychobot_CommandScript() : CommandScript("psychobot_CommandScript") { }

    std::vector<ChatCommand> GetCommands() const override
    {
        static std::vector<ChatCommand> factoryCommandTable =
        {
            { "purge", 1212, false, &HandleFactoryPurgeCommand, "" },
            { "spawn", 1214, false, &HandleFactorySpawnCommand, "" },
        };
        static std::vector<ChatCommand> psychobotCommandTable =
        {
            { "add",      1201, false, &HandleAddCommand,      "" },
            { "remove",   1202, false, &HandleRemoveCommand,   "" },
            { "list",     1203, false, &HandleListCommand,     "" },
            { "spec",     1204, false, &HandleSpecCommand,     "" },
            { "group",    1205, false, &HandleGroupCommand,    "" },
            { "follow",   1206, false, &HandleFollowCommand,   "" },
            { "stay",     1207, false, &HandleStayCommand,     "" },
            { "attack",   1208, false, &HandleAttackCommand,   "" },
            { "cast",     1209, false, &HandleCastCommand,     "" },
            { "strategy", 1210, false, &HandleStrategyCommand, "" },
            { "factory",  1211, false, nullptr, "", factoryCommandTable },
            { "help",     1213, false, &HandleHelpCommand,     "" },
            { "summon",   1215, false, &HandleSummonCommand,   "" },
        };
        static std::vector<ChatCommand> commandTable =
        {
            { "psychobot", 1200, false, nullptr, "", psychobotCommandTable },
        };
        return commandTable;
    }

    static bool HandleAddCommand(ChatHandler* handler, char const* args)
    {
        Player* master = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string name = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->AddBot(master, name).c_str());
        return true;
    }

    static bool HandleRemoveCommand(ChatHandler* handler, char const* args)
    {
        Player* master = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string name = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->RemoveBot(master, name).c_str());
        return true;
    }

    static bool HandleListCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* master = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        handler->SendSysMessage(sPsychobotMgr->ListBots(master).c_str());
        return true;
    }

    static bool HandleSpecCommand(ChatHandler* handler, char const* args)
    {
        Player* master = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string a = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->SetSpec(master, a).c_str());
        return true;
    }

    static bool HandleGroupCommand(ChatHandler* handler, char const* args)
    {
        Player* master = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string name = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->GroupBot(master, name).c_str());
        return true;
    }

    // --- S27 order/grammar commands ---------------------------------------
    static bool HandleFollowCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* m = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        handler->SendSysMessage(sPsychobotMgr->OrderFollow(m).c_str());
        return true;
    }

    static bool HandleStayCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* m = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        handler->SendSysMessage(sPsychobotMgr->OrderStay(m).c_str());
        return true;
    }

    static bool HandleAttackCommand(ChatHandler* handler, char const* /*args*/)
    {
        Player* m = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        handler->SendSysMessage(sPsychobotMgr->OrderAttack(m).c_str());
        return true;
    }

    static bool HandleCastCommand(ChatHandler* handler, char const* args)
    {
        Player* m = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string spell = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->OrderCast(m, spell).c_str());
        return true;
    }

    static bool HandleStrategyCommand(ChatHandler* handler, char const* args)
    {
        Player* m = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string a = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->ToggleStrategy(m, a).c_str());
        return true;
    }

    static bool HandleFactoryPurgeCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->SendSysMessage("Purging all bot characters and accounts...");
        sPsychobotFactory->PurgeAllBots();
        handler->SendSysMessage("Purge complete.");
        return true;
    }

    static bool HandleFactorySpawnCommand(ChatHandler* handler, char const* args)
    {
        int count = 1;
        if (args && *args)
            count = atoi(args);

        if (count < 1) count = 1;
        if (count > 50) count = 50;

        handler->SendSysMessage(Trinity::StringFormat("Attempting to generate %d bots...", count).c_str());
        int success = 0;
        for (int i = 0; i < count; ++i)
        {
            if (sPsychobotFactory->GenerateOneBot())
                success++;
        }
        handler->SendSysMessage(Trinity::StringFormat("Successfully generated %d bots. (They will be picked up by the PopulationMgr next tick).", success).c_str());
        return true;
    }

    static bool HandleSummonCommand(ChatHandler* handler, char const* args)
    {
        Player* master = handler->GetSession() ? handler->GetSession()->GetPlayer() : nullptr;
        std::string name = args ? args : "";
        handler->SendSysMessage(sPsychobotMgr->SummonBot(master, name).c_str());
        return true;
    }

    static bool HandleHelpCommand(ChatHandler* handler, char const* /*args*/)
    {
        handler->SendSysMessage("Psychobot commands:");
        handler->SendSysMessage("  .psychobot add|remove|list|group <name>   - manage bots");
        handler->SendSysMessage("  .psychobot spec <name> <0-3>              - set a bot's spec");
        handler->SendSysMessage("  .psychobot follow|stay|attack            - order all your bots");
        handler->SendSysMessage("  .psychobot cast <spell name>             - all bots cast a spell");
        handler->SendSysMessage("  .psychobot strategy <name>               - toggle a combat strategy");
        return true;
    }
};

// ---------------------------------------------------------------------------
// Registrator called by the module loader.
// ---------------------------------------------------------------------------
void AddSC_mod_psychobot()
{
    new psychobot_WorldScript();
    new psychobot_PlayerScript();
    new psychobot_CommandScript();
}
