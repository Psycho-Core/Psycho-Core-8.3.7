-- ===========================================================================
--  Copyright (c) 2026 Psycho-core. All rights reserved.
--  Original work authored 100% from scratch for Psycho_Core.
--  Licensed under LICENSE.MYCODE (see LICENSE.MYCODE.txt in the repo root).
--  NOT covered by the base GPL framework license. Development/evaluation only.
-- ===========================================================================

local addonName = "PsychobotUI"
local botTable = {}
local selectedBot = nil

-- ---------------------------------------------------------------------------
-- UI COMPONENTS
-- ---------------------------------------------------------------------------

local MainPanel = CreateFrame("Frame", "PsychobotMainPanel", UIParent, "BackdropTemplate")
MainPanel:SetSize(320, 420)
MainPanel:SetPoint("CENTER")
MainPanel:SetMovable(true)
MainPanel:EnableMouse(true)
MainPanel:RegisterForDrag("LeftButton")
MainPanel:SetScript("OnDragStart", MainPanel.StartMoving)
MainPanel:SetScript("OnDragStop", MainPanel.StopMovingOrSizing)

MainPanel:SetBackdrop({
    bgFile = "Interface\DialogFrame\UI-DialogBox-Background",
    edgeFile = "Interface\DialogFrame\UI-DialogBox-Border",
    tile = true, tileSize = 32, edgeSize = 32,
    insets = { left = 8, right = 8, top = 8, bottom = 8 }
})
MainPanel:SetBackdropBorderColor(0, 0, 0, 1)
MainPanel:Hide()

local Header = MainPanel:CreateFontString(nil, "OVERLAY", "GameFontNormalLarge")
Header:SetPoint("TOP", MainPanel, "TOP", 0, -15)
Header:SetText("Psychobot Management")

local ScrollFrame = CreateFrame("ScrollFrame", "PsychobotScrollFrame", MainPanel, "UIPanelScrollFrameTemplate")
ScrollFrame:SetPoint("TOPLEFT", MainPanel, "TOPLEFT", 20, -40)
ScrollFrame:SetPoint("BOTTOMRIGHT", MainPanel, "BOTTOMRIGHT", -110, 60)

local Content = CreateFrame("Frame", nil, ScrollFrame)
Content:SetSize(250, 1)
ScrollFrame:SetScrollChild(Content)

local botList = {}

local ActionPanel = CreateFrame("Frame", "PsychobotActionPanel", MainPanel, "BackdropTemplate")
ActionPanel:SetSize(90, 350)
ActionPanel:SetPoint("RIGHT", MainPanel, "RIGHT", -10, 0)
ActionPanel:SetBackdrop({
    bgFile = "Interface\DialogFrame\UI-DialogBox-Background",
    edgeFile = "Interface\DialogFrame\UI-DialogBox-Border",
    tile = true, tileSize = 32, edgeSize = 32,
    insets = { left = 5, right = 5, top = 5, bottom = 5 }
})

local function CreateActionButton(label, cmd, iconName, yOffset)
    local btn = CreateFrame("Button", nil, ActionPanel, "UIPanelButtonTemplate")
    btn:SetSize(80, 30)
    btn:SetPoint("TOP", ActionPanel, "TOP", 0, yOffset)
    btn:SetText(label)
    
    local icon = btn:CreateTexture(nil, "BACKGROUND")
    icon:SetSize(16, 16)
    icon:SetPoint("LEFT", btn, "LEFT", 4, 0)
    icon:SetTexture("Interface\Addons\PsychobotUI\Images\" .. iconName .. ".tga")
    
    btn:SetScript("OnClick", function()
        if selectedBot then
            DoEmote("/psychobot " .. cmd .. " " .. selectedBot)
        else
            print("|cFFFF0000PsychobotUI:|r Please select a bot first!")
        end
    end)
    return btn
end

local btnFollow = CreateActionButton("Follow", "follow", "follow_master", -20)
local btnStay = CreateActionButton("Stay", "stay", "stay", -55)
local btnAttack = CreateActionButton("Attack", "attack", "attack_dps", -90)
local btnSpec = CreateActionButton("Spec", "spec", "stats", -125)
local btnLogout = CreateActionButton("Logout", "logout", "logout", -160)

local function UpdateBotList()
    for _, frame in ipairs(botList) do
        frame:Hide()
    end
    table.clear(botList)

    local yOffset = 0
    for name, data in pairs(botTable) do
        local btn = CreateFrame("Button", nil, Content, "UIPanelButtonTemplate")
        btn:SetSize(230, 25)
        btn:SetPoint("TOPLEFT", Content, "TOPLEFT", 0, yOffset)
        btn:SetText(name .. (data.online and " [+]" or " [-]"))
        
        btn:SetScript("OnClick", function()
            selectedBot = name
            for _, b in ipairs(botList) do b:SetNormalFontObject("GameFontNormal") end
            btn:SetNormalFontObject("GameFontHighlight")
            print("|cFF00FF00PsychobotUI:|r Selected bot " .. name)
        end)
        
        table.insert(botList, btn)
        yOffset = yOffset - 30
    end
    Content:SetHeight(math.abs(yOffset))
end

local function OnSystemMessage(message)
    if message:find("Bot roster: ") then
        local rawList = message:gsub("Bot roster: ", "")
        local newBotTable = {}
        for botInfo in rawList:gmatch("([^,]+)") do
            local name = botInfo:match("^%s*(.-)%s*%(%s*([%+%-%s])%s*%)")
            if not name then name = botInfo:match("^%s*(.-)%s*$") end
            local status = botInfo:match("%((%s*[%+%-%s]%s*)%)")
            local online = (status and status:find("%+"))
            if name then newBotTable[name] = { online = online } end
        end
        botTable = newBotTable
        UpdateBotList()
    end
end

MainPanel:RegisterEvent("CHAT_MSG_SYSTEM")
MainPanel:SetScript("OnEvent", function(self, event, arg1)
    if event == "CHAT_MSG_SYSTEM" then
        OnSystemMessage(arg1)
    end
end)

SLASH_PSYCHOBOTUI1 = "/pb"
SlashCmdList.PSYCHOBOTUI = function(msg)
    if MainPanel:IsShown() then
        MainPanel:Hide()
    else
        DoEmote("/psychobot list")
        MainPanel:Show()
    end
end

print("|cFF00FF00Psychobot UI Loaded!|r Use |cFFFFFF00/pb|r to toggle.")
